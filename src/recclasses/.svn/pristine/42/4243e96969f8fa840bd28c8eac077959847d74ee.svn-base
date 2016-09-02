.. _laputop_params:

I3LaputopParams
===============

I3LaputopParams stores the fit result of Laputop, using :ref:`parameter_storage` class as its foundation.

To get a deeper understanding of how I3LaputopParams works, we suggest the documentation of :ref:`parameter_storage` , then this document, and finally the `doxygen documentation <../../doxygen/recclasses/index.html>`_ (or the source code). A mini-tutorial follows.

Usage
-----

In C++
^^^^^^

.. code-block:: cpp

    #include <recclasses/I3LaputopParams.h>
    #include <recclasses/LaputopEnums.h>
    // see LaputopEnums.h for the available parameter names

    // useful typedef to make names shorter
    typedef Laputop::Parameter Par;

    // [...] get a frame from somewhere

    // get I3LaputopParams from frame
    I3LaputopParamsConstPtr params =
        frame->Get<I3LaputopParams>("LaputopParams");
    // ... also works if the frame contains an old I3TopLateralFitParams!

    // get fitted values (short prefix "Par::" only works 
    // with the typedef from above)
    const double lg_s125 = params->GetValue(Par::Log10_S125);
    const double beta = params->GetValue(Par::Beta);

    // get a fit error
    const double lg_s125_err =
        params->GetError(Par::Log10_S125);

    // get a covariance
    const double cov_lg_s125_beta =
        params->GetCovariance(Par::Log10_S125, Par::Beta);

    // for more things to retrieve, see the class documentation

In Python
^^^^^^^^^

.. code-block:: python

    from icecube.recclasses import I3LaputopParams, LaputopParameter as Par
    from icecube.icetray import I3Units

    # [...] get a frame from somewhere

    # get I3LaputopParams from frame
    params = I3LaputopParams.from_frame(frame, "Laputop")
    # ... also works if the frame contains an old I3TopLateralFitParams!

    # get values, errors, covariance
    lgS125_value = params.value(Par.Log10_S125)
    beta_error = params.error(Par.Beta)
    cov_lgS125_Beta = params.cov (Par.Log10_S125, Par.Beta)

    # other values (not parameters) accessed in usual way
    chi2_LDF = params.chi2_LDF

    # query LDF
    s_predicted = params.expected_signal(10 * I3Units.meter)

    # query curvature model (time difference to plane shower front)
    dt_predicted = params.expected_shower_front_delay(10 * I3Units.meter)

    # get energy estimate
    energy = params.Energy() # returns most up-to-date calibration for that data

    # you can also ask for a specific energy calibration
    from icecube.recclasses import LaputopEnergy as LE
    params.Energy(LE.IC73SpectrumPaperProton)  # get proton-calibrated energy
    params.Energy(LE.IC73SpectrumPaperIron)  # get iron-calibrated energy

    # for more things to retrieve, see the class documentation

Parameters and errors computed on the fly
-----------------------------------------

I3LaputopParams extends the functionality of the ParameterStorage by computing certain parameters on the fly, which can be obtained via a transformation form the original stored parameters. Currently, this is implemented for the parameters ``Theta`` and ``Phi``.

All this is transparently done behind the scenes, the user just calls ``I3LaputopParams::GetValue(...)`` and ``I3LaputopParams::GetCovariance(...)`` as usual.

Angular resolution
------------------

I3LaputopParams has an option to return the angular resolution ``I3LaputopParams::GetAngularResolution(...)``. We define it as the angular distance between the true direction and the reconstructed direction that contains 68 % of the reconstructed directions. It is computed from the reconstructed uncertainty of the shower direction, therefore it is only accurate if the reconstructed uncertainty is accurate.
