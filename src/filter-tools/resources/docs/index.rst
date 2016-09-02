.. _filter-tools:

filter-tools Documentation
==========================

**Maintainer:** Erik Blaufuss

.. toctree::
   :maxdepth: 3

   release_notes

Overview
~~~~~~~~
A set of tools used in the online filter.  Also used in 
verifying and simulating the online filters.

The Modules
~~~~~~~~~~~
* `CreateFilterMask <../../doxygen/filter-tools/classCreateFilterMask.html>`_ - A c++ module to create a I3FilterResultMap (aka FilterMask) based on I3Bools located in the Frame
* `FilterMaskMaker <../../doxygen/filter-tools/classpython_1_1FilterMaskMaker_1_1FilterMaskMaker.html>`_ - A python module to creat a FilterResultMap (aka FilterMask) based on I3Bools in the frame with configurable prescales.
* `DistributePnFObjects <../../doxygen/filter-tools/classDistributePnFObjects.html>`_ - A module used to help rehydrate the QP frame structure in the compact PFFilt files transfered from pole.
* `FilterMaskFilter <../../doxygen/filter-tools/classFilterMaskFilter.html>`_ - An icepick module that select frames based on a online fiter result
* `FilterCheckModule <../../doxygen/filter-tools/classFilterCheckModule.html>`_ - A utility module that is used to compare an I3FilterResultMap against a set of I3Bools
* `ParticleCheck <../../doxygen/filter-tools/classParticleCheck.html>`_ - A utility module to compare sets of I3Particles. 
* `FilterMask2Bools <../../doxygen/filter-tools/classFilterMask2Bools.html>`_ - Creates a set of I3Bools in the frame based on the contrnts of the  I3FilterResultMap.
* `KeepFromSubstream <../../doxygen/filter-tools/classKeepFromSubstream.htm>`_ - A utility module used in the online filter to flatten the QP strucure for the PFFilt files. Keeps selected objects in Q frame and labels them to match the source split and subevent number.
* `OrPframeFilterMasks <../../doxygen/filter-tools/classOrPframeFilterMasks.html>`_ - A utility module used in the online filter to combine the filter results in I3FilterResultMap entries from multiple P frames.

Also see the `doxygen <../../doxygen/filter-tools/index.html>`_ docs.
