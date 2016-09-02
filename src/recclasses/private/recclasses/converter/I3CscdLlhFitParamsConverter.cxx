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

#include "I3CscdLlhFitParamsConverter.h"

I3TableRowDescriptionPtr I3CscdLlhFitParamsConverter::CreateDescription(const I3CscdLlhFitParams& params) {
    I3TableRowDescriptionPtr desc(new I3TableRowDescription());
    desc->AddField<int32_t>("hit_count",      ""   , "number of hits in the event");
    desc->AddField<int32_t>("hit_om_count",   ""   , "number of hit OMs in the event");
    desc->AddField<int32_t>("unhit_om_count", ""   , "number of active OMs that were not hit");
    desc->AddField<int32_t>("status",         ""   , "status flag of the minimizer");
    desc->AddField<double>("error_time",      "ns" , "error of the vertex time");
    desc->AddField<double>("error_x",         "m"  , "error of the vertex x coordinate");
    desc->AddField<double>("error_y",         "m"  , "error of the vertex y coordinate");
    desc->AddField<double>("error_z",         "m"  , "error of the vertex z coordinate");
    // omitting theta and phi coordinates here! 
    desc->AddField<double>("error_energy",    ""   , "error of the reconstructed energy");
    desc->AddField<double>("logL",            ""   , "negative log likelihood at the minimum");
    desc->AddField<double>("rlogL",           ""   , "reduced negative log likelihood at the minimum");

    return desc;
}
    
size_t I3CscdLlhFitParamsConverter::FillRows(const I3CscdLlhFitParams& params,
                                     I3TableRowPtr rows) {
    rows->Set<int32_t>("hit_count",     params.GetHitCount());
    rows->Set<int32_t>("hit_om_count",  params.GetHitOmCount());
    rows->Set<int32_t>("unhit_om_count",params.GetUnhitOmCount());
    rows->Set<int32_t>("status",        params.GetStatus());
    rows->Set<double>("error_time",     params.GetErrT());
    rows->Set<double>("error_x",        params.GetErrX());
    rows->Set<double>("error_y",        params.GetErrY());
    rows->Set<double>("error_z",        params.GetErrZ());
    rows->Set<double>("error_energy",   params.GetErrEnergy());
    rows->Set<double>("logL",           params.GetNegLlh());
    rows->Set<double>("rlogL",          params.GetReducedLlh());

    return 1;
}
