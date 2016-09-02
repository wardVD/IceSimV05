#include "recclasses/I3CompactKeyList.h"

#include <I3Test.h>
#include <dataclasses/I3Vector.h>
#include <icetray/I3Frame.h>
#include <icetray/portable_binary_archive.hpp>

TEST_GROUP(I3CompactKeyListTest)

TEST(SerializeEmptyList)
{
	I3CompactKeyList in_list, out_list;
	
	std::ostringstream oarchive_stream;
	/* This is the archive type instantiated by I3_SERIALIZABLE */
	boost::archive::portable_binary_oarchive oarchive(oarchive_stream);
	oarchive << in_list;
	
	ENSURE_EQUAL( oarchive_stream.str().size(), unsigned(12 + 2 + 2),
	    "Empty list takes up only 2 extra bytes.");
	
	std::istringstream iarchive_stream(oarchive_stream.str());
	boost::archive::portable_binary_iarchive iarchive(iarchive_stream);
	iarchive >> out_list;
	
	ENSURE_EQUAL(out_list.size(), 0u, "Deserialized list is empty.");
}

TEST(SerializeSingleElement)
{
	I3CompactKeyList in_list, out_list;
	
	in_list.push_back(OMKey(42,7));
	
	std::ostringstream oarchive_stream;
	/* This is the archive type instantiated by I3_SERIALIZABLE */
	boost::archive::portable_binary_oarchive oarchive(oarchive_stream);
	oarchive << in_list;
	
	ENSURE_EQUAL( oarchive_stream.str().size(), unsigned(12 + 2 + 2),
	    "Single-element list takes up 2 extra bytes.");
	
	std::istringstream iarchive_stream(oarchive_stream.str());
	boost::archive::portable_binary_iarchive iarchive(iarchive_stream);
	iarchive >> out_list;
	
	ENSURE_EQUAL(out_list.size(), 1u, "Deserialized list has one element.");
	ENSURE_EQUAL(in_list.front().GetString(), out_list.front().GetString());
	ENSURE_EQUAL(in_list.front().GetOM(), out_list.front().GetOM());
}

TEST(SerializeLots)
{
	I3CompactKeyList in_list, out_list;
	
	in_list.push_back(OMKey(42,7));
	in_list.push_back(OMKey(23,36));
	in_list.push_back(OMKey(18,3));
	in_list.push_back(OMKey(86,60));
	in_list.push_back(OMKey(34,17));
	
	std::ostringstream oarchive_stream;
	/* This is the archive type instantiated by I3_SERIALIZABLE */
	boost::archive::portable_binary_oarchive oarchive(oarchive_stream);
	oarchive << in_list;
	
	ENSURE_EQUAL( oarchive_stream.str().size(), unsigned(12 + 2 + 5*2),
	    "5-element list takes up 10 extra bytes.");
	
	std::istringstream iarchive_stream(oarchive_stream.str());
	boost::archive::portable_binary_iarchive iarchive(iarchive_stream);
	iarchive >> out_list;
	
	ENSURE_EQUAL(out_list.size(), in_list.size(), "Deserialized list has 5 elements.");
	
	I3CompactKeyList::const_iterator lit = in_list.begin();
	I3CompactKeyList::const_iterator rit = out_list.begin();
	for ( ; lit != in_list.end(); lit++, rit++) {
		ENSURE_EQUAL(lit->GetString(), rit->GetString());
		ENSURE_EQUAL(lit->GetOM(), rit->GetOM());
	}

}

TEST(Convertibility)
{
	I3CompactKeyListPtr in_list(new I3CompactKeyList());
	I3Frame frame;
	I3Frame deframe;
	
	in_list->push_back(OMKey(42,7));
	frame.Put("foo", in_list);
	
	I3VectorOMKeyConstPtr vect = frame.Get<I3VectorOMKeyConstPtr>("foo");
	
	ENSURE_EQUAL(in_list->front(), vect->front());
	
	/* Now, check that everything survives serialization. */
	std::ostringstream oarchive_stream;
	std::ostream &ostream = oarchive_stream;
	frame.save(ostream);
	
	std::istringstream iarchive_stream(oarchive_stream.str());
	std::istream &istream = iarchive_stream;
	deframe.load(istream);
	
	vect = deframe.Get<I3VectorOMKeyConstPtr>("foo");
	
	ENSURE_EQUAL(in_list->front(), vect->front());
}

