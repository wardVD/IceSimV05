/**
 * copyright (C) 2011
 * Claudio Kopper <claudio.kopper@nikhef.nl>
 * $Id: I3GENIEResultDictToMCTree.h 137708 2015-09-17 20:30:37Z melanie.day $
 *
 * @file I3GENIEResultDictToMCTree.h
 * @version $Revision$
 * @date $Date: 2015-09-17 15:30:37 -0500 (Thu, 17 Sep 2015) $
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

#ifndef I3GENIERESULTDICTTOMCTREE_H_INCLUDED
#define I3GENIERESULTDICTTOMCTREE_H_INCLUDED

#include "icetray/I3Module.h"
#include "icetray/I3ConditionalModule.h"

class I3GENIEResultDictToMCTree : public I3ConditionalModule
{
public:
    /**
     * Builds an instance of this class
     */
    I3GENIEResultDictToMCTree(const I3Context& ctx);
    
    /**
     * Destroys an instance of this class
     */
    ~I3GENIEResultDictToMCTree();

    /**
     * This module takes configuration parameters
     */
    virtual void Configure();

#ifdef IS_Q_FRAME_ENABLED
    /**
     * The module needs to process DAQ frames
     */
    virtual void DAQ(I3FramePtr frame);
#else
    /**
     * The module needs to process Physics frames
     */
    virtual void Physics(I3FramePtr frame);
#endif

private:
    // parameters

    /// Parameter: Name of the input I3GENIEResultDict frame object
    std::string resultDictName_;

    /// Parameter: Name of the output I3MCTree frame object
    std::string mcTreeName_;

    /// Parameter: Name of the output I3MCWeightDict frame object
    std::string weightDictName_;


private:
    // default, assignment, and copy constructor declared private
    I3GENIEResultDictToMCTree();
    I3GENIEResultDictToMCTree(const I3GENIEResultDictToMCTree&);
    I3GENIEResultDictToMCTree& operator=(const I3GENIEResultDictToMCTree&);


    SET_LOGGER("I3GENIEResultDictToMCTree");
};

#endif //I3GENIERESULTDICTTOMCTREE_H_INCLUDED
