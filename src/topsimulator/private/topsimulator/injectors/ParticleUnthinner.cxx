#include "topsimulator/injectors/ParticleUnthinner.h"

#include <phys-services/I3Calculator.h>
#include <phys-services/surfaces/Cylinder.h>
#include <dataclasses/I3Constants.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/geometry/I3TankGeo.h>

#include <boost/foreach.hpp>
#include <cmath>
#include <limits>

namespace {

    inline
    void Prod(double x[3],
              const double a[3][3],
              const double b[3]) {
        x[0] = a[0][0] * b[0] + a[0][1] * b[1] + a[0][2] * b[2];
        x[1] = a[1][0] * b[0] + a[1][1] * b[1] + a[1][2] * b[2];
        x[2] = a[2][0] * b[0] + a[2][1] * b[1] + a[2][2] * b[2];
    }

    inline
    void Prod(double x[3][3],
              const double a[3][3],
              const double b[3][3]) {
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
        {
            x[i][j] = a[i][0] * b[0][j] 
                    + a[i][1] * b[1][j]
                    + a[i][2] * b[2][j];
        }
    }

    inline double Sqr(double x) { return x * x; }

    void SetTransform(double m[3][3],
                      const I3Particle& p) {
        const double ph = -p.GetDir().GetAzimuth();
        const double th = -p.GetDir().GetZenith();
        const double cp = std::cos(ph);
        const double sp = std::sin(ph);
        const double ct = std::cos(th);
        const double st = std::sin(th);
        const double mz[3][3] = { {  cp, -sp, 0.0 },
                                  {  sp,  cp, 0.0 },
                                  { 0.0, 0.0, 1.0 } };
        const double my[3][3] = { {  ct, 0.0, -st },
                                  { 0.0, 1.0, 0.0 },
                                  {  st, 0.0,  ct } };
        Prod(m, my, mz);
    }

    // compute time relative to shower front
    inline
    double FrontTimeDelta(const I3Particle& primary, const I3Particle& p) {
        // time when the shower front arrives at position of particle
        const double t_front = primary.GetTime() +
            primary.GetDir() * (p.GetPos() - primary.GetPos())
            / I3Constants::c;            
        return p.GetTime() - t_front; // should be always positive, particle arrives after front
    }

    inline
    unsigned ParticleClass(const I3Particle& p)
    {
        switch (p.GetType()) {
            case I3Particle::Gamma:
            return 0;
            case I3Particle::EMinus:
            case I3Particle::EPlus:
            return 1;
            case I3Particle::MuMinus:
            case I3Particle::MuPlus:
            return 2;
            default:
            break;
        }
        return 3;
    }

    inline
    I3Position TankCenter(const I3TankGeo& tgeo)
    {
        // tankGeo.position corresponds to the
        // average position of the two DOMs in a tank,
        // which is at the top of the ice block in the tank
        return I3Position(
            tgeo.position.GetX(),
            tgeo.position.GetY(),
            tgeo.position.GetZ()
            - tgeo.fillheight + 0.5 * tgeo.tankheight
        );
    }
}

ParticleUnthinner::ParticleUnthinner(
    double r_max,
    unsigned r_bin,
    unsigned phi_bin) :
    sqrt_r_max_(std::sqrt(r_max)),
    r_bin_(r_bin),
    phi_bin_(phi_bin),
    grid_(new GridNode[r_bin_ * phi_bin_]),
    primary_(I3Position(0, 0, 0), I3Direction(0, 0, -1), 0.0)
{
    SetTransform(transform_, primary_);
}

ParticleUnthinner::~ParticleUnthinner()
{
    delete [] grid_;
}


ParticleUnthinner::Station::Station(const I3Position& posA,
                                    const I3Position& posB,
                                    double radius,
                                    double height) :
    radius_(radius), height_(height)
{
    pos_[0] = posA; pos_[1] = posB;
}

/// always returns a valid index
ParticleUnthinner::Index
ParticleUnthinner::ToIndex(double sqrt_r, double phi)
    const
{
    const unsigned i = std::min(r_bin_ - 1,
        unsigned(r_bin_ * sqrt_r / sqrt_r_max_));

    // phi-bins are symmetric around zero
    if (phi < 0.0)
        phi += 2 * M_PI;
    int j = int(phi_bin_ * phi / (2 * M_PI) + 0.5);
    j = j % phi_bin_;

    return Index(i, Index::elem_t(j));
}

inline
ParticleUnthinner::GridNode&
ParticleUnthinner::GetGridNode(ParticleUnthinner::Index ind)
{
    return grid_[ind.i * phi_bin_ + ind.j];
}

const ParticleUnthinner::GridNode&
ParticleUnthinner::GetGridNode(ParticleUnthinner::Index ind)
    const
{
    return grid_[ind.i * phi_bin_ + ind.j];
}

void
ParticleUnthinner::ComputeSamplingArea(ParticleUnthinner::Station& sta)
    const
{
    double ring_center_r, ring_center_phi, ring_center_z;
    ToShowerFrontCS(ring_center_r,
                    ring_center_phi,
                    ring_center_z,
                    0.5 * (sta.pos_[0] + sta.pos_[1]));

    double ring_delta_r = 0.1 * ring_center_r;
    const double ring_delta_phi = 20 * I3Units::degree;

    // make sure that sampling region is not smaller than tank cross-section
    const double ct = std::abs(primary_.GetDir().GetZ());
    const double st = std::sqrt(1.0 - Sqr(ct));
    ring_delta_r = std::max(ring_delta_r,
        sta.height_ * st + 2.0 * sta.radius_ * ct);
    ring_delta_r = std::max(ring_delta_r, 2.0 * sta.radius_);

    for (int i = 0; i < 2; ++i) {
        sta.ring_radius_[i] = ring_center_r + (i - 0.5) * ring_delta_r;
        sta.ring_phi_[i] = ring_center_phi + (i - 0.5) * ring_delta_phi;
    }

    // special case for tanks really close to the core
    if (sta.ring_radius_[0] < 0.0) {
        sta.ring_phi_[0] = 0.0;
        sta.ring_phi_[1] = 2 * M_PI;
        sta.ring_radius_[0] = 0.0;
    }

    // normalize to internal angle format
    if (sta.ring_phi_[0] < -M_PI) {
        sta.ring_phi_[0] += 2 * M_PI;
        sta.ring_phi_[1] += 2 * M_PI;
    }
}

void
ParticleUnthinner::AssignStationToGrid(unsigned sta_ind, const Station& sta)
{
    for(unsigned j = 0; j < phi_bin_; ++j) {                
        for (unsigned i = 0; i < r_bin_; ++i) {
            Index ind(i, j);
            double r1, r2, phi1, phi2;
            GetGridSegment(r1, r2, phi1, phi2, ind);

            // Most efficient test of overlapping rectangles:
            // - test all four cases where they can't overlap
            // - OR the results and negate
            // Also see http://stackoverflow.com/questions/2752349/fast-rectangle-to-rectangle-intersection
            // Ring segments are 'rectangles' in polar coordinates,
            // the same principle applies, but angles need extra care. 

            // if ring is completely left of grid segment,
            // test whether it is also completely right
            if (sta.ring_phi_[1] < phi1) {
                phi1 -= 2 * M_PI;
                phi2 -= 2 * M_PI;
            }

            const bool no_phi_intersection =
                sta.ring_phi_[0] > phi2 ||
                sta.ring_phi_[1] < phi1;

            if (no_phi_intersection)
                break;

            // safety: if collection region falls
            // completely outside of grid, assign it
            // nevertheless to outermost radial bin
            if (sta.ring_radius_[0] >= GetGridRMax()) {
                ind.i = r_bin_ - 1;
                GridNode& gnode = GetGridNode(ind);
                gnode.stations_.push_back(sta_ind);
                break;
            }

            const bool no_r_intersection = 
                sta.ring_radius_[0] > r2 || 
                sta.ring_radius_[1] < r1;

            if (no_r_intersection)
                continue;

            GridNode& gnode = GetGridNode(ind);
            gnode.stations_.push_back(sta_ind);
        }
    }
}

void
ParticleUnthinner::AddStation(
    const I3Position& posA,
    const I3Position& posB,
    double tank_radius,
    double tank_height)
{
    stations_.push_back(Station(posA, posB,
                                tank_radius,
                                tank_height));
    ComputeSamplingArea(stations_.back());
    AssignStationToGrid(stations_.size() - 1, stations_.back());
}

void
ParticleUnthinner::SetStations(const I3Geometry& geo,
                               double dr_safety,
                               double dh_safety)
{
    // for the sake of unthinning, make the tanks slightly larger than
    // the real tanks (real intersection is tested anyway by I3TopSimulator)
    // so that particle interactions in snow are reflected properly
    stations_.clear();
    stations_.reserve(geo.stationgeo.size());
    BOOST_FOREACH (const I3StationGeoMap::value_type& mp, geo.stationgeo) {
        const I3StationGeo& tanks = mp.second;
        if(tanks.size() > 2)
          log_fatal("Can't handle stations with more than two tanks");
        if(tanks.size() == 1){
          log_warn("Station with only one tank");
          AddStation(TankCenter(tanks[0]),
                     TankCenter(tanks[0]),
                     tanks[0].tankradius + dr_safety,
                     tanks[0].tankheight + dh_safety);
        } else {
          AddStation(TankCenter(tanks[0]),
                     TankCenter(tanks[1]),
                     std::max(tanks[0].tankradius,
                              tanks[1].tankradius) + dr_safety,
                     std::max(tanks[0].tankheight,
                              tanks[1].tankheight) + dh_safety);
        }
    }
}

void
ParticleUnthinner::ClearStations()
{
    stations_.clear();
    // reset station mapping
    for (unsigned i = 0; i < r_bin_ * phi_bin_; ++i) {
        GridNode& gnode = grid_[i];
        gnode.stations_.clear();
    }
}

void
ParticleUnthinner::SetPrimary(const I3Particle& primary)
{
    primary_ = primary;
    SetTransform(transform_, primary);
    // reset grid
    for (unsigned i = 0; i < r_bin_ * phi_bin_; ++i)
        grid_[i] = GridNode();
    SetPrimaryPos(primary.GetPos());
}

void
ParticleUnthinner::SetPrimaryPos(const I3Position& pos)
{
    primary_.SetPos(pos);

    // reset station mapping
    for (unsigned i = 0; i < r_bin_ * phi_bin_; ++i) {
        GridNode& gnode = grid_[i];
        gnode.stations_.clear();
    }

    // compute new sampling regions and update grid association
    unsigned sta_ind = 0;
    BOOST_FOREACH(Station& sta, stations_) {
        ComputeSamplingArea(sta);
        AssignStationToGrid(sta_ind++, sta);
    }
}

ParticleUnthinner::Index
ParticleUnthinner::AddParticle(
    const I3Particle& p,
    double weight)
{
    Index ind = ToIndex(p.GetPos());
    Statistics& stats =
        GetGridNode(ind).stats_[ParticleClass(p)];;
    double dt = FrontTimeDelta(primary_, p);
    log_trace_stream(p.GetID() << " dt[front] "
        << (dt / I3Units::ns) << " ns");
    // correcting values which are non-positive,
    // because of rounding errors
    if( dt == 0.0 )
        dt = std::numeric_limits<double>::epsilon();
    stats.w_sum += weight;
    const double logdt = std::log(std::abs(dt));
    stats.logdt_sum += weight * logdt;
    stats.logdt2_sum += weight * Sqr(logdt);
    if (weight > stats.wmax)
        stats.wmax = weight;
    return ind;
}

unsigned
ParticleUnthinner::Sample(
    std::vector<ExtendedI3Particle>& sample,
    const ExtendedI3Particle& p,
    double weight,
    I3RandomService& rng)
    const
{
    double rho, phi, z;
    ToShowerFrontCS(rho, phi, z, p.GetPos());
    const Index ind = ToIndex(std::sqrt(rho), phi);
    const GridNode& gnode = GetGridNode(ind);
    if (gnode.stations_.empty())
        return 0;
    unsigned nclones = 0;
    BOOST_FOREACH(unsigned i, gnode.stations_){
        const Station& sta = stations_[i];

        // test whether particle falls into sampling region
        if(rho < sta.ring_radius_[0] || sta.ring_radius_[1] < rho)
            continue;
        if(phi < sta.ring_phi_[0] && (phi + 2 * M_PI) > sta.ring_phi_[1])
            continue;
        if(sta.ring_phi_[1] < phi && (phi - 2 * M_PI) < sta.ring_phi_[0])
            continue;

        const double npart = NParticles(ind, sta, p, weight);
        log_trace_stream(p.GetTypeString() << " " << p.GetID() << " "
                         "weight " << weight << " "
                         "npart " << npart);
        nclones += GenerateClones(sample, sta, p, npart, rng);
    }
    return nclones;
}


double
ParticleUnthinner::NParticles(
    Index ind,
    const Station& sta,
    const I3Particle& p,
    double weight)
    const
{
    // compute tank shadow for this particle direction in the ground plane
    const double ct = std::abs(p.GetDir().GetZ());
    const double st = std::sqrt(1.0 - ct * ct);
    const double tank_area_side =
        (st / ct) * sta.height_ * 2.0 * sta.radius_;
    const double tank_area_top = Sqr(sta.radius_) * M_PI;
    const double tank_area = tank_area_top + tank_area_side;

    // compute size of collection area in the ground plane
    const double collection_area =
        (sta.ring_phi_[1] - sta.ring_phi_[0]) *
        0.5 * (Sqr(sta.ring_radius_[1]) - Sqr(sta.ring_radius_[0])) /
        std::abs(primary_.GetDir().GetZ());

    log_trace("collection_area %.2g tank_area %.2g",
        collection_area, tank_area);

    // two tanks per collection area give twice the tank area;
    // we assume no overlap between tank shadows!
    return weight / collection_area * 2.0 * tank_area;
}


unsigned
ParticleUnthinner::GenerateClones(
    std::vector<ExtendedI3Particle>& sample,
    const Station& sta,
    const ExtendedI3Particle& p,
    double npart,
    I3RandomService& rng)
    const
{
    const unsigned nclones = rng.Poisson(npart);

    if (!nclones)
        return 0;

    // fetch mean and standard deviation of logTime distribution
    const Index ind = ToIndex(p.GetPos());
    const Statistics& stats =
        GetGridNode(ind).stats_[ParticleClass(p)];
    double meanLogTimeDelta = stats.logdt_sum / stats.w_sum;
    // due to round-off error, the variance can be smaller than zero
    double stdLogTimeDelta = std::sqrt(std::max(0.0,
        stats.logdt2_sum / stats.w_sum - Sqr(meanLogTimeDelta)));
    // fallback which should never happen
    if (std::isnan(meanLogTimeDelta) || std::isnan(stdLogTimeDelta)) {
        log_error("grid cell (%u,%u) has no particle statistics for class %u",
                  ind.i, ind.j, ParticleClass(p));
        meanLogTimeDelta = std::log(FrontTimeDelta(primary_, p));
        stdLogTimeDelta = 0.2; // from Billoir paper
    }

    for (unsigned i = 0; i < nclones; ++i) {
        ExtendedI3Particle clone = p.Clone(); // clone produces new particle ID
        // randomly assign clone to one of the tanks
        const I3Position& sta_pos = sta.pos_[rng.Integer(2)];
        RelocateParticle(clone, sta_pos, sta.radius_, sta.height_, rng);
        if (nclones == 1) {
            // if there is only one clone, return use dt of original particle
            clone.SetTime(clone.GetTime() +
                FrontTimeDelta(primary_, p));
        }
        else {
            // Sample random dt from a log-normal distribution,
            // whose parameters are extracted from particles that
            // fall into the same grid node
            clone.SetTime(clone.GetTime() + 
                std::exp(rng.Gaus(meanLogTimeDelta, stdLogTimeDelta)));
        }
        sample.push_back(clone);
    }

    return nclones; 
}


void
ParticleUnthinner::RelocateParticle(
    ExtendedI3Particle& p,
    const I3Position& pos,
    double radius, double height,
    I3RandomService& rng)
    const
{
    // create random point on cylinder surface
    I3Surfaces::Cylinder cyl(height, radius, pos);
    I3Position pos2 = cyl.SampleImpactPosition(
        p.GetDir(), rng);

    // propagate impact point back to starting altitude
    const double s = (p.GetPos().GetZ() - pos2.GetZ()) / p.GetDir().GetZ();
    pos2 += s * p.GetDir();

    p.SetPos(pos2);
    
    // set time so that particle is exactly coincident with shower front
    p.SetTime(primary_.GetTime()
              + primary_.GetDir() * (p.GetPos() - primary_.GetPos())
                / I3Constants::c);
}

void
ParticleUnthinner::ToShowerFrontCS(double& rho,
                                   double& phi,
                                   double& z,
                                   const I3Position& pos)
    const
{
    const double pos1[3] = { pos.GetX() - primary_.GetX(),
                             pos.GetY() - primary_.GetY(),
                             pos.GetZ() - primary_.GetZ() };
    double pos2[3];
    Prod(pos2, transform_, pos1);
    rho = std::sqrt(Sqr(pos2[0]) + Sqr(pos2[1]));
    phi = std::atan2(pos2[1], pos2[0]);
    z = pos2[2];
}

ParticleUnthinner::Index
ParticleUnthinner::ToIndex(const I3Position& pos)
    const
{
    double r, phi, z;
    ToShowerFrontCS(r, phi, z, pos);
    return ToIndex(std::sqrt(r), phi);
}

void
ParticleUnthinner::GetGridSegment(
    double& r1, double& r2,
    double& phi1, double& phi2,
    Index ind)
    const
{
    r1 = Sqr(double(ind.i) / r_bin_ * sqrt_r_max_);
    r2 = Sqr(double(ind.i + 1) / r_bin_ * sqrt_r_max_);
    phi1 = (double(ind.j) - 0.5) / phi_bin_ * 2 * M_PI;
    phi2 = (double(ind.j + 1) - 0.5) / phi_bin_ * 2 * M_PI;
}
