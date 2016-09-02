/**
 * I3TableService.cxx (pybindings)
 *
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3HDFTableService.cxx 114027 2013-12-04 05:27:48Z jvansanten $
 *
 * @version $Revision: 114027 $
 * @date $LastChangedDate: 2013-12-03 23:27:48 -0600 (Tue, 03 Dec 2013) $
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy: jvansanten $
 */

#include <hdfwriter/I3HDFTableService.h>

namespace bp = boost::python;


void register_I3HDFTableService() {
	
   typedef bp::init<const std::string&, int, char> ctor;
   typedef bp::init<I3::dataio::shared_filehandle, int, char> fh_ctor;
   bp::class_<I3HDFTableService, 
      boost::shared_ptr<I3HDFTableService>, bp::bases<I3TableService> >
      ("I3HDFTableService", ctor((bp::args("filename"),
          bp::arg("compression_level")=1, bp::arg("mode")='w')))
      .def(fh_ctor((bp::args("filehandle"),
          bp::arg("compression_level")=1, bp::arg("mode")='w')))
      ;
}
