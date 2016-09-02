# $Id: segments.py 123227 2014-09-08 08:51:29Z mwolf $
#
# Copyright (C) 2012
# Martin Wolf <martin.wolf@icecube.wisc.edu>
# and the IceCube Collaboration <http://www.icecube.wisc.edu>
#
# Version: $Revision$
# Date:    $Date: 2014-09-08 04:51:29 -0400 (Mon, 08 Sep 2014) $
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

"""The segments Python module of the track characteristics sub project of the
CommonVariables project contains all available IceCube tray segments for
calculating and booking track characteristics cut variables.

"""
from icecube import icetray

from icecube.common_variables.track_characteristics import I3TrackCharacteristicsCalculator
from icecube.recclasses.converters import I3TrackCharacteristicsValuesConverter
from icecube.recclasses.converters import I3TimeCharacteristicsValuesConverter

#_______________________________________________________________________________
@icetray.traysegment_inherit(I3TrackCharacteristicsCalculator)
def I3TrackCharacteristicsCalculatorSegment(tray, name,
    BookIt = False,
    **i3module_kwargs
):
    """This tray segment adds the
    :py:class:`icecube.common_variables.track_characteristics.I3TrackCharacteristicsCalculator`
    icetray module to the tray. The traysegment takes the same arguments as the
    icetray module does, plus the following additional keyword arguments:

    :type  BookIt: bool
    :param BookIt: The switch if this tray segment should also generate and
        return the tableio converter keys for the generated
        :py:class:`icecube.common_variables.track_characteristics.I3TrackCharacteristicsValues`
        frame object.

        The name of the output tableio table for the generated
        I3TrackCharacteristicsValues frame object will be the same as the name
        of the I3TrackCharacteristicsValues frame object holding the track
        characteristics cut variables.

        Default value is ``False``.

    :return None: If the "BookIt" keyword argument has been set to ``False``.
    :return list: The list of tableio converter keys to book the
        I3TrackCharacteristicsValues frame object, if the "BookIt" keyword
        argument has been set to ``True``.

    """
    # By default, run the calculator module only on physics frames.
    i3module_kwargs_ = dict(
        If = lambda frame: frame.Stop == icetray.I3Frame.Physics
    )
    i3module_kwargs_.update(i3module_kwargs)

    tray.AddModule(I3TrackCharacteristicsCalculator,
        "I3TrackCharacteristicsCalculator_"+name,
        **i3module_kwargs_
    )

    if BookIt:
        if(not 'OutputI3TrackCharacteristicsValuesName' in i3module_kwargs_.keys()):
            raise ValueError(
                'The BookIt keyword argument of this traysegment has been set '\
                'to ``True`` but the "OutputI3TrackCharacteristicsValuesName" '\
                'parameter of the '\
                'common_variables.track_characteristics.I3TrackCharacteristicsCalculator '\
                'icetray module has not been specified, which is needed in '\
                'this case!')

        tableio_keys =\
        tray.AddSegment(I3TrackCharacteristicsValuesBookerSegment, name,
            OutputI3TrackCharacteristicsValuesName =\
                i3module_kwargs_['OutputI3TrackCharacteristicsValuesName']
        )
        return tableio_keys

    return None

#_______________________________________________________________________________
@icetray.traysegment
def I3TrackCharacteristicsValuesBookerSegment(tray, name,
    OutputI3TrackCharacteristicsValuesName
):
    """This traysegment generates and returns tableio converter keys to book
    the I3TrackCharacteristicsValues frame object from the frame.

    The parameters of this traysegment have the same types and meanings as the
    parameters of the
    :py:class:`icecube.common_variables.track_characteristics.I3TrackCharacteristicsCalculator`
    icetray module.

    It will create one tableio table having the name specified through the
    *OutputI3TrackCharacteristicsValuesName* keyword argument.

    :return list: The list of *tableio* converter keys.

    """
    tableio_keys = [
        dict(
            key       = OutputI3TrackCharacteristicsValuesName,
            converter = I3TrackCharacteristicsValuesConverter(),
            name      = OutputI3TrackCharacteristicsValuesName
        )
    ]

    return tableio_keys
