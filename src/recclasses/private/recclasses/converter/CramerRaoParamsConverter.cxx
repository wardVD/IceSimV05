/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3CramerRaoParamsConverter.cxx 87997 2012-05-07 00:07:52Z jvansanten $
 *
 * @version $Revision: 87997 $
 * @date $LastChangedDate: 2012-05-07 02:07:52 +0200 (Mon, 07 May 2012) $
 * @author Fabian Kislat <fabian.kislat@desy.de>, last changed by $LastChangedBy: jvansanten $
 */

#include "CramerRaoParamsConverter.h"

I3TableRowDescriptionPtr
CramerRaoParamsConverter::CreateDescription(const CramerRaoParams &p) {
	I3TableRowDescriptionPtr desc(new I3TableRowDescription());
	desc->AddField<double>("cramer_rao_theta", "radian", "Estimated Zenith Error");
	desc->AddField<double>("cramer_rao_phi", "radian", "Estimated Azimuthal Error");
	desc->AddField<double>("variance_theta", "radian^2",
	    "Variance in Zenith");
	desc->AddField<double>("variance_phi", "radian^2", "Variance in Azimuth");
	desc->AddField<double>("variance_x", "m^2", "Variance in Track X");
	desc->AddField<double>("variance_y", "m^2", "Variance in Track Y");
	desc->AddField<double>("covariance_theta_phi", "radian^2",
	    "Zenith/Azimuth Covariance");
	desc->AddField<double>("covariance_theta_x", "radian-m",
	    "Zenith/X Covariance");
	desc->AddField<double>("covariance_theta_y", "radian-m",
	    "Zenith/Y Covariance");
	desc->AddField<double>("covariance_phi_x", "radian-m",
	    "Azimuth/X Covariance");
	desc->AddField<double>("covariance_phi_y", "radian-m",
	    "Azimuth/Y Covariance");
	desc->AddField<double>("covariance_x_y", "m^2", "X/Y Covariance"); 
	
	/* Remainder of parameters unused as of r67870 */
	
	MAKE_ENUM_VECTOR(status, CramerRaoParams, CramerRaoParams::CramerRaoStatus, (NotSet)(OK)(MissingInput)(SingularMatrix)(InsufficientHits)(OneStringEvent)(OtherProblems));
	desc->AddEnumField<CramerRaoParams::CramerRaoStatus>("status", status,
	    "", "Status of Error Estimation");

	return desc;
}

size_t
CramerRaoParamsConverter::FillRows(const CramerRaoParams &p, I3TableRowPtr rows) {
	rows->Set<double>("cramer_rao_theta", p.cramer_rao_theta);
	rows->Set<double>("cramer_rao_phi", p.cramer_rao_phi);
	rows->Set<double>("variance_theta", p.variance_theta);
	rows->Set<double>("variance_phi", p.variance_phi);
	rows->Set<double>("variance_x", p.variance_x);
	rows->Set<double>("variance_y", p.variance_y);
	rows->Set<double>("covariance_theta_phi", p.covariance_theta_phi);
	rows->Set<double>("covariance_theta_x", p.covariance_theta_x);
	rows->Set<double>("covariance_theta_y", p.covariance_theta_y);
	rows->Set<double>("covariance_phi_x", p.covariance_phi_x);
	rows->Set<double>("covariance_phi_y", p.covariance_phi_y);
	rows->Set<double>("covariance_x_y", p.covariance_x_y);
	rows->Set<CramerRaoParams::CramerRaoStatus>("status", p.status);

	return 1;
}
