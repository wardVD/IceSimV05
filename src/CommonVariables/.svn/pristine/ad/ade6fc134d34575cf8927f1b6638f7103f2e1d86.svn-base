#
# Anna Pollmann <anna.pollmann@uni-wuppertal.de>
# and the IceCube Collaboration <http://www.icecube.wisc.edu>


"""The segments Python module of the track  sub project of the
CommonVariables project contains all available IceCube tray segments for
calculating and booking track  cut variables.

"""
from icecube import icetray

from . import I3TimeCharacteristicsCalculator
from icecube.recclasses.converters import I3TimeCharacteristicsValuesConverter

#_______________________________________________________________________________
@icetray.traysegment_inherit(I3TimeCharacteristicsCalculator)
def I3TimeCharacteristicsCalculatorSegment(tray, name,
    BookIt = False,
    **i3module_kwargs
):
    """This tray segment adds the
    :py:class:`icecube.common_variables.track_.I3TrackCalculator`
    icetray module to the tray. The traysegment takes the same arguments as the
    icetray module does, plus the following additional keyword arguments:

    :type  BookIt: bool
    :param BookIt: The switch if this tray segment should also generate and
        return the tableio converter keys for the generated
        :py:class:`icecube.common_variables.track_.I3TrackValues`
        frame object.

        The name of the output tableio table for the generated
        I3TrackValues frame object will be the same as the name
        of the I3TrackValues frame object holding the track
         cut variables.

        Default value is ``False``.

    :return None: If the "BookIt" keyword argument has been set to ``False``.
    :return list: The list of tableio converter keys to book the
        I3TrackValues frame object, if the "BookIt" keyword
        argument has been set to ``True``.

    """
    # By default, run the calculator module only on physics frames.
    i3module_kwargs_ = dict(
        If = lambda frame: frame.Stop == icetray.I3Frame.Physics
    )
    i3module_kwargs_.update(i3module_kwargs)

    tray.AddModule(I3TimeCharacteristicsCalculator,
        "I3TimeCharacteristicsCalculator"+name,
        **i3module_kwargs_
    )

    if BookIt:
        if(not 'OutputI3TimeCharacteristicsValuesName' in i3module_kwargs_.keys()):
            raise ValueError(
                'The BookIt keyword argument of this traysegment has been set '\
                'to ``True`` but the "OutputI3TimeCharacteristicsValuesName" '\
                'parameter of the '\
                'common_variables.time_characteristics.I3TimeCharacteristicsCalculator '\
                'icetray module has not been specified, which is needed in '\
                'this case!')

        tableio_keys =\
        tray.AddSegment(I3TimeCharacteristicsValuesBookerSegment, name,
            OutputI3TimeCharacteristicsValuesName =\
                i3module_kwargs_['OutputI3TimeCharacteristicsValuesName']
        )
        return tableio_keys

    return None

#_______________________________________________________________________________
@icetray.traysegment
def I3TimeCharacteristicsValuesBookerSegment(tray, name,
    OutputI3TimeCharacteristicsValuesName
):
    """This traysegment generates and returns tableio converter keys to book
    the I3TimeCharacteristicsValues frame object from the frame.

    The parameters of this traysegment have the same types and meanings as the
    parameters of the
    :py:class:`icecube.common_variables.time_characteristics.I3TimeCharacteristicsCalculator`
    icetray module.

    It will create one tableio table having the name specified through the
    *OutputI3TimeCharacteristicsValuesName* keyword argument.

    :return list: The list of *tableio* converter keys.

    """
    tableio_keys = [
        dict(
            key       = OutputI3TimeCharacteristicsValuesName,
            converter = I3TimeCharacteristicsValuesConverter(),
            name      = OutputI3TimeCharacteristicsValuesName
        )
    ]

    return tableio_keys
