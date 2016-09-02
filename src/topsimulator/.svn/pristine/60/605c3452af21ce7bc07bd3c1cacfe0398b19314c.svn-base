//
//   Copyright (c) 2008   Troy D. Straszheim and the IceCube Collaboration 
//   
//   This file is part of IceTray.
//
//   IceTray is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
//   IceTray is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

using namespace boost::python;
namespace bp = boost::python;

// This is direct copy of what was done for I3Calculator in phys_services

#include <topsimulator/responses/vem_yield.h>

void register_vem_yield()
{
  // map the vem_yield namespace to a sub-module  
  // make "from topsimulator import <whatever>" work  
  bp::object vem_yieldModule(bp::handle<>(bp::borrowed(PyImport_AddModule("topsimulator.vem_yield"))));  
  // make "from topsimulator import vem_yield" work  
  bp::scope().attr("vem_yield") = vem_yieldModule;  
  // set the current scope to the new sub-module  
  bp::scope vem_yield_scope = vem_yieldModule;  
  // export stuff in the vem_yield namespace  
  def("em_snow", vem_yield::em_snow);
  def("ga_snow", vem_yield::ga_snow);
  def("mm_snow", vem_yield::mm_snow);
  def("em", vem_yield::em);
  def("ep", vem_yield::ep);
  def("ga", vem_yield::ga);
  def("mm", vem_yield::mm);
  def("mp", vem_yield::mp);
  def("p", vem_yield::p);
  def("ap", vem_yield::ap);
  def("n", vem_yield::n);
  def("an", vem_yield::an);
  def("pm", vem_yield::pm);
  def("pp", vem_yield::pp);
  def("km", vem_yield::km);
  def("kp", vem_yield::kp);
  def("k0", vem_yield::k0);
}
