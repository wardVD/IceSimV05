/**
 * I3Converter.cxx (pybindings)
 *
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3Converter.cxx 143491 2016-03-19 06:28:17Z cweaver $
 *
 * @version $Revision: 143491 $
 * @date $LastChangedDate: 2016-03-19 01:28:17 -0500 (Sat, 19 Mar 2016) $
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy: cweaver $
 */


#include <tableio/converter/PythonConverter.h>
#include "tableio/detail/I3ConverterMill.h"

namespace bp = boost::python;

void register_I3Converter() {
	bp::enum_<I3Converter::ConvertState>("ConvertState")
	.value("NoConversion", I3Converter::NoConversion)
	.value("ExactConversion", I3Converter::ExactConversion)
	.value("InexactConversion", I3Converter::InexactConversion)
	;
	
	bp::class_<PythonConverter, boost::shared_ptr<PythonConverter>, boost::noncopyable>
		("I3Converter")
	
	.add_property("description",
		(I3TableRowDescriptionConstPtr (PythonConverter::*)()) &PythonConverter::GetDescription)
	.add_property("current_frame",
		(I3FramePtr (PythonConverter::*)()) &PythonConverter::GetCurrentFrame)
	.def("CreateDescription",(I3TableRowDescriptionPtr (PythonConverter::*)(I3FrameObjectConstPtr))
                    &PythonConverter::CreateDescription)
	.def("Convert",(size_t (PythonConverter::*)(I3FrameObjectConstPtr, 
	                                                  I3TableRowPtr, 
	                                                  I3FramePtr))
	               &PythonConverter::Convert)
	.def("CanConvert", (I3Converter::ConvertState (PythonConverter::*)(I3FrameObjectPtr)) &PythonConverter::CanConvert)
	.def("FillRows",&PythonConverter::FillRows)
	//.def("GetNumberOfRows",&PythonConverter::GetNumberOfRows)
	.def("GetNumberOfRows",(size_t (PythonConverter::*)(I3FrameObjectConstPtr))
                           &PythonConverter::GetNumberOfRows)
	;
	
	bp::class_<I3ConverterMill, I3ConverterMillPtr >
	    ("I3ConverterMill", bp::init<bp::object>())
	;
}

