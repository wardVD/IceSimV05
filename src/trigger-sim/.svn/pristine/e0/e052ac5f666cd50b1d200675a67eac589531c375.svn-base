/**
 * copyright  (C) 2004
 * the icecube collaboration
 *  $Id: I3Pruner.h,v 1.1.1.1 2004/11/06 21:00:46 bouchta Exp $
  *
 * @file I3Pruner.h
 * @version $Revision: 1.1.1.1 $
 * @date $Date: 2004/11/06 21:00:46 $
 * @author olivas
 */

#ifndef I3PRUNER_H
#define I3PRUNER_H

#include "icetray/I3ConditionalModule.h"
/**
 * @brief IceTray module to remove launches outside the readout window
 */
class I3Pruner : public I3ConditionalModule
{
public:

    I3Pruner(const I3Context& ctx);
    ~I3Pruner();
    void Configure();

    void DAQ(I3FramePtr frame);

private:

    I3Pruner();
    I3Pruner(const I3Pruner& source);
    I3Pruner& operator=(const I3Pruner& source);
    
    std::vector<std::string> dataReadoutNames_;
    std::string triggerName_;

    SET_LOGGER("I3Pruner");

};	// end of class I3Pruner

#endif //I3PRUNER_H
