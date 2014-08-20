
#include "vhwd.h"


using namespace vhwd;

TEST_DEFINE(TEST_Scripting_variant)
{

	Variant a,b,c,d,e;
	a.reset<double>(10.0);
	double v;
	TEST_ASSERT(a.get<double>(v) && v==10.0);
	TEST_ASSERT(a.set<double>(20.0));
	TEST_ASSERT(a.get<double>(v) && v==20.0);

	double* p=a.ptr<double>();
	TEST_ASSERT(p!=NULL && *p==20.0);
	TEST_ASSERT(a.ref<double>()==20.0);

	b.reset<double>(20.0);
	c.reset<double>(11);

	TEST_ASSERT(a==b);
	TEST_ASSERT(a!=c);

	c=a;
	TEST_ASSERT(a==c);

	d.reset<String>("10");
	e.reset<int64_t>(15);


	TEST_ASSERT(PLCast<int>::g(a)==20);
	TEST_ASSERT(PLCast<int>::g(d)==10);	// cast string "10" to int
	TEST_ASSERT(PLCast<String>::g(e)==String("15"));	// cast int64_t 15 to string "15"
	

}

TEST_DEFINE(TEST_Scripting_Executor)
{
	
	Executor ks;
	int nsp=ks.stack.nsp;

	ks.push(1);
	ks.push(2);
	ks.call2<XOP2_ADD>(); // eval 1+2
	TEST_ASSERT(PLCast<int>::g(ks.top())==3);
	ks.popq();
	TEST_ASSERT(ks.stack.nsp==nsp);

	ks.push(1.23);
	ks.get_ggvar("math");
	ks.get_index("sin");	// top()=math.sin
	ks.calln(1);			// call math.sin(1.23)
	ks.set_table("ans");	// save result to table["ans"]
	TEST_ASSERT(PLCast<double>::g(ks.table["ans"])==::sin(1.23));
	TEST_ASSERT(ks.stack.nsp==nsp);

// string cat operator..
	ks.execute("a=\"hello\";b=\"world\";c=a..b;");
	TEST_ASSERT(PLCast<String>::g(ks.table["c"])==String("helloworld"));


#define EVAL_ASSERT(T,X) TEST_ASSERT(ks.eval<T>(#X)==(X));

// add sub mul div mod
	EVAL_ASSERT(int,2+3);
	EVAL_ASSERT(int,2-3);
	EVAL_ASSERT(int,2*3);
	EVAL_ASSERT(int,4/2);
	EVAL_ASSERT(int,6%4);

// bitwise
	EVAL_ASSERT(int,6|2);
	EVAL_ASSERT(int,6^2);
	EVAL_ASSERT(int,6&2);
	EVAL_ASSERT(int,~6);

// relation
	EVAL_ASSERT(bool,6!=2);
	EVAL_ASSERT(bool,6==2);
	EVAL_ASSERT(bool,6>2);
	EVAL_ASSERT(bool,6>=2);
	EVAL_ASSERT(bool,6<2);
	EVAL_ASSERT(bool,6<=2);

// logical
	EVAL_ASSERT(bool,true&&true);
	EVAL_ASSERT(bool,true&&false);
	EVAL_ASSERT(bool,false&&true);
	EVAL_ASSERT(bool,false&&false);

	EVAL_ASSERT(bool,true||true);
	EVAL_ASSERT(bool,true||false);
	EVAL_ASSERT(bool,false||true);
	EVAL_ASSERT(bool,false||false);

	TEST_ASSERT(ks.eval<bool>("true^^true")==false);
	TEST_ASSERT(ks.eval<bool>("true^^false")==true);
	TEST_ASSERT(ks.eval<bool>("false^^true")==true);
	TEST_ASSERT(ks.eval<bool>("false^^false")==false);

	EVAL_ASSERT(bool,!false);
	EVAL_ASSERT(bool,!true);

// complex
	TEST_ASSERT(ks.eval<std::complex<double> >("complex(1.2,2.3)")==std::complex<double>(1.2,2.3));
	TEST_ASSERT(ks.eval<double>("math.real(complex(1.2,2.3))")==1.2);
	TEST_ASSERT(ks.eval<double>("math.imag(complex(1.2,2.3))")==2.3);
	TEST_ASSERT(ks.eval<double>("math.abs(complex(3,4))")==std::abs(std::complex<double>(3,4)));

	TEST_ASSERT(ks.eval<std::complex<double> >("math.conj(complex(1.2,2.3))")==std::complex<double>(1.2,-2.3));
	TEST_ASSERT(ks.eval<std::complex<double> >("math.sqrt(complex(3,4))")==std::sqrt(std::complex<double>(3,4)));
	TEST_ASSERT(ks.eval<std::complex<double> >("math.pow(complex(3,4),2)")==std::pow(std::complex<double>(3,4),2));
	TEST_ASSERT(ks.eval<std::complex<double> >("math.sin(complex(3,4))")==std::sin(std::complex<double>(3,4)));
	TEST_ASSERT(ks.eval<std::complex<double> >("math.cos(complex(3,4))")==std::cos(std::complex<double>(3,4)));
	TEST_ASSERT(ks.eval<std::complex<double> >("math.tan(complex(3,4))")==std::tan(std::complex<double>(3,4)));

// casting

	TEST_ASSERT(ks.eval<String>("123")==String("123"));
	TEST_ASSERT(ks.eval<String>("1.25")==String("1.25"));
	TEST_ASSERT(ks.eval<double>("\"1.25\"")==1.25);
	TEST_ASSERT(ks.eval<int64_t>("\"123\"")==123);

}


TEST_DEFINE(TEST_Scripting_Executor2)
{
	Executor ks;

	ks.execute("a=1;b=2;c=a+b;");
	TEST_ASSERT(PLCast<int>::g(ks.table["c"])==3);

	ks.execute("x=1;y=2;x,y=y,x;");
	TEST_ASSERT(PLCast<int>::g(ks.table["x"])==2);
	TEST_ASSERT(PLCast<int>::g(ks.table["y"])==1);

	ks.execute("s=0;if(1+2==3) s=1;else s=-1;");
	TEST_ASSERT(PLCast<int>::g(ks.table["s"])==1);

	ks.execute("s=0;for(local i=0;i<10;i++) {if(i==2) continue; if(i==5) break; s+=i;}");
	TEST_ASSERT(PLCast<int>::g(ks.table["s"])==1+3+4);
	
	ks.execute("s=0;while(s%2==0&&s<100) {s=s+2;}");
	TEST_ASSERT(PLCast<int>::g(ks.table["s"])==100);

	ks.execute("s=0;do {s=s+2;} while(false);");
	TEST_ASSERT(PLCast<int>::g(ks.table["s"])==2);


// switch
	ks.execute(
"s=4;"
"switch(s)"
"{"
"	case 0,1,2: io.println(\"0,1,2\");"
"	case \"abc\",4: io.println(\"string abc or 4\");"
"	default: io.println(\"other\");"
"};");



// array
	ks.execute("s=linspace(0,10,11)*2;");
	arr_xt<double>& s(ks.table["s"].ref<arr_xt<double> >());
	TEST_ASSERT(s.size()==11);
	for(size_t i=0;i<s.size();i++)
	{
		TEST_ASSERT(s(i)==(double)i*2);
	}

// function
	ks.execute("function fn(x,y){return x+y,x-y;}; a,b=fn(1,2);");
	TEST_ASSERT(PLCast<int>::g(ks.table["a"])==1+2);
	TEST_ASSERT(PLCast<int>::g(ks.table["b"])==1-2);


// class 
	ks.execute(
"class Rect"
"{"
"	member.x=0;"
"	member.y=0;"
"	static.area=function()"
"	{"
"		return this.x*this.y;"
"	};"
"};"
"a=Rect();"
"a.x=10;"
"a.y=20;"
"s=a.area();"
);
	TEST_ASSERT(PLCast<int>::g(ks.table["s"])==10*20);


// for_each
	ks.execute("s=linspace(0,10,11);for_each(v in s) io.print(v,\" \");io.println();");
	ks.execute("s=table();s.a=1;s.b=2;for_each(k,v in s) {io.print(k,\"=\",v);io.println();}");
	ks.execute("s=array(0,\"hello\",3.5);;for_each(v in s) io.print(v,\" \");io.println();");
}
