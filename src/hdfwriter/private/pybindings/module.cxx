/**
 * hdf-writer pybindings
 *
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: module.cxx 87951 2012-05-04 17:19:19Z olivas $
 *
 * @version $Revision: 87951 $
 * @date $LastChangedDate: 2012-05-04 12:19:19 -0500 (Fri, 04 May 2012) $
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy: olivas $
 */

#include <icetray/load_project.h>

namespace bp = boost::python;

#define REGISTER_THESE_THINGS \
   (I3HDFTableService)

#define I3_REGISTRATION_FN_DECL(r, data, t) void BOOST_PP_CAT(register_,t)();
#define I3_REGISTER(r, data, t) BOOST_PP_CAT(register_,t)();
BOOST_PP_SEQ_FOR_EACH(I3_REGISTRATION_FN_DECL, ~, REGISTER_THESE_THINGS)

I3_PYTHON_MODULE(hdfwriter)
{

  load_project("hdfwriter", false); 

  BOOST_PP_SEQ_FOR_EACH(I3_REGISTER, ~, REGISTER_THESE_THINGS);
}
