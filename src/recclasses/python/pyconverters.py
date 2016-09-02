# $Id: converters.py 93523 2012-09-26 09:16:32Z jacobi $
#
# Copyright (C) 2012
# Martin Wolf <martin.wolf@icecube.wisc.edu>
# and the IceCube Collaboration <http://www.icecube.wisc.edu>
#
# Version: $Revision: 93523 $
# Date:    $Date: 2012-09-26 05:16:32 -0400 (Wed, 26 Sep 2012) $
#
# This module is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This module is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>

from icecube import tableio, dataclasses, icetray
from icecube.icetray import I3Units
from icecube.recclasses import I3DirectHitsValues
from icecube.recclasses import I3Veto, I3VetoShort
from icecube.recclasses import I3HitMultiplicityValues
from icecube.recclasses import I3HitStatisticsValues
from icecube.recclasses import I3TimeCharacteristicsValues
from icecube.recclasses import I3TrackCharacteristicsValues


class I3DirectHitsValuesConverter(tableio.I3Converter):
    """The I3DirectHitsValuesConverter tableio converter converts an
    I3DirectHitsValues frame object into a data table verbatim.

    """
    booked = I3DirectHitsValues

    #___________________________________________________________________________
    def CreateDescription(self, dh_values):
        desc = tableio.I3TableRowDescription()

        desc.add_field("dir_track_length",
            tableio.types.Float64, "m",
            I3DirectHitsValues.dir_track_length.__doc__
        )
        desc.add_field("dir_track_hit_distribution_smoothness",
            tableio.types.Float64, "",
            I3DirectHitsValues.dir_track_hit_distribution_smoothness.__doc__
        )
        desc.add_field("n_dir_pulses",
            tableio.types.UInt64, "number",
            I3DirectHitsValues.n_dir_pulses.__doc__
        )
        desc.add_field("q_dir_pulses",
            tableio.types.Float64, "PE",
            I3DirectHitsValues.q_dir_pulses.__doc__
        )
        desc.add_field("n_early_pulses",
            tableio.types.UInt64, "number",
            I3DirectHitsValues.n_early_pulses.__doc__
        )
        desc.add_field("q_early_pulses",
            tableio.types.Float64, "PE",
            I3DirectHitsValues.q_early_pulses.__doc__
        )
        desc.add_field("n_late_pulses",
            tableio.types.UInt64, "number",
            I3DirectHitsValues.n_late_pulses.__doc__
        )
        desc.add_field("q_late_pulses",
            tableio.types.Float64, "PE",
            I3DirectHitsValues.q_late_pulses.__doc__
        )
        desc.add_field("n_dir_strings",
            tableio.types.UInt32, "number",
            I3DirectHitsValues.n_dir_strings.__doc__
        )
        desc.add_field("n_dir_doms",
            tableio.types.UInt32, "number",
            I3DirectHitsValues.n_dir_doms.__doc__
        )
        desc.add_field("n_early_strings",
            tableio.types.UInt32, "number",
            I3DirectHitsValues.n_early_strings.__doc__
        )
        desc.add_field("n_early_doms",
            tableio.types.UInt32, "number",
            I3DirectHitsValues.n_early_doms.__doc__
        )
        desc.add_field("n_late_strings",
            tableio.types.UInt32, "number",
            I3DirectHitsValues.n_late_strings.__doc__
        )
        desc.add_field("n_late_doms",
            tableio.types.UInt32, "number",
            I3DirectHitsValues.n_late_doms.__doc__
        )

        return desc

    #___________________________________________________________________________
    def FillRows(self, dh_values, row):
        row["dir_track_length"]                      = dh_values.dir_track_length
        row["dir_track_hit_distribution_smoothness"] = dh_values.dir_track_hit_distribution_smoothness
        row["n_dir_pulses"]                          = dh_values.n_dir_pulses
        row["q_dir_pulses"]                          = dh_values.q_dir_pulses
        row["n_early_pulses"]                        = dh_values.n_early_pulses
        row["q_early_pulses"]                        = dh_values.q_early_pulses
        row["n_late_pulses"]                         = dh_values.n_late_pulses
        row["q_late_pulses"]                         = dh_values.q_late_pulses
        row["n_dir_strings"]                         = dh_values.n_dir_strings
        row["n_dir_doms"]                            = dh_values.n_dir_doms
        row["n_early_strings"]                       = dh_values.n_early_strings
        row["n_early_doms"]                          = dh_values.n_early_doms
        row["n_late_strings"]                        = dh_values.n_late_strings
        row["n_late_doms"]                           = dh_values.n_late_doms

        return 1

tableio.I3ConverterRegistry.register(I3DirectHitsValuesConverter)


class I3HitMultiplicityValuesConverter(tableio.I3Converter):
    """The I3HitMultiplicityValuesConverter tableio converter converts an
    I3HitMultiplicityValues frame object into a table verbatim.

    """
    booked = I3HitMultiplicityValues

    #___________________________________________________________________________
    def __init__(self):
        """Creates an instance of the I3HitMultiplicityValuesConverter class.

        """
        super(I3HitMultiplicityValuesConverter, self).__init__()

    #___________________________________________________________________________
    def CreateDescription(self, hm_values):
        desc = tableio.I3TableRowDescription()

        desc.add_field("n_pulses",
            tableio.types.UInt64, "number",
            I3HitMultiplicityValues.n_pulses.__doc__
        )
        desc.add_field("n_hit_strings",
            tableio.types.UInt32, "number",
            I3HitMultiplicityValues.n_hit_strings.__doc__
        )
        desc.add_field("n_hit_doms",
            tableio.types.UInt32, "number",
            I3HitMultiplicityValues.n_hit_doms.__doc__
        )
        desc.add_field("n_hit_doms_one_pulse",
            tableio.types.UInt32, "number",
            I3HitMultiplicityValues.n_hit_doms_one_pulse.__doc__
        )

        return desc

    #___________________________________________________________________________
    def FillRows(self, hm_values, row):
        row["n_pulses"]             = hm_values.n_pulses
        row["n_hit_strings"]        = hm_values.n_hit_strings
        row["n_hit_doms"]           = hm_values.n_hit_doms
        row["n_hit_doms_one_pulse"] = hm_values.n_hit_doms_one_pulse

        nrows = 1

        return nrows

tableio.I3ConverterRegistry.register(I3HitMultiplicityValuesConverter)


class I3HitStatisticsValuesConverter(tableio.I3Converter):
    """The I3HitStatisticsValuesConverter tableio converter converts an
    I3HitStatisticsValues frame object into a data table verbatim.

    """
    booked = I3HitStatisticsValues

    #___________________________________________________________________________
    def __init__(self,
        COGBookRefFrame = dataclasses.converters.I3PositionConverter.BookRefFrame.Car
    ):
        """Creates an instance of the I3HitStatisticsValuesConverter class.

        :type  COGBookRefFrame: enum ``icecube.dataclasses.converters.I3PositionConverter.BookRefFrame``
        :param COGBookRefFrame: The specifier of the reference frame into which
            the COG position should be booked.

            Default value is
            ``icecube.dataclasses.converters.I3PositionConverter.BookRefFrame.Car``.

        """
        super(I3HitStatisticsValuesConverter, self).__init__()

        self._cog_i3position_converter = dataclasses.converters.I3PositionConverter(
            BookRefFrame          = COGBookRefFrame,
            TableColumnNamePrefix = "cog_"
        )

    #___________________________________________________________________________
    def CreateDescription(self, hs_values):
        desc = tableio.I3TableRowDescription()

        desc.add_field("cog_z_sigma",
            tableio.types.Float64, "m",
            I3HitStatisticsValues.cog_z_sigma.__doc__
        )
        desc.add_field("min_pulse_time",
            tableio.types.Float64, "ns",
            I3HitStatisticsValues.min_pulse_time.__doc__
        )
        desc.add_field("max_pulse_time",
            tableio.types.Float64, "ns",
            I3HitStatisticsValues.max_pulse_time.__doc__
        )
        desc.add_field("q_max_doms",
            tableio.types.Float64, "PE",
            I3HitStatisticsValues.q_max_doms.__doc__
        )
        desc.add_field("q_tot_pulses",
            tableio.types.Float64, "PE",
            I3HitStatisticsValues.q_tot_pulses.__doc__
        )
        desc.add_field("z_min",
            tableio.types.Float64, "m",
            I3HitStatisticsValues.z_min.__doc__
        )
        desc.add_field("z_max",
            tableio.types.Float64, "m",
            I3HitStatisticsValues.z_max.__doc__
        )
        desc.add_field("z_mean",
            tableio.types.Float64, "m",
            I3HitStatisticsValues.z_mean.__doc__
        )
        desc.add_field("z_sigma",
            tableio.types.Float64, "m",
            I3HitStatisticsValues.z_sigma.__doc__
        )
        desc.add_field("z_travel",
            tableio.types.Float64, "m",
            I3HitStatisticsValues.z_travel.__doc__
        )

        # Add the description of the I3Position for the COG using the I3Position
        # converter.
        cog_i3position_desc = self._cog_i3position_converter.CreateDescription(hs_values.cog)
        if cog_i3position_desc.is_multi_row:
            desc.is_multi_row = True
        for i in range(0, cog_i3position_desc.number_of_fields):
            desc.add_field(
                cog_i3position_desc.field_names[i],
                cog_i3position_desc.field_types[i],
                cog_i3position_desc.field_units[i],
                cog_i3position_desc.field_doc_strings[i],
                cog_i3position_desc.field_array_lengths[i]
            )

        return desc

    #___________________________________________________________________________
    def FillRows(self, hs_values, row):
        row["cog_z_sigma"]    = hs_values.cog_z_sigma/I3Units.m
        row["min_pulse_time"] = hs_values.min_pulse_time/I3Units.ns
        row["max_pulse_time"] = hs_values.max_pulse_time/I3Units.ns
        row["q_max_doms"]     = hs_values.q_max_doms
        row["q_tot_pulses"]   = hs_values.q_tot_pulses
        row["z_min"]          = hs_values.z_min/I3Units.m
        row["z_max"]          = hs_values.z_max/I3Units.m
        row["z_mean"]         = hs_values.z_mean/I3Units.m
        row["z_sigma"]        = hs_values.z_sigma/I3Units.m
        row["z_travel"]       = hs_values.z_travel/I3Units.m

        # Fill also the I3Position table columns for the COG using the
        # I3Position converter.
        cog_i3position_nrows = self._cog_i3position_converter.FillRows(hs_values.cog, row)

        nrows = 1

        return max(nrows, cog_i3position_nrows)

tableio.I3ConverterRegistry.register(I3HitStatisticsValuesConverter)


class I3TimeCharacteristicsValuesConverter(tableio.I3Converter):
    """The I3TimeCharacteristicsValues tableio converter converts an
    I3TimeCharacteristicsValues frame object into a data table verbatim.

    """
    booked = I3TimeCharacteristicsValues

    #___________________________________________________________________________
    def __init__(self):
        """Creates an instance of the I3TimeCharacteristicsValuesConverter
        class.

        """
        super(I3TimeCharacteristicsValuesConverter, self).__init__()

    #___________________________________________________________________________
    def CreateDescription(self, tc_values):
        desc = tableio.I3TableRowDescription()

        desc.add_field("timelength_fwhm",
            tableio.types.Float64, "ns",
            I3TimeCharacteristicsValues.timelength_fwhm.__doc__
        )
        desc.add_field("timelength_last_first",
            tableio.types.Float64, "ns",
            I3TimeCharacteristicsValues.timelength_last_first.__doc__
        )
        desc.add_field("timelength_maxgap",
            tableio.types.Float64, "m",
            I3TimeCharacteristicsValues.timelength_maxgap.__doc__
        )
        desc.add_field("zpattern",
            tableio.types.Float64, "value",
            I3TimeCharacteristicsValues.zpattern.__doc__
        )


        return desc

    #___________________________________________________________________________
    def FillRows(self, tc_values, row):
        row["timelength_fwhm"]         = tc_values.timelength_fwhm/I3Units.ns
        row["timelength_last_first"]   = tc_values.timelength_last_first/I3Units.ns
        row["timelength_maxgap"]       = tc_values.timelength_maxgap/I3Units.m
        row["zpattern"]                = tc_values.zpattern

        nrows = 1

        return nrows

tableio.I3ConverterRegistry.register(I3TimeCharacteristicsValuesConverter)


class I3TrackCharacteristicsValuesConverter(tableio.I3Converter):
    """The I3TrackCharacteristicsValues tableio converter converts an
    I3TrackCharacteristicsValues frame object into a data table verbatim.

    """
    booked = I3TrackCharacteristicsValues

    #___________________________________________________________________________
    def __init__(self):
        """Creates an instance of the I3TrackCharacteristicsValuesConverter
        class.

        """
        super(I3TrackCharacteristicsValuesConverter, self).__init__()

    #___________________________________________________________________________
    def CreateDescription(self, tc_values):
        desc = tableio.I3TableRowDescription()

        desc.add_field("avg_dom_dist_q_tot_dom",
            tableio.types.Float64, "m",
            I3TrackCharacteristicsValues.avg_dom_dist_q_tot_dom.__doc__
        )
        desc.add_field("empty_hits_track_length",
            tableio.types.Float64, "m",
            I3TrackCharacteristicsValues.empty_hits_track_length.__doc__
        )
        desc.add_field("track_hits_separation_length",
            tableio.types.Float64, "m",
            I3TrackCharacteristicsValues.track_hits_separation_length.__doc__
        )
        desc.add_field("track_hits_distribution_smoothness",
            tableio.types.Float64, "value",
            I3TrackCharacteristicsValues.track_hits_distribution_smoothness.__doc__
        )

        return desc

    #___________________________________________________________________________
    def FillRows(self, tc_values, row):
        row["avg_dom_dist_q_tot_dom"]             = tc_values.avg_dom_dist_q_tot_dom/I3Units.m
        row["empty_hits_track_length"]            = tc_values.empty_hits_track_length/I3Units.m
        row["track_hits_separation_length"]       = tc_values.track_hits_separation_length/I3Units.m
        row["track_hits_distribution_smoothness"] = tc_values.track_hits_distribution_smoothness

        nrows = 1

        return nrows

tableio.I3ConverterRegistry.register(I3TrackCharacteristicsValuesConverter)


class I3VetoConverter(tableio.I3Converter):
    booked = I3Veto

    def CreateDescription(self,obj):
        desc = tableio.I3TableRowDescription()
        desc.add_field("nUnhitTopLayers",           tableio.types.Int16,    "", "number of horizontal layers on top without hits")
        desc.add_field("nLayer",                    tableio.types.Int16,     "", "number of vertical layers with hits")
        desc.add_field("earliestLayer",             tableio.types.Int16,  "", "vertical layer with the earliest hit")
        desc.add_field("earliestOM",                tableio.types.Int16,     "", "OM no. of the earliest hit")
        desc.add_field("earliestContainment",       tableio.types.Int16,    "", "100*earliestLayer + earliestOM")
        desc.add_field("latestLayer",               tableio.types.Int16,  "", "vertical layer with the latest hit")
        desc.add_field("latestOM",                  tableio.types.Int16,     "", "OM no. of the latest hit")
        desc.add_field("latestContainment",         tableio.types.Int16,    "", "100*latestLayer + latestOM")
        desc.add_field("mostOuterLayer",            tableio.types.Int16,  "", "most outer vertical layer with a hit")
        desc.add_field("depthHighestHit",           tableio.types.Float64,  "", "z position of the highest hit")
        desc.add_field("depthFirstHit",             tableio.types.Float64,  "", "z position of the first hit")       
        desc.add_field("maxDomChargeString",        tableio.types.Int16,     "", "String of Dom with highest charge")
        desc.add_field("maxDomChargeOM",            tableio.types.Int16,     "", "OM of Dom with highest charge")
        desc.add_field("maxDomChargeLayer",         tableio.types.Int16,     "", "Layer of Dom with highest charge")
        desc.add_field("maxDomChargeLayer_xy",      tableio.types.Int16,     "", "Vertical layer of Dom with highest charge")
        desc.add_field("maxDomChargeLayer_z",       tableio.types.Int16,     "", "Horizontal layer of Dom with highest charge")        
        desc.add_field("nDomsBeforeMaxDOM",         tableio.types.Int16,    "", "Number of DOMs hit before the DOM with the highest charge")
        desc.add_field("maxDomChargeContainment",   tableio.types.Int16,    "", "100*maxDomChargeLayer_xy + maxDomChargeOM")

        return desc

    def FillRows(self,obj,rows):
        rows["nUnhitTopLayers"]         = obj.nUnhitTopLayers 
        rows["nLayer"]                  = obj.nLayer 
        rows["earliestLayer"]           = obj.earliestLayer
        rows["earliestOM"]              = obj.earliestOM 
        rows["earliestContainment"]     = obj.earliestContainment
        rows["latestLayer"]             = obj.latestLayer
        rows["latestOM"]                = obj.latestOM 
        rows["latestContainment"]       = obj.latestContainment
        rows["mostOuterLayer"]          = obj.mostOuterLayer 
        rows["depthHighestHit"]         = obj.depthHighestHit 
        rows["depthFirstHit"]           = obj.depthFirstHit 
        rows["maxDomChargeString"]      = obj.maxDomChargeString 
        rows["maxDomChargeOM"]          = obj.maxDomChargeOM 
        rows["maxDomChargeLayer"]       = obj.maxDomChargeLayer
        rows["maxDomChargeLayer_xy"]    = obj.maxDomChargeLayer_xy
        rows["maxDomChargeLayer_z"]     = obj.maxDomChargeLayer_z
        rows["nDomsBeforeMaxDOM"]       = obj.nDomsBeforeMaxDOM
        rows["maxDomChargeContainment"] = obj.maxDomChargeContainment

        return 1

tableio.I3ConverterRegistry.register(I3VetoConverter)


class I3VetoShortConverter(tableio.I3Converter):
    booked = I3VetoShort
    
    def CreateDescription(self, obj):
        desc = tableio.I3TableRowDescription()
        desc.add_field("earliestContainment",       tableio.types.Int16,    "", "100*earliestLayer + earliestOM")
        desc.add_field("maxDomChargeContainment",   tableio.types.Int16,    "", "100*maxDomChargeLayer_xy + maxDomChargeOM")
        
        return desc
        
    def FillRows(self, obj, rows):
        rows["earliestContainment"]     = obj.earliestContainment
        rows["maxDomChargeContainment"] = obj.maxDomChargeContainment
        
        return 1
        
tableio.I3ConverterRegistry.register(I3VetoShortConverter)
