/**
 *
 *  Copyright  (C) 2005
 *  The IceCube collaboration
 *  $Id: $
 *  @file I3NuGEvent.cxx
 *  @version $Revision:  $
 *  @date    $Date:      $
 *  @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 *  @brief I3NuGEvent class implementation file, 
 *  the main class for neutrino-generator program 
 */
/////////////////////////////////////////////////////
#include "dataclasses/I3Constants.h"
#include "neutrino-generator/Steering.h"
#include "neutrino-generator/Particle.h"
#include "neutrino-generator/legacy/I3NuGEvent.h"
#include "neutrino-generator/legacy/I3NuGInteractionInfo.h"
#include "neutrino-generator/legacy/StaticParams.h"
#include "neutrino-generator/utils/Constants.h"
#include "neutrino-generator/utils/EnumTypeDefs.h"
#include "neutrino-generator/utils/Calculator.h"
#include "neutrino-generator/utils/Utils.h"
#include "phys-services/I3RandomService.h"
#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/I3Map.h"

using namespace std;
using namespace nugen;
using namespace Constants;
using namespace earthmodel;

/////////////////////////////////////////////////////
static const int    NSTEPS      = 10000;
/////////////////////////////////////////////////////
I3NuGEvent::I3NuGEvent(SteeringPtr steer, 
                       I3NuGInteractionInfoPtr inter_p,
		       I3RandomServicePtr random):
  I3NuGVEvent(),
  random_(random), 
  interaction_ptr_(inter_p)
{
  log_trace("I3NuGEvent constructed");
  propagating_list_.clear(); //ParticlePtrList
  buffer_list_.clear();  //ParticlePtrList

  steer_ptr_ = steer;
  earthservice_ptr_ = steer_ptr_->GetEarthModelService();
}
/////////////////////////////////////////////////////
I3NuGEvent::~I3NuGEvent(){
}

/////////////////////////////////////////////////////
bool I3NuGEvent::MakeNeutrinoPropagation(ParticlePtr injected_ptr,
                                         bool skipFinalInteraction)
{

  log_debug("Make a neutrino propagation!");
  //First of all push_back incident neutrino to the propagation particle
  GetInEarthPropagatingParticleList().push_back(injected_ptr);
  
  ///////////////////////////////////////
  // having non-zero neutrino in in-earth vector, propagate the neutrino
  // which fills daughters if interacted
  //
  // Possible output from this function is that initial_neu is not changed but any daughters if interacted
  // if no daughter is created it means that it propagated upto the end side of detection volume
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
  // Step 1: propagate initial neutrino only when GetInjectOnCylinder() returns false.
  // If injectOnCylinder is true, we start propagation from 
  // front side of detection volume and no InEarth propagation is performed.
  // 

  if (! (steer_ptr_->GetSimMode() == FINALONLY)) {
    initial_interacted = PropagateNeutrino(initial_neu_ptr);
  }

  //
  // Step 2: Check whether an interaction occurred or not.
  // If no interaction happened, the primary neutrino reached to a front side of 
  // detection volume then we make force interaction within the detection volume.
  // (call MakeFinalInteraction)
  // 

  if( (!initial_interacted) ){
    log_trace("initial_primary is not interacted, fill final interaction with probability");

    if (skipFinalInteraction) {
       initial_neu_ptr->SetLocationType(I3Particle::InIce);
       finalInteractionNuCandidates_.push_back(initial_neu_ptr);
       return true;
    }

    //then this time initial_ptr and primary are the same 
    MakeFinalInteraction(initial_neu_ptr); 
    //in-ice must be filled by now
    GetInIceParticle()->CheckParticle();
    
    log_trace("    --- Final Interaction finished with %zu particles ---", 
             (size_t)GetInIceParticle()->GetTotalNDaughters());
    
    return true; //and done!
  }

  //
  // Step 3: At this stage, our initial neutrino should have interacted. Then: 
  //  - select particles which need to be propagated in Step 4(e.g. neutrions, taus)
  //    and push_back them to InEarth particle vector
  //  - select neutrinos which reached to the detection volume 
  //    and push_back them to InIce particle vector
  // 

  // sanitary check
  if(!initial_neu_ptr->InteractedOrDecayed()){
    log_error("CHECK!!  initial_primary interacted, but no secondary created");
    return false;
  }
  log_trace("      initial_primary interacted, secondary created");

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
  while(GetInEarthPropagatingParticleList().size() > number_propagated){

    
    ParticlePtr thisPart = GetInEarthPropagatingParticleList()[number_propagated];
    log_trace("------ Looking at %d th particle to be propagated, out of %zu particles ------", 
	      number_propagated+1, GetInEarthPropagatingParticleList().size());
    
    I3Particle::ParticleType ttype = thisPart->GetType();
      
    if(ttype==I3Particle::TauMinus||ttype==I3Particle::TauPlus){
      log_trace("This particle is tau. Let it decay.");
      //tau always decays but it is possible it looses all of thier energy before producing daughters
      MakeTauDecay(thisPart);

      if(thisPart->InteractedOrDecayed()){ 
        log_trace(" tau decayed, secondary created");
        CheckAndPushBack_Daughters(thisPart);
      }
      
    }else if(ttype == I3Particle::NuE || ttype == I3Particle::NuEBar
	     ||ttype == I3Particle::NuMu || ttype == I3Particle::NuMuBar
	     ||ttype == I3Particle::NuTau || ttype == I3Particle::NuTauBar){
      
      log_trace("This particle is neutrino. Let it propagate.");
      //thisPart->SetExitPosition();
      bool this_interacted = PropagateNeutrino(thisPart);
	
      if( this_interacted && thisPart->InteractedOrDecayed() ){      
        log_trace("This particle interacted, secondary created");
        CheckAndPushBack_Daughters(thisPart);
      }

    }else{
      log_trace("This particle is not propagated");

    }//particle type if

    ++number_propagated;

  }//while propagation loop

  ///////////////////Main propagation ends here///////////////////////
  log_trace("Finish filling propagating particles(in-earth), %d particles propagated", number_propagated);
  
  //
  // Step 5:
  // Find neutrinos (and taus?) which didn't interacted within the propagation loop
  // and select them as possible candidates of final "parent" particles
  // (the "parent" particle should generate visible daughter particles for IceCube )
  //
  int n_in_earth = GetInEarthPropagatingParticleList().size();
  log_trace("Number of in-Earth %d", n_in_earth);
  
  for(int i=0; i< n_in_earth; ++i){
    ParticlePtr currentPart = GetInEarthPropagatingParticleList()[i];
    I3Particle::ParticleType ctype= currentPart->GetType();

    if(!currentPart->InteractedOrDecayed()){       
      
      if(ctype == I3Particle::NuE || ctype == I3Particle::NuEBar
        ||ctype == I3Particle::NuMu || ctype == I3Particle::NuMuBar
        ||ctype == I3Particle::NuTau || ctype == I3Particle::NuTauBar
        //||ctype == I3Particle::TauMinus || ctype == I3Particle::TauPlus){
        ){
        log_trace("this is candidate for final interaction neutrino");
        finalInteractionNuCandidates_.push_back(currentPart);
      } else {
        log_trace("This particle has been interacted,  not candidate");
      }
      
    } 
    log_trace("This particle is already interacted");

  }//all in earth particle loop
  
  //
  // Step 6:
  // Here we choose only one primary among all "InIce" parent candidates.
  // It is true that one incident neutrino may generate multiple "InIce" neutrinos,
  // and there is non-zero possibility to generate multiple visible particles (e.g. mu, tau)
  // from each InIce neutrinos. With this case, we may observe multiple tracks
  // within IceCube.
  // However, this probability is very tiny and what mostly we observe is only one
  // track (or cascade) from one neutrino.
  // Since we force to interact within detection volume, making final interaction
  // for all InIce neutrino candidate results in making too much multi-track events.
  // In order to avoid the bias to this kind of special event, we randomly select
  // only one of "InIce" parent candidate and make it final interaction.
  //
 
  if(finalInteractionNuCandidates_.size()){

    log_trace("got %zu possible primaries.", finalInteractionNuCandidates_.size());

    if (skipFinalInteraction) {
      for (unsigned int i=0; i<finalInteractionNuCandidates_.size(); ++i) {
           finalInteractionNuCandidates_[i]->SetLocationType(I3Particle::InIce);
        }
      return true;
    }

    // Inside ChooseWhichNeutrinoToInteracts, FinalParticleVect will be filled
    ParticlePtr final_inice = ChooseNeutrinoToInteractFromBuffer();

    SetInIceParticle(final_inice);
    MakeFinalInteraction(final_inice);
    return true;

  }

  log_trace("NO PossibleParentFinalParticleVect.");	 
  log_trace("Initial Primary is CC interacted and its daughters are attenuated.");
  return false;

}
/////////////////////////////////////////////////////
bool I3NuGEvent::PropagateNeutrino(ParticlePtr particle_ptr){

  if(particle_ptr->InteractedOrDecayed()) {
    log_error("propagation requested for neutrino already interacted"); 
    return true;
  }

  //Get start position of this particle
  I3Position StartPosDC(particle_ptr->GetPos());

  //Get intersection point of the particle and front plane of detection volume
  InteractionGeo &intgeo = particle_ptr->GetInteractionGeoRef();
  I3Position EndPosDC(intgeo.GetEnterDetVolDC());

  I3Position StartPosCE(earthservice_ptr_->GetEarthCoordPosFromDetCoordPos(StartPosDC));
  I3Position EndPosCE(earthservice_ptr_->GetEarthCoordPosFromDetCoordPos(EndPosDC));

  I3Direction dir(EndPosCE-StartPosCE);
  //double   theta = dir.Theta();
  //double   phi = dir.Phi() > 0 ? dir.Phi() : 2 * I3Constants::pi + dir.Phi();
  double   PropLength = (StartPosCE-EndPosCE).Magnitude();
  double   dx = PropLength/NSTEPS;

  //particle propagation from production position to the exit position
  log_trace("   -----------------------------------------------------------");
  log_trace("   Propagate Neutrino: start DC(%.2f, %.2f, %.2f)", StartPosDC.GetX(),StartPosDC.GetY(),StartPosDC.GetZ());  
  log_trace("                     :  end  DC(%.2f, %.2f, %.2f)", EndPosDC.GetX(), EndPosDC.GetY(), EndPosDC.GetZ());  
  log_trace("   -----------------------------------------------------------");
  log_trace("   Propagate Neutrino: start CE(%.2f, %.2f, %.2f), %.2f m", 
	    StartPosCE.GetX(),StartPosCE.GetY(),StartPosCE.GetZ(), StartPosCE.Magnitude());  
  log_trace("                     :  end  CE(%.2f, %.2f, %.2f), %.2f m", 
	    EndPosCE.GetX(), EndPosCE.GetY(), EndPosCE.GetZ(), EndPosCE.Magnitude());  
  log_trace("   -----------------------------------------------------------");
  log_trace("    over length %.2f [m] with NStep %d, stepsize %f [m]", PropLength, NSTEPS, dx);
  log_trace("   -----------------------------------------------------------");

  //I3Position localDC(StartPosDC);//current position
  I3Position localCE(StartPosCE);  //current position in CE
  double columnDepthFromStart = 0.0;
  //int    n_daughter = GetTotalNumberOfInEarthDaughters();

  //
  // Start propagation from start position to flont plane of detection Volume
  //
  log_trace("  about to start propagation loop"); 
  for(int istep=0; istep < NSTEPS; ++istep) {

     localCE += dir * dx;  

     //GetTotalCrosssection returns in mb
     //this requires only particle energy and medium at the position(ice or rock)
     double sigtotmb = interaction_ptr_->GetTotalCrosssection(particle_ptr, localCE);
     double density = earthservice_ptr_->GetEarthDensityInCGS(localCE)*1.e6;//g/cm3*1.e6 = g/m3
     columnDepthFromStart += density*dx;
     
     /**
      *
      * density is in g/m3, PMASS is proton mass in g, sigtotmb is in mb, dx is in m
      * -> mb * m * g/m3 /g => mb/m2 = 10-27cm2/10^4cm2 => 10^-31 
      * probability to have interaction in distance dx
      *
      */
      //Interaction probability that this particle at local interacts within a length dx
      double survprob = exp(-sigtotmb * dx * density * 1.0e-31 / PMASS);
      double thisStepInteractionProbability = 1.0 - survprob;
      double temprandom = random_->Uniform(0.0, 1.0);
	   log_trace("sigma %g, dx %g, dens %g,  survprop %g,  IntProb %g, random %g", 
                sigtotmb, dx, density, survprob, thisStepInteractionProbability, temprandom);

     if(thisStepInteractionProbability > temprandom) {
        //Neutrino is interacted !
        localCE += dir * temprandom * dx;
        log_trace("   Interact at %d th step, propagated %.2f [m]", istep, (localCE-StartPosCE).Magnitude());
        log_trace("   At the position CE (%.2f, %.2f, %.2f) [m]", localCE.GetX(), localCE.GetY(), localCE.GetZ());
        particle_ptr->SetLength((localCE-StartPosCE).Magnitude());

        /* column depth from creation position to the interaction position [g/m2]*/
        InteractionInfo &intinfo = particle_ptr->GetInteractionInfoRef();
        // columnDepthFromStart is in g/m2, change it in g/cm2
        intinfo.SetInteractionColumnDepthCGS(columnDepthFromStart*1e-4);
        intinfo.SetInteractionWeight(1.0);
        /* length between the entrance of cylder to the interaction position [m]*/
        intinfo.SetLengthInVolume(1.0);  //ie = NA
        intinfo.SetTotalColumnDepthCGS(1.0);
        /* sum of possible crosssections at the energy
           change it from mb to cm2
         */
        intinfo.SetTotalXsecCGS(sigtotmb*1e-27);	

        log_trace("   Now choose the type of interaction");
        InteractionType int_type = interaction_ptr_->MakeInteraction(particle_ptr);

        log_debug("neutrino is propagating---interaction type is %d", int_type);
        if (int_type==NOINTERACTION) {
           log_error("no neutrino to make interaction --- check!!");
        }
        log_trace("   We have total %zu secondary particles", (size_t)particle_ptr->GetTotalNDaughters());

        // This neutrino interacted. Return now.
        return particle_ptr->InteractedOrDecayed(); 
	    }

  }//end step

  if(!particle_ptr->InteractedOrDecayed())log_trace("   Neutrino did not interacted.");
  if(!particle_ptr->GetTotalNDaughters())log_trace("   No secondary has been created.");

  log_trace("  End of propagate neutrino"); 
  return false;
}

/////////////////////////////////////////////////////
ParticlePtr I3NuGEvent::ChooseNeutrinoToInteractFromBuffer()
{ 

  bool choose_neutrino = true; // for debugging turn to false, so that always
                               // the first neutrino is selected
  unsigned int nNeutrino = 1;
  if(choose_neutrino){
    nNeutrino = finalInteractionNuCandidates_.size();
    log_trace("We have %d possible primary neutrino", nNeutrino);
  }
  
  // if we have only one neutrino that's it.
  if(nNeutrino==1)
  {
    return finalInteractionNuCandidates_[0];
  }

  //-------------------------------------------------------------------
  double sumprobability=0;
  for(unsigned int i=0; i<nNeutrino;++i){
      
      ParticlePtr neu_part =  finalInteractionNuCandidates_[i];
      log_trace("Calculate probability for %d th neutrino", i);
      neu_part->CheckParticle();
      
      double thisProbability = CalculateMaximumTotalInteractionProbability(neu_part);
      
      sumprobability += thisProbability; 
      neu_part->GetInteractionInfoRef().SetInteractionWeight(thisProbability);
    }
  
  double randomprob = random_->Uniform(0, sumprobability);
  log_trace("Sum of all neutrino's probablities %.12f and random probability %.12f", 
	    sumprobability, randomprob);
  
  double sumprob = 0;
  unsigned int ipart   = 0;
  for(;ipart < nNeutrino && sumprob <= randomprob; ++ipart){
    sumprob += finalInteractionNuCandidates_[ipart]->GetInteractionInfoRef().GetInteractionWeight();  
    log_trace("Compare probablities %.12f and random probability %.12f for %d th particle", 
	      sumprob, randomprob, ipart); 
  }
  //-------------------------------------------------------------------
  log_trace("%d th neutrino has been chosen among %d particles", ipart, nNeutrino);
  return finalInteractionNuCandidates_[ipart-1];
}

////////////////////////////////////////////////////////////////
void I3NuGEvent::MakeFinalInteraction(ParticlePtr neu_part)
{
  /**
   * Final particle  
   */
  SetFinalInteractionPosition(neu_part);
  //this requires for particle id-ing. 
  //but should be zero for initial neu
  //int n_daughter = GetTotalNumberOfInEarthDaughters();

  //Decide which of NC/CC/GR interactions takes place?
  //and daughter particle properties
  InteractionType int_type = interaction_ptr_->MakeInteraction(neu_part);

  log_debug("interaction type is %d",int_type);

  if(int_type==NOINTERACTION)log_error("no neutrino to make interaction --- check!!");

  SetInIceParticle(neu_part);
}
/////////////////////////////////////////////////////
bool I3NuGEvent::MakeTauDecay(ParticlePtr tau_ptr)
{ 
  if(tau_ptr->InteractedOrDecayed()) return false;
  if(tau_ptr->GetType() != I3Particle::TauMinus && tau_ptr->GetType() != I3Particle::TauPlus) return false;

  //int n_daughter = GetTotalNumberOfInEarthDaughters();
  //Set Decay Length, Energy at the time of Decay which equals to the sum of daughter energy  
  bool decayed = interaction_ptr_->MakeDecay(tau_ptr);
  //now tau_ptr has daughters
  log_trace("decay daughters %d", tau_ptr->GetTotalNDaughters());
  
  return decayed;
}
/////////////////////////////////////////////////////
void I3NuGEvent::CheckAndPushBack_Daughters(ParticlePtr thisPart){
  int n_daughters=0;
  int n_pushed=0;

  log_trace("Check if this particle is created in volume /or/ Possible in Earth(Nu or Tau)");

      //other wise to propagating neutrino/tau
      for(vector<ParticlePtr>::const_iterator this_daughter_itr = thisPart->GetDaughters().begin();
	  this_daughter_itr!=thisPart->GetDaughters().end();
	  ++this_daughter_itr)
	{ 
	  I3Particle::ParticleType type = (*this_daughter_itr)->GetType();
	  
 	  if(IsStartInExpectedDetectionVolume(**this_daughter_itr)){
	    //if any daughter particle which is within active volume 
	    //(e.g. for muon active volume is larger)
	    //push back the parent to inice
	    log_debug("daughter found in volume: %s", (*this_daughter_itr)->GetTypeString().c_str());

            // Only set parent to InIce if the daughter in the detector volume
            // is something other than a neutrino
            if (type!=I3Particle::NuMu  && type!=I3Particle::NuMuBar  &&
                type!=I3Particle::NuTau && type!=I3Particle::NuTauBar &&
                type!=I3Particle::NuE   && type!=I3Particle::NuEBar) {
	       log_debug("Set parent of daughter: %s to the in-ice particle:", thisPart->GetTypeString().c_str());
	       SetInIceParticle(thisPart);
            }

	    break;
	  } else if( type==I3Particle::NuMu    || type==I3Particle::NuMuBar
		     ||type==I3Particle::NuTau || type==I3Particle::NuTauBar
		     ||type==I3Particle::NuE   || type==I3Particle::NuEBar){
	    
	    ++n_pushed;
	    GetInEarthPropagatingParticleList().push_back(*this_daughter_itr);
	    log_debug("daughter is neutrino, push back to in earth");

	  } else if(type==I3Particle::TauMinus || type==I3Particle::TauPlus){

	    ++n_pushed;
	    GetInEarthPropagatingParticleList().push_back(*this_daughter_itr);
	    log_debug("daughter is tau, push back to in earth");
	  } else { 
	    log_debug("daughter is not neutrino nor in-volume");
	  }
	  
	  ++n_daughters;
	}//daughter loop
      log_trace("%d daughters, %d pushed back to in-earth",  n_daughters, n_pushed);
}
/////////////////////////////////////////////////////
void I3NuGEvent::SetFinalInteractionPosition(ParticlePtr particle_ptr)
{
  const I3Particle::ParticleType particletype = particle_ptr->GetType();
  log_trace("particle type %d is %s", particletype, particle_ptr->GetTypeString().c_str());

  if (!particle_ptr->IsNeutrino()) {
     log_fatal("This is not neutrino"); 
     return;
  }
    
  const double                         energy = particle_ptr->GetEnergy();
  double               detection_range_before = steer_ptr_->GetActiveHeightBefore();
  double                detection_range_after = steer_ptr_->GetActiveHeightAfter();
  I3Direction dir(particle_ptr->GetDir());    

  //obtain relevant positions
  I3Position startPos(particle_ptr->GetPos());
  log_trace("created DC at (%.2f, %.2f, %.2f)[m]", startPos.GetX(),startPos.GetY(),startPos.GetZ());

  InteractionGeo &intgeo = particle_ptr->GetInteractionGeoRef();
  double impact = intgeo.GetImpactParam();

  const double distanceToDCPlaneBefore = sqrt( startPos.Mag2()-impact*impact);

  I3Position DCPlanePos = startPos + dir*distanceToDCPlaneBefore;
  log_trace("Closest Approach to DC at (%.2f, %.2f, %.2f)[m]", DCPlanePos.GetX(), DCPlanePos.GetY(), DCPlanePos.GetZ());

  I3Position endPos = DCPlanePos + dir*detection_range_after;
  log_trace("exit DC at (%.2f, %.2f, %.2f)[m]", endPos.GetX(), endPos.GetY(), endPos.GetZ());

  I3Position endPosCE(earthservice_ptr_->GetEarthCoordPosFromDetCoordPos(endPos)); 
  double earthrad = steer_ptr_->GetWorldRadius();

  if(endPosCE.Magnitude() > earthrad){
    log_trace("bring back end position to the EARTH surface");
    I3Position DCPlanePosCE(earthservice_ptr_->GetEarthCoordPosFromDetCoordPos(DCPlanePos));
      
    double angle      = Calculator::Angle(DCPlanePosCE,dir);
    double Rc         = DCPlanePosCE.Magnitude();
    double delta      = (earthrad -Rc*sin(angle))*(earthrad +Rc*sin(angle));
    double length_max = -Rc*cos(angle)+sqrt(delta);
    endPos=DCPlanePos+dir*length_max;
    log_trace("exit DC at (%.2f, %.2f, %.2f)[m]", endPos.GetX(), endPos.GetY(), endPos.GetZ());
  }


  bool doAutoExtension=false;
  if(steer_ptr_->DoMuonRangeExtension() && 
     (particletype==I3Particle::NuMu  || particletype==I3Particle::NuMuBar ||
      particletype==I3Particle::NuTau || particletype==I3Particle::NuTauBar))doAutoExtension=true;

  if(doAutoExtension){
    log_trace("this is numu or numu bar or nutau or nutau bar event %d",particletype); 
    log_trace("auto extension is enabled for this event");
    double lepton_range = particle_ptr->GetMuonRange();
    if (lepton_range > detection_range_before)
      detection_range_before = lepton_range;
    log_trace("lepton range from parameterization is %f [m], for lepton energy %f [GeV]", 
	      detection_range_before, energy/I3Units::GeV);
  }

  const double bring_forward = distanceToDCPlaneBefore - detection_range_before;
  if(bring_forward<0){log_trace("intersection position is the same as start pos for final interaction calculation.");}
  else{ startPos += dir*bring_forward;
  log_trace("entering DC is to (%.2f, %.2f, %.2f)[m]", startPos.GetX(),startPos.GetY(),startPos.GetZ());
  }

  log_trace("Set Final Interaction Position for particle with flavor %s",particle_ptr->GetTypeString().c_str());
  double    max_length = (startPos-endPos).Magnitude();
  log_trace("will be interacted within the range (before+after) %f [m]", max_length);  
  
  log_trace("   || Entrance volume position   (%.2f, %.2f, %.2f)[m]", startPos.GetX(),startPos.GetY(),startPos.GetZ());
  log_trace("   || End      exit   position   (%.2f, %.2f, %.2f)[m]", endPos.GetX(),endPos.GetY(),endPos.GetZ());
  log_trace("   || Over            range       %f [m]",               max_length);
  
  //double    dx = max_length/NSTEPS;
  I3Position local(startPos);
  
  double length_in_volume = random_->Uniform(0, max_length); 
  I3Position interaction_position = local + dir * length_in_volume; 

  I3Position produced_position(particle_ptr->GetPos());
  double length_propagated = (interaction_position - produced_position).Magnitude();    
  log_trace(" Interaction position DC is chosen (%.4f, %.4f, %.4f) [m]",  
	    interaction_position.GetX(),interaction_position.GetY(),interaction_position.GetZ());
  I3Position interaction_positionCE(earthservice_ptr_->GetEarthCoordPosFromDetCoordPos(interaction_position));

  log_debug("   || %.5f out of %.2f total length", length_in_volume, max_length);

  log_trace("calculate total crosssetion for energy %f [GeV]", energy);
  double total_crosssection = interaction_ptr_->GetTotalCrosssection(particle_ptr, interaction_positionCE);
  //if(total_crosssection)log_trace("Total Crosssection: log{sigma [cm^2]} = %.14f", log10(total_crosssection*1e-27)); 
  if(total_crosssection)log_trace("Total Crosssection: log{sigma [mb]} = %.14f", log10(total_crosssection)); 
  else log_error("Error: Crosssection is zero, out of range tables?");
  //ColumnDepth is in (g/m3)*m = g/m2
  I3Position startPosCE(earthservice_ptr_->GetEarthCoordPosFromDetCoordPos(startPos));
  endPosCE = earthservice_ptr_->GetEarthCoordPosFromDetCoordPos(endPos);
  const double column_depth  = 
    interaction_ptr_->CalculateTotalColumnDepth(startPosCE, interaction_positionCE, NSTEPS);
  //this is just for info for debugging later
  const double total_column_depth = 
    interaction_ptr_->CalculateTotalColumnDepth(startPosCE, endPosCE, NSTEPS);
  log_trace("   ||Total Column depth = %.10f [g/m2] over the range %f [m]", total_column_depth, max_length);
  //
  const double exponential_factor = (total_crosssection * 1.0e-31) * (column_depth / PMASS);
  const double probability        = (total_crosssection * 1.0e-31) * (total_column_depth / PMASS) * exp(-exponential_factor);
  //const double probability        = 1-exp(-exponential_factor);

  //now weight results...
  log_trace("   ||Column depth = %.10f [g/m2] over the length propagated %f [m]", column_depth, length_propagated);
  log_trace("   ||total crosssection %.2f x10^-12 [mb]", total_crosssection*pow(10.,12.));
  log_trace("   ||Total interaction probabiliby weight = %.5f x10^-6", probability*pow(10., 6.));		


  /* creation point to interaction point [m]*/
  particle_ptr->SetLength(length_propagated);

  //fill NeuG additional info - 9 info

  InteractionInfo &intinfo = particle_ptr->GetInteractionInfoRef();

  /* sum of possible crosssections at the energy 
     change it from [mb] to [cm2]*/
  intinfo.SetTotalXsecCGS(total_crosssection*1e-27);	

  /* corresponds to the interaction weight [dimemsionless]*/
  intinfo.SetInteractionWeight(probability);
	
  /* column depth from the entrance to the generator cylinder to 
     the interaction position [g/m2]
     store it in g/cm2*/
  intinfo.SetInteractionColumnDepthCGS(column_depth*1e-4);

  /* column depth from the entrance of the cylinder to the 
    end of cylinder [g/m2]. store it in [g/cm2]*/
  intinfo.SetTotalColumnDepthCGS(total_column_depth*1e-4);

  /* length between the entrance of cylder to the interaction position [m]*/
  intinfo.SetLengthInVolume(length_in_volume);

  /* length between the entrance of cylder to the end of cylinder [m]*/
  //particle_ptr->SetTotalDetectionLength(max_length);

  /* */
  intinfo.SetEnergyLost(0.0);
  //two more to be filled in the next function (MakeFinalInteraction) are
  //SetInteractionType(InteractionType type);
  //SetInteractionCrosssection(double cross);
}
/////////////////////////////////////////////////////////////////////
void I3NuGEvent::PrintEvent(){
  log_trace("== Begin Print Event ==");
  log_trace("total number of in-earth particles %zu", GetInEarthPropagatingParticleList().size());
  log_trace("total number of in-earth daughters %d", GetTotalNumberOfInEarthDaughters());
  if(GetInIceParticle())log_trace("total number of in-ice particles %d (must be 1)", 1);
  else log_trace("NO in-ice particle found");
  if(GetInIceParticle())log_trace("total number of in-ice daughters   %d", GetInIceParticle()->GetTotalNDaughters());
  log_trace("== End Print Event ==");
}
/////////////////////////////////////////////////////////////////////
void I3NuGEvent::CheckFilledMCWeights(){
  I3MapStringDoublePtr weight_ptr = GetMCWeightDictPtr(); 
  log_debug("Got %zu weights in this event",weight_ptr->size());
  I3MapStringDouble::const_iterator iter;

  for(iter = weight_ptr->begin();iter!=weight_ptr->end();++iter){
    log_trace("weight name  %s", (iter->first).c_str());
    log_trace("weight value %f",  iter->second);
  }  
}

///////////////////////////////////////////////////
int I3NuGEvent::GetTotalNumberOfInEarthDaughters(){
  ParticlePtrList&  list = GetInEarthPropagatingParticleList();
  int NDaughters=0;

  for(ParticlePtrList::iterator this_itr = list.begin();
      this_itr != list.end(); ++this_itr) 
    NDaughters += (*this_itr)->GetDaughters().size();

  return NDaughters;
}
///////////////////////////////////////////////////
double I3NuGEvent::CalculateMaximumTotalInteractionProbability(ParticlePtr particle_ptr)
{
  log_trace("CALCULATE interaction position probability for this particle");
  const I3Particle::ParticleType particletype = particle_ptr->GetType();
  log_trace("particle type %d is %s", particletype, particle_ptr->GetTypeString().c_str());

  if (!particle_ptr->IsNeutrino()) {
     log_fatal("This is not neutrino"); 
     return 0.0;
  }
  
  const double energy = particle_ptr->GetEnergy();
  double       detection_range_before = steer_ptr_->GetActiveHeightBefore();
  double       detection_range_after = steer_ptr_->GetActiveHeightAfter();
  I3Direction dir(particle_ptr->GetDir());    

  //obtain relevant positions
  I3Position startPos(particle_ptr->GetPos());
  log_trace("created DC at (%.2f, %.2f, %.2f)[m]", startPos.GetX(),startPos.GetY(),startPos.GetZ());

  const InteractionGeo & intgeo =  particle_ptr->GetInteractionGeoRef();
  double impact = intgeo.GetImpactParam();
  const double distanceToDCPlaneBefore = sqrt( startPos.Mag2()-(impact*impact));

  I3Position DCPlanePos = startPos + dir*distanceToDCPlaneBefore;
  log_trace("Closest Approach to DC at (%.2f, %.2f, %.2f)[m]", DCPlanePos.GetX(), DCPlanePos.GetY(), DCPlanePos.GetZ());

  I3Position endPos = DCPlanePos + dir*detection_range_after;
  I3Position endPosCE(earthservice_ptr_->GetEarthCoordPosFromDetCoordPos(endPos)); 
  log_trace("exit DC at (%.2f, %.2f, %.2f)[m]", endPos.GetX(), endPos.GetY(), endPos.GetZ());

  double earthrad = steer_ptr_->GetWorldRadius();
  if(endPosCE.Magnitude() > earthrad){
    log_trace("bring back end position to the EARTH surface");
    I3Position DCPlanePosCE(earthservice_ptr_->GetEarthCoordPosFromDetCoordPos(DCPlanePos));
      
    double angle      = Calculator::Angle(DCPlanePosCE,dir);
    double Rc         = DCPlanePosCE.Magnitude();
    double delta      = (earthrad -Rc*sin(angle))*(earthrad +Rc*sin(angle));
    double length_max = -Rc*cos(angle)+sqrt(delta);
    endPos=DCPlanePos+dir*length_max;
    log_trace("exit DC at (%.2f, %.2f, %.2f)[m]", endPos.GetX(), endPos.GetY(), endPos.GetZ());
  }

  bool doAutoExtension=false;
  if(steer_ptr_->DoMuonRangeExtension() && 
     (particletype==I3Particle::NuMu  || particletype==I3Particle::NuMuBar ||
      particletype==I3Particle::NuTau || particletype==I3Particle::NuTauBar))doAutoExtension=true;

  if(doAutoExtension){
    log_trace("this is numu or numu bar or nutau or nutau bar event %d",particletype); 
    log_trace("auto extension is enabled for this event");
    double lepton_range = particle_ptr->GetMuonRange();
    if (lepton_range > detection_range_before)
      detection_range_before = lepton_range;
    log_trace("lepton range from parameterization is %f [m], for lepton energy %f [GeV]", 
	      detection_range_before, energy/I3Units::GeV);
  }

  const double bring_forward = distanceToDCPlaneBefore - detection_range_before;
  if(bring_forward<0){log_trace("intersection position is the same as start pos for final interaction calculation.");}
  else{ startPos += dir*bring_forward;
  log_trace("entering DC is to (%.2f, %.2f, %.2f)[m]", startPos.GetX(),startPos.GetY(),startPos.GetZ());
  }

  //log_trace("Set Final Interaction Position for particle with flavor %s",particle_ptr->GetTypeString().c_str());
  double    max_length = 0; 
  max_length = (startPos-endPos).Magnitude();
  log_trace("calculate total probability of interaction within the range (before+after) %f [m]", max_length);  
  
  /**** in CE *****/
  endPosCE = earthservice_ptr_->GetEarthCoordPosFromDetCoordPos(endPos);
  I3Position startPosCE(earthservice_ptr_->GetEarthCoordPosFromDetCoordPos(startPos));

  log_trace("   || Entrance volume position DC  (%.2f, %.2f, %.2f)[m]", startPos.GetX(),startPos.GetY(),startPos.GetZ());
  log_trace("   || End      exit   position DC  (%.2f, %.2f, %.2f)[m]", endPos.GetX(),endPos.GetY(),endPos.GetZ());
  log_trace("--------------------------------------------------------");  
  log_trace("   || Entrance volume position CE  (%.2f, %.2f, %.2f)[m]", startPosCE.GetX(),startPosCE.GetY(),startPosCE.GetZ());
  log_trace("   || End      exit   position CE  (%.2f, %.2f, %.2f)[m]", endPosCE.GetX(),endPosCE.GetY(),endPosCE.GetZ());
  log_trace("--------------------------------------------------------");  
  log_trace("   || Over            range         %f [m]",               max_length);

  
  I3Position local(startPos);
  double total_crosssection = interaction_ptr_->GetTotalCrosssection(particle_ptr, startPosCE);

  //ColumnDepth is in (g/m3)*m = g/m2
  const double total_column_depth = 
    interaction_ptr_->CalculateTotalColumnDepth(startPosCE, endPosCE, NSTEPS);

  log_trace("   ||Total Column depth = %.10f [g/m2] over the range %f [m]", total_column_depth, max_length);
  //
  const double exponential_factor = total_crosssection * total_column_depth * 1.0e-31 / PMASS;
  const double probability        = 1-exp(-exponential_factor);

  log_trace("   ||total crosssection %.2f x10^-12 [mb]", total_crosssection*pow(10.,12.));
  log_trace("   ||Max total-interaction probabiliby = %.5f x10^-6", probability*pow(10., 6.));		

  if(probability > 1) log_fatal("My job is to calculate a probability and apparently I don't know how to do that properly."); 

  return probability;
}
/////////////////////////////////////////////////////////////////////

//_________________________________________________________
/**
 * Check function if this is CREATED=GetPosition() in Detection volume
 * Defined by semi-sphears of radius active length before and after
 *
 * Mar 8 2011 K.Hoshina:
 * This function is only used in I3NuGEvent.cxx, which is flozen and
 * no longer be supported.
 * Since I3NuGEvent.cxx is flozen, this function is also flozen.
 */
bool I3NuGEvent::IsStartInExpectedDetectionVolume(const Particle &p) 
{
  // Aug 26 2010 K.Hoshina
  // following comparison should be done with hight of starting position
  // with respect to detection volume instead of  
  // particle_start_position.Magnitude()...

  //const I3Particle::ParticleType particle_type = GetType(); 
  const I3Direction particle_direction(p.GetDir());
  const I3Position particle_start_position(p.GetPos());

  double length_within = 0.0;
  bool   pos_before    = false;
  
  //at first check if this position is before or after detector center and or muon
  if (particle_direction*particle_start_position < 0 ) pos_before=true;

  if (pos_before) { //before
    
    if (steer_ptr_->DoMuonRangeExtension()&& (p.IsMuon() || p.IsTau())) {
      
      if (steer_ptr_->GetActiveHeightBefore()<p.GetMuonRange()) {
         length_within = p.GetMuonRange();
      } else {
         length_within = steer_ptr_->GetActiveHeightBefore();
      }
      
    } else {
      length_within = steer_ptr_->GetActiveHeightBefore();
    }
    
  } else {//after
    length_within = steer_ptr_->GetActiveHeightAfter();
  }

  if (length_within > particle_start_position.Magnitude()) return true;
  return false;
  
}


 
