#ifndef _model_jxz_tlm_tb_def_h_
#define _model_jxz_tlm_tb_def_h_
#include "model_jxz_tlm_def.h"
#define NUM_IN_PORTS                   0.0
#define NUM_OUT_PORTS                  0.0
#define NUM_VECTORS                    0.0
#define IN_BUFFER_DEPTH                0
#define OUT_BUFFER_DEPTH               0
#define ALG_PROC_TIME                  100.0
#define WRSINGLE_TIME                  10.0
#define WRBURST_TIME                   10.0
#define RDSINGLE_TIME                  10.0
#define RDBURST_TIME                   10.0
#define QUANTUM_TIME                   1000.0
#ifndef model_jxz_COMSTAT_REG_ADDR
#define COMSTAT_REG_ADDR               -1
#else
#define COMSTAT_REG_ADDR               model_jxz_COMSTAT_REG_ADDR
#endif

class model_jxz_tlm_tb;
typedef tlm_utils::simple_initiator_socket<model_jxz_tlm_tb> mw_tlmsocket_tb;
typedef sc_core::sc_in<bool> mw_irq_tb;
typedef uint32_T mw_comstatreg_tb;

#define INPUT_VECTOR_FILE              "vectors/tlmg_tlminvec.mat"
#define OUTPUT_VECTOR_FILE             "vectors/tlmg_tlmoutvec.mat"
#endif
