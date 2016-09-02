#ifndef I3DISTANCE_MAP_H
#define I3DISTANCE_MAP_H

#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/I3Position.h>
#include <boost/foreach.hpp>
#include <limits>
#include <cmath>

/**
 * @author Kevin J Meagher
 * @date March 2007
 *
 * @brief Creates a very large vector that contains a map of the distance squared between each OM.
 *
 * This class trades memory for speed by creating ~300M vector. As distances are requested 
 * they are added to the vector. This generated an approximate 5 fold increace in speed.
 */

class I3DistanceMap
{
 public:

  /**
   * constructor
   * @param geo the geomerty to map
   *
   * Gets the geometry to map and allocates the vector
   */
  inline I3DistanceMap(const I3OMGeoMap &geo):
    geo_(geo),
    min_string_(std::numeric_limits<int>::max()),
    min_om_(std::numeric_limits<unsigned int>::max())
  {
    int max_string=std::numeric_limits<int>::min();
    unsigned int max_om=std::numeric_limits<unsigned int>::min();

    //find min and max values for each om and string number
    /*for_each*/BOOST_FOREACH(I3OMGeoMap::const_reference itr, geo)
      {
	if ( itr.first.GetString() < min_string_)
	  {
	    min_string_=itr.first.GetString();
	  }
	if ( itr.first.GetString() > max_string)
	  {
	    max_string=itr.first.GetString();
	  }
	if ( itr.first.GetOM() < min_om_)
	  {
	    min_om_=itr.first.GetOM();
	  }
	if ( itr.first.GetOM() > max_om)
	  {
	    max_om=itr.first.GetOM();
	  }
      }
  
    //find range wor each om and string number
    int string_range=max_string-min_string_+1;
    om_range_=max_om-min_om_+1;

    size_=string_range*om_range_;

    log_info("Creating new I3DistanceMap; min_string_: %d, min_om_: %u, om_range_: %u, size_: %u",min_string_,min_om_,om_range_,size_);

    ///allocate map as all nans 
    map_.resize(size_*size_,NAN);
  }

  /**
   * @return the geometry being used by the distance map
   */
  inline const I3OMGeoMap& getGeo() const
  {
    return geo_;
  }
 
  /**
   * @param key1 
   * @param key2
   * @return the distance squared between key1 and key2
   *
   * Looks up distance squared in map if it has been calculated return it 
   * else calculate it
   */
  inline double getDistance(const OMKey &key1, const OMKey &key2)
  {
    double dist=map_[getIndex(key1,key2)];
    if (std::isnan(dist))
      {
	dist=setDistance(key1,key2);
      }
    log_trace("key pair OMKey(%2d,%2u),OMKey(%2d,%2u) got distance squared : %7f",key1.GetString(),key1.GetOM(),key2.GetString(),key2.GetOM(),dist);
    return dist;
  }

private:
  ///geometry to map
  const I3OMGeoMap &geo_;

  ///vector to hold distances squared
  std::vector <double> map_;

  ///"size" of hash, must be larger that the greatest value of getHash
  int32_t size_;

  ///lowest string number
  int min_string_;

  ///losest om number
  unsigned int min_om_;

  ///range of om number
  unsigned int om_range_;
 
  /**
   * @param key 
   * @return A unique hash for key
   */
  inline int32_t getHash(const OMKey &key)const
  {
    int32_t hash=(key.GetString()-min_string_)*om_range_+key.GetOM()-min_om_;
    if ( (hash < 0 ) || (hash > size_))
      {
	log_fatal("OMKey(%d,%u) is out of range: Check Geometry. MinString=%d, MaxString=%d, MinOM=%u, MaxOM=%u",
		  key.GetString(),key.GetOM(),min_string_,min_string_+size_/om_range_-1,min_om_,min_om_+om_range_-1);
      }
    return hash;
  }

  /**
   * @param key1 
   * @param key2
   * @returns the a index for a pair of keys
   */
  inline int32_t getIndex(const OMKey &key1, const OMKey &key2)const
  {
    int32_t h1=getHash(key1);
    int32_t h2=getHash(key2);
    if (h1<h2) 
      {
	return h1*size_+h2;
      }
    else
      {
	return h2*size_+h1;
      }
  }

  /**
   * @param key1
   * @param key2
   * @return the distance squared between key1 and key2
   */
  inline double setDistance(const OMKey &key1, const OMKey &key2)
  {
    I3Position pos1=geo_.find(key1)->second.position;
    I3Position pos2=geo_.find(key2)->second.position;

    double dx=pos1.GetX() - pos2.GetX();
    double dy=pos1.GetY() - pos2.GetY();
    double dz=pos1.GetZ() - pos2.GetZ();

    double distance = dx*dx + dy*dy +dz*dz;

    map_[getIndex(key1,key2)]=distance;

    return distance;
  }

  SET_LOGGER("I3DistanceMap");
};


#endif
