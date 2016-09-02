# $Id: I3HitStatisticsCalculator.py 123227 2014-09-08 08:51:29Z mwolf $
#
# Copyright (C) 2012
# Martin Wolf <martin.wolf@icecube.wisc.edu>
# and the IceCube Collaboration <http://www.icecube.wisc.edu>
#
# Version: $Revision: 123227 $
# Date:    $Date: 2014-09-08 04:51:29 -0400 (Mon, 08 Sep 2014) $
#
# This file is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>

"""This python module contains the I3HitStatisticsCalculator icetray module.

"""
import logging

from icecube.icetray import I3ConditionalModule
from icecube import dataclasses

from icecube.common_variables.hit_statistics import calculate_hit_statistics

#===============================================================================
class I3HitStatisticsCalculator(I3ConditionalModule):
    """This icetray module calculates the hit statistics variables, e.g.
    COG, PulseTimeSpan, QMinDoms, QMaxDoms, QTotPulses, ZMean, ZSigma, and
    ZTravel.

    This icetray module puts an
    :py:class:`icecube.common_variables.hit_statistics.I3HitStatisticsValues`
    I3FrameObject, holding the calculated values, into the frame. Its name will
    be the one specified through the *OutputI3HitStatisticsValuesName* module
    parameter.

    """
    ###
    # Define Module parameters. They are defined as properties in order to
    # get their docstring for the Sphinx documentation automatically using the
    # .. autoattribute:: directive.
    # This way the documentation will be only at one single place!
    #___________________________________________________________________________
    @property
    def PulseSeriesMapName(self):
        """The frame object name of the pulse series map used to calculate the
        hit statistics values.
        """
        pass
    #---------------------------------------------------------------------------
    @property
    def OutputI3HitStatisticsValuesName(self):
        """The name of the output I3HitStatisticsValues frame object.
        """
        pass
    #---------------------------------------------------------------------------
    @property
    def PyLogLevel(self):
        """The Python ``logging`` module log level for this module.
        """
        pass

    #___________________________________________________________________________
    def __init__(self, context):
        # Call __init__ method of the I3ConditionalModule class.
        super(I3HitStatisticsCalculator, self).__init__(context)
        self.logger = logging.getLogger(__name__)

        #-----------------------------------------------------------------------
        self.pulse_series_map_name = ""
        self.AddParameter("PulseSeriesMapName",
            I3HitStatisticsCalculator.PulseSeriesMapName.__doc__,
            self.pulse_series_map_name
        )
        #-----------------------------------------------------------------------
        self.output_I3HitStatisticsValues_name = ""
        self.AddParameter("OutputI3HitStatisticsValuesName",
            I3HitStatisticsCalculator.OutputI3HitStatisticsValuesName.__doc__,
            self.output_I3HitStatisticsValues_name
        )
        #-----------------------------------------------------------------------
        self.py_log_level = logging.WARNING
        self.AddParameter("PyLogLevel",
            I3HitStatisticsCalculator.PyLogLevel.__doc__,
            self.py_log_level
        )

        self.AddOutBox("OutBox")

    #___________________________________________________________________________
    def _is_frame_input_complete(self, frame):
        """Checks if the frame has all needed input data.
        """
        if(not self.pulse_series_map_name in frame):
            self.logger.warning('%s: I3RecoPulseSeriesMap "%s" is missing in '\
                'frame! Cannot calculate hit statistics for this frame!'%
                (self.name, self.pulse_series_map_name)
            )
            return False

        return True

    #___________________________________________________________________________
    def Configure(self):
        super(I3HitStatisticsCalculator, self).Configure()

        self.pulse_series_map_name             = self.GetParameter("PulseSeriesMapName")
        self.output_I3HitStatisticsValues_name = self.GetParameter("OutputI3HitStatisticsValuesName")
        self.py_log_level                      = self.GetParameter("PyLogLevel")

        self.logger.setLevel(self.py_log_level)

        # Check the configuration.
        if self.pulse_series_map_name == "":
            raise ValueError(
                '%s: Module parameter "PulseSeriesMapName" not specified!'%
                (self.name)
            )
        if self.output_I3HitStatisticsValues_name == "":
            raise ValueError(
                '%s: Module parameter "OutputI3HitStatisticsValuesName" '\
                'not specified!'%
                (self.name)
            )

    #___________________________________________________________________________
    def Physics(self, frame):
        if(not self._is_frame_input_complete(frame)):
            self.PushFrame(frame)
            return

        geometry = frame["I3Geometry"]

        pulses_map = frame[self.pulse_series_map_name]
        if(isinstance(pulses_map, dataclasses.I3RecoPulseSeriesMapMask)):
            # We've got an I3RecoPulseSeriesMapMask, get an I3RecoPulseSeriesMap
            # out of it.
            pulses_map = pulses_map.apply(frame)

        # Call the utility function to calculate the hit statistics.
        hs_values = calculate_hit_statistics(
            geometry,
            pulses_map
        )

        # Put the results into the frame.
        frame[self.output_I3HitStatisticsValues_name] = hs_values

        self.PushFrame(frame)

#===============================================================================
