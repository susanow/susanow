

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

/**
 * @brief init susanow-base
 * @param [in] argc command line argc
 * @param [in] argv command line argv
 * @details
 *   User must run this function before using susanow-APIs
 *   This function call many allocation functions inside this.
 */
void ssn_init(int argc, char** argv);

/**
 * @brief init susanow subprocess
 * @param [in] argc command line argc
 * @param [in] argv command line argv
 * @details
 *   User must run this function before using susanow-APIs
 *   on Subprocess
 *   This function call many allocation functions inside this.
 */
void ssn_subproc__init(int argc, char** argv);

/**
 * @details
 *   User must run this function after using susanow-APIs
 *   This function call many free functions inside this.
 */
void ssn_fin();

/**
 * @detail
 *   This function block that finish all native-threads.
 *   however, developers plan remove this function...
 */
void ssn_wait_all_lcore();


