Using the Various Random Number Services
----------------------------------------

Description
&&&&&&&&&&&

**I3RandomService** is patterned off of ROOT's TRandom, and provides access 
to random numbers drawn from several possible distributions.  The 
I3RandomService is an abstract interface, which can be serviced by any of
several implementations.  

The advantage of having an I3RandomService interface between us and the 
random number generator.  We can dynamically switch between any implementations 
of I3RandomSerivce.

Currently there are three implementations of this interface.  

* **I3TRandomService** - uses a private instance of a TRandom to create
the random numbers.  If you want to use this implementation of the
I3RandomService you install it with the I3TRandomServiceFactory.  That
factory takes one integer parameter 'Seed' which is the seed passed to the 
instance of TRandom for a seed. 

* **I3GSLRandomService** - uses the gnu scientific library
to generate random numbers.  It is added to the framework with the
I3GSLRandomServiceFactory.  It takes a single integer parameter 'Seed' which is 
given to gsl as the seed for the random number sequence. 

* **I3SPRNGRandomService** - uses the SPRNG library to
produce independent streams of pseudo-random numbers for distributed
computation.

