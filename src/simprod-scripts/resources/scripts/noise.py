#!/bin/env python
import logging
logging.basicConfig()
rootLogger = logging.getLogger('')
rootLogger.setLevel(logging.INFO)

from icecube.simprod.modules import Noise

if __name__ == '__main__':
   stats = {}
   det = Noise()
   det.ExecuteOpts(stats)
   print stats
