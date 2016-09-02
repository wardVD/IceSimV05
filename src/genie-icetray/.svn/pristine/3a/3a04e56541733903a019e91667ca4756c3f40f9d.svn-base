/**
 * copyright (C) 2011
 * Claudio Kopper <claudio.kopper@nikhef.nl>
 * $Id$
 *
 * @file I3GENIEGenerator.cxx
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

#ifndef I3LOGGINGAPPENDER_H_INCLUDED
#define I3LOGGINGAPPENDER_H_INCLUDED

// log4cpp
#include <log4cpp/LayoutAppender.hh>

#include <string>

// feed log4cpp messages to I3Logging

namespace log4cpp {
    class I3LoggingAppender : public LayoutAppender
    {
    public:
        I3LoggingAppender(const std::string& name, bool stepDownLoggingLevels);
        virtual ~I3LoggingAppender();
        
        virtual bool reopen();
        virtual void close();

    protected:
        virtual void _append(const LoggingEvent& event);
        
    private:
        bool stepDownLoggingLevels_;
    };
};


#endif //I3LOGGINGAPPENDER_H_INCLUDED
