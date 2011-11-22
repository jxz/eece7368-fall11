#ifndef _mw_backdoorcfg_if_
#define _mw_backdoorcfg_if_
#include "systemc.h"

class mw_backdoorcfg_IF:virtual public sc_core::sc_interface
{
 public:
  enum eTimingType
  {
    UNTIMED,
    TIMED
  };

  virtual void SetTimingParam(eTimingType Type) = 0;
  virtual eTimingType GetTimingParam(void)= 0;
};

#endif                                 /* _mw_backdoorcfg_if_ */
