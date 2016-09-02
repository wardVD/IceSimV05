
import sys
from icecube import icetray
if sys.version_info[0] >= 3:
	from functools import reduce

# Quietly.
icetray.load('WaveCalibrator', False)

def MergeTimeWindows(frame, Names, Output):
	"""
	OR the given I3TimeWindowSeriesMaps together.
	"""
	import operator
	windows = [frame[key] for key in Names if key in frame]
	if len(windows):
		frame[Output] = reduce(operator.or_, windows)
