#ifndef MP_DIPLOPIA_UTILS_H_INLCUDED
#define MP_DIPLOPIA_UTILS_H_INLCUDED

#include <icetray/I3TrayHeaders.h>
#include <icetray/I3Logging.h>
#include <simclasses/I3MCPE.h>
#include "dataclasses/physics/I3MCTree.h"
#include "dataclasses/physics/I3MCTreeUtils.h"
#include "simclasses/I3MMCTrack.h"
#include "dataclasses/I3Double.h"
#include "dataclasses/I3String.h"

#include <icetray/I3Frame.h>
#include <icetray/open.h>
#include <fstream>
#include <boost/iostreams/filtering_stream.hpp>

#include <cmath>
#include <assert.h>

/**
 * copyright  (C) 2006
 * The IceCube Collaboration
 *
 * @date $Date:$
 * @author Juan Carlos Diaz Velez
 *                                                                       
 * @brief Utilities for merging MCTrees MCInfo and MCHitSeriesMaps in separate 
 *	events to produce coincident in-ice events.
 *
 */

namespace DiplopiaUtils
{

I3MCTree::optional_value
GetEarliestInIce(const I3MCTree& t);

I3MCTree::optional_value
GetLatestInIce(const I3MCTree& t);


/**
  * MergeHits - Takes two I3MCPESeriesMaps and merges their hits.
  * The times of the hits in the second map are scrambled around
  * the time of the one in the first map with within timeWindow_
  *
  * @param map1 original map to which we will add the new hits
  * @param map2 second map hit times will be set within window of first map
  * @param offsetTime time difference between first hit in map2 relative to map1
  */
  void MergeHits(
				  I3MCPESeriesMap& map1, 
				  const I3MCPESeriesMap& map2,
				  float offsetTime);



/**
  * MergeMMCInfo - Adds the contents of the second MMCInfoList to the first
  *
  * @param dest - list we will add MMCInfo to
  * @param src  - list we will copy MMCInfo from
  */
  void MergeMMCInfo(I3MMCTrackListPtr dest, I3MMCTrackListConstPtr src, double timeOffset); 


/**
  * MergePrimaries - Add I3MCTree src at root level of tree dest
  *
  * @param dest - tree to merge into
  * @param src - tree to merge
  * @param timeOffset -- value to offset particle times in src 
  */
  void MergePrimaries(I3MCTreePtr dest, I3MCTreeConstPtr src, double timeOffset);

  void CopyWeights(I3MapStringDouble& dest, const I3MapStringDouble src);


/**
  * CompareHits - compare times of two hits
  *
  * @param hit1 
  * @param hit2
  * @return true if hit1 is earlier than hit2
  */
  bool CompareHits(const I3MCPE& hit1,const I3MCPE& hit2);



/**
  * SortHits - iterate through hitmap and sort hitseries
  *
  * @param hitmap 
  */
  void SortHits(I3MCPESeriesMap& hitmap);




/**
  * Add time offset to MCTree
  * @param ctree  - tree to modify
  * @param offsetTime - time offset
  */
  void OffsetTime(I3MCTreePtr ctree,double offsetTime);

  I3Frame MergeFrames(I3Frame frame1, I3Frame frame2, I3Map<std::string,std::string> names, float delta_t);

  void MergeFrames(I3FramePtr frame1, const I3FramePtr frame2, const I3Map<std::string,std::string> names, float delta_t);



 /*
  * Temporarily implementing own reader since I3SequentialFile doesn't
  * have a public interface.
  */

  class FileReader
  {
  public:
  
    FileReader() { };
    FileReader(const FileReader&);
    FileReader(const std::string& filename);
    ~FileReader();

    int open_file(const std::string& filename);
    void close();

    bool more();
    void push(I3FramePtr f);
    I3FramePtr pop_frame(I3Frame::Stream s);

  private:
  
    boost::iostreams::filtering_istream ifs_;
    I3Frame cache_;
    std::string path_;
    unsigned int nframe_;

  };

};


#endif
