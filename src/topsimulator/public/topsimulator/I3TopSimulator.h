/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3TopSimulator.h
 * @version $Rev: $
 * @date $Date: $
 * @author Tilo Waldenmaier
 */


#ifndef _TOPSIMULATOR_I3TOPSIMULATOR_H_
#define _TOPSIMULATOR_I3TOPSIMULATOR_H_


#include <icetray/I3Module.h>
#include <topsimulator/interface/I3InjectorService.h>
#include <topsimulator/interface/I3IceTopResponseService.h>
#include <topsimulator/ExtendedI3Particle.h>
#include <dataclasses/TankKey.h>

/**
 * \brief The I3Topsimulator module handles the whole simulation and 
 * stores the results in the frame.
 */

class I3TopSimulator : public I3Module 
{
public:
  I3TopSimulator(const I3Context& context);

  ~I3TopSimulator();

  void Configure();
  
  void DetectorStatus(I3FramePtr frame);
  
  void DAQ(I3FramePtr frame);
 
private:
  
  /// This function generates an event header and put it to the frame
  void WriteEventHeader(I3FramePtr frame, int runID, int evtID);

  TankKey GetTankKey(std::string key) const;
  AirShowerComponent GetAirShowerComponent(const I3Particle& p) const;
  std::map<std::string, int> GetAirShowerComponentNameMap() const;

  std::string injectorServiceName_;
  std::string responseServiceName_;
  std::string mcPrimaryName_;
  std::string hitSeriesName_;
  std::string peSeriesName_;
  std::string cherSeriesName_;
  std::string testPulsesName_;
  std::string icMCTreeName_;
  std::string itMCTreeName_;
  double hitBinWidth_;
  double muEnergyCut_;
  bool writeEvtHeader_;
  int compressPEs_;
  bool useInjectorComponents_;
  std::set<TankKey> tankKeys_;

  I3InjectorServicePtr injector_;
  I3IceTopResponseServicePtr response_;
  
  static const std::string INC_ID_NAME;
  
  SET_LOGGER("I3TopSimulator");
};

#endif
