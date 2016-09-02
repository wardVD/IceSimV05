
==============
Code Structure
==============

Class-Inheritance
-----------------

The main classes in PROPOSAL are the Physics Model-Class and Math Model class.

From the math model class inherit all other classes which are needed for the calculation of the cross sections. All cross sections are are first integrated and then "mostly" needed results are interpolated between the precalculated points. The Number of interpolation points can be set in MathModel.h.

The other main class is the Physics Model class. Most of the classes inherit from the physics model except (except Output). The cross sections are splitt into the different interactions for each class and inherit from cross sections. Each class is then split into an continuous and a stochastic class.

The whole code structer is displayed in the image below.

.. figure:: inheritance.png
   :scale: 50 %
   :align: center

The whole propagation is then done by the Amanda class which builds a propagator for each media and each particle to propagate.


Propagation Routine
-------------------

The propagation routine which does all the physics work is in the Propagate class and is the Propagate\:\:propagateTo(r,e) function. The steps which PROPOSAL does during the propagation are best explained in the figure below.

.. figure:: PROPOSAL_propagation_flow.png
   :scale: 30 %
   :align: center

If one wants to look in the function and understand it in detail it is usefull to know that most variables which carry an "i" are for the interaction and these who carry and "d" are for the decay.


Mediadef File
-------------
The mediadef file defines how the media looks which is used for the propagation. The standard mediadef file (resources/mediadef) looks like this::
	
	all                 0 0.05 -1 1 -1 5.e2 0 0.673 Air
	sphere 1948 6374134 0 0.05 -1 1 -1 5.e2 0 0.832 Ice
	sphere 1748 6373934 1 0.05 -1 1 -1 5.e2 0 1.005 Ice
	sphere -862 6371324 0 0.05 -1 1 -1 5.e2 0 1.0 Standard Rock
	
The first value defines the geometric structure. For a sphere you can build a sphere shell with outer radius R and inner Radius R-z (e.g first sphere: R = 6374134 , Ri = 6374134-1948 ). The third value defines if the detector is in this geometric object. 

Then a pair of three variables follows:

- Vcut
- Ecut
- cont

The cut defines the minimal relative energy loss which is calculated and Ecut the absolute. All stochastic losses below this limit are are averaged. The cont parameter adds a gaussian smearing to the averaged losses (for details see paper).

The following parameter are the material and a modification factor for the density of this media.
