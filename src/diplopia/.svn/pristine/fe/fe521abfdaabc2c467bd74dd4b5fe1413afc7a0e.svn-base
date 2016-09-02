//
//   Copyright (c) 2008   Troy D. Straszheim and the IceCube Collaboration 
//   
//   This file is part of IceTray.
//
//   IceTray is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
//   IceTray is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

using namespace boost::python;
namespace bp = boost::python;

#include <diplopia/DiplopiaUtils.h>
#include <icetray/load_project.h>

#define REGISTER_THESE_THINGS  (I3MapStringString)(I3GeneratorService)

#define I3_REGISTRATION_FN_DECL(r, data, t) void BOOST_PP_CAT(register_,t)();
#define I3_REGISTER(r, data, t) BOOST_PP_CAT(register_,t)();
BOOST_PP_SEQ_FOR_EACH(I3_REGISTRATION_FN_DECL, ~, REGISTER_THESE_THINGS)

I3Frame MergeFrames(I3Frame frame1, I3Frame frame2, I3Map<std::string,std::string> names, float delta_t)
{ 
    return DiplopiaUtils::MergeFrames(frame1, frame2, names,delta_t);
}

BOOST_PYTHON_MODULE(diplopia)
{
  load_project("libdiplopia", false);
  import("icecube.icetray");

  // export stuff in the DiplopiaUtils namespace  
  def("MergeHits", DiplopiaUtils::MergeHits);
  def("MergeMMCInfo", DiplopiaUtils::MergeMMCInfo);
  def("MergePrimaries", DiplopiaUtils::MergePrimaries);
  def("CopyWeights", DiplopiaUtils::CopyWeights);
  def("OffsetTime", DiplopiaUtils::OffsetTime);
  def("MergeEvents", MergeFrames);

  BOOST_PP_SEQ_FOR_EACH(I3_REGISTER, ~, REGISTER_THESE_THINGS);
}
