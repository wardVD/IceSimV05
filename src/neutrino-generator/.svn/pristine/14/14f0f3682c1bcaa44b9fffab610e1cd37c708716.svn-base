
Cross section
--------------

To set cross section, set name of cross section to I3NuGInteractionInfo. 
::

 tray.AddService("I3NuGInteractionInfoFactory", "interaction",
                  RandomService = random,
                  SteeringName = "steering",
                  CrossSectionModel = "csms"
               )

The available crosssections are 

* csms (recommended)
* cteq5 (old cross section)
* css (not recommended)


Note about Iso-scalar target
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you use I3NuGInteractionInfo, the total cross section of neutrino doesn't change whatever the target material is.

This is not correct, because ice (:math:`H_{2}O`) has more protons than neutron, while rock (:math:`SiO_{2}`) has same amount of protons and neutrons.</font>

I3NuGInteractionInfo service uses cross section table that is based on ''iso-scalar parton model'', a hypothetical particle that composed from 

.. math::
 \frac{1}{2} ~ proton + \frac{1}{2} ~ neutron

This approximation works well for :math:`SiO_{2}` but not for :math:`H_{2}O` for low energy particle.
The difference of cross section will be around 4% at low energy, and will be negligible at high energy regime.

See following documents for details.

* A. Heijboer, ''Track reconstruction and point source searches with ANTARES'', PhD thesis, 2004, p39. ([http://cdsweb.cern.ch/record/798707 pdf])
* T. Montaruli and I. Sokalski "Influence of neutrino interaction and muon propagation media on neutrino-induced muln rates in deep underwater detectors'', internal report of ANTARE'', 2003, p5. ([http://www.icecube.wisc.edu/~hoshina/docs/sea.pdf pdf])

For full-support of different cross sections between ice and rock, we need code update of NuGen.




