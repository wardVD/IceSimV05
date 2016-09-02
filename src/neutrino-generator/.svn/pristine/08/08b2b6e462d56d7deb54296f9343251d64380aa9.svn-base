#include"I3NuGenTestModule.h"

#include<I3Test.h>
#include<icetray/I3Frame.h>
#include<dataclasses/physics/I3Particle.h>
#include<dataclasses/physics/I3MCTree.h>
#include<dataclasses/physics/I3MCTreeUtils.h>
#include<neutrino-generator/utils/Calculator.h>
#include <icetray/I3Logging.h>

using namespace std;

I3_MODULE(I3NuGenTestModule);

I3NuGenTestModule::I3NuGenTestModule(const I3Context& context) 
    : I3Module(context), nunubarratio_(2, 1)
{
  //GetIcetrayLogger()->SetLogLevel(I3LOG_DEBUG);
  AddParameter("NuNuBarRatio", "NuNuBar ratio vec", nunubarratio_);
  AddParameter("runmode", "name of runmode", runmode_);
  AddOutBox("OutBox");
  evtno_ = 0;
}

void
I3NuGenTestModule::Configure()
{
  GetParameter("NuNuBarRatio", nunubarratio_);
  GetParameter("runmode", runmode_);
  evtno_ = 0;
}

void
I3NuGenTestModule::DAQ(I3FramePtr frame)
{

  I3MapStringDoubleConstPtr wmapptr = 
          frame->Get<I3MapStringDoubleConstPtr>("I3MCWeightDict");
  ENSURE((bool)wmapptr);

  // make copy of map
  I3MapStringDouble wmap(*wmapptr);
  
  ENSURE(wmap.size() != 0);

  /*
  cerr << "*** I3NuGenTestModule wmap for " << runmode_ << " event " << evtno_ << endl;
  I3MapStringDouble::iterator iter;
  for (iter = wmap.begin(); iter != wmap.end(); ++iter) {
     cerr << iter->first << ", " << iter->second << endl;
  }
  cerr << "*** I3NuGenTestModule wmap end for " << runmode_ << endl;
  */
  ++evtno_;

  // check weights
  CheckPrimaryInfo(wmap);
  CheckInteractionInfo(wmap);
  CheckInteractionWeights(wmap);

  // Check MCTree
  I3MCTreeConstPtr mctree = frame->Get<I3MCTreeConstPtr>();
  ENSURE((bool)mctree);
  ENSURE(mctree->size() != 0);

  vector<I3Particle> primaries = I3MCTreeUtils::GetPrimaries(*mctree);
  ENSURE(primaries.size()==1);

  double totw = wmap["TotalWeight"];
  if (totw != 0) {
     // particle interacted. check if the tree has inice particles.
     int exit = 0;
     int nnu = 0;
     I3MCTree::const_iterator iter = mctree->begin();
     for (;iter != mctree->end();++iter) {
        if (iter->GetLocationType() == I3Particle::InIce) {
           exit += 1;
           if (iter->GetType() == I3Particle::NuE ||
               iter->GetType() == I3Particle::NuEBar ||
               iter->GetType() == I3Particle::NuMu ||
               iter->GetType() == I3Particle::NuMuBar ||
               iter->GetType() == I3Particle::NuTau ||
               iter->GetType() == I3Particle::NuTauBar) {
               nnu += 1;
           }
           //cerr << exit << "th inice particle " << iter->GetTypeString() << " at " << iter->GetPos() << endl;
        }
     }
     ENSURE(exit <= 3);
     ENSURE(nnu <= 2); // we may have two InIce nu if NC interaction happens
  }

  PushFrame(frame,"OutBox");
}

void I3NuGenTestModule::CheckPrimaryInfo(I3MapStringDouble &wmap)
{
   vector<string> keys;
   keys.push_back("NEvents");
   keys.push_back("CylinderRadius");
   keys.push_back("CylinderHeight");
   keys.push_back("InjectionAreaCGS");
   keys.push_back("SolidAngle");
   keys.push_back("MinEnergyLog");
   keys.push_back("MaxEnergyLog");
   keys.push_back("PowerLawIndex");

   keys.push_back("PrimaryNeutrinoAzimuth");
   keys.push_back("PrimaryNeutrinoZenith");
   keys.push_back("PrimaryNeutrinoType");
   keys.push_back("PrimaryNeutrinoEnergy");

   for (unsigned int i=0; i<keys.size(); ++i) {
      ENSURE(wmap.count(keys[i]));
      ENSURE(wmap[keys[i]] != 0);
   }
}

void I3NuGenTestModule::CheckInteractionInfo(I3MapStringDouble &wmap)
{

   ENSURE(wmap.count("OneWeight"));
   double onew = wmap["OneWeight"];
   if (onew == 0) return;

   vector<string> keys;
   keys.push_back("BjorkenX");
   keys.push_back("BjorkenY");
   keys.push_back("TotalColumnDepthCGS");
   keys.push_back("TotalXsectionCGS");
   keys.push_back("InteractionType");
   keys.push_back("TrueActiveLengthBefore");
   keys.push_back("TrueActiveLengthAfter");

   ///* they are not essential...
   keys.push_back("ImpactParam");
   keys.push_back("RangeInMeter");
   keys.push_back("RangeInMWE");
   keys.push_back("InteractionColumnDepthCGS");
   keys.push_back("InteractionXsectionCGS");
   keys.push_back("LengthInVolume");
   // */

   for (unsigned int i=0; i<keys.size(); ++i) {
      ENSURE(wmap.count(keys[i]));
      //cerr << keys[i] << " " << wmap[keys[i]] << endl;
      ENSURE(wmap[keys[i]] != 0);
   }
}

void I3NuGenTestModule::CheckInteractionWeights(I3MapStringDouble &wmap)
{

   ENSURE(wmap.count("OneWeight"));
   double onew = wmap["OneWeight"];
   if (onew == 0) return;

   vector<string> keys;
   keys.push_back("TotalWeight");
   keys.push_back("DirectionWeight");
   keys.push_back("TypeWeight");
   keys.push_back("SelectionWeight");
   keys.push_back("PropagationWeight");
   keys.push_back("InteractionPositionWeight");
   keys.push_back("InteractionWeight");

   for (unsigned int i=0; i<keys.size(); ++i) {
      ENSURE(wmap.count(keys[i]));
   }

   // check TypeWeight
   double sum = 0;
   for (unsigned int i=0; i<nunubarratio_.size(); ++i) {
      sum += nunubarratio_[i];
   }
   double nuratio = nunubarratio_[0] / sum;
   double antinuratio = nunubarratio_[1] / sum;
   double pid = wmap["PrimaryNeutrinoType"];
   double ratio = nuratio;
   if (pid < 0) ratio = antinuratio;
   ENSURE_DISTANCE(wmap["TypeWeight"], ratio, 1e-3, "Type weight does not match");

   // totw does NOT include TypeWeight
   double dirw = wmap["DirectionWeight"];
   double selw = wmap["SelectionWeight"];
   double propw = wmap["PropagationWeight"];
   double intposw = wmap["InteractionPositionWeight"];
   double intw = wmap["InteractionWeight"];

   /*
   cerr << "dirw " << dirw << " selw " << selw 
        << " propw " << propw << " intposw " << intposw 
        << " intw " << intw << endl;
   */

   double mytotw = dirw * selw * propw * intposw * intw;
   double totw = wmap["TotalWeight"];
   ENSURE_DISTANCE(totw, mytotw, 1e-8, "TotalWeight mismatch");

   // calculate oneweight
   
   double primE = wmap["PrimaryNeutrinoEnergy"];
   double elogmin = wmap["MinEnergyLog"];
   double elogmax = wmap["MaxEnergyLog"];
   double gamma = wmap["PowerLawIndex"];
   double solidangle = wmap["SolidAngle"];
   double injarea = wmap["InjectionAreaCGS"];

   /*
   cerr << "primE " << primE << " elogmin " << elogmin 
        << " elogmax " << elogmax << " gamma " << gamma 
        << " solidangle " << solidangle << " injarea " 
        << injarea << endl;
   */

   // calculate energy factor: energy-integral / E^-gama
   double energyFactor = nugen::Calculator::CalcPowerLawEnergyFactor(
                           primE, gamma, elogmin, elogmax);

   double myonew = mytotw * energyFactor * injarea * solidangle;
   double diff = onew - myonew;
   //printf("onew - myonew = %16.16f\n", diff);
   ENSURE_DISTANCE(diff, 0,  1e-12, "oneweight mismatch!");

}
