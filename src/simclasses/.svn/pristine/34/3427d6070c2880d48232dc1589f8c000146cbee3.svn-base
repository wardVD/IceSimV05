/**
 * Copyright (C) 2010
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3CorsikaShowerInfo.h
 * @version $Rev: $
 * @date $Date: $
 * @author Tilo Waldenmaier
 */

#ifndef _TOPSIMULATOR_I3CORSIKASHOWERINFO_H_
#define _TOPSIMULATOR_I3CORSIKASHOWERINFO_H_


#include <dataclasses/Utility.h>
#include <icetray/I3FrameObject.h>
#include <icetray/I3DefaultName.h>
#include "simclasses/CorsikaLongStep.h"
#include <ostream>

/**
 * \brief This is a container class to store additional
 * shower information
 */

static const unsigned i3corsikashowerinfo_version_ = 2;

struct I3CorsikaShowerInfo: public I3FrameObject
{
 public:
  int32_t crsRunID;
  int32_t crsEventID;
  int32_t crsSampleID;
  
  double firstIntHeight;
  double firstIntDepth;
  double obsLevelHeight;
  
  // Gaisser-Hillas fit parameters (using all charged particles)
  double ghMaxNum;
  double ghStartDepth;
  double ghMaxDepth;
  double ghRedChiSqr;
  
  CorsikaLongProfile longProfile;

  double resampleRadius;
  uint16_t nResample;

  uint16_t nResampleNominal;
  double weight;

  I3CorsikaShowerInfo();
  
  virtual ~I3CorsikaShowerInfo();
  
  void clear();

  bool operator==(const I3CorsikaShowerInfo& rhs);
  
  template <class Archive> void serialize(Archive & ar, unsigned version);
};

std::ostream& operator<<(std::ostream& os, const I3CorsikaShowerInfo&);

BOOST_CLASS_VERSION(I3CorsikaShowerInfo, i3corsikashowerinfo_version_);
I3_DEFAULT_NAME(I3CorsikaShowerInfo);
I3_POINTER_TYPEDEFS(I3CorsikaShowerInfo);

#endif
