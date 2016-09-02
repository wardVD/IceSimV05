/**
 * rootwriter pybindings
 *
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: module.cxx 94948 2012-11-04 16:21:52Z nwhitehorn $
 *
 * @version $Revision: 94948 $
 * @date $LastChangedDate: 2012-11-04 10:21:52 -0600 (Sun, 04 Nov 2012) $
 * @author Fabian Kislat <fabian.kislat@desy.de> Last changed by: $LastChangedBy: nwhitehorn $
 */

#include <icetray/load_project.h>

namespace bp = boost::python;

#define REGISTER_THESE_THINGS \
   (I3ROOTTableService)

#define I3_REGISTRATION_FN_DECL(r, data, t) void BOOST_PP_CAT(register_,t)();
#define I3_REGISTER(r, data, t) BOOST_PP_CAT(register_,t)();
BOOST_PP_SEQ_FOR_EACH(I3_REGISTRATION_FN_DECL, ~, REGISTER_THESE_THINGS)

I3_PYTHON_MODULE(rootwriter)
{
  load_project("rootwriter", false); 

  bp::import("icecube.tableio");

  BOOST_PP_SEQ_FOR_EACH(I3_REGISTER, ~, REGISTER_THESE_THINGS);
}
