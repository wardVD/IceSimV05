/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3IceTopResponseService.h
 * @version $Rev: $
 * @date $Date: $
 * @author Tilo Waldenmaier
 */


#ifndef _TOPSIMULATOR_I3ICETOPRESPONSESERVICE_H_
#define _TOPSIMULATOR_I3ICETOPRESPONSESERVICE_H_

#include <icetray/I3Units.h>
#include <icetray/I3ServiceBase.h>
#include <dataclasses/Utility.h>
#include <dataclasses/TankKey.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/calibration/I3Calibration.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include <dataclasses/physics/I3Particle.h>
#include <topsimulator/interface/I3TankResponse.h>
#include <icetray/I3DefaultName.h>

/**
 * \brief This is the IceTop response service which keeps all I3TankResponses.
 * It furthermore provides methods for fast access to the tank responses.
 */

class I3IceTopResponseService: public I3ServiceBase
{
public:
  I3IceTopResponseService(const I3Context& context);
  virtual ~I3IceTopResponseService() {};
  
  /**
   * This method configures this class
   */
  void Configure();
  
  /**
   * The implementation of this method is done in I3IceTopResponseTemplate.h.
   * It initializes all tank responses and adds them to the search grid
   */
  virtual void Initialize(const I3Geometry& geometry,
                          const I3Calibration& calib,
                          const I3DetectorStatus& status) = 0;
  
  /**
   * This method returns the tank response for a single tank
   */
  I3TankResponsePtr GetTankResponse(const TankKey& tankKey);
  
  /**
   * This method returns the map of all registered tank responses
   */
  I3TankResponseMapPtr GetTankResponseMap() {return tankMap_;};
  
  /**
   * This method returns the map of all tank responses which
   * are assigned to a certain bin of the search grid
   */
  I3TankResponseMapPtr GetTankResponseMap(int bin) { return fastGrid_.at(bin); }
  
  /**
   * This method returns the map of all tank responses which
   * are assigned to a certain position on the search grid
   */
  I3TankResponseMapPtr GetTankResponseMap(double x, double y);
  
  /**
   * This method returns the map of all tank responses at the position 
   * where the track intersects the tank plane
   */
  I3TankResponseMapPtr GetTankResponseMap(const I3Particle& track);
  
  /**
   * This method returns the average z position off all registered tanks
   * which is used as the z-position of the search grid plane
   */
  double GetMeanTankZ() const { return meanz_; }
  
  /**
   * This method returns the tank sampling radius. A tank response is assigned to all 
   * bins (pixels) within the sampling radius
   */
  double GetTankSamplingRadius() const { return samplRadius_; }
  
  /**
   * This method is called by the I3TopSimulator at the beginning of each event.
   * It calls the corresponding method of all registered tank responses
   */
  void BeginEvent(const I3Particle& primary);
  
  /**
   * This method is called by the I3TopSimulator at the end of each event.
   * It calls the corresponding method of all registered tank responses
   */
  void EndEvent(HitHistoCollection &hitHC, HitHistoCollection& cherHitCollection);
  
  /**
   * This method prints the coordinates of all registered tanks on the screen
   * (for debugging purpose)
   */
  void Print();
  
#ifdef I3_USE_ROOT
  /**
   * This method generates a root file whuch contains a histogram
   * which displays the search grid (for debugging purpose)
   */
  void FillHistogram();
#endif

protected:
  
  void InitializeGrid();
  
  template <class ParamType>
  void AddParameter(const std::string& name,
                    const std::string& description,
                    const ParamType& defaultValue)
  { I3ServiceBase::AddParameter<ParamType>(name, description, defaultValue); }

  template <class ParamType>
  void GetParameter(const std::string& name, ParamType& value) const
  { I3ServiceBase::GetParameter<ParamType>(name, value); }
  
  const I3Context& GetContext()
  { return I3ServiceBase::GetContext(); }
  const I3Configuration& GetConfiguration() const { return *configuration_; }
  I3Configuration& GetConfiguration() { return *configuration_; }
  
  I3TankResponseMapPtr tankMap_;

private:

  void ResetGrid(double xmin, double xwidth, int xbins, double ymin, double ywidth, int ybins); 
  void AddTank(double x, double y, const TankKey& tankKey, I3TankResponsePtr response);
  
  int GetBin(double x, double y);
  int GetBinX(double x);
  int GetBinY(double y);
  
  double GetBinCenterX(int xbin);
  double GetBinCenterY(int ybin);
  
  int xbins_;
  double xwidth_;
  double xmin_;
  double xmax_;
  
  int ybins_;
  double ywidth_;
  double ymin_;
  double ymax_;
  
  double meanz_;
  double gridSize_;
  double samplRadius_;
  
  std::vector<I3TankResponseMapPtr> fastGrid_;
  
  SET_LOGGER("I3IceTopResponseService");
};

I3_DEFAULT_NAME(I3IceTopResponseService);
I3_POINTER_TYPEDEFS(I3IceTopResponseService);

#endif
