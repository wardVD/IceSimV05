#
# Anna Pollmann <anna.pollmann@uni-wuppertal.de>
# and the IceCube Collaboration <http://www.icecube.wisc.edu>



"""This python module contains the I3TimeCharacteristicsConverter
icetray module.

"""
import logging

from icecube.icetray import I3ConditionalModule, I3Units
from icecube import dataclasses

from . import calculate_time_characteristics_values

#===============================================================================
class I3TimeCharacteristicsCalculator(I3ConditionalModule):
    """This icetray module calculates the time_characteristics  cut variables
    for a given I3Geometry, a given I3RecoPulseSeriesMap.

    The module put an I3TimeCharacteristicsCalculator I3FrameObject object into the
    frame having the key name, which has been specified through the module
    parameter *OutputI3TimeCharacteristicsValuesName*.

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
    def OutputI3TimeCharacteristicsValuesName(self):
        """The name of the output I3TimeCharacteristicsValues frame object
        holding the calculation results.

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
        super(I3TimeCharacteristicsCalculator, self).__init__(context)
        self.logger = logging.getLogger(__name__)

        #-----------------------------------------------------------------------
        self.pulse_series_map_name = ""
        self.AddParameter("PulseSeriesMapName",
            I3TimeCharacteristicsCalculator.PulseSeriesMapName.__doc__,
            self.pulse_series_map_name
        )

        #-----------------------------------------------------------------------
        self.output_I3TimeCharacteristicsValues_name = ""
        self.AddParameter("OutputI3TimeCharacteristicsValuesName",
            I3TimeCharacteristicsCalculator.OutputI3TimeCharacteristicsValuesName.__doc__,
            self.output_I3TimeCharacteristicsValues_name
        )
        #-----------------------------------------------------------------------
        self.py_log_level = logging.WARNING
        self.AddParameter("PyLogLevel",
            I3TimeCharacteristicsCalculator.PyLogLevel.__doc__,
            self.py_log_level
        )

        self.AddOutBox("OutBox")

    #___________________________________________________________________________
    def _is_frame_input_complete(self, frame):
        """Checks if the frame has all needed input data.

        """
        if(not self.pulse_series_map_name in frame):
            self.logger.warning('%s: I3RecoPulseSeriesMap "%s" is missing in '\
                'frame! Cannot calculate time_characteristics  for this frame!'%
                (self.name, self.pulse_series_map_name)
            )
            return False

        return True

    #___________________________________________________________________________
    def Configure(self):
        #super(I3TimeCharacteristicsCalculator, self).Configure()

        self.pulse_series_map_name =\
            self.GetParameter("PulseSeriesMapName")
        self.output_I3TimeCharacteristicsValues_name =\
            self.GetParameter("OutputI3TimeCharacteristicsValuesName")
        self.py_log_level =\
            self.GetParameter("PyLogLevel")
        self.logger.setLevel(self.py_log_level)

        # Check the configuration.
        if self.pulse_series_map_name == "":
            raise ValueError(
                '%s: Module parameter "PulseSeriesMapName" not specified!'%
                (self.name)
            )
        if self.output_I3TimeCharacteristicsValues_name == "":
            raise ValueError(
                '%s: Module parameter "OutputI3TimeCharacteristicsValuesName" '\
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

        # Call the utility function to calculate the TimeCharacteristics .
        time_characteristics_values = calculate_time_characteristics_values(
            geometry,
            pulses_map,
        )

        # Put the results into the frame.
        frame[self.output_I3TimeCharacteristicsValues_name] =\
            time_characteristics_values

        self.PushFrame(frame)

#===============================================================================
