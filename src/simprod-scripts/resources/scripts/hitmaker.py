#!/bin/env python
from icecube.simprod.modules import HitMakerIC

if __name__ == '__main__':
   stats = {}
   cors = HitMakerIC()
   cors.ExecuteOpts(stats)
   print stats
