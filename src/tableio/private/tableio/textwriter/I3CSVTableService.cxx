/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3CSVTableService.cxx 94948 2012-11-04 16:21:52Z nwhitehorn $
 *
 * @version $Revision: 94948 $
 * @date $LastChangedDate: 2012-11-04 10:21:52 -0600 (Sun, 04 Nov 2012) $
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy: nwhitehorn $
 */

#include "tableio/textwriter/I3CSVTable.h"
#include "tableio/textwriter/I3CSVTableService.h"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

I3CSVTableService::I3CSVTableService(const std::string& foldername) : folderName_(foldername) {
    fs::remove_all( folderName_ );
    fs::create_directory( folderName_ );
    fs::create_directory( folderName_ + "/__I3Index__" );
}

I3TablePtr I3CSVTableService::CreateTable(const std::string& tableName, 
                               I3TableRowDescriptionConstPtr description) {
        
        I3TableRowDescriptionConstPtr index_desc = GetIndexDescription();
        std::string indexpath = folderName_ + "/__I3Index__"; 
        I3TablePtr index_table(new I3CSVTable(*this,tableName,index_desc,indexpath));
        return I3TablePtr(new I3CSVTable(*this,tableName,description,folderName_,index_table));
}

void I3CSVTableService::CloseFile() {
    
}

I3CSVTableService::~I3CSVTableService() {}

