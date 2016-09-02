#!/usr/bin/env python

"""
Verify normalization from :func:`from_simprod` against OneWeight
"""

from icecube.weighting import weighting
from icecube import icetray, dataio, dataclasses
import os, math
import urllib2
urlparse = urllib2.urlparse.urlparse
from numpy.testing import assert_array_almost_equal_nulp

def fetch_filename(dataset):
    """
    Get a random file from the given dataset.
    """
    mysql = weighting._import_mysql()

    db = mysql.connect(host='dbs4.icecube.wisc.edu', user='i3simprod-ro', passwd='Twed9~Dret', db='i3simprod')
    cursor = db.cursor()

    cursor.execute('SELECT path, name FROM urlpath WHERE dataset_id=%s AND size>0 LIMIT 1', (dataset, ))
    path, name = cursor.fetchone()

    return str(os.path.join(urlparse(path).path, name))

def get_random_filename(dataset):
    url = fetch_filename(dataset)
    if not os.path.exists(url):
        url = 'http://icecube:skua@convey.icecube.wisc.edu' + url

    icetray.logging.log_info('Got a file for dataset %d: %s' % (dataset, url))

    return url

def check_oneweight(dataset):
    generator = weighting.from_simprod(dataset)

    url = get_random_filename(dataset)
    try:
        if 'pnfs' in url:
            raise RuntimeError("Can't get %s from convey" % url)
        frame = dataio.I3File(url).pop_daq()
    except RuntimeError as e:
        icetray.logging.log_error(str(e))
        return
    if frame is None:
        icetray.logging.log_error('Could not read ' + url)
        return
    else:
        icetray.logging.log_info("Got " + url)

    nu = [p for p in frame['I3MCTree'].primaries if p.is_neutrino][0]
    icetray.logging.log_info(str(nu))
    wdict = frame['I3MCWeightDict']
    mine = wdict['TotalInteractionProbabilityWeight']/generator(nu.energy, nu.type, math.cos(nu.dir.zenith))
    # OneWeight is in units of cm^2, and must be normalized to the number of
    # neutrinos or antineutrinos ()
    theirs = wdict['OneWeight']/(1e4*wdict['NEvents']/2.)

    assert_array_almost_equal_nulp(mine, theirs, 4)

if __name__ == "__main__":

    test_sets = [11029, 11069, 11070, 11191, 11374, # upright-cylinder NuGen
                 10602, 10601, 11184, # legacy spherical nugen
                 ]

    from optparse import OptionParser
    parser = OptionParser(usage='%prog DATASET_IDS', description=__doc__)
    opts, args = parser.parse_args()

    if len(args) > 0:
        test_sets = map(int, args)

    try:
        for dataset in test_sets:
            check_oneweight(dataset)
    except ImportError:
        print("This test is disabled temporarily until mysql is installed on the bots.")
        print("To follow the progress check out : http://code.icecube.wisc.edu/projects/icecube/ticket/1295")
