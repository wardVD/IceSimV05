.. _parameter_storage:

ParameterStorage
================

The ParameterStorage is a general purpose mixin-style class which provides an elegant interface to store fit parameters, their errors and their correlations. Ideally, a reconstruction code which performs a fit should produces a meaningful covariance matrix as well. The covariance matrix contains useful information and should be stored alongside the fit result. With the ParameterStorage, it is easy to make such a class.

The class provides very fast access, space-efficient storage, backward- and forward-compatibility, and is cache-friendly.

I3LaputopParams is the first user of the ParameterStorage and serves as a prototype for others.

Usage (for developers)
----------------------

Creating a new storage class based on the ParameterStorage requires only two simple steps.

1) Define an enum which lists your fit parameters that you want to store.
2) Derive your class from the templated ParameterStorage class, with your enum and its size as the template arguments, and I3FrameObject.

That is all. The ParameterStorage implements its own serialization code and initializes into a default, space efficient empty state.

Forward- and Backward-compatibility
-----------------------------------

It is safe to define more fit parameters in the enum than are actually used. The ParameterStorage uses only space for parameters that are actually assigned values by the user. We chose such a design to make the storage forward- and backward-compatible. Fit parameters which are not used anymore simply stay in the enum, new fit parameters are added at the end of the enum. Old and new versions of the fit results can be stored with the same class without changing anything else.

Fast access and cache-friendliness
----------------------------------

The ParameterStorage is almost as fast as storing a value in vector, it requires only one additional jump in memory. Parameter access is much faster than for an ordered map or even a hash map. It is safe to use ParameterStorage even in a tight loop. Since the ParameterStorage internally consists of (small) segments of continuous memory, it is very cache-friendly, too.
