/**
 *  $Id: open.h 1432 2012-11-04 16:17:04Z nwhitehorn $
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
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
#ifndef ICETRAY_OPEN_H_INCLUDED
#define ICETRAY_OPEN_H_INCLUDED

#include <string>
#include <boost/iostreams/filtering_stream.hpp>

namespace I3 {
  namespace dataio {

    void open(boost::iostreams::filtering_istream&, const std::string& filename);
    void open(boost::iostreams::filtering_ostream&, 
	      const std::string& filename, 
	      int gzip_compression_level_=6, 
	      std::ios::openmode = std::ios::binary);

  } // namespace dataio
}  //  namespace I3


#endif
