#include "recclasses/I3TrackCharacteristicsValues.h"
#include "wrap.h"
#include <boost/python.hpp>

void register_I3TrackCharacteristicsValues()
{
	using namespace boost::python;

    // Tell boost::python that it should create the python docstrings with
    // user-defined docstrings, python signatures, but no C++ signatures
    docstring_options docstring_opts(true, true, false);

    wrap::I3FrameObject<I3TrackCharacteristicsValues>("I3TrackCharacteristicsValues",
        "The I3TrackCharacteristicsValues class is an I3FrameObject class which   \n"
        "stores the calculared track characteristics.                             \n"
        )
        .def(init<double, double, double, double>(
            (arg("avg_dom_dist_q_tot_dom"),
             arg("empty_hits_track_length"),
             arg("track_hits_separation_length"),
             arg("track_hits_distribution_smoothness")
            )
        ))
        .add_property("avg_dom_dist_q_tot_dom",
            &I3TrackCharacteristicsValues::GetAvgDomDistQTotDom,
            &I3TrackCharacteristicsValues::SetAvgDomDistQTotDom,
            "The average DOM distance from the track weighted by the total charge \n"
            "of each DOM."
        )
        .add_property("empty_hits_track_length",
            &I3TrackCharacteristicsValues::GetEmptyHitsTrackLength,
            &I3TrackCharacteristicsValues::SetEmptyHitsTrackLength,
            "The maximal track length of the track, which got no hits within the  \n"
            "specified track cylinder radius."
        )
        .add_property("track_hits_distribution_smoothness",
            &I3TrackCharacteristicsValues::GetTrackHitsDistributionSmoothness,
            &I3TrackCharacteristicsValues::SetTrackHitsDistributionSmoothness,
            "The track hits distribution smoothness value [-1.;+1.] how smooth the\n"
            "hits of the given I3RecoPulseSeriesMap, within the specified track   \n"
            "cylinder radius, are distributed along the track."
        )
        .add_property("track_hits_separation_length",
            &I3TrackCharacteristicsValues::GetTrackHitsSeparationLength,
            &I3TrackCharacteristicsValues::SetTrackHitsSeparationLength,
            "The length how far the COG positions of the first and the last       \n"
            "quartile of the hits, within the specified track cylinder radius, are\n"
            "separated from each other."
        )
    ;
}
