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

#include "tableio/converter/I3EventHeaderConverter.h"

I3TableRowDescriptionPtr I3EventHeaderConverter::CreateDescription(const I3EventHeader& header) {
    
    I3TableRowDescriptionPtr desc(new I3TableRowDescription());
    desc->AddField<int64_t>("time_start_utc_daq", "1e-10 s", "start of event in daq time - tenths of nanoseconds since the start of the UTC year");
    // utc times seem to be redundant 
    //desc->AddField<int32_t>("time_start_utc_year","",        "start of event utc time - year");
    //desc->AddField<int32_t>("time_start_utc_sec", "s",       "start of event in utc time - seconds");
    //desc->AddField<double> ("time_start_utc_ns",  "ns",      "start of event in utc time - nanoseconds");
    desc->AddField<int32_t>("time_start_mjd_day", "days",    "start of event in Modified Julian Days - days");
    desc->AddField<int32_t>("time_start_mjd_sec", "seconds", "start of event in Modified Julian Days - seconds");
    desc->AddField<double> ("time_start_mjd_ns",  "ns",      "start of event in Modified Julian Days - nanoseconds");
    
    desc->AddField<int64_t>("time_end_utc_daq", "1e-10 s", "end of event in daq time - tenths of nanoseconds since the start of the UTC year");
    //desc->AddField<int32_t>("time_end_utc_year","",        "end of event in utc time - year");
    //desc->AddField<int32_t>("time_end_utc_sec", "s",       "end of event in utc time - seconds");
    //desc->AddField<double> ("time_end_utc_ns",  "ns",      "end of event in utc time - nanoseconds");
    desc->AddField<int32_t>("time_end_mjd_day", "days",    "end of event in Modified Julian Days - days");
    desc->AddField<int32_t>("time_end_mjd_sec", "seconds", "end of event in Modified Julian Days - seconds");
    desc->AddField<double> ("time_end_mjd_ns",  "ns",      "end of event in Modified Julian Days - nanoseconds");
    return desc;
}
    
size_t I3EventHeaderConverter::FillRows(const I3EventHeader& header,
                                        I3TableRowPtr rows) {
    I3Time start = header.GetStartTime();
    I3Time end = header.GetEndTime();

    rows->Set<int64_t>("time_start_utc_daq", start.GetUTCDaqTime());
    rows->Set<int32_t>("time_start_mjd_day", start.GetModJulianDay());
    rows->Set<int32_t>("time_start_mjd_sec", start.GetModJulianSec());
    rows->Set<double >("time_start_mjd_ns",  start.GetModJulianNanoSec());
    
    rows->Set<int64_t>("time_end_utc_daq", end.GetUTCDaqTime());
    rows->Set<int32_t>("time_end_mjd_day", end.GetModJulianDay());
    rows->Set<int32_t>("time_end_mjd_sec", end.GetModJulianSec());
    rows->Set<double >("time_end_mjd_ns",  end.GetModJulianNanoSec());

    return 1;
}
