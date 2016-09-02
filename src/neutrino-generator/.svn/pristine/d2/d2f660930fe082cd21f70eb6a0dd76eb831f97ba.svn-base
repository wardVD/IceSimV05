/**
 *  copyright  (C) 2005
 *  the IceCube collaboration
 *  $Id: $
 *
 *  @version $Revision: $
 *  @date    $Date: $
 *  @author  Kotoyo Hoshina <hoshina@icecube.wisc.edu>
 *
 *  @brief   Calculator IMPLEMENTATION FILE
 */

#include "neutrino-generator/Particle.h"
#include <neutrino-generator/utils/Pickup.h>

namespace nugen {

class Candidate {
   public :
   Candidate(): nu(ParticlePtr()), bias_weight(1.0), survival_weight(1.0){}
   ParticlePtr nu;
   double bias_weight;
   double survival_weight;
};

//_________________________________________________________
std::pair<ParticlePtr, double>
Pickup::PickupANeutrino(I3RandomServicePtr rnd,
                  std::vector<ParticlePtr> &particles, 
                  bool useHE)
{
   if (useHE) {
      return PickupANeutrinoHE(rnd, particles);
   }
   return PickupANeutrinoLE(rnd, particles);
}

//_________________________________________________________
std::pair<ParticlePtr, double>
Pickup::PickupANeutrinoHE(I3RandomServicePtr rnd,
                       std::vector<ParticlePtr> &particles)
{
   std::vector<Candidate> neutrinos;
   double noninteraction_weight = 1.;
   for (unsigned int i=0; i<particles.size(); ++i) {

      ParticlePtr nu = particles[i];

      // this line calculates total surviving prob. etc.
      const DoubleStepMap &steps = nu->GetStepMapRef();

      // get total surviving probability from last entry
      DoubleStepMap::const_iterator ii = steps.end();
      --ii;

      Step laststep = ii->second;
      double psurv = laststep.psurv_out_;

      Candidate c;
      c.nu = nu;
      c.survival_weight = psurv;
      noninteraction_weight *= c.survival_weight;

      neutrinos.push_back(c);
   }

   // select a neutrino

   double total_weight = 0;
   double total_bias_weight = 0;
   double total_survival_weight = 0;
   std::map <double, Candidate> choices;
   for (unsigned int i=0; i<neutrinos.size(); ++i) {

      Candidate &c = neutrinos[i];

      // The weight for selecting a particular neutrino should be proportional
      // to the probability that all the other neutrinos would *not* interact.
      // The probability of the chosen neutrino interacting will be part of
      // its forced-interaction weight.

      // Psurv_notinteracted
      c.survival_weight = noninteraction_weight / c.survival_weight;

      // Bias selection, use class T's function  
      c.bias_weight = SelectionWeighter(c.nu);

      total_weight += c.survival_weight*c.bias_weight;
      total_survival_weight += c.survival_weight;
      total_bias_weight += c.bias_weight;

      choices[total_weight] = c;
   }

  double r = rnd->Uniform(0., total_weight);
  std::map<double, Candidate>::iterator choice = choices.lower_bound(r);
  if (choice == choices.end()) {
     log_fatal("Couldn't find prob %f in (%f %f)]", r, 
                0., total_weight);
  }

  log_trace_stream("Picked " << choice->second.nu->GetEnergy() 
        << " GeV " << choice->second.nu->GetTypeString() 
        << " to interact");

  // The weight associated with picking exactly one neutrino to interact is
  // the ratio between the natural frequency (survival_weight/total_survival_weight)
  // and the weighted frequency (survival_weight*bias_weight/total_weight)
  // = total_weight / (total_suvival_weight * bias_weight)

  double selection_weight = total_weight / 
     (choice->second.bias_weight*total_survival_weight);
  log_trace_stream("psurv/total " 
        << (choice->second.survival_weight / total_survival_weight)
        << " bias/total " << (choice->second.bias_weight / total_bias_weight)
        << " selection weight " <<  selection_weight);

  std::pair<ParticlePtr, double> apair(choice->second.nu, selection_weight);

  return apair;

}

//_________________________________________________________
std::pair<ParticlePtr, double>
Pickup::PickupANeutrinoLE(I3RandomServicePtr rnd,
                       std::vector<ParticlePtr> &particles)
{
  std::vector<Candidate> neutrinos;
  std::map <double, Candidate> choices;
  double total_bias_weight = 0;
  for (unsigned int i=0; i<particles.size(); ++i) {
     Candidate c;
     c.nu = particles[i]; 
     c.bias_weight = SelectionWeighter(c.nu);
     total_bias_weight += c.bias_weight;
     choices[total_bias_weight] = c;
  }

  double r = rnd->Uniform(0., total_bias_weight);
  std::map<double, Candidate>::iterator choice = choices.lower_bound(r);
  if (choice == choices.end()) {
     log_fatal("Couldn't find prob %f in (%f %f)]", r, 
                0., total_bias_weight);
  }

  log_trace_stream("Picked " << choice->second.nu->GetEnergy() 
        << " GeV " << choice->second.nu->GetTypeString() 
        << " to interact");

  // The weight associated with picking is apploximatery
  // (1/N) / (bias / total_bias_weight)
  // = total_bias_weight / (total_bias_weight * N)
  // this is correct as long as we regards product of surviving
  // probability of all NOT SELECTED neutrinos (Psurv_notselected)
  // is nearly equal 1.0.

  unsigned int nNu = choices.size();
  double selection_weight = total_bias_weight / 
                            (choice->second.bias_weight * nNu);

  std::pair<ParticlePtr, double> apair(choice->second.nu, selection_weight);

  return apair;

}

}

