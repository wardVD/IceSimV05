
Tutorial
--------

For the following examples, we will assume that we are working from an HDF5
file produced by tableio, with a table named "MCPrimary" containing the
generated cosmic ray::

	>>> import tables
	>>> hdf = tables.openFile('corsika.hdf5')
	>>> hdf.root
	/ (RootGroup) ''
	  children := ['CorsikaWeightMap' (Table), '__I3Index__' (Group), 'MCPrimary' (Table)]

Getting normalizations for SimProd datasets
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. py:currentmodule:: icecube.weighting.weighting

In the most common use case, you just want to calculate weights for an
official simulation set. Luckily the generator settings are stored in the
Simulation Production database, and we can construct the normalization term
automatically from the dataset number::

	>>> from icecube.weighting import weighting
	>>> generator = weighting.from_simprod(10087)
	>>> generator *= nfiles

The object returned by :py:func:`from_simprod` represents the normalization for
a single generated file. To get the normalization for the entire set, multiply
by the total number of files in the set `nfiles`, or by the number you
processed if you only have a subset.

Normalizations for custom simulation sets can be constructed directly from
instances of :py:class:`GenerationProbability` or through the factory functions
:py:func:`Hoerandel`, :py:func:`FiveComponent`, and :py:func:`NeutrinoGenerator`.

Weighting "unweighted" dCORSIKA to a different flux model
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. py:currentmodule:: icecube.weighting.fluxes

While we have a lot of simulation lying around generated with the Hoerandel_
model, it does not describe our data terribly well. Luckily, though, it *can*
be re-weighted to reflect a different flux model (e.g. :py:class:`GaisserH3a`)::

	>>> from icecube.weighting.fluxes import GaisserH3a
	>>> from icecube.weighting.weighting import from_simprod
	>>> generator = from_simprod(6939)
	>>> flux = GaisserH3a()
	>>> energy = hdf.root.MCPrimary.cols.energy[:]
	>>> ptype = hdf.root.MCPrimary.cols.type[:]
	>>> weights = flux(energy, ptype)/generator(energy, ptype)
	>>> weights
	array([ 0.11307773,  0.16582993,  0.11443925, ...,  0.10568692,
	        0.12293974,  0.10502099])

.. py:currentmodule:: icecube.weighting

.. note:: The table ``MCPrimary`` must contain exactly one primary for each
          event (see :py:func:`get_weighted_primary`).

.. py:currentmodule:: icecube.weighting.fluxes

Since :py:class:`GaisserH3a` predicts a higher flux than Hoerandel_, the weights
will be slightly larger than the "native" weights::

	>>> weights/(1./hdf.root.CorsikaWeightMap.cols.TimeScale[:])
	array([ 1.13484598,  1.66426601,  1.1485101 , ...,  1.06067193,
	        1.23382088,  1.05398866])

Weighting 5-component dCORSIKA
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Instead of treating all elements from H to Fe like the "unweighted" Hoerandel
simulation, the 5-component version of dCORSIKA only treats H, He, N, Al, and
Fe, representing the contributions of the 5 dominant elements (or in the case
of C/N/O and Mg/Si/Al, element groups) to the cosmic ray flux at Earth. Weighting
this simulation is analagous to the re-weighting case discussed above::

	>>> from icecube.weighting.fluxes import Hoerandel5
	>>> from icecube.weighting import weighting
	>>> generator = weighting.from_simprod(6514)
	>>> generator *= nfiles
	>>> flux = GaisserH3a()
	>>> energy = hdf.root.MCPrimary.cols.energy[:]
	>>> ptype = hdf.root.MCPrimary.cols.type[:]
	>>> weights = flux(energy, ptype)/generator(energy, ptype)

In this example, we've used :py:class:`Hoerandel5`, the 5-component version of
the Hoerandel_ model.

Weights for combined dCORSIKA sets with overlapping energy ranges
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If energy ranges for two simulation sets overlap, then the weights must be
adjusted to account for the fact that more events are generated in the overlap
region of the combined sample than in either one individually. In this example,
we'll use the :py:func:`from_simprod` convenience method to look up the
dCORSIKA configuration in the Simulation Production database. In this case,
we'll combine a set generated on an :math:`E^{-2}` spectrum with one generated
on an :math:`E^{-2.6}` spectrum::

	>>> from icecube.weighting.weighting import from_simprod
	>>> hard = from_simprod(6514)
	>>> soft = from_simprod(9654)

If we have 100k files from the hard spectrum and 77k from the soft spectrum,
the normalization is the sum of the individual generators::

	>>> generator = 100e3*hard + 77e3*soft

After that, the procedure is exactly the same as for the single-set case::
	>>> hdf = tables.openFile('combined_corsika.hdf5')
	>>> energy = hdf.root.MCPrimary.cols.energy[:]
	>>> ptype = hdf.root.MCPrimary.cols.type[:]
	>>> weights = flux(energy, ptype)/generator(energy, ptype)

Weighting NeutrinoGenerator simulation to an atmospheric flux
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For NeutrinoGenerator simulation, the weight map has a different name and
different contents::

	>>> hdf = tables.openFile('nugen.hdf5')
	>>> hdf.root
	/ (RootGroup) ''
	  children := ['I3MCWeightDict' (Table), '__I3Index__' (Group), 'MCPrimary' (Table)]

.. note:: This example uses atmospheric flux models implemented in the `NewNuFlux <http://code.icecube.wisc.edu/svn/sandbox/cweaver/NewNuFlux/>`_ project.

Weighting NeutrinoGenerator simulation requires a few extra complications.
Since interactions are forced, the interaction probability needs to be
multiplied into the weight. Also, atmospheric neutrino fluxes depends on zenith
angle, and are given in different units::

	>>> from icecube import NewNuFlux
	>>> from icecube.icetray import I3Units
	>>> flux = NewNuFlux.makeFlux('honda2006').getFlux
	>>> generator = from_simprod(9250)
	>>> energy = hdf.root.MCPrimary.cols.energy[:]
	>>> ptype = hdf.root.MCPrimary.cols.type[:]
	>>> cos_theta = numpy.cos(hdf.root.MCPrimary.cols.zenith[:])
	>>> p_int = hdf.root.I3MCWeightDict.cols.TotalInteractionProbabilityWeight[:]
	>>> unit = I3Units.cm2/I3Units.m2
	>>> weights = p_int*(flux(ptype, energy, cos_theta)/unit)/generator(energy, ptype, cos_theta)

NeutrinoGenerator calculates and stores a quantity called "OneWeight" that is
analogous to the inverse of the generated fluence. In this case we can use it
to double-check our calculation::

	>>> oneweight = flux(ptype, energy, cos_theta)*hdf.root.I3MCWeightDict.cols.OneWeight[:]/(hdf.root.I3MCWeightDict.cols.NEvents[:]/2)
	>>> weight/oneweight
	array([ 1.,  1.,  1., ...,  1.,  1.,  1.])

In the above calculation we had to divide by 2 to account for the fact that NeutrinoGenerator generates equal numbers of neutrinos and anti-neutrinos of the configured flavor.

Calculating an effective area with NeutrinoGenerator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In order to convert from a theoretical neutrino flux to an event rate in the
detector it is necessary to know the efficiency of the trigger and event
selection. This is usually phrased as an "effective area" that expresses the
surface area of a theoretical 100% efficient detector that would collect the
same number of neutrinos as the real detector and event selection. Calculating
this from NuGen simulation is just another version of the weighting problem:
instead of accumulating fractional contributions to the rate in :math:`1/s`, we
accumulate fractional contributions to the effective area in :math:`m^2`. This
also means that we can combine simulation sets with different energy spectra to
get better statistics at low and high energies. Since
the generation probability has units of :math:`GeV^{-1} \, m^{-2} \, sr^{-1}`,
:math:`1/p_{gen}` has units of :math:`GeV \, m^{2} \, sr`::

	>>> generator = from_simprod(9250)*nfiles
	>>> energy = hdf.root.MCPrimary.cols.energy[:]
	>>> ptype = hdf.root.MCPrimary.cols.type[:]
	>>> areas = 1./generator(energy, ptype)

At this point, the accumulated fractional areas are subject only to selection effects:
the only reason that the total area is not equal to the injection area is that some
events failed to trigger or pass the event selection. In order to obtain a real effective
area, however, we still have to take the neutrino interaction cross-section into account.
We account for the probability that the forced interaction would have happened at natural
rates by multiplying the fractional areas by the interaction probability weight::

	>>> areas *= hdf.root.I3MCWeightDict.cols.TotalInteractionProbabilityWeight[:]

To obtain the effective area in :math:`m^2`, we fill :math:`p_{int}/p_{gen}`
into a histogram, and divide the contents of each bin by its width in primary
energy and solid angle. For example, to calculate the effective area averaged
over the whole sky (i.e. solid angle :math:`4\pi`)::

	>>> energy_bins = numpy.logspace(3, 6, 21)
	>>> binwidth = 4*numpy.pi*numpy.diff(energy_bins)
	>>> area = numpy.histogram(energy, weights=areas, bins=energy_bins)[0]/binwidth

or in zenith bands::
	>>> zenith = numpy.cos(hdf.root.MCPrimary.cols.zenith[:])
	>>> zenith_bins = numpy.linspace(-1, 1, 11)
	>>> binwidth = 2*numpy.pi*reduce(numpy.multiply, meshgrid(numpy.diff(energy_bins), numpy.diff(zenith_bins), indexing='ij'))
	>>> area = numpy.histogramdd(numpy.hstack((energy, zenith)), weights=areas, bins=(energy_bins, zenith_bins))[0]/binwidth

Because :py:func:`from_simprod` returns separate generation probabilities for
neutrinos and anti-neutrinos, the effective area calculated this way is the
*sum* of the neutrino and anti-neutrino effective areas. You can obtain the
specific effective area for neutrinos or anti-neutrinos by filling only
neutrino or anti-neutrino events into the histograms above.

.. note::
	The quantity usually called the "effective area" in IceCube analyses is the
	*average* of the neutrino and anti-neutrino effective areas. Since NuGen
	generates equal numbers of neutrino and anti-neutrino events, you can divide
	by 2 to get an average effective area.

.. _Hoerandel: http://dx.doi.org/10.1016/S0927-6505(02)00198-6
