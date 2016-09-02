# $Id: I3TrackCharacteristicsCalculator.py 123227 2014-09-08 08:51:29Z mwolf $
#
# Copyright (C) 2012
# Martin Wolf <martin.wolf@icecube.wisc.edu>
# and the IceCube Collaboration <http://www.icecube.wisc.edu>
#
# Version: $Revision$
# Date:    $Date: 2014-09-08 04:51:29 -0400 (Mon, 08 Sep 2014) $
#
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

"""This python module contains the I3TrackCharacteristicsCalculator
icetray module.

"""
import logging

from icecube.icetray import I3ConditionalModule, I3Units
from icecube import dataclasses

from icecube.common_variables.track_characteristics import calculate_track_characteristics

#===============================================================================
class I3TrackCharacteristicsCalculator(I3ConditionalModule):
    """This icetray module calculates the track characteristics cut variables
    for a given I3Geometry, a given I3RecoPulseSeriesMap, and a given track
    (given through an I3Particle object).

    The module put an I3TrackCharacteristicsValues I3FrameObject object into the
    frame having the key name, which has been specified through the module
    parameter *OutputI3TrackCharacteristicsValuesName*.

    """
    ###
    # Define Module parameters. They are defined as properties in order to
    # get their docstring for the Sphinx documentation automatically using the
    # .. autoattribute:: directive.
    # This way the documentation will be only at one single place!
    #___________________________________________________________________________
    @property
    def PulseSeriesMapName(self):
        """The frame object name of the pulse series map used to identify the
        hits.

        """
        pass
    #---------------------------------------------------------------------------
    @property
    def ParticleName(self):
        """The frame object name of the reconstructed particle used as track for
        the calculation of the track characteristics.

        """
        pass
    #---------------------------------------------------------------------------
    @property
    def OutputI3TrackCharacteristicsValuesName(self):
        """The name of the output I3TrackCharacteristicsValues frame object
        holding the calculation results.

        """
        pass
    #---------------------------------------------------------------------------
    @property
    def PyLogLevel(self):
        """The Python ``logging`` module log level for this module.

        """
        pass
    #---------------------------------------------------------------------------
    @property
    def TrackCylinderRadius(self):
        """The cylinder radius around the track. Only pulses within this
        cylinder radius around the track will be considered for the calculation
        of the track characteristics variables.

        """
        pass

    #___________________________________________________________________________
    def __init__(self, context):
        # Call the __init__ method of the I3ConditionalModule class.
        super(I3TrackCharacteristicsCalculator, self).__init__(context)
        self.logger = logging.getLogger(__name__)

        #-----------------------------------------------------------------------
        self.pulse_series_map_name = ""
        self.AddParameter("PulseSeriesMapName",
            I3TrackCharacteristicsCalculator.PulseSeriesMapName.__doc__,
            self.pulse_series_map_name
        )
        #-----------------------------------------------------------------------
        self.particle_name = ""
        self.AddParameter("ParticleName",
            I3TrackCharacteristicsCalculator.ParticleName.__doc__,
            self.particle_name
        )
        #-----------------------------------------------------------------------
        self.output_I3TrackCharacteristicsValues_name = ""
        self.AddParameter("OutputI3TrackCharacteristicsValuesName",
            I3TrackCharacteristicsCalculator.OutputI3TrackCharacteristicsValuesName.__doc__,
            self.output_I3TrackCharacteristicsValues_name
        )
        #-----------------------------------------------------------------------
        self.py_log_level = logging.WARNING
        self.AddParameter("PyLogLevel",
            I3TrackCharacteristicsCalculator.PyLogLevel.__doc__,
            self.py_log_level
        )
        #-----------------------------------------------------------------------
        self.track_cylinder_radius = 150*I3Units.m
        self.AddParameter("TrackCylinderRadius",
            I3TrackCharacteristicsCalculator.TrackCylinderRadius.__doc__,
            self.track_cylinder_radius
        )

        self.AddOutBox("OutBox")

    #___________________________________________________________________________
    def _is_frame_input_complete(self, frame):
        """Checks if the frame has all needed input data.

        """
        if(not self.pulse_series_map_name in frame):
            self.logger.warning('%s: I3RecoPulseSeriesMap "%s" is missing in '\
                'frame! Cannot calculate track characteristics for this frame!'%
                (self.name, self.pulse_series_map_name)
            )
            return False
        if(not self.particle_name in frame):
            self.logger.warning('%s: I3Particle "%s" is missing in frame! '\
                'Cannot calculate track characteristics for this frame!'%
                (self.name, self.particle_name)
            )
            return False

        return True

    #___________________________________________________________________________
    def Configure(self):
        super(I3TrackCharacteristicsCalculator, self).Configure()

        self.pulse_series_map_name =\
            self.GetParameter("PulseSeriesMapName")
        self.particle_name =\
            self.GetParameter("ParticleName")
        self.output_I3TrackCharacteristicsValues_name =\
            self.GetParameter("OutputI3TrackCharacteristicsValuesName")
        self.py_log_level =\
            self.GetParameter("PyLogLevel")
        self.track_cylinder_radius =\
            self.GetParameter("TrackCylinderRadius")

        self.logger.setLevel(self.py_log_level)

        # Check the configuration.
        if self.pulse_series_map_name == "":
            raise ValueError(
                '%s: Module parameter "PulseSeriesMapName" not specified!'%
                (self.name)
            )
        if self.particle_name == "":
            raise ValueError(
                '%s: Module parameter "ParticleName" not specified!'%
                (self.name)
            )
        if self.output_I3TrackCharacteristicsValues_name == "":
            raise ValueError(
                '%s: Module parameter "OutputI3TrackCharacteristicsValuesName" '\
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

        particle = frame[self.particle_name]

        # Call the utility function to calculate the track characteristics.
        track_characteristics_values = calculate_track_characteristics(
            geometry,
            pulses_map,
            particle,
            self.track_cylinder_radius
        )

        # Put the results into the frame.
        frame[self.output_I3TrackCharacteristicsValues_name] =\
            track_characteristics_values

        self.PushFrame(frame)

#===============================================================================
