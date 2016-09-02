/** $Id: Generator.cxx 137064 2015-08-31 18:24:47Z jvansanten $
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision: 137064 $
 * $Date: 2015-08-31 13:24:47 -0500 (Mon, 31 Aug 2015) $
 */

#include <diplopia/MuonGunBackgroundService.h>
#include <MuonGun/SamplingSurface.h>

#include <dataclasses/physics/I3MCTreeUtils.h>
#include <phys-services/I3RandomService.h>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>



MuonGunBackgroundService::MuonGunBackgroundService  (
                    I3MuonGun::GeneratorPtr generator, 
                    I3RandomServicePtr rng): 
            generator_(generator), 
            rng_(rng),
            rate_(NAN),
            mctreeName_("I3MCTree")
	{
	}


void 
MuonGunBackgroundService::SetRate(double rate) 
{
       rate_ = rate;
}
    
void 
MuonGunBackgroundService::SetMCTreeName(std::string mctreeName) 
{
       mctreeName_ = mctreeName;
}
    
void 
MuonGunBackgroundService::SetRNG(I3RandomServicePtr rng) 
{
       rng_ = rng;
}
	
void MuonGunBackgroundService::SetGenerator(I3MuonGun::GeneratorPtr generator) 
{
       generator_ = generator; 
}

void 
MuonGunBackgroundService::Init()
{ 
        if (!rng_)
			log_fatal("No RandomService configured!");
		if (!generator_)
			log_fatal("No Generators configured!");
		if (std::isnan(rate_)) {
		   log_warn("Automatic rate calculation not implemented and has not been manually configured");
        }
}

double 
MuonGunBackgroundService::GetRate()
{
		return rate_;
}
	
	
I3MCTreePtr 
MuonGunBackgroundService::GetNextEvent()
{
		I3MCTreePtr mctree = boost::make_shared<I3MCTree>();
		I3MuonGun::BundleConfiguration bundlespec;

		generator_->Generate(*rng_, *mctree, bundlespec);
		return mctree;
}

I3FramePtr 
MuonGunBackgroundService::GetNextFrame()
{ 
		I3FramePtr frame = boost::make_shared<I3Frame>('Q');
		I3MCTreePtr mctree = GetNextEvent();
		frame->Put(mctreeName_, mctree);
		return frame;
}
	

