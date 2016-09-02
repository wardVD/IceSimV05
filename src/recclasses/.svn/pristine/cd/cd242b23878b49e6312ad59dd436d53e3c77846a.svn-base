/**
 * $Id$
 * (c) 2011 the IceCube Collaboration
 *
 * @version $Revision: 71590 $
 * @date $Date: 2011-02-24 11:58:18 -0500 (Thu, 24 Feb 2011) $
 * @author Jakob van Santen <vansanten@wisc.edu>
 */

#ifndef RECCLASSES_I3COMPACTKEYLIST_H_INCLUDED
#define RECCLASSES_I3COMPACTKEYLIST_H_INCLUDED

#include <icetray/I3FrameObject.h>
#include <icetray/OMKey.h>
#include <icetray/serialization.h>
#include <dataclasses/I3Vector.h>

/**
 *  @brief Specialization of I3Vector<OMKey> with extremely compact serialization.
 *
 *  This is useful for efficiently storing a list of keys whose average size
 *  is expected to be near 1, e.g. a list of DOMs with calibration failures.
 */
class I3CompactKeyList : public I3Vector<OMKey> {
private:
	friend class boost::serialization::access;
	
	template <class Archive> void save(Archive & ar, unsigned version) const;
	template <class Archive> void load(Archive & ar, unsigned version);
	
	BOOST_SERIALIZATION_SPLIT_MEMBER();
};

static const unsigned i3compactkeylist_version_ = 0;
BOOST_CLASS_VERSION(I3CompactKeyList, i3compactkeylist_version_);

I3_POINTER_TYPEDEFS(I3CompactKeyList);

#endif /* WAVEREFORM_I3COMPACTKEYLIST_H_INCLUDED */
