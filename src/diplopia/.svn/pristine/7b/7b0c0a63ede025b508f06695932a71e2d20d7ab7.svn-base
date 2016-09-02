#include "diplopia/DiplopiaUtils.h"
#include "dataclasses/I3Time.h"
#include <algorithm>

#include <icetray/I3Int.h>
#include <dataclasses/external/CompareFloatingPoint.h>
#include <dataclasses/physics/I3MCTreePhysicsLibrary.hh>
#include "dataclasses/physics/I3ParticleID.h"
#include "dataclasses/physics/I3MCTreeUtils.h"

#include <boost/function.hpp>

using CompareFloatingPoint::Compare;
using I3MCTreeUtils::GetBestFilter;
using I3MCTreeUtils::GetFilter;
using I3MCTreeUtils::GetBestFilterPtr;

using namespace std;

namespace{


  bool Earliest(const I3Particle& a, const I3Particle& b){
    float a_time = a.GetTime();
    float b_time = b.GetTime();
    if (std::isnan(a_time) && std::isnan(b_time))
        return 0;
    else if (std::isnan(a_time))
        return -1;
    else if (std::isnan(b_time))
        return 1;
    else
        return a_time < b_time;
  }

  bool Latest(const I3Particle& a, const I3Particle& b){
    float a_time = a.GetTime();
    float b_time = b.GetTime();
    if (std::isnan(a_time) && std::isnan(b_time))
        return 0;
    else if (std::isnan(a_time))
        return -1;
    else if (std::isnan(b_time))
        return 1;
    else
        return a_time > b_time;
  }

  bool IsInIce(const I3Particle& p){return p.GetLocationType() == I3Particle::InIce; }

}

I3MCTree::optional_value
DiplopiaUtils::GetEarliestInIce(const I3MCTree& t){
  I3MCTree::optional_value rval = I3MCTreeUtils::GetBestFilter(t, IsInIce, Earliest);
  return rval;
}

I3MCTree::optional_value
DiplopiaUtils::GetLatestInIce(const I3MCTree& t){
  I3MCTree::optional_value rval = I3MCTreeUtils::GetBestFilter(t, IsInIce, Latest);
  return rval;
}


void 
DiplopiaUtils::MergeMMCInfo(I3MMCTrackListPtr dest, I3MMCTrackListConstPtr src, double timeOffset) 
{
   I3MMCTrackListPtr mmc_list_ptr=I3MMCTrackListPtr(new I3MMCTrackList(*src));
   I3MMCTrackList::iterator mmciter = mmc_list_ptr->begin();
   for (;mmciter!=mmc_list_ptr->end();mmciter++) 
   { 
       I3Particle new_particle(mmciter->GetI3Particle());
       double t_0=new_particle.GetTime();
       new_particle.SetTime(t_0+timeOffset);
       mmciter->SetParticle(new_particle); 
  
       mmciter->ti += timeOffset ; 
       mmciter->tc += timeOffset ; 
       mmciter->tf += timeOffset ; 
       dest->push_back(*mmciter); 
   }
}

void 
DiplopiaUtils::MergePrimaries(I3MCTreePtr dest, I3MCTreeConstPtr src, double timeOffset) 
{
    // Make a copy of tree set new time of I3Particle 
    I3MCTreePtr ctree(new I3MCTree(*src));
    DiplopiaUtils::OffsetTime(ctree,timeOffset);

    // merge tree into main tree 
    dest->merge(*ctree);
}


bool 
DiplopiaUtils::CompareHits(const I3MCPE& hit1,const I3MCPE& hit2) 
{ 
        return hit1.time < hit2.time;
}

void 
DiplopiaUtils::SortHits(I3MCPESeriesMap& hitmap) 
{ 
    I3MCPESeriesMap::iterator om_iter;
    for(om_iter = hitmap.begin(); om_iter != hitmap.end(); om_iter++)
    { 
        sort(om_iter->second.begin(),om_iter->second.end(), CompareHits );
    } 
}





void 
DiplopiaUtils::CopyWeights(I3MapStringDouble& dest, const I3MapStringDouble src) 
{
    I3MapStringDouble::const_iterator weight_iter=src.begin(); 
    for (; weight_iter!=src.end(); weight_iter++) 
    { 
        dest[weight_iter->first] = weight_iter->second; 
    } 
}


void
DiplopiaUtils::OffsetTime(I3MCTreePtr ctree, double offsetTime) 
{ 
    I3MCTree::pre_order_iterator iter = ctree->begin();
    for (; iter != ctree->end(); iter++ ) 
    { 
        iter->SetTime(iter->GetTime() + offsetTime); 
    }
}

I3Frame DiplopiaUtils::MergeFrames(I3Frame frame1, I3Frame frame2, I3Map<std::string,std::string> names, float delta_t)
{
     MergeFrames(I3FramePtr(&frame1), I3FramePtr(&frame2), names, delta_t);
     return frame1;
}

void DiplopiaUtils::MergeFrames(I3FramePtr frame1, const I3FramePtr frame2, const I3Map<std::string,std::string> names, float delta_t)
{

  // Cache frame into vector for event merging

  double time1      = NAN;
  double time2      = NAN;

  // Iterate over cached frames and merge objects
  double offsetTime1,offsetTime2; 
  vector<string>::iterator name_iter;
  string mcTreeName    = names.at("MCTreeName");
  string mmcTrackName  = names.at("MMCTrackName");

  I3MCTreeConstPtr tree1 = I3MCTreeUtils::Get(*frame1,mcTreeName);
  I3MCTreeConstPtr tree2 = I3MCTreeUtils::Get(*frame2,mcTreeName);
  if (!(tree1 && tree2)) 
       log_fatal("Could not find I3MCTree (or I3MCList) '%s'", mcTreeName.c_str());

  // get earliest time for each event 
  time1 = DiplopiaUtils::GetEarliestInIce(*tree1)->GetTime();
  time2 = DiplopiaUtils::GetEarliestInIce(*tree2)->GetTime();
  if ( std::isnan(time1) || std::isnan(time2) ) {
      log_fatal("Unable to find a valid time in given events");
  }

  // this gets the internal time in event 2 to zero (-time2) and adds the delta_t 
  // for merging (no matter if its negative or positive)
  offsetTime1= -time1;
  offsetTime2= -time2+delta_t;

  // Initialize I3MCTree 
  I3MCTreePtr  mergedMCTree = I3MCTreePtr(new I3MCTree());

  // Initialize MMCInfoTree and create the first MMCList as an empty list
  I3MMCTrackListPtr mergedMMCTrack = I3MMCTrackListPtr(new I3MMCTrackList());

  // Merge trees from each of the cached frames
  log_trace("Merging '%s' I3MCTree ", mcTreeName.c_str());

  log_trace("src_id = %u, dest_id =%u", 
                    I3MCTreeUtils::GetPrimaries(*tree1)[0].GetMinorID(), 
                    I3MCTreeUtils::GetPrimaries(*tree2)[0].GetMinorID()); 

  // Make a copy of tree set new time of I3Particle 
  I3MCTreePtr ctree1(new I3MCTree(*tree1));
  I3MCTreePtr ctree2(new I3MCTree(*tree2));

  // offset time of main tree 
  OffsetTime(ctree1,offsetTime1);

  // merge into main tree 
  MergePrimaries(ctree1,ctree2, offsetTime2);

  // Merge I3MMCTrackLists 
  I3MMCTrackListConstPtr mmcList1 = frame1->Get<I3MMCTrackListConstPtr>(mmcTrackName);
  I3MMCTrackListConstPtr mmcList2 = frame2->Get<I3MMCTrackListConstPtr>(mmcTrackName);
  
  // only merge mmcList1 if it contains an actual mmclist
  // this is not the case for nue e simulation
  if(mmcList1)
  {
    MergeMMCInfo(mergedMMCTrack, mmcList1, offsetTime1);
  }
 
  if(mmcList2)
  {
     MergeMMCInfo(mergedMMCTrack, mmcList2, offsetTime2);
     if(frame1->Has(mmcTrackName)) { 
	     frame1->Delete(mmcTrackName); 
     } 
     frame1->Put(mmcTrackName,mergedMMCTrack); 
  } else {
    log_debug("Could not find I3MMCTrackList '%s'", mmcTrackName.c_str());
  }

  // merge the I3MCPESeriesMaps if present
  if (names.count("I3MCPESeriesName")) {
     string hitSeriesName = names.at("I3MCPESeriesName");
     I3MCPESeriesMapConstPtr hitmap1 = frame1->Get<I3MCPESeriesMapConstPtr>(hitSeriesName); 
     I3MCPESeriesMapConstPtr hitmap2 = frame2->Get<I3MCPESeriesMapConstPtr>(hitSeriesName); 

     if ( ( hitmap1 && hitmap2 ) ) { 
          log_trace("merging hiseries maps"); 
          I3MCPESeriesMapPtr  mergedHits  = I3MCPESeriesMapPtr(new I3MCPESeriesMap()); 
          DiplopiaUtils::MergeHits(*mergedHits, *hitmap1,offsetTime1); 
          DiplopiaUtils::MergeHits(*mergedHits, *hitmap2,offsetTime2);
          frame1->Delete(hitSeriesName); 
          frame1->Put(hitSeriesName,mergedHits); 
     }
  }


  // merge weights
  I3MapStringDoubleConstPtr weights2 = frame2->Get<I3MapStringDoubleConstPtr>(names.at("Weights2"));
  I3MapStringDouble mergedWeights;
 
  if (frame1->Has(names.at("Weights2")) ) { // check if frame1 already has this weightmap
     I3MapStringDoubleConstPtr weights1 = frame1->Get<I3MapStringDoubleConstPtr>(names.at("Weights2"));
     CopyWeights(mergedWeights,*weights1);
     if (weights2 && weights2->count("Weight") && mergedWeights.count("Weights"))
        mergedWeights["Weight"] *= weights2->at("Weight");
  } else if (weights2) {
     frame1->Put(names.at("Weights2"),weights2);
  }


  // Now replace object in frame
  frame1->Delete(mcTreeName);
  frame1->Put(mcTreeName,ctree1);
 
  return;
}

void
DiplopiaUtils::MergeHits( 
                I3MCPESeriesMap& map1, 
                const I3MCPESeriesMap& map2,
                float offsetTime)
{
  I3MCPESeriesMap::const_iterator om_iter;
  I3MCPESeries::const_iterator hit_iter;

  // Add the coinc hits
  for(om_iter = map2.begin(); om_iter != map2.end(); om_iter++){
    //if no MC hit, just set the hit series to be the coinc hitseries
    if( map1.find(om_iter->first) == map1.end() ){
      I3MCPESeries hitSeries;
      for(
              hit_iter = om_iter->second.begin(); 
              hit_iter != om_iter->second.end(); 
              hit_iter++)
      {
            I3MCPE newhit(*hit_iter);
            newhit.time = hit_iter->time+offsetTime;
            hitSeries.push_back(newhit);
      }
      map1[om_iter->first] = hitSeries;
    }
    else{
        log_trace("adding hits from coincident event into hitseries");
      I3MCPESeries newSeries = om_iter->second;
      for( // iterate over coincident event hist and set new time
              hit_iter = om_iter->second.begin(); 
              hit_iter != om_iter->second.end(); 
              hit_iter++)
      {
            I3MCPE newhit(*hit_iter);
            newhit.time = hit_iter->time+offsetTime;
            map1[om_iter->first].push_back(newhit);
      }
    }
  }
  // make sure hits are sorted by time
  DiplopiaUtils::SortHits(map1); 
}


