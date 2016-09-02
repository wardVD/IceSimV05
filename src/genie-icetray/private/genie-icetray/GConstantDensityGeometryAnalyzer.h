/**
 * simple GENIE geometry analyzer, heavily based
 * on the "point geometry" analyzer taken from the
 * GENIE source code. Supplies a constant column
 * depth (or "weighted path length") per target
 * to the caller.
 *
 * - Claudio Kopper
 */

#ifndef GCONSTANTDENSITYGEOMETRYANALYZER_H_INCLUDED
#define GCONSTANTDENSITYGEOMETRYANALYZER_H_INCLUDED

#include <map>

#include "EVGDrivers/GeomAnalyzerI.h"

namespace genie { namespace geometry {
    
    class GConstantDensityGeometryAnalyzer : public GeomAnalyzerI
    {
    public:
        // expects a map <pdgcode, density> with density in units of kg/m^3
        GConstantDensityGeometryAnalyzer(const std::map<int,double> & tgtmap,
                                            double cylinderLength);
        ~GConstantDensityGeometryAnalyzer();
        
        // implement the GeomAnalyzerI interface
        
        const PDGCodeList &ListOfTargetNuclei(void);
        const PathLengthList &ComputeMaxPathLengths(void);
        
        const PathLengthList &ComputePathLengths(const TLorentzVector &x,
                                                 const TLorentzVector &p);
        const TVector3 &GenerateVertex(const TLorentzVector &x,
                                       const TLorentzVector &p,
                                       int tgtpdg);
    private:
        void Initialize(const std::map<int,double> & tgtmap,
                        double cylinderLength);
        void CleanUp(void);
        
        double fCylinderLength;
        bool pathLengthsUpToDate;
        
        TVector3 *fCurrVertex;               ///< current generated vertex
        PathLengthList *fDensityList;        ///< list of densities
        PathLengthList *fCurrPathLengths;    ///< current list of path-lengths
        PathLengthList *fCurrMaxPathLengths; ///< current list of path-lengths
        PDGCodeList *fCurrPDGCodeList;       ///< current list of target nuclei
    };
    
}; };

#endif // GCONSTANTDENSITYGEOMETRYANALYZER_H_INCLUDED
