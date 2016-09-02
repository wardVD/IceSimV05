/**
 * Definition of I3FirstPulsifier class
 *
 * copyright (C) 2012
 * The IceCube collaboration
 * $Id$
 * 
 * @file I3FirstPulsifier.h
 * @version $Revision$
 * @date $Date$
 * @author Markus Voge <voge@physik.uni-bonn.de>
 * @brief This class takes a pulse series map, extracts only the first pulse of each pulse series
 *        and puts a new pulse series map with only the first pulses into the frame. The charge of
 *        the new pulse is by default the total charge of all pulses.
 */
 
#ifndef I3FIRSTPULSIFIER_H_INCLUDED
#define I3FIRSTPULSIFIER_H_INCLUDED

#include "icetray/I3ConditionalModule.h"
#include "icetray/I3Context.h"
#include "dataclasses/physics/I3RecoPulse.h"
#include "dataclasses/I3MapOMKeyMask.h"

/**
 * @class I3FirstPulsifier
 * @brief Returns a pulse series map containing only the first pulses, however each first
 *        pulse carrying the total charge of all pulses by default
 *
 * This module takes a regular pulse series map as input and returns a pulse series map that has
 * only the first pulses in it. I.e. it converts all the pulse series to a vector of length one
 * containing only the first pulse. This new first pulse has the time and width of the original
 * first pulse, but by default carries the total charge of all pulses that are in the full pulse series. 
 * This behaviour can be modified though.
 *
 * The module is motivated by the calculation of cut variables such as Ndir and Ldir that use the
 * information of when the modules are hit. If not only the first pulse, but all pulses are used for
 * calculation of hit-time-critical cut variables, then late hits on the same module can seriously
 * disturb the result. It has been found that the cut variables effectively lose their
 * cut efficiency if all pulses are used. Calculating Ndir, Ldir etc. using all pulses is more or
 * less meaningless. So please make sure to first-pulsify your pulse series map first.
 *
 * Keeping only the time and width of the first hit, but the total charge of all hits is meant to circumvent the
 * problem mentioned above, however keeping as much information as possible. If you want to weight
 * the DOMs with their charge, then this might be interesting. However, it may also turn out that
 * keeping the charge of the later hits is not desirable. If you want to keep only the charge of the
 * first pulse, then you can do so by setting the parameter 'KeepOnlyFirstCharge' to True. 
 * 
 * 
 */

class I3FirstPulsifier : public I3ConditionalModule {
   public:
      I3FirstPulsifier(const I3Context& ctx);
      ~I3FirstPulsifier() {}
      
      void Configure();
      void Physics(I3FramePtr frame);
      
   private:
      std::string pulseSeriesMapName_;
      std::string outputPulseSeriesMapName_;
      bool keepOnlyFirstCharge_;
      bool useMask_;
};

#endif // I3FIRSTPULSIFIER_H_INCLUDED
