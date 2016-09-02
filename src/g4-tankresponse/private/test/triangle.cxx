#include <I3Test.h>
#include <icetray/I3Logging.h>

#include <cstdlib>

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


TEST_GROUP(triangle);


namespace {

  void InitTriangulateIO(triangulateio& trio)
  {
    trio.pointlist = (REAL*) NULL;              // In / out
    trio.pointattributelist = (REAL*) NULL;     // In / out
    trio.pointmarkerlist = (int*) NULL;          // In / out
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

}


struct Point {
  REAL x;
  REAL y;
};


triangulateio run_triangulate_test(size_t nPoints, const Point *points,
				   int nTriangles)
{
  triangulateio in, out;
	 
  InitTriangulateIO(in);
  InitTriangulateIO(out);
	
  // Define input points.
  in.numberofpoints = 3;
  in.pointlist = (REAL*)malloc(in.numberofpoints * 2 * sizeof(REAL));
  
  for (size_t i = 0; i < nPoints; ++i) {
    in.pointlist[2*i]   = points[i].x;
    in.pointlist[2*i+1] = points[i].y;
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
  char switches[] = "zcNV";

  triangulate(switches, &in, &out, NULL);

  ENSURE_EQUAL(out.numberofcorners, 3, "Invalid number of triangle corners");

  ENSURE_EQUAL(out.numberoftriangles, nTriangles, "Invalid number of triangles");

  for (int i = 0; i<out.numberoftriangles; ++i) {
    log_info("%4d: (%3d, %3d, %3d)", i, out.trianglelist[3*i],
	     out.trianglelist[3*i + 1], out.trianglelist[3*i + 2]);
  }
	
  FreeTriangulateMemory(in);

  return out;
}


TEST(primitive)
{
  static const size_t nPoints = 3;
  static const Point points[nPoints] = {
    { 0, 0 },
    { 2, 0 },
    { 1, 1 }
  };

  triangulateio out = run_triangulate_test(nPoints, points, 1);
  FreeTriangulateMemory(out);
}


/*TE*//*ST(square)   // I'd love to figure out why this takes forever
{
  static const size_t nPoints = 4;
  static const Point points[nPoints] = {
    { 0, 0 },
    { 1, 0 },
    { 1, 1 },
    { 0, 1 }
  };

  triangulateio out = run_triangulate_test(nPoints, points, 1);
  FreeTriangulateMemory(out);
}
*/
