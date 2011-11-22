#ifndef _model_jxz_tlm_tb_
#define _model_jxz_tlm_tb_
#include <systemc.h>
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "mw_support_tb.h"
#include "mw_backdoorcfg_if.h"
#include "model_jxz_tlm_tb_def.h"

// ------------------------------------------------------------------------------
class model_jxz_tlm_tb : public sc_module {
 public:
  mw_tlmsocket_tb m_tlm2_isocket;      // TLM socket
  mw_irq_tb m_int_ip;                  // interrupt port; active high
  sc_port<mw_backdoorcfg_IF> m_backdoorcfg;// timed/untimed modes
  tlm::tlm_sync_enum nb_transport_bw(  // dummy implementation of backwards path
    tlm::tlm_generic_payload & trans,
    tlm::tlm_phase & phase,
    sc_time & delay )
  {
    return tlm::TLM_COMPLETED;
  }

  // constructors and destructors
  SC_HAS_PROCESS(model_jxz_tlm_tb);
  model_jxz_tlm_tb(const sc_module_name & n);
  ~model_jxz_tlm_tb(void);
  int getErrorStatus();                // called from main
 private:
  // ---------- METHODS ----------
  void singleInitiatorThread();        // initiator
  void initialize();

  // ---------- INPUT/OUTPUT MEMBERS ----------
  // declare data members which hold input data set, output data set, and
  // expected output data set (from MAT file)
  //

  // ---------- HELPER OBJS ----------
  mw_utils_tb m_utils;
  mw_portinfovec_tb m_inFieldInfo;
  mw_portinfovec_tb m_actOutFieldInfo;
  mw_portinfovec_tb m_expOutFieldInfo;
  mw_vecmat_tb m_vecmat;
  int m_returnStatus;
  int m_quantumTime;
  int m_bufWriteCount;
  sc_mutex m_mutex;
};

#endif                                 // _model_jxz_tlm_tb_
