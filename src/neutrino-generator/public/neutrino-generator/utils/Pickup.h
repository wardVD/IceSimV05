#ifndef NuGPickup_H
#define NuGPickup_H
/**
 *  copyright  (C) 2005
 *  the IceCube collaboration
 *  $Id: $
 *
 *  @version $Revision: $
 *  @date    $Date: $
 *  @author  Kotoyo Hoshina <hoshina@icecube.wisc.edu>
 *
 * @brief pickup a neutrino... 
 * I tried to make it a non-class function, but couldn't find a 
 * good solution.
 * To use PickupANeutrino function, inherit the class and implement
 * SelectionWeighter function.
 */

#include "phys-services/I3RandomService.h"
#include "neutrino-generator/utils/EnumTypeDefs.h"

namespace nugen {

I3_FORWARD_DECLARATION(Particle);

class Pickup {

  public:

  Pickup() {}
  virtual ~Pickup() {}

  virtual double SelectionWeighter(boost::shared_ptr<Particle> p) = 0;

  
  /**
   * Interface function
   */
  std::pair<boost::shared_ptr<Particle>, double>
       PickupANeutrino(I3RandomServicePtr rnd,
         std::vector<boost::shared_ptr<Particle> > &particles,
         bool useHE = false);

  /**
   * PickupANeutrinoHE is a function to select a neutrino 
   * from several candidates that takes into account of each neutrino's
   * interaction probability.
   * This function is always correct even if Psurv_notselected (see
   * below) is not nearly equal 1.0.
   * Mostly copied from Jakob's private module (SimpleSourceSelector). 
   *
   * A probability that makes force-interaction for one neutrino 
   * among several neutrinos is
   *
   * P = Pint_selected * Psurv_notselected
   *
   * where Pint_selected is the total interaction probability of the 
   * selected neutrino, and Psurv_notselected is the product of 
   * survival probabilities of all other neutrinos.
   *
   * In other words, selectiong a neutrino(which will be interacted)
   * is same as selecting all other neutrinos that do not interact.
   * Thus, a neutrino is selected in ratio of 
   * Psurv_notselected(for the selected neutrino) / sum(Psurv_notselected)
   *
   * This function also allow to weight selection with a class 
   * member function SelectionWeighter().
   * The function could be, for example, selecting more high-energy 
   * neutrinos than low energy neutrinos.
   * The weight factor is calculated and returned with the selected
   * particle, as pair<ParticlePtr, weight>. 
   */
  std::pair<boost::shared_ptr<Particle>, double>
       PickupANeutrinoHE(I3RandomServicePtr rnd,
                       std::vector<boost::shared_ptr<Particle> > &particles);


  /**
   * PickupANeutrinoLE is a function to select a neutrino 
   * from several candidates randomly with a user-defind 
   * weight-selection.
   * This function is correct as long as Psurv_notselected 
   * is regarded as nearly equal 1.0.
   * This function also allow to weight selection with a class 
   * member function SelectionWeighter().
   * The function could be, for example, selecting more high-energy 
   * neutrinos than low energy neutrinos.
   * The weight factor is calculated and returned with the selected
   * particle, as pair<ParticlePtr, weight>. 
   */

  std::pair<boost::shared_ptr<Particle>, double>
       PickupANeutrinoLE(I3RandomServicePtr rnd,
                       std::vector<boost::shared_ptr<Particle> > &particles);
 

};
}

#endif 
