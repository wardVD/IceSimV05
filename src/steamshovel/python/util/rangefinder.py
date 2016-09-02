from icecube import icetray, dataclasses
import operator


def _i3map_values_iter(i3map, e1, e2):
    for dom, values in i3map:
        for v in values:
            yield (e1(v), e2(v))


def _particleTimeRange(obj):
    v = obj.speed
    t0 = obj.time
    x0 = obj.pos.x
    y0 = obj.pos.y
    z0 = obj.pos.z
    dx = obj.dir.x
    dy = obj.dir.y
    dz = obj.dir.z
    # particle track position function as function of time
    fx = lambda t: v * dx * (t - t0) + x0
    fy = lambda t: v * dx * (t - t0) + y0
    fz = lambda t: v * dx * (t - t0) + z0
    # define 1km**3 cube around IceCube and calculate
    # punch-through times of the particle track.
    d_IC = 1000.
    t_x = sorted([(d_IC - x0) / v / dx + t0,
                  (-1 * d_IC - x0) / v / dx + t0])
    t_y = sorted([(d_IC - y0) / v / dy + t0,
                  (-1 * d_IC - y0) / v / dy + t0])
    t_z = sorted([(d_IC - z0) / v / dz + t0,
                  (-1 * d_IC - z0) / v / dz + t0])
    # choose start and end times to minimize time window length
    start = max([t_x[0], t_y[0], t_z[0]])
    end = min([t_x[1], t_y[1], t_z[1]])

    return start, end


def getTimeRange(frame, key):
    '''
    Return the visible time range of frame[key], if one can be extracted.

    This is the backend of the python RangeFinder in TimelineWidget.  It returns a
    tuple of ints (start,end) incating the beginning and end of the given event
    according to the arbitrary frame object frame[key].  In general this is meant to
    detect the range of a RecoPulseSeriesMap or similar object.

    If no range can be gathered, this function returns None or throws an exception.
    Any python exception may be thrown; this simply indicates the key cannot be used
    for the given frame.

    Exotic types from unusual projects may be added here as well, but this module should
    not throw an exception when imported, so projects that aren't in offline-software
    should be loaded in a try block.
    '''

    obj = frame[key]

    # Handle I3EventHeader
    if isinstance(obj, dataclasses.I3EventHeader):
        return 0, obj.end_time - obj.start_time

    if isinstance(obj, dataclasses.I3TimeWindow):
        return obj.start, obj.stop

    # Handle I3Particle time-related position
    if isinstance(obj, dataclasses.I3Particle):
        return _particleTimeRange(obj)

    # Handle maps of time-related data
    if not hasattr(obj, 'values'):
        # perhaps it's a mask
        obj = dataclasses.I3RecoPulseSeriesMap.from_frame(frame, key)

    vals = obj.values()
    if len(vals) > 0:
        start = float('inf')
        end = 0
        # If the frame object has some values(), then find (min, max)
        # where max is either the maximum value.time, or the maximum
        # value[i].time for all i.  Similarly for min, mutatis mutandis.
        for x in vals:
            try:
                start = min(start, x.time)
                end = max(end, x.time)
            except AttributeError:
                for y in x:
                    start = min(start, y.time)
                    end = max(end, y.time)
        if start == float('inf'):
            start = 0
        duration = max(1, end - start) # make sure duration is always valid
        margin = 0.1
        return start - margin * duration, end + margin * duration


def getRangeHistogram(frame, key, timeRange):
    '''
    Given a frame, a key, and a tuple (start,end), attempt to create a histogram for the timeline widget.

    The returned histogram should be a dataclasses.I3VectorFloat with all values between 0 and 1.

    As with getTimeRange, this function may return None, or throw any exception, to indicate that
    producing a histogram with the given key is not possible.
    '''
    import numpy # numpy has to be available for this function to yield histograms

    obj = frame[key]

    # If a thing doesn't have a values attribute, we can't use it below;
    # but maybe it's a mask.  If not, this will throw and end the function.
    if not hasattr(obj, 'values'):
        obj = dataclasses.I3RecoPulseSeriesMap.from_frame(frame, key)

    divs = min(256, int(timeRange[1] - timeRange[0]))

    if type(obj) == dataclasses.I3RecoPulseSeriesMap:
        # histogram the pulse times, weighting by charge
        e1 = operator.attrgetter('time')
        e2 = operator.attrgetter('charge')
    elif type(obj) == dataclasses.I3DOMLaunchSeriesMap:
        # histogram the launch times, with equal weighting
        e1 = operator.attrgetter('time')
        e2 = lambda x: 1
    else:
        return None

    hist_data, hist_weights = zip(*_i3map_values_iter(obj, e1, e2))
    hist, _ = numpy.histogram(
        hist_data, bins=divs, range=timeRange, weights=hist_weights)
    # normalize with logarithmic scaling
    hist = numpy.log1p(hist) / numpy.log(sum(hist))
    return dataclasses.I3VectorFloat(hist)
