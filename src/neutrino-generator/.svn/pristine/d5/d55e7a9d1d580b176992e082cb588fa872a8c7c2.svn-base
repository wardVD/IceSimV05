#include"I3NuGenTestModule.h"

#include<I3Test.h>
#include<icetray/I3Frame.h>
#include<dataclasses/physics/I3Particle.h>
#include<dataclasses/physics/I3MCTree.h>
#include<dataclasses/physics/I3MCTreeUtils.h>
#include<neutrino-generator/utils/Calculator.h>
#include<earthmodel-service/EarthModelCalculator.h>
#include <icetray/I3Logging.h>

using namespace std;

I3_MODULE(I3NuGenTestModule);

I3NuGenTestModule::I3NuGenTestModule(const I3Context& context) 
    : I3Module(context), nunubarratio_(2, 1)
{
  //GetIcetrayLogger()->SetLogLevel(I3LOG_DEBUG);
  AddParameter("runmode", "name of runmode", runmode_);
  AddParameter("zenith", "zenith angle", zenith_);
  AddParameter("energy", "energy", energy_);
  AddOutBox("OutBox");
  evtno_ = 0;
}

void
I3NuGenTestModule::Configure()
{
  GetParameter("runmode", runmode_);
  GetParameter("zenith", zenith_);
  GetParameter("energy", energy_);
  evtno_ = 0;
}

void
I3NuGenTestModule::DAQ(I3FramePtr frame)
{

  I3MapStringDoubleConstPtr wmapptr = 
          frame->Get<I3MapStringDoubleConstPtr>("I3MCWeightDict");
  ENSURE(wmapptr);

  // make copy of map
  I3MapStringDouble wmap(*wmapptr);
  ENSURE(wmap.size() != 0);

  // Check MCTree
  I3MCTreeConstPtr mctree = frame->Get<I3MCTreeConstPtr>();
  ENSURE(mctree);
  ENSURE(mctree->size() != 0);

  // get InIce neutrino
  double onew = wmap["OneWeight"];

  I3Particle inice; 

  if (onew == 0) {
     PushFrame(frame,"OutBox");
     return;
  }

  // particle interacted. check if the tree has inice particles.
  int nnu = 0;
  int ninice = 0;
  I3MCTree::const_iterator iter = mctree->begin();
  for (;iter != mctree->end();++iter) {
     
     if (iter->GetLocationType() == I3Particle::InIce) {
        ninice++;
        if (iter->GetType() == I3Particle::NuE ||
            iter->GetType() == I3Particle::NuEBar ||
            iter->GetType() == I3Particle::NuMu ||
            iter->GetType() == I3Particle::NuMuBar ||
            iter->GetType() == I3Particle::NuTau ||
            iter->GetType() == I3Particle::NuTauBar) {
            inice = (*iter);
            exit++;
        }
     }
  }
  ENSURE_EQUAL(exit, 1, "No InIce neutrino exist!");
  ENSURE_EQUAL(ninice, 3, "Number of inice must be three!");

  // do manual calculation. 
  // following checks assume particles injected from
  // fixed zenith angle and fixed energy with CIRCLE injection.
  // Injection cylinder is 900m radius, 1200m activeHeightBefore,
  // 1200m activeHeightAfter.

  double lepton_range = -1, total_cdep = -1, total_xsec = -1, 
  double lepton_range_tol = -1, total_cdep_tol = -1, total_xsec_tol = -1, 
  double nsigma = 10.0;

  if (inice.GetType == I3Particle::NuE 
      || inice.GetType() == I3Particle::NuEBar) {

     lepton_range = -1;
     lepton_range_tol = 0;

     if (zenith_ == 0.*I3Units::degree) {

        if (energy_ == 1e3) {
           total_cdep = 221180;
           total_cdep_tol = nsigma * 2.03727e-10;
           total_xsec = 6.45224e-36; 
           total_xsec_tol = nsigma * 1.70324e-36;

        } else if (energy == 1e6) {
           total_cdep = 280041;
           total_cdep_tol = nsigma * 5.23869e-10;
           total_xsec = 9.42632e-34; 
           total_xsec_tol = nsigma * 1.71469e-36;

        } 

     } else if (zenith_ == 90. * I3Units::degree) {

        if (energy_ == 1e3) {
           total_cdep = 221180;
           total_cdep_tol = nsigma * 2.03727e-10;
           total_xsec = 6.45249e-36; 
           total_xsec_tol = nsigma * 1.70301e-36;

        } else if (energy == 1e6) {
           total_cdep = 280041;
           total_cdep_tol = nsigma * 5.23869e-10;
           total_xsec = 9.42632e-34; 
           total_xsec_tol = nsigma * 1.71469e-36;

        } 

     } else if (zenith_ == 180. * I3Units::degree) {

        if (energy_ == 1e3) {
           total_cdep = 221180;
           total_cdep_tol = nsigma * 2.03727e-10;
           total_xsec = 6.45233e-36; 
           total_xsec_tol = nsigma * 1.70315e-36;

        } else if (energy == 1e6) {
           total_cdep = 279557;
           total_cdep_tol = nsigma * 6.40284e-10;
           total_xsec = 9.42802e-34; 
           total_xsec_tol = nsigma * 1.72707e-36;

        } 





  } else if (inice.GetType == I3Particle::NuTau 
             || inice.GetType() == I3Particle::NuTauBar) {


  } else if (inice.GetType == I3Particle::NuMu 
             || inice.GetType() == I3Particle::NuMuBar) {

     if (zenith_ == 0.*I3Units::degree) {

        if (energy_ == 1e3) {
           lepton_range = 3734.54; 
           lepton_range_tol = nsigma * 3.18323e-12; 
           total_cdep = 346358;
           total_cdep_tol = nsigma * 2.0039;
           total_xsec = 6.71191e-36; 
           total_xsec_tol = nsigma * 1.70703e-36;

        } else if (energy == 1e6) {
           lepton_range = 33836.5;
           lepton_range_tol = nsigma * 3.63798e-11;
           total_cdep = 346358;
           total_cdep_tol = nsigma * 2.10792;
           total_xsec = 9.24808e-34; 
           total_xsec_tol = nsigma * 1.94421e-35;

        } 

     } else if (zenith_ == 90. * I3Units::degree) {

        if (energy_ == 1e3) {
           lepton_range = 3734.54; 
           lepton_range_tol = nsigma * 3.18323e-12; 
           total_cdep = 594634;
           total_cdep_tol = nsigma * 7.09656e-10;
           total_xsec = 6.71191e-36; 
           total_xsec_tol = nsigma * 1.70703e-36;

        } else if (energy == 1e6) {
           lepton_range = 33836.5;
           lepton_range_tol = nsigma * 3.63798e-11;
           total_cdep = 3.60483e+06;
           total_cdep_tol = nsigma * 2.78074e-09;
           total_xsec = 9.24808e-34; 
           total_xsec_tol = nsigma * 1.94421e-35;

        } 

     } else if (zenith_ == 180. * I3Units::degree) {

        if (energy_ == 1e3) {
           lepton_range = 3734.54; 
           lepton_range_tol = nsigma * 3.18323e-12; 
           total_cdep = 653951;
           total_cdep_tol = nsigma * 3.49246e-10;
           total_xsec = 6.71191e-36; 
           total_xsec_tol = nsigma * 1.70703e-36;

        } else if (energy == 1e6) {
           lepton_range = 33836.5;
           lepton_range_tol = nsigma * 3.63798e-11;
           total_cdep = 3.67115e+06;
           total_cdep_tol = nsigma * 3.72529e-09;
           total_xsec = 9.24808e-34; 
           total_xsec_tol = nsigma * 1.94421e-35;

        } 
     }
  }


  
     
  ENSURE_DISTANCE(lepton_range, wmap["RangeInMWE"], 1e-3, "RangeInMWE doesn't match");

  ENSURE_DISTANCE(total_cdep,  wmap["TotalColumnDepthCGS"], 1e-3, "Column depth doesn't match");
  ENSURE_DISTANCE(total_xsec,  wmap["TotalXsectionCGS"], 1e-37, "total Xsec doesn't match");

  double myintw = total_cdep*total_xsec;
  ENSURE_DISTANCE(myintw,  wmap["InteractionWeight"], 1e-6, "InteractionWeight doesn't match");
  
  


  
  


 

  // 

  cerr << "*** I3NuGenTestModule wmap for " << runmode_ << " event " << evtno_ << endl;
  I3MapStringDouble::iterator iter;
  for (iter = wmap.begin(); iter != wmap.end(); ++iter) {
     cerr << iter->first << ", " << iter->second << endl;
  }
  cerr << "*** I3NuGenTestModule wmap end for " << runmode_ << endl;
  ++evtno_;

  // check weights
  CheckPrimaryInfo(wmap);
  CheckInteractionInfo(wmap);
  CheckInteractionWeights(wmap);

  // Check MCTree
  I3MCTreeConstPtr mctree = frame->Get<I3MCTreeConstPtr>();
  ENSURE(mctree);
  ENSURE(mctree->size() != 0);

  vector<I3Particle> primaries = I3MCTreeUtils::GetPrimaries(*mctree);
  ENSURE(primaries.size()==1);

  double onew = wmap["OneWeight"];
  if (onew != 0) {
     // particle interacted. check if the tree has inice particles.
     int exit = 0;
     I3MCTree::const_iterator iter = mctree->begin();
     for (;iter != mctree->end();++iter) {
        if (exit) break;
        
        if (iter->GetLocationType() == I3Particle::InIce) {
           if (iter->GetType() == I3Particle::NuE ||
               iter->GetType() == I3Particle::NuEBar ||
               iter->GetType() == I3Particle::NuMu ||
               iter->GetType() == I3Particle::NuMuBar ||
               iter->GetType() == I3Particle::NuTau ||
               iter->GetType() == I3Particle::NuTauBar) {
               exit = 1;
           }
        }
     }
     ENSURE_EQUAL(exit, 1, "No InIce neutrino exist!");
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
   keys.push_back("EnergyLost");
   keys.push_back("InteractionColumnDepthCGS");
   keys.push_back("InteractionXsectionCGS");
   keys.push_back("LengthInVolume");
   //*/

   for (unsigned int i=0; i<keys.size(); ++i) {
      ENSURE(wmap.count(keys[i]));
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

   cerr << "dirw " << dirw << " selw " << selw 
        << " propw " << propw << " intposw " << intposw 
        << " intw " << intw << endl;

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

   cerr << "primE " << primE << " elogmin " << elogmin 
        << " elogmax " << elogmax << " gamma " << gamma 
        << " solidangle " << solidangle << " injarea " 
        << injarea << endl;

   // calculate energy factor: energy-integral / E^-gama
   double energyFactor = nugen::Calculator::CalcPowerLawEnergyFactor(
                           primE, gamma, elogmin, elogmax);

   double myonew = mytotw * energyFactor * injarea * solidangle;
   double diff = onew - myonew;
   printf("onew - myonew = %16.16f\n", diff);
   ENSURE_DISTANCE(diff, 0,  1e-12, "oneweight mismatch!");

}
