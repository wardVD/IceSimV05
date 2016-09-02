/**
 *
 *  Copyright  (C) 2005
 *  The IceCube collaboration
 *  $Id: $
 *  @file I3NuGWeightedEvent.cxx
 *  @version $Revision:  $
 *  @date    $Date:      $
 *  @author Kotoyo Hoshina <hoshina@icecube.wisc.edu>
 *
 *  @brief I3NuGWeightedEvent class implementation file, 
 *  the main class for neutrino-generator program 
 */

#include "dataclasses/I3Constants.h"
#include "neutrino-generator/Steering.h"
#include "neutrino-generator/Particle.h"
#include "neutrino-generator/legacy/I3NuGWeightedEvent.h"
#include "neutrino-generator/legacy/I3NuGInteractionInfo.h"
#include "neutrino-generator/legacy/StaticParams.h"
#include "neutrino-generator/utils/Constants.h"
#include "neutrino-generator/utils/EnumTypeDefs.h"
#include "neutrino-generator/utils/Pickup.h"
#include "neutrino-generator/utils/Utils.h"
#include "phys-services/I3RandomService.h"
#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/I3Map.h"
#include <boost/foreach.hpp>

using namespace std;
using namespace nugen;
using namespace earthmodel;

// static parameters

//__________________________________________________________________
I3NuGWeightedEvent::I3NuGWeightedEvent(
                       SteeringPtr steer,
		       I3NuGInteractionInfoPtr inter_p,
		       I3RandomServicePtr random):
  I3NuGVEvent(), Pickup(),
  random_(random), 
  interaction_ptr_(inter_p)
{
  log_trace("I3NuGWeightedEvent constructed");

  propagating_list_.clear(); //ParticlePtrList
  buffer_list_.clear();  //ParticlePtrList

  // set default interaction crosssection weights for
  // final interaction
  interaction_cc_factor_ = 1;
  interaction_nc_factor_ = 1;
  interaction_gr_factor_ = 1;

  steer_ptr_ = steer;
  earthservice_ptr_ = steer_ptr_->GetEarthModelService();
}

//__________________________________________________________________
I3NuGWeightedEvent::~I3NuGWeightedEvent()
{
}

//__________________________________________________________________
bool I3NuGWeightedEvent::MakeNeutrinoPropagation(ParticlePtr injected_ptr,
                                                 bool skipFinalInteraction)
{


  log_debug("Make a neutrino propagation!");
  //printf("%ith event Make a neutrino propagation!\n", eventCounter_);
  //First of all push_back incident neutrino to the propagation particle
  GetInEarthPropagatingParticleList().push_back(injected_ptr);
  
  ///////////////////////////////////////
  // having non-zero neutrino in in-earth vector, propagate the neutrino
  // which fills daughters if interacted
  //
  // Possible output from this function is that initial_neu is not changed but any daughters if interacted
  // if no daughter is created it means that it propagated upto the 
  // end side of detection volume
  //
  //           inicident_nu    composite_daughters
  // 1)CC case: Nu_e,mu,tau   - e,mu,tau 
  //                          - hadron
  // 2)NC case: Nu_e,mu,tau   - Nu_e,mu,tau
  //                          - hadron
  // 3)Not Interacted 
  //            Nu_e,mu,tau   - empity
  //
  ///////////////////////////////////////


  // let's get the first element and propagate it
  // sanity check
  if(GetInEarthPropagatingParticleList().size()!=1){
     log_fatal("no neutrino to propagate");
     return false;
  }
  
  bool initial_interacted = false;

  ParticlePtr initial_neu_ptr = GetInEarthPropagatingParticleList().front();


  //
  // Step 1: propagate initial neutrino only when GetInjectOnCylinder()
  // returns false.
  // If injectOnCylinder is true, we start propagation from front side 
  // of detection volume and InEarth propagation is not performed.
  // 

  if (! (steer_ptr_->GetSimMode() == FINALONLY)) {
     initial_interacted = PropagateNeutrino(initial_neu_ptr);
  } else {
     initial_neu_ptr->SetLocationType(I3Particle::InIce);
  }

  //
  // Step 2: Check whether an interaction occurred or not.
  // If no interaction happened, the primary neutrino reached to a 
  // front side of detection volume then we make force interaction 
  // within the detection volume.
  // (call MakeFinalInteraction)
  // 

  if( (!initial_interacted) ){
     log_info("initial_primary is not interacted, fill final interaction with probability");

     if (skipFinalInteraction) {
        initial_neu_ptr->SetLocationType(I3Particle::InIce);
        finalInteractionNuCandidates_.push_back(initial_neu_ptr);
        return true;
     }

     if (initial_neu_ptr->GetShape() == I3Particle::Dark) {
        log_trace("this is dark neutrino, return false now!");
        return false;
     }
    
     //then this time initial_ptr and primary are the same 
     MakeFinalInteraction(initial_neu_ptr); 
     //in-ice must be filled by now
     GetInIceParticle()->CheckParticle();

     log_debug("   --- Final Interaction finished with %zu particles ---", 
             (size_t)GetInIceParticle()->GetTotalNDaughters());
    
     return true; //and done!
  }

  //
  // Step 3: At this stage, our initial neutrino should have interacted. 
  // Then: 
  //  - select particles which need to be propagated in Step 4
  //    (e.g. neutrions, taus)
  //    and push_back them to InEarth particle vector
  //  - select neutrinos which reached to the detection volume 
  //    and push_back them to InIce particle vector
  // 

  // sanitary check
  if(!initial_neu_ptr->InteractedOrDecayed()){
     log_error("CHECK!!  initial_primary interacted, but no secondary created");
     return false;
  }
  log_info("      initial_primary interacted, secondary created");

  CheckAndPushBack_Daughters(initial_neu_ptr);
  
  //
  // Step 4: Perform InEarth propagation.
  // This process recursively calls "PropagateNeutrino" function
  // until all "InEarth" particles are propagated.
  // Note that one call of "PropagateNeutrino" may generate daughter 
  // "InEarth" particles.
  //
  unsigned int number_propagated=1; //first particle has propagated already   
      
  ///////////////////Main propagation starts here///////////////////////
  while (GetInEarthPropagatingParticleList().size() > number_propagated) {

    
     ParticlePtr thisPart = GetInEarthPropagatingParticleList()[number_propagated];
     log_trace("------ Looking at %d th particle to be propagated, "
               " out of %zu particles ------", 
	      number_propagated+1, GetInEarthPropagatingParticleList().size());
      
     if (thisPart->IsTau()) {
        log_trace("This particle is tau. Let it decay.");
        //tau always decays but it is possible it looses all of 
        // their energy before producing daughters
        MakeTauDecay(thisPart);

        if (thisPart->InteractedOrDecayed()) { 
           log_trace(" tau decayed, secondary created");
           CheckAndPushBack_Daughters(thisPart);
        }
      
     } else if (thisPart->IsNeutrino()) {

        if (thisPart->GetShape() == I3Particle::Dark) {
           log_trace("this is dark neutrino, skip it!");
           ++number_propagated;
           continue;
        }
      
        log_trace("This particle is neutrino. Let it propagate.");
        //thisPart->SetExitPosition();
        bool this_interacted = PropagateNeutrino(thisPart);
	
        if (this_interacted && thisPart->InteractedOrDecayed() ){      
           log_trace("This particle interacted, secondary created");
           CheckAndPushBack_Daughters(thisPart);
        }

     } else {
        log_trace("This particle is not propagated");
     } 

     ++number_propagated;

  } //while propagation loop

  ///////////////////Main propagation ends here///////////////////////
  log_trace("Finish filling propagating particles(in-earth)," 
            " %d particles propagated", number_propagated);
  
  //
  // Step 5:
  // Find neutrinos (and taus?) which didn't interacted within the propagation loop
  // and select them as possible candidates of final "parent" particles
  // (the "parent" particle generates visible daughter particles in 
  //  IceCube later)
  //
  int n_in_earth = GetInEarthPropagatingParticleList().size();
  log_info("Number of in-Earth %d", n_in_earth);

  finalInteractionNuCandidates_.clear();
  
  for (int i=0; i< n_in_earth; ++i) {
     ParticlePtr currentPart = GetInEarthPropagatingParticleList()[i];

     if (!currentPart->InteractedOrDecayed()) {       
      
        if(currentPart->IsNeutrino() && 
           currentPart->GetShape() != I3Particle::Dark) {
           log_info("this is candidate for InEarthNu");
           finalInteractionNuCandidates_.push_back(currentPart);
        } else {
           log_debug("This particle is not neutrino, not candidate");
        }
     } 
     I3Position p = earthservice_ptr_->GetEarthCoordPosFromDetCoordPos(currentPart->GetEndPosition()); 
     log_info("This particle %s is already interacted at r=%f",
               currentPart->GetTypeString().c_str(), 
               p.Magnitude());

  } //all in earth particle loop
  
  //
  // Step 6:
  // Here we choose only one primary among all "InIce" parent candidates.
  // It is true that one incident neutrino may generate multiple "InIce"
  // neutrinos (if decay happens), and there is non-zero possibility that 
  // every InIce neutrino generate a visible particle.
  // In this case, we may observe multiple tracks within IceCube.
  // However, this probability is very tiny and what mostly we observe
  // only one track (or cascade) from one incident neutrino.
  // Since we force to interact within detection volume, making final 
  // interaction for all InIce neutrino candidate results in generating 
  // too much multi-track events.
  // In order to avoid the bias to this kind of special event, 
  // we randomly select only one of "InIce" parent candidate and make 
  // final interaction onto the selected parent.
  //
 
  if (finalInteractionNuCandidates_.size()) {

     log_trace("got %zu possible primaries.", finalInteractionNuCandidates_.size());

     if (skipFinalInteraction) {
        for (unsigned int i=0; i<finalInteractionNuCandidates_.size(); ++i) {
           finalInteractionNuCandidates_[i]->SetLocationType(I3Particle::InIce);
        }
        return true;
     }

     // this is old function
     // Inside ChooseWhichNeutrinoToInteracts, FinalParticleVect will be filled
     //ParticlePtr final_inice = ChooseNeutrinoToInteractFromBuffer();

     //
     // this is new code
     //
     pair<ParticlePtr, double> picked = PickupANeutrino(random_,
                            finalInteractionNuCandidates_);
     ParticlePtr final_inice = picked.first;
     double selection_weight = picked.second;

     // Where do we store selection_weight?
     // It happens in the end of the propagation, so I think
     // propagation weight is one of the appropreate one...
     const InteractionInfo &info = final_inice->GetInteractionInfoRef();
     double prop_weight = info.GetPropagationWeight();
     final_inice->SetPropagationWeight(prop_weight*selection_weight);
     SetInIceParticle(final_inice);

     // here we set weights etc.
     MakeFinalInteraction(final_inice);
     return true;

  }

  log_info("NO PossibleParentFinalParticleVect.");	 

  return false;
}


//__________________________________________________________________
bool I3NuGWeightedEvent::PropagateNeutrino(ParticlePtr particle_ptr)
{
//
// Dec 14 2010 K.Hoshina
// Now we have two options to propagate neutrinos.
//
// mode 1 "Legacy" mode
//   This option is same as original NuGen neutrino propagation:
//   some neutrinos may be absorved by the Earth due to cc interactions.
//
// mode 2 "NCGRWeighted" mode
//   This option make all neutrino propagate through the Earth.
//   Some particle may have a propagation weight which should be 
//   multipled with the interaction weight later.
//   It allows only NC and GR interactions (if applicable) during 
//   the propagation.
//


  if(particle_ptr->InteractedOrDecayed()) {
    log_error("propagation requested for neutrino already interacted"); 
    return true;
  }

  InteractionGeo &intgeo = particle_ptr->GetInteractionGeoRef();
  if(! particle_ptr->IsGeoCalculated()) {
    log_error("Geom calculation failed, return false now"); 
    return false;
  }

  //Get start position of this particle
  I3Position startPosDC(particle_ptr->GetPos());

  //
  // Particle::GetInteractionGeoRef() calculates interaction geom
  // automatically.
  //
  I3Position endPosDC   = intgeo.GetEnterDetVolDC();
  I3Position exitPosDC  = intgeo.GetExitDetVolDC();

  I3Position startPosCE(earthservice_ptr_->GetEarthCoordPosFromDetCoordPos(startPosDC));
  I3Position endPosCE(earthservice_ptr_->GetEarthCoordPosFromDetCoordPos(endPosDC));  

  I3Direction dirCE(endPosCE-startPosCE);
  
  double   propLength = (startPosCE-endPosCE).Magnitude();
  int      nsteps = StaticParams::gNSTEPS;
  double   dx = propLength/nsteps;
  double   mindx = 10*StaticParams::gFINE_STEP_SIZE;
  if (dx < mindx) {
     //The step is too small. let's change nsteps.
     nsteps = (int)propLength / mindx;
     dx = propLength / nsteps;
  }

  //particle propagation from production position to the exit position
  log_trace("   -----------------------------------------------------------");
  log_trace("   Propagate Neutrino: start DC(%.2f, %.2f, %.2f)", startPosDC.GetX(),startPosDC.GetY(),startPosDC.GetZ());  
  log_trace("                     :  end  DC(%.2f, %.2f, %.2f)", endPosDC.GetX(), endPosDC.GetY(), endPosDC.GetZ());  
  log_trace("                     :  exit DC(%.2f, %.2f, %.2f)", exitPosDC.GetX(), exitPosDC.GetY(), exitPosDC.GetZ());  
  log_trace("   -----------------------------------------------------------");
  log_trace("   Propagate Neutrino: start CE(%.2f, %.2f, %.2f), %.2f m", 
	    startPosCE.GetX(),startPosCE.GetY(),startPosCE.GetZ(), startPosCE.Magnitude());  
  log_trace("                     :  end  CE(%.2f, %.2f, %.2f), %.2f m", 
	    endPosCE.GetX(), endPosCE.GetY(), endPosCE.GetZ(), endPosCE.Magnitude());  
  log_trace("   -----------------------------------------------------------");
  log_trace("    over length %.2f [m] with NStep %d, stepsize %f [m]", propLength, nsteps, dx);
  log_trace("   -----------------------------------------------------------");

  I3Position localCE(startPosCE);  //current position in CE
  double columnDepthFromStart = 0.0;

  //
  // Start propagation from start position to flont plane of detection Volume
  //
  for(int istep=0; istep < nsteps; ++istep) {

     localCE += dirCE * dx;  

     //gettotalcrosssection returns in mb
     //this requires only particle energy and medium at the position(ice or rock)
     double sigtotmb = interaction_ptr_->GetTotalCrosssection(particle_ptr, localCE);
     double density = earthservice_ptr_->GetEarthDensityInCGS(localCE)*1.e6;//g/cm3*1.e6 = g/m3
     columnDepthFromStart += density*dx;
     
     /**
      * density is in g/m3, pmass is proton mass in g, sigtotmb is in mb, dx is in m
      * -> mb * m * g/m3 /g => mb/m2 = 10-27cm2/10^4cm2 => 10^-31 
      * probability to have interaction in distance dx
      *
      */

     //interaction probability that this particle at local interacts within a length dx
     double surviving_prob = exp(-sigtotmb * dx * density * 1.0e-31 / Constants::PMASS);
     double thisstepinteractionprobability = 1.0 - surviving_prob;
     double temprandom = random_->Uniform(0.0, 1.0);

     if (temprandom < thisstepinteractionprobability) {
        //neutrino is interacted !
        localCE += dirCE * temprandom * dx;
        log_trace("   interact at %d th step, propagated %.2f [m]", istep, (localCE-startPosCE).Magnitude());
        log_trace("   at the position ce (%.2f, %.2f, %.2f) [m]", localCE.GetX(), localCE.GetY(), localCE.GetZ());
        particle_ptr->SetLength((localCE-startPosCE).Magnitude());

        /* column depth from creation position to the interaction position [g/m2]*/
        InteractionInfo &intinfo = particle_ptr->GetInteractionInfoRef();
        // unit g/m2 to g/cm2
        intinfo.SetInteractionColumnDepthCGS(columnDepthFromStart*1e-4);
        intinfo.SetInteractionWeight(1.0);
        /* length between the entrance of cylder to the interaction position [m]*/
        intinfo.SetLengthInVolume(1.0);  //ie = na
        intinfo.SetTotalColumnDepthCGS(1.0);
        /* sum of possible crosssections at the energy 
           from in [mb] to in [cm2]*/
        intinfo.SetTotalXsecCGS(sigtotmb*1e-27);	

        log_trace("   now choose the type of interaction");
        InteractionType int_type = UNDEFINED;

        bool doWeightedPropagation = true;
        if (StaticParams::gPROPAGATION_MODE == nugen::NOPROPWEIGHT || 
            (StaticParams::gPROPAGATION_MODE == nugen::AUTODETECT && particle_ptr->IsTauFlavor()))  {
           doWeightedPropagation = false; 
        }

        if (doWeightedPropagation) {

           // now we avoid to make cc interaction so that the
           // neutrino is not absorbed in the earth.
           // the return int_type is always nc or gr.
           // propagation weight is filled to both particle_ptr 
           // and its daughter particles in order to compensate 
           // the modified interaction ratio.

           int_type = interaction_ptr_->MakeInteraction(particle_ptr,
                                          0,           // cc ratio
                                          1,           // nc ratio
                                          1);          // gr ratio
        } else {  
          
           // should be staticparams::gpropagation_mode == "noweight" 
           // use old nugen mode.
           int_type = interaction_ptr_->MakeInteraction(particle_ptr);

        }


        log_debug("neutrino is propagating---interaction type is %d", int_type);
        if (int_type==NOINTERACTION) {
           log_error("no neutrino to make interaction --- check!!");
        }
        log_trace("   we have total %zu secondary particles", (size_t)particle_ptr->GetTotalNDaughters());

        // this neutrino interacted. return now.
        return particle_ptr->InteractedOrDecayed(); 
     }

  }//end step

  if (!particle_ptr->InteractedOrDecayed()) {
     log_trace("   neutrino did not interacted.");
  }
  if (!particle_ptr->GetTotalNDaughters()) {
     log_trace("   No secondary has been created.");
  }
  return false;
}

//__________________________________________________________________
double I3NuGWeightedEvent::SelectionWeighter(ParticlePtr p)
{ 
   // dummy weighter
   return 1.0;
}

//__________________________________________________________________
ParticlePtr I3NuGWeightedEvent::ChooseNeutrinoToInteractFromBuffer()
{ 
  bool choose_neutrino = true; // for debugging set it false in order to 
                               // force to select the first neutrino 

  unsigned int nNeutrino = 1;
  if (choose_neutrino) {
    nNeutrino = finalInteractionNuCandidates_.size();
    log_trace("We have %d possible primary neutrino", nNeutrino);
  }
  
  // if we have only one neutrino that's it.
  if (nNeutrino==1) {
    return finalInteractionNuCandidates_[0];
  }

  //
  // calculate sum of interaction probabilities of all daughter particles
  //
  double sumprobability=0;
  map<double, ParticlePtr> particlemap;
  for (unsigned int i=0; i<nNeutrino;++i) {
      
     ParticlePtr neu_part =  finalInteractionNuCandidates_[i];
     log_trace("Calculate probability for %d th neutrino", i);
     neu_part->CheckParticle();
      
     double thisProbability = CalculateMaximumTotalInteractionProbability(neu_part);
      
     sumprobability += thisProbability; 
     neu_part->GetInteractionInfoRef().SetInteractionWeight(thisProbability);
     particlemap[sumprobability] = neu_part;
  }
  
  // 
  // choose a random number within [0, sumprobability]
  //
  double randomprob = random_->Uniform(0, sumprobability);
  log_trace("Sum of all neutrino's probablities %.12f and random probability %.12f", 
	    sumprobability, randomprob);

  //
  // select a parent particle
  //
  // key : accumulated probability
  // value: particle pointer
  // ---------------------------------------------------------
  // entry id       | 0    | 1      | 2        | 3 ...... 
  // key(prob)      | c0   | c0+c1  | c0+c1+c2 | ....
  // val(particle)  | p0   | p1     | p2       | ....
  // ---------------------------------------------------------
  // if 0 <= x <= c0 then
  // lower_bound(x) gives entry id 0
  // if c0 < x <= (c0+c1) then
  // lower_bound(x) gives entry id 1
 
  map<double, ParticlePtr>::iterator iter;
  iter = particlemap.lower_bound(randomprob);
  if (iter == particlemap.end() ) {
     log_fatal("Failed to find randomprob %f within [0, %f]", randomprob, sumprobability);
  }

  return iter->second;

}

//__________________________________________________________________
void I3NuGWeightedEvent::MakeFinalInteraction(ParticlePtr neu_part)
{
  //
  // Set Final Interaction position
  // it calculate interaction weight.
  // It also sets following params:
  //   - parent particle length
  //   - total cross section at interaction vertex
  //   - total interaction probability (interaction weight)
  //   - total column depth up to interaction vertex
  //   - total column depth within detection volume
  //   - length from front plane of detection volume to vertex
  //   - total length within detection volume
  //
  SetFinalInteractionPosition(neu_part);

  //
  // Choose one of NC/CC/GR interactions
  // and daughter particle properties.
  //
  // interaction_cc_factor_ etc. modify fraction of each 
  // interaction simulated, with correct weights.
  // See descriptions in source code of 
  // I3NuGInteractionInfo::MakeInteraction()
  // It sets following params:
  //   - interaction type
  //   - cross section for the chosen interaction at given energy
  //   - update propagation weight (not interaction weight)
  //
  InteractionType int_type = 
            interaction_ptr_->MakeInteraction(neu_part, 
                                              interaction_cc_factor_, 
                                              interaction_nc_factor_, 
                                              interaction_gr_factor_);

  log_debug("interaction type is %d",int_type);

  if (int_type==NOINTERACTION) {
     log_warn("no neutrino to make interaction : neutrino energy %g [GeV]", neu_part->GetEnergy()); 
  }

  SetInIceParticle(neu_part);
}

//__________________________________________________________________
bool I3NuGWeightedEvent::MakeTauDecay(ParticlePtr tau_ptr)
{ 
  if (tau_ptr->InteractedOrDecayed()) return false;
  if (! tau_ptr->IsTau())             return false;

  //Set Decay Length, Energy at the time of Decay which equals to the sum of daughter energy  
  bool decayed = interaction_ptr_->MakeDecay(tau_ptr);
  //now tau_ptr has daughters
  log_trace("decay daughters %d", tau_ptr->GetTotalNDaughters());
  
  return decayed;
}


//__________________________________________________________________
void I3NuGWeightedEvent::CheckAndPushBack_Daughters(ParticlePtr thisPart){
  int n_daughters=0;
  int n_pushed=0;

  log_trace("Check if this particle is created in volume /or/ Possible in Earth(Nu or Tau)");

  // loop over all daughters of thisPart
  for (vector<ParticlePtr>::const_iterator this_daughter_itr = thisPart->GetDaughters().begin();
       this_daughter_itr!=thisPart->GetDaughters().end();
       ++this_daughter_itr) { 

     ParticlePtr daughter = *this_daughter_itr;
     I3Position dpos(daughter->GetPos());
     string   dname = daughter->GetTypeString();

     InteractionGeo &intgeo = daughter->GetInteractionGeoRef();

     if (intgeo.IsInside(dpos)) {
        //
        // TODO :
        // Oct.1 2012 K.Hoshina
        //
        // I think the if-condition is quite 
        // confusing, and if the condition is 
        // fulfilled, calling break in the end 
        // of the part looks like a dangerous bug.
        // However, this function is always called
        // after PropagateNeutrino function which
        // NEVER generate a vertex inside the 
        // detection volume.
        // So I guess this if-condition is 
        // never fulfilled too...
        // For now, I keep the part until I find
        // a time to check detailed behavior.
        //

        // Check where the particle start from.
        // If the daughter start within detection volume, its parent could be
        // an inice particle. 
        log_debug("daughter %s found in volume pos(%f,%f,%f)", 
                  dname.c_str(), dpos.GetX(), dpos.GetY(), dpos.GetZ());

        // Set parent to InIce only when the daughter in the detector volume
        // is something other than a neutrino
        if (! daughter->IsNeutrino()) {
           log_debug("Set parent %s of daughter %s to in-ice particles:",
                     thisPart->GetTypeString().c_str(), dname.c_str());
           SetInIceParticle(thisPart);
        }

        break;

     } else if ( daughter->IsNeutrino() ) {
        // the daughter does NOT start within the detection volume
	    
        ++n_pushed;
        GetInEarthPropagatingParticleList().push_back(daughter);
        log_debug("daughter is %s at pos(%f,%f,%f), push back to InEarth",
                  dname.c_str(),  dpos.GetX(), dpos.GetY(), dpos.GetZ() );

     } else if ( daughter->IsTau() ) {
        // the daughter does NOT start within the detection volume

        ++n_pushed;
        GetInEarthPropagatingParticleList().push_back(daughter);
        log_debug("daughter is %s at pos(%f,%f,%f), push back to InEarth",
                  dname.c_str(),  dpos.GetX(), dpos.GetY(), dpos.GetZ() );

     } else { 
        // this could be muon or electron generated outside of 
        // detection volume, for example.

        log_debug("daughter %s at pos(%f,%f,%f) is not neutrino nor in-volume",
                  dname.c_str(),  dpos.GetX(), dpos.GetY(), dpos.GetZ());
     }
	  
     ++n_daughters;

  }//daughter loop

  log_trace("%d daughters, %d pushed back to in-earth",  n_daughters, n_pushed);

}

//__________________________________________________________________
void I3NuGWeightedEvent::SetFinalInteractionPosition(ParticlePtr particle_ptr)
{
//
// Now we provide many options for weight and position calculation.
// Some of them are not perfectly correct, but we keep it for backward compatible.
//
// StaticParams::gINTPOS_SAMPLE_OPT = 0:
//   This option activate legacy nugen interaction position calculation.
//   sample interaction position linearly in length[m].
//   This is efficient for rock path, however, event statistics will 
//   strongly depend on zenith angle.
//   We don't find a correct weight equation for 
//   this case yet.
//   DONT USE IT UNLESS YOU SIMULATE OLD SIM! 
//
// StaticParams::gINTPOS_SAMPLE_OPT = 1:
//   This option samples interaction position linearly in column_depth[g/m2].
//   
// StaticParams::gINTERACT_WEIGHT_OPT = 0:
//   This option activate legacy nugen weight calculation.
//     1) calculate interaction weight in approximation form
//     2) multiply interaction weight and position weight exp(-sigma*column_depth/PMASS)
//        in order to compensate linear sampling of interaction position.
//        the position weight is NOT normalized 
//        (thus final interaction weight is always smaller than 1))
//
// StaticParams::gINTERACT_WEIGHT_OPT = 1:
//   This option activate updated weight 
//     1) calculate interaction weight in UHE compatible form
//     2) multiply interaction weight and position weight exp(-sigma*total_column_depth/PMASS)
//        in order to compensate linear sampling of interaction position.
//        the position weight is normalized.
//  
// StaticParams::gCROSSSECTIONxCOLUMNDEPTH_OPT = 0 or 1 :
//   Currently we will not see any difference between these two options
//   because we use same cross section tables for proton and neutron.
//   In future, we may use separated tables and then these option may affect
//   your results.
//

  log_trace("particle type %s", particle_ptr->GetTypeString().c_str());

  if (!particle_ptr->IsNeutrino()) {
     log_fatal("This is not neutrino"); 
     return;
  }

  //--------------------------------
  // get detection volume geometry
  //--------------------------------

  InteractionGeo &intgeo = particle_ptr->GetInteractionGeoRef();
  double max_length     = intgeo.GetMaxLength();
  I3Position startPos   = intgeo.GetEnterDetVolDC();
  I3Position endPos     = intgeo.GetExitDetVolDC();
  I3Position startPosCE(earthservice_ptr_->GetEarthCoordPosFromDetCoordPos(startPos));
  I3Position endPosCE(earthservice_ptr_->GetEarthCoordPosFromDetCoordPos(endPos));

  I3Position produced_position(particle_ptr->GetPos());
  log_trace("This particle is produced at (%f,%f,%f)",
             produced_position.GetX(), produced_position.GetY(),
             produced_position.GetZ());

  log_trace("EnterDetVolDC (%f,%f,%f)",
             startPos.GetX(), startPos.GetY(), startPos.GetZ());
  log_trace("ExitDetVolDC (%f,%f,%f)",
             endPos.GetX(), endPos.GetY(), endPos.GetZ());
  
  //--------------------------------
  // following params need to be 
  // obtained for weight calculation
  //--------------------------------

  double length_in_volume = -1;
  double length_propagated = -1;
  I3Position interaction_positionCE;

  double total_crosssection = -1;
  double column_depth = -1; 
  double total_column_depth = -1;
  double product_total = -1;
  double product_interaction = -1;
  double probability = -1; 
  double exp_factor_tot = -1; 
  double exp_factor_interaction = -1; 
  double psurv = -1;
  double pint = -1;
  double sum = -1;
  double norm = -1;
  double intpos_weight = -1;

  // number of target with unit conversion parameter
  // number of nucleons per 1 gram : 1/PMASS
  // unit conversion parameter :
  // mb * m * g/m3 /g => mb/m2 = 10-27cm2/10^4cm2 => 10^-31 
  double number_of_targets =  1.0e-31 / Constants::PMASS;

  I3Direction dir(particle_ptr->GetDir());

  //-------------------------------
  // 1. Sample interaction position
  //-------------------------------
    
  if (StaticParams::gINTPOS_SAMPLE_OPT == 0) {
     //
     // A: legacy style (length linear sampling)
     //
     // choose interaction position linearly in length
     // we will apply weight later
     //

     length_in_volume = random_->Uniform(0, max_length); 

  } else if  (StaticParams::gINTPOS_SAMPLE_OPT == 1) {
     //
     // B: gram (columndepth) linear sampling
     //
     // choose interaction position linearly in column depth
     // we will apply weight later
     //

     // get total column depth within detection volume (g/m3)*m = g/m2
     total_column_depth = 
        interaction_ptr_->CalculateTotalColumnDepth(
                             startPosCE, //enter to detection vol
                             endPosCE,   //exit from detection vol
                             StaticParams::gNSTEPS_FINAL, 
                             StaticParams::gCROSSSECTIONxCOLUMNDEPTH_OPT_STEPSIZE, // maximum step size
                             particle_ptr);

     // then find a random columndepth
     column_depth = random_->Uniform(0, total_column_depth); 

     // then find a position
     length_in_volume = 
        interaction_ptr_->ConvertColumnDepthToMeter(column_depth);

  }

  //-------------------------------
  // 2. calculate interaction position
  //-------------------------------

  I3Position interaction_position = startPos + dir * length_in_volume; 

  length_propagated = (interaction_position - produced_position).Magnitude();    

  log_trace(" Interaction position DC is chosen (%.4f, %.4f, %.4f)[m]",
             interaction_position.GetX(),interaction_position.GetY(),
             interaction_position.GetZ());

  // interaction position in Earth center coodinate
  interaction_positionCE = earthservice_ptr_->GetEarthCoordPosFromDetCoordPos(interaction_position);

  // get total crosssection at the interaction position
  // return unit is [mb]
  total_crosssection = interaction_ptr_->GetTotalCrosssection(particle_ptr, interaction_positionCE);

  if (total_crosssection <= 0) {
     log_info("Crosssection is zero, out of range tables? partice energy %f[GeV], interaction pos(%f,%f,%f)", particle_ptr->GetEnergy(), interaction_position.GetX(), interaction_position.GetY(), interaction_position.GetZ());
  } 
  log_trace("Total Crosssection: log{sigma [mb]} = %.14f", 
             log10(total_crosssection)); 


  //-------------------------------
  // 3. calculate product and product_interaction
  //    (cross section * column depth)
  //-------------------------------

  if (StaticParams::gCROSSSECTIONxCOLUMNDEPTH_OPT == 0) {

     //
     // A: legacy style
     //
    
     if (StaticParams::gINTPOS_SAMPLE_OPT == 0) {
        // need to calculate column depth and total column depth
        // in simple form
        // columndepth is in (g/m3)*m = g/m2
        column_depth  = 
          interaction_ptr_->CalculateTotalColumnDepth(startPosCE, 
                                     interaction_positionCE, 
                                     StaticParams::gNSTEPS_FINAL,
                                     StaticParams::gFINE_STEP_SIZE*10);


        // get total column depth within detection volume 
        // (g/m3)*m = g/m2
        total_column_depth = 
           interaction_ptr_->CalculateTotalColumnDepth(startPosCE,
                                     endPosCE, 
                                     StaticParams::gNSTEPS_FINAL,
                                     StaticParams::gFINE_STEP_SIZE*10);
     }

     // parameters for interaction position weight
     product_total       = total_crosssection * total_column_depth;
     product_interaction = total_crosssection * column_depth;

  } else if (StaticParams::gCROSSSECTIONxCOLUMNDEPTH_OPT == 1) {

     //
     // B: calculate columndepth * crosssection at each step and
     //    accumulate them upto total column depth
     //
     // ColumnDepth is in (g/m3)*m = g/m2
     //

     if (StaticParams::gINTPOS_SAMPLE_OPT == 0) {

        // calculate get total column depth
        total_column_depth = 
        interaction_ptr_->CalculateTotalColumnDepth(
                             startPosCE, //enter to detection vol
                             endPosCE,   //exit from detection vol
                             StaticParams::gNSTEPS_FINAL, 
                             StaticParams::gCROSSSECTIONxCOLUMNDEPTH_OPT_STEPSIZE, // maximum step size
                             particle_ptr);

        // then get column depth at chosen length_in_volume
        column_depth = 
           interaction_ptr_->ConvertMeterToColumnDepth(length_in_volume);
     }

     // obtain sum(total_crosssection*columndepth_dx) for total column depth
     product_total = interaction_ptr_->GetCrosssectionColumndepthAt(total_column_depth);

     // obtain sum(total_crosssection*columndepth_dx) for column_depth
     product_interaction = interaction_ptr_->GetCrosssectionColumndepthAt(column_depth);
  }

  //-------------------------------
  // 4. Construct weight equation
  //-------------------------------

  if (StaticParams::gINTERACT_WEIGHT_OPT == 0) {
     //
     // A: legacy weight
     //
     // it uses low energy approximation
     // psurv = exp(- (total_crosssection * total_column_depth * 1.0e-31 / PMASS));
     //       ~= 1 - (total_crosssection*total_column_depth*1.0e-31/ PMASS)
     // pint  = 1 - psurv 
     //       ~= (total_crosssection*total_column_depth*1.0e-31/ PMASS) 
     //

     exp_factor_tot = product_total * number_of_targets;
     exp_factor_interaction = product_interaction * number_of_targets;
     intpos_weight = exp(-exp_factor_interaction);
     //probability = (total_crosssection * 1.0e-31) *
     //              (total_column_depth / PMASS) *
     //              intpos_weight;
     probability = product_total * number_of_targets * intpos_weight;

  } else if (StaticParams::gINTERACT_WEIGHT_OPT == 1) {

     //
     // B: UHE compatible form
     //
     // Use UHE compatible form if exponential factor > 1e-5.
     // In order to avoid round-off error, use approximation form 
     // for exp_factor < 1e-5.
     // 
     // also normalization for interaction position weight is applied.
     // Since interaction weight is applied separately, the average of 
     // interaction position weight must be 1.0.
     //
     // [1) get integral of interaction position weight curve(surviving probability)]
     //
     //          total_column_depth
     // sum    = intg ( exp (-total_crosssection*number_of_targets*X)) dX
     //          0
     //        = (1/(total_crosssection*number_of_targets))*(1 - exp(-total_crosssection*number_of_targets*total_column_depth))
     //        = (1/(total_crosssection*number_of_targets)) * pint
     //
     // then, normalized position weight curve will be
     // 1/sum*exp(-total_crosssection*number_of_targets*X)
     // 
     //
     // [2) get normalization of position weight curve]
     //
     // We assume original position sample as 1/(total_column_depth),
     // which integral from 0 to total_column_depth is 1.
     // sampling probability at X is then always 1/(total_column_depth).
     //
     // We want to change the flat sample to exponential curve.
     // Then interaction position weight should be 
     // (1/sum*exp(-total_crosssection*number_of_targets*X)) / (1/total_column_depth)
     // => (total_column_depth / sum) * exp(-total_crosssection*number_of_targets*X)
     // => norm * exp(-total_crosssection*number_of_targets*X)
     //  
     // CAUTION : this normalization assumes same cross section for ice 
     //           target and rock target, but cross sections for ice 
     //           target may be 4% larger than rock target at low energy.
     //

     // surviving probability
     // double psurv = exp(- (total_crosssection * total_column_depth 
     //                         * 1.0e-31 / PMASS));

     // get approximation form, 
     // see StaticParams::gINTERACT_WEIGHT_OPT == 1
     exp_factor_tot = product_total * number_of_targets;
     pint = exp_factor_tot;

     // If exponential factor is greater than gEXPTERM_THRESHOLD, 
     // use UHE compatible form
     if (exp_factor_tot > StaticParams::gEXPTERM_THRESHOLD) {
        psurv = exp(- exp_factor_tot); 
        // interaction probability
        pint  = 1. - psurv;
     }

     // normalization for interaction position weight
     sum  = pint / (total_crosssection * number_of_targets);
     norm = total_column_depth / sum;

     // get position weight curve
     exp_factor_interaction = product_interaction * number_of_targets;
     intpos_weight = norm * exp(-exp_factor_interaction);

     // final probability (interaction weight)
     // actually, because intpos_weight have term 1/pint,
     // it's just simple equation
     // p = total_cdep * total_crosssec * ntargets / pint 
     //     * exp(-exp_factor) * pint
     //   = total_cdep * total_cressec * ntargets * exp(-exp_factor)
     // but here I just multiply pint and intpos_weight 
     // because it's easy to understand.
     probability  = pint * intpos_weight;

  }

  //
  // Done!
  // probability(TotalInteractionProbability)
  // may exceed 1.0 due to the intpos_weight.
  // (we artificially sampled interaction position
  // with flat distribution ). 
  // It may happen when primary energy is high.
  // 

  log_debug("*************************************");
  log_debug("* Final Interaction Weight Info      ");
  log_debug("*------------------------------------");
  log_debug("   ||log10 energy is %.2f , zenith is %.5f degree", log10(particle_ptr->GetEnergy()), dir.CalcTheta()*I3Units::degree);
  log_debug("   ||length_in_volume is %.5f out of %.2f max length", 
            length_in_volume, max_length);
  log_debug("   ||Total Column depth = %.10f [g/m2] over the max_length %f [m]", total_column_depth, max_length);
  log_debug("   ||Column depth = %.10f [g/m2] over the length_in_volume %f [m]", column_depth, length_in_volume);
  log_debug("   ||total crosssection %.2f x10^-12 [mb] at LogE %g", total_crosssection*pow(10.,12.), log10(particle_ptr->GetEnergy()));
  log_debug("   ||exponential factor total %g ", exp_factor_tot);

  log_debug("   ||exponential factor interaction %g ", exp_factor_interaction);
  log_debug("   ||psurv %g ", psurv);
  log_debug("   ||pint %g ", pint);
  log_debug("   ||sum %g ", sum);
  log_debug("   ||norm %g ", norm);
  log_debug("   ||intpos_weight %g ", intpos_weight);
  log_debug("   ||Total interaction probabiliby weight = %.5f x10^-6", probability*pow(10., 6.));		


  //-------------------------------
  // 5. fill information to the particle
  //-------------------------------

  /* creation point to interaction point [m]*/
  particle_ptr->SetLength(length_propagated);

  //fill NuG additional info - 9 info

  InteractionInfo &intinfo = particle_ptr->GetInteractionInfoRef();

  /* sum of possible crosssections at the energy 
     from in [mb] to [cm2]*/
  intinfo.SetTotalXsecCGS(total_crosssection*1e-27);	

  /* corresponds to the interaction weight [dimemsionless]*/
  intinfo.SetInteractionWeight(pint);
  intinfo.SetInteractionPositionWeight(intpos_weight);
	
  /* column depth from the entrance to the generator cylinder 
     to the interaction position from [g/m2] to [g/cm2]*/
  intinfo.SetInteractionColumnDepthCGS(column_depth*1e-4);

  /* column depth from the entrance of the cylinder to the 
     end of cylinder from [g/m2] to [g/cm2]*/
  intinfo.SetTotalColumnDepthCGS(total_column_depth*1e-4);

  /* length between the entrance of cylder to the interaction position [m]*/
  intinfo.SetLengthInVolume(length_in_volume);

  /* */
  intinfo.SetEnergyLost(0.0);

  //two more to be filled in the next function (MakeFinalInteraction) are
  //SetInteractionType(InteractionType type);
  //SetInteractionCrosssection(double cross);
}

//__________________________________________________________________
void I3NuGWeightedEvent::PrintEvent()
{
  log_trace("== Begin Print Event ==");
  log_trace("total number of in-earth particles %zu", 
                     GetInEarthPropagatingParticleList().size());
  log_trace("total number of in-earth daughters %d", 
                     GetTotalNumberOfInEarthDaughters());
  if (GetInIceParticle()) {
     log_trace("total number of in-ice particles %d (must be 1)", 1);
     log_trace("total number of in-ice daughters %d", 
                GetInIceParticle()->GetTotalNDaughters());
  } else {
     log_trace("NO in-ice particle found");
  }
  log_trace("== End Print Event ==");
}

//__________________________________________________________________
void I3NuGWeightedEvent::CheckFilledMCWeights()
{
  I3MapStringDoublePtr weight_ptr = GetMCWeightDictPtr(); 
  log_debug("Got %zu weights in this event",weight_ptr->size());
  I3MapStringDouble::const_iterator iter;

  for(iter = weight_ptr->begin();iter!=weight_ptr->end();++iter){
    log_trace("weight name  %s \t\t value %g", (iter->first).c_str(), iter->second);
  }  
}

//__________________________________________________________________
int I3NuGWeightedEvent::GetTotalNumberOfInEarthDaughters()
{
  ParticlePtrList&  list = GetInEarthPropagatingParticleList();
  int NDaughters=0;

  for (ParticlePtrList::iterator this_itr = list.begin();
       this_itr != list.end(); ++this_itr) {
     NDaughters += (*this_itr)->GetDaughters().size();
  }

  return NDaughters;
}

//__________________________________________________________________
double I3NuGWeightedEvent::CalculateMaximumTotalInteractionProbability(ParticlePtr particle_ptr)
{

  log_trace("CALCULATE interaction position probability for this particle");
  log_trace("particle type %d is %s", particle_ptr->GetType(), 
                                      particle_ptr->GetTypeString().c_str());

  if (!particle_ptr->IsNeutrino()) {
     log_fatal("This is not neutrino"); 
     return 0.0;
  }
  InteractionGeo &intgeo = particle_ptr->GetInteractionGeoRef();
  I3Position startPos   = intgeo.GetEnterDetVolDC();
  I3Position endPos     = intgeo.GetExitDetVolDC();
  I3Position startPosCE(earthservice_ptr_->GetEarthCoordPosFromDetCoordPos(startPos));
  I3Position endPosCE(earthservice_ptr_->GetEarthCoordPosFromDetCoordPos(endPos));

  log_trace("   || Entrance volume position DC  (%.2f, %.2f, %.2f)[m]", startPos.GetX(),startPos.GetY(),startPos.GetZ());
  log_trace("   || End      exit   position DC  (%.2f, %.2f, %.2f)[m]", endPos.GetX(),endPos.GetY(),endPos.GetZ());
  log_trace("--------------------------------------------------------");  
  log_trace("   || Entrance volume position CE  (%.2f, %.2f, %.2f)[m]", startPosCE.GetX(),startPosCE.GetY(),startPosCE.GetZ());
  log_trace("   || End      exit   position CE  (%.2f, %.2f, %.2f)[m]", endPosCE.GetX(),endPosCE.GetY(),endPosCE.GetZ());
  log_trace("--------------------------------------------------------");  
  log_trace("   || Over            range         %f [m]",               intgeo.GetMaxLength());

  //
  // option:
  // choose how to get crosssection * columndepth
  //
  double total_column_depth = -1;
  double exponential_factor = -1;

  if (StaticParams::gCROSSSECTIONxCOLUMNDEPTH_OPT == 0) {
     //
     // A: legacy style
     //

     total_column_depth = 
          interaction_ptr_->CalculateTotalColumnDepth(startPosCE, 
                                     endPosCE, 
                                     StaticParams::gNSTEPS_FINAL,
                                     StaticParams::gFINE_STEP_SIZE*10);
 
     log_trace("   ||Total Column depth = %.10f [g/m2] over the range %f [m]", total_column_depth, intgeo.GetMaxLength());

     // get total crosssection at the interaction position
     double total_crosssection = interaction_ptr_->GetTotalCrosssection(particle_ptr, startPosCE);

     exponential_factor = (total_crosssection * 1.0e-31) * 
                          (total_column_depth / Constants::PMASS);

  } else if (StaticParams::gCROSSSECTIONxCOLUMNDEPTH_OPT == 1) {
     //
     // B: calculate total column depth and interaction probability
     // with fine step. With particle pointer, it also calculates
     // accumulated value of (total crosssection * column depth)
     //
     // ColumnDepth is in (g/m3)*m = g/m2

     total_column_depth = 
           interaction_ptr_->CalculateTotalColumnDepth(startPosCE, 
                                      endPosCE, 
                                      StaticParams::gNSTEPS_FINAL, 
                                      StaticParams::gCROSSSECTIONxCOLUMNDEPTH_OPT_STEPSIZE,
                                      particle_ptr);
     log_trace("   ||Total Column depth = %.10f [g/m2] over the range %f [m]", total_column_depth, intgeo.GetMaxLength());

     // now interaction info knows totalcrosssection*columndepth
     double product = interaction_ptr_->GetCrosssectionColumndepthAt(total_column_depth);
     exponential_factor = product* 1.0e-31 / Constants::PMASS;

  }

  // this is good approximation up to UHE...
  double probability = exponential_factor;

  // but we switch to UHE compatible form 
  // unless expfactor is tiny.
  if (exponential_factor > StaticParams::gEXPTERM_THRESHOLD) {
     // here is UHE form
     probability = 1. - exp(- exponential_factor);
  }

  log_trace("   ||Max total-interaction probabiliby = %.5f x10^-6", probability*pow(10., 6.));		

  if (probability > 1) {
     log_fatal("Hmm...this probability shouldn't exceed 1. Check code. probability = %f", probability); 
  }

  return probability;
}
 
