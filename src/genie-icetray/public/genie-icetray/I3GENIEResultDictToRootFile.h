/**
 * copyright (C) 2011
 * Claudio Kopper <claudio.kopper@nikhef.nl>
 * $Id: I3GENIEResultDictToRootFile.h 103608 2013-05-03 16:54:54Z claudio.kopper $
 *
 * @file I3GENIEResultDictToRootFile.h
 * @version $Revision$
 * @date $Date: 2013-05-03 12:54:54 -0400 (Fri, 03 May 2013) $
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

/**
 *
 */
class I3GENIEResultDictToRootFile : public I3ConditionalModule
{
public:
    /**
     * Builds an instance of this class
     */
    I3GENIEResultDictToRootFile(const I3Context& ctx);

    /**
     * Destroys an instance of this class
     */
    ~I3GENIEResultDictToRootFile();

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
    I3GENIEResultDictToRootFile();
    I3GENIEResultDictToRootFile(const I3GENIEResultDictToRootFile&);
    I3GENIEResultDictToRootFile& operator=(const I3GENIEResultDictToRootFile&);


    SET_LOGGER("I3GENIEResultDictToRootFile");
};

#endif //I3GENIERESULTDICTTOMCTREE_H_INCLUDED
