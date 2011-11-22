#include "model_jxz_tlm_tb.h"

static const int tlmgPrintSilent = 0x0000;
static const int tlmgPrintTerse = (
  TLMG_PRINT_START_OF_SIM
  | TLMG_PRINT_VEC_INIT
  | TLMG_PRINT_VEC_PROGRESS_DOT
  | TLMG_PRINT_VEC_BYTE_BUF_ON_MISCOMPARE
  | TLMG_PRINT_END_OF_SIM
  );
static const int tlmgPrintAll = 0xffff;

#ifndef TLMG_CURR_VERBOSITY
#define TLMG_CURR_VERBOSITY            tlmgPrintAll
#endif

// ------------------------------------------------------------------------------
// model_jxz_tlm_tb
// ------------------------------------------------------------------------------
model_jxz_tlm_tb::model_jxz_tlm_tb(const
  sc_module_name & n) :
  sc_module(n),
  m_int_ip("m_int_ip"),
  m_utils(TLMG_CURR_VERBOSITY),
  m_inFieldInfo(NUM_IN_PORTS),
  m_actOutFieldInfo(NUM_OUT_PORTS),
  m_expOutFieldInfo(NUM_OUT_PORTS),
  m_vecmat(m_utils, m_inFieldInfo, m_actOutFieldInfo, m_expOutFieldInfo),
  m_mutex("m_mutex")
{
  m_tlm2_isocket.register_nb_transport_bw( this,
    &model_jxz_tlm_tb::nb_transport_bw );
  SC_THREAD(singleInitiatorThread);
  initialize();
  if (getErrorStatus())
    exit(getErrorStatus());            // cannot continue
}

// ------------------------------------------------------------------------
model_jxz_tlm_tb::~model_jxz_tlm_tb(void)
{
}

// ------------------------------------------------------------------------
int model_jxz_tlm_tb::getErrorStatus()
{
  return m_utils.getErrorStatus();
}

// ------------------------------------------------------------------------
// SINGLE INITIATOR THREAD
//
// This thread is a simple writer and reader thread.  It is used when the 'TLM
// Testbench' and 'TLM Generation' options indicate a simple target component.
// It will write a single input data set, wait for the algorithm processing to
// be complete, then read the output data set for comparison with the Simulink
// data.  It does not use buffering or CSR buffer status.
//
// If you choose more complex 'TLM Generation' options, such as buffering and
// inclusion of a command/status register, a separate writer and reader thread
// will be created for use by the testbench.
// ------------------------------------------------------------------------
void model_jxz_tlm_tb::singleInitiatorThread(void)
{
  m_utils.printMsg(TLMG_PRINT_START_OF_SIM,
                   "## Start of vectors from MAT file.  Will display '.' for every 100 vectors played.\n");

  // ------------------------------------------------------
  // Set core TLM component timing mode: Untimed or Timed.
  // ------------------------------------------------------
  // Set whether the target should return delays based on the specified timing
  // parameters in the 'TLM Generation' options or to just return '0' for all
  // transaction and algorithm delays.
  //
  // In your TLM environment, this can be dynamically programmed during
  // simulation execution, but for this testbench we set it just once at the
  // beginning of the simulation based on the 'TLM Testbench' configset
  // options.
  //
  // m_backdoorcfg->SetTimingParam(mw_backdoorcfg_IF::UNTIMED);
  m_backdoorcfg->SetTimingParam(mw_backdoorcfg_IF::TIMED);

  // The timing mode also needs to be set up in a testbench support object.
  //
  // With timing  Enable quantum  Testbench timing mode
  // -----------  --------------  ---------------------
  //     No            X          Untimed
  //     Yes           No         FunctionallyTimed
  //     Yes           Yes        LooselyTimed
  //
  // Untimed           : Sync to '0' delays after every transaction and for
  //                     every timed wait.
  // Functionally Timed: Sync to the delays returned from the target after
  //                     every transaction and use non-zero times for timed
  //                     waits.
  // Loosely Timed     : Track returned delays from the target by using a
  //                     tlm_quantumkeeper and sync when the quantum has been
  //                     reached.  Occassionally, for proper operation of the
  //                     initiator threads, perform synchronization-on-demand
  //                     prior to reaching the quantum.
  //
  // mw_sync_tb       sync(m_utils, mw_sync_tb::Untimed);
  mw_sync_tb sync(m_utils, mw_sync_tb::FunctionallyTimed);

  // mw_sync_tb       sync(m_utils, mw_sync_tb::LooselyTimed, m_quantumTime);

  // ------------------------------------------------------
  // Instantiate thread-specific testbench support objects.
  // ------------------------------------------------------
  // Each initiator thread needs to keep track of their local time
  // independently.  Here, we instantiate the main time-dependent testbench
  // helper objects for TLM transactions and a command/status register
  // interface.
  //
  // mw_tr_tb  : Interface for executing TLM transactions based on current tb
  //             timing mode.
  // mw_csr_tb : Interface for executing command/status register TLM
  //             transactions and higher-level CSR functions such as polling.
  //
  mw_tr_tb tr(m_utils, sync, m_tlm2_isocket);
  mw_csr_tb csr(m_utils, sync, tr, COMSTAT_REG_ADDR, m_mutex);
  bool endOfVectors = false;
  while (!endOfVectors) {
    endOfVectors = m_vecmat.getNextInput();
    if (!endOfVectors) {
    } else {
      break;
    }

    // --------------------------------------------------
    // Wait until there is data to read.
    // --------------------------------------------------
    // We can wait for data ready by either polling the INTR bit in the CSR,
    // or by waiting for the IRQ signal attached to the m_int_ip port of the
    // testbench.  If neither is available, we can only do a timed wait.
    //
    // Using IRQ:
    sync.syncToDataReady(m_int_ip);

    //
    // Using CSR:
    // m_utils.printMsg(TLMG_PRINT_SYNC_TO_FUNCS,
    //         "##\t polling status bit in register for data ready in output buffer...\n");
    // bool sawTimeout = csr.pollForStatus(INT_STAT_BIT_MASK, INT_STAT_BIT_MASK, csreg,
    //                                     1000, sc_time(1000, SC_NS));
    // if (sawTimeout) {
    //     m_utils.printErr(TLMG_ERR_SYNC_TIMEOUT,
    //         "## ERROR: Timed-out waiting for buffer non-empty status.\n");
    //     break;
    // }

    // We compare the actual results with the expected results from the
    // Simulink vector generation.
    //
    m_vecmat.putNextActualOutput();    // write actual output to MAT file
    endOfVectors = m_vecmat.getNextExpectedOutput();// read expeced output from MAT file
    m_vecmat.compareResult();
  }

  // ------------------------------------------------------
  // Simulation summary.
  // ------------------------------------------------------
  // We cannot check buffer status bits without a command/status register.
  sync.syncToLocalTime();
  if (!endOfVectors) {
    m_utils.printMsg(TLMG_PRINT_END_OF_SIM,
                     "## Error condition caused pre-mature end to vector replay.  Terminating simulation.\n");
    m_vecmat.terminate();
    sc_stop();
  } else {
    m_utils.printMsg(TLMG_PRINT_END_OF_SIM,
                     "## end of data...Terminating initiator thread.\n");
  }

  m_vecmat.terminate();
  sc_stop();
}

// ------------------------------------------------------------------------
// initialize all of the port info members:
// m_inFieldInfo, m_actOutFieldInfo, m_expOutFieldInfo
//
void model_jxz_tlm_tb::initialize()
{
  // --------------------------------
  // initialize the MAT file fields:
  //   pmxa   : pointer to main mxArray in MAT File
  //   pslf   : pointer to specific field of that mxArray
  //   sldsize: the size of the data mxArray pointer for that field
  // --------------------------------
  m_vecmat.initialize(m_inFieldInfo, m_actOutFieldInfo, m_expOutFieldInfo);

  // --------------------------------
  // initialize the C datatype fields:
  //   pcf    : pointer to the field in the C structure
  //   cfsize : size of the field in the C structure
  //   tlmaddr: TLM address of field in the C structure
  // --------------------------------

  // initialize the structure for info about the input data set
  //

  // initialize the structure for the info about the output data set
  //

  // same with the expected output data set--we use this when doing a data
  // comparison.
  //

  // Since we have generated a TLM with a single input and output address, we do
  // not need to initialize a structure to hold the TLM addresses for
  // individual inputs and outputs.  We will directly specify the appropriate
  // address argument when sending TLM transactions.
  //
  // Initialize the input TLM addresses:  not needed
  // Initialize the output TLM addresses: not needed
  //

  // ------------------------------------------------------
  // Adjust global quantum if necessary
  // ------------------------------------------------------
  // When running a temporally-decoupled simulation, it is important that the
  // global quantum be chosen carefully to avoid synchronization issues
  // between the various threads in the system.  It is possible to choose a
  // value that is too coarse or too fine for the system in which the core
  // component is being integrated.  Since we are using a generic
  // synchronization scheme between the reader and writer threads, we adjust the
  // quantum so that proper synchronization should occur.
  //
  // In your TLM environment, you should choose a value that makes sense for
  // your system.
  //
  m_quantumTime = QUANTUM_TIME;
  if (QUANTUM_TIME > OUT_BUFFER_DEPTH*ALG_PROC_TIME) {
    m_quantumTime = OUT_BUFFER_DEPTH*ALG_PROC_TIME;
  }

  // ------------------------------------------------------
  // Adjust buf write count if necessary
  // ------------------------------------------------------
  // When running a multi-threaded simulation, it is necessary to ensure there
  // are no buffer overflow or underflow scenarios.  Since we are using a generic
  // synchronization scheme between the reader and writer threads, we adjust
  // the buffer write count so that the writerThread will not cause an overflow.
  //
  // In your TLM environment, you should construct initiators and their
  // write and read data rates such that overflow and underflow do not happen.
  //
  m_bufWriteCount = (IN_BUFFER_DEPTH <= OUT_BUFFER_DEPTH) ?
    IN_BUFFER_DEPTH : OUT_BUFFER_DEPTH;
}
