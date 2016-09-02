#include "diplopia/MPHitFilter.h"
#include <simclasses/I3MCPE.h>
#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/physics/I3MCTreeUtils.h"
#include "dataclasses/geometry/I3OMGeo.h"


using namespace std;

I3_MODULE(MPHitFilter);


/**
 * @brief A filtering module which selects events which 
 * have MCPEs >= threshold
 */

MPHitFilter::MPHitFilter(const I3Context& context) : 
	I3Module(context),
	hitSeriesMapName_("I3MCPESeriesMap"),
	threshold_(1),
	weightSum_(0.0),
	weightPartSum_(0.0),
	weightSumName_("WeightSum"),
	weightPartSumName_("WeightPartSum"),
	timeScaleSingle_(NAN),
	nprimaries_(1),
	eventCount_(1),
	keptEvents_(0),
	rejectedEvents_(0),
	removedBranches_(0),
	weightprod_(1.0),
	pruneTree_(true),
	removeBackgroundOnly_(true),
	filter_(true),
	mcTreeName_("I3MCTree"),
	weightMap_("CorsikaWeightMap") 
{ 
	AddParameter("I3MCPESeriesMapName",
		   "Name of HitSeriesMap object to scan",
		   hitSeriesMapName_); 
	AddParameter("HitOMThreshold","Minimum number of hit OMs to pass filter ",threshold_); 
	AddParameter("WeightMap", "map of weights to merge", weightMap_); 
	AddParameter("WeightSumName", "Name to store weightsum in summary service", weightSumName_); 
	AddParameter("WeightPartSumName", "Name to store weightpartsum in summary service", weightPartSumName_); 
	AddParameter("NumberOfPrimaries","Number of primary particles to merge", nprimaries_); 
	AddParameter("PruneTree", "Cut dead MCTree branches that don't produce MCPEs", pruneTree_); 
	AddParameter("RemoveBackgroundOnly", "Remove events that don't contain primary", removeBackgroundOnly_); 
	AddParameter("MCTreeName", "Name of I3MCTree object in frame", mcTreeName_); 
	AddParameter("Filter", "Remove events that don't pass threshold", filter_); 
	AddOutBox("OutBox"); 
}

void 
MPHitFilter::Configure() 
{ 
	GetParameter("I3MCPESeriesMapName", hitSeriesMapName_); 
	GetParameter("HitOMThreshold",threshold_); 
	GetParameter("WeightMap", weightMap_);
       	GetParameter("WeightSumName", weightSumName_); 
	GetParameter("WeightPartSumName", weightPartSumName_); 
	GetParameter("NumberOfPrimaries", nprimaries_);
	GetParameter("PruneTree", pruneTree_);
	GetParameter("RemoveBackgroundOnly", removeBackgroundOnly_);
	GetParameter("MCTreeName",  mcTreeName_); 
	GetParameter("Filter", filter_); 
}

void 
MPHitFilter::DAQ(I3FramePtr frame)
{ 
    unsigned hitcount= 0;
    map<I3ParticleID,bool> primarymap;
    I3MCTree mctree = frame->Get<I3MCTree>(mcTreeName_);
    I3MCTreePtr mctree_ptr = I3MCTreePtr(new I3MCTree(mctree));
    I3Particle* temp_primary;
    I3MapStringDoubleConstPtr coincWeights;

    // Clean hits for missing OMs
    I3Geometry geo = frame->Get<I3Geometry>();
    if (!frame->Has(hitSeriesMapName_)){
	    log_fatal("No I3MCPESeriesMap named %s found in frame", 
			    hitSeriesMapName_.c_str());
    }


    I3MCPESeriesMap::const_iterator om_iter;
    I3MCPESeriesMapConstPtr hitmap = 
		frame->Get<I3MCPESeriesMapConstPtr>(hitSeriesMapName_); 

    for(om_iter = hitmap->begin(); om_iter != hitmap->end(); om_iter++) 
    { 
	    // Check that om is part of geometry
            if (geo.omgeo.find(om_iter->first) != geo.omgeo.end()) { 
		    hitcount += om_iter->second.size(); 
		    BOOST_FOREACH(const I3MCPE& pe, om_iter->second){ 
			    temp_primary = I3MCTreeUtils::GetPrimaryPtr(mctree_ptr, pe.ID); 
			    primarymap[temp_primary->GetID()] = true; 
		    } 
	    } 
	    // if we have passed the threshold, no need to keep looking 
	    //  (unless we are prunning the mctree)
            if (!pruneTree_ && hitcount >= threshold_ ) { 
		    log_debug("Found at least %d hits. Not filtering", hitcount); 
		    break; 
	    } 
    } 

    if (removeBackgroundOnly_ && filter_){ 
	    I3ParticleConstPtr primaryptr = 
		    frame->Get<I3ParticleConstPtr>("PolyplopiaPrimary"); 
	    if (!primaryptr) 
		    log_fatal("PolyplopiaPrimary not found in I3Frame");

	    if ( !(primarymap.count(primaryptr->GetID()) >= threshold_) )  { 
		    log_debug("Only found %d hits (threshold=%d). filtering frame.", 
				    hitcount, threshold_); 
		    rejectedEvents_++; 
		    return; // don't push frame. Primary has not produced any hits 
	    }
    }


	if (frame->Has(weightMap_)) 
            coincWeights = frame->Get<I3MapStringDoubleConstPtr>(weightMap_); 

    if (pruneTree_) 
    { 

	    if (!frame->Has(weightMap_)) 
                log_fatal("WeightMap '%s' not found in I3Frame",weightMap_.c_str());

	    // Now clean up the MCTree and remove dead branches
	    std::vector<I3Particle>::iterator p;
	    std::vector<I3Particle> primaries = mctree_ptr->get_heads();
	    for (p = primaries.begin(); p != primaries.end(); p++)
	    { 
		    if ( !primarymap.count(p->GetID()) ){ 
			    if (!mctree_ptr->at(*p))
			       log_fatal("Trying to delete a primary that is not in the tree");

			    mctree_ptr->erase(*p);
			    removedBranches_++;
			    if (mctree_ptr->at(*p))
			       log_fatal("Primary is still in tree after deletion");
		    } 
		    
	    }
	    frame->Delete(mcTreeName_);
	    frame->Put(mcTreeName_,mctree_ptr);

	    // replace weights with updated hitcount
	    I3MapStringDoublePtr newWeights = I3MapStringDoublePtr(new I3MapStringDouble());
	    BOOST_FOREACH(const I3MapStringDouble::value_type val, *coincWeights)  
	    { 
                (*newWeights)[val.first] = val.second; 
	    }
	    (*newWeights)[hitSeriesMapName_ + "Count"] = hitcount;
	    frame->Delete(weightMap_);
	    frame->Put(weightMap_,newWeights);
    }

    
    if ( hitcount >= threshold_ ) { 
	    eventCount_++; 
	    PushFrame(frame,"OutBox"); 
	    keptEvents_++;

	    // sum over all weights  and add to summary service 
	    if (coincWeights) { 
		    weightSum_ += coincWeights->at("Weight"); 
		    timeScaleSingle_ = coincWeights->at("TimeScale"); 
		    if ( eventCount_%nprimaries_ ) { 
			    weightprod_ *= coincWeights->at("Weight"); 
		    } else { 
			    weightprod_ *= coincWeights->at("Weight"); 
			    weightPartSum_ += weightprod_; 
			    weightprod_ = 1.0; 
		    } 
	    } 
    } else { 
	    log_debug("Only found %d hits (threshold=%d). filtering frame.", 
		    hitcount, threshold_); 
	    rejectedEvents_++;

	    if (!filter_)
	       PushFrame(frame,"OutBox"); 
    }
    return; 
}

void 
MPHitFilter::Finish()
{
    I3SummaryServicePtr summary = context_.Get<I3SummaryServicePtr>(); 
    if (summary) { 
        (*summary)[weightSumName_] = weightSum_;
        (*summary)[weightPartSumName_] = weightPartSum_;
        if (!std::isnan(timeScaleSingle_))
            (*summary)["TimeScale"] = timeScaleSingle_;
	
    } 
    log_info("%u events passed" ,keptEvents_); 
    log_info("%u events rejected" ,rejectedEvents_); 
    log_info("%u tree branches removed" ,removedBranches_);
}
