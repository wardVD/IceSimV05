/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Fabian Kislat <fabian.kislat@desy.de> , last changed by $LastChangedBy$
 */

#include "I3TopRecoPlaneFitParamsConverter.h"
#include <icetray/I3Units.h>

I3TableRowDescriptionPtr I3TopRecoPlaneFitParamsConverter::CreateDescription(const I3TopRecoPlaneFitParams& params)
{
    I3TableRowDescriptionPtr desc(new I3TableRowDescription());

    desc->AddField<int64_t> ("event_time", "s", "The event time in secs since year began");
    desc->AddField<double>  ("delta_t", "s", "The time in secs which passed since the last event");
    desc->AddField<double>  ("t0", "s", "The t0 of the track");
    desc->AddField<double>  ("x0", "m", "The x0 of the track");
    desc->AddField<double>  ("y0", "m", "The y0 of the track");
    desc->AddField<double>  ("chi2", "", "The chi2");
    desc->AddField<uint32_t>("nPulses", "", "The number of pulses used in this fit");

    return desc;
}
    
size_t I3TopRecoPlaneFitParamsConverter::FillRows(const I3TopRecoPlaneFitParams& params,
						I3TableRowPtr rows)
{
    rows->Set<int64_t> ("event_time", params.ET);
    rows->Set<double>  ("delta_t", params.DeltaT/I3Units::s);
    rows->Set<double>  ("t0", params.T0/I3Units::s);
    rows->Set<double>  ("x0", params.X0);
    rows->Set<double>  ("y0", params.Y0);
    rows->Set<double>  ("chi2", params.Chi2);
    rows->Set<uint32_t>("nPulses", params.NPulses);

    return 1;
}
