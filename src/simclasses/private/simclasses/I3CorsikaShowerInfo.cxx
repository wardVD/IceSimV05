/**
 * Copyright (C) 2010
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3CorsikaShowerInfo.cxx
 * @version $Rev: $
 * @date $Date: $
 * @author Tilo Waldenmaier
 */


#include "simclasses/I3CorsikaShowerInfo.h"
#include <icetray/serialization.h>


I3CorsikaShowerInfo::I3CorsikaShowerInfo():I3FrameObject()
{
  clear();
}


I3CorsikaShowerInfo::~I3CorsikaShowerInfo()
{
  
}


void I3CorsikaShowerInfo::clear()
{
  crsRunID    = -1;
  crsEventID  = -1;
  crsSampleID = -1;

  firstIntHeight = NAN;
  firstIntDepth  = NAN;
  obsLevelHeight = NAN;
  
  ghMaxNum     = NAN;
  ghStartDepth = NAN;
  ghMaxDepth   = NAN;
  ghRedChiSqr  = NAN;

  resampleRadius = NAN;
  nResample      = 0;
  nResampleNominal = 0;
  weight      = 1.;
  
  longProfile.clear();
}


template <class Archive>
void I3CorsikaShowerInfo::serialize(Archive& ar, unsigned version)
{
  if(version>i3corsikashowerinfo_version_)
  {
    log_fatal("Attempting to read version %u from file but running version %u of I3CorsikaShowerInfo class.",
	      version,
	      i3corsikashowerinfo_version_);
  }
  
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("crsRunID",       crsRunID);
  ar & make_nvp("crsEventID",     crsEventID);
  ar & make_nvp("crsSampleID",    crsSampleID); 
  ar & make_nvp("firstIntHeight", firstIntHeight);
  ar & make_nvp("firstIntDepth",  firstIntDepth);
  ar & make_nvp("obsLevelHeight", obsLevelHeight);
  ar & make_nvp("ghMaxNum",       ghMaxNum);
  ar & make_nvp("ghStartDepth",   ghStartDepth);
  ar & make_nvp("ghMaxDepth",     ghMaxDepth);
  ar & make_nvp("ghRedChiSqr",    ghRedChiSqr);
  ar & make_nvp("longProfile",    longProfile);

  if (version > 0) {
    ar & make_nvp("resampleRadius", resampleRadius);
    ar & make_nvp("nResample", nResample);
  }
  if (version > 1) {
    ar & make_nvp("weight", weight);
    ar & make_nvp("nResampleNominal", nResampleNominal);
  }
}


bool I3CorsikaShowerInfo::operator==(const I3CorsikaShowerInfo& rhs) {
  return crsRunID == rhs.crsRunID &&
         crsEventID == rhs.crsEventID &&
         crsSampleID == rhs.crsSampleID &&
         firstIntHeight == rhs.firstIntHeight &&
         firstIntDepth == rhs.firstIntDepth &&
         obsLevelHeight == rhs.obsLevelHeight &&
         ghMaxNum == rhs.ghMaxNum &&
         ghStartDepth == rhs.ghStartDepth &&
         ghRedChiSqr == rhs.ghRedChiSqr &&
         longProfile == rhs.longProfile &&
         resampleRadius == rhs.resampleRadius &&
         nResample == rhs.nResample &&
         nResampleNominal == rhs.nResampleNominal &&
         weight == rhs.weight;
}


std::ostream& operator<<(std::ostream& os, const I3CorsikaShowerInfo& x) {
  os << "[ I3CorsikaShowerInfo::"
     << "\n  crsRunID        :" << x.crsRunID
     << "\n  crsEventID      :" << x.crsEventID
     << "\n  crsSampleID     :" << x.crsSampleID
     << "\n  firstIntHeight  :" << x.firstIntHeight
     << "\n  firstIntDepth   :" << x.firstIntDepth
     << "\n  obsLevelHeight  :" << x.obsLevelHeight
     << "\n  ghMaxNum        :" << x.ghMaxNum
     << "\n  ghStartDepth    :" << x.ghStartDepth
     << "\n  ghRedChiSqr     :" << x.ghRedChiSqr
     << "\n  resampleRadius  :" << x.resampleRadius
     << "\n  nResample       :" << x.nResample
     << "\n  nResampleNominal:" << x.nResampleNominal
     << "\n  weight          :" << x.weight
     << "\n  longProfile     :" << x.longProfile
     << " ]";
  return os;
}



I3_SERIALIZABLE(I3CorsikaShowerInfo);
