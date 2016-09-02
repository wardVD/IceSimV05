#include "wrappers.h"

#include "photonics-service/I3PhotonicsTableService.h"

#define SERVFIELDS (SetInterpolationMode)(SetPhotonicsVerbosity)	\
    (LoadTables)(ClearTables)

using namespace boost::python;

void register_I3PhotonicsTableService() {
	class_<I3PhotonicsTableService,
	    boost::shared_ptr<I3PhotonicsTableService >,
	    bases<I3PhotonicsService > >("I3PhotonicsTableService")
	    .def(init<const std::string &, const std::string &,
	        const std::string &, const std::string &, int, int, double,
	        double, double, double, int, bool>(
	        args("photonicsTopLevelDirectory", "driverFileDirectory",
	    	"level1DriverFile", "level2DriverFile", "interpolMode",
	    	"verbosity", "angularSelectLow", "angularSelectHigh",
	    	"zSelectLow", "zSelectHigh", "photonicsTableSelection",
	    	"isPhotorecTable"),
	        "Create an I3PhotonicsTableService object. This method performs"
	        " the logic of the former factory."))
	    BOOST_PP_SEQ_FOR_EACH(WRAP_DEF, I3PhotonicsTableService, SERVFIELDS);
}
