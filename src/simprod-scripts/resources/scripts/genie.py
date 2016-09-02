#!/usr/bin/env python
import logging
logging.basicConfig()
rootLogger = logging.getLogger('')
rootLogger.setLevel(logging.INFO)

from icecube.simprod.modules import Genie

if __name__ == '__main__':
   stats = {}
   gen = Genie()
   gen.ExecuteOpts(stats)
   print stats
