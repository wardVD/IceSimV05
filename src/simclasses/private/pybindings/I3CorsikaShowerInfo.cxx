/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3CorsikaShowerInfo.cxx 126444 2014-12-02 00:19:10Z hdembinski $
 *
 * @version $Revision: 126444 $
 * @date $LastChangedDate: 2014-12-01 18:19:10 -0600 (Mon, 01 Dec 2014) $
 * @author Fabian Kislat <fabian.kislat@desy.de> $LastChangedBy: hdembinski $
 */

#include <tableio/converter/pybindings.h>
#include "simclasses/I3CorsikaShowerInfo.h"
#include "simclasses/converter/I3CorsikaShowerInfoConverter.h"
#include <icetray/python/dataclass_suite.hpp>

namespace bp = boost::python;

void register_I3CorsikaShowerInfo()
{
  bp::class_<I3CorsikaShowerInfo, bp::bases<I3FrameObject>, I3CorsikaShowerInfoPtr>("I3CorsikaShowerInfo")
    .def(bp::dataclass_suite<I3CorsikaShowerInfo>())
    .def_readwrite("crsRunID", &I3CorsikaShowerInfo::crsRunID)
    .def_readwrite("crsEventID", &I3CorsikaShowerInfo::crsEventID)
    .def_readwrite("crsSampleID", &I3CorsikaShowerInfo::crsSampleID)
    .def_readwrite("firstIntHeight", &I3CorsikaShowerInfo::firstIntHeight)
    .def_readwrite("firstIntDepth", &I3CorsikaShowerInfo::firstIntDepth)
    .def_readwrite("obsLevelHeight", &I3CorsikaShowerInfo::obsLevelHeight)
    .def_readwrite("ghMaxNum", &I3CorsikaShowerInfo::ghMaxNum)
    .def_readwrite("ghStartDepth", &I3CorsikaShowerInfo::ghStartDepth)
    .def_readwrite("ghMaxDepth", &I3CorsikaShowerInfo::ghMaxDepth)
    .def_readwrite("ghRedChiSqr", &I3CorsikaShowerInfo::ghRedChiSqr)
    .def_readwrite("longProfile", &I3CorsikaShowerInfo::longProfile)
    .def_readwrite("resampleRadius", &I3CorsikaShowerInfo::resampleRadius)
    .def_readwrite("weight", &I3CorsikaShowerInfo::weight)
    .def_readwrite("nResample", &I3CorsikaShowerInfo::nResample)
    .def_readwrite("nResampleNominal", &I3CorsikaShowerInfo::nResampleNominal)
    .def("clear", &I3CorsikaShowerInfo::clear)
    ;
  register_pointer_conversions<I3CorsikaShowerInfo>();

  I3CONVERTER_NAMESPACE(simclasses);
  I3CONVERTER_EXPORT_DEFAULT(I3CorsikaShowerInfoConverter, "Dumps I3CorsikaShowerInfo objects with information on primary air showers");
}
