/*
 * File: model_jxz.h
 *
 * Code generated for Simulink model 'model_jxz'.
 *
 * Model version                  : 1.10
 * Simulink Coder version         : 8.1 (R2011b) 08-Jul-2011
 * TLC version                    : 8.1 (Aug  6 2011)
 * C/C++ source code generated on : Mon Nov 21 13:41:03 2011
 *
 * Target selection: tlmgenerator.tlc
 * Embedded hardware selection: Generic->Custom
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_model_jxz_h_
#define RTW_HEADER_model_jxz_h_
#ifndef model_jxz_COMMON_INCLUDES_
# define model_jxz_COMMON_INCLUDES_
#include <stddef.h>
#include "rtwtypes.h"
#endif                                 /* model_jxz_COMMON_INCLUDES_ */

#include "model_jxz_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

#ifndef rtmGetStopRequested
# define rtmGetStopRequested(rtm)      ((void*) 0)
#endif

/* Real-time Model Data Structure */
struct RT_MODEL_model_jxz {
  const char_T *errorStatus;
};

#ifdef __cplusplus

extern "C" {

#endif

  /* Model entry point functions */
  extern void model_jxz_initialize(RT_MODEL_model_jxz *const model_jxz_M);
  extern void model_jxz_step(void);
  extern void model_jxz_terminate(void);

#ifdef __cplusplus

}
#endif

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S1>/Gain' : Unused code path elimination
 * Block '<S2>/Scope' : Unused code path elimination
 * Block '<S3>/Constant' : Unused code path elimination
 */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'model_jxz'
 * '<S1>'   : 'model_jxz/dut'
 * '<S2>'   : 'model_jxz/monitor'
 * '<S3>'   : 'model_jxz/stimulus'
 */
#endif                                 /* RTW_HEADER_model_jxz_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
