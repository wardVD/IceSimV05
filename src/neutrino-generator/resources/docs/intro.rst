
Introduction
------------

The neutrino-generator(NuGen) is originally coded by Aya Ishihara, as an IceTray version of ANIS simulator.
The physics implemented in this program is based on the ANIS-All Neutrino Interaction Generator-(`ANIS: High energy neutrino generator for neutrino telescopes <http://www.sciencedirect.com/science/article/pii/S0010465505004194>`). 

What neutrino-generator does 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The basic function of original NuGen provided following functions.

#. Prepare a primary neutrino and inject it to the Earth
#. Propagate the neutrino and make interactions inside the Earth (if it happens)
#. If any neutrino reaches to the detector site (we call it as Detection Volume), make force interaction inside the detection volume
#. Store injected neutrino and all generated secondaries to I3MCTree
#. Store interaction weight information to the I3Map<string, double> 

Injected neutrinos are spacified with flavor. If NuMu flavor is selected, NuGen injects NuMu and NuMuBar with a ratio of (1:1) (ratio could be changed with the latest NuGen).

At propagation inside the Earth, NuGen makes interactions with matter. Following processes are supported.

#. Charged Current Interaction (CC) - makes charged leptons and showers(hadron, EM)
#. Neutral Current Interaction (NC) - makes one secondary neutrinoand a hadron shower
#. Grashow Resonance for NuEBar (GR) - makes hadron cascade, charged leptons and neutrinos depending on the branching ratio of W
#. Tau Decay - activated only when Tau or TauMinus are generated, the final state could be any leptons and hadron shower

As a result of an interaction, NuGen generates new secondary particles. If a secondary is a charged particle or a shower, this particle is absorbed by the Earth and will not be propagated anymore. In the case of CC interaction, all secondary particles are absorbed and thus the simulation stops at the vertex point. On the other hand, NC and GR interactions (may) create new secondary neutrinos. These neutrinos are propagated from the vertex point to IceCube detector.

If multiple neutrinos arrive at the detector, NuGen randomly selects one of them to make an interaction. **We ignore the case that two or more of them makes interaction inside the detector, because the probability is too small.** The selected neutrino **always** makes **one** interaction within a given detection volume. The supported interactions are:

#. Charged Current Interaction (CC)
#. Neutral Current Interaction (NC)
#. Grashow Resonance for NuEBar (GR)

The true probability of interaction is stored as *InteractionWeight* in **I3MCWeightDict**. 

Finally, the generated secondary particles as marked as *InIce* for ParticleLocation. If a **charged*** particle is *InIce*, this particle will be propagated by lepton propagator such as **MMC** or **PROPOSAL**.

All the particles generated inside the NuGen is stored in **I3MCTree**.

The core programs of this simulation are stored in *legacy* directory under the neutrino-generator project. 

What neutrino-generator does *not* do
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Following processes are not simulated with neutrino-generator.

#. Making cascades along a muon (or tau) --- cf. mmc, PROPOSAL
#. Making LPM effect of hadron cascades --- cf. cmc
#. Tau decay **at the final interaction volume** --- cf. mmc, PROPOSAL
#. If the first interaction inside the Detection Volume is NC, theout product neutrino doesn't interact within the detection volume again. Any descending programs does not do it too. This is not true, but NuGen ignore it because the probability that a neutrino makes two(or more) weak interactions inside the detection volume is negligibly small. 


New parameters for IceSim V4 or later
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The legacy NuGen was good enough for most of analysis, however, had an issue of statistics at high energy and high zenith angle for analysis that requires precisions of simulation.

The original NuGen stops simulation when all secondary particles are absorbed. For example, a neutrino with primary energy 1PeV from zenith angle 180 deg will not arrive at the detector, because the column depth of the Earth is much thicker than the mean free path of the 1PeV neutrino. As a result, the simulation always have statistical fluctuation at the high-energy tail.

As a simple solution of the problem, the newer NuGen has a function to switch off the CC interaction during the propagation inside the Earth. This switch is called **PropagationWeightMode** in I3NeutrinoGenerator module. This option is applicable for NuMu and NuE simulation. We should not switch off CC interaction for NuTau simulation, because NuTau - CC interaction may regenerate new neutrinos via decay process of Tau and Muon. By default, it is set to *AutoDetect* parameter and that switch off the CC interaction for NuMu and NuE only.
As long as one uses OneWeight for weighting, this option does not affect to the final result while one can get better statistics at high energy tail.

The **PropagationWeightMode** helps to reduce the statistical issue a lot, however, still it's not enough for some analysis: because the arrival energy of neutrino at detector is always less than the primary neutrinos. At the edge of high energy tail in arrival energy, the statistics is still poor.
The only way to solve the issue is inject neutrino at the detector and weight propagation weights later. This option is called "SimMode" parameter in **I3NuGSteering** class. If the parameter is **FINALONLY**, NuGen injects primary neutrino at detector (This option exists from the beginning as a switch of I3NeutrinoGenerator).

In order to weight the **FINALONLY** simulation, one must calculate propagation weight for each events and estimate its primary energy. A simple example is provided in neutrino-generator/resources/examples/ApplyPropWeight.py. 

See `this wiki page <https://wiki.icecube.wisc.edu/index.php/Neutrino_Generator/Starting_Simulation_at_Detector_%28FinalOnly_mode%29>` for details about **FINALONLY** simulation.

