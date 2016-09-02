/**
 * I3TableWriter.cxx (pybindings)
 *
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3TableWriter.cxx 84991 2012-02-10 05:32:24Z jvansanten $
 *
 * @version $Revision: 84991 $
 * @date $LastChangedDate: 2012-02-09 23:32:24 -0600 (Thu, 09 Feb 2012) $
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy: jvansanten $
 */

#include <tableio/I3TableWriter.h>

namespace bp = boost::python;

void register_I3TableWriter() {
    bp::class_<std::vector<I3ConverterMillPtr> >("vector_I3ConverterMillPtr")
        .def(bp::vector_indexing_suite<std::vector<I3ConverterMillPtr> >());
    
	bp::scope tw = bp::class_<I3TableWriter, boost::noncopyable>
		("I3TableWriterWorker",
		bp::init<I3TableServicePtr, std::vector<I3ConverterMillPtr>&, std::vector<std::string>& >(bp::args("table_service","converter_list","streams")))
		.def("add_object", (void (I3TableWriter::*)(const std::string, I3TableWriter::TableSpec)) &I3TableWriter::AddObject)
		.def("add_type", (void (I3TableWriter::*)(I3TableWriter::TypeSpec, I3TableWriter::TableSpec)) &I3TableWriter::AddType)
		.def("convert", (void (I3TableWriter::*)(I3FramePtr)) &I3TableWriter::Convert)
		.def("finish", &I3TableWriter::Finish)
	;
	
	bp::class_<I3TableWriter::TableSpec>("TableSpec")
		.def(bp::init<const std::string,I3ConverterPtr>(bp::args("name","converter")))
		.def(bp::init<const std::string>(bp::args("name")))
		.def(bp::init<I3ConverterPtr>(bp::args("converter")))
		;
		
	bp::class_<I3TableWriter::TypeSpec>("TypeSpec", bp::init<bp::object>())
		.def("check", (bool (I3TableWriter::TypeSpec::*)(I3FrameObjectPtr) const) &I3TableWriter::TypeSpec::check)
	;
}
