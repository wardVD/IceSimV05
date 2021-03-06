//
//   Copyright (c) 2009
//   the icecube collaboration
//   $Id: module.cxx 138010 2015-09-28 22:01:13Z juancarlos $
//                                                                                                       
//   @version $Revision: 138010 $                                                                                                    
//   @author Henrike Wissing 
//

#include <icetray/I3FrameObject.h>
#include <icetray/load_project.h>

using namespace boost::python;
namespace bp = boost::python;

void register_I3SumGenerator();
void register_I3PropagatorService();
void register_I3SimConstants();
void register_I3GeneratorService();

BOOST_PYTHON_MODULE(sim_services)
{
  load_project("libsim-services", false);

  register_I3SumGenerator();
  register_I3PropagatorService();
  register_I3GeneratorService();
  register_I3SimConstants();
}

