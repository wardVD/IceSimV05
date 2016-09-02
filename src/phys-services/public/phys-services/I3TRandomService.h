#ifndef I3TRANDOMSERVICE_H
#define I3TRANDOMSERVICE_H

#include "phys-services/I3RandomService.h"
#include "TRandom3.h"

/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id: I3TRandomService.h 94949 2012-11-04 16:40:30Z nwhitehorn $
 *
 * @brief An implementation of the I3RandomService interface.  
 * 
 * Just uses
 * a private TRandom to implement the methods of I3RandomService.
 *
 * @version $Revision: 94949 $
 * @date $Date: 2012-11-04 10:40:30 -0600 (Sun, 04 Nov 2012) $
 * @author pretz
 */
class I3TRandomService : public I3RandomService{
 public:
  /**
   * default constructor
   * Builds the private TRandom in the default way.
   */
  I3TRandomService(){};

  /**
   * constructor
   * Builds the private TRandom with the indicated seed
   * @param seed the seed value you want for the TRandom
   */
  I3TRandomService(unsigned int seed) : rand_(seed){}

  /**
   * destructor
   */
  virtual ~I3TRandomService(){};

  /**
   * a number drawn from a binomial distribution
   */
  virtual int Binomial(int ntot, double prob);

  /**
   * A number from an Exponential distribution
   */
  virtual double Exp(double tau);

  /**
   * An integer drawn uniformly from [0,imax)
   */
  virtual unsigned int Integer(unsigned int imax);

  /**
   * An integer drawn from a Poisson distribution
   */
  virtual int Poisson(double mean);

  /**
   * A number drawn from a Poisson distribution, as a double
   */
  virtual double PoissonD(double mean);

  /**
   * a double drawn from a uniform distribution (0,x1)
   */
  virtual double Uniform(double x1 = 1);

  /**
   * a double drawn from a uniform distribution (x1,x2)
   */
  virtual double Uniform(double x1, double x2);

  /**
   * a double drawn from a Gaussian distribution with given
   * mean and standard deviation
   */
  virtual double Gaus(double mean, double stddev);
 private:
  TRandom3 rand_;

  SET_LOGGER("I3TRandomService")

};

I3_POINTER_TYPEDEFS(I3TRandomService);


#endif //I3TRANDOMSERVICE_H
