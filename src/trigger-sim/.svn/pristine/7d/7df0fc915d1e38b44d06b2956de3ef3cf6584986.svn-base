//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id$
//
//   This file is part of IceTray.
//
//   IceTray is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
//   IceTray is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <vector>

#include <trigger-sim/algorithms/GlobalTriggerSim.h>

using namespace boost::python;

static bool
HasThroughputTriggers(const I3TriggerHierarchy &triggers)
{
	I3TriggerHierarchy::iterator it = triggers.begin();
	for ( ; it != triggers.end(); it++)
		switch (it->GetTriggerKey().GetType()) {
			case TriggerKey::THROUGHPUT:
			case TriggerKey::MERGED:
				return true;
				break;
			default:
				break;
		}
	return false;
}

static bool
compare(const std::pair<I3Trigger,I3Trigger> &tp1, const std::pair<I3Trigger,I3Trigger> &tp2)
{
	return tp1.first.GetTriggerTime() < tp2.first.GetTriggerTime();
}

static I3TriggerHierarchyPtr
Reinflate(GlobalTriggerSim &self, I3TriggerHierarchyPtr subtriggers)
{
	if (HasThroughputTriggers(*subtriggers)) {
		PyErr_SetString(PyExc_ValueError, "The trigger tree already has global triggers!");
		throw error_already_set();
	}
	
	std::vector<I3Trigger> flat_triggers;
	I3TriggerPairVector trigger_pairs;
	std::copy(subtriggers->begin(), subtriggers->end(),
	    std::back_inserter(flat_triggers));
	self.InsertThroughputTriggers(flat_triggers, trigger_pairs);
	
	// Ensure that the merger sees throughput triggers ordered in time,
	// otherwise it misses overlaps.
	std::sort(trigger_pairs.begin(), trigger_pairs.end(), compare);
	
	return self.Merge(trigger_pairs);
}

void register_GlobalTriggerSim()
{

  class_<GlobalTriggerSim, boost::shared_ptr<GlobalTriggerSim> >
    ("GlobalTriggerSim",init<I3DetectorStatus>())
    .def("Merge", &GlobalTriggerSim::Merge)
    .def("InsertThroughputTriggers", &GlobalTriggerSim::InsertThroughputTriggers)
    .def("Inflate", &Reinflate)
    ;

} 
