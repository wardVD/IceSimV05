#include "I3ShieldHitRecordConverter.h"

I3TableRowDescriptionPtr I3ShieldHitRecordConverter::CreateDescription(const I3ShieldHitRecord &r){
	I3TableRowDescriptionPtr desc(new I3TableRowDescription());
	desc->AddField<double>("time_offset", "nanoseconds", "Arrival Time Delay");
	desc->AddField<double>("distance", "meters", "Perpendicular distance from track");
	desc->AddField<int>("string", "string", "String key for the pulse");
	desc->AddField<unsigned int>("DOM", "DOM", "DOM key for the pulse");
	desc->AddField<unsigned char>("PMT", "PMT", "PMT key for the pulse");
	return desc;
}

size_t I3ShieldHitRecordConverter::FillRows(const I3ShieldHitRecord &r, I3TableRowPtr rows){	
	rows->Set<double>("time_offset", r.GetTimeResidual());
	rows->Set<double>("distance", r.GetDistance());
	OMKey DOMkey = r.GetDOMkey();
	rows->Set<int>("string", DOMkey.GetString());
	rows->Set<unsigned int>("DOM", DOMkey.GetOM());
	rows->Set<unsigned char>("PMT", DOMkey.GetPMT());
	return(1);
}
