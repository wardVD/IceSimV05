/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Fabian Kislat <fabian.kislat@desy.de> $LastChangedBy$
 */

#include "I3LineFitParamsConverter.h"


I3TableRowDescriptionPtr
I3LineFitParamsConverter::CreateDescription(const I3LineFitParams &params) {
  I3TableRowDescriptionPtr desc(new I3TableRowDescription());

  desc->AddField<double>("lf_vel", "Gm/s",
			 "Velocity of the fitted track (from LineFit)");
  desc->AddField<double>("lf_vel_x", "Gm/s",
			 "x-component of the velocity of the fitted track");
  desc->AddField<double>("lf_vel_y", "Gm/s",
			 "y-component of the velocity of the fitted track");
  desc->AddField<double>("lf_vel_z", "Gm/s",
			 "z-component of the velocity of the fitted track");
  desc->AddField<int32_t>("n_hits", "number",
			  "Number of reconstructed hits used to calculate the fit");

  return desc;
}
    
size_t I3LineFitParamsConverter::FillRows(const I3LineFitParams &params,
					  I3TableRowPtr rows) {

  rows->Set<double>("lf_vel", params.LFVel);
  rows->Set<double>("lf_vel_x", params.LFVelX);
  rows->Set<double>("lf_vel_y", params.LFVelY);
  rows->Set<double>("lf_vel_z", params.LFVelZ);
  rows->Set<int32_t>("n_hits", params.nHits);

  return 1;
}
