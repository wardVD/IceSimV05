#!/bin/env python
import logging
logging.basicConfig()
rootLogger = logging.getLogger('')
rootLogger.setLevel(logging.INFO)

from icecube.simprod.modules import TopAirShowerGeneartor

if __name__ == '__main__':
   stats = {}
   it = TopAirShowerGeneartor()
   it.ExecuteOpts(stats)
   print stats
