#!/bin/env python
import logging
logging.basicConfig()
rootLogger = logging.getLogger('')
rootLogger.setLevel(logging.INFO)

from icecube.simprod.modules import PPC

if __name__ == '__main__':
   stats = {}
   ppc  = PPC()
   ppc.ExecuteOpts(stats)
   print stats
