/**
    copyright  (C) 2004
    the icecube collaboration
    $Id: I3TreeUtilsTest.cxx 98646 2013-02-07 21:32:34Z nwhitehorn $

    @version $Revision: 98646 $
    @date $Date: 2013-02-07 15:32:34 -0600 (Thu, 07 Feb 2013) $
    @author dule
*/
#include <dataclasses/I3TreeUtils.h>
#include <vector>
#include <algorithm>
#include <sstream>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <I3Test.h>

using namespace std;

// This string identifies the test suite in the list of test suites.
TEST_GROUP(I3TreeUtilsTest);

class int_t
{
public:
  uint64_t id;
  int GetMinorID() const { return id; }
  uint64_t GetMajorID() const { return id; }
  int_t(uint64_t i) : id(i) { }
};

TEST(a_insert_and_append)
{
  I3Tree<int_t> t;
  
  ENSURE(t.empty());
  ENSURE_EQUAL(0, t.size());
  ENSURE(!t.is_valid(t.begin()));
  ENSURE(t.begin() == t.end());

  int_t one(1);
  I3TreeUtils::AddTopLevel<int_t>(t, one);

  ENSURE(!t.empty());
  ENSURE_EQUAL(1, t.size());
  ENSURE(t.begin() != t.end());
  I3Tree<int_t>::iterator iter1 = t.begin();
  ENSURE(t.begin() == iter1);
  ENSURE(t.is_valid(iter1));
  ENSURE_EQUAL(0, t.depth(iter1));
  ENSURE_EQUAL(1, iter1->GetMinorID());
  ENSURE_EQUAL(0u, iter1.number_of_children());
  ENSURE_EQUAL(0u, t.number_of_siblings(iter1));
  
  int_t two(2);
  I3TreeUtils::AppendChild<int_t>(t, one, two);

  ENSURE_EQUAL(2, t.size());
  ENSURE_EQUAL(1u, t.begin().number_of_children());
  iter1++;
  ENSURE(t.is_valid(iter1));
  ENSURE_EQUAL(1, t.depth(iter1));
  ENSURE_EQUAL(2, iter1->GetMinorID());
  ENSURE_EQUAL(0u, iter1.number_of_children());
  ENSURE_EQUAL(0u, t.number_of_siblings(iter1));
  ENSURE_EQUAL(2, (t.child(t.begin(), 0))->GetMinorID());
  
  int_t three(3);
  I3TreeUtils::AppendChild<int_t>(t, one, three);

  ENSURE_EQUAL(3, t.size());
  ENSURE_EQUAL(2u, t.begin().number_of_children());
  iter1++;
  ENSURE(t.is_valid(iter1));
  ENSURE_EQUAL(1, t.depth(iter1));
  ENSURE_EQUAL(3, iter1->GetMinorID());
  ENSURE_EQUAL(0u, iter1.number_of_children());
  ENSURE_EQUAL(0u, t.number_of_siblings(iter1));
  iter1--;
  ENSURE_EQUAL(1u, t.number_of_siblings(iter1));
  ENSURE_EQUAL(3, (t.child(t.begin(), 1))->GetMinorID());
}

namespace UtilsTest
{
  void FillTree(I3Tree<int_t>& t)
  {
    int_t one(1);
    int_t two(2);
    int_t three(3);
    int_t four(4);
    int_t five(5);
    I3TreeUtils::AddTopLevel<int_t>(t, one);
    I3TreeUtils::AppendChild<int_t>(t, one, two);
    I3TreeUtils::AppendChild<int_t>(t, one, three);
    I3TreeUtils::AppendChild<int_t>(t, two, four);
    I3TreeUtils::AppendChild<int_t>(t, two, five);
    int_t eleven(11);
    int_t twelve(12);
    I3TreeUtils::AddTopLevel<int_t>(t, eleven);
    I3TreeUtils::AppendChild<int_t>(t, eleven, twelve);
  }
}

TEST(b_get_object)
{
  I3Tree<int_t> t;
  UtilsTest::FillTree(t);

  int_t i = I3TreeUtils::GetObject<int_t>(t,5,5);
  ENSURE_EQUAL(i.GetMinorID(),5);

  i = I3TreeUtils::GetObject<int_t>(t,2,2);
  ENSURE_EQUAL(i.GetMinorID(),2);
}

TEST(c_get_toplevel)
{
  I3Tree<int_t> t;
  UtilsTest::FillTree(t);

  vector<int_t> v = I3TreeUtils::GetTopLevelList<int_t>(t);
  ENSURE((int)v.size() > 0);
  ENSURE_EQUAL(v[0].GetMinorID(), 1);
}

TEST(d_get_daughters)
{
  I3Tree<int_t> t;
  UtilsTest::FillTree(t);

  vector<int_t> v = I3TreeUtils::GetTopLevelList<int_t>(t);
  ENSURE((int)v.size() > 0);

  vector<int_t> v1 = I3TreeUtils::GetDaughters<int_t>(t,v[0]);
  ENSURE_EQUAL((int)v1.size(), 2);
  ENSURE_EQUAL(v1[0].GetMinorID(), 2);
  ENSURE_EQUAL(v1[1].GetMinorID(), 3);

  vector<int_t> v2 = I3TreeUtils::GetDaughters<int_t>(t,v1[0]);
  ENSURE_EQUAL((int)v2.size(), 2);
  ENSURE_EQUAL(v2[0].GetMinorID(), 4);
  ENSURE_EQUAL(v2[1].GetMinorID(), 5);
}

TEST(e_is_toplevel)
{
  I3Tree<int_t> t;
  UtilsTest::FillTree(t);

  int_t i1(1);
  ENSURE(I3TreeUtils::IsTopLevel<int_t>(t,i1));

  int_t i2(2);
  ENSURE(!I3TreeUtils::IsTopLevel<int_t>(t,i2));
}

TEST(f_has_parent)
{
  I3Tree<int_t> t;
  UtilsTest::FillTree(t);

  int_t i1(1);
  ENSURE(!(I3TreeUtils::HasParent<int_t>(t,i1)));
  int_t i2(2);
  ENSURE(I3TreeUtils::HasParent<int_t>(t,i2));
  int_t i3(3);
  ENSURE(I3TreeUtils::HasParent<int_t>(t,i3));
  int_t i4(4);
  ENSURE(I3TreeUtils::HasParent<int_t>(t,i4));
  int_t i5(5);
  ENSURE(I3TreeUtils::HasParent<int_t>(t,i5));

  int_t i11(11);
  ENSURE(!I3TreeUtils::HasParent<int_t>(t,i11));
  int_t i12(12);
  ENSURE(I3TreeUtils::HasParent<int_t>(t,i12));
}

TEST(g_get_parent)
{
  I3Tree<int_t> t;
  UtilsTest::FillTree(t);

  int_t i1(3);
  ENSURE_EQUAL(I3TreeUtils::GetParent<int_t>(t,i1).GetMinorID(), 1);
  int_t i2(5);
  ENSURE_EQUAL(I3TreeUtils::GetParent<int_t>(t,i2).GetMinorID(), 2);

  int_t i3(12);
  ENSURE_EQUAL(I3TreeUtils::GetParent<int_t>(t,i3).GetMinorID(), 11);
}

TEST(h_get_nonexistant_parent)
{
  I3Tree<int_t> t;
  UtilsTest::FillTree(t);

  int_t i1(1);
  try {
    ENSURE_EQUAL(I3TreeUtils::GetParent<int_t>(t,i1).GetMinorID(), 0);
    FAIL("getting parent of toplevel object should have called log_fatal.");
  }
  catch(const std::exception &e){  }

  int_t i2(20);
  try {
    ENSURE_EQUAL(I3TreeUtils::GetParent<int_t>(t,i2).GetMinorID(), 0);
    FAIL("getting parent of object which is not in the tree should have "
	 "called log_fatal.");
  }
  catch(const std::exception &e){  }

}
