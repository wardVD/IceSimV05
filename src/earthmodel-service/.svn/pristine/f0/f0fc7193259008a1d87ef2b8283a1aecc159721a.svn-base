.. 
.. copyright  (C) 2013
.. Kotoyo Hoshina <hoshina@icecube.wisc.edu>
.. and The Icecube Collaboration http://www.icecube.wisc.edu
.. 
.. $Id$
.. 
.. @version $Revision$
.. @date $LastChangedDate$
.. @author Kotoyo Hoshina <hoshina@icecube.wisc.edu> $LastChangedBy$

.. highlight:: python

.. _earthmodel-service:

==================
earthmodel-service
==================

now its for all uses....
see https://wiki.icecube.wisc.edu/index.php/Earthmodel-service for updates.

.. toctree::
   :maxdepth: 3
   
   release_notes

EarthModelService
=================

overview
--------

EarthModelService provides density of the Earth at a given point in Earth-center coordinate.
Following users are strongly encouraged to use this module to make your simulation consistent.

   1. Users who need information of density of the Earth (includes bedrook and ice, air)
   2. Users who need to coordinate conversion between IceCube coordinate and Earth Center coordinate
   3. Users who need to calculate muon range
   4. Users who are using root just because they want to use TVector3

They are treated by following classes respectively.

   1. EarthModelService
   2. EarthModelService
   3. EarthModelCalculator or EarthModelService
   4. RVector3

What you need to prepare to get your own Earth Model
----------------------------------------------------

Currently all density profile below Moho-boundary is hardcoded inside EarthModelService.
Upper structures such as bedrook, ice, air and the definition of the EarthRadius are provided with crust model files, which must follow following format. See details in resources/earthparams/PREM_mmc.dat .

    * Format should follow next three colums
      upper_radius[m]  density[g/cm^3]   label[string]
    * Data must be in ascending order in upper_radius
    * no space before first column
    * delimiter must be whitespaces
    * data must include at least 5 lines with labels of "moho_boundary", "rockice_boundary","iceair_boundary", "earth_radius", and "atmo_radius"
    * density must be the density up to the defined radius, except for "moho_boundary"
    * density of "moho_boundary" should be same as  the density of next layer
    * "earth_radius" is NOT the geophysical boundary. This is the definition that all simulation program regards "the radius of the Earth".
    * lines start from '#' or whitespace are treated as comments


Note about Earth Radius for NuGen users
---------------------------------------

If you want to use NuGen with a large injection volume that may cross over the boundary of the Earth surface (with default setting it's same as the radius of the ice surface),  you have to increase earth_radius parameter so that contain all your injection volume within the radius.
In this case, the earth_radius parameter is quite similar concept of "World" volume of Geant4.
Do not forget to make all data in ascending order in upper_radius.


Classes descriptions
====================
 
RVector3 (Root-style Vector3)
-----------------------------

This is just copy of the TVector3, but is removed any root-dependency (does not delive TObject, inactivated some functions that use TVector2 etc.).
If your module depend on root just because you want to use TVector3, you may switch it to RVector3 and your module won't require root anymore.

It also offer constructors that takes I3Position or I3Direction as an argument. If you want an I3Position from a RVector3, you may use GetI3Pos() function. See examples.

::
   // conversion from I3Position etc. to RVector3

   I3Position pos;
   I3Direction dir;

   RVector3 p(pos);
   RVector3 d(dir);

   // conversion from R3Vector3 to I3Position etc.

   I3Postion pos2 = p.GetI3Pos();
   I3Direction dir2 = d.GetI3Dir();


EarthModelService
-----------------

Coordinate conversion 
~~~~~~~~~~~~~~~~~~~~~

EarthModelService offers coordinate conversion to get EarthCenter coordinate position from IceCube coordinate position, etc. Arguments and return value could be I3Position, I3Direction, and RVector3. See details in header comments.

GetEarthDensityInCGS, GetMedium
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

GetEarthDensityInCGS(point) function returns density at the given point.
REMEMBER, the return value is in CGS unit [g/cm3] !

GetMedium(point) function returns medium type which is defined in EarthModelService as enum:

    * INNERCORE
    * OUTERCORE
    * MANTLE
    * ROCK
    * ICE
    * AIR
    * VACUUM

GetMuonRangeInMeter
~~~~~~~~~~~~~~~~~~~

This function returns muon range in [m] with a given track geometory, energy and earth model.
You must choose arguments carefully to get expected behavior.


EarthModelCalculator
--------------------

It provides function to obtain muon range (in meter water equiv.)  and some geometrical calculation tools.

* GetImpactParameter -- gives impact parameter of a track to origin of your coordinate.
* GetIntersectionsWithSphere -- gives intersections between a track and sphere. The track must be in a coordinate system that have origin at the center of the sphere.
* GetIntersectionsWithCylinder -- gives intersections between a track and cylinder. The track must be in a coordinate system that have origin at the center of the cylinder.







