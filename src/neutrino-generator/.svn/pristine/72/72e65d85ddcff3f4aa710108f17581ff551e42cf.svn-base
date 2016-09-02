/**
 * copyright (C) 2011
 * $Id: $
 *
 * @file Step.h
 * @version $Revision$
 * @date $Date: $
 * @author Kotoyo Hoshina
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

#ifndef NuGStep_H
#define NuGStep_H

#include <vector>
#include <map>
#include <string>

/**
 * @brief a container class of a step
 * step object is used for making interaction within 
 * detection area. A detection volume is divided with 
 * several "steps". Each step must have (apploximately) 
 * uniform profile in terms of material components and density.
 * Note that the "step" instants always hold "unweighted" results.
 * Weighting interaction should be managed by InteractionManagerBase.
 */

namespace nugen {

class Step {

  public :
  Step() : istep_(-1), total_xsec_(0), total_xsec_per_gram_(0), 
           column_depth_in_(0), column_depth_out_(0), 
           start_len_(0), end_len_(0),
           psurv_in_(1.0), psurv_out_(1.0), lowEApproxLimit_(1e-3)
           {}
  virtual ~Step() 
  {
  }

  /**
   * @brief static function to calculate psurv
   * calculate surviving probability within the step
   *
   * @param[in] totxsec_per_gram = total cross section per gram [cm^2/g] 
   * @param[in] cdep = column depth in [g/cm^2]
   * @param[in] lowEapproxLimit (default 1e-3, which gives reasonablly 
   *            good agreement with precise equation)
   * @return    pprob = surviving probability
   */
  const double CalcPsurv(double totxsec_per_gram, double cdep) const
  {
      if (totxsec_per_gram * cdep < lowEApproxLimit_) {
         // due to the accuracy limitation of double, 
         // accuracy of prob will be worse if the exponential 
         // term is small.
         // use approximate form instead of exp.
         return  1.0 - totxsec_per_gram * cdep;

      } 
      return exp(- totxsec_per_gram * cdep);
  }

  /**
   * @brief returns a position within the step 
   * as a function of column depth 
   * the step must be small enough so that 
   * we may assume the material is uniform
   *
   * @param[in] cdep column depth
   * @return I3Position position
   */
  I3Position GetPos(double cdep) const
  {
     if (cdep < column_depth_in_ || cdep > column_depth_out_) {
        log_error("cdep %g is out of range[%g, %g]", cdep,
                   column_depth_in_, column_depth_out_);
     }
     I3Direction dir(end_pos_ - start_pos_);  
     double dist = (end_pos_ - start_pos_).Magnitude();  
     double x = dist * cdep / (column_depth_out_ - column_depth_in_);
     return start_pos_ + x*dir;
  }

  /**
   * @brief returns surviving probability at column depth = cdep
   * Psurv = exp(- total_xsec_per_gram_ * cdep)
   * @param[in] cdep column depth
   * @return double total surviving probability
   */
  double GetPsurv(double cdep) const
  {
     if (cdep < column_depth_in_ || cdep > column_depth_out_) {
        log_error("cdep %g is out of range[%g, %g]", cdep,
                   column_depth_in_, column_depth_out_);
     } 
     double tot_psurv = psurv_out_;

     if (cdep == 0) {
        tot_psurv = 1.0;
     } else if (cdep < column_depth_out_) {
        double dcdep = cdep - column_depth_in_;
        double dPsurv = CalcPsurv(total_xsec_per_gram_, 
                                  dcdep);
        tot_psurv = psurv_in_ * dPsurv;
     }
     return tot_psurv;
  }

  /**
   * @brief returns - (d/dx) (Psurv(x)) that is used to 
   * compensate interaction position which is sampled
   * from flat distribution in column depth along the track path.
   * Psurv = exp(-total_xsec_per_gram_*cdep)
   * - d(Psurv)/d(cdep) = total_xsec_per_gram_ * Psurv(cdep)
   * Note that the return value is not normalized.
   *
   * @param[in] cdep column depth
   * @return double -dPsurv(cdep)/d(cdep)
   */
  double GetdPdX(double cdep) const
  {
     return total_xsec_per_gram_ * GetPsurv(cdep);     
  }

  void Print() const
  {
    log_debug("************************************\n");
    log_debug("Step ");
    log_debug("-------- ");
    log_debug("istep        = %d", istep_);
    log_debug("TotalXSec    = %g", total_xsec_);
    log_debug("TotalXsecPerGram  = %g", total_xsec_per_gram_);
    log_debug("StartLen     = %g", start_len_);
    log_debug("EndLen       = %g", end_len_);
    log_debug("ColumnDepIn  = %g", column_depth_in_);
    log_debug("ColumnDepOut = %g", column_depth_out_);
    log_debug("PsurvIn      = %.15g", psurv_in_);
    log_debug("PsurvOut     = %.15g", psurv_out_);
    log_debug("LowEApproxLimit = %.15g", lowEApproxLimit_);
    log_debug("************************************\n");
  }


  /**
   * Step ID
   */
  int istep_;

  /**
   * Total cross section in this segment.
   */
  double total_xsec_;

  /**
    @brief total Xsec per gram in this segment.
    n: number of targets of a specific atom per g [1/g]
    sigma: cross section of the atom cm2 [cm2]

    example for GENIE :
    In the case of compound target (e.g. H2O): 
    total_xsec_per_gram_ = n1*sigma1 + n2*sigma2 
    where n1 : number of hydrogen atoms in 1g medium
          n2 : number of oxigen atoms in 1g medium
          sigma1 : cross section of a hidrogen
          sigma2 : cross section of an oxigen 

    example for NUGEN:
    In the case of SiO2
    total_xsec_per_gram_ = n1*sigma1 + n2*sigma2 + n3*sigma3 
    where n1 : number of protons in 1g SiO2
          n2 : number of neutrons in 1g SiO2
          n3 : number of electrons in 1g SiO2
          sigma1 : cross section of a proton 
          sigma2 : cross section of a neutron
          sigma3 : cross section of an electron 
 
   */
  double total_xsec_per_gram_;

  /**
   @brief column depth at the beginning of the step
   measured from the entrance to the detection area
   [g/cm2]
   */
  double column_depth_in_;

  /**
   @brief column depth at the end of the step
   measured from the entrance to the detection area
   [g/cm2]
   */
  double column_depth_out_;

  /**
   @brief position of the beginning of the step
   */
  I3Position start_pos_;

  /**
   @brief position of the end of the step
   */
  I3Position end_pos_;

  /**
   @brief distance from the entrance to the detection
   area to the beginning of the step [m]
   */
  double   start_len_;

  /**
   @brief distance from the entrance to the detection
   area to the end of the step [m]
   */
  double   end_len_;

  /**
   @brief Survival probability from the entrance of detection area
   to the beginning of the step
   */
  double psurv_in_; 

  /**
   @brief Survival probability from the entrance of detection area
   to the end of the step
   */
  double psurv_out_; 

  /**
   * limit value to use simple calculation for Psurv.
   * Default value 1e-3 is obtained from a ratio plot
   * r = 10^(-x)/(1-exp(-10^(-x))
   */
  double lowEApproxLimit_;

    SET_LOGGER("NuG");
};

typedef std::map< double, Step > DoubleStepMap;  

}

#endif 
