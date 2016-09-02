/* ******************************************************************** */ 
/** class Inject
 *  Version $Id: $
 *  date: $Date: $
 *  @author D. Jason Koskinen
 *  (c) 2012 IceCube Collaboration
 *
 *  \brief This class will add all the parameters that are used in the
 *   vuvuzela noise generator to the Calibration frame
 *//******************************************************************* */ 
#ifndef INJECT_H_INCLUDED
#define INJECT_H_INCLUDED

#include <dataclasses/I3Map.h>
#include <dataclasses/calibration/I3Calibration.h>
#include <dataclasses/calibration/I3DOMCalibration.h>

#include <icetray/I3ConditionalModule.h>
#include <icetray/I3Context.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Logging.h>
#include <icetray/I3Units.h>

/**
 * @brief IceTray module to add the noise parameters to the frame for Vuvuzela simulation
 */
class Inject : public I3ConditionalModule
{
  public:

    /**
     * \brief Constructor:  builds an instance of the module, with the
     *         context provided by IceTray.
     */
    Inject( const I3Context& context);

    ~Inject();

    /**
     * \brief Configure: Reads the parameters from the python processing script
     */
    void Configure();

    /**
     * \brief Calibration: Add the NoiseParamsMap to the Calibration frame.
     */
    void Calibration(I3FramePtr frame);

  private:

    std::string InNoiseFileName_;

    SET_LOGGER("Inject");
};
#endif
