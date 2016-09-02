.. _diplopia:

Diplopia
========

.. toctree::
   :maxdepth: 3
   
   release_notes
   poisson_merger
   polyplopia
   mphit_filter



Overview
--------

This project contains tools for generatig Monte Carlo IceCube events consisting of multiple primaries (and secondaries) that generate coincident detector hits. These types of events are often misreconstructed as upgoing muons and are difficult to distinguish from such.

At this time, the project consists of the following two modules:

   * I3PolyplopiaExp takes events from multiple frames and merges events of various multiplicities by sampling time intervals from an exponential distribution based on the rate of minimum-threshold events.

   * PoissonMerger injects background event read from a separate file ontop of primary events in the chain by sampling from a Poisson distribution.  This C++ module replaces an oder Python module and can merge events at the I3MCTree-level after generation. This is likely to become the prefered way since the combined MCTree can be processed through photon propagation at once instead of having to do this in separate tasks during production.
   * MPHitFilter removes events that don't produce light in the detector and removes branches of I3MCTrees whose particles don't produce enough PEs in the detector, making it much easier to read and reducing the storage requirements.

The C++ PoissonMerger module also makes use of a CoincidentEventService that could be drop-in replaced with other event services such as a MuonGun-based service in the future.

It is then up to CoincidenceAfterProcessing from trigger-sim to split up and clean up events based on triggers.
