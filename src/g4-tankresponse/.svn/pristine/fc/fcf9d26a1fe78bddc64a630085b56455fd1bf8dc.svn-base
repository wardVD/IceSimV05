/**
 * Copyright (C) 2010
 * The IceCube collaboration
 * ID: $Id: G4Delaunay.h 67744 2010-10-21 15:14:29Z kislat $
 *
 * @file G4Delaunay.h
 * @version $Rev: 67744 $
 * @date $Date: 2010-10-21 17:14:29 +0200 (Do, 21. Okt 2010) $
 * @author Tilo Waldenmaier
 */


#ifndef _TOPSIMULATOR_G4DELAUNAY_H_
#define _TOPSIMULATOR_G4DELAUNAY_H_

#include <icetray/I3Logging.h>

#include <G4TessellatedSolid.hh>

#include <G4ThreeVector.hh>

#include <vector>


class G4Delaunay
{
 public:
  G4Delaunay();
  ~G4Delaunay();
  
  void AddPoint(double x, double y, double z);
  
  void BuildSolid(G4TessellatedSolid* solid, double margin, double spaceing);
  
  const G4ThreeVector& GetOrigin() const {return origin_;};
  
  const double GetXmin() const {return xmin_;};
  const double GetXmax() const {return xmax_;};
  const double GetYmin() const {return ymin_;};
  const double GetYmax() const {return ymax_;};
  const double GetZmin() const {return zmin_;};
  const double GetZmax() const {return zmax_;};
  
 private:
  void CalcOrigin();
  
  void ComputeBorderPoints(double margin, double spaceing);
  
  double GetZValue(const std::vector<G4ThreeVector>& plist, double x, double y);
  
  void CreateSurfaceTriangles(G4TessellatedSolid* solid);
  
  void CreateBorderTriangles(const std::vector<int>& border, G4TessellatedSolid* solid);
  
  void CreateBottomFacet(G4TessellatedSolid* solid);
  
  double xmin_;
  double xmax_;
  double ymin_;
  double ymax_;
  double zmin_;
  double zmax_;
  
  G4ThreeVector origin_;
  
  std::vector<G4ThreeVector> pointList_;
  
  std::vector<int> border1_;
  std::vector<int> border2_;
  std::vector<int> border3_;
  std::vector<int> border4_;
  
  SET_LOGGER("G4Delaunay");
};

#endif
