#include "mw_support_tb.h"
#include <string>
#include <stdarg.h>

using std::string;

#ifndef TLMG_MAX_NUM_MISCOMPARES
#define TLMG_MAX_NUM_MISCOMPARES       20
#endif

// ------------------------------------------------------------------------------
// Utils
// ------------------------------------------------------------------------------
mw_utils_tb::mw_utils_tb(int currVerbosity) :
  m_errorStatus(0), m_currVerbosity(currVerbosity)
{
}

mw_utils_tb::~mw_utils_tb()
{
}

// ------------------------------------------------------------------------
void mw_utils_tb::markError(int errKind)
{
  m_errorStatus |= errKind;
}

// ------------------------------------------------------------------------
void mw_utils_tb::printMsg(int level, const char * fmt, ...)
{
  if (level & m_currVerbosity) {
    // prepend thread name and time to message

    // thread name:
    sc_process_handle ph = sc_get_current_process_handle();
    string thrName(ph.name());
    size_t shortNameIdx = thrName.find_last_of('.');
    string shortName = thrName.substr(shortNameIdx+1);
    size_t isReader = shortName.find("readerThread");

    // sim time:
    const sc_time & simtime = sc_time_stamp();
    static char buf[512];              // FIXME: detect err on overrun
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    if (isReader != string::npos) {
      printf("[%10s] (%10s) \t\t%s", (simtime.to_string()).c_str(),
             shortName.c_str(), buf);
    } else {
      printf("[%10s] (%10s) %s", (simtime.to_string()).c_str(), shortName.c_str(),
             buf);
    }
  }
}

// ------------------------------------------------------------------------
void mw_utils_tb::printErr(int err, const char * fmt, ...)
{
  sc_process_handle ph = sc_get_current_process_handle();
  char buf[256];                       // FIXME: detect err on overrun
  va_list args;
  va_start(args, fmt);
  vsprintf(buf, fmt, args);
  va_end(args);
  printf("(%10s) %s", ph.name(), buf);
  markError(err);
}

void mw_utils_tb::turnOnMsgKind(int msgKindMask)
{
  m_currVerbosity |= msgKindMask;
}

void mw_utils_tb::turnOffMsgKind(int msgKindMask)
{
  m_currVerbosity &= ~msgKindMask;
}

// ------------------------------------------------------------------------
int mw_utils_tb::getErrorStatus(void)
{
  return m_errorStatus;
}

// ------------------------------------------------------------------------------
// Port Info
// ------------------------------------------------------------------------
mw_portinfo_tb::~mw_portinfo_tb()
{
}

// ------------------------------------------------------------------------------
// MAT File
// ------------------------------------------------------------------------------
//mw_matfile_tb::mw_matfile_tb() :
//  m_filename(""), m_perm(""), m_matVar(""), m_numInFields(0), m_numOutFields(0),
//  m_numVectors(0), m_sclog(NULL), m_pmat(NULL)
//{
//}

// ------------------------------------------------------------------------
mw_matfile_tb::mw_matfile_tb(
  mw_utils_tb & utils, const char * fn, int numIn, int numOut,
  const char * perm, const char * mvar, int numVec) :
  m_utils(utils),
  m_filename(fn),
  m_numInFields(numIn),
  m_numOutFields(numOut),
  m_perm(perm),
  m_matVar(mvar),
  m_numVectors(numVec),
  m_sclog(NULL), m_pmat(NULL)
{
}

// ------------------------------------------------------------------------
mw_matfile_tb::~mw_matfile_tb()
{
  mxDestroyArray(m_sclog);
  if (matClose(m_pmat) != 0) {
    printf("## ERROR: closing file %s\n", m_filename);
  }
}

// ------------------------------------------------------------------------
// expected structure of the MAT file info. . .
//      tlmg_inXXX
//          Data (buffer of elem data type)
//          SampleInfo
//              elemDataType
//              (etc)
//          DataTypeInfo
//              numerictype
//              isComplex
//      tlmg_outXXX
//          (etc)
//
void mw_matfile_tb::initializeInMatFieldInfo(
  mw_portinfovec_tb & inFieldInfo,
  mw_portinfovec_tb & expOutFieldInfo)
{
  if (openMat()) {
    m_utils.markError(TLMG_ERR_MAT_INIT);
    return;
  }

  if (readSclog()) {
    m_utils.markError(TLMG_ERR_MAT_INIT);
    return;
  }

  int numfields = mxGetNumberOfFields(m_sclog);
  int in_dcount = 0;
  int out_dcount = 0;
  SignalDirectionEnumT sigdir;
  for (int i=0; i<numfields; i++) {
    // ----------------------------------------
    // get fieldname and direction
    // ----------------------------------------
    const char * fname = mxGetFieldNameByNumber(m_sclog, i);
    if (!strncmp("tlmg_in", fname, 7)) {
      sigdir = INPUT_SIG;
      m_utils.printMsg(TLMG_PRINT_VEC_INIT,
                       "## found input field %s at %s fieldnum %d\n", fname,
                       m_matVar, i);
    } else if (!strncmp("tlmg_out", fname, 8)) {
      sigdir = OUTPUT_SIG;
      m_utils.printMsg(TLMG_PRINT_VEC_INIT,
                       "## found output field %s at %s fieldnum %d\n", fname,
                       m_matVar, i);
    } else {
      m_utils.printMsg(TLMG_PRINT_VEC_INIT,
                       "## skipping unknown field %s in var %s\n", fname,
                       m_matVar);
      continue;
    }

    // -------------------------------------------------------
    // get hold of Data field and its underlying data buffer
    // -------------------------------------------------------
    mxArray * pmxa_field;
    if ((pmxa_field = mxGetFieldByNumber(m_sclog, 0, i)) == NULL) {
      m_utils.printErr(TLMG_ERR_MAT_INIT,
                       "## ERROR: %s.tlmg_XX var could not be read.\n",
                       m_matVar);
      return;                          //return(1);
    }

    int fnum;
    if ((fnum = mxGetFieldNumber(pmxa_field, "Data")) == -1) {
      m_utils.printErr(TLMG_ERR_MAT_INIT,
                       "## ERROR: %s.tlmg_XX var did not have Data field.\n",
                       m_matVar);
      return;                          //return(1);
    }

    mxArray * pmxa_dfield;
    if ((pmxa_dfield = mxGetFieldByNumber(pmxa_field, 0, fnum)) == NULL) {
      m_utils.printErr(TLMG_ERR_MAT_INIT,
                       "## ERROR: %s.tlmg_XX.Data var could not be read.\n",
                       m_matVar);
      return;                          //return(1);
    }

    if (sigdir == INPUT_SIG) {
      inFieldInfo[in_dcount].pmxa = pmxa_dfield;
      inFieldInfo[in_dcount].pslf = (char *)mxGetData(pmxa_dfield);
      inFieldInfo[in_dcount].sldsize =
        mxGetNumberOfElements(pmxa_dfield) * mxGetElementSize(pmxa_dfield);
      in_dcount++;
    } else {
      expOutFieldInfo[out_dcount].pmxa = pmxa_dfield;
      expOutFieldInfo[out_dcount].pslf = (char *)mxGetData(pmxa_dfield);
      expOutFieldInfo[out_dcount].sldsize =
        mxGetNumberOfElements(pmxa_dfield) * mxGetElementSize(pmxa_dfield);
      out_dcount++;
    }
  }

  if (in_dcount != m_numInFields) {
    m_utils.printErr(TLMG_ERR_MAT_INIT,
                     "## found %d input fields in var %s, expected %d\n",
                     in_dcount, m_matVar, NUM_IN_PORTS);
  }

  if (out_dcount != m_numOutFields) {
    m_utils.printErr(TLMG_ERR_MAT_INIT,
                     "## found %d output fields in var %s, expected %d\n",
                     out_dcount, m_matVar, NUM_OUT_PORTS);
  }

  m_utils.printMsg(TLMG_PRINT_VEC_INIT,
                   "## setup %d input data fields, %d output data fields\n",
                   in_dcount,
                   out_dcount);
}

// ------------------------------------------------------------------------
// expected structure of the MAT file info. . .
//      tlmg_outXXX
//          Data (buffer of elem data type)
//
void mw_matfile_tb::initializeOutMatFieldInfo(
  mw_portinfovec_tb & expOutFieldInfo,
  mw_portinfovec_tb & actOutFieldInfo)
{
  if (openMat()) {
    m_utils.markError(TLMG_ERR_MAT_INIT);
    return;
  }

  const char * topFields[] = {
    "tlmg_outX"
  };

  mwSize dims[2] = { 1, 1 };

  m_sclog = mxCreateStructArray(2, dims, m_numOutFields, topFields);
  if (m_sclog == NULL) {
    m_utils.printErr(TLMG_ERR_MAT_INIT,
                     "## ERROR: Could not create top-level mx struct for MAT file %s\n",
                     m_filename);
    exit(-1);                          //return(1);
  }

  const char * outStructFieldNames[] = { "Data" };// may add DataTypeInfo later.

  mxArray * paOutStruct;
  for (int i=0; i<m_numOutFields; i++) {
    // create Data array -- should match expected data out. . .
    dims[1] = expOutFieldInfo[i].sldsize;
    actOutFieldInfo[i].sldsize = dims[1];
    actOutFieldInfo[i].pmxa = mxCreateNumericArray(2, dims, mxUINT8_CLASS,
      mxREAL);
    if (actOutFieldInfo[i].pmxa == NULL) {
      m_utils.printErr(TLMG_ERR_MAT_INIT,
                       "## ERROR: Could not create mx numeric array for MAT file %s\n",
                       m_filename);
      exit(-1);                        //return(1);
    }

    actOutFieldInfo[i].pslf = (char *)mxGetData(actOutFieldInfo[i].pmxa);

    // create and attach child data out struct
    dims[1] = 1;
    paOutStruct = mxCreateStructArray(2, dims, 1, outStructFieldNames);
    if (paOutStruct == NULL) {
      m_utils.printErr(TLMG_ERR_MAT_INIT,
                       "## ERROR: Could not create mx data out struct for MAT file %s\n",
                       m_filename);
      exit(-1);                        //return(1);
    }

    mxSetFieldByNumber(paOutStruct, 0, 0, actOutFieldInfo[i].pmxa);
    mxSetFieldByNumber(m_sclog, 0, i, paOutStruct);
  }
}

// ------------------------------------------------------------------------
int mw_matfile_tb::openMat()
{
  m_pmat = matOpen(m_filename, m_perm);
  if (m_pmat == NULL) {
    printf("## ERROR: opening file %s with permissions %s\n", m_filename, m_perm);
    return -1;
  }

  return 0;
}

// ------------------------------------------------------------------------
int mw_matfile_tb::readSclog()
{
  m_sclog = matGetVariable(m_pmat, m_matVar);
  if (m_sclog == NULL) {
    printf("## ERROR: could not get %s variable in file %s\n",
           m_matVar, m_filename);
    return -1;
  }

  if (!mxIsStruct(m_sclog)) {
    printf("## ERROR: var %s was not a struct.\n", m_matVar);
    return -1;
  }

  return 0;
}

// ------------------------------------------------------------------------
int mw_matfile_tb::writeSclog()
{
  if (matPutVariable(m_pmat, m_matVar, m_sclog)) {
    printf("## ERROR: Could not write MAT file %s.\n",
           m_filename);
    return -1;
  }

  return 0;
}

// ------------------------------------------------------------------------------
// Vector/MAT
// ------------------------------------------------------------------------------
mw_vecmat_tb::mw_vecmat_tb(
  mw_utils_tb & utils,
  mw_portinfovec_tb & inFieldInfo,
  mw_portinfovec_tb & actOutFieldInfo,
  mw_portinfovec_tb & expOutFieldInfo) :
  m_utils(utils),
  m_inFieldInfo(inFieldInfo),
  m_actOutFieldInfo(actOutFieldInfo),
  m_expOutFieldInfo(expOutFieldInfo),
  m_vecCountIn(0),
  m_vecCountOut(0),
  m_vecMiscompares(0),
  m_matInfoIn(m_utils, INPUT_VECTOR_FILE, NUM_IN_PORTS, NUM_OUT_PORTS,
              "r", "tlmg_tlminvec", NUM_VECTORS),
  m_matInfoOut(m_utils, OUTPUT_VECTOR_FILE, 0, NUM_OUT_PORTS,
               "w", "tlmg_tlmoutvec", NUM_VECTORS)
{
}

// ------------------------------------------------------------------------------
mw_vecmat_tb::~mw_vecmat_tb()
{
}

// ------------------------------------------------------------------------
void mw_vecmat_tb::initialize(
  mw_portinfovec_tb & inFieldInfo,
  mw_portinfovec_tb & actOutFieldInfo,
  mw_portinfovec_tb & expOutFieldInfo)
{
  m_matInfoIn.initializeInMatFieldInfo(inFieldInfo, expOutFieldInfo);
  m_matInfoOut.initializeOutMatFieldInfo(expOutFieldInfo, actOutFieldInfo);
}

// ------------------------------------------------------------------------
bool mw_vecmat_tb::getNextInput()
{
  bool endOfVec = false;

  // get stimulus data from MAT and put into rU.
  for (int i=0; i<m_matInfoIn.m_numInFields; i++) {
    int offset = m_vecCountIn*m_inFieldInfo[i].cfsize;
    if (offset+m_inFieldInfo[i].cfsize-1 > m_inFieldInfo[i].sldsize) {
      m_utils.printMsg(TLMG_PRINT_VEC_NUM,
                       "## end of input vectors in MAT file %s for input number %d\n",
                       m_matInfoIn.m_filename, i);
      endOfVec = true;
    } else {
      memcpy(m_inFieldInfo[i].pcf, m_inFieldInfo[i].pslf+offset, m_inFieldInfo[i]
             .
             cfsize);
    }
  }

  if (!endOfVec) {
    m_utils.printMsg(TLMG_PRINT_VEC_NUM,
                     "## vec %5d:\n##\t initialized input dataset\n",
                     m_vecCountIn);
    m_vecCountIn++;
    if ((!((m_vecCountIn-1) % 100)) && (m_vecCountIn != 1)) {
      m_utils.printMsg(TLMG_PRINT_VEC_PROGRESS_DOT, ".\n");
    }
  }

  return endOfVec;
}

// ------------------------------------------------------------------------
bool mw_vecmat_tb::getNextExpectedOutput()
{
  bool endOfVec = false;
  for (int i=0; i<m_matInfoIn.m_numOutFields; i++) {
    int offset = m_vecCountOut*m_expOutFieldInfo[i].cfsize;
    if (offset+m_expOutFieldInfo[i].cfsize == m_expOutFieldInfo[i].sldsize) {
      m_utils.printMsg(TLMG_PRINT_VEC_NUM,
                       "## end of output vectors in MAT file %s for output number %d\n",
                       m_matInfoIn.m_filename, i);
      endOfVec = true;
    } else if (offset+m_expOutFieldInfo[i].cfsize > m_expOutFieldInfo[i].sldsize)
    {
      // error
      endOfVec = true;
      return endOfVec;
    }

    memcpy(m_expOutFieldInfo[i].pcf, m_expOutFieldInfo[i].pslf+offset,
           m_expOutFieldInfo[i].cfsize);
  }

  m_utils.printMsg(TLMG_PRINT_VEC_NUM,
                   "##\t initialized expected output dataset\n",
                   m_vecCountOut);
  return endOfVec;
}

// ------------------------------------------------------------------------
void mw_vecmat_tb::putNextActualOutput()
{
  int offset;
  for (int i=0; i<m_matInfoOut.m_numOutFields; i++) {
    offset = m_vecCountOut*m_actOutFieldInfo[i].cfsize;
    if (offset+m_actOutFieldInfo[i].cfsize-1 > m_actOutFieldInfo[i].sldsize) {
      m_utils.printErr(TLMG_ERR_MAT_WRITE,
                       "## ERROR: cannot write actual output data at vec %d, field %d because mxArray is not large enough.\n",
                       m_vecCountOut, i);
      return;
    }

    memcpy(m_actOutFieldInfo[i].pslf+offset, m_actOutFieldInfo[i].pcf,
           m_actOutFieldInfo[i].cfsize);
  }

  m_utils.printMsg(TLMG_PRINT_VEC_NUM,
                   "##\t wrote actual output dataset to MAT file buffer.\n",
                   m_vecCountOut);
}

// ------------------------------------------------------------------------
void mw_vecmat_tb::compareResult()
{
  bool hadMiscompare = false;
  for (int i=0; i<m_matInfoIn.m_numOutFields; i++) {
    if (memcmp(m_expOutFieldInfo[i].pcf, m_actOutFieldInfo[i].pcf,
               m_expOutFieldInfo[i].cfsize)) {
      m_utils.printErr(TLMG_ERR_MISCOMPARE,
                       "## ERROR: vec %d miscompare at output %d\n",
                       m_vecCountOut, i);
      hadMiscompare = true;

      //printf("## vector %d, field %d\n", m_vecCountOut, i);
      for (unsigned int j=0; j<m_expOutFieldInfo[i].cfsize; j++) {
        m_utils.printMsg(TLMG_PRINT_VEC_BYTE_BUF_ON_MISCOMPARE,
                         "\tbyte %3d: exp=%2x act=%2x\n", j,
                         (*(m_expOutFieldInfo[i].pcf+j)&0xff),
                         (*(m_actOutFieldInfo[i].pcf+j)&0xff));
      }
    }
  }

  if (hadMiscompare) {
    if (m_vecMiscompares++ == TLMG_MAX_NUM_MISCOMPARES) {
      m_utils.turnOffMsgKind(TLMG_PRINT_VEC_BYTE_BUF_ON_MISCOMPARE);
    }

    m_utils.printMsg(TLMG_PRINT_VEC_COMPARE_OK,
                     "##\t actual output dataset did not match expected output dataset\n");
  } else {
    m_utils.printMsg(TLMG_PRINT_VEC_COMPARE_OK,
                     "##\t actual output dataset matched expected output dataset\n");
  }

  m_vecCountOut++;
}

// ------------------------------------------------------------------------
void mw_vecmat_tb::terminate()
{
  m_utils.printMsg(TLMG_PRINT_END_OF_SIM,
                   "\n"
                   "#############################################\n"
                   "## END OF VECTORS. PLAYED %5d VECTORS.   ##\n"
                   "## DATA MISCOMPARES     : %5d            ##\n"
                   "## TRANSPORT ERRORS     : %5s            ##\n"
                   "## MAT FILE WRITE ERRORS: %5s            ##\n"
                   "#############################################\n",
                   m_vecCountIn, m_vecMiscompares,
                   (m_utils.getErrorStatus() & TLMG_ERR_TRANSPORT) ? "YES" :
                   "NO",
                   (m_utils.getErrorStatus() & TLMG_ERR_MAT_WRITE) ? "YES" :
                   "NO");
  if (m_matInfoOut.writeSclog()) {
    m_utils.markError(TLMG_ERR_MAT_WRITE);
    return;
  } else {
    m_utils.printMsg(TLMG_PRINT_END_OF_SIM, "##\t Wrote results MAT file.\n");
  }

  // destroy mx allocated memory done in destructors of instantiated
  // structures.
  // CHECKME: need to ensure FieldInfo destroyed before MatInfo.
}

// ------------------------------------------------------------------------------
// Synchronization
// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------
mw_sync_tb::mw_sync_tb(mw_utils_tb & utils, TimingModeT tmode, int quantum) :
  m_utils(utils),
  m_timingMode(tmode),
  m_syncUntimed(utils),
  m_syncFunctimed(utils),
  m_syncQuantum(utils, quantum)
{
  setTimingMode(tmode);
}

// ------------------------------------------------------------------------
mw_sync_tb::~mw_sync_tb()
{
}

// ------------------------------------------------------------------------
mw_sync_tb::TimingModeT mw_sync_tb::setTimingMode(TimingModeT tmode)
{
  TimingModeT tmp = m_timingMode;
  m_timingMode = tmode;
  switch (tmode) {
   case Untimed:
    m_activeSyncObj = &m_syncUntimed;
    break;

   case FunctionallyTimed:
    m_activeSyncObj = &m_syncFunctimed;
    break;

   case LooselyTimed:
    m_activeSyncObj = &m_syncQuantum;
    break;
  }

  return tmp;
}

// ------------------------------------------------------------------------
void mw_sync_tb::incLocalTime(sc_time t)
{
  m_activeSyncObj->incLocalTime(t);
}

void mw_sync_tb::setLocalTime(sc_time t)
{
  m_activeSyncObj->setLocalTime(t);
}

sc_time mw_sync_tb::getLocalTime()
{
  return m_activeSyncObj->getLocalTime();
}

void mw_sync_tb::syncToExplicitTime(sc_time t)
{
  m_activeSyncObj->syncToExplicitTime(t);
}

void mw_sync_tb::syncToLocalTime()
{
  m_activeSyncObj->syncToLocalTime();
}

void mw_sync_tb::syncToDataReady(sc_core::sc_in<bool> & irq)
{
  m_activeSyncObj->syncToDataReady(irq);
}

// ------------------------------------------------------------------------
mw_syncuntimed_tb::mw_syncuntimed_tb(mw_utils_tb & utils) :
  m_utils(utils)
{
}

mw_syncuntimed_tb::~mw_syncuntimed_tb()
{
}

void mw_syncuntimed_tb::incLocalTime(sc_time t)
{
  syncToLocalTime();
}

void mw_syncuntimed_tb::setLocalTime(sc_time t)
{
  // do nothing
}

sc_time mw_syncuntimed_tb::getLocalTime()
{
  return SC_ZERO_TIME;
}

void mw_syncuntimed_tb::syncToExplicitTime(sc_time t)
{
  m_utils.printMsg(TLMG_PRINT_SYNC_TO_FUNCS,
                   "##\t waiting for explicit amount of time (time = 0 ns + 0 ns).\n");
  wait(SC_ZERO_TIME);
}

void mw_syncuntimed_tb::syncToLocalTime()
{
  m_utils.printMsg(TLMG_PRINT_SYNC_TO_FUNCS,
                   "##\t syncing to local time offset (delay = 0 ns).\n");
  wait(SC_ZERO_TIME);
}

void mw_syncuntimed_tb::syncToDataReady(sc_core::sc_in<bool> & irq)
{
  m_utils.printMsg(TLMG_PRINT_SYNC_TO_FUNCS,
                   "##\t syncing to interrupt signal for data ready in output buffer...\n");
  if (irq.read() == true) {
    m_utils.printMsg(TLMG_PRINT_SYNC_TO_FUNCS,
                     "##\t...saw interrupt already asserted\n");
  } else {
    wait(irq.posedge_event());
    m_utils.printMsg(TLMG_PRINT_SYNC_TO_FUNCS, "##\t...saw interrupt\n");
  }
}

// ------------------------------------------------------------------------
mw_syncfunctimed_tb::mw_syncfunctimed_tb(mw_utils_tb & utils) :
  m_utils(utils), m_delay(SC_ZERO_TIME)
{
}

mw_syncfunctimed_tb::~mw_syncfunctimed_tb()
{
}

void mw_syncfunctimed_tb::incLocalTime(sc_time t)
{
  m_delay += t;
  syncToLocalTime();
}

void mw_syncfunctimed_tb::setLocalTime(sc_time t)
{
  m_delay = t;
}

sc_time mw_syncfunctimed_tb::getLocalTime()
{
  return m_delay;
}

void mw_syncfunctimed_tb::syncToExplicitTime(sc_time t)
{
  m_utils.printMsg(TLMG_PRINT_SYNC_TO_FUNCS,
                   "##\t waiting for explicit amount of time (time = %s + %s).\n",
                   m_delay.to_string().c_str(), t.to_string().
                   c_str() );
  wait(m_delay);
  wait(t);
  m_delay = SC_ZERO_TIME;
}

void mw_syncfunctimed_tb::syncToLocalTime()
{
  m_utils.printMsg(TLMG_PRINT_SYNC_TO_FUNCS,
                   "##\t syncing to local time offset (delay = %s).\n",
                   (m_delay.to_string()).c_str() );
  wait(m_delay);
  m_delay = SC_ZERO_TIME;
}

void mw_syncfunctimed_tb::syncToDataReady(sc_core::sc_in<bool> & irq)
{
  m_utils.printMsg(TLMG_PRINT_SYNC_TO_FUNCS,
                   "##\t syncing to interrupt signal for data ready in output buffer...\n");
  if (irq.read() == true) {
    m_utils.printMsg(TLMG_PRINT_SYNC_TO_FUNCS,
                     "##\t...saw interrupt already asserted\n");
  } else {
    wait(irq.posedge_event());
    m_utils.printMsg(TLMG_PRINT_SYNC_TO_FUNCS, "##\t...saw interrupt\n");
  }
}

// ------------------------------------------------------------------------
mw_syncquantum_tb::mw_syncquantum_tb(mw_utils_tb & utils, int quantum) :
  m_utils(utils),
  m_delay(SC_ZERO_TIME),
  m_quantum(quantum)
{
  m_qk.set_global_quantum(sc_time(m_quantum, SC_NS));
  m_qk.reset();
}

mw_syncquantum_tb::~mw_syncquantum_tb()
{
}

void mw_syncquantum_tb::incLocalTime(sc_time incTime)
{
  m_qk.inc(incTime);
  if (m_qk.need_sync()) {
    m_utils.printMsg(TLMG_PRINT_SYNC_TO_FUNCS,
                     "##\t Quantum reached: syncing to local time offset (delay = %s).\n",
                     (m_qk.get_local_time().to_string()).c_str() );
    m_qk.sync();
  }
}

void mw_syncquantum_tb::setLocalTime(sc_time locTime)
{
  m_qk.set(locTime);
}

sc_time mw_syncquantum_tb::getLocalTime()
{
  return m_qk.get_local_time();
}

void mw_syncquantum_tb::syncToExplicitTime(sc_time time)
{
  m_utils.printMsg(TLMG_PRINT_SYNC_TO_FUNCS,
                   "##\t waiting for explicit amount of time (time = %s + %s).\n",
                   m_qk.get_local_time().to_string().c_str(), time.to_string().
                   c_str() );
  m_qk.sync();
  wait(time);
}

void mw_syncquantum_tb::syncToLocalTime()
{
  m_utils.printMsg(TLMG_PRINT_SYNC_TO_FUNCS,
                   "##\t syncing to local time offset (delay = %s).\n",
                   (m_qk.get_local_time().to_string()).c_str() );
  m_qk.sync();
}

void mw_syncquantum_tb::syncToDataReady(sc_core::sc_in<bool> & irq)
{
  m_utils.printMsg(TLMG_PRINT_SYNC_TO_FUNCS,
                   "##\t syncing to interrupt signal for data ready in output buffer...\n");
  if (irq.read() == true) {
    m_utils.printMsg(TLMG_PRINT_SYNC_TO_FUNCS,
                     "##\t...saw interrupt already asserted\n");
    return;
  }

  m_qk.sync();
  if (irq.read() == true) {
    m_utils.printMsg(TLMG_PRINT_SYNC_TO_FUNCS,
                     "##\t...saw interrupt already asserted\n");
    return;
  }

  wait(irq.posedge_event());
  m_utils.printMsg(TLMG_PRINT_SYNC_TO_FUNCS, "##\t...saw interrupt\n");
}

// ------------------------------------------------------------------------------
// Transaction
// ------------------------------------------------------------------------------
mw_tr_tb::mw_tr_tb(mw_utils_tb & utils,
                   mw_sync_tb & sync,
                   mw_tlmsocket_tb & socket) :
  m_utils(utils),
  m_sync(sync),
  m_socket(socket)
{
}

// ------------------------------------------------------------------------
mw_tr_tb::~mw_tr_tb()
{
}

// ------------------------------------------------------------------------
void mw_tr_tb::sendTlmWriteTransaction(sc_dt::uint64 addr, unsigned char *dptr,
  size_t dsize)
{
  sc_time delay = m_sync.getLocalTime();
  tlm::tlm_generic_payload gp;
  gp.set_address(addr);
  gp.set_command(tlm::TLM_WRITE_COMMAND);
  gp.set_data_ptr(dptr);
  gp.set_data_length(dsize);
  m_socket->b_transport(gp, delay);
  tlm::tlm_response_status gp_status = gp.get_response_status();
  if (gp_status != tlm::TLM_OK_RESPONSE) {
    m_utils.printErr(TLMG_ERR_TRANSPORT, "## ERROR: TLM write error.\n");
  } else {
    m_utils.printMsg(TLMG_PRINT_TRANSPORT,
                     "##\t TLM write transaction to target component at address %#08x\n",
                     (unsigned int) addr);
    if (addr == (unsigned int) COMSTAT_REG_ADDR) {
      m_utils.printMsg(TLMG_PRINT_CSR_TRANSPORT,
                       "##\t (Command/Status reg data value = %#010x)\n",
                       *((unsigned int *) dptr));
    }
  }

  m_sync.setLocalTime(delay);
  m_sync.incLocalTime(SC_ZERO_TIME);
}

// ------------------------------------------------------------------------
void mw_tr_tb::sendTlmReadTransaction(sc_dt::uint64 addr, unsigned char *dptr,
  size_t dsize)
{
  sc_time delay = m_sync.getLocalTime();
  tlm::tlm_generic_payload gp;
  gp.set_address(addr);                // memory address
  gp.set_command(tlm::TLM_READ_COMMAND);// memory command
  gp.set_data_ptr(dptr);
  gp.set_data_length(dsize);
  m_socket->b_transport(gp, delay);
  tlm::tlm_response_status gp_status = gp.get_response_status();
  if (gp_status != tlm::TLM_OK_RESPONSE) {
    m_utils.printErr(TLMG_ERR_TRANSPORT, "## ERROR: TLM read error.\n");
  } else {
    m_utils.printMsg(TLMG_PRINT_TRANSPORT,
                     "##\t TLM read  transaction to target component at address %#08x\n",
                     (unsigned int)addr);
    if (addr == (unsigned int) COMSTAT_REG_ADDR) {
      m_utils.printMsg(TLMG_PRINT_CSR_TRANSPORT,
                       "##\t (Command/Status reg data value = %#010x)\n",
                       *((unsigned int *) dptr));
    }
  }

  m_sync.setLocalTime(delay);
  m_sync.incLocalTime(SC_ZERO_TIME);
}

// ------------------------------------------------------------------------------
// Command/Status Register
// ------------------------------------------------------------------------------
mw_csr_tb::mw_csr_tb(mw_utils_tb & utils, mw_sync_tb & sync, mw_tr_tb & tr,
                     sc_dt::uint64 csrAddr, sc_mutex & mutex) :
  m_utils(utils),
  m_sync(sync),
  m_tr(tr),
  m_csrAddr(csrAddr),
  m_mutex(mutex)
{
}

// ------------------------------------------------------------------------
mw_csr_tb::~mw_csr_tb()
{
}

// ------------------------------------------------------------------------
void mw_csr_tb::readModifyWrite(mw_comstatreg_tb bitMask, mw_comstatreg_tb
  bitValue)
{
  m_mutex.lock();

  // read comstat reg
  m_tr.sendTlmReadTransaction(m_csrAddr, (unsigned char *) & m_csreg, sizeof
    (m_csreg));

  // modify
  m_csreg &= ~bitMask;                 // zero out bits of interest
  m_csreg |= bitValue;                 // set bits of interest

  // write
  m_tr.sendTlmWriteTransaction(m_csrAddr, (unsigned char *) & m_csreg, sizeof
    (m_csreg));
  m_mutex.unlock();
}

// ------------------------------------------------------------------------
void mw_csr_tb::writeCommand (mw_comstatreg_tb bitValue)
{
  m_csreg = bitValue;
  m_tr.sendTlmWriteTransaction(m_csrAddr, (unsigned char *)& m_csreg, sizeof
    (m_csreg));
}

// ------------------------------------------------------------------------
mw_comstatreg_tb mw_csr_tb::readStatus()
{
  m_tr.sendTlmReadTransaction(m_csrAddr, (unsigned char *)&m_csreg, sizeof
    (m_csreg));
  return m_csreg;
}

// ------------------------------------------------------------------------
// e.g., looking for inbuf not empty or outbuf full
// pollForStatus((INBUF_EMPTY_BIT_MASK | OUTBUF_FULL_BIT_MASK),
//                OUTBUF_FULL_BIT_MASK, x,x,x)
//
// returns true on timeout, false if saw expected status.
//
bool mw_csr_tb::pollForStatus(mw_comstatreg_tb bitMask,
  mw_comstatreg_tb bitValue,
  mw_comstatreg_tb & comstatreg,
  int maxIters, sc_time iterWait)
{
  m_sync.syncToLocalTime();            // ensure read current status
  m_csreg = readStatus();
  mw_comstatreg_tb pollResult = (m_csreg & bitMask);
  while ((pollResult != bitValue) && (maxIters > 0) ) {
    m_sync.syncToLocalTime();
    m_sync.syncToExplicitTime(iterWait);
    m_csreg = readStatus();
    pollResult = m_csreg & bitMask;
    maxIters--;
  }

  m_sync.syncToLocalTime();
  comstatreg = m_csreg;
  if (maxIters <= 0) {
    m_utils.printErr(TLMG_ERR_SYNC_TIMEOUT,
                     "## ERROR: Timed out waiting for bitValue=%#010x. (bitMask=%#010x, masked csr=%#010x)\n",
                     bitValue, bitMask, pollResult);
    return true;
  } else {
    m_utils.printMsg(TLMG_PRINT_SYNC_TO_FUNCS,
                     "##\t...saw polling return expected value. (bitValue=%#010x, bitMask=%#010x, masked csr=%#010x)\n",
                     bitValue, bitMask, pollResult);
    return false;
  }
}
