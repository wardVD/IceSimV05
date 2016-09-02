/**
 * copyright (C) 2011
 * Kotoyo Hoshina<hoshina@icecube.wisc.edu>
 * $Id: I3NuGGenie.h 103608 2013-05-03 16:54:54Z claudio.kopper $
 *
 * @file I3NuGGenie.h
 * @version $Revision$
 * @date $Date: 2013-05-03 11:54:54 -0500 (Fri, 03 May 2013) $
 * @author Kotoy Hoshina
 *
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 */

#ifndef I3NuGGenie_H
#define I3NuGGenie_H

#include "icetray/I3Module.h"
#include "icetray/I3ConditionalModule.h"

#include "phys-services/I3RandomService.h"
#include "earthmodel-service/EarthModelService.h"
#include "genie-icetray/I3GENIEResultDict.h"
#include "dataclasses/physics/I3MCTree.h"

#include <vector>
#include <map>
#include <string>

// forward declarations
namespace genie {
    class GEVGDriver;
};

class TRandomWrapperForI3RandomService;

namespace nugen {
I3_FORWARD_DECLARATION(Steering);
}

/**
 *
 */
class I3NuGGenie : public I3ConditionalModule
{
public:
    /**
     * Builds an instance of this class
     */
    I3NuGGenie(const I3Context& ctx);

    /**
     * Destroys an instance of this class
     */
    ~I3NuGGenie();

    /**
     * This module takes a configuration parameter and so it must be configured.
     */
    virtual void Configure();

    /**
     * The module needs to process DAQ frames
     */
    virtual void DAQ(I3FramePtr frame);

    /**
     * Warn the user if the module is aborted prematurely
     */
    virtual void Finish();


private:
    // parameters

    /// Parameter: Path to the LHAPDF data files. The directory should contain a \"PDFsets\" subdir.
    std::string LHAPDFPath_;

    /// Parameter: Spline XML filename for GENIE
    std::string splineFilename_;

    /// Parameter: material option
    std::string materialOption_;

    /// name of Steering service
    std::string steeringName_;
    boost::shared_ptr<nugen::Steering> steer_;

    /// name of primary neutrino
    std::string primaryNuName_;

    /// Parameter: The I3RandomService we are going to use. If None, the
    /// module tries to get it from the context.
    boost::python::object randomService_;

    /// internal: the I3RandomService we are going to use
    I3RandomServicePtr random_;

    double minEnergyLog_;
    double maxEnergyLog_;
    int    nKnots_for_spline_;
    double stepsize_;

    earthmodel::EarthModelServicePtr earth_;

    uint64_t eventNumber_;
    TRandomWrapperForI3RandomService *myTRandom_;

private:

   // local stuff (mostly GENIE)

   class StepInfo{

     public :
     StepInfo() : fIstep_(-1), fTotalXSec_(0), fTotalNsigma_(0), 
                  fColumnDepthIn_(-1), fColumnDepthOut_(-1), 
                  fStartLen_(-1), fEndLen_(-1),
                  fPsurvIn_(-1), fPsurvOut_(-1), 
                  fPsurvPedestalIn_(1), fPsurvPedestalOut_(1)
                  {}
     ~StepInfo() 
     {
        // do NOT delete pointers
        fDriverMap_.clear();
     }
     
     /**
      static function to calculate psurv
      */
     static void CalcPsurv(double nsigma, double cdep, 
                           double &pprob, double &ped) 
     {
         // calculate surviving probability within the step
         // TotalNsigma [cm^2/g] * dcdep [g/cm^2]
         // double surviving prob = exp(- step.fTotalNsigma_ * dcdep);
         // However, due to the accuracy limitation of double, 
         // accuracy of prob get worse if the exponential term is small.

         // To avoid the problem, try to add pedestal for exp term
         // and compensate it later.
         // pprob = exp(- step.fTotalNsigma_*dcdep * (ped+1)) 
         //       = exp(- step.fTotalNsigma_*dcdep)
         //         * exp(- step.fTotalNsigma_*dcdep * ped) ;
         // prob_orig = pprob * exp(step.fTotalNsigma_*dcdep*ped);

         static const double kExpPedestal = 1.0;
         pprob = exp(- nsigma * cdep * (kExpPedestal+1));
         ped = exp(nsigma * cdep * kExpPedestal);
     }

     double Compensate(double psurv) 
     {
        // to get original psurv, you have to compensate pedestal.
        //cerr << "ptot_psurv = " << tot_psurv << endl;
        //cerr << "pedestal   = " << fPsurvPedestal_ << endl;
        //cerr << "tot_psurv = " << tot_psurv * fPsurvPedestal_ << endl;
        
        return psurv * fPsurvPedestalOut_;
     } 

     /**
      returns a position by assuming the step is small
      enough so that we may assume the material is uniform
      */
     I3Position GetPos(double cdep) 
     {
        if (cdep < fColumnDepthIn_ || cdep > fColumnDepthOut_) {
           log_error("cdep %g is out of range[%g, %g]", cdep,
                      fColumnDepthIn_, fColumnDepthOut_);
        }
        I3Direction dir(fEndPos_ - fStartPos_);
        double dist = (fEndPos_ - fStartPos_).Magnitude();  
        double x = dist * cdep / (fColumnDepthOut_ - fColumnDepthIn_);
        return fStartPos_ + x*dir;
     }

     /**
      returns surviving probability at column depth = cdep
      Psurv = exp(- fTotalNsigma_ * cdep)
      */
     double GetPsurv(double cdep) 
     {
        if (cdep < fColumnDepthIn_ || cdep > fColumnDepthOut_) {
           log_error("cdep %g is out of range[%g, %g]", cdep,
                      fColumnDepthIn_, fColumnDepthOut_);
        } 
        double tot_psurv = fPsurvOut_;

        if (cdep == 0) {
           tot_psurv = 1.0;
        } else if (cdep < fColumnDepthOut_) {
           double dcdep = cdep - fColumnDepthIn_;
           double dPsurv, dped;
           CalcPsurv(fTotalNsigma_, dcdep, dPsurv, dped);
           tot_psurv = fPsurvIn_ * dPsurv;
        }
        return tot_psurv;
     }

     /**
      returns - d/dx (Psurv(x)) that is used to compensate linear 
      sampling of interaction position in column depth along 
      the track path.
      - d/dx(Psurv) = fTotalNsigma_ * Psurv(cdep)
      */
     double GetdPdX(double cdep) 
     {
        return fTotalNsigma_ * GetPsurv(cdep);     
     }

     void Print() 
     {
       log_debug("************************************\n");
       log_debug("StepInfo \n");
       log_debug("-------- \n");
       log_debug("istep        = %d\n", fIstep_);
       log_debug("TotalXSec    = %g\n", fTotalXSec_);
       log_debug("TotalNsigma  = %g\n", fTotalNsigma_);
       log_debug("StartLen     = %g\n", fStartLen_);
       log_debug("EndLen       = %g\n", fEndLen_);
       log_debug("ColumnDepIn  = %g\n", fColumnDepthIn_);
       log_debug("ColumnDepOut = %g\n", fColumnDepthOut_);
       log_debug("PsurvIn      = %.15g\n", fPsurvIn_);
       log_debug("PsurvOut     = %.15g\n", fPsurvOut_);
       log_debug("PsurvPedIn   = %.15g\n", fPsurvPedestalIn_);
       log_debug("PsurvPedOut  = %.15g\n", fPsurvPedestalOut_);
       log_debug("************************************\n");
     }

     /**
      Step ID
      */
     int fIstep_;

     /**
      Total cross section in this segment.
      */
     double fTotalXSec_;

     /**
      total n*sigma in this segment.
      n: number of targets a specific atom per g [1/g]
      sigma: cross section of the atom cm2 [cm2]
      In the case of compound target (e.g. H2O): 
      totalNsigma = n1*sigma1 + n2*sigma2 
      where n1 : number of hydrogen atoms
            n2 : number of oxigen atoms
            sigma1 : cross section of a hidrogen
            sigma2 : cross section of an oxigen 
      */
     double fTotalNsigma_;

     /**
      map of accumulated Nsigma within this segment
      and corresponding driver.
      ex. material H20
      index   0     1     
      nsigma  n1s1--n1s1+n2s2 (=fTotalNsigma_)
      driver  dH----dO
      */
     std::map<double, boost::shared_ptr<genie::GEVGDriver> > fDriverMap_;

     /**
      column depth markers of the step [g/cm2]
      accounted from the entrance to the
      detection area
      */
     double fColumnDepthIn_;
     double fColumnDepthOut_;

     /**
      position markers of the step
      accounted from the entrance to the
      detection area
      */
     I3Position fStartPos_;
     I3Position fEndPos_;
     double   fStartLen_;
     double   fEndLen_;

     /**
      Accumulated survival probability from the
      entrance of detection area
      Psurv(x) = exp(-n1*sigma1 + n2*sigma2)*x
      However, the stored value may be applied pedestal
      in order to avoid round-off problem.
      Check 
      */
     double fPsurvIn_; 
     double fPsurvOut_; 
     double fPsurvPedestalIn_; 
     double fPsurvPedestalOut_; 

   };

   typedef std::map< std::pair<int, int>, boost::shared_ptr<genie::GEVGDriver> > DriverMap;  
   typedef std::map< double, StepInfo > SteppingMap;  
   typedef std::map< int, double > MatRatio;
   typedef std::map< earthmodel::EarthModelService::MediumType, MatRatio > MatRatioMap;

   void GenerateDrivers();
   void GetMaterial(I3Position localp,
                    earthmodel::EarthModelService::MediumType &med,
                    double &density);

   double GetNSigma(double xsec, int A, double r);
   int    GetPDGCode(I3Particle::ParticleType p);

   void CalcSteppingInfo(int pdgcode,
                         const I3Position &enterpos,
                         const I3Position &exitpos,
                         double energy,
                         SteppingMap &stepmap,
                         double &tot_cdep);

   I3MCTreePtr GenerateMCTree(const I3GENIEResultDict &gstMap);

   MatRatioMap matratiomap_; 
   DriverMap   drivers_;

   void PrintMaps() ;

private:
    // default, assignment, and copy constructor declared private
    I3NuGGenie();
    I3NuGGenie(const I3NuGGenie&);
    I3NuGGenie& operator=(const I3NuGGenie&);


    SET_LOGGER("I3NuGGenie");
};

#endif //I3NuGGenie_H
