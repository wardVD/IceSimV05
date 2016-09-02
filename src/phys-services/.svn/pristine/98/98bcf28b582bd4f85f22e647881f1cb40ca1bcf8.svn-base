/*
 * class: I3TRandomService
 *
 * Version: $Id$
 *
 * Date: $Date$
 *
 * (c) IceCube Collaboration
 */

#include "phys-services/I3TRandomService.h"

// not much going on here.  These calls just forward the call to the 
// underlying instance of TRandom

int I3TRandomService::Binomial(int ntot, double prob)
{
  return rand_.Binomial(ntot,prob);
}

double I3TRandomService::Exp(double tau)
{
  return rand_.Exp(tau);
}

unsigned int I3TRandomService::Integer(unsigned int imax)
{
  return rand_.Integer(imax);
}

int I3TRandomService::Poisson(double mean)
{
  return rand_.Poisson(mean);
}

double I3TRandomService::PoissonD(double mean)
{
  return rand_.PoissonD(mean);
}

double I3TRandomService::Uniform(double x1)
{
  return rand_.Uniform(x1);
}

double I3TRandomService::Uniform(double x1, double x2)
{
  return rand_.Uniform(x1,x2);
}

double I3TRandomService::Gaus(double mean,double stddev)
{
  return rand_.Gaus(mean,stddev);
}
