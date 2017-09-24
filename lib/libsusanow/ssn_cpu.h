

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

#pragma once
#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <ssn_types.h>



void ssn_cpu_init(int argc, char** argv);
bool ssn_cpu_debug_dump(FILE* fp);

ssn_lcore_state ssn_get_lcore_state(size_t lcore_id);
void ssn_set_lcore_state(ssn_lcore_state s, size_t lcore_id);

size_t ssn_lcore_id();
size_t ssn_lcore_count();

bool ssn_lcoreid_is_green_thread(size_t lcore_id);
bool ssn_lcoreid_is_tthread(size_t lcore_id);

void ssn_sleep(size_t msec);

void ssn_yield();
