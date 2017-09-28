
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
 * @file   example_ssn_rest.cc
 * @brief  ssn_rest module example
 * @author Hiroki SHIROKURA
 * @date   2017.9.20
 */

#include <ssn_rest.h>
#include <ssn_green_thread.h>
#include <ssn_native_thread.h>
#include <ssn_cpu.h>
#include <ssn_common.h>
#include <slankdev/util.h>
#include <slankdev/socketfd.h>


void callback_root(int fd, const void* buf, size_t len, void* a)
{
  slankdev::fdprintf(fd,
      "{"
         "\"avalable_route\" : ["
            "{ \"route\"   : \"author: get statistics data\" },"
            "{ \"route\"   : \"stats : get author infos\"    } "
         "]"
      "}"
      );
}

void callback_authorr(int fd, const void* buf, size_t len, void* a)
{
  slankdev::fdprintf(fd,
      "{"
        "\"name\"    : \"Hiroki SHIROKURA\"     ,"
        "\"emal\"    : \"slank.dev@gmail.com\"  ,"
        "\"twitter\" : \"@slankdev\"            ,"
        "\"github\"  : \"slankdev\"              "
      "}"
      );
}

void callback_stats(int fd, const void* buf, size_t len, void* a)
{
  slankdev::fdprintf(fd,
      "{"
        "\"nb_vnfs\" : 3,"
        "\"nb_nics\" : 4,"
        "\"nb_thds\" : 5 "
      "}"
      );
}

int main(int argc, char** argv)
{
  ssn_init(argc, argv);

  ssn_rest rest(INADDR_ANY, 8888);
  rest.add_route("/"       , callback_root   , nullptr);
  rest.add_route("/stats"  , callback_stats  , nullptr);
  rest.add_route("/author" , callback_authorr, nullptr);

  ssn_native_thread_launch(ssn_rest_poll_thread, &rest, 1);
  getchar();
  ssn_rest_poll_thread_stop();

  ssn_wait_all_lcore();
  ssn_fin();
}


