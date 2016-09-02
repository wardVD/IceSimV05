/**
 * copyright  (C) 2005
 * the icecube collaboration
 * Some functionalities of this module have been taken from the Sieglinde SLHitSel(ector) module
 * $Id$
 *
 * @file I3IsolatedHitsCutModule.h
 * @version $Revision: 1.45 $
 * @date $Date$
 * @author Ralf Ehrlich
 */
#ifndef DOMTOOLS_I3ISOLATEDHITSCUTMODULE_H_INCLUDED
#define DOMTOOLS_I3ISOLATEDHITSCUTMODULE_H_INCLUDED

#include <string>
#include <icetray/I3ConditionalModule.h>
#include <icetray/I3Frame.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <icetray/I3Units.h>
#include <DomTools/I3ResponseIter.h>
#include <DomTools/I3DistanceMap.h>

template <class responseClass> 
class I3IsolatedHitsCutModule : public I3ConditionalModule
{
 
  public:
  I3IsolatedHitsCutModule(const I3Context& context);

  void Configure();
  
  private:
  I3IsolatedHitsCutModule();
  I3IsolatedHitsCutModule(const I3IsolatedHitsCutModule& source);
  I3IsolatedHitsCutModule& operator=(const I3IsolatedHitsCutModule& source);
  std::string inputResponse_, outputResponse_; 

  void HitCleaning(I3FramePtr frame);
  void WriteToFrame(I3FramePtr ptr, 
                    boost::shared_ptr<I3Map<OMKey,std::vector<responseClass> > > output);
  ///cut hits outside this radius
  double RTRadius_;

  ///cut hits outside this time
  double RTTime_;

  ///cut hits with a multiplicity lower than this
  int    RTMultiplicity_;

  //Width of the Hit object to consider (-1: use GetWidth)
  int UseWidth_;

  ///pointer to a class that remembers distances bewteen OMs
  boost::shared_ptr <I3DistanceMap> distPtr;

  SET_LOGGER("I3IsolatedHitsCutModule");

};

#endif
