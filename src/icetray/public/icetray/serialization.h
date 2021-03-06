/**
 *  $Id: serialization.h 2240 2015-08-21 17:40:16Z nega $
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
#ifndef ICETRAY_SERIALIZATION_H_INCLUDED
#define ICETRAY_SERIALIZATION_H_INCLUDED

#include <boost/version.hpp>

#ifndef __CINT__

#if BOOST_VERSION < 103600
#define BOOST_ARCHIVE_CUSTOM_OARCHIVE_TYPES boost::archive::portable_binary_oarchive
#define BOOST_ARCHIVE_CUSTOM_IARCHIVE_TYPES boost::archive::portable_binary_iarchive
#endif

#if BOOST_VERSION > 104100
#include <icetray/i3_extended_type_info.h>
#endif
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <icetray/portable_binary_archive.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>
// #include <boost/serialization/is_abstract.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/version.hpp>

#include <icetray/is_shared_ptr.h>
#include <boost/utility/enable_if.hpp>

#include <sstream>

using boost::serialization::make_nvp;
using boost::serialization::base_object;

template <typename T>
std::string
AsXML(const T& t)
{
  std::ostringstream oss;
  boost::archive::xml_oarchive xoa(oss, boost::archive::no_header);
  xoa << make_nvp("obj", t);
  return oss.str();
}

//

#define I3_BASIC_SERIALIZABLE(T) \
  template void T::serialize(boost::archive::portable_binary_oarchive&, unsigned); \
  template void T::serialize(boost::archive::portable_binary_iarchive&, unsigned); \
  template void T::serialize(boost::archive::xml_iarchive&, unsigned);	\
  template void T::serialize(boost::archive::xml_oarchive&, unsigned);
  
#if BOOST_VERSION > 104100

#define I3_EXPORT(T)				\
  static i3_export_key_setter<T> BOOST_PP_CAT(i3_export_key_setter_, __LINE__) (BOOST_PP_STRINGIZE(T));	\
  BOOST_CLASS_EXPORT(T);			\
  BOOST_SERIALIZATION_SHARED_PTR(T);

#elif BOOST_VERSION > 103310

#define I3_EXPORT(T)				\
  BOOST_SERIALIZATION_SHARED_PTR(T);		\
  BOOST_CLASS_EXPORT(T);

#else

#define I3_EXPORT(T)				\
  BOOST_CLASS_EXPORT(T);

#endif

#define I3_SERIALIZABLE(T)						\
  I3_BASIC_SERIALIZABLE(T)						\
  template std::string AsXML(const T&);					\
  template std::string AsXML(boost::shared_ptr<T> const&);		\
  template std::string AsXML(boost::shared_ptr<const T> const&);	\
  I3_EXPORT(T)

#define I3_SPLIT_SERIALIZABLE(T)					\
  I3_SERIALIZABLE(T)							\
  template void T::save(boost::archive::portable_binary_oarchive&, unsigned) const; \
  template void T::load(boost::archive::portable_binary_iarchive&, unsigned); \
  template void T::load(boost::archive::xml_iarchive&, unsigned);	\
  template void T::save(boost::archive::xml_oarchive&, unsigned) const;
  

#else // __CINT__

#define BOOST_CLASS_VERSION(T,V) 
#define BOOST_IS_ABSTRACT(X)
#define BOOST_CLASS_EXPORT(X) 
#define BOOST_SHARED_POINTER_EXPORT(X) 
#define BOOST_SERIALIZATION_SPLIT_MEMBER()

namespace boost 
{
  namespace serialization 
  {
    // normal forward declarations:
    template <class T> struct nvp;
    template <class T> const nvp<T> make_nvp(const char* name, T& t);

    template <class Retval, class Derived>
      Retval base_object(Derived);
  }
}
using boost::serialization::make_nvp;

#endif

#endif //BOOSTHEADERS_H_INCLUDED
