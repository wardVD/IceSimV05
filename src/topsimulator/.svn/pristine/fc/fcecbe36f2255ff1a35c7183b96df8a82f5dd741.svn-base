#pragma once

#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/I3Position.h>
#include <icetray/I3Logging.h>
#include <topsimulator/ExtendedI3Particle.h>

#include <boost/noncopyable.hpp>

#include <vector>
#include <map>

class I3RandomService;
class I3Geometry;

/** ParticleUnthinner

    Re-samples re-weighted particles into tanks, using
    a ring segment around the shower axis as a virtual
    larger collection area. Uses an internal grid to
    accelerate matching particles to sampling areas.
 */
class ParticleUnthinner: private boost::noncopyable {
    SET_LOGGER("ParticleUnthinner");
public:
    struct Index {
        typedef unsigned short elem_t;
        elem_t i, j;
        Index(elem_t a, elem_t b) : i(a), j(b) {}
        bool operator<(Index other) const {
            if (i < other.i)
                return true;
            if (i > other.i)
                return false;
            return j < other.j;
        }
    };

    struct Statistics {
        double wmax;
        double w_sum;
        double logdt_sum;
        double logdt2_sum;

        Statistics() :
            wmax(0.0),
            w_sum(0.0),
            logdt_sum(0.0),
            logdt2_sum(0.0)
        {}
    };

    struct GridNode {
        // statistics for gamma, electron, muon, other
        Statistics stats_[4];
        std::vector<unsigned> stations_;
    };

    struct Station {
        I3Position pos_[2]; // cylinder centers [TankA, TankB]
        double radius_; // cylinder radius
        double height_; // cylinder height
        // boundary of sampling ring
        double ring_radius_[2]; 
        double ring_phi_[2];

        Station(const I3Position& posA,
                const I3Position& posB,
                double radius,
                double height);
    };

private:
    double sqrt_r_max_;
    unsigned r_bin_, phi_bin_;
    GridNode* grid_;
    std::vector<Station> stations_;
    I3Particle primary_;
    double transform_[3][3]; // to shower front CS

private:
    Index ToIndex(double sqrt_r, double phi) const;
    GridNode& GetGridNode(Index);

    void ComputeSamplingArea(Station&) const;
    void AssignStationToGrid(unsigned, const Station&);
    double NParticles(Index,
                      const Station&,
                      const I3Particle&,
                      double weight) const;
    unsigned GenerateClones(std::vector<ExtendedI3Particle>&,
                            const Station&,
                            const ExtendedI3Particle&,
                            double npart,
                            I3RandomService&) const;
    void RelocateParticle(ExtendedI3Particle&,
                          const I3Position&,
                          double radius,
                          double height,
                          I3RandomService&) const;

public:
    ParticleUnthinner(double r_max,
                      unsigned r_bin,
                      unsigned phi_bin);
    ~ParticleUnthinner();

    // interface for users of this class
    void AddStation(const I3Position& posA,
                    const I3Position& posB,
                    double tank_radius,
                    double tank_height);
    void SetStations(const I3Geometry&,
                     double dr_safety,
                     double dh_safety);

    void ClearStations();
    void SetPrimary(const I3Particle&);
    const I3Particle& GetPrimary() const { return primary_; }
    void SetPrimaryPos(const I3Position&);
    Index AddParticle(const I3Particle&, double weight);
    unsigned Sample(std::vector<ExtendedI3Particle>&,
                    const ExtendedI3Particle&,
                    double weight,
                    I3RandomService&) const;

    // interface for introspection and debugging
    void ToShowerFrontCS(double& rho,
                         double& phi,
                         double& z,
                         const I3Position&) const;
    Index ToIndex(const I3Position&) const;
    Index GetGridShape() const { return Index(r_bin_, phi_bin_); }
    double GetGridRMax() const { return sqrt_r_max_ * sqrt_r_max_; }
    const GridNode& GetGridNode(Index) const;
    void GetGridSegment(double& r1, double& r2,
                        double& phi1, double& phi2,
                        Index) const;
    const std::vector<Station>& GetStations() const { return stations_; }
};
