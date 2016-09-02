#
# Anna Pollmann <anna.pollmann@uni-wuppertal.de>


"""The python module
``icecube.common_variables.time_characteristics.converters`` contains tableio
converters for the time_characteristics sub module of the common_variables
python module of the CommonVariables project.

"""
from icecube.icetray import I3Units
from icecube import tableio

from icecube.recclasses import I3TimeCharacteristicsValues

#===============================================================================
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
#===============================================================================
