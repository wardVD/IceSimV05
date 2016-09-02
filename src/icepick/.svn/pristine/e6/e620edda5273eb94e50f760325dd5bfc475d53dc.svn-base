"""
Python implementations of IcePicks.
"""

from icecube import icetray, dataclasses

#
# The I3NChFilter example gives a general idea of a useful pattern:
#
# - Define and return a function that incorporates the
#   additional arguments necessary for the picker logic
# - Provide a docstring for the factory function, so
#   users know what the parameters mean
# - Provide a docstring for the returned picker function,
#   including the frozen values of the additional arugments
# - Wrap the returned function in a pypick so that it can
#   be combined with other pickers using bitwise operators

def I3NChFilter(Key, NCh=200):
	"""
	Cut on the number of OMKeys.
	
	:param Key: The name of a frame object.
	:param Nch: The number of keys the object must have to be accepted.
	"""
	def I3NChFilter(frame):
		if Key in frame:
			obj = frame[Key]
			if hasattr(obj, "apply"):
				obj = obj.apply(frame)
			print("I3NChFilter returning : ", len(obj) >= NCh)
			print("  NCh = ", len(obj))
			print("  Cut Val = ", NCh)
			return len(obj) >= NCh		
		else:
			return False
	I3NChFilter.__doc__ = "'%s' has more than %d keys" % (Key, NCh)
	return icetray.pypick(I3NChFilter)

def I3ChargeFilter(Key, Charge=50):
	"""
	Cut on the number of PEs.
	
	:param Key: The name of a frame object.
	:param Charge: The minimum charge sum the frame object must have to be accepted.
	"""
	def I3ChargeFilter(frame):
		if Key in frame:
			obj = frame[Key]
			# this is needed for I3OMKeyMaskMap objects
			if hasattr(obj, "apply"):
				obj = obj.apply(frame)
			print("I3ChargeFilter returning : ", sum([ sum([h.charge for h in v]) for k,v in obj] ) >= Charge)
			print("  Charge = ", sum([ sum([h.charge for h in v]) for k,v in obj] )) 
			print("  Cut Val = ", Charge)
			return sum([ sum([h.charge for h in v]) for k,v in obj] ) >= Charge

		else:
			return False
	I3ChargeFilter.__doc__ = "'%s' has more than %d PEs" % (Key, Charge)
	return icetray.pypick(I3ChargeFilter)

