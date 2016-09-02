#ifndef CREATEFILTERMASK_H_INCLUDED
#define CREATEFILTERMASK_H_INCLUDED
/** 
 * @author Kevin Meagher
 *
 * @brief This module creates an I3FilterResultMap based on the I3Bools in the frame
 *
 * It assumes that every I3Bool in the frame is a filter and the prescale passed
 */

#include <icetray/I3Module.h>

class CreateFilterMask : public I3Module
{
public:
  CreateFilterMask(const I3Context& context);  

  void Configure();

  void Physics(I3FramePtr frame);
  
  void Finish();

private:
  ///Name to give the I3FilterResultMap object in the frame
  std::string output_name_;

};

#endif  // CREATEFILTERMASK_H_INCLUDED
