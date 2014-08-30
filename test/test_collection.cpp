
#include "vhwd.h"

using namespace vhwd;


TEST_DEFINE(TEST_arr_1t)
{

	arr_1t<int> hh;
	TEST_ASSERT(hh.empty());
	TEST_ASSERT(hh.size()==0);

	hh.push_back(3);
	TEST_ASSERT(hh.size()==1);
	TEST_ASSERT(hh[0]==3);

	hh.insert(hh.begin(),2);
	TEST_ASSERT(hh.size()==2);
	TEST_ASSERT(hh[0]==2);
	TEST_ASSERT(hh[1]==3);

	hh.clear();
	TEST_ASSERT(hh.size()==0);
	//TEST_ASSERT_THROW_ANY(hh.back());
	//TEST_ASSERT_THROW_ANY(hh.front());
	//TEST_ASSERT_THROW_ANY(hh.pop_back());

	hh.shrink_to_fit();
	TEST_ASSERT(hh.capacity()==0);

	int a[3]= {1,2,3};

	hh.append(a,3);
	hh.insert(hh.begin()+1,a,2);

	wassert(hh[0]==1);
	wassert(hh[1]==1);
	wassert(hh[2]==2);
	wassert(hh[3]==2);
	wassert(hh.back()==3);
	hh.erase(hh.begin()+2,hh.end());
	wassert(hh.back()==1);
}

TEST_DEFINE(TEST_pod_1t)
{
	pod_1t<int> hh;
	TEST_ASSERT(hh.empty());
	TEST_ASSERT(hh.size()==0);

	hh.push_back(3);
	TEST_ASSERT(hh.size()==1);
	TEST_ASSERT(hh[0]==3);

	hh.insert(hh.begin(),2);
	TEST_ASSERT(hh.size()==2);
	TEST_ASSERT(hh[0]==2);
	TEST_ASSERT(hh[1]==3);

	hh.clear();
	TEST_ASSERT(hh.size()==0);
	//TEST_ASSERT_THROW_ANY(hh.back());
	//TEST_ASSERT_THROW_ANY(hh.front());
	//TEST_ASSERT_THROW_ANY(hh.pop_back());

	hh.shrink_to_fit();
	TEST_ASSERT(hh.capacity()==0);

	int a[3]= {1,2,3};

	hh.append(a,3);
	hh.insert(hh.begin()+1,a,2);

	wassert(hh[0]==1);
	wassert(hh[1]==1);
	wassert(hh[2]==2);
	wassert(hh[3]==2);
	wassert(hh.back()==3);
	hh.erase(hh.begin()+2,hh.end());
	wassert(hh.back()==1);


}

void test_bst_set()
{
	std::vector<int> aInts;
	for(int i=0; i<1024; ++i)
	{
		aInts.push_back(i);
	}
	std::random_shuffle(aInts.begin(),aInts.end());

	bst_set<int> s;
	s.insert(aInts.begin(),aInts.end());


	TEST_ASSERT(s.size()==1024);
	TEST_ASSERT(s.find(9)!=s.end());
	TEST_ASSERT(s.count(9)==1);
	s.erase(9);
	TEST_ASSERT(s.find(9)==s.end());
	TEST_ASSERT(s.count(9)==0);
	TEST_ASSERT(s.size()==1023);

	TEST_ASSERT(s.insert(1).second==false);
	TEST_ASSERT(s.insert(2048).second==true);

	s.clear();
	TEST_ASSERT(s.size()==0);

}


void test_bst_multiset()
{
	std::vector<int> aInts;
	for(int i=0; i<1024; ++i)
	{
		aInts.push_back(i);
	}
	for(int i=0; i<1024; ++i)
	{
		aInts.push_back(i);
	}
	std::random_shuffle(aInts.begin(),aInts.end());


	bst_multiset<int> s;
	s.insert(aInts.begin(),aInts.end());

	TEST_ASSERT(s.size()==2048);
	TEST_ASSERT(s.find(9)!=s.end());
	TEST_ASSERT(s.count(9)==2);
	s.erase(9);
	TEST_ASSERT(s.find(9)==s.end());
	TEST_ASSERT(s.count(9)==0);
	TEST_ASSERT(s.size()==2046);

	s.insert(1);
	TEST_ASSERT(s.count(1)==3);

	std::pair<bst_multiset<int>::iterator,bst_multiset<int>::iterator> eqr(s.equal_range(1));
	for(bst_multiset<int>::iterator it=eqr.first; it!=eqr.second; ++it)
	{
		TEST_ASSERT((*it)==1);
	}

	s.clear();
	TEST_ASSERT(s.size()==0);

}


TEST_DEFINE(TEST_Collection)
{
	test_bst_set();
	test_bst_multiset();

	indexer_set<String> sh;

	sh.insert("hello");
	sh.insert("world");

	TEST_ASSERT(sh.size()==2);
	TEST_ASSERT(sh.get(0)=="hello");
	TEST_ASSERT(sh.get(1)=="world");
	TEST_ASSERT(sh.find("hello")==0);
	TEST_ASSERT(sh.find("world")==1);
	TEST_ASSERT(sh.find("adfs")==-1);

	indexer_map<String,int> sm;

	sm["a"]=1;
	sm["b"]=4;
	sm["c"]=2;

	TEST_ASSERT(sm["a"]==1);
	TEST_ASSERT(sm["b"]==4);
	sm.rehash(12425);
	TEST_ASSERT(sm["a"]==1);
	TEST_ASSERT(sm["b"]==4);

	sm.erase("b");
	TEST_ASSERT(sm.find("b")==-1);

	vhwd::arr_xt<double,AllocatorM<double,128> > arr;

	//TEST_ASSERT_THROW(arr.resize((size_t)(-1)),std::bad_alloc);

	arr.resize(6,5,4,3,2,1);
	arr(3,3)=3.25;
	for(size_t i=0; i<6; i++)
	{
		TEST_ASSERT(arr.size(i)==6-i);
	}
	TEST_ASSERT(arr.size()==6*5*4*3*2*1);
	TEST_ASSERT( ((char*)&arr(0)-(char*)NULL)%128==0);
	arr.resize(6,6);
	TEST_ASSERT( ((char*)&arr(0)-(char*)NULL)%128==0);

	TEST_ASSERT(arr(3,3)==3.25);
	TEST_ASSERT(arr.size()==36);
	arr.clear();

	TEST_ASSERT(arr.size()==0);

};
