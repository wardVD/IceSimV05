/**
 * Copyright (C) 2010
 * The IceCube collaboration
 * ID: $Id: G4Delaunay.cxx 67844 2010-10-25 12:16:01Z kislat $
 *
 * @file G4Delaunay.cxx
 * @version $Rev: 67844 $
 * @date $Date: 2010-10-25 14:16:01 +0200 (Mo, 25. Okt 2010) $
 * @author Tilo Waldenmaier
 */


#include <g4-tankresponse/g4classes/G4Delaunay.h>

#include <G4TessellatedSolid.hh>
#include <G4TriangularFacet.hh>
#include <G4QuadrangularFacet.hh>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <boost/foreach.hpp>

// The following defintions are
// needed by triangle.h 
#ifdef SINGLE
#define REAL float
#else
#define REAL double
#endif
#define VOID int

extern "C" {
  #include <g4-tankresponse/triangle/triangle.h>
}

void InitTriangulateIO(triangulateio& trio)
{
  trio.pointlist = (REAL*) NULL;              // In / out
  trio.pointattributelist = (REAL*) NULL;     // In / out
  trio.pointmarkerlist = (int*) NULL;         // In / out
  trio.numberofpoints = 0;                    // In / out
  trio.numberofpointattributes = 0;           // In / out

  trio.trianglelist = (int*) NULL;            // In / out
  trio.triangleattributelist = (REAL*) NULL;  // In / out
  trio.trianglearealist = (REAL*) NULL;       // In only
  trio.neighborlist = (int*) NULL;            // Out only
  trio.numberoftriangles = 0;                 // In / out
  trio.numberofcorners = 0;                   // In / out
  trio.numberoftriangleattributes = 0;        // In / out

  trio.segmentlist = (int*) NULL;             // In / out
  trio.segmentmarkerlist = (int*) NULL;       // In / out
  trio.numberofsegments = 0;                  // In / out
  
  trio.holelist = (REAL*) NULL;               // In / pointer to array copied out
  trio.numberofholes = 0;                     // In / copied out
  
  trio.regionlist = (REAL*) NULL;             // In / pointer to array copied out
  trio.numberofregions = 0;                   // In / copied out

  trio.edgelist = (int*) NULL;                // Out only
  trio.edgemarkerlist = (int*) NULL;          // Not used with Voronoi diagram; out only
  trio.normlist = (REAL*) NULL;               // Used only with Voronoi diagram; out only
  trio.numberofedges = 0;                     // Out only
};

void FreeTriangulateMemory(triangulateio& trio)
{
  log_trace("Free triangulate memory allocation");
  
  if(trio.pointlist)          free(trio.pointlist);
  if(trio.pointattributelist) free(trio.pointattributelist);
  if(trio.pointmarkerlist)    free(trio.pointmarkerlist);
  
  if(trio.trianglelist)          free(trio.trianglelist);
  if(trio.triangleattributelist) free(trio.triangleattributelist);
  if(trio.trianglearealist)      free(trio.trianglearealist);
  if(trio.neighborlist)          free(trio.neighborlist);
  
  if(trio.segmentlist)       free(trio.segmentlist);
  if(trio.segmentmarkerlist) free(trio.segmentmarkerlist);
  
  if(trio.holelist)   free(trio.holelist);
  if(trio.regionlist) free(trio.regionlist);
  
  if(trio.edgelist)       free(trio.edgelist);
  if(trio.edgemarkerlist) free(trio.edgemarkerlist);
  if(trio.normlist)       free(trio.normlist);
};


//***************************************************************//


G4Delaunay::G4Delaunay():
xmin_(NAN),
xmax_(NAN),
ymin_(NAN),
ymax_(NAN),
zmin_(NAN),
zmax_(NAN),
origin_(0,0,0)
{
  pointList_.clear();
  border1_.clear();
  border2_.clear();
  border3_.clear();
  border4_.clear();
}


G4Delaunay::~G4Delaunay()
{
  
}


void G4Delaunay::AddPoint(double x, double y, double z)
{
  log_trace("Adding point (x=%.2f, y=%.2f, z=%.2f)", x, y, z);
  
  if(pointList_.empty())
  {
    xmin_ = x;
    xmax_ = x;
    ymin_ = y;
    ymax_ = y;
    zmin_ = z;
    zmax_ = z;
  }
  else
  {
    xmin_ = std::min(xmin_, x);
    xmax_ = std::max(xmax_, x);
    ymin_ = std::min(ymin_, y);
    ymax_ = std::max(ymax_, y);
    zmin_ = std::min(zmin_, z);
    zmax_ = std::max(zmax_, z);
  }
  
  pointList_.push_back(G4ThreeVector(x,y,z));
}


void G4Delaunay::CreateSurfaceTriangles(G4TessellatedSolid* solid)
{
  struct triangulateio in, out;
  
  InitTriangulateIO(in);
  InitTriangulateIO(out);

  // Define input points.
  in.numberofpoints = pointList_.size();
  in.pointlist = (REAL *) malloc(in.numberofpoints * 2 * sizeof(REAL));
  for(size_t i=0; i<pointList_.size(); ++i)
  {
    const G4ThreeVector& point = pointList_.at(i);
    in.pointlist[2*i]     = point.x();
    in.pointlist[2*i + 1] = point.y();
  }
  
  /**
   * Define triangulate switches:
   * (For more information see triangulate.h) 
   * z: start indexing from zero (IMPORTANT!)
   * c: preserve convex hull
   * N: no boundary markers
   * Q: Quiet
   * V: Verbose
   */
  char switches[] = "zcNQ";
  
  triangulate(switches, &in, &out, NULL);
  
  if(out.numberofcorners!=3)
  {
    log_error("Invalid number (%d) of triangle corners!", out.numberofcorners);
  }
  
  log_trace("Found %d Delaunay triangles:", out.numberoftriangles);
  for(int i=0; i<out.numberoftriangles; ++i)
  {
    log_trace("%4d: (%3d, %3d, %3d)", i, out.trianglelist[3*i], out.trianglelist[3*i + 1], out.trianglelist[3*i + 2]);
    
    G4TriangularFacet *facet = new G4TriangularFacet(pointList_.at(out.trianglelist[3*i]),
						     pointList_.at(out.trianglelist[3*i + 1]),
						     pointList_.at(out.trianglelist[3*i + 2]),
                                                     ABSOLUTE);
    solid->AddFacet(facet);
  }

  FreeTriangulateMemory(in);
  FreeTriangulateMemory(out);
}


void G4Delaunay::CalcOrigin()
{
  if(!border1_.empty())
  {
    log_warn("Border points have already been computed. Cannot move origin anymore!");
    return;
  }
 
  log_trace("Calculating origin ...");
  
  origin_.set(0.5*(xmin_ + xmax_), 0.5*(ymin_ + ymax_), 0.0);
  
  // Transform coordinates relative to origin
  BOOST_FOREACH(G4ThreeVector& point, pointList_)
  {
    point -= origin_;
  }
  
  xmin_ -= origin_.x();
  xmax_ -= origin_.x();
  ymin_ -= origin_.y();
  ymax_ -= origin_.y();
  zmin_ -= origin_.z();
  zmax_ -= origin_.z();
}


void G4Delaunay::ComputeBorderPoints(double margin, double spaceing)
{
  if(!border1_.empty())
  {
    log_warn("Border points have already been computed!");
    return;
  }
  
  log_trace("Computing border points ...");
  
  double xmin = xmin_ - margin;
  double xmax = xmax_ + margin;
  double ymin = ymin_ - margin;
  double ymax = ymax_ + margin;
  
  double xhl = std::max(std::fabs(xmin), std::fabs(xmax));
  double yhl = std::max(std::fabs(ymin), std::fabs(ymax));
  
  // Round to multiples of spaceing
  int numx = static_cast<int>(std::ceil(2.0 * xhl / spaceing));
  int numy = static_cast<int>(std::ceil(2.0 * yhl / spaceing));
  
  xmin = -0.5 * numx * spaceing;
  xmax = +0.5 * numx * spaceing;
  
  ymin = -0.5 * numy * spaceing;
  ymax = +0.5 * numy * spaceing;
  
  border1_.clear();
  border2_.clear();
  border3_.clear();
  border4_.clear();
  
  // Make a copy original point list
  std::vector<G4ThreeVector> tmpList(pointList_);
  
  for(int i=0; i<=numy; i++)
  {
    double y = ymin + i * spaceing;
    double z = GetZValue(tmpList, xmin, y);
    border1_.push_back(pointList_.size());
    AddPoint(xmin, y, z);
    
    y = ymax - i * spaceing;
    z = GetZValue(tmpList, xmax, y);
    border3_.push_back(pointList_.size());
    AddPoint(xmax, y, z);
  }
  
  border2_.push_back(border1_.back());
  border4_.push_back(border3_.back());
  
  for(int k=1; k<numx; k++)
  {
    double x = xmin + k * spaceing;
    double z = GetZValue(tmpList, x, ymax);
    border2_.push_back(pointList_.size());
    AddPoint(x, ymax, z);
    
    x = xmax - k * spaceing;
    z = GetZValue(tmpList, x, ymin);
    border4_.push_back(pointList_.size());
    AddPoint(x, ymin, z);
  }
  
  border2_.push_back(border3_.front());
  border4_.push_back(border1_.front());
}


double G4Delaunay::GetZValue(const std::vector<G4ThreeVector>& plist, double x, double y)
{
  double z(NAN), min_dist(-1);
  BOOST_FOREACH(const G4ThreeVector& point, plist)
  {
    double dx = x - point.x();
    double dy = y - point.y();
    double dist = sqrt(dx*dx + dy*dy);
    
    if(min_dist<0 || dist<min_dist)
    {
      min_dist = dist;
      z = point.z();
    }
  }
  
  log_trace("z(x=%.2f, y=%.2f) = %.2f", x, y, z);
  
  return z;
}


void G4Delaunay::BuildSolid(G4TessellatedSolid* solid, double margin, double spaceing)
{
  if(!border1_.empty())
  {
    log_fatal("Border already calculated. Cannot build tesselated solid %s!", solid->GetName().c_str());
    return;
  }
  
  log_debug("Building tesselated solid %s ...", solid->GetName().c_str());
  
  CalcOrigin();
  
  ComputeBorderPoints(margin, spaceing);
  
  CreateSurfaceTriangles(solid);
  
  CreateBorderTriangles(border1_, solid);
  CreateBorderTriangles(border2_, solid);
  CreateBorderTriangles(border3_, solid);
  CreateBorderTriangles(border4_, solid);
  
  CreateBottomFacet(solid);
  
  solid->SetSolidClosed(true);
}


void G4Delaunay::CreateBorderTriangles(const std::vector<int>& border, G4TessellatedSolid* solid)
{
  log_debug("Compute border triangles ...");
  
  for(size_t i=0; i<border.size()-1; i++)
    {
    double x1 = pointList_.at(border.at(i)).x();
    double y1 = pointList_.at(border.at(i)).y();
    double z1 = pointList_.at(border.at(i)).z();
    
    double x2 = pointList_.at(border.at(i+1)).x();
    double y2 = pointList_.at(border.at(i+1)).y();
    double z2 = pointList_.at(border.at(i+1)).z();
    
    G4TriangularFacet *facet1 = new G4TriangularFacet(G4ThreeVector(x1, y1, 0.0),
                                                      G4ThreeVector(x1, y1, z1),
                                                      G4ThreeVector(x2, y2, z2),
                                                      ABSOLUTE);
    
    G4TriangularFacet *facet2 = new G4TriangularFacet(G4ThreeVector(x1, y1, 0.0),
                                                      G4ThreeVector(x2, y2, z2),
                                                      G4ThreeVector(x2, y2, 0.0),
                                                      ABSOLUTE);
    
    solid->AddFacet(facet1);
    solid->AddFacet(facet2);
  }
}


void G4Delaunay::CreateBottomFacet(G4TessellatedSolid* solid)
{
  log_debug("Compute bottom facet ...");
  
  G4QuadrangularFacet *facet = new G4QuadrangularFacet(G4ThreeVector(xmin_, ymin_, 0.0),
                                                       G4ThreeVector(xmin_, ymax_, 0.0),
                                                       G4ThreeVector(xmax_, ymax_, 0.0),
                                                       G4ThreeVector(xmax_, ymin_, 0.0),
                                                       ABSOLUTE);
  solid->AddFacet(facet);
}


