/**
 * I3ROOTTableService.cxx (pybindings)
 *
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3ROOTTableService.cxx 114067 2013-12-05 01:39:00Z jvansanten $
 *
 * @version $Revision: 114067 $
 * @date $LastChangedDate: 2013-12-04 19:39:00 -0600 (Wed, 04 Dec 2013) $
 * @author Fabian Kislat <fabian.kislat@desy.de> Last changed by: $LastChangedBy: jvansanten $
 */

#include <rootwriter/I3ROOTTableService.h>

namespace bp = boost::python;


void register_I3ROOTTableService() {

  typedef bp::init<const std::string&,const std::string&,
    int,const std::string&> ctor;
  typedef bp::init<I3::dataio::shared_filehandle,const std::string&,
    int,const std::string&> fh_ctor;
  bp::class_<I3ROOTTableService, 
    boost::shared_ptr<I3ROOTTableService>, bp::bases<I3TableService> >
    ("I3ROOTTableService",ctor((bp::args("filename"), bp::arg("master")="MasterTree",
        bp::arg("compression_level")=1, bp::arg("mode")="RECREATE")))
    .def(fh_ctor((bp::args("filehandle"), bp::arg("master")="MasterTree",
        bp::arg("compression_level")=1, bp::arg("mode")="RECREATE")))
    ;

  bp::def("setMaxTreeSize", &I3ROOTTableService::setMaxTreeSize, bp::args("maxSize"),
	  "Set the maximum file size in MiB. ROOT will automatically do the splitting.");

}
