#ifndef _mw_support_tb_h_
#define _mw_support_tb_h_
#include <systemc.h>
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/tlm_quantumkeeper.h"
#include "mat.h"
#include "model_jxz_tlm_tb_def.h"
#include <vector>

// ------------------------------------------------------------------------------
// Utils
// ------------------------------------------------------------------------------

// print-level bit definitions
#define TLMG_PRINT_START_OF_SIM        0x0001
#define TLMG_PRINT_END_OF_SIM          0x0002
#define TLMG_PRINT_VEC_INIT            0x0010
#define TLMG_PRINT_VEC_COMPARE_OK      0x0020
#define TLMG_PRINT_VEC_BYTE_BUF_ON_MISCOMPARE 0x0040
#define TLMG_PRINT_VEC_PROGRESS_DOT    0x0080
#define TLMG_PRINT_VEC_NUM             0x0100
#define TLMG_PRINT_TRANSPORT           0x1000
#define TLMG_PRINT_CSR_TRANSPORT       0x2000
#define TLMG_PRINT_SYNC_TO_FUNCS       0x4000

// error type bit definitions
#define TLMG_ERR_MISCOMPARE            0x0010
#define TLMG_ERR_MAT_INIT              0x0020
#define TLMG_ERR_TRANSPORT             0x0040
#define TLMG_ERR_MAT_WRITE             0x0080
#define TLMG_ERR_SYNC_TIMEOUT          0x0100
#define TLMG_ERR_OVFLW_UNFLW           0x0200

class mw_utils_tb {
 public:
  mw_utils_tb(int currVerbosity);
  ~mw_utils_tb();
  void markError(int errKind);
  void printMsg(int level, const char * fmt, ...);
  void printErr(int err, const char * fmt, ...);
  void turnOnMsgKind(int msgKindMask);
  void turnOffMsgKind(int msgKindMask);
  int getErrorStatus();
 private:
  int m_errorStatus;
  int m_currVerbosity;
};

// ------------------------------------------------------------------------------
// PortInfo
// ------------------------------------------------------------------------------
struct mw_portinfo_tb {
  // Simulink MAT file field info
  mxArray * pmxa;                      // pointer to Data field mxArray for MAT data
  char * pslf;                         // pointer to SL data buffer (mxGetData on Data mxArray)
  size_t sldsize;                      // SL data buffer size
  char * matname;                      // name of field in MAT file

  // Generated C-code field info
  unsigned char * pcf;                 // pointer to C struct data field (e.g. &rtU.inXX, &rtY.outXX)
  size_t cfsize;                       // C struct field size (in bytes)
  char * cfname;                       // C struct field name
  sc_dt::uint64 tlmaddr;               // target address offset for each field
  ~mw_portinfo_tb();
};

typedef std::vector<mw_portinfo_tb> mw_portinfovec_tb;

// ------------------------------------------------------------------------------
// MAT file interface
// ------------------------------------------------------------------------------
class mw_matfile_tb {
 public:
  enum SignalDirectionEnumT { INPUT_SIG, OUTPUT_SIG };

  mw_matfile_tb();
  mw_matfile_tb(mw_utils_tb & utils,
                const char * fn,
                int numIn,
                int numOut,
                const char * perm,
                const char * mvar,
                int numVec);
  ~mw_matfile_tb();

  // these next two are or could be made to be static...however, we may need
  // to change if signal name binding is required.
  void initializeInMatFieldInfo(mw_portinfovec_tb & inFieldInfo,
    mw_portinfovec_tb & expOutFieldInfo);
  void initializeOutMatFieldInfo(mw_portinfovec_tb & expOutFieldInfo,
    mw_portinfovec_tb & actOutFieldInfo);
 private:
  mw_utils_tb & m_utils;
 public:
  int writeSclog();
  const char * m_filename;
  int m_numInFields;
  int m_numOutFields;
 private:
  int openMat();
  int readSclog();
  const char * m_perm;
  const char * m_matVar;
  int m_numVectors;
  mxArray * m_sclog;
  MATFile * m_pmat;
};

// ------------------------------------------------------------------------------
// Vector/MAT
// ------------------------------------------------------------------------------
class mw_vecmat_tb {
 public:
  mw_vecmat_tb(mw_utils_tb & utils,
               mw_portinfovec_tb & inFieldInfo,
               mw_portinfovec_tb & actOutFieldInfo,
               mw_portinfovec_tb & expOutFieldInfo);
  ~mw_vecmat_tb();
  void initialize(mw_portinfovec_tb & inFieldInfo,
                  mw_portinfovec_tb & actOutFieldInfo,
                  mw_portinfovec_tb & expOutFieldInfo);
  bool getNextInput();
  bool getNextExpectedOutput();
  void putNextActualOutput();
  void compareResult();
  void terminate();
 private:
  mw_utils_tb & m_utils;
  mw_portinfovec_tb & m_inFieldInfo;
  mw_portinfovec_tb & m_actOutFieldInfo;
  mw_portinfovec_tb & m_expOutFieldInfo;
  int m_vecCountIn;
  int m_vecCountOut;
  int m_vecMiscompares;
  mw_matfile_tb m_matInfoIn;           // input stimulus, expected outputs
  mw_matfile_tb m_matInfoOut;          // actual outputs
};

// ------------------------------------------------------------------------------
// Synchronization
// ------------------------------------------------------------------------------
class mw_sync_tb_IF {
 public:
  virtual void incLocalTime(sc_time incTime) = 0;
  virtual void setLocalTime(sc_time locTime) = 0;
  virtual sc_time getLocalTime() = 0;
  virtual void syncToExplicitTime(sc_time time) = 0;
  virtual void syncToLocalTime() = 0;
  virtual void syncToDataReady(mw_irq_tb & irq) = 0;
};

// -----------------------------------------------
class mw_syncuntimed_tb : public virtual mw_sync_tb_IF {
 public:
  mw_syncuntimed_tb(mw_utils_tb & utils);
  ~mw_syncuntimed_tb();
  void incLocalTime(sc_time incTime);
  void setLocalTime(sc_time locTime);
  sc_time getLocalTime();
  void syncToExplicitTime(sc_time time);
  void syncToLocalTime();
  void syncToDataReady(mw_irq_tb & irq);
 private:
  mw_utils_tb m_utils;
};

// -----------------------------------------------
class mw_syncfunctimed_tb : public virtual mw_sync_tb_IF {
 public:
  mw_syncfunctimed_tb(mw_utils_tb & utils);
  ~mw_syncfunctimed_tb();
  void incLocalTime(sc_time incTime);
  void setLocalTime(sc_time locTime);
  sc_time getLocalTime();
  void syncToExplicitTime(sc_time time);
  void syncToLocalTime();
  void syncToDataReady(mw_irq_tb & irq);
 private:
  mw_utils_tb m_utils;
  sc_time m_delay;
};

// -----------------------------------------------
class mw_syncquantum_tb : public virtual mw_sync_tb_IF {
 public:
  mw_syncquantum_tb(mw_utils_tb & utils, int quantum);
  ~mw_syncquantum_tb();
  void incLocalTime(sc_time incTime);
  void setLocalTime(sc_time locTime);
  sc_time getLocalTime();
  void syncToExplicitTime(sc_time time);
  void syncToLocalTime();
  void syncToDataReady(mw_irq_tb & irq);
 private:
  mw_utils_tb m_utils;
  tlm_utils::tlm_quantumkeeper m_qk;
  sc_time m_delay;
  int m_quantum;
};

// -----------------------------------------------
class mw_sync_tb : public virtual mw_sync_tb_IF {
 public:
  enum TimingModeT { Untimed, FunctionallyTimed, LooselyTimed };

  mw_sync_tb(mw_utils_tb & utils, TimingModeT tmode, int quantum= 0);
  ~mw_sync_tb();
  TimingModeT setTimingMode(TimingModeT tmode);
  void incLocalTime(sc_time incTime);
  void setLocalTime(sc_time locTime);
  sc_time getLocalTime();
  void syncToExplicitTime(sc_time time);
  void syncToLocalTime();
  void syncToDataReady(mw_irq_tb & irq);
 private:
  mw_utils_tb m_utils;
  TimingModeT m_timingMode;
  mw_syncuntimed_tb m_syncUntimed;
  mw_syncfunctimed_tb m_syncFunctimed;
  mw_syncquantum_tb m_syncQuantum;
  mw_sync_tb_IF * m_activeSyncObj;
};

// ------------------------------------------------------------------------------
// Transaction
// ------------------------------------------------------------------------------
class mw_tr_tb {
 public:
  mw_tr_tb(mw_utils_tb & utils,
           mw_sync_tb & sync,
           mw_tlmsocket_tb & tlmsocket);
  ~mw_tr_tb();
  void sendTlmWriteTransaction(sc_dt::uint64 addr, unsigned char *dptr, size_t
    dsize);
  void sendTlmReadTransaction(sc_dt::uint64 addr, unsigned char *dptr, size_t
    dsize);
 private:
  mw_utils_tb & m_utils;
  mw_sync_tb & m_sync;
  mw_tlmsocket_tb & m_socket;
};

// ------------------------------------------------------------------------------
// Command/Status Register
class mw_csr_tb {
 public:
  mw_csr_tb(mw_utils_tb & utils, mw_sync_tb & sync, mw_tr_tb & tr,
            sc_dt::uint64 csrAddr, sc_mutex & mutex);
  ~mw_csr_tb();
  void readModifyWrite(mw_comstatreg_tb bitMask,
                       mw_comstatreg_tb bitValue);
  void writeCommand(mw_comstatreg_tb bitValue);
  mw_comstatreg_tb readStatus();
  bool pollForStatus(mw_comstatreg_tb bitMask,
                     mw_comstatreg_tb bitValue,
                     mw_comstatreg_tb & comstatreg,
                     int maxIters, sc_time iterWait);
 private:
  mw_utils_tb & m_utils;
  mw_sync_tb & m_sync;
  mw_tr_tb & m_tr;
  mw_comstatreg_tb m_csreg;
  sc_dt::uint64 m_csrAddr;
  sc_mutex & m_mutex;
};

#endif
