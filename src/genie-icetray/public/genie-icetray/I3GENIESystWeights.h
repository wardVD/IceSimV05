/**
 * Helper class for calculation of event weights based on genie
 * systematic uncertainties.
 *
 * Writted By: dadwyer@lbl.gov 2014/04/14
 * Last Updated: dadwyer@lbl.gov 2014/04/14
 */

#ifndef I3GENIESYSTWEIGHTS_H_INCLUDED
#define I3GENIESYSTWEIGHTS_H_INCLUDED

#include "EVGCore/EventRecord.h"
#include "genie-icetray/I3GENIEResultDict.h"
#include "ReWeight/GReWeight.h"

namespace genie { 
    
  class I3GENIESystWeights
  {
  public:
    I3GENIESystWeights();
    ~I3GENIESystWeights();
    
    // Configure the systematic weight calculator
    void Configure();
    
    // Calculate weights for one event, and add weights to genie result
    void Calculate(const genie::EventRecord &event, 
		   I3GENIEResultDict &gstMap);

    // Set the list of systematics to calculate, by name (See GENIE GSyst.h)
    void SetSystematics(const std::vector<std::string>& systNames);

    // Set the syst steps to calculate, in units of sigma (e.g. [0,-2,-1,1,2]) 
    void SetSteps(const std::vector<double>& systSteps);

  private:
    std::vector<std::string> systNames_; // List of systematic parameters
    std::vector<double> systSteps_; // Steps values for tweaking systematics
    std::map<std::string, std::vector< boost::shared_ptr<genie::rew::GReWeight> > > systCalculators_; // References to systematic weight calculators
  };
  
};

#endif // I3GENIESYSTWEIGHTS_H_INCLUDED
