from .. import ipmodule


class MuonGunGenerator(ipmodule.ParsingModule):
    """IceProd module for ``MuonGun`` simulations

    It defines parameters for and executes
    :func:`icecube.simprod.segments.GenerateCosmicRayMuons` and
    :func:`icecube.simprod.segments.PropagateMuons`.

    **Module parameters:**

        :attr:`nproc`
            Number of processes for RNG
        :attr:`pronum`
            Process number
        :attr:`seed`
            RNG seed
        :attr:`gcdfile`
            GeoCalibDetStatus filename
        :attr:`nevents`
            Number of generated events
        :attr:`model`
            Primary cosmic-ray flux parametrization
        :attr:`gamma`
            Power law spectral index
        :attr:`offset`
            Power law offset in GeV
        :attr:`emin`
            Mininum generated energy in GeV
        :attr:`emax`
            Maximum generated energy in GeV
        :attr:`length`
            Cylinder length in m
        :attr:`radius`
            Cylinder radius in m
        :attr:`x`
            Cylinder x-position in m
        :attr:`y`
            Cylinder y-position in m
        :attr:`z`
            Cylinder z-position in m
        :attr:`length_dc`
            Inner cylinder length in m
        :attr:`radius_dc`
            Inner cylinder radius in m
        :attr:`x_dc`
            Inner cylinder x-position in m
        :attr:`y_dc`
            Inner cylinder y-position in m
        :attr:`z_dc`
            Inner cylinder z-position in m
        :attr:`deepcore`
            Use inner cylinder
        :attr:`outputfile`
            Output filename
        :attr:`summaryfile`
            XMLSummary filename

    """
    def __init__(self):
        ipmodule.ParsingModule.__init__(self)

        self.AddParameter("nproc", "number of processes for RNG", 1)
        self.AddParameter("procnum", "process number", 0)
        self.AddParameter("seed", "RNG seed", 1)

        self.AddParameter("gcdfile", "GeoCalibDetStatus filename", "")
        self.AddParameter("nevents", "number of generated events", 1)
        self.AddParameter("model", "primary cosmic-ray flux parametrization",
                          "Hoerandel5_atmod12_SIBYLL")
        self.AddParameter("gamma", "power law spectral index", 2.)
        self.AddParameter("offset", "power law offset in GeV", 700.)
        self.AddParameter("emin", "mininum generated energy in GeV", 1e4)
        self.AddParameter("emax", "maximum generated energy in GeV", 1e7)
        self.AddParameter("length", "cylinder length in m", 1600.)
        self.AddParameter("radius", "cylinder radius in m", 800.)
        self.AddParameter("x", "cylinder x-position in m", 0.)
        self.AddParameter("y", "cylinder y-position in m", 0.)
        self.AddParameter("z", "cylinder z-position in m", 0.)
        self.AddParameter("length_dc", "inner cylinder length in m", 500.)
        self.AddParameter("radius_dc", "inner cylinder radius in m", 150.)
        self.AddParameter("x_dc", "inner cylinder x-position in m", 46.3)
        self.AddParameter("y_dc", "inner cylinder y-position in m", -34.9)
        self.AddParameter("z_dc", "inner cylinder z-position in m", -300.)
        self.AddParameter("deepcore", "use inner cylinder", False)

        self.AddParameter("outputfile", "output filename", "corsika.i3")
        self.AddParameter("summaryfile", "XMLSummary filename", "summary.xml")

        self.AddParameter('HistogramFilename', 'Histogram filename.', None)

    def Execute(self, stats):
        if not ipmodule.ParsingModule.Execute(self, stats):
            return 0

        import icecube.icetray
        import icecube.dataclasses
        import icecube.dataio
        import icecube.phys_services
        from I3Tray import I3Tray

        from ..util import BasicCounter
        from ..segments import GenerateCosmicRayMuons, PropagateMuons

        # Instantiate a tray.
        tray = I3Tray()

        tray.AddService("I3XMLSummaryServiceFactory", "summary",
                        outputfilename=self.summaryfile)

        randomService = icecube.phys_services.I3SPRNGRandomService(
            self.seed, self.nproc, self.procnum)
        tray.context["I3RandomService"] = randomService

        tray.AddModule("I3InfiniteSource","TheSource",
                       Prefix=self.gcdfile,
                       Stream=icecube.icetray.I3Frame.DAQ)

        # Configure tray segment that actually does stuff.
        tray.AddSegment(GenerateCosmicRayMuons, "muongun",
                        mctree_name="I3MCTree_no_propagation",
                        num_events=self.nevents,
                        flux_model=self.model,
                        gamma_index=self.gamma,
                        energy_offset=self.offset,
                        energy_min=self.emin,
                        energy_max=self.emax,
                        cylinder_length=self.length,
                        cylinder_radius=self.radius,
                        cylinder_x=self.x,
                        cylinder_y=self.y,
                        cylinder_z=self.z,
                        inner_cylinder_length=self.length_dc,
                        inner_cylinder_radius=self.radius_dc,
                        inner_cylinder_x=self.x_dc,
                        inner_cylinder_y=self.y_dc,
                        inner_cylinder_z=self.z_dc,
                        use_inner_cylinder=self.deepcore)

        tray.AddSegment(PropagateMuons, "propagator",
                        RandomService=randomService,
                        CylinderLength=self.length,
                        CylinderRadius=self.radius,
                        InputMCTreeName="I3MCTree_no_propagation",
                        OutputMCTreeName="I3MCTree")

        tray.AddModule(BasicCounter, "count_events",
                       Streams=[icecube.icetray.I3Frame.DAQ],
                       name="Generated Events",
                       Stats=stats)


        if self.histogramfilename:         
            from icecube.production_histograms import ProductionHistogramModule
            from icecube.production_histograms.histogram_modules.simulation.mctree_primary import I3MCTreePrimaryModule
            from icecube.production_histograms.histogram_modules.simulation.mctree import I3MCTreeModule
        
            tray.AddModule(ProductionHistogramModule, 
                           Histograms = [I3MCTreePrimaryModule, I3MCTreeModule],
                           OutputFilename = self.histogramfilename)

        tray.AddModule("I3Writer", "writer",
                       filename=self.outputfile,
                       streams=[icecube.icetray.I3Frame.DAQ])

        tray.AddModule("TrashCan", "trashcan")

        # Execute the tray.
        tray.Execute()
        tray.Finish()

        del tray
        return 0
