
/*
 * MIT License
 *
 * Copyright (c) 2017 Susanow
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

#include <ssn_common.h>
#include <ssn_ma_ring.h>

int main(int argc, char** argv)
{
  constexpr size_t n_que = 8;
  constexpr size_t n_enq_acc = 4;
  constexpr size_t n_deq_acc = 2;
  ssn_init(argc, argv);

  ssn_ma_ring ring0("test_ring");
  ring0.configure_que(n_que);
  ring0.configure_acc(n_enq_acc, n_deq_acc);
  ring0.debug_dump(stdout);

  size_t n_enq;
  size_t n_deq;
  size_t* send_buf[1] = { nullptr };
  size_t* recv_buf[32];

  slankdev::waitmsg("phase 1");
  n_enq = ring0.enqueue_burst(0, (void**)send_buf, 1);
  printf("n_enq: %zd \n", n_enq);
  ring0.debug_dump(stdout);

  slankdev::waitmsg("phase 2");
  n_enq = ring0.enqueue_burst(0, (void**)send_buf, 1);
  printf("n_enq: %zd \n", n_enq);
  ring0.debug_dump(stdout);

  slankdev::waitmsg("phase 3");
  n_enq = ring0.enqueue_burst(1, (void**)send_buf, 1);
  printf("n_enq: %zd \n", n_enq);
  ring0.debug_dump(stdout);

  slankdev::waitmsg("phase 4");
  n_deq = ring0.dequeue_burst(0, (void**)recv_buf, 32);
  printf("n_deq: %zd \n", n_deq);
  ring0.debug_dump(stdout);

  slankdev::waitmsg("phase 4");
  n_deq = ring0.dequeue_burst(0, (void**)recv_buf, 32);
  printf("n_deq: %zd \n", n_deq);
  ring0.debug_dump(stdout);

  slankdev::waitmsg("phase 4");
  n_deq = ring0.dequeue_burst(0, (void**)recv_buf, 32);
  printf("n_deq: %zd \n", n_deq);
  ring0.debug_dump(stdout);

  slankdev::waitmsg("phase 5");
  n_enq = ring0.enqueue_burst(0, (void**)send_buf, 1);
  printf("n_enq: %zd \n", n_enq);
  ring0.debug_dump(stdout);
}



