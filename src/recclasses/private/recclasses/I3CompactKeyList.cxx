/**
 * (c) 2011 the IceCube Collaboration
 *
 * $Id:$
 * @version $Revision: 123102 $
 * @date $Date: 2014-09-03 18:09:07 -0400 (Wed, 03 Sep 2014) $
 * @author Jakob van Santen <vansanten@wisc.edu>
 */

#include "recclasses/I3CompactKeyList.h"
#include <boost/serialization/binary_object.hpp>

namespace {

/**
 * @brief Compact storage for an OMKey.
 *
 * NB: While the internal layout of bitfields is implementation-defined,
 * (see ISO 14882 [http://www-d0.fnal.gov/~dladams/cxx_standard.pdf] p. 159)
 * GCC >= 3.4.6 obeys the following conventions under __attribute((packed)):
 *
 * - on little-endian platforms, fields are laid out in native byte order
 *   starting from the least significant bit of the parent struct
 * 
 * - on big-endian platforms, fields are laid out starting from the most
 *   significant bit of the parent struct
 *
 * To properly interpet the binary representation of a bitfield from a little-
 * endian platform on a big-endian one (and vice versa), it is necessary not
 * only to swap the order of the bytes in memory, but also to reverse the
 * order of the fields.
 */
struct CompactOMKey {

#ifdef BOOST_PORTABLE_BINARY_ARCHIVE_BIG_ENDIAN
	uint16_t stop : 1;
	uint16_t slop : 2;
	uint16_t om : 6;
	uint16_t string : 7;
#else
	uint16_t string : 7;
	uint16_t om : 6;
	uint16_t slop : 2;
	uint16_t stop : 1;
#endif

	template <class Archive> 
	void serialize(Archive &ar, unsigned version) 
	{ 
		ar & make_nvp("Bytes", 
		    boost::serialization::make_binary_object( 
		    this, 2));       
	}
} __attribute((packed));
	
}

template <class Archive>
void
I3CompactKeyList::save(Archive & ar, unsigned version) const
{
	CompactOMKey key;
	key.slop = 0;
	
	ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
	
	if (this->size() == 0) {
		key.string = 0;
		key.om = 0;
		key.stop = true;
		ar & make_nvp("Blob", key);
		return;
	}
	
	const_iterator it = this->begin();
	const_iterator end = this->end()-1;
	for ( ; it != end; ++it ) {
		key.string = it->GetString();
		key.om = it->GetOM();
		key.stop = false;
		ar & make_nvp("Blob", key);
	}
	
	assert(it == end);
	
	key.string = it->GetString();
	key.om = it->GetOM();
	key.stop = true;
	ar & make_nvp("Blob", key);
}

template <class Archive>
void
I3CompactKeyList::load(Archive & ar, unsigned version)
{
	CompactOMKey key;
	
	ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
	ar & make_nvp("Blob", key);
	
	if (key.string == 0 && key.om == 0) /* Sentinel for an empty list. */
		return;
	
	this->push_back(OMKey(key.string, key.om));
	
	while (!key.stop) {
		ar & make_nvp("Blob", key);
		this->push_back(OMKey(key.string, key.om));
	}
}

I3_SPLIT_SERIALIZABLE(I3CompactKeyList);
