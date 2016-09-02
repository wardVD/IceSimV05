/**
 *   copyright  (C) 2005
 *   the IceCube Collaboration
 *   $Id:  $
 *   @version $Revision: $
 *   @date    $Date:     $
 *   @author Aya Ishihara <aya.ishihara@icecube.wisc.edu>
 *
 *   @brief Interaction implimentation file
 *
 */
//////////////////////////////////////////////////////////////////////

#include "neutrino-generator/Steering.h"
#include "neutrino-generator/Particle.h"
#include "neutrino-generator/interactions/InteractionBase.h"
#include "neutrino-generator/table-interface/CrosssectionTableReader.h"
#include "neutrino-generator/table-interface/FinalStateTableReader.h"
#include "neutrino-generator/utils/Constants.h"
#include "neutrino-generator/utils/Calculator.h"
#include "icetray/I3Units.h"

using namespace std;
using namespace earthmodel;

namespace nugen {

//____________________________________________________________________
InteractionBase::InteractionBase(I3RandomServicePtr random,
                                 SteeringPtr steer)
       :random_(random), steer_(steer)
{
  cross_file_ptr_ = CrosssectionTableReaderPtr(new CrosssectionTableReader());
  final_file_ptr_ = FinalStateTableReaderPtr(new FinalStateTableReader());
}

//____________________________________________________________________
InteractionBase::~InteractionBase()
{ 
  log_trace("Interaction deconstructed");
}

//____________________________________________________________________
void InteractionBase::InitializeCrosssectionTable(const string& sigmafile)
{
  sigmaname_ = sigmafile;
  cross_file_ptr_->ReadCrosssectionTableFillArray(sigmafile);
}

//____________________________________________________________________
void InteractionBase::InitializeFinalStateTable(const string& finalfile)
{  
  finalname_ = finalfile;
  final_file_ptr_->ReadFinalStateTableFillArray(finalfile);
}

//____________________________________________________________________
double InteractionBase::GetXsecCGS(const double energy) 
{
    double log_e = log10(energy);  
    double sigma = cross_file_ptr_->EvaluateCrosssection(log_e);
    //log_trace("Interaction type %s, Sigma for log(E[GeV])=%f is %f * 10 ^-12 [mb]", 
    //           GetInteractionTypeString().c_str(), log_e, sigma*1.0e12);

    // conversion : mb to cm2
    return sigma * 1e-27;
}

//___________________________________________________________________
I3Direction InteractionBase::GetLeptonDirection(const I3Direction& direction, 
                                            const double lepton_energy, 
                                            const double cos_theta) {
  //
  // Build an arbitrary dir axil, perpendicular
  // to the neutrino 3-momentum and assume it is {1,0,z};
  // as axil*nu_p = 0, px*1+py*0+pz*z=0, => z = -px/pz
  //

  // for following calculation I3Position is more convenient than
  // I3Direction, so let's use I3Position.
  I3Position axil(0.0, 0.0, 0.0);
  I3Position vector_out_direction(0.0, 0.0, 0.0);

  if (direction.GetZ() != 0.0){
    axil.SetX(1.0);
    axil.SetY(0.0);
    axil.SetZ((-direction).GetX()/direction.GetZ());
  } else {
    axil.SetX(1.0);
  }

  // throw an arbitrary azimuth angle
  double phi = random_->Uniform( 0.0, 2.0*I3Constants::pi );  
  I3Direction vector_in_direction(direction);
  // the rotated transversal unitary vector
  I3Direction axil_dir(Calculator::Rotate(axil, phi,
                                    vector_in_direction));

  double sin_theta = sqrt((1 + cos_theta) * (1 - cos_theta));

  // transversal  component of muon 3-momentum
  I3Position mu_trans = lepton_energy * sin_theta * axil_dir;
  // longitudinal component of muon 3-momentum
  I3Position mu_long  = lepton_energy * cos_theta * vector_in_direction;   
  vector_out_direction = mu_long + mu_trans;

  I3Direction in_dir(vector_in_direction);
  I3Direction out_dir(vector_out_direction);

  log_trace("incoming neu  direction (theta, phi)=(%f,%f)", 
                   in_dir.CalcTheta(), in_dir.CalcPhi());
  log_trace("outgoing muon direction (theta, phi)=(%f,%f)", 
                   out_dir.CalcTheta(), out_dir.CalcPhi());

  return out_dir;

}

//___________________________________________________________________
std::vector<double> InteractionBase::SelectXY(double log_e) 
{
  // make final states of the produced particles
  // if muon, lepton moves in a different direction  
  double finalxy[2]={0.0, 0.0};

  //fill final stage x and y to finalxy
  final_file_ptr_->SampleFinalState(log_e, random_->Uniform(0.0, 1.0), 
                                   random_->Uniform(0.0, 1.0), finalxy);
  
  // checks are necessary in case tables contain buggy numbers
  while (finalxy[1]<0||finalxy[1]>1||finalxy[0]<0||finalxy[0]>1) {
     log_warn("NC interaction: x=%f or y=%f is out of range, retry", 
               finalxy[0], finalxy[1]);
     final_file_ptr_->SampleFinalState(log_e, 
                                   random_->Uniform(0.0, 1.0), 
                                   random_->Uniform(0.0, 1.0), 
                                   finalxy);
  }

  std::vector<double> result;
  result.push_back(finalxy[0]);
  result.push_back(finalxy[1]);
  log_trace("final state x and y = (%f, %f)", result[0], result[1]);
  return result;
}

//___________________________________________________________________
double InteractionBase::SetSecondaryLepton(ParticlePtr nuin_ptr,
                        I3Particle::ParticleType leptonFlavor,
                        double lepton_mass, 
                        bool skipCalcCosTheta)
{

  double nu_energy = nuin_ptr->GetEnergy();
  if (std::isnan(nu_energy) || nu_energy <= 0) {
     log_fatal("Invalid neutrino energy %f", nu_energy);
  }

  // select a set of x, y
  const double log_e = log10(nu_energy/I3Units::GeV);
  std::vector<double> xy = SelectXY(log_e);
  double x = xy[0];
  double y = xy[1];
  InteractionInfo& intinfo = nuin_ptr->GetInteractionInfoRef();
  intinfo.SetBjorkenX(x);
  intinfo.SetBjorkenY(y);

  // set outgoing lepton energy
  double out_lepton_E = (1 - y) * nu_energy;

  // get original direction
  I3Direction direction = nuin_ptr->GetDir();

  ParticlePtr lepton_daughter_particle(new Particle(I3Particle::Null,
                                       I3Particle::unknown, steer_));
  lepton_daughter_particle->CopyExtraInfoFrom(*nuin_ptr);

  if (out_lepton_E <= lepton_mass) {

     // secondary lepton has no energy.
     // create a rest lepton

     out_lepton_E = lepton_mass;

     lepton_daughter_particle->SetFitStatus(I3Particle::NotSet);
     lepton_daughter_particle->SetPos(nuin_ptr->GetEndPosition());
     double daughter_time = nuin_ptr->GetTime()+nuin_ptr->GetLength()/nuin_ptr->GetSpeed();
     lepton_daughter_particle->SetTime(daughter_time);//7
     lepton_daughter_particle->SetDir(direction);//8
     lepton_daughter_particle->SetType(leptonFlavor);
     lepton_daughter_particle->SetEnergy(out_lepton_E);
     lepton_daughter_particle->SetLength(NAN);//9
     lepton_daughter_particle->SetSpeed(0.);//10

     // set it dark since anyway it's not visible
     lepton_daughter_particle->SetShape(I3Particle::Dark);

     nuin_ptr->AddDaughter(lepton_daughter_particle);

     return out_lepton_E;
  }

  // if secondary particle is a track with energy 1GeV,
  // scattering angle will be taken into account

  if (!skipCalcCosTheta &&
      (Utils::IsMuFlavor(leptonFlavor) || 
       Utils::IsTauFlavor(leptonFlavor) || 
       Utils::IsNuE(leptonFlavor))) {

     double outgoing_costheta = 1.0;

     if (steer_->UseSimpleScatterForm()) {
        outgoing_costheta = CalcOutgoingCosThetaSimple(
                            nu_energy, x, y);

     } else {
        outgoing_costheta = CalcOutgoingCosTheta(
                        nu_energy, lepton_mass, x, y);
     }

     // sanity check!
     // sometimes outgoing energy is too small and 
     // the outgoing_costheta is out of range.
     // If outgoing energy is less than 0.2GeV (which muon range is 1.2m),
     // we may be able to ignore the direction, then assign random costheta.
     // If the outgoing energy exceeds 0.2GeV, let's try to get
     // another sample.
     // this is dirty fix, check your setup if you see too much
     // errors.

     static const double ethres = 0.2 * I3Units::GeV;

     if (fabs(outgoing_costheta) > 1.0) {
        log_notice("Outgoing Muon direction (x = %f, y = %f, nu_energy = %f, out_lepton_E = %f, cos_theta = %f) "
               " cos_theta is out of range.",
               x, y, nuin_ptr->GetEnergy(), out_lepton_E,
               outgoing_costheta);
        if (out_lepton_E < ethres) {
           outgoing_costheta = random_->Uniform(-1.0, 1.0);
           log_notice("because outgoing lepton energy is less than %f GeV, random costheta = %f is assigned) "
                     , ethres, outgoing_costheta);

        } else {
           log_warn("outgoing energy is more than %f GeV. Dirty Fix : "
                     "retry to get new x and y", ethres);
           SetSecondaryLepton(nuin_ptr, leptonFlavor, 
                              lepton_mass, skipCalcCosTheta);
           return out_lepton_E;
        }
     }

     // update direction
     direction = GetLeptonDirection(direction, out_lepton_E, 
                                    outgoing_costheta);
     log_debug("Outgoing angle is %f deg", acos(outgoing_costheta)/I3Units::degree);

  } //else direction for the secondary is the same as parent neutrino
  
  lepton_daughter_particle->SetFitStatus(I3Particle::NotSet);
  lepton_daughter_particle->SetPos(nuin_ptr->GetEndPosition());
  double daughter_time = nuin_ptr->GetTime()+nuin_ptr->GetLength()/nuin_ptr->GetSpeed();
  lepton_daughter_particle->SetTime(daughter_time);//7
  lepton_daughter_particle->SetDir(direction);//8
  lepton_daughter_particle->SetType(leptonFlavor);
  lepton_daughter_particle->SetEnergy(out_lepton_E);
  lepton_daughter_particle->SetLength(NAN);//9
  lepton_daughter_particle->SetSpeed(I3Constants::c);//10
  
  if (Utils::IsTau(leptonFlavor)) {
      // no additional setting?
  }

  if (Utils::IsElectron(leptonFlavor)) {
     lepton_daughter_particle->SetShape(I3Particle::Cascade);

  } else {
     // since we set length as NAN (length of mu/tau will be 
     // treated by mmc!),
     // we set shape as Null.
     lepton_daughter_particle->SetShape(I3Particle::Null);
  }

  nuin_ptr->AddDaughter(lepton_daughter_particle);

  // return energy of outgoing lepton
  return out_lepton_E;

}

//___________________________________________________________________
double InteractionBase::CalcOutgoingCosThetaSimple(
                        double ene,
                        double x, double y) 
{
  //--------------------
  // original equation :
  //--------------------
   //double outgoing_costheta = 1 - (x*y*M_N/nuin_ptr->GetEnergy()/(1. - y));
  // M_N is nucleon mass
  // however the second term could be greater than 1 if
  // y is close to 1 and particle energy is small.
  // (outgoing_lepton_energy = (1-y)*energy is too small.)

  double out_lepton_E = (1 - y) * ene;
  double costheta = 1.0 - (x*y*Constants::M_N/out_lepton_E);
  return costheta;
}


//___________________________________________________________________
double InteractionBase::CalcOutgoingCosTheta(
                        double ene,
                        double lepton_mass, 
                        double x, double y) 
{
  //--------------------
  // equation for costheta :
  //--------------------
  // 
  // Aug 13 2013 
  // in order to adopt the costheta to tau flavor
  // old approximate form is updated to precise form
  // coded by Askhat Gazizov (see resources/scripts/angles_dis.py)
  //

  const double m2 = lepton_mass * lepton_mass;
  const double ene2 = ene * ene; // square of neutrino E
  const double m_loverE = lepton_mass / ene;
  static const double m_n = Constants::M_N; //nuclear mass
  
  double y1  = 1 - y;
  double xmin = m2 / (2 * m_n * (ene - lepton_mass));

  if (x < xmin) {
     log_debug("x(%g) < x_min(%g), error!", x, xmin);
  }

  // upper branch of Y(x) due to mass of lepton
  double a =  (1.0 - m2*(1.0/(2.0*m_n*ene*x) + 0.5/ene2)) / 
              (2 + m_n * x / ene);

  // calculate limits of y(yl, yu)
  double s1 = 1.0 - m2/(2.0*m_n*ene*x) - m_loverE;
  if (s1 < 0.) s1 = 0.0;
  double s2 = 1.0 - m2/(2.0*m_n*ene*x) + m_loverE;
  double b =  sqrt(s1)*sqrt(s2)/(2.0 + m_n*x/ene);
  double yl = a - b;
  double yu = a + b;

  // check y range
  if ((y-yl)*(y-yu) > 0) {
     log_debug("y=%g must be in range [%g,%g]. log10E = %g",
                y, yl, yu, log10(ene));
  }

  // calculate costheta
  double kappa = m_loverE * m_loverE;
  double beta_l = sqrt(1 - kappa / (y1*y1));
  double cos_theta = (1 - (m_n*x*y/ene + kappa/2)/y1)/beta_l;

  log_debug("cos_theta=%e, theta in degrees=%e", cos_theta,
            acos(cos_theta)/I3Units::degree);

  return cos_theta;

}

}
