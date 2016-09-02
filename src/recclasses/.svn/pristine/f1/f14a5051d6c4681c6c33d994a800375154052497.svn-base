#ifndef RECCLASSES_PYBINDINGS_WRAP_H
#define RECCLASSES_PYBINDINGS_WRAP_H

#include <icetray/name_of.h>
#include <icetray/I3FrameObject.h>
#include <icetray/python/dataclass_suite.hpp>
#include <dataclasses/I3Vector.h>

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>

namespace boost { namespace python { namespace wrap {

    // explicit version
    template <typename T, class Init>
    class_<
        T
        , bases< ::I3FrameObject >
        , boost::shared_ptr<T>
    >
    I3FrameObject(const std::string classname,
                  const char* doc,
                  const Init& init)
    {
        class_<
            T
            , bases< ::I3FrameObject >
            , boost::shared_ptr<T>
        > cls(classname.c_str(), doc, init);
        cls.def(dataclass_suite<T>());
        register_pointer_conversions<T>();
        return cls;
    }

    // lazy version
    template <typename T>
    class_<
        T
        , bases< ::I3FrameObject >
        , boost::shared_ptr<T>
    >
    I3FrameObject(const std::string classname = icetray::name_of<T>(),
                  const char* doc = "")
    {
        class_<
            T
            , bases< ::I3FrameObject >
            , boost::shared_ptr<T>
        > cls(classname.c_str(), doc);
        cls.def(dataclass_suite<T>());
        register_pointer_conversions<T>();
        return cls;
    }

    template <typename T>
    object
    I3Vector(std::string classname = std::string())
    {
        if(classname.empty()){
            classname = "I3Vector";
            classname += icetray::name_of<T>();
        }

        class_<
            ::I3Vector<T>
            , bases< ::I3FrameObject >
            , boost::shared_ptr< ::I3Vector<T> >
        >cls(classname.c_str());
        cls.def(dataclass_suite< ::I3Vector<T> >());
        register_pointer_conversions< ::I3Vector<T> >();
        return cls;
    }

    template <typename T>
    object
    vector(std::string classname = std::string())
    {
        if(classname.empty()){
            classname = "vector_";
            classname += icetray::name_of<T>();
        }

        class_<std::vector<T> > cls(classname.c_str());
        cls.def(dataclass_suite< std::vector<T> >());
        return cls;
    }

}}}

#endif
