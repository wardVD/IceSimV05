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

#include <cassert>
#include <cmath>
#include <algorithm>

#include <TVector3.h>

#include "Conventions/Constants.h"
#include "genie-icetray/GCylindPowerLawFlux.h"
#include "Messenger/Messenger.h"
#include "Numerical/RandomGen.h"
#include "PDG/PDGCodeList.h"
#include "Utils/PrintUtils.h"

using namespace genie;
using namespace genie::constants;
using namespace genie::flux;

//____________________________________________________________________________
GCylindPowerLawFlux::GCylindPowerLawFlux(const std::string &flavorString,
                                         double injectionLength,
                                         double injectionRadius,
                                         double powerLawIndex,
                                         double minEnergy,
                                         double maxEnergy,
                                         double zenithMin,
                                         double zenithMax,
                                         double azimuthMin,
                                         double azimuthMax, 
                                         double nuFraction)
:
fInjectionLength(injectionLength),
fInjectionRadius(injectionRadius),
fPowerLawIndex(powerLawIndex),
fMinEv(minEnergy),
fMaxEv(maxEnergy),
fZenithMin(zenithMin), 
fZenithMax(zenithMax), 
fAzimuthMin(azimuthMin),
fAzimuthMax(azimuthMax),
fNuFraction(nuFraction)
{
    fPdgCList = new PDGCodeList();
    
    if (flavorString=="NuE") {
        fPdgCList->push_back(-12); fPdgCList->push_back(12);
    } else if (flavorString=="NuMu") {
        fPdgCList->push_back(-14); fPdgCList->push_back(14);
    } else if (flavorString=="NuTau") {
        fPdgCList->push_back(-16); fPdgCList->push_back(16);
    } else {
        LOG("Flux", pFATAL) << "Invalid neutrino flavor selected: (Use \"NuE\", \"NuMu\" or \"NuTau\") " << flavorString;
    }
    //std::cout<<"Fraction of neutrinos: "<<fNuFraction<<std::endl;

}
//___________________________________________________________________________
GCylindPowerLawFlux::~GCylindPowerLawFlux()
{
    this->CleanUp();
}
//___________________________________________________________________________
bool GCylindPowerLawFlux::GenerateNext(void)
{
    //-- Reset previously generated neutrino code / 4-p / 4-x
    this->ResetSelection();
    
    //-- Generate an energy according to the configured power law
    const double Ev = this->GenerateEnergy();
    
    //-- Generate a random neutrino direction (4pi)
    TVector3 dirVec;
    GenerateDirection(dirVec);

    TVector3 p3(dirVec); // momentum along the neutrino direction
    p3.SetMag(Ev);       // with |p|=Ev
    
    fgP4.SetPxPyPzE(p3.Px(), p3.Py(), p3.Pz(), Ev);
    
    //-- Select a neutrino species from the flux fractions at the
    //   selected energy
    fgPdgC = (*fPdgCList)[this->SelectNeutrino(Ev)];
    
    //-- Compute neutrino 4-x
    double phiDisc, radiusDisc;
    GeneratePhiAndRadiusOnDisc(phiDisc, radiusDisc);

    TVector3 vec0(-dirVec);                 // vector along the input direction
    TVector3 vec = vec0.Orthogonal();       // orthogonal vector
    vec.Rotate(phiDisc,vec0);               // rotate around original vector
    vec.SetMag(radiusDisc*fInjectionRadius); // set new norms
    vec0.SetMag(fInjectionLength/2.);
    
    TVector3 injPoint = vec0+vec;
    
    fgX4.SetXYZT(injPoint.X(),injPoint.Y(),injPoint.Z(),0.); // injected to time==0
    
    LOG("Flux", pINFO) << "Generated neutrino pdg-code: " << fgPdgC;
    LOG("Flux", pINFO)
    << "Generated neutrino p4: " << utils::print::P4AsShortString(&fgP4);
    LOG("Flux", pINFO)
    << "Generated neutrino x4: " << utils::print::X4AsString(&fgX4);
    
    return true;
}
//___________________________________________________________________________
void GCylindPowerLawFlux::ResetSelection()
{
    // initializing running neutrino pdg-code, 4-position, 4-momentum
    fgPdgC = 0;
    fgP4.SetPxPyPzE (0.,0.,0.,0.);
    fgX4.SetXYZT    (0.,0.,0.,0.);
}
//___________________________________________________________________________
void GCylindPowerLawFlux::CleanUp()
{
    LOG("Flux", pNOTICE) << "Cleaning up...";
    
    if (fPdgCList   ) delete fPdgCList;
}
//___________________________________________________________________________
double GCylindPowerLawFlux::GenerateEnergy() const
{
    // taken from neutrino-generator I3NuGInjector.cxx
    
    const double gamma    = fPowerLawIndex;
    const double eminLog  = std::log10(fMinEv);
    const double emaxLog  = std::log10(fMaxEv);
    
    
    double EnergyLog = 0.0;
    
    LOG("Flux", pDEBUG) << "injected with power law index " << fPowerLawIndex << ", btween log10(minE)=" << eminLog << " and log10(maxE)=" << emaxLog;
    
    if(eminLog==emaxLog){ 
        EnergyLog = eminLog;
    }else{
        RandomGen* rnd = RandomGen::Instance();

        if(gamma==1.0){
            const double eLogRange = emaxLog-eminLog;
            EnergyLog = eminLog + (rnd->RndFlux().Rndm()*eLogRange);
        }else if (gamma>0.){
            const double rand = rnd->RndFlux().Rndm();
            const double energyP = (1.-rand)*std::pow(fMinEv,1.-gamma)+rand*std::pow(fMaxEv,1.-gamma);
            const double energy  =  std::pow(energyP,1./(1.-gamma));
            EnergyLog = std::log10(energy);
        }else{
            const double range = fMaxEv - fMinEv;
            const double energy = fMinEv + (rnd->RndFlux().Rndm()*range);
            EnergyLog = std::log10(energy);
            //LOG("Flux", pERROR) << "Wrong power, we will get constant energy " << energy << " instead";
        }
        
    }
    
    LOG("Flux", pDEBUG) << "injected with log10(E)=" << EnergyLog;
    
    return std::pow(10., EnergyLog); // it's in GeV
}
//___________________________________________________________________________
void GCylindPowerLawFlux::GenerateDirection(TVector3 &v) const
{
    // taken from neutrino-generator I3NuGInjector.cxx

    RandomGen* rnd = RandomGen::Instance();

    double Zenith=0.0, Azimuth=0.0, cosZenith=0.0;
    
    if(fZenithMin==fZenithMax){ 
        Zenith = fZenithMin;
    }else{
        const double cosZenithMin = std::cos(fZenithMin);
        const double cosZenithMax = std::cos(fZenithMax);
        const double cosZenithRange = cosZenithMin-cosZenithMax;
        
        cosZenith = cosZenithMax + (rnd->RndFlux().Rndm() * cosZenithRange);
        Zenith = std::acos(cosZenith);
    }
    
    if(fAzimuthMin==fAzimuthMax){ 
        Azimuth = fAzimuthMin;
    }else{
        const double azimuthRange = fAzimuthMax-fAzimuthMin;
        Azimuth = fAzimuthMin + (rnd->RndFlux().Rndm() * azimuthRange);
    }

    const double theta = kPi-Zenith;
    const double phi = Azimuth-kPi;
    const double rho = std::sin(theta);
    v.SetXYZ(rho*std::cos(phi), rho*std::sin(phi), std::cos(theta));

    LOG("Flux", pDEBUG) << "injected to (theta,phi)=(" << theta << "," << phi << ")[rad]";
}
//___________________________________________________________________________
void GCylindPowerLawFlux::GeneratePhiAndRadiusOnDisc(double &phiDisc, double &radius) const
{
    RandomGen* rnd = RandomGen::Instance();
    double x,y;
    do {
        // generate position on square from [-1;1]x[-1;1]
        x = rnd->RndFlux().Rndm()*2. - 1.;
        y = rnd->RndFlux().Rndm()*2. - 1.;
        
        radius = std::sqrt(x*x+y*y);
    } while (radius > 1.); // we need a radius <= 1
    
    // calculate theta&phi
    phiDisc=0.;
    if (x || y) phiDisc=std::atan2(y,x);
}
//___________________________________________________________________________
int GCylindPowerLawFlux::SelectNeutrino(double Ev)
{
    const unsigned int n = fPdgCList->size();
    if (n!=2) LOG("Flux", pFATAL) << "n is expected to be 2, it is " << n;
    
    RandomGen* rnd = RandomGen::Instance();
    unsigned int randomChoice = 1000;
    if ( (1.-rnd->RndFlux().Rndm())> fNuFraction) {randomChoice = 0;}
    else {randomChoice = 1;}
    
    // use 1-r to get a distribution [0,1[ (i.e., not including 1)
    //const unsigned int randomChoice = static_cast<unsigned int>(static_cast<double>(n)*(1.-rnd->RndFlux().Rndm()));
    if (randomChoice > n) LOG("Flux", pFATAL) << "Invalid result from random number generator, randomChoice==" << randomChoice;

    return randomChoice;
}
//___________________________________________________________________________
