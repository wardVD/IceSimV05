/**
 *  @version $Id$
 *  @date $Date$
 *  @author $Author$
*/
#ifndef RECCLASSES_I3VETO_H
#define RECCLASSES_I3VETO_H

#include <icetray/I3FrameObject.h>
#include <dataclasses/Utility.h>

/**
 * @brief Stores parameters of veto calculation (see project CascadeVariables).
 *
 * Output of I3VetoModule.
 */
class I3Veto : public I3FrameObject {
 public:
  I3Veto();
  virtual ~I3Veto();
  short nUnhitTopLayers;
  short nLayer;
  short earliestLayer;
  short earliestOM;
  short earliestContainment;
  short latestLayer;
  short latestOM;
  short latestContainment;
  short mostOuterLayer;
  double depthHighestHit;
  double depthFirstHit;
  short maxDomChargeLayer;
  short maxDomChargeString;
  short maxDomChargeOM;
  short nDomsBeforeMaxDOM;
  short maxDomChargeLayer_xy;
  short maxDomChargeLayer_z;
  short maxDomChargeContainment;

 private:
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive& ar, unsigned version);
};

I3_POINTER_TYPEDEFS( I3Veto );

/**
 * @brief Stores a subset of parameters of veto calculation (see project CascadeVariables).
 */
class I3VetoShort : public I3FrameObject {
 
 public:
  I3VetoShort();
  virtual ~I3VetoShort();
  short earliestContainment;
  short maxDomChargeContainment;
  
 private:
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive& ar, unsigned version);
  
};

I3_POINTER_TYPEDEFS( I3VetoShort );

#endif
