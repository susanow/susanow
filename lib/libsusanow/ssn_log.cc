
/*
 * MIT License
 *
 * Copyright (c) 2017 Hiroki SHIROKURA
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/**
 * @file   ssn_log.cc
 * @brief  log management
 * @author Hiroki SHIROKURA
 * @date   2017.9.20
 */

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




