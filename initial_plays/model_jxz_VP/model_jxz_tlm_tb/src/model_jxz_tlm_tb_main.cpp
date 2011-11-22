#include "model_jxz_tlm_tb.h"
#include "model_jxz_tlm.h"

int sc_main(int, char **)
{
  model_jxz_tlm_tb testbench("model_jxz_tlm_tb");
  model_jxz_tlm model("model_jxz_tlm");
  sc_signal<bool> interruptSignal;

  // bind to the model for use of the backdoor interface
  testbench.m_backdoorcfg(model);

  // port binding
  testbench.m_tlm2_isocket(model.m_tlm2_tsocket);
  testbench.m_int_ip(interruptSignal);
  model.m_int_op(interruptSignal);
  std::cout << "## STARTING SIMULATION" << std::endl;

  // ensure we have a sim end time.  increase this if running vectors for
  // more than 1000 seconds.
  sc_start(1000, SC_SEC);
  std::cout << "## SIMULATION HAS ENDED" << std::endl;
  return(testbench.getErrorStatus());
}
