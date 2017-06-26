
#pragma once
#include <stdint.h>
#include <stddef.h>

enum ssn_log_level {
  SSN_LOG_EMERG  = 1,
  SSN_LOG_ALERT  = 2,
  SSN_LOG_CRIT   = 3,
  SSN_LOG_ERR    = 4,
  SSN_LOG_WARN   = 5,
  SSN_LOG_NOTICE = 6,
  SSN_LOG_INFO   = 7,
  SSN_LOG_DEBUG  = 8,
};

const char* ssn_log_level2str(ssn_log_level e);
void ssn_openlog_stream(FILE* fp);
void ssn_log(ssn_log_level lv, const char* fmt, ...);
ssn_log_level ssn_log_get_level();
void ssn_log_set_level(ssn_log_level lv);




