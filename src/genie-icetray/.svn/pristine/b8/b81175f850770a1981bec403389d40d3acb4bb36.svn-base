/**
 * simple GENIE geometry analyzer, based
 * on the "GCylindTH1Flux" analyzer taken from the
 * GENIE source code. 
 *
 * Generates neutrinos of a given flavor according
 * to a given power law spectrum on a cylinder surface.
 *
 * - Claudio Kopper
 */

#ifndef GCYLINDPOWERLAWFLUX_H_INCLUDED
#define GCYLINDPOWERLAWFLUX_H_INCLUDED

#include <string>
#include <vector>

#include <TLorentzVector.h>

#include "EVGDrivers/GFluxI.h"

class TVector3;

namespace genie {namespace flux  {
    
    class GCylindPowerLawFlux : public GFluxI
    {
        public :
        GCylindPowerLawFlux(const std::string &flavorString,///< NuE, NuMu or NuTau
                            double injectionLength,         ///< injection cylinder (full) length [m]
                            double injectionRadius,         ///< injection cylinder radius [m]
                            double powerLawIndex,           ///< power law index, spectrum will be E^-index
                            double minEnergy,               ///< minimum neutrino energy [GeV]
                            double maxEnergy,               ///< maximum neutrino energy [GeV]
                            double zenithMin,               ///< minimum zenith angle [rad]
                            double zenithMax,               ///< maximum zenith angle [rad]
                            double azimuthMin,              ///< minimum azimuth angle [rad]
                            double azimuthMax,              ///< maximum azimuth angle [rad]
                            double nuFraction);             ///< fraction of neutrinos in sample (other - to nubars)
        
        ~GCylindPowerLawFlux();
        
        // methods implementing the GENIE GFluxI interface
        const PDGCodeList &    FluxParticles (void) { return *fPdgCList; }
        double                 MaxEnergy     (void) { return  fMaxEv;    }
        bool                   GenerateNext  (void);
        int                    PdgCode       (void) { return  fgPdgC;    }
        double                 Weight        (void) { return  1.0;       }
        const TLorentzVector & Momentum      (void) { return  fgP4;      }
        const TLorentzVector & Position      (void) { return  fgX4;      }
        bool                   End           (void) { return  false;     }
        
        // Genie 2.6.2 added those to the abstract base class definition.
        // - use dummy implementations for now:
        long int               Index            ()                 {return 0;}
        void                   Clear            (const Option_t *) {}
        void                   GenerateWeighted (bool)             {}

    private:
        
        // private methods
        void   CleanUp           ();
        void   ResetSelection    ();
        int    SelectNeutrino    (double Ev);
        double GenerateEnergy    () const;
        void   GenerateDirection (TVector3 &v) const;
        void   GeneratePhiAndRadiusOnDisc(double &phiDisc, double &radius) const; ///< radius will be <= 1

        // private data members
        PDGCodeList*   fPdgCList;       ///< list of neutrino pdg-codes [length==2]
        
        int            fgPdgC;          ///< running generated nu pdg-code
        TLorentzVector fgP4;            ///< running generated nu 4-momentum
        TLorentzVector fgX4;            ///< running generated nu 4-position

        double         fInjectionLength;///< injection cylinder (full) length
        double         fInjectionRadius;///< injection cylinder radius
        double         fPowerLawIndex;  ///< power law index
        double         fMinEv;          ///< minimum energy
        double         fMaxEv;          ///< maximum energy
        double         fZenithMin;      ///< minimum zenith
        double         fZenithMax;      ///< maximum zenith
        double         fAzimuthMin;      ///< minimum azimuth
        double         fAzimuthMax;      ///< maximum azimuth
        double         fNuFraction;      ///< fraction of neutrinos in sample (other - to anti-neutrinos)
    };
}};

#endif // GCYLINDPOWERLAWFLUX_H_INCLUDED
