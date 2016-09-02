#!/bin/env python
import logging
logging.basicConfig()
rootLogger = logging.getLogger('')
rootLogger.setLevel(logging.INFO)

from icecube.simprod.modules import HybridPhotons

if __name__ == '__main__':
   stats = {}
   cors = HybridPhotons()
   cors.ExecuteOpts(stats)
   print stats
