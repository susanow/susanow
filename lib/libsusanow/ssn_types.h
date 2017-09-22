

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
 * @file   ssn_types.h
 * @brief  typedefs
 * @author Hiroki SHIROKURA
 * @date   2017.9.20
 */

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
