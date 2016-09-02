# -*- coding: utf-8 -*-
# 
# copyright  (C) 2010
# The Icecube Collaboration
# 
# $Id: pytables_ext.py 136484 2015-08-19 07:40:07Z jvansanten $
# 
# @version $Revision: 136484 $
# @date $LastChangedDate: 2015-08-19 03:40:07 -0400 (Wed, 19 Aug 2015) $
# @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy: jvansanten $
#

try:
    import tables,numpy
    have_pytables = True
except ImportError:
    have_pytables = False
    
if have_pytables:
    
    # Since we've extended the HDF5 high-level Table interface, we
    # have to provide our own convenience functions for PyTables
    def monkeypatch(target):
        def patcher(func):
            original = getattr(target, func.__name__)
            setattr(target, func.__name__, func)
            setattr(target, func.__name__ + '__unpatched', original)
            return func
        return patcher

    # Add access to units and docstrings at table level
    @monkeypatch(tables.Table)
    def _cacheDescriptionData(self):
        self._cacheDescriptionData__unpatched()
        ncols = len(self.description._v_names)
        # tables.attributeset.AttributeSet implements __getitem__, but not get(key,default)
        def get(item,key,default=None):
            try:
                return item[key]
            except KeyError:
                return default
        self.colunits = [get(self._v_attrs,'FIELD_%d_UNIT'%i,'') for i in range(ncols)]
        self.coldoc   = [get(self._v_attrs,'FIELD_%d_DOC'%i,'') for i in range(ncols)]
        ragged = get(self._v_attrs,'__I3RaggedTable__',None)
        if ragged is not None:
            self.ragged = (ragged[0] != 0)
        else:
            self.ragged = True # this is the more conservative assumption
        try:
            self.I3Index = self._v_file._getNode('/__I3Index__/%s' % self.name)
        except tables.NoSuchNodeError:
            self.I3Index = None
        
    def instancemethod(target):
        def adder(func):
            setattr(target,func.__name__,func)
            return func
        return adder
    
    @instancemethod(tables.Table)
    def readEvent(self,n):
        if self.I3Index is None:
            raise TypeError("This table does not appear to have an index.")
        limits = self.I3Index.read(n)
        if limits['start'] == limits['stop']:
            return numpy.empty(0,dtype=self.description._v_dtype)
        else:
            return self.read(limits['start'],limits['stop'])
    
        
