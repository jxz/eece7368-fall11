#include "model_jxz_tlm.h"

using namespace std;
model_jxz_tlm::model_jxz_tlm( sc_core::sc_module_name module_name, eTimingType
  DefaultTiming, eModeType InputDefaultMode, eModeType OutputDefaultMode)
  : sc_module(module_name),
  m_tlm2_tsocket("tlm2_tsocket"),
  m_int_op("m_int_op"),
  m_start_event("start_event"),
  m_trigint("trigint"),
  m_trig(true), m_ack(false),
  m_current_timing(DefaultTiming),
  m_input_default_mode(InputDefaultMode),
  m_output_default_mode(OutputDefaultMode)
{
  m_int_op.initialize(false);
  reg_init();
  m_tlm2_tsocket.register_b_transport(this, &model_jxz_tlm::entry_b_transport);
  m_tlm2_tsocket.register_transport_dbg(this, &model_jxz_tlm::
    entry_transport_dbg);
  SC_THREAD(targetThread);
  SC_METHOD(intMethod);
  sensitive << m_trigint.get_event();
  dont_initialize();
}

model_jxz_tlm::~model_jxz_tlm(void)
{
  model_jxz_terminate();
  ;
}

//==============================================================================
//  b_transport implementation calls from initiators
//
//=============================================================================
void model_jxz_tlm::entry_b_transport (tlm::tlm_generic_payload &gp, sc_core::
  sc_time &delay_time)
{
  tlm::tlm_command command = gp.get_command();// memory command
  unsigned char *data = gp.get_data_ptr();// data pointer
  unsigned int length = gp.get_data_length();// data length
  unsigned long timefactor = ((length+BUSWIDTHBYTE-1)/BUSWIDTHBYTE)-1;
  bool write_ok = true;
  gp.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
  return;
}

//==============================================================================
//  dbg_transport implementation calls from initiators
//
//=============================================================================
unsigned int model_jxz_tlm::entry_transport_dbg (tlm::tlm_generic_payload &gp)
{
  tlm::tlm_command command = gp.get_command();// memory command
  unsigned char *data = gp.get_data_ptr();// data pointer
  unsigned int length = gp.get_data_length();// data length
  gp.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
  return 0;
}

void model_jxz_tlm::targetThread (void)
{
  bool read_ok;
  sc_core::sc_time delay_time = sc_core::SC_ZERO_TIME;
  while (1) {
    //Auto trigger
    if ((m_comstat_reg & INPUT_AUTMOD_BIT_MASK)) {
      m_start_event.notify(sc_core::SC_ZERO_TIME);
    }

    read_ok = false;
    while (!read_ok) {
      wait(m_start_event.default_event());
      read_ok = true;
    }

    model_jxz_step();
    delay_time += AssertDelay(sc_core::sc_time(100.0, sc_core::SC_NS));
    wait(AssertDelay(delay_time));
    delay_time = sc_core::SC_ZERO_TIME;
    m_trigint.notify(m_trig,delay_time);
  }
}

void model_jxz_tlm::intMethod (void)
{
  const bool* val;
  while (NULL!=(val=m_trigint.get_next_transaction())) {
    if (*val)                          //trigger interrupt
    {
      m_comstat_reg |= INT_STAT_BIT_MASK;
      if (!(m_comstat_reg & INT_DIS_BIT_MASK)) {
        m_int_op.write(true);          // trigger interrupt port;
      }
    }

    if (!*val)                         //acknowledge interrupt
    {
      m_comstat_reg &= ~INT_STAT_BIT_MASK;
      m_int_op.write(false);           // ack interrupt port;
    }
  }
}

void model_jxz_tlm::reg_init(void)
{
  m_start_event.cancel_all();
  m_trigint.cancel_all();
  m_trigint.notify(m_ack,sc_core::SC_ZERO_TIME);
  m_testset_reg = 0;
  m_comstat_reg = 0;
  model_jxz_initialize(&(m_model_jxz_M));
  m_comstat_reg |= INBUF_EMPTY_BIT_MASK;
  m_comstat_reg |= OUTBUF_EMPTY_BIT_MASK;
  m_comstat_reg = (m_input_default_mode == AUTO)? (m_comstat_reg |
    INPUT_AUTMOD_BIT_MASK):m_comstat_reg ;
  m_comstat_reg = (m_output_default_mode == AUTO)? (m_comstat_reg |
    OUTPUT_AUTMOD_BIT_MASK):m_comstat_reg ;
}
