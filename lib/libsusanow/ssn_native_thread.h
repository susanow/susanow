
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
 * @file   ssn_native_thread.h
 * @brief  native thread management
 * @author Hiroki SHIROKURA
 * @date   2017.9.20
 */

#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <ssn_types.h>

void ssn_native_thread_init();
void ssn_native_thread_fin();

uint32_t ssn_native_thread_launch(ssn_function_t f, void* arg, size_t lcore_id);
void ssn_native_thread_join(uint32_t tid);
bool ssn_native_thread_joinable(uint32_t tid);

void ssn_native_thread_debug_dump(FILE* fp);


#if 0
/*
 * This function return after finish the thread.
 */
void ssn_lcore_join(size_t lcore_id);
bool ssn_lcore_joinable(size_t lcore_id);

void ssn_lcore_join_poll_thread_stop();
void ssn_lcore_join_poll_thread(void*);
#endif

