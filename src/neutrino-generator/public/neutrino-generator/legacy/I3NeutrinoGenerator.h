#ifndef I3NEUTRINOGENERATOR_H
#define I3NEUTRINOGENERATOR_H
/**
 *  copyright  (C) 2005
 *  the IceCube collaboration
 *  $Id: $
 *
 *  @version $Revision: $
 *  @date    $Date: $
 *  @author  Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 * @brief I3NeutrinoGenerator header file
 * This class fills dataclasses with the event information 
 * obtained from the neutrino-generator simulation based on ANIS
 * 
 */

#include "icetray/I3ConditionalModule.h"
#include "neutrino-generator/legacy/I3NeutrinoGeneratorBase.h"

namespace nugen {
I3_FORWARD_DECLARATION(Steering);
I3_FORWARD_DECLARATION(Particle);
}

class I3NeutrinoGenerator : public I3ConditionalModule, 
                            public I3NeutrinoGeneratorBase
{

 public:
  /**
   *constructor for I3Module
   */
  I3NeutrinoGenerator(const I3Context& ctx);

  virtual ~I3NeutrinoGenerator() {}
  
  void Configure();
  
  void DAQ(I3FramePtr frame);
   
  void Finish();

 private:
  /**
   *private constructors 
   */
  I3NeutrinoGenerator();
  I3NeutrinoGenerator& operator=(const I3NeutrinoGenerator&);

};
typedef boost::shared_ptr<I3NeutrinoGenerator> I3NeutrinoGeneratorPtr;
#endif //I3NEUTRINOGENERATOR_H
