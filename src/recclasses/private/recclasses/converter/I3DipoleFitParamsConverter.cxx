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

#include "I3DipoleFitParamsConverter.h"

I3TableRowDescriptionPtr I3DipoleFitParamsConverter::CreateDescription(const I3DipoleFitParams& params) {
    I3TableRowDescriptionPtr desc(new I3TableRowDescription());
    desc->AddField<double>("magnet", "FIXME" , "FIXME");
    desc->AddField<double>("magnet_x", "FIXME" , "FIXME");
    desc->AddField<double>("magnet_y", "FIXME" , "FIXME");
    desc->AddField<double>("magnet_z", "FIXME" , "FIXME");
    desc->AddField<double>("ampsum", "FIXME" , "FIXME");
    desc->AddField<double>("maxamp", "FIXME" , "FIXME");
    desc->AddField<int32_t>("nhits", "" , "FIXME");
    desc->AddField<int32_t>("npairs", "" , "FIXME");
    return desc;
}
    
size_t I3DipoleFitParamsConverter::FillRows(const I3DipoleFitParams& params,
                                     I3TableRowPtr rows) {

    rows->Set<double>("magnet"  , params.magnet);
    rows->Set<double>("magnet_x", params.magnetX);
    rows->Set<double>("magnet_y", params.magnetY);
    rows->Set<double>("magnet_z", params.magnetZ);
    rows->Set<double>("ampsum"  , params.ampSum);
    rows->Set<double>("maxamp"  , params.maxAmp);
    rows->Set<int32_t>("nhits"  , params.nHits);
    rows->Set<int32_t>("npairs" , params.nPairs);

    return 1;
}
