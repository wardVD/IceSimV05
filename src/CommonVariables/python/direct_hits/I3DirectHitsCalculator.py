# $Id: I3DirectHitsCalculator.py 97956 2013-01-26 12:00:15Z mwolf $
#
# Copyright (C) 2012
# Martin Wolf <martin.wolf@icecube.wisc.edu>
# and the IceCube Collaboration <http://www.icecube.wisc.edu>
#
# Version: $Revision: 97956 $
# Date:    $Date: 2013-01-26 07:00:15 -0500 (Sat, 26 Jan 2013) $
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

"""This python module contains the I3DirectHitsCalculator icetray module.

"""
import logging

from icecube.icetray import I3ConditionalModule
from icecube import dataclasses

from icecube.common_variables.direct_hits import calculate_direct_hits
from icecube.common_variables.direct_hits import get_default_definitions

#===============================================================================
class I3DirectHitsCalculator(I3ConditionalModule):
    """This icetray module calculates the common event variables for direct
    hits. A hit is considered direct when the time residual falls within a
    specified time window. The time residual is defined as the observed
    time of a hit minus the expected time, based on a track emitting light
    at the Cherenkov angle.

    This module calculates many information about the direct hits of a
    particular direct hits time window. See the
    :py:class:`icecube.common_variables.direct_hits.I3DirectHitsValues` class
    for the documentation of each variable.

    The default time windows, which will be used when no time windows have been
    specified explicitly through the module parameter
    *DirectHitsDefinitionSeries*, will be determined through the function
    :func:`icecube.common_variables.direct_hits.get_default_definitions`.

    For each I3DirectHitsDefinition object inside the
    DirectHitsDefinitionSeries, the module puts an
    :class:`icecube.common_variables.direct_hits.I3DirectHitsValues`
    I3FrameObject into the frame having a frame object key name of the following
    structure:
    ``{OutputI3DirectHitsValuesBaseName}{I3DirectHitsDefinition.name}``.

    """
    ###
    # Define Module parameters. They are defined as properties in order to
    # get their docstring for the Sphinx documentation automatically using the
    # .. autoattribute:: directive.
    # This way the documentation will be only at one single place!
    #___________________________________________________________________________
    @property
    def DirectHitsDefinitionSeries(self):
        """An I3DirectHitsDefinitionSeries object or a Python list of
        I3DirectHitsDefinition objects defining the time windows for
        the direct hits calculation.

        """
        pass
    #---------------------------------------------------------------------------
    @property
    def PulseSeriesMapName(self):
        """The frame object name of the pulse series map used to identify the
        hits.

        """
        pass
    #---------------------------------------------------------------------------
    @property
    def ParticleName(self):
        """The frame object name of the reconstructed particle used as track in
        the direct hits calculation.

        """
        pass
    #---------------------------------------------------------------------------
    @property
    def OutputI3DirectHitsValuesBaseName(self):
        """The base name of the output I3DirectHitsValues frame objects.

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
        # Call the __init__ method of the I3ConditionalModule class.
        super(I3DirectHitsCalculator, self).__init__(context)
        self.logger = logging.getLogger(__name__)

        #-----------------------------------------------------------------------
        self.dh_definitions = get_default_definitions()
        self.AddParameter("DirectHitsDefinitionSeries",
            I3DirectHitsCalculator.DirectHitsDefinitionSeries.__doc__,
            self.dh_definitions
        )
        #-----------------------------------------------------------------------
        self.pulse_series_map_name = ""
        self.AddParameter("PulseSeriesMapName",
            I3DirectHitsCalculator.PulseSeriesMapName.__doc__,
            self.pulse_series_map_name
        )
        #-----------------------------------------------------------------------
        self.particle_name = ""
        self.AddParameter("ParticleName",
            I3DirectHitsCalculator.ParticleName.__doc__,
            self.particle_name
        )
        #-----------------------------------------------------------------------
        self.output_I3DirectHitsValues_base_name = ""
        self.AddParameter("OutputI3DirectHitsValuesBaseName",
            I3DirectHitsCalculator.OutputI3DirectHitsValuesBaseName.__doc__,
            self.output_I3DirectHitsValues_base_name
        )
        #-----------------------------------------------------------------------
        self.py_log_level = logging.WARNING
        self.AddParameter("PyLogLevel",
            I3DirectHitsCalculator.PyLogLevel.__doc__,
            self.py_log_level
        )

        self.AddOutBox("OutBox")

    #___________________________________________________________________________
    def _is_frame_input_complete(self, frame):
        """Checks if the frame has all needed input data.

        """
        if(not self.pulse_series_map_name in frame):
            self.logger.warning('%s: I3RecoPulseSeriesMap "%s" is missing in '\
                'frame! Cannot calculate direct hits for this frame!'%
                (self.name, self.pulse_series_map_name)
            )
            return False
        if(not self.particle_name in frame):
            self.logger.warning('%s: I3Particle "%s" is missing in '\
                'frame! Cannot calculate direct hits for this frame!'%
                (self.name, self.particle_name)
            )
            return False

        return True

    #___________________________________________________________________________
    def Configure(self):
        super(I3DirectHitsCalculator, self).Configure()

        self.dh_definitions                      = self.GetParameter("DirectHitsDefinitionSeries")
        self.pulse_series_map_name               = self.GetParameter("PulseSeriesMapName")
        self.particle_name                       = self.GetParameter("ParticleName")
        self.output_I3DirectHitsValues_base_name = self.GetParameter("OutputI3DirectHitsValuesBaseName")
        self.py_log_level                        = self.GetParameter("PyLogLevel")

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
        if self.output_I3DirectHitsValues_base_name == "":
            raise ValueError(
                '%s: Module parameter "OutputI3DirectHitsValuesBaseName" '\
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

        # Call the utility function to calculate the direct hits.
        direct_hits_map = calculate_direct_hits(
            self.dh_definitions,
            geometry,
            pulses_map,
            particle
        )

        # Put the results into the frame.
        for key in direct_hits_map.keys():
            frame[self.output_I3DirectHitsValues_base_name+key] =\
                direct_hits_map[key]

        self.PushFrame(frame)

#===============================================================================