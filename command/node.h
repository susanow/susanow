
/*-
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
 * @file command/node.h
 * @author slankdev
 */


#pragma once
#include <slankdev/vty.h>


inline slankdev::node* fixed_port()
{ return new slankdev::node_fixedstring("port", "port operation commands"); }
inline slankdev::node* fixed_link()
{ return new slankdev::node_fixedstring("link", "link operation"); }
inline slankdev::node* fixed_dev()
{ return new slankdev::node_fixedstring("dev", "device operation"); }
inline slankdev::node* fixed_show()
{ return new slankdev::node_fixedstring("show", "show information"); }


inline slankdev::node* fixed_lthread()
{
  return new slankdev::node_fixedstring(
      "lthread", "Slow Thread running on lthreadsched");
}

inline slankdev::node* fixed_fthread()
{
  return new slankdev::node_fixedstring(
      "fthread", "Falst Thread runnig on Lcore");
}

inline slankdev::node* fixed_list()
{ return new slankdev::node_fixedstring("list", "List emements"); }

inline slankdev::node* fixed_find()
{ return new slankdev::node_fixedstring("find", "Find Thread"); }

inline slankdev::node* fixed_launch()
{ return new slankdev::node_fixedstring("launch", "Launch Thread"); }

inline slankdev::node* fixed_kill()
{ return new slankdev::node_fixedstring("kill", "Kill Thread"); }

inline slankdev::node* fixed_scheduler()
{ return new slankdev::node_fixedstring("scheduler", "lthread scheduler"); }


