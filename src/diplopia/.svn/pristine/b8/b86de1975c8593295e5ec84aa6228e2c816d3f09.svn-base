PoissonMerger
=============

To inject background cosmic-rays into a signal stream, you must first configure an instance of a :cpp:class:`I3GeneratorService` 
and pass it to the I3Module PoissonMerger.  For example, to
pregenerated .i3 files (generated for example by corsika-reader)::

    from icecube.icetray import I3Units
    from icecube.diplopia import CoicidentI3ReaderService

    # Read events from file ""BACKGROUNDFILE.i3"
    background = diplopia.CoincidentI3ReaderService()
    background.open("BACKGROUNDFILE.i3")

    tray.AddModule("PoissonMerger","merge",
       BGWeights = "CorsikaWeightMap",
       CoincidentEventService = background,
     )

