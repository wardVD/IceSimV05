#!/usr/bin/env python
import logging
logging.basicConfig()
rootLogger = logging.getLogger('')
rootLogger.setLevel(logging.INFO)

from icecube.simprod.modules import MuonGunGenerator

if __name__ == '__main__':
   stats = {}
   mu = MuonGunGenerator()
   mu.ExecuteOpts(stats)
   print stats
