/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Fabian Kislat <fabian.kislat@desy.de> $LastChangedBy$
 */

#include "simclasses/CorsikaLongStep.h"
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

void register_CorsikaLongStep()
{
  bp::class_<CorsikaLongStep, CorsikaLongStepPtr>("CorsikaLongStep")
    .def(bp::dataclass_suite<CorsikaLongStep>())
    .def_readwrite("depth", &CorsikaLongStep::depth)
    .def_readwrite("numGamma", &CorsikaLongStep::numGamma)
    .def_readwrite("numEMinus", &CorsikaLongStep::numEMinus)
    .def_readwrite("numEPlus", &CorsikaLongStep::numEPlus)
    .def_readwrite("numMuMinus", &CorsikaLongStep::numMuMinus)
    .def_readwrite("numMuPlus", &CorsikaLongStep::numMuPlus)
    .def_readwrite("numHadron", &CorsikaLongStep::numHadron)
    .def_readwrite("numCharged", &CorsikaLongStep::numCharged)
    .def_readwrite("numNuclei", &CorsikaLongStep::numNuclei)
    .def_readwrite("numCherenkov", &CorsikaLongStep::numCherenkov)
    ;

  bp::class_< std::vector<CorsikaLongStep> >("CorsikaLongProfile")
    .def(bp::dataclass_suite<std::vector<CorsikaLongStep> >())
    ;
}
