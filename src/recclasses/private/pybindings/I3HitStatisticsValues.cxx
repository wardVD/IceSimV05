#include "recclasses/I3HitStatisticsValues.h"
#include "wrap.h"
#include <boost/python.hpp>
#include <dataclasses/I3Position.h>

void register_I3HitStatisticsValues()
{
	using namespace boost::python;

    // Tell boost::python that it should create the python docstrings with
    // user-defined docstrings, python signatures, but no C++ signatures
    docstring_options docstring_opts(true, true, false);

    wrap::I3FrameObject<I3HitStatisticsValues>("I3HitStatisticsValues",
        "The I3HitStatisticsValues class is an I3FrameObject class which stores   \n"
        "the calculared hit statistics.                                           \n"
        )
        .def(init<const I3Position&, double, double, double, double, double, double, double, double, double, double>(
            (arg("cog"),
             arg("cog_z_sigma"),
             arg("min_pulse_time"),
             arg("max_pulse_time"),
             arg("q_max_doms"),
             arg("q_tot_pulses"),
             arg("z_min"),
             arg("z_max"),
             arg("z_mean"),
             arg("z_sigma"),
             arg("z_travel")
            )
        ))
        .add_property("cog",
            make_function(
                (I3Position& (I3HitStatisticsValues::*) ()) &I3HitStatisticsValues::GetCOG,
                return_internal_reference<1>()
            ),
            &I3HitStatisticsValues::SetCOG,
            "The I3Position object holding the position of the Center-of-Gravity  \n"
            "(CoG) of the event."
        )
        .add_property("cog_z_sigma",
            &I3HitStatisticsValues::GetCOGZSigma,
            &I3HitStatisticsValues::SetCOGZSigma,
            "The sigma value of the z-component of the COG position."
        )
        .add_property("min_pulse_time",
            &I3HitStatisticsValues::GetMinPulseTime,
            &I3HitStatisticsValues::SetMinPulseTime,
            "The minimal time of all pulses, e.g. the time of the first pulse."
        )
        .add_property("max_pulse_time",
            &I3HitStatisticsValues::GetMaxPulseTime,
            &I3HitStatisticsValues::SetMaxPulseTime,
            "The maximal time of all pulses, e.g. the time of the last pulse."
        )
        .add_property("q_max_doms",
            &I3HitStatisticsValues::GetQMaxDoms,
            &I3HitStatisticsValues::SetQMaxDoms,
            "The maximal value of all DOM pulse charge sums."
        )
        .add_property("q_tot_pulses",
            &I3HitStatisticsValues::GetQTotPulses,
            &I3HitStatisticsValues::SetQTotPulses,
            "The total charge of all pulses."
        )
        .add_property("z_min",
            &I3HitStatisticsValues::GetZMin,
            &I3HitStatisticsValues::SetZMin,
            "The minimal OM_Z value of all hits."
        )
        .add_property("z_max",
            &I3HitStatisticsValues::GetZMax,
            &I3HitStatisticsValues::SetZMax,
            "The maximal OM_Z value of all hits."
        )
        .add_property("z_mean",
            &I3HitStatisticsValues::GetZMean,
            &I3HitStatisticsValues::SetZMean,
            "The mean of all OM_Z values."
        )
        .add_property("z_sigma",
            &I3HitStatisticsValues::GetZSigma,
            &I3HitStatisticsValues::SetZSigma,
            "The sigma (RMSD) of all OM_Z values."
        )
        .add_property("z_travel",
            &I3HitStatisticsValues::GetZTravel,
            &I3HitStatisticsValues::SetZTravel,
            "The average over (OM_Z minus the average over the OM_Z values of the \n"
            "first quartile of all hit DOMs), thus an OM_Z value.                 \n"
        )
    ;
}
