/**
 * copyright (C) 2011
 * Claudio Kopper <claudio.kopper@nikhef.nl>
 * $Id$
 *
 * @file I3GENIEResultDict.h
 * @version $Revision$
 * @date $Date$
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

#ifndef I3GENIERESULTDICT_H_INCLUDED
#define I3GENIERESULTDICT_H_INCLUDED

#include <vector>
#include <map>
#include <string>

#include <dataclasses/I3Map.h>

#include <boost/variant.hpp>

typedef boost::variant<int32_t, double, bool,  std::vector<int32_t>, std::vector<double>,  std::vector<bool> > I3GENIEResult;
typedef I3Map<std::string, I3GENIEResult> I3GENIEResultDict;

I3_POINTER_TYPEDEFS(I3GENIEResultDict);

#endif //I3GENIERESULTDICT_H_INCLUDED
