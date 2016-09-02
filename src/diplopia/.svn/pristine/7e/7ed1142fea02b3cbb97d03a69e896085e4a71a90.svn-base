//
//   Copyright (c) 2015   Juan Carlos Diaz Velez and the IceCube Collaboration 
//   
//   This is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
//   This sofware is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <icetray/python/context_suite.hpp>
#include <diplopia/CoincidentI3ReaderService.h>
#include <diplopia/MuonGunBackgroundService.h>

using namespace boost::python;
namespace bp = boost::python;



//class_<T, boost::shared_ptr<T>, boost::noncopyable>
template <typename T, typename Init>
class_<T, boost::shared_ptr<T>, boost::python::bases<I3GeneratorService>, boost::noncopyable>
register_coincidenteventservice(const char* name, const char* doc, const Init& init)
{
  //return class_<T, boost::shared_ptr<T>, boost::noncopyable>(name, doc, init) ;
  return class_<T, boost::shared_ptr<T>, boost::python::bases<I3GeneratorService>, boost::noncopyable>(name,
							     doc,
							     init)
    ;
}


void register_I3GeneratorService()
{
  register_coincidenteventservice<CoincidentI3ReaderService>("CoincidentI3ReaderService", "corsika reader for coincident bg", init<>())
	        .def("get_rate", &CoincidentI3ReaderService::GetRate)
	        .def("get_next", &CoincidentI3ReaderService::GetNextEvent)
	        .def("get_next_frame", &CoincidentI3ReaderService::GetNextFrame)
	        .def("open", &CoincidentI3ReaderService::Open)
	;

  register_coincidenteventservice<MuonGunBackgroundService>("MuonGunBackgroundService", "MuonGun for coincident bg", init<>())
	        .def("get_rate", &MuonGunBackgroundService::GetRate)
	        .def("get_next", &MuonGunBackgroundService::GetNextEvent)
	        .def("get_next_frame", &MuonGunBackgroundService::GetNextFrame)
	        .def("set_rate", &MuonGunBackgroundService::SetRate)
	        .def("set_mctree_name", &MuonGunBackgroundService::SetMCTreeName)
	        .def("set_rng", &MuonGunBackgroundService::SetRNG)
	        .def("set_generator", &MuonGunBackgroundService::SetGenerator)
	;

}
