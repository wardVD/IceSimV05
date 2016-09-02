Weights in NuGen
----------------

What is an interaction weight?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This not is about **interaction weight** which compensates force interactions
carried out inside neutrino-generator.

If you apply **interaction weight** to your NuGen simulation, you will get a
histogram that follows generation energy spectrum of primary particles.
For example, if you generate your simulation with E^-1, the resulting
histogram also follows E^-1.

*In order to get physics histograms under assumption of Atmospheric neutrino
or any physical astro-neutrino, you have to apply another weight using
`OneWeight` or weighting module*


How an event is weighted?
^^^^^^^^^^^^^^^^^^^^^^^^^

Currently event weight of neutrino generator (NuGen) consist of two parts:
propagation weight and interaction weight.
These weights correspond to following weighting procedures respectively:

* Forbid CC interaction inside the Earth so that all neutrinos are propagated
  up to near by the IceCube detector (weighted propagation)
* Make force interaction inside a detection volume near by the IceCube
  (weighted interaction)

The propagation weight was always 1 for version V00-05-04 or older, and
configurable from version V00-05-05 or later.

.. CAUTION::

   Do not use weighted propagation for NuTau simulation!
   Since weighted propagation forbids CC interaction, tau regeneration
   won't happen.

Overview of NuGen propagation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. figure:: figs/LpLd.gif

* Calculate total path length inside the Earth using injected neutrino
  geometry. Separate the total path length into propagation area
  (distance SF) and detection volume (distance FE).
* Define a step length dx[m] using propagation area (distance SF) and
  step number nx. Default nx is 10000.
* In each step, calculate a column depth within dx[m] and an Earth's
  density at the step point.
* Calculate a total cross section at the step point.
* Calculate a probability that the injected neutrino interacts within
  the step. Try Monte-Carlo, and decide whether an interaction happened
  within the step.
* If any interaction occurred, Choose an interaction with another random toss.

  * If CC-interaction is selected with injection particle NuMu or NuE, stop
    the propagation so that this event is killed. Start new propagation with
    a new particle.
  * For other cases, generate secondaries and go to the next step.

* If nothing happens, go to the next step.
* finish propagation when the injected neutrino and generated secondaries
  reach to the front surface of detection volume (point F), then process a
  weighted interaction.

Weighted propagation
^^^^^^^^^^^^^^^^^^^^

If we activate weighted propagation, the procedure 6 is modified.

NuGen always choose NC or GR interaction, then apply a propagation weight as 

.. math::
   (\sigma_{NC} + \sigma_{GR}) / (\sigma_{NC} + \sigma_{CC} + \sigma_{GR})

   GR: Glashow resonance if applicable

Note that the verdict whether any interaction happens or not (procedure 5)
is done based on the total cross section *which includes CC interaction,*
even if we apply weighted propagation.

Following figures show that the final (weighted) number of events of weighted
propagation and non-weighted propagation matches well within statistical
errors. The plot shows OneWeight parameter (which is proportional to the
number of events) for starting or contained events only. OneWeight parameter
depends on detection volume sizes, therefore we always have to compare number
of "triggered" events to do a reasonable comparison.

parameter to control weighted propagation
"""""""""""""""""""""""""""""""""""""""""

Set parameter to I3NeutrinoGenerator or I3NeutrinoPropagator.

* **PropagationWeightMode** 

  *NoPropWeight* :
    Do *not* activate propagation weight. Propagation weight
    is always 1. *(default)*
  
  *NCGRWeighted* :
    Activate propagation weight, CC interaction is forbidden during
    In-Earth propagation.
  
  *AutoDetect* :
    Use NCGRWeighted for NuE, NuMu and NoPropWeight for NuTau
  
  *Legacy* :
    This option activates old event class. If you want to reproduce
    NuGen V00-05-04 or older, use it. (propagation weight is 1)

* Use **NoPropWeight** option for NuTau simulation

Weighted interaction
^^^^^^^^^^^^^^^^^^^^

If neutrino(s) reach to the front surface of detection volume, NuGen makes
force interaction somewhere inside the detection volume. Interaction weight
must be applied in order to compensate it.

.. note::

   In the case of NuTau simulation, we may have multiple neutrinos at the
   front surface of detection volume. However, NuGen randomly chose only
   one of them to make force interaction, with taking into account of
   "total interaction probability inside the detection volume" of each
   neutrino candidate. Theoretically we might have two (or more) neutrinos
   interacted inside the detection volume, but the probability of this
   type of event should be negligibly small. On the other hand, taus
   generated inside propagation area and reached to the detection volume
   will be stored and handed over to the mmc program.

A pure interaction weight is defined as:

.. math::
   P_{surviving} = exp(-\sigma_{all} * L_{d} / M_{p} * C)

   P_{interaction} =  1 - P_{surviving}
    
where

   :math:`L_{d}` : total column depth within the detection volume [:math:`g/m^2`]

   :math:`M_{p}` : proton mass [:math:`g`]

   :math:`\sigma_{all}` : total cross section at interaction point [:math:`mb`]

   :math:`C` : unit conversion factor 1.0e-31 (1[:math:`mb`] = 1.0e-31[:math:`m^2`])
 
This is a part of total interaction weight, because NuGen samples
interaction position with a flat probability distribution function

.. math::
   P_{pos\_NuGen}(X) = 1 / L_{d}
    
where 
    
   :math:`X` : interaction column depth from the entrance of detection
   volume to the interaction position [:math:`g/m^2`]

instead of using the following exponential distribution

.. math::
   P_{pos\_True}(X) = 1 / sum * exp(- \sigma_{all} * X / M_{p} * C)

   sum = \int_0^{L_d}  exp(- \sigma_{all} * X / M_{p} * C) dX 

Thus we need to apply a position weight too.

.. math::
   \begin{align}
   W_{pos} & = P_{pos\_True}(X) / P_{pos\_NuGen}(X) \\
   & =  (L_{d} / sum) * exp(- \sigma_{all} * X / M_{p} * C) \\
   \end{align}

The net interaction weight is then expressed as:

.. math::
   W_{interaction} = P_{interaction} * W_{pos}

If you want to calculate interaction weight by yourself, I3MCWeightsDict
provides all information you need. See following parameters:

========================  ========================    ==========================
 Variables                    Units                    Names in I3MCWeightDict
========================  ========================    ==========================
:math:`W_{interaction}`   (unit less)                 <s>TotalInteractionProbabilityWeight</s> TotalWeight
:math:`P_{interaction}`   (unit less)                 <s>TotalInteractionProbability</s>  InteractionWeight
:math:`W_{pos}`           (unit less)                 <s>TotalPropagationProbability</s> PropagationWeight
:math:`L_{d}`             :math:`g/cm^{2}`            <s>TotalColumnDepth</s> TotalColumnDepthCGS
:math:`X`                 :math:`g/cm^{2}`            <s>InteractionColumnDepth</s> InteractionColumnDepthCGS
:math:`\sigma_{all}`      <s>mb</s> :math:`cm^{2}`    <s>TotalCrosssection</s> TotalXsectionCGS
:math:`M_{p}`             g                           Not stored, use a constant value *1.6726215e-24*
:math:`C`                 :math:`m^{2}`/mb            <s>Not stored, use a constant value '''1.0e-31'''</s> Stored unit has been changed you don't need conversion factor anymore (use 1.0).
========================  ========================    ==========================

