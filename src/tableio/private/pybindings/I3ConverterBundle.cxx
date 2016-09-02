/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3ConverterBundle.cxx 129552 2015-02-25 13:45:04Z jvansanten $
 *
 * @version $Revision: 129552 $
 * @date $LastChangedDate: 2015-02-25 07:45:04 -0600 (Wed, 25 Feb 2015) $
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy: jvansanten $
 */

#include "tableio/utils/I3ConverterBundle.h"

namespace bp = boost::python;

// init from list rather than exposing vector<I3ConverterPtr>
static boost::shared_ptr<I3ConverterBundle> list_init( bp::list converters ) {
    bp::ssize_t n = bp::len(converters);
    std::vector<I3ConverterPtr> converter_vec;
    for (bp::ssize_t i = 0; i < n; i++) {
        bp::extract<I3ConverterPtr> extractor(converters[i]);
        if (!extractor.check()) {
            log_fatal("List element could not be interpreted as an I3Converter.");
            continue;
        }
        converter_vec.push_back(extractor());
    }
    return boost::shared_ptr<I3ConverterBundle>(new I3ConverterBundle(converter_vec));
}

void register_I3ConverterBundle() {
    bp::class_<I3ConverterBundle, 
               boost::shared_ptr<I3ConverterBundle>,
               bp::bases<I3Converter> >
               ("I3ConverterBundle",bp::no_init)
        .def("__init__",bp::make_constructor(&list_init))
    ;
}
