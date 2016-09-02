/**
 * copyright (C) 2011
 * Claudio Kopper <claudio.kopper@nikhef.nl>
 * $Id: I3GENIEResultDictToMCTree.cxx 137710 2015-09-17 21:07:13Z melanie.day $
 *
 * @file I3GENIEResultDictToMCTree.cxx
 * @version $Revision$
 * @date $Date: 2015-09-17 16:07:13 -0500 (Thu, 17 Sep 2015) $
 * @author Claudio Kopper
 *
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *  
 */

#include "genie-icetray/I3GENIEResultDictToMCTree.h"
#include "genie-icetray/I3GENIEResultDict.h"

//Helpers
#include "genie-icetray/ConvertToMCTree.h"
#ifndef I3PARTICLE_SUPPORTS_PDG_ENCODINGS
#include "genie-icetray/ConvertPDGEncodingToI3ParticleType.h"
#endif

#include "dataclasses/I3Constants.h"

#include <boost/foreach.hpp>

// dataclasses
#include "dataclasses/I3Map.h"
#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/physics/I3MCTree.h"
#include "dataclasses/physics/I3MCTreeUtils.h"

// ROOT stuff
#include "TVector3.h"

// The module
I3_MODULE(I3GENIEResultDictToMCTree);


I3GENIEResultDictToMCTree::I3GENIEResultDictToMCTree(const I3Context& context) 
: I3ConditionalModule(context)
{
    // define parameters
    resultDictName_="I3GENIEResultDict";
    AddParameter("ResultDictName",
                 "Name of the input I3GENIEResultDict frame object.",
                 resultDictName_);

    mcTreeName_="I3MCTree";
    AddParameter("MCTreeName",
                 "Name of the output I3MCTree frame object.",
                 mcTreeName_);

    weightDictName_="I3MCWeightDict";
    AddParameter("WeightDictName",
                 "Name of the output I3MCWeightDict frame object.",
                 weightDictName_);
    
    // add an outbox
    AddOutBox("OutBox");

}


I3GENIEResultDictToMCTree::~I3GENIEResultDictToMCTree()
{

}

void I3GENIEResultDictToMCTree::Configure()
{
    log_trace("%s", __PRETTY_FUNCTION__);
    
    GetParameter("ResultDictName", resultDictName_);
    GetParameter("MCTreeName", mcTreeName_);
    GetParameter("WeightDictName", weightDictName_);
    
    if (resultDictName_=="")
        log_fatal("You have to configure a non-empty name for the \"ResultDictName\" parameter.");

    if (mcTreeName_=="")
        log_fatal("You have to configure a non-empty name for the \"MCTreeName\" parameter.");

    if (weightDictName_=="")
        log_fatal("You have to configure a non-empty name for the \"WeightDictName\" parameter.");

    
    
}

#ifdef IS_Q_FRAME_ENABLED
void I3GENIEResultDictToMCTree::DAQ(I3FramePtr frame)
#else
void I3GENIEResultDictToMCTree::Physics(I3FramePtr frame)
#endif
{
    log_trace("%s", __PRETTY_FUNCTION__);

    // get input data
    I3GENIEResultDictConstPtr resultDict = frame->Get<I3GENIEResultDictConstPtr>(resultDictName_);
    if (!resultDict)
        log_fatal("Could not find an I3GENIEResultDict object with name \"%s\" in the frame.",
                  resultDictName_.c_str());

    // prepare output objects
    I3MCTreePtr mcTree = I3MCTreePtr(new I3MCTree());
    I3MapStringDoublePtr weightDict = I3MapStringDoublePtr(new I3MapStringDouble());
    //Get MCInfo from I3GENIEResultDict 
    const bool success = I3GENIE_Helper::ConvertToMCTree(mcTree,weightDict,resultDict);
    if(!success)
        log_fatal("Could not convert event to MCTree!");
    
    // add things to the frame
    frame->Put(mcTreeName_, mcTree);
    frame->Put(weightDictName_, weightDict);
    
    // push the frame
    PushFrame(frame, "OutBox");
}

