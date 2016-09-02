#ifndef I3MPHITFILTER_H
#define I3MPHITFILTER_H

#include <icetray/I3TrayHeaders.h>
#include <icetray/I3Module.h>
#include <phys-services/I3SummaryService.h>

/**
 * copyright  (C) 2005
 * The IceCube Collaboration
 *
 * @date $Date:$
 * @author Juan Carlos Diaz Velez
 *                                                                       
 * @brief An I3IcePick which selects events which have
 * have MCPEs >= threshold
 *
 */
class MPHitFilter : public I3Module 
{
 public:

  MPHitFilter(const I3Context& context) ;

  void Configure();

  void DAQ(I3FramePtr frame);
  void Finish();

 private:
  std::string hitSeriesMapName_;
  std::vector<std::string> hitSeriesMapNames_;
  unsigned int threshold_;
  double weightSum_;
  double weightPartSum_;
  std::string weightSumName_;
  std::string weightPartSumName_;
  double timeScaleSingle_;
  unsigned int nprimaries_;
  unsigned int eventCount_;
  unsigned int keptEvents_;
  unsigned int rejectedEvents_;
  unsigned int removedBranches_;
  double weightprod_;
  bool pruneTree_;
  bool removeBackgroundOnly_;
  bool filter_;
  std::string mcTreeName_;
  std::string weightMap_;

  SET_LOGGER("MPHitFilter");
};

#endif
