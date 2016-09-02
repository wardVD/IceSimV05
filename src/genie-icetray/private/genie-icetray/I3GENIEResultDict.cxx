/**
 * copyright (C) 2011
 * Claudio Kopper <claudio.kopper@nikhef.nl>
 * $Id: I3GENIEResultDict.cxx 103608 2013-05-03 16:54:54Z claudio.kopper $
 *
 * @file I3GENIEResultDict.cxx
 * @version $Revision$
 * @date $Date: 2013-05-03 11:54:54 -0500 (Fri, 03 May 2013) $
 * @author Claudio Kopper
 *
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *  
 */

#include "genie-icetray/I3GENIEResultDict.h"

#include "icetray/serialization.h"
#include <boost/serialization/variant.hpp>


I3_SERIALIZABLE(I3GENIEResultDict);

