
Introduction
------------

	"...the weighting is the hardest part."
	
	-- Tom Petty

Experimental data from an instrument like IceCube are ultimately just a set of
counts. These are typically expressed as rates, which are independent of the
observation time:

.. math::
	
	R \equiv \frac{N}{\Delta t}

In order to simulated data to real data, it is necessary to calculate the rate
predicted by the simulation. Most simulation in IceCube is *weighted*, meaning
that it is generated from a spectrum that does not necessarily reflect any
realistic physical model, and you must calculate *weights* that reflect the
frequency of any given event in the model of interest. These models are given
as differential fluxes:

.. math::
	
	\frac{d\Phi}{dE} \equiv \frac{dN}{dt dA d\Omega dE}

that is, a number of expected events per unit time, area, solid angle, and
energy. The generation spectrum of the simulation is a differential fluence,
that is, the total number of events generated per unit area, solid angle, and
energy. The ratio of the two is a weight in units of :math:`s^{-1}`:

.. math::
	
	w \equiv \frac{dN_{expected}/dt dA d\Omega dE}{dN_{generated}/dA d\Omega dE}

While simple in principle, the calculation of the weight given above can be
quite tedious, especially when multiple particle types or overlapping energy
ranges are involved. This project automates the calculation of the denominator
in the above equation for the most common types of simulation used in IceCube,
and also provides implementations of a few flux models that can be used in the
numerator to weight cosmic ray simulations.

