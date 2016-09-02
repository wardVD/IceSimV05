/**
 * simple GENIE geometry analyzer, heavily based
 * on the "point geometry" analyzer taken from the
 * GENIE source code. Supplies a constant column
 * depth (or "weighted path length") per target
 * to the caller.
 *
 * - Claudio Kopper
 */

#include <TLorentzVector.h>
#include <TVector3.h>

#include "genie-icetray/GConstantDensityGeometryAnalyzer.h"
#include "EVGDrivers/PathLengthList.h"
#include "Messenger/Messenger.h"
#include "Numerical/RandomGen.h"
#include "PDG/PDGCodeList.h"
#include "PDG/PDGLibrary.h"
#include "Utils/PrintUtils.h"

using namespace genie;
using namespace genie::geometry;

//___________________________________________________________________________
// expects a map <pdgcode, density> with density in units of kg/m^3.
// the cylinder length is expected to be in m.
GConstantDensityGeometryAnalyzer::GConstantDensityGeometryAnalyzer(const std::map<int,double> &tgtmap,
                                                                   double cylinderLength) :
GeomAnalyzerI(),
pathLengthsUpToDate(false)
{
    this->Initialize(tgtmap, cylinderLength);
}
//___________________________________________________________________________
GConstantDensityGeometryAnalyzer::~GConstantDensityGeometryAnalyzer()
{
    this->CleanUp();
}
//___________________________________________________________________________
const PDGCodeList &GConstantDensityGeometryAnalyzer::ListOfTargetNuclei(void)
{
    return *fCurrPDGCodeList;
}
//___________________________________________________________________________
const PathLengthList &GConstantDensityGeometryAnalyzer::ComputeMaxPathLengths(void)
{
    // use a scaled version of the density map
    for (PathLengthList::iterator it=fDensityList->begin();
         it != fDensityList->end(); ++it)
    {
        (*fCurrMaxPathLengths)[it->first] = it->second * fCylinderLength;
    }
    
    return *fCurrMaxPathLengths;
}
//___________________________________________________________________________
const PathLengthList &GConstantDensityGeometryAnalyzer::ComputePathLengths
(const TLorentzVector &x, const TLorentzVector &p)
{
    // this is currently scaled with the maximum distance (corresponding to
    // the full cylinder length). Change that if the flux can be injected
    // at arbitrary distances..
    
    // use a scaled version of the density map
    for (PathLengthList::iterator it=fDensityList->begin();
         it != fDensityList->end(); ++it)
    {
        (*fCurrPathLengths)[it->first] = it->second * fCylinderLength;
    }
    
    return *fCurrPathLengths;
}
//___________________________________________________________________________
const TVector3 &GConstantDensityGeometryAnalyzer::GenerateVertex(const TLorentzVector &x,
                                                                 const TLorentzVector &p,
                                                                 int tgtpdg)
{
    // the flux currently has to be injected at a cylinder centered
    // on (0,0,0) with a length of cylinderLength. Check if this is the case.
    
    TVector3 fluxInjectionPoint(x.X(),x.Y(),x.Z());
    TVector3 fluxInjectionDirection(p.X(),p.Y(),p.Z());
    fluxInjectionDirection.SetMag(1.);
    
    // is it on the cylinder?
    const double distInCylinder = -fluxInjectionPoint*fluxInjectionDirection;
    if (fabs(distInCylinder-(fCylinderLength/2.)) > 1e-5)
        LOG("GConstantDensityGeometryAnalyzer", pFATAL) << "flux injection not on geometry cylinder, dist is " << distInCylinder;
    else
        LOG("GConstantDensityGeometryAnalyzer", pDEBUG) << "flux injection _IS_ on geometry cylinder, dist is " << distInCylinder;

    RandomGen *rnd = RandomGen::Instance();
    *fCurrVertex = fluxInjectionPoint + fluxInjectionDirection * fCylinderLength * rnd->RndGeom().Rndm();
    
    return *fCurrVertex;
}
//___________________________________________________________________________
void GConstantDensityGeometryAnalyzer::Initialize(const std::map<int,double> &tgtmap, 
                                                  double cylinderLength)
{
    fCurrVertex = new TVector3(0,0,0);
    
    fCylinderLength = cylinderLength;
    
    fCurrPDGCodeList = new PDGCodeList();
    fCurrPDGCodeList->clear();
    
    map<int,double>::const_iterator iter;
    for(iter = tgtmap.begin(); iter != tgtmap.end(); ++iter) {
        int tgtpdgc = iter->first;
        fCurrPDGCodeList->push_back(tgtpdgc);
    }
    
    fDensityList = new PathLengthList(tgtmap);
    fCurrPathLengths = new PathLengthList(tgtmap);
    fCurrMaxPathLengths = new PathLengthList(tgtmap);
}
//___________________________________________________________________________
void GConstantDensityGeometryAnalyzer::CleanUp(void)
{
    if( fCurrMaxPathLengths ) delete fCurrMaxPathLengths;
    if( fCurrPathLengths ) delete fCurrPathLengths;
    if( fDensityList ) delete fDensityList;
    if( fCurrPDGCodeList    ) delete fCurrPDGCodeList;
}
//___________________________________________________________________________

