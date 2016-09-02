
#include <genie-icetray/I3GENIEResultDict.h>

#include <boost/serialization/variant.hpp>
#include <icetray/python/dataclass_suite.hpp>


namespace bp=boost::python;

struct I3GENIEResult_to_object : boost::static_visitor<PyObject *> {
  static PyObject *convert(I3GENIEResult const &v) {
    return apply_visitor(I3GENIEResult_to_object(), v);
  }

  template<typename T>
  PyObject *operator()(T const &t) const {
    return boost::python::incref(boost::python::object(t).ptr());
  }
};

void register_I3GENIEResultDict()
{
	bp::to_python_converter<I3GENIEResult, I3GENIEResult_to_object>();
	bp::implicitly_convertible<int32_t, I3GENIEResult>();
	bp::implicitly_convertible<double, I3GENIEResult>();
	bp::implicitly_convertible<bool, I3GENIEResult>();
	bp::implicitly_convertible<std::vector<int32_t>, I3GENIEResult>();
	bp::implicitly_convertible<std::vector<double>, I3GENIEResult>();
	bp::implicitly_convertible<std::vector<bool>, I3GENIEResult>();

    bp::class_<I3GENIEResultDict, bp::bases<I3FrameObject>, I3GENIEResultDictPtr>("I3GENIEResultDict")
    .def(bp::std_map_indexing_suite<I3GENIEResultDict, true>())
    ;

}
