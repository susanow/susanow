
#pragma once

typedef void(*ssn_function_t)(void*);

enum ssn_lcore_state {
  SSN_LS_WAIT,
  SSN_LS_RUNNING_GREEN,
  SSN_LS_RUNNING_NATIVE,
  SSN_LS_RUNNING_TIMER,
  SSN_LS_FINISHED,
};

inline const char* ssn_lcore_state2str(enum ssn_lcore_state e)
{
  switch (e) {
    case SSN_LS_WAIT           : return "WAIT"    ;
    case SSN_LS_RUNNING_GREEN  : return "RUNGRN"  ;
    case SSN_LS_RUNNING_NATIVE : return "RUNNAT"  ;
    case SSN_LS_RUNNING_TIMER  : return "RUNTMR"  ;
    case SSN_LS_FINISHED       : return "FINISHED";
    default: return "UNKNOWN";
  }
}
