/**
 * Copyright (C) 2013
 * Martin Wolf <martin.wolf@fysik.su.se>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @version $Id$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@fysik.su.se>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
#ifndef RECCLASSES_I3STCONFIGURATION_H_INCLUDED
#define RECCLASSES_I3STCONFIGURATION_H_INCLUDED

#include "recclasses/OMKeyLinkSet.h"

#include "icetray/OMKey.h"
#include "icetray/I3FrameObject.h"
#include "icetray/I3PointerTypedefs.h"

#include <ostream>
#include <string>
#include <vector>

#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>

static const unsigned i3stconfiguration_version_ = 0;

/**
 * @brief Base class for all ST algorithm-specific configurations (see project STTools).
 *
 * It holds an I3Vector<OMKey> for a list of OMs for which the
 * particular ST configuration applies to. In this way, the entire detector
 * can be easily split into several sub-detectors.
 */
class I3STConfiguration
  : public I3FrameObject
{
  public:
    /** The default constructor, needed by boost::serialization.
     */
    I3STConfiguration()
    {}

    /** The normal constructor.
     *  It allows to set all configuration options.
     */
    I3STConfiguration(
        const std::string          &name,
        const I3VectorOMKeyLinkSet &omKeyLinkSets
    )
      : name_(name),
        omKeyLinkSets_(omKeyLinkSets)
    {}

    I3STConfiguration(const I3STConfiguration &rhs)
      : name_(rhs.GetName()),
        omKeyLinkSets_(rhs.GetOMKeyLinkSets())
    {}

    virtual
    ~I3STConfiguration();

    std::string GetName() const {
        return name_;
    }

    const I3VectorOMKeyLinkSet& GetOMKeyLinkSets() const {
        return omKeyLinkSets_;
    }
    I3VectorOMKeyLinkSet& GetOMKeyLinkSets() {
        return omKeyLinkSets_;
    }

    void SetName(const std::string &s) {
        name_ = s;
    }

    void SetOMKeyLinkSets(const I3VectorOMKeyLinkSet &omKeyLinkSets)
    {
        omKeyLinkSets_ = omKeyLinkSets;
    }

    /// Equality operator, useful for testing.
    bool
    operator==(const I3STConfiguration &rhs) const {
        return
            name_          == rhs.GetName() &&
            omKeyLinkSets_ == rhs.GetOMKeyLinkSets()
        ;
    }

    /** Returns a std::string with all the settings pretty printed. Each line
     *  in the string will have nLeadingWS white space characters at the
     *  beginning.
     */
    virtual
    std::string
    GetPrettySettingsStr(unsigned int nLeadingWS=0) const;

    /** The GetSTVolumetime method should be implemented by the derived class
     *  and should return the ST volume time (i.e. ST volume times ST time) that
     *  is spaned up by this ST configuration. This value can be used to compare
     *  two ST configurations in terms of their effectiveness.
     *
     *  @returns 0
     */
    virtual
    double
    GetSTVolumetime() const {
        return 0;
    };

  protected:
    /** The name of the ST configuration. For example, this name could name the
     *  "sub-detector" for which this ST configuration applies.
     *  @note This name is not treated as an unique identifier!
     */
    std::string name_;

    /** The list of OM links (i.e. OM pairs), to which this ST configuration
     *  should get applied to.
     */
    I3VectorOMKeyLinkSet omKeyLinkSets_;

  private:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive &ar, unsigned version);

    SET_LOGGER("I3STConfiguration");
};

std::ostream& operator<<(std::ostream& os, const I3STConfiguration &rhs);

I3_POINTER_TYPEDEFS(I3STConfiguration);
BOOST_CLASS_VERSION(I3STConfiguration, i3stconfiguration_version_);

typedef I3Vector<I3STConfiguration> I3VectorSTConfiguration;
I3_POINTER_TYPEDEFS(I3VectorSTConfiguration);

#endif// ! STTOOLS_I3STCONFIGURATION_H_INCLUDED
