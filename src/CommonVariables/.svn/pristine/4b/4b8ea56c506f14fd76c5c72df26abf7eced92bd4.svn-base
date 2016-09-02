# $Id: segments.py 93523 2012-09-26 09:16:32Z jacobi $
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

"""The segments Python module of the direct hits sub project of the
CommonVariables project contains all available IceCube tray segments for
calculating and booking direct hits.

"""
from icecube import icetray
from icecube.common_variables.direct_hits import I3DirectHitsCalculator
from icecube.recclasses.converters import I3DirectHitsValuesConverter

#_______________________________________________________________________________
@icetray.traysegment_inherit(I3DirectHitsCalculator)
def I3DirectHitsCalculatorSegment(tray, name,
    BookIt = False,
    **i3module_kwargs
):
    """This tray segment adds the
    :class:`icecube.common_variables.direct_hits.I3DirectHitsCalculator`
    icetray module to the tray. The traysegment takes the same arguments as the
    icetray module does, plus the following additional keyword arguments:

    :type  BookIt: bool
    :param BookIt: The switch if this tray segment should also generate and
        return the tableio converter keys for the calculated direct hits.

        The name of the output tableio table for the calculated hits will be
        the same as the name of the frame object holding the calculation
        results.

        Default value is ``False``.

    :return None: If the "BookIt" keyword argument has been set to ``False``.
    :return list: The list of tableio converter keys to book the direct hits,
        if the "BookIt" keyword argument has been set to ``True``.

    """
    # By default, run the calculator module only on physics frames.
    i3module_kwargs_ = dict(
        If = lambda frame: frame.Stop == icetray.I3Frame.Physics
    )
    i3module_kwargs_.update(i3module_kwargs)

    tray.AddModule(I3DirectHitsCalculator, "I3DirectHitsCalculator_"+name,
        **i3module_kwargs_
    )

    if BookIt:
        if(not 'OutputI3DirectHitsValuesBaseName' in i3module_kwargs_.keys()):
            raise ValueError(
                'The BookIt keyword argument of this traysegment has been set '\
                'to ``True`` but the "OutputI3DirectHitsValuesBaseName" '\
                'parameter of the '\
                'common_variables.direct_hits.I3DirectHitsCalculator icetray '\
                'module has not been specified, which is needed in this case!')

        if(not 'DirectHitsDefinitionSeries' in i3module_kwargs_.keys()):
            raise ValueError(
                'The BookIt keyword argument of this traysegment has been '\
                'set to ``True`` but the "DirectHitsDefinitionSeries" '\
                'parameter of the '\
                'common_variables.direct_hits.I3DirectHitsCalculator icetray '\
                'module has not been specified, which is needed in this case!')

        tableio_keys =\
        tray.AddSegment(I3DirectHitsValuesBookerSegment, name,
            DirectHitsDefinitionSeries       = i3module_kwargs_['DirectHitsDefinitionSeries'],
            OutputI3DirectHitsValuesBaseName = i3module_kwargs_['OutputI3DirectHitsValuesBaseName']
        )
        return tableio_keys

    return None

#_______________________________________________________________________________
@icetray.traysegment
def I3DirectHitsValuesBookerSegment(tray, name,
    DirectHitsDefinitionSeries,
    OutputI3DirectHitsValuesBaseName
):
    """This traysegment generates and returns tableio converter keys to book the
    direct hits calculation results from the frame.

    The parameters of this traysegment have the same types and meanings as the
    parameters of the
    :class:`icecube.common_variables.direct_hits.I3DirectHitsCalculator`
    icetray module.

    For each direct hits definition, it will create one tableio table having
    the name structure
    ``{OutputI3DirectHitsValuesBaseName}{DirectHitsDefinition.name}``.

    :return list: The list of *tableio* converter keys.

    """
    tableio_keys = []

    # Get the list of frame object key names to book.
    for dh_def in DirectHitsDefinitionSeries:
        fokey = OutputI3DirectHitsValuesBaseName+dh_def.name
        tableio_keys.append(
            dict(
                key       = fokey,
                converter = I3DirectHitsValuesConverter(),
                name      = fokey
            )
        )

    return tableio_keys
