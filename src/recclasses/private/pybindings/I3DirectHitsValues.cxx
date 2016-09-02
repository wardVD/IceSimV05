#include "recclasses/I3DirectHitsValues.h"
#include <boost/python.hpp>
#include "wrap.h"

void register_I3DirectHitsValues()
{
	using namespace boost::python;

    // Tell boost::python that it should create the python docstrings with
    // user-defined docstrings, python signatures, but no C++ signatures.
    docstring_options docstring_opts(true, true, false);

    wrap::I3FrameObject<I3DirectHitsValues>("I3DirectHitsValues",
        "The I3DirectHitsValues class is an I3FrameObject class which stores      \n"
        "information about the calculared direct hits.                            \n"
        )
        .def(init<>(
            "Constructs an I3DirectHitsValues object with initial values set to 0, or \n"
            "nan."
        ))
        .def(init<
             uint32_t,
             uint32_t,
             uint64_t,
             double,
             uint32_t,
             uint32_t,
             uint64_t,
             double,
             uint32_t,
             uint32_t,
             uint64_t,
             double,
             double,
             double
            >(
            (arg("n_dir_strings"),
             arg("n_dir_doms"),
             arg("n_dir_pulses"),
             arg("q_dir_pulses"),
             arg("n_early_strings"),
             arg("n_early_doms"),
             arg("n_early_pulses"),
             arg("q_early_pulses"),
             arg("n_late_strings"),
             arg("n_late_doms"),
             arg("n_late_pulses"),
             arg("q_late_pulses"),
             arg("dir_track_length"),
             arg("dir_track_hit_distribution_smoothness")
            ),
            "Constructs an I3DirectHitsValues object with initial values."
        ))
        .add_property("n_dir_strings",
            &I3DirectHitsValues::GetNDirStrings,
            &I3DirectHitsValues::SetNDirStrings,
            "The total number of strings, which have at least one direct pulse. A     \n"
            "direct pulse has a time residual, that is inside the given direct hits   \n"
            "time window."
        )
        .add_property("n_dir_doms",
            &I3DirectHitsValues::GetNDirDoms,
            &I3DirectHitsValues::SetNDirDoms,
            "The total number of DOMs, which have at least one direct pulse. A direct \n"
            "pulse has a time residual, that is inside the given direct hits time     \n"
            "window."
        )
        .add_property("n_dir_pulses",
            &I3DirectHitsValues::GetNDirPulses,
            &I3DirectHitsValues::SetNDirPulses,
            "The total number of direct pulses. A direct pulse has a time residual,   \n"
            "that is inside the given direct hits time window."
        )
        .add_property("q_dir_pulses",
            &I3DirectHitsValues::GetQDirPulses,
            &I3DirectHitsValues::SetQDirPulses,
            "The total charge of all direct pulses. A direct pulse has a time         \n"
            "residual, that is inside the given direct hits time window."
        )
        .add_property("n_early_strings",
            &I3DirectHitsValues::GetNEarlyStrings,
            &I3DirectHitsValues::SetNEarlyStrings,
            "The total number of strings, which have at least one early pulse. An     \n"
            "early pulse has a time residual, that is before the direct hits time     \n"
            "window."
        )
        .add_property("n_early_doms",
            &I3DirectHitsValues::GetNEarlyDoms,
            &I3DirectHitsValues::SetNEarlyDoms,
            "The total number of DOMs, which have at least one early pulse. An early  \n"
            "pulse has a time residual, that is before the direct hits time window."
        )
        .add_property("n_early_pulses",
            &I3DirectHitsValues::GetNEarlyPulses,
            &I3DirectHitsValues::SetNEarlyPulses,
            "The total number of pulses before the given time window."
        )
        .add_property("q_early_pulses",
            &I3DirectHitsValues::GetQEarlyPulses,
            &I3DirectHitsValues::SetQEarlyPulses,
            "The total charge of all early pulses."
        )
        .add_property("n_late_strings",
            &I3DirectHitsValues::GetNLateStrings,
            &I3DirectHitsValues::SetNLateStrings,
            "The total number of strings, which have at least one late pulse. A late  \n"
            "pulse has a time residual, that is after the direct hits time window."
        )
        .add_property("n_late_doms",
            &I3DirectHitsValues::GetNLateDoms,
            &I3DirectHitsValues::SetNLateDoms,
            "The total number of DOMs, which have at least one late pulse. A late     \n"
            "pulse has a time residual, that is after the direct hits time window."
        )
        .add_property("n_late_pulses",
            &I3DirectHitsValues::GetNLatePulses,
            &I3DirectHitsValues::SetNLatePulses,
            "The total number of pulses after the given time window."
        )
        .add_property("q_late_pulses",
            &I3DirectHitsValues::GetQLatePulses,
            &I3DirectHitsValues::SetQLatePulses,
            "The total charge of all late pulses. A late pulse "
        )
        .add_property("dir_track_length",
            &I3DirectHitsValues::GetDirTrackLength,
            &I3DirectHitsValues::SetDirTrackLength,
            "The length of the track, which is defined as the distance along the      \n"
            "track from the first hit DOM to the last hit DOM perpendicular to the    \n"
            "track direction.                                                         \n"
        )
        .add_property("dir_track_hit_distribution_smoothness",
            &I3DirectHitsValues::GetDirTrackHitDistributionSmoothness,
            &I3DirectHitsValues::SetDirTrackHitDistributionSmoothness,
            "The smoothness value, based on the direct hit DOMs: How uniformly        \n"
            "distributed are the direct hit DOM projections onto the track.           \n"
            "This is a value between [-1;+1], where 0 means uniformly distributed.    \n"
        )
    ;

    wrap::I3FrameObject<I3DirectHitsValuesMap>("I3DirectHitsValuesMap")
    ;
}
