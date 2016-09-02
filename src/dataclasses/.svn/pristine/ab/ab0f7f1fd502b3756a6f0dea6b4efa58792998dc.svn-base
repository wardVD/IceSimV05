#ifndef ICETRAY_PYTHON_OSTREAM_OVERLOADS_HPP_INCLUDED
#define ICETRAY_PYTHON_OSTREAM_OVERLOADS_HPP_INCLUDED

#include <dataclasses/I3Vector.h>
#include <dataclasses/I3Map.h>

template <typename T>
std::ostream& operator<<(std::ostream& oss, const I3Vector<T> l){
  oss << "[";
  for(typename I3Vector<T>::const_iterator i = l.begin(); i != l.end(); i++){
    oss << *i;
    if( i + 1 != l.end()) 
      oss << ", ";
  }
  oss << "]";
  return oss;
}

template <typename T>
std::ostream& operator<<(std::ostream& oss, const std::vector<T> l){
  oss << "[";
  for(typename std::vector<T>::const_iterator i = l.begin(); i != l.end(); i++){
    oss << *i;
    if( i + 1 != l.end()) 
      oss << ", ";
  }
  oss << "]";
  return oss;
}

template <typename K, typename V>
std::ostream& operator<<(std::ostream& oss, const I3Map<K,V> m){
  oss << "{";
  for(typename I3Map<K,V>::const_iterator i = m.begin(); i != m.end(); i++){
    oss << i->first << " : "
	<< i->second ;
    if( ++i != m.end() )
      oss << ", ";
    --i;
  }
  oss << "}";
  return oss;
}

// specialize I3Vector<char> because ...well it's special
std::ostream& operator<<(std::ostream& oss, const I3Vector<char> l);

#endif // ICETRAY_PYTHON_OSTREAM_OVERLOADS_HPP_INCLUDED
