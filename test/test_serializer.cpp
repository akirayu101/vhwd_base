
#include "vhwd/testing/test.h"
#include "vhwd/threading.h"
#include "vhwd/collection/array_xt.h"
#include "vhwd/serialization.h"



using namespace vhwd;

class serializable_data : public ObjectData
{
public:
	float64_t val;
	bool serialize(Serializer& ar)
	{
		ar & val;
		return ar.good();
	}
	DECLARE_OBJECT_INFO(serializable_data,ObjectInfo)
};

IMPLEMENT_OBJECT_INFO(serializable_data,ObjectInfo)

class sample_data
{
public:


	std::map<String,float64_t> aMaps;

	arr_1t<int32_t> aInts;
	String s1;
	DataPtrT<serializable_data> dptr1;
	DataPtrT<ObjectData> dptr2;

	int32_t ivals[10];
	arr_xt<float64_t> arr;

	void serialize(Serializer& ar)
	{
		ar & aInts & aMaps & s1 & dptr1 & dptr2 & ivals & arr;
	}

};


TEST_DEFINE(TEST_Serializer)
{


	StreamSerializer<SerializerReader> reader;
	StreamSerializer<SerializerWriter> writer;

	sample_data dat[2];

	dat[0].aInts.push_back(1);
	dat[0].aInts.push_back(5);
	dat[0].aMaps["hello"]=1.033;
	dat[0].aMaps["gg"]=1.034;
	dat[0].s1="sample_string";

	serializable_data* p1=new serializable_data;
	p1->val=1.324;
	dat[0].dptr1.reset(p1);
	dat[0].dptr2.reset(p1);

	dat[0].arr.resize(6,3,2);
	dat[0].arr(1,2,1)=3.2;
	dat[0].arr(0)=1.234;

	dat[0].ivals[3]=32;

	writer.open("test_serializer.dat");
	writer & dat[0];
	writer.close();

	reader.open("test_serializer.dat");
	reader & dat[1];
	reader.close();

	TEST_ASSERT(dat[1].aInts==dat[0].aInts);
	TEST_ASSERT(dat[1].aMaps==dat[0].aMaps);
	TEST_ASSERT(dat[1].s1==dat[0].s1);

	TEST_ASSERT(dat[1].dptr1==dat[1].dptr2);
	TEST_ASSERT(dat[1].dptr1!=NULL);
	TEST_ASSERT(dat[1].dptr1->val==dat[0].dptr1->val);

	TEST_ASSERT(dat[1].arr.size(0)==dat[0].arr.size(0));
	TEST_ASSERT(dat[1].arr.size(1)==dat[0].arr.size(1));
	TEST_ASSERT(dat[1].arr.size(2)==dat[0].arr.size(2));

	if(dat[1].arr.size(0)==dat[0].arr.size(0))
	{
		for(size_t i=0;i<dat[1].arr.size(0);i++)
		{
			TEST_ASSERT(dat[1].arr(i)==dat[0].arr(i));
		}
	}

	for(int i=0;i<10;i++)
	{
		TEST_ASSERT(dat[1].ivals[i]==dat[0].ivals[i]);
	}

	SerializerBuffer sbuf;

	double v1[4]={1.234,234.0,323,432};
	double v2[4];

	sbuf.writer() & v1; // write data to buffer
	sbuf.reader() & v2; // read data from buffer

	TEST_ASSERT(memcmp(v1,v2,sizeof(double)*4)==0);
	TEST_ASSERT(sbuf.skip()); // reader reach the end of buffer

}
