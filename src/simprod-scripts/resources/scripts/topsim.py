#!/bin/env python
import logging
logging.basicConfig()
rootLogger = logging.getLogger('')
rootLogger.setLevel(logging.INFO)

from icecube.simprod.modules import TopSimulator

if __name__ == '__main__':
   stats = {}
   cors = TopSimulator()
   cors.ExecuteOpts(stats)
   print stats
