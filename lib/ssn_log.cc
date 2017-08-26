
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <ssn_log.h>
#include <slankdev/exception.h>
#include <dpdk/hdr.h>


FILE* ssn_log_stdout = stderr;
ssn_log_level log_level = SSN_LOG_INFO;

const char* ssn_log_level2str(ssn_log_level e)
{
  switch (e) {
    case SSN_LOG_EMERG : return "EMERG" ;
    case SSN_LOG_ALERT : return "ALERT" ;
    case SSN_LOG_CRIT  : return "CRIT"  ;
    case SSN_LOG_ERR   : return "ERR"   ;
    case SSN_LOG_WARN  : return "WARN"  ;
    case SSN_LOG_NOTICE: return "NOTICE";
    case SSN_LOG_INFO  : return "INFO"  ;
    case SSN_LOG_DEBUG : return "DEBUG" ;
    default: throw slankdev::exception("UNKNOWN");
  }
}

void ssn_openlog_stream(FILE* fp)
{
  rte_openlog_stream(fp);
  ssn_log_stdout = fp;
}

void ssn_log(ssn_log_level lv, const char* fmt, ...)
{
  if (lv <= log_level) {
    fflush(ssn_log_stdout);
    fprintf(ssn_log_stdout, "%s: ", ssn_log_level2str(lv));

    va_list args;
    va_start(args, fmt);
    vfprintf(ssn_log_stdout, fmt, args);
    va_end(args);

    fflush(ssn_log_stdout);
  }
}

ssn_log_level ssn_log_get_level()
{
  return log_level;
}

void ssn_log_set_level(ssn_log_level lv)
{
  switch (lv) {
    case SSN_LOG_EMERG :
    case SSN_LOG_ALERT :
    case SSN_LOG_CRIT  :
    case SSN_LOG_ERR   :
    case SSN_LOG_WARN  :
    case SSN_LOG_NOTICE:
    case SSN_LOG_INFO  :
    case SSN_LOG_DEBUG :
      break;
    default: throw slankdev::exception("UNKNOWN");
  }
  uint32_t rte_loglevel = lv;
  rte_log_set_global_level(rte_loglevel);
  log_level = lv;
}




