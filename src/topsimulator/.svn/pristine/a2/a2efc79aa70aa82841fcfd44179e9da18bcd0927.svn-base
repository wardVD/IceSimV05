
#include "topsimulator/interface/I3InjectorService.h"
#include "icetray/I3SingleServiceFactory.h"
#include "icetray/I3Frame.h"
#include "dataclasses/physics/I3MCTree.h"
#include "dataclasses/physics/I3EventHeader.h"
#include "topsimulator/ExtendedI3Particle.h"

namespace topsim {

class MCTreeShimInjector : public I3InjectorService {

public:
	MCTreeShimInjector(const I3Context &ctx) : I3InjectorService(ctx)
	{}

	bool NextEvent(int &run, int &event, I3Particle &primary, I3FrameConstPtr frame)
	{
		mctree_ = frame->Get<I3MCTreeConstPtr>();
		if (mctree_ == NULL || mctree_->size() == 0) {
			log_fatal("No MCTree in frame!");
			return false;
		}
		static int n_event = 0;

		currentParticle_ = mctree_->begin();
		
		primary = *currentParticle_;
		run = 0;
		event = n_event;
		
		currentParticle_++;
		++n_event;
		return true;
	}

	bool NextParticle(ExtendedI3Particle &p)
	{
		while (currentParticle_ != mctree_->end() &&
		    currentParticle_->GetLocationType() == I3Particle::Anywhere) {
			currentParticle_++;
		}

		if (currentParticle_ != mctree_->end()) {
			p = ExtendedI3Particle(*currentParticle_);
			p.SetAirShowerComponent(Undefined);
			currentParticle_++;
			return true;
		}
		return false;
	}

  std::map<std::string, int> GetAirShowerComponentNameMap() const
  {
    static std::map<std::string, int> m;
    if (!m.size())
      m["Undefined"] = Undefined;
    return m;
  }



private:
	I3MCTreeConstPtr mctree_;
	I3MCTree::const_iterator currentParticle_;
};

typedef I3SingleServiceFactory<MCTreeShimInjector, I3InjectorService> MCTreeShimInjectorFactory;

};

I3_SERVICE_FACTORY(topsim::MCTreeShimInjectorFactory);

