/* ******************************************************************** */ 
/** class Vuvuzela
 *  Version $Id: $
 *  date: $Date: $
 *  @author Michael Larson
 *  (c) 2012 IceCube Collaboration
 *
 *  \brief A noise generator for IceCube used to simulate both thermal noise
 *   and decay+scintillation noise.
 *//******************************************************************* */ 
#ifndef VUVUZELASET_H_INCLUDED
#define VUVUZELASET_H_INCLUDED

#include <algorithm>
#include <vector>
#include <set>
#include <iterator>

#include "icetray/I3ConditionalModule.h"
#include "icetray/I3Frame.h"
#include "icetray/I3Units.h"

#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/calibration/I3Calibration.h"
#include "dataclasses/calibration/I3DOMCalibration.h"
#include "dataclasses/physics/I3EventHeader.h"
#include "simclasses/I3MCPE.h"

#include "phys-services/I3RandomService.h"

#include "vuvuzela/VuvuzelaFunctions.h"

/**
 * @brief IceTray module to simulate thermal and correlated noise
 */
class Vuvuzela : public I3Module
{
  public:

    /**
     * \brief Constructor:  builds an instance of the module, with the
     *         context provided by IceTray.
     */
    explicit Vuvuzela(const I3Context& ctx);

    ~Vuvuzela();

    /**
     * \brief Configure: Reads the parameters from the python processing script
     */
    void Configure();

    /**
     * \brief DAQ: Add thermal and nonthermal noise to the frame
     */
    void DAQ(I3FramePtr frame);

    /**
     * \brief Finish: Print out the number of hits and the total livetime of noise
     */
    void Finish();

    /**
     * \brief GetGoodDoms: Produce the list of DOMs to simulate
     */
    void GetGoodDoms(const I3Geometry& geometry,
		     const I3Calibration& calibration);

    /**
     * \brief GetNoiseHits: Produce the map of noise hits to be added to this frame
     */
    I3MCPESeriesMapConstPtr GetNoiseHits(const I3Calibration& calibration,
					  double start, double stop);


 private:

    // Option variables
    std::string inputHitSeriesMapName_;
    std::string outputHitSeriesMapName_;
    std::string inputI3EventHeaderName_;

    std::vector<OMKey> excludeList_;
    std::vector<OMKey> goodDOMs;

    double startWindow_;
    double endWindow_;
    double scaleFactor_;
    double deepCoreScaleFactor_;

    bool icetop_;
    bool inIce_;
    bool useIndividual_;    
    bool simulateNewDoms_;
    bool useEventHeaderBounds_;
    bool disableLowDTcutoff_;

    double thermalRate_;
    double decayRate_; 
    double scintillationMean_;
    double scintillationSigma_;
    double scintillationHits_; 

    // Runtime variables
    I3RandomServicePtr randomService;
    std::string randomServiceName_;
    I3Map<OMKey, std::set<double> > bufferMap;

    bool firstTime;
    int nhits;
    double bufferTime;

    SET_LOGGER("Vuvuzela");
};

#endif

