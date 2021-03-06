#
#  $Id: cdk.cmake 1584 2013-04-08 00:43:57Z jvansanten $
#  
#  Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#  
#  This file is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>
#  
tooldef (cdk
    include
    cdk/cdk.h 
    lib/cdk
    NONE
    cdk
)
if (CDK_FOUND)
    LIST(APPEND CDK_INCLUDE_DIR ${CDK_INCLUDE_DIR}/cdk)
else (CDK_FOUND)
    tooldef (cdk
        include
        cdk.h 
        lib
        NONE
        cdk
    )
endif (CDK_FOUND)
