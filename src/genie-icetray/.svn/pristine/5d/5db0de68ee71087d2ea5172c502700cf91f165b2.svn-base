/**
 * copyright (C) 2011
 * Claudio Kopper <claudio.kopper@nikhef.nl>
 * $Id$
 *
 * @file ConvertToGST.h
 * @version $Revision$
 * @date $Date$
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

#ifndef CONVERTTOGST_H_INCLUDED
#define CONVERTTOGST_H_INCLUDED


#include "genie-icetray/I3GENIEResultDict.h"

#include "EVGCore/EventRecord.h"

namespace I3GENIE_Helper
{
    /**
     * Convert a GENIE event into a GST structure.
     * Heavily based on GENIE code (gNtpConv.cxx)
     */
    bool ConvertToGST(const genie::EventRecord &event, int32_t iev, I3GENIEResultDict &gstMap);
    
};

#endif //CONVERTTOGST_H_INCLUDED
