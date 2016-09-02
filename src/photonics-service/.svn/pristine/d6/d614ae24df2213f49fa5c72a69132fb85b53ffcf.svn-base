from icecube import icetray,photonics_service,phys_services
import numpy

def gen_pandel(distance=50, overdispersion=1.1): 
    """ Pandel model for time of arrival for photons"""
    from scipy.stats import gamma
    lambda_ = 71.; # meter
    tau = 671.; # ns
    x0 = 154.; # meter
    c = 0.2998; # meter/ns
    n = 1.34;
    gamma_overdispersion = 1.1
    gamma_scale = overdispersion/(1./tau + c/n/x0);
    gamma_shape = distance/(overdispersion*lambda_);
    return gamma(gamma_shape, scale=gamma_scale)

def sampler_demo(pxs):
    """Plot samples from an EM cascade light source."""
    import pylab as p
    
    rand = phys_services.I3GSLRandomService(1337) # creating random number generator

    pxs.SelectModuleCoordinates(0, 50, 0)  # First step for photonics services - select position of OM

    ps = photonics_service.PhotonicsSource(x=0, y=0, z=0, 
                                          zenith=90, azimuth=0, 
                                          speed =1., length=0, energy=1e5, type=1) ### Creating photonics source, type = 1 for cascade and =0 fo tracks
    meanpe, distance, geotime = pxs.SelectSource(ps) # Getting the expectation for the OM position selected before from source 
                                                     # returns expectation (in photo-electrons), distance to emmision point and time to travel this time 

    num_pe = rand.poisson(meanpe) ## Getting random number of photonos for expectation

    dt = numpy.array(pxs.GetTimeDelays(rand, num_pe)) ## Generates delay num_pe delay times (for a given OM position and given source)

    fig = p.figure(figsize=(8,6)) ## Figure for the distribution plots
    fig.subplots_adjust(bottom=0.12, left=0.25)

    edges = numpy.linspace(0, 1000, 1001) # Time edges 

    quantiles = pxs.GetProbabilityQuantiles(edges, 0, 0) ## Reading charge distribution (normalized to 1) 
    centers = 0.5*(edges[1:]+edges[:-1]) 

    p.plot(centers, quantiles, color='k', ls='--', label='Mean') 
    
    pandel = gen_pandel(distance) ## Comparing to pandel function
    prop_quantiles =  (pandel.cdf(edges[1:])-pandel.cdf(edges[:-1])) ## Reading Pandel distribution (not part of photonics
    p.plot(centers, prop_quantiles, color='k', ls=':', label='Proposal')
    
    bins, edges, patches = p.hist(dt, bins=700, range=(0, 7000), histtype='step',
        color='k', normed=True, linestyle='solid', label='Samples')
    
    p.xlabel('Delay time [ns]')
    p.ylabel('dP/dt')
    p.xlim((0,1000))
    
    p.legend(prop=dict(size='medium'))
    
def gradient_demo(pxs):
    """Plot mean amplitudes (and their gradients) EM cascade light source."""
    
    t_edges, az_edges, amps, grads = azi_scan(pxs, t_edges = numpy.linspace(0, 300, 51),
        az_edges = numpy.linspace(-180, 180, 51), omx = -50) ### See azi_scan() for comments

    import pylab as p
    from icecube.icetray import I3Units

    mid = lambda t: 0.5*(t[1:]+t[:-1])
    T, A = numpy.meshgrid(mid(t_edges), mid(az_edges))

    fig = p.figure(figsize=(12,6))
    fig.subplots_adjust(left=0.06, right=0.96)

    p.subplot(1,2,1)
    ax = p.gca()
    colorplot = ax.pcolorfast(az_edges, t_edges, amps.transpose())
    cb = p.colorbar(colorplot)
    cb.set_label('mean PE')
    p.xlabel('source-observer angle [deg]')
    p.ylabel('delay time [ns]')
    p.title('Mean amplitudes')

    p.subplot(1,2,2)
    p.quiver(A, T, grads[:,:,5]*I3Units.degree, grads[:,:,3])
    p.xlabel('source-observer angle [deg]')
    p.ylabel('delay time [ns]')
    p.title('Amplitude gradients')
    
def azi_scan(pxs, omx = -50, omy = 0, omz = 0,
    t_edges = numpy.linspace(0, 500, 101), az_edges = numpy.linspace(-180, 180, 101),
    zcenter = 0, gradient = True):

    with_numpy = hasattr(pxs, 'GetProbabilityQuantileGradients')

    if gradient and not with_numpy:
        raise ValueError("""You asked for gradients, but the PhotonicsService instance you passed
            (%s) doesn't appear to be able to return Numpy arrays.""" % (pxs))

    pxs.SelectModuleCoordinates(omx, omy, omz) # selecting modules coordinates
    t0 = 0
    t_centers = 0.5*(t_edges[1:]+t_edges[:-1])    
    az_centers = 0.5*(az_edges[1:]+az_edges[:-1])

    amp_shape = (az_centers.size,t_centers.size)

    zenith = 90.

    sources = [photonics_service.PhotonicsSource(0, 0, zcenter, zenith, azi, 1.0,
        0, 1e5, 1) for azi in az_centers] # creating photonics sources (rotating azimuth direction for cascades)

    amps = numpy.empty(amp_shape)
    if gradient:
        grads = numpy.empty(amp_shape + (7,))
    else:
        grads = None

    if with_numpy:
        te = t_edges
    else: # sometimes you have to bend over backwards.
        te = icetray.vector_double()
        te.extend(t_edges)

    for i,source in enumerate(sources):
        meanamp, distance, geotime = pxs.SelectSource(source) # Selecting source from sources (cascades with different directions)
        # Numpy-less versions of GetProbabilityQuantiles return a
        # wrapped vector<double> rather than a useful Numpy array
        quantiles = numpy.asarray(pxs.GetProbabilityQuantiles(te, t0, 0)) # Getting timing distribution, t0 should be added since 
        amps[i,:] = meanamp * quantiles ## Quantiles are normalized to 1, have to scale up
        if gradient:
            # amplitude gradients are given with respect to x, y, z, theta, phi, E
            # quantile gradients are given with respect to x, y, z, t, theta, phi
            amp_grad = numpy.zeros((1,1,7))
            temp = pxs.GetMeanAmplitudeGradient() # Amplitude gradients for given source and position
            amp_grad[:,:,:3] = temp[:3]
            amp_grad[:,:,4:] = temp[3:]
            quant_grads = numpy.zeros(quantiles.shape + (7,)) 
            temp = pxs.GetProbabilityQuantileGradients(t_edges, t0) # Reading time gradients
            quant_grads[:,:-1] = temp
            # drizzle lightly with Chain Rule sauce
            grads[i] = amp_grad*quantiles[:,numpy.newaxis] + meanamp*quant_grads

    return (t_edges, az_edges, amps, grads) # returning time and azimuth distribution of the charge + gradients

