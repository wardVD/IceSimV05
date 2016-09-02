
Weighting in IceTray scripts
----------------------------

Some users may prefer to calculate weights in ahead of time in IceTray and
write them to the frame during their processing instead of calculating them
after the fact. This approach has significant disadvantages: it requires that
the user choose the flux model that will be used for weighting ahead of time,
makes it impossible to combine simulation sets with overlapping energy ranges,
and is in general less efficient, since vector calculations on Numpy arrays are
usually faster than looping over I3Frames. Within these limitations, though,
this project provides a module, :py:class:`icecube.weighting.CORSIKAWeightCalculator`,
that can be used to insert weights in frames as they go by. It can be used for
example as follows::
	
	from icecube.weighting import CORSIKAWeightCalculator
	from icecube.weighting import fluxes
	from os.path import expandvars
	tray.Add(CORSIKAWeightCalculator, 'GaisserH3aWeight', Dataset=10285, Flux=fluxes.GaisserH3a())

In this example, :py:class:`icecube.weighting.CORSIKAWeightCalculator` will put an
I3Double called 'GaisserH3aWeight' in each frame. A similar module,
:py:class:`icecube.weighting.NuGenWeightCalculator`, can be used to calculate
weights for NeutrinoGenerator simulation using fluxes from the NewNuFlux project.

Note that these modules query normalization information from the simprod
database, and will fail on clusters if the nodes do not have internet access or
generate so too connection attempts. For best performance, pre-populate the
local cache (by default stored at `$I3_BUILD/weighting/resources/dbcache.pickle`) with
the datasets you will query before you submit your jobs, e.g.::
	
	from icecube import weighting
	cache = weighting.SimprodNormalizations()
	for dataset in (10285, 11029, 11069, 11070):
		cache.refresh(dataset)

.. automodule:: icecube.weighting.weighting

.. class:: icecube.weighting.CORSIKAWeightCalculator
	
	Parameters:
	
	- **Dataset**: Either a SimProd dataset number, or a pre-configured :py:class:`GenerationProbability` object
	
	- **Flux**: Flux to use for weighting (e.g. :py:class:`icecube.weighting.fluxes.GaisserH3a`)

:py:class:`icecube.weighting.CORSIKAWeightCalculator` needs to know which of the
primaries in the MCTree was drawn from a biased spectrum. The function that it
uses to extract this information,
:py:func:`icecube.weighting.get_weighted_primary`, can also be run as an
I3Module, for example when writing HDF5 files with
:py:func:`icecube.hdfwriter.I3HDFWriter`.

.. autofunction:: icecube.weighting.get_weighted_primary

.. class:: icecube.weighting.NuGenWeightCalculator
	
	Parameters:
	
	- **Dataset**: Either a SimProd dataset number, or a pre-configured :py:class:`GenerationProbability` object
	
	- **Flux**: Flux to use for weighting. This should be an object with a
	            member function `getFlux(particleType, energy, cos_theta)`
	            that returns a flux per neutrino type (i.e. neutrinos and
	            antineutrinos separately) in units of 1/GeV cm^2 sr s (e.g. an
	            instance of :py:class:`icecube.NewNuFlux.FluxFunction`).

