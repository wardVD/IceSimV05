/**
 * @brief Service Generator for MuonGun
 */
#include <sim-services/I3GeneratorService.h>
#include <MuonGun/Generator.h>
#include <dataclasses/physics/I3MCTreeUtils.h>
#include <phys-services/I3RandomService.h>

class MuonGunBackgroundService: public I3GeneratorService 
{ 
        public: 
                MuonGunBackgroundService() { };
                MuonGunBackgroundService(
                                I3MuonGun::GeneratorPtr generator, 
                                I3RandomServicePtr rng); 
 
                void SetRate(double rate); 
                void SetMCTreeName(std::string mctreeName); 
                void SetRNG(I3RandomServicePtr rng); 
                void SetGenerator(I3MuonGun::GeneratorPtr generator); 
                virtual void Init(); 
                virtual double GetRate(); 
                virtual I3MCTreePtr GetNextEvent(); 
                virtual I3FramePtr GetNextFrame(); 

        private: 
                I3MuonGun::GeneratorPtr generator_; 
                I3RandomServicePtr rng_; 
                double rate_; 
                std::string mctreeName_;

};


I3_POINTER_TYPEDEFS(MuonGunBackgroundService);
