#!/usr/bin/env python

import sys

from icecube import icetray, dataio, dataclasses
from icecube.dataclasses import I3MCTree,I3Particle,I3ParticleID

from I3Tray import I3Units

# override ENSURE
class EnsureException(Exception):
  pass
def ENSURE(cond,msg):
  if not cond :
    raise EnsureException(msg)

def makeParticle():
  p = I3Particle();
  p.time = 0.0
  p.length = 0.0
  p.energy = 0.0
  p.pos = dataclasses.I3Position(0.0,0.0,0.0)
  p.dir = dataclasses.I3Direction(0.0,0.0)
  return p

class TreeTest:
  def constructors(self):
    t = I3MCTree()
    ENSURE( t.empty(), "tree is not empty" )
    
    p1 = makeParticle()
    t2 = I3MCTree(p1)
    ENSURE( not t2.empty(), "tree is empty" )
    ENSURE( t2.get_head() == p1, "head != p1" )
    ENSURE( t2.has(p1), "p1 is not in the tree")
        
    t3 = I3MCTree(t2)
    ENSURE( not t3.empty(), "t3 is empty" )
    ENSURE( t3.get_head() == p1, "copy constructor missing p1" )
  
  def insertion(self):
    head = makeParticle()
    t = I3MCTree()
    ENSURE( t.empty(), "tree is not empty" )
    
    t.insert(head)
    ENSURE( not t.empty(), "tree is empty after insertion" )
    ENSURE( t.get_head() == head, "head is not head" )
    
    p1 = makeParticle()
    t.insert_after(head,p1)
    ENSURE( t.next_sibling(head) == p1, "p1 is not the sibling of head" )
    
    p2 = makeParticle()
    t.insert(p1,p2)
    ENSURE( t.next_sibling(head) == p2, "p2 is not the next sibling of head" )
    ENSURE( t.next_sibling(p2) == p1, "p1 is not the next sibling of p2" )
    
    p3 = makeParticle()
    t.insert_after(p3)
    ENSURE( t.next_sibling(p1) == p3, "p3 is not inserted as the last primary" )
    
    p4 = makeParticle()
    t.insert_after(p2,p4)
    ENSURE( t.next_sibling(p2) == p4, "p4 is not the next sibling of p2" )
    
    head2 = makeParticle()
    t2 = I3MCTree(head2)
    t2.append_child(head2,makeParticle())
    t.insert_subtree(p3,t2,head2)
    ENSURE( t.next_sibling(p1) == head2, "head2 missing" )
    ENSURE( t.children(head2), "children of subtree2 not copied" )
    
    head3 = makeParticle()
    t3 = I3MCTree(head3)
    t3.append_child(head3,makeParticle())
    t.insert_subtree_after(head2,t3,head3)
    ENSURE( t.next_sibling(head2) == head3, "head3 missing" )
    ENSURE( t.children(head3), "children of subtree3 not copied" )
  
  def appending(self):
    head = makeParticle()
    t = I3MCTree(head)
    p1 = makeParticle()
    t.append_child(head,p1)
    ENSURE( t.first_child(head) == p1, "p1 is not first child" )
    
    p2 = makeParticle()
    t.append_child(head,p2)
    ENSURE( t.first_child(head) == p1, "p1 is not still first child" )
    ENSURE( t.next_sibling(p1) == p2, "p2 is not sibling of p1" )
    
    new_children = [makeParticle() for x in range(10)]
    t.append_children(p1,new_children)
    ENSURE( t.first_child(p1) == new_children[0], "p1 first child is wrong" )
    ENSURE( t.children(p1) == new_children, "children are not equal" )
    
    head2 = makeParticle()
    t2 = I3MCTree(head2)
    t2.append_child(head2,makeParticle())
    t.append_child(p2,t2,head2)
    ENSURE( t.first_child(p2) == head2, "head2 missing" )
    ENSURE( t.children(head2), "children of subtree2 not copied" )
  
  def erase(self):
    t = I3MCTree(makeParticle())
    ENSURE( not t.empty(), "t is empty" )
    t.clear()
    ENSURE( t.empty(), "t is not empty" )
    
    head = makeParticle()
    t2 = I3MCTree(head)
    p1 = makeParticle()
    t2.append_child(head,p1)
    t2.append_child(p1,makeParticle())
    t2.erase(p1)
    ENSURE( not t2.children(head), "children remain" )
    
    p2 = makeParticle()
    t2.append_child(head,p2)
    p3 = makeParticle()
    t2.append_child(p2,p3)
    t2.append_child(p3,makeParticle())
    t2.append_child(p3,makeParticle())
    t2.append_child(p3,makeParticle())
    t2.erase_children(p2)
    ENSURE( t2.at(p2), "p2 disappeared" )
    ENSURE( not t2.children(p2), "p2 still has children" )
  
  def replace(self):
    head = makeParticle()
    t = I3MCTree(head)
    p1 = makeParticle()
    t.append_child(head,p1)
    p2 = makeParticle()
    t.replace(p1,p2)
    ENSURE( t.at(p2), "p2 not in tree" )
    ENSURE( t.first_child(head) == p2, "p2 not in right place" )
    
    head2 = makeParticle()
    t2 = I3MCTree(head2)
    p3 = makeParticle()
    t2.append_child(head2,p3)
    t2.replace(p3,t,head)
    ENSURE( t2.first_child(head2) == head, "p3 still present" )
    ENSURE( t2.children(head) == [p2], "p2 not in t2" )
  
  def flatten(self):
    head = makeParticle()
    t = I3MCTree(head)
    p1 = makeParticle()
    t.append_child(head,p1)
    new_children = [makeParticle() for x in range(10)]
    t.append_children(p1,new_children)
    t.flatten(p1)
    ENSURE( not t.children(p1), "p1 still has children" )
    ENSURE( t.children(head) == [p1]+new_children,
           "p1 has siblings in the right order" )
  
  def reparent(self):
    head = makeParticle()
    t = I3MCTree(head)
    p1 = makeParticle()
    t.append_child(head,p1)
    p2 = makeParticle()
    t.append_child(head,p2)
    p1_children = [makeParticle() for x in range(10)]
    t.append_children(p1,p1_children)
    t.reparent(p2,p1)
    ENSURE( not t.children(p1), "p1 still has children" )
    ENSURE( t.children(p2) == p1_children, "p2 does not have the children" )
  
  def merge(self):
    head = makeParticle()
    t = I3MCTree(head)
    p1 = makeParticle()
    t.append_child(head,p1)
    
    head2 = makeParticle()
    t2 = I3MCTree(head2)
    p2 = makeParticle()
    t2.append_child(head2,p2)
    
    t.merge(t2)
    ENSURE( t.get_head() == head, "head missing" )
    ENSURE( t.next_sibling(head) == head2, "head2 missing" )
    ENSURE( t.children(head) == [p1], "p1 missing" )
    ENSURE( t.children(head2) == [p2], "p2 missing" )
  
  def swap(self):
    head = makeParticle()
    t = I3MCTree(head)
    p1 = makeParticle()
    t.append_child(head,p1)
    
    head2 = makeParticle()
    t2 = I3MCTree(head2)
    p2 = makeParticle()
    t2.append_child(head2,p2)
    
    t.swap(t2)
    ENSURE( t.get_head() == head2, "head2 missing" )
    ENSURE( t2.get_head() == head, "head missing" )
    ENSURE( t.children(head2) == [p2], "p2 missing" )
    ENSURE( t2.children(head) == [p1], "p1 missing" )
  
  def iterator(self):
    head = makeParticle()
    t = I3MCTree(head)
    p1 = makeParticle()
    t.append_child(head,p1)
    p1_children = [makeParticle() for x in range(3)]
    t.append_children(p1,p1_children)
    p2_children = [makeParticle() for x in range(3)]
    t.append_children(p1_children[1],p2_children)
    
    # tree should look like:
    #       head
    #         |
    #         V
    #         p1
    #       / | \
    #      /  V  \
    #  p1c1 p1c2  p1c3
    #         |
    #         V
    #       / | \
    #      /  V  \
    #  p2c1 p2c2  p2c3
    
    pre_order = [head,p1,p1_children[0],p1_children[1],p2_children[0],
                 p2_children[1],p2_children[2],p1_children[2]]
    post_order = [p1_children[0],p2_children[0],p2_children[1],
                  p2_children[2],p1_children[1],p1_children[2],p1,head]
    leaf_iter = [p1_children[0],p2_children[0],p2_children[1],
                 p2_children[2],p1_children[2]]
    
    ENSURE( list(t) == pre_order, "default iter failed")
    ENSURE( list(t.pre_order_iter()) == pre_order, "pre_order failed" )
    ENSURE( list(t.post_order_iter()) == post_order, "post_order failed" )
    ENSURE( list(t.sibling_iter(p1_children[0])) == p1_children,
           "sibling_iter failed" )
    fast_iter_ret = set([x.minor_id for x in t.fast_iter()])
    fast_iter_ret ^= set([x.minor_id for x in pre_order])
    ENSURE( not fast_iter_ret , "fast_iter failed" )
    leaf_iter_ret = set([x.minor_id for x in t.leaf_iter()])
    leaf_iter_ret ^= set([x.minor_id for x in leaf_iter])
    ENSURE( not leaf_iter_ret , "leaf_iter failed" )

def printer(fn,value):
  s = fn+':'
  if len(s) < 60:
    s += (' '*(60-len(s)))
  if value:
    print(s+'OK')
  else:
    print(s+'FAILED')

if __name__ == '__main__':
  tree = TreeTest()
  failed = False
  for t in dir(tree):
    if t[0] != '_':
      tt = getattr(tree,t)
      if callable(tt):
        try:
          tt()
        except EnsureException,e:
          printer(t,False)
          print('\t'+str(e))
          failed = True
        else:
          printer(t,True)
  if failed:
    sys.exit(1)
