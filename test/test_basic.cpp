
#include "vhwd/testing/test.h"
#include "vhwd/basic.h"


using namespace vhwd;


TEST_DEFINE(TEST_String)
{

	String s1;
	String s2("hello");
	TEST_ASSERT(s1=="");
	TEST_ASSERT(s1.size()==0);
	TEST_ASSERT(s1!=s2);
	TEST_ASSERT(s2.size()==5);
	s1=s2;
	TEST_ASSERT(s1==s2);

	String s3="world";
	String s4="helloworld";

	TEST_ASSERT(s4.substr(0,5)=="hello");
	TEST_ASSERT(s4.substr(5)=="world");
	TEST_ASSERT(s4.substr(2,2)=="ll");

	TEST_ASSERT(s3.size()==5);

	String s5(s2);
	s5+=s3;
	String s6;
	s6<<s2<<s3;

	String s7=String::Format("%s%s",s2,s3);

	TEST_ASSERT(s2+s3==s4);
	TEST_ASSERT(s5==s4);
	TEST_ASSERT(s6==s4);
	TEST_ASSERT(s7==s4);

}

TEST_DEFINE(TEST_BitFlags)
{
	enum
	{
		FLAG1=1<<0,
		FLAG2=1<<1,
		FLAG3=1<<2
	};

	BitFlags flags;
	TEST_ASSERT(flags.val()==0);

	flags.add(FLAG1);
	TEST_ASSERT(flags.val()==FLAG1);
	TEST_ASSERT(flags.get(FLAG1));
	TEST_ASSERT(!flags.get(FLAG2));

	flags.set(FLAG2,true);
	TEST_ASSERT(flags.get(FLAG1));
	TEST_ASSERT(flags.get(FLAG2));

	flags.inv(FLAG1);
	TEST_ASSERT(!flags.get(FLAG1));
	TEST_ASSERT(flags.get(FLAG2));

	flags.clr(FLAG3);
	TEST_ASSERT(!flags.get(FLAG2));
	TEST_ASSERT(flags.get(FLAG3));
	
}

TEST_DEFINE(TEST_Clock)
{
	time_t n=time(NULL);
	TimePoint p1=Clock::now();
	TimePoint p2=p1+TimeSpan::Seconds(120);
	TimeSpan s1=p2-p1;
	TEST_ASSERT(p1+s1==p2);
	TEST_ASSERT(s1.GetSeconds()==120);
	TEST_ASSERT(s1/TimeSpan::Minutes(1)==2.0);
	TEST_ASSERT(p2>p1);

	struct tm* ptm=localtime(&n);
	TimeDetail td(p1,TimeDetail::LOCAL);

	TEST_ASSERT(td.GetYear()==ptm->tm_year+1900);
	TEST_ASSERT(td.GetMonth()==ptm->tm_mon+1);
	TEST_ASSERT(td.GetDay()==ptm->tm_mday);
	
}



int func(int v1,int v2,int v3,int v4,int v5)
{
	return v1*2+v2*3+v3*5+v4*7+v5*11;
}


TEST_DEFINE(TEST_Factor)
{
	int ret=vhwd::hbind<int>::g(func,1,2,3,4,5)();
	TEST_ASSERT(ret==func(1,2,3,4,5));

	vhwd::Factor<int()> fac0;
	vhwd::Factor<int(int)> fac1;
	vhwd::Factor<int(int,int)> fac2;
	fac0.reset(&func,1,2,3,4,5);
	TEST_ASSERT(fac0()==func(1,2,3,4,5));

	fac1.reset(&func,1,2,3,4,vhwd::_1);
	TEST_ASSERT(fac1(10)==func(1,2,3,4,10));
	fac1.reset(&func,vhwd::_1,1,2,3,4);
	TEST_ASSERT(fac1(10)==func(10,1,2,3,4));

	fac2.reset(&func,vhwd::_1,1,vhwd::_2,3,4);
	TEST_ASSERT(fac2(10,3)==func(10,1,3,3,4));

}

TEST_DEFINE(TEST_Basic_other)
{


	Console::SetColor(Console::COLOR_R|Console::COLOR_S);
	Console::WriteLine("red text");
	Console::SetColor(Console::COLOR_B|Console::COLOR_S);
	Console::WriteLine("blue text");
	Console::SetColor(Console::COLOR_G|Console::COLOR_S);
	Console::WriteLine("green text");
	Console::SetColor(Console::COLOR_D);
	Console::WriteLine("white text");

	Console::WriteLine(
		String::Format("cores: %d, pagesize: %d, cacheline: %d",
		System::GetCpuCount(),
		System::GetPageSize(),
		System::GetCacheLineSize()));

}



template<typename T>
class MyObjectT : public Object
{
public:
	T impl;
	DECLARE_OBJECT_INFO_T(MyObjectT<T>,ObjectInfo)
};

IMPLEMENT_OBJECT_INFO_NAME_T(MyObjectT<T>,ObjectInfo,"myobj")


TEST_DEFINE(TEST_Object)
{

	MyObjectT<int32_t> my_obj;
	String sObjName=my_obj.GetObjectName();
	TEST_ASSERT(sObjName=="myobj#i32");


	ObjectCreator& objcreator(ObjectCreator::current());

	Object* obj=objcreator.Create("Object");
	Object* dat=objcreator.Create("ObjectData");
	Object* myobj=objcreator.Create("myobj#i32");

	TEST_ASSERT(obj!=NULL);
	TEST_ASSERT(dynamic_cast<ObjectData*>(dat)!=NULL);
	TEST_ASSERT(dynamic_cast<MyObjectT<int32_t>*>(myobj)!=NULL);

	delete obj;
	delete dat;
	delete myobj;

};