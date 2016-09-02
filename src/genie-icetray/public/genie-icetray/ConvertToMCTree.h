/**
 * copyright (C) 2015
 * Melanie Day <melanie.day@icecube.wisc.edu>
 *
 * @file ConvertToMCTree.h
 * @version $Revision$
 * @author Melanie Day 
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

#ifndef CONVERTTOMCTREE_H_INCLUDED
#define CONVERTTOMCTREE_H_INCLUDED

#ifndef I3PARTICLE_SUPPORTS_PDG_ENCODINGS
#include "genie-icetray/ConvertPDGEncodingToI3ParticleType.h"
#endif

// dataclasses
 #include "dataclasses/I3Map.h"
 #include "dataclasses/physics/I3Particle.h"
 #include "dataclasses/physics/I3MCTree.h"
 #include "dataclasses/physics/I3MCTreeUtils.h"

#include "genie-icetray/I3GENIEResultDict.h"

namespace I3GENIE_Helper
{
    /**
     * Convert a GENIEResultDict into an I3MCTree.
     * Moved from I3GENIEResultDictToMCTree module 
     */
//These things are defined as pointers in I3GENIEResultDictToMCTree to make them nice to output to an i3 file
    bool ConvertToMCTree(I3MCTreePtr mcTree,  I3MapStringDoublePtr weightDict, I3GENIEResultDictConstPtr resultDict);

    template <typename T>  
    const T &GetFromVariantMap(const std::string &name, const I3GENIEResultDict &dict); 
};

#endif //CONVERTTOMCTREE_H_INCLUDED
