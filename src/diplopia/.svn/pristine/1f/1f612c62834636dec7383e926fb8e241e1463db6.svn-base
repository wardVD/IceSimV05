#include <dataclasses/I3Double.h>
#include <icetray/I3Units.h>
#include <icetray/I3Module.h>
#include <dataclasses/I3Constants.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/physics/I3MCTree.h>
#include <dataclasses/physics/I3MCTreeUtils.h>
#include <phys-services/I3SummaryService.h>


using namespace std;

class TestWeights: public I3Module
{

public:

TestWeights(const I3Context& context) : 
  I3Module(context), 
  weight_name_("CorsikaWeightMap"),	// I3Double in the Frame
  mcTreeName_("I3MCTree"), 
  corsikaOversampling_(1.), 
  thetaMin_(0.),
  thetaMax_(90.*I3Units::deg),
  spectralIndexChange_(0.), 
  cylinderRadius_(-1), 
  cylinderLength_(-1), 
  fluxSum_(-1), 
  spectrumType_(0),
  energyPrimaryMin_(0),
  energyPrimaryMax_(0),
  primarySpectralIndex_(-2.7),
  corsikaShowers_(-1)			//no sane default
{
  AddOutBox("OutBox");

  AddParameter("NEvents", "Number of generated showers in CORSIKA", corsikaShowers_); 
  AddParameter("Name", "Name to give the frame", weight_name_);
  AddParameter("MCTreeName", "Name of MCTree List to get from frame", mcTreeName_);
}

void Configure()
{
  GetParameter("NEvents",corsikaShowers_);
  GetParameter("Name",weight_name_);
  GetParameter("MCTreeName", mcTreeName_);

}

void DAQ(I3FramePtr frame)
{
  // Get the I3MCTree from the frame
  I3MCTreeConstPtr mctree = frame->Get<I3MCTreeConstPtr>(mcTreeName_);
  if (!mctree) {
    log_fatal("Frame does not contain MCTree %s", mcTreeName_.c_str());
  }

  // Get list of primaries
  vector<I3Particle> primaries = I3MCTreeUtils::GetPrimaries(*mctree);
  if (primaries.size() != 1 && abs(spectralIndexChange_) > 0.01) {
    log_fatal("%s contains more than one primary", mcTreeName_.c_str());
  } else if (primaries.size() == 0) {
    log_error("%s contains no primary", mcTreeName_.c_str());
  }

  // Get the primary particle energy: i.e. the first of the list
  double cosmicParticleEnergy = primaries[0].GetEnergy();
  double TimeScale = 1.0;

  // Instantiate a Map where to store weighting info
  I3MapStringDoublePtr frame_weightdict_ptr(new I3MapStringDouble);

  // Start filling the Map
  (*frame_weightdict_ptr)["NEvents"] = corsikaShowers_; 

  double energyFactor = cosmicParticleEnergy / I3Units::GeV;
  CorsikaWeight = 1.0 / energyFactor;

  (*frame_weightdict_ptr)["Weight"] = CorsikaWeight; 
  (*frame_weightdict_ptr)["TimeScale"] = TimeScale; 

  // add single CORSIKA file TimeScale to the Summary Service
  I3SummaryServicePtr summary = context_.Get<I3SummaryServicePtr>();  

  if (summary) {
    (*summary)["TimeScale"] = TimeScale;
  }

  frame->Put(weight_name_, frame_weightdict_ptr);

  PushFrame(frame,"OutBox");
} 

// the folowing silences "unused-private-field" warnings, but is insecure
friend void i_dont_exist();

private:

  std::string weight_name_;
  std::string mcTreeName_;
  double corsikaOversampling_;
  double thetaMin_;
  double thetaMax_;
  double spectralIndexChange_;
  double cylinderRadius_;
  double cylinderLength_;
  double fluxSum_;
  int spectrumType_;
  double energyPrimaryMin_;
  double energyPrimaryMax_;
  double primarySpectralIndex_;
  int corsikaShowers_;

  double energyIntegral;
  double energyFactor;
  double CorsikaWeight;
  double TimeScale;

};


I3_MODULE(TestWeights);
