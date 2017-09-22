
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
 * @file   ssn_log.h
 * @brief  log management
 * @author Hiroki SHIROKURA
 * @date   2017.9.20
 */

#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

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




