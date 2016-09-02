/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3CorsikaGridInjector.h
 * @version $Rev: $
 * @date $Date: $
 * @authors Tilo Waldenmaier
 */


#ifndef _TOPSIMULATOR_I3CORSIKAGRIDINJECTOR_H_
#define _TOPSIMULATOR_I3CORSIKAGRIDINJECTOR_H_


#include <topsimulator/interface/I3InjectorService.h>
#include <topsimulator/injectors/I3CorsikaReader.h>
#include <topsimulator/ExtendedI3Particle.h>
#include <phys-services/I3RandomService.h>
#include <dataclasses/I3Position.h>
#include <dataclasses/TankKey.h>

#include <map>
#include <string>


struct GridKey
{
private:
  double width_;
  int x_;
  int y_;

public:
  GridKey(double x, double y, double width)
  {
    width_ = width;
    x_ = static_cast<int>(floor(x/width_));
    y_ = static_cast<int>(floor(y/width_));
  };
  
  double GetX() const {return (x_ + 0.5)*width_;};
  double GetY() const {return (y_ + 0.5)*width_;};
  
  bool operator==(const GridKey& rhs) const
  {
    if(rhs.x_ == x_ && rhs.y_ == y_)
      return true;
    return false;
  };

  bool operator!=(const GridKey& rhs) const
  {
    if(rhs == *this)
      return false;
    return true;
  };
  
  bool operator<(const GridKey& rhs) const
  {
    if(x_ < rhs.x_)
      return true;
    if(x_ > rhs.x_)
      return false;
    if(y_ < rhs.y_)
      return true;
    return false;
  };
};
  

/**
 * \brief Scans through a CORSIKA file, samples all particles on a grid
 * centered around the specified tanks or stations, and passes those to I3TopSimulator.
 *
 * It is based on an idea of Arne Van Overloop and provides a very efficient
 * way for generating a lot of statistics for single tank/station studies
 * (e.g. to make a muon spectrum). It is not suited for doing full 
 * air-shower simulations! 
 */

class I3CorsikaGridInjector: public I3InjectorService
{
public:
  I3CorsikaGridInjector(const I3Context& context);
  ~I3CorsikaGridInjector();
  
  void Configure();
  
  bool NextEvent(int& runID, int& evtID, I3Particle& primary, I3FrameConstPtr frame);
  
  bool NextParticle(ExtendedI3Particle& primary);
  
  std::map<std::string, int> GetAirShowerComponentNameMap() const;

  I3FrameObjectConstPtr GetEventInfo();
  
private:
  void FillTankList(const std::vector<std::string>& tankList);
  void FillCenterList();
  
  void RelocateParticle(I3Particle& particle, double shiftX, double shiftY);
  
  std::map<GridKey, std::vector<I3Particle> > eventBuffer_;
  std::map<GridKey, std::vector<I3Particle> >::const_iterator eventIter_;
  std::vector<I3Particle>::const_iterator particleIter_;
  
  std::vector<I3Position> centerList_;
  std::vector<I3Position>::const_iterator centerIter_;
  
  std::vector<std::string> corsikaFiles_;
  I3CorsikaReader reader_;
  
  I3Particle currentPrimary_;
  int currentRunID_;
  int currentEvtID_;
  
  double gridSize_;
  
  std::vector<TankKey> tankList_;
  std::vector<int> stationList_;
  
  std::string randomServiceName_;
  I3RandomServicePtr randomService_;
  
  SET_LOGGER("I3CorsikaGridInjector");
};


#endif
