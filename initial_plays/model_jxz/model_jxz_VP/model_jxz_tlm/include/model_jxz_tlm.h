#ifndef _model_jxz_tlm_
#define _model_jxz_tlm_
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/tlm_quantumkeeper.h"
#include "tlm_utils/peq_with_get.h"
#include "model_jxz.h"
#include "model_jxz_tlm_def.h"
#include "mw_support.h"
#include "mw_backdoorcfg_if.h"

using namespace std;
class model_jxz_tlm
  : public sc_core::sc_module, public mw_backdoorcfg_IF
{
 public:
  enum eModeType
  {
    MANUAL,
    AUTO
  };

  static const int BUSWIDTHBYTE = 4;
  SC_HAS_PROCESS(model_jxz_tlm);
  model_jxz_tlm( sc_core::sc_module_name module_name, eTimingType DefaultTiming =
                TIMED, eModeType InputDefaultMode = AUTO, eModeType
                OutputDefaultMode = AUTO);
  ~model_jxz_tlm(void);
  tlm_utils::simple_target_socket<model_jxz_tlm,BUSWIDTHBYTE*8> m_tlm2_tsocket;
  sc_core::sc_out<bool> m_int_op;      // interrupt port; active high
  virtual void SetTimingParam(eTimingType Type)
  {
    m_current_timing = Type;
  }

  ;
  virtual eTimingType GetTimingParam(void)
  {
    return m_current_timing;
  }

  ;
 private:
  sc_core::sc_event_queue m_start_event;
  tlm_utils::peq_with_get<const bool> m_trigint;
  const bool m_trig, m_ack;
  void intMethod (void);
  void reg_init(void);
  RT_MODEL_model_jxz m_model_jxz_M;
  uint32_T m_comstat_reg;
  uint32_T m_testset_reg;
  eTimingType m_current_timing;
  eModeType m_input_default_mode;
  eModeType m_output_default_mode;
  inline sc_core::sc_time AssertDelay(sc_core::sc_time delay)
  {
    return (m_current_timing==UNTIMED)?sc_core::sc_time(0,sc_core::SC_NS):delay;
  }

  ;
  void entry_b_transport (tlm::tlm_generic_payload &gp, sc_core::sc_time &
    delay_time);
  unsigned int entry_transport_dbg (tlm::tlm_generic_payload &gp);
  void targetThread(void);
};

#endif                                 /* _model_jxz_tlm_ */
