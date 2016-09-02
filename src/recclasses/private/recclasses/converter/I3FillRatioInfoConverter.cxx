/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> $LastChangedBy$
 */

#include "I3FillRatioInfoConverter.h"

I3TableRowDescriptionPtr I3FillRatioInfoConverter::CreateDescription(const I3FillRatioInfo& params) {
  I3TableRowDescriptionPtr desc(new I3TableRowDescription());

    desc->AddField<double>("mean_distance", "m", "mean distance between hit OMs and the supplied vertex");
    desc->AddField<double>("rms_distance", "m", "rms of distance between hit OMs and the supplied vertex");
    desc->AddField<double>("nch_distance", "m", "SPE Radius, estimated from the NCh");
    desc->AddField<double>("energy_distance", "m", "SPE Radius, estimated from the energy estimate");

    desc->AddField<double>("fillradius_from_rms", "m", "radius of the sphere defined by the rms");
    desc->AddField<double>("fillradius_from_mean", "m", "radius of the sphere defined by the mean");
    desc->AddField<double>("fillradius_from_mean_plus_rms", "m", "radius of the sphere defined by the mean and rms");
    desc->AddField<double>("fillradius_from_nch", "m", "radius of the sphere defined by the NCh");
    desc->AddField<double>("fillradius_from_energy", "m", "radius of the sphere defined by the energy estimate");
    
    desc->AddField<double>("fillratio_from_rms", "", "fraction of OMs that are inside the rms-defined sphere");
    desc->AddField<double>("fillratio_from_mean", "", "fraction of OMs that are inside the mean-defined sphere");
    desc->AddField<double>("fillratio_from_mean_plus_rms", "", "fraction of OMs that are inside the (mean+rms)-defined sphere");
    desc->AddField<double>("fillratio_from_nch", "", "fraction of OMs that are inside the NCh-defined sphere");
    desc->AddField<double>("fillratio_from_energy", "", "fraction of OMs that are inside the sphere defined by the energy estimate");

    desc->AddField<int32_t>("hit_count", "", "number of OMs that were present in this event");

    return desc;
}
    
size_t I3FillRatioInfoConverter::FillRows(const I3FillRatioInfo& params,
					  I3TableRowPtr rows) {
    rows->Set<double>("mean_distance",   params.GetMeanDistance());
    rows->Set<double>("rms_distance",    params.GetRMSDistance());
    rows->Set<double>("nch_distance",    params.GetNChDistance());
    rows->Set<double>("energy_distance", params.GetEnergyDistance());

    rows->Set<double>("fillradius_from_rms",           params.GetFillRadiusFromRMS());
    rows->Set<double>("fillradius_from_mean",          params.GetFillRadiusFromMean());
    rows->Set<double>("fillradius_from_mean_plus_rms", params.GetFillRadiusFromMeanPlusRMS());
    rows->Set<double>("fillradius_from_nch",           params.GetFillRadiusFromNCh() );
    rows->Set<double>("fillradius_from_energy",        params.GetFillRadiusFromEnergy());
    
    rows->Set<double>("fillratio_from_rms",           params.GetFillRatioFromRMS());
    rows->Set<double>("fillratio_from_mean",          params.GetFillRatioFromMean());
    rows->Set<double>("fillratio_from_mean_plus_rms", params.GetFillRatioFromMeanPlusRMS());
    rows->Set<double>("fillratio_from_nch",           params.GetFillRatioFromNCh());
    rows->Set<double>("fillratio_from_energy",        params.GetFillRatioFromEnergy());

    rows->Set<int32_t>("hit_count", params.GetHitCount());

    return 1;
}
