#include <topsimulator/injectors/ParticleUnthinner.h>
#include <phys-services/I3RandomService.h>
#include <dataclasses/geometry/I3Geometry.h>

#include <boost/python.hpp>
#include <boost/foreach.hpp>

namespace {

  using namespace boost::python;

  tuple
  AddParticle(
    ParticleUnthinner& pu,
    const I3Particle& p,
    double weight)
  {
    ParticleUnthinner::Index ind =
      pu.AddParticle(p, weight);
    return make_tuple(ind.i, ind.j);
  }

  tuple
  ToShowerFrontCS(const ParticleUnthinner& pu,
                 const I3Position& pos)
  {
    double r, phi, z;
    pu.ToShowerFrontCS(r, phi, z, pos);
    return make_tuple(r, phi, z);
  }

  tuple
  ToIndex(
    const ParticleUnthinner& pu,
    const I3Position& pos)
  {
    ParticleUnthinner::Index ind = pu.ToIndex(pos);
    return make_tuple(ind.i, ind.j);
  }

  tuple
  GetGridShape(const ParticleUnthinner& pu)
  {
    ParticleUnthinner::Index ind = pu.GetGridShape();
    return make_tuple(ind.i, ind.j);
  }

  list
  Sample(
    const ParticleUnthinner& pu,
    const ExtendedI3Particle& p,
    double weight,
    I3RandomServicePtr rng)
  {
    std::vector<ExtendedI3Particle> sample;
    pu.Sample(sample, p, weight, *rng);
    list lst;
    BOOST_FOREACH(ExtendedI3Particle p, sample)
      lst.append(p);
    return lst;
  }

  tuple
  GetGridSegment(const ParticleUnthinner& pu,
                 unsigned i, unsigned j)
  {
    double r1, r2, phi1, phi2;
    pu.GetGridSegment(r1, r2, phi1, phi2,
                      ParticleUnthinner::Index(i, j));
    return make_tuple(r1, r2, phi1, phi2);
  }

  const ParticleUnthinner::GridNode&
  GetGridNode(const ParticleUnthinner& pu,
              unsigned i, unsigned j)
  {
    return pu.GetGridNode(ParticleUnthinner::Index(i, j));
  }

  list
  GetStations(const ParticleUnthinner& pu)
  {
    const std::vector<ParticleUnthinner::Station>& v =
      pu.GetStations();
    list lst;
    BOOST_FOREACH(const ParticleUnthinner::Station& sta, v)
      lst.append(sta);
    return lst;
  }

  list
  GetGridNodeStations(const ParticleUnthinner::GridNode& gnode)
  {
    list lst;
    BOOST_FOREACH(unsigned i, gnode.stations_)
      lst.append(i);
    return lst;
  }

  tuple
  GetStationPos(const ParticleUnthinner::Station& sta)
  {
    return make_tuple(sta.pos_[0], sta.pos_[1]);
  }

  tuple
  GetStationRingRadius(const ParticleUnthinner::Station& sta)
  {
    return make_tuple(sta.ring_radius_[0], sta.ring_radius_[1]);
  }

  tuple
  GetStationRingPhi(const ParticleUnthinner::Station& sta)
  {
    return make_tuple(sta.ring_phi_[0], sta.ring_phi_[1]);
  }

  dict
  GetGridStatistics(const ParticleUnthinner::GridNode& gnode)
  {
    dict d;
    const char* labels[] = { "gamma", "electron", "muon", "other" };
    for (int i = 0; i < 4; ++i) {
      const ParticleUnthinner::Statistics& stats = gnode.stats_[i];
      const double avg = stats.logdt_sum / stats.w_sum;
      const double var = stats.logdt2_sum / stats.w_sum - avg * avg;
      const double std = std::sqrt(std::max(0.0, var));
      d[labels[i]] = make_tuple(stats.w_sum,
                                stats.wmax,
                                avg, std);
    }
    return d;
  }
}

void register_ParticleUnthinner()
{
  using namespace boost::python;

  scope class_scope =
  class_<ParticleUnthinner, boost::noncopyable>(
    "ParticleUnthinner", 
    init<double, unsigned, unsigned>())
    .def("set_primary", &ParticleUnthinner::SetPrimary)
    .def("to_shower_front_cs", ToShowerFrontCS)
    .def("to_index", ToIndex)
    .def("add_station", &ParticleUnthinner::AddStation)
    .def("set_stations", &ParticleUnthinner::SetStations)
    .def("add_particle", AddParticle)
    .def("sample", Sample)
    .add_property("grid_shape", GetGridShape)
    .def_readonly("grid_rmax", &ParticleUnthinner::GetGridRMax)
    .def("grid_node", GetGridNode,
         return_internal_reference<>())
    .def("grid_segment", GetGridSegment)
    .add_property("stations", GetStations)
    ;

  class_<ParticleUnthinner::GridNode>("GridNode", no_init)
    .add_property("statistics", GetGridStatistics)
    .add_property("stations", GetGridNodeStations)
    ;

  class_<ParticleUnthinner::Station>("Station", no_init)
    .add_property("pos", GetStationPos)
    .def_readonly("radius", &ParticleUnthinner::Station::radius_)
    .def_readonly("height", &ParticleUnthinner::Station::height_)
    .add_property("ring_radius", GetStationRingRadius)
    .add_property("ring_phi", GetStationRingPhi)
    ;
}
