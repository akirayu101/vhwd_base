#include "vhwd/config.h"

VHWD_ENTER

template<typename T>
class factor_reference
{
	const factor_reference& operator=(const factor_reference&);
public:
	T* ref;
	factor_reference(T& f):ref(&f){}
	operator T&(){return *ref;}
};

template<typename T>
factor_reference<T> mk_ref(T& v){return factor_reference<T>(v);}

template<typename P0,typename P1=void,typename P2=void,typename P3=void,typename P4=void,typename P5=void,typename P6=void>
class FParmSel
{
public:
	static P0 g(P0 p0,...){return p0;}
};

template<typename P1,typename P2,typename P3,typename P4,typename P5,typename P6>
class FParmSel<tl::int2type<1>,P1,P2,P3,P4,P5,P6>
{
public:
	typedef tl::int2type<1> P0;
	static P1 g(P0,P1 p1,...){return p1;}
};

template<typename P1,typename P2,typename P3,typename P4,typename P5,typename P6>
class FParmSel<tl::int2type<2>,P1,P2,P3,P4,P5,P6>
{
public:
	typedef tl::int2type<2> P0;
	static P2 g(P0,P1,P2 p2,...){return p2;}
};

template<typename P1,typename P2,typename P3,typename P4,typename P5,typename P6>
class FParmSel<tl::int2type<3>,P1,P2,P3,P4,P5,P6>
{
public:
	typedef tl::int2type<3> P0;
	static P3 g(P0,P1,P2,P3 p3,...){return p3;}
};

template<typename P1,typename P2,typename P3,typename P4,typename P5,typename P6>
class FParmSel<tl::int2type<4>,P1,P2,P3,P4,P5,P6>
{
public:
	typedef tl::int2type<4> P0;
	static P4 g(P0,P1,P2,P3,P4 p4,...){return p4;}
};

template<typename P1,typename P2,typename P3,typename P4,typename P5,typename P6>
class FParmSel<tl::int2type<5>,P1,P2,P3,P4,P5,P6>
{
public:
	typedef tl::int2type<5> P0;
	static P5 g(P0,P1,P2,P3,P4,P5 p5,...){return p5;}
};

template<typename P1,typename P2,typename P3,typename P4,typename P5,typename P6>
class FParmSel<tl::int2type<6>,P1,P2,P3,P4,P5,P6>
{
public:
	typedef tl::int2type<6> P0;
	static P6 g(P0,P1,P2,P3,P4,P5,P6 p6,...){return p6;}
};


template<typename T>
class RealParm
{
public:
	typedef T type;
};

template<typename T,bool F>
class RealParamPointer;

template<typename T>
class RealParamPointer<T,true>
{
public:
	typedef DataPtrT<T> type;
};

template<typename T>
class RealParamPointer<T,false>
{
public:
	typedef T* type;
};

template<typename T>
class RealParm<T*> : public RealParamPointer<T,tl::is_convertible<T,ObjectData>::value>
{
public:

};

template<typename T>
class RealParm<factor_reference<T> >
{
public:
	typedef T& type;
};


template<typename G>
class FBind;

template<typename G>
class FBindVoid;

template<typename Rt,typename P0>
class FBind<Rt(P0)>
{
	FBind& operator=(const FBind&);
public:
	typedef Rt ret_type;
	FBind(P0 p0_):p0(p0_){}
	typename RealParm<P0>::type p0;
	Rt operator()() {return (Rt)p0();}
};

template<typename Rt,typename P0,typename P1>
class FBind<Rt(P0,P1)>
{
	FBind& operator=(const FBind&);
public:
	typedef Rt ret_type;
	FBind(P0 p0_,P1 p1_):p0(p0_),p1(p1_){}
	typename RealParm<P0>::type p0;typename RealParm<P1>::type p1;
	Rt operator()() {return (Rt)p0(p1);}

	template<typename X1>
	Rt operator()(X1 x1) {
		return (Rt)p0(
			FParmSel<P1,X1>::g(p1,x1)
			);
	}
};

template<typename Rt,typename P1,typename Sd,typename Rv>
class FBind<Rt(Rv(Sd::*)(),P1)>
{
	FBind& operator=(const FBind&);
public:
	typedef Rt ret_type;
	typedef Rv (Sd::*P0)();

	FBind(P0 p0_,P1 p1_):p0(p0_),p1(p1_){}
	typename RealParm<P0>::type p0;typename RealParm<P1>::type p1;
	Rt operator()() {return (Rt)(p1->*p0)();}

	template<typename X1>
	Rt operator()(X1 x1) 
	{
		return (Rt)(FParmSel<P1,X1>::g(p1,x1)->*p0)();
	}
};

template<typename Rt,typename P0,typename P1,typename P2>
class FBind<Rt(P0,P1,P2)>
{
	FBind& operator=(const FBind&);
public:
	typedef Rt ret_type;
	FBind(P0 p0_,P1 p1_,P2 p2_):p0(p0_),p1(p1_),p2(p2_){}
	typename RealParm<P0>::type p0;typename RealParm<P1>::type p1;typename RealParm<P2>::type p2;

	Rt operator()() {return (Rt)p0(p1,p2);}
	
	template<typename X1>
	Rt operator()(X1 x1) 
	{		
		return (Rt)p0(
			FParmSel<P1,X1>::g(p1,x1),
			FParmSel<P2,X1>::g(p2,x1)
			);
	}

	template<typename X1,typename X2>
	Rt operator()(X1 x1,X2 x2) 
	{		
		return (Rt)p0(
			FParmSel<P1,X1,X2>::g(p1,x1,x2),
			FParmSel<P2,X1,X2>::g(p2,x1,x2)
			);
	}
};

template<typename Rt,typename P1,typename P2,typename Sd,typename Rv,typename M1>
class FBind<Rt(Rv (Sd::*)(M1),P1,P2)>
{
	FBind& operator=(const FBind&);
public:
	typedef Rt ret_type;
	typedef Rv (Sd::*P0)(M1);
	FBind(P0 p0_,P1 p1_,P2 p2_):p0(p0_),p1(p1_),p2(p2_){}
	typename RealParm<P0>::type p0;typename RealParm<P1>::type p1;typename RealParm<P2>::type p2;
	Rt operator()() {return (Rt)(p1->*p0)(p2);}

	template<typename X1>
	Rt operator()(X1 x1) 
	{		
		return (Rt)(FParmSel<P1,X1>::g(p1,x1)->*p0)(
			FParmSel<P2,X1>::g(p2,x1)
			);
	}

	template<typename X1,typename X2>
	Rt operator()(X1 x1,X2 x2) 
	{
		return (Rt)(FParmSel<P1,X1,X2>::g(p1,x1,x2)->*p0)
			(
				FParmSel<P2,X1,X2>::g(p2,x1,x2)
			);
	}
};


template<typename Rt,typename P0,typename P1,typename P2,typename P3>
class FBind<Rt(P0,P1,P2,P3)>
{
	FBind& operator=(const FBind&);
public:
	typedef Rt ret_type;
	FBind(P0 p0_,P1 p1_,P2 p2_,P3 p3_):p0(p0_),p1(p1_),p2(p2_),p3(p3_){}
	typename RealParm<P0>::type p0;typename RealParm<P1>::type p1;typename RealParm<P2>::type p2;typename RealParm<P3>::type p3;

	Rt operator()() {return (Rt)p0(p1,p2,p3);}
	
	template<typename X1>
	Rt operator()(X1 x1) 
	{		
		return (Rt)p0(
			FParmSel<P1,X1>::g(p1,x1),
			FParmSel<P2,X1>::g(p2,x1),
			FParmSel<P3,X1>::g(p3,x1)
			);
	}

	template<typename X1,typename X2>
	Rt operator()(X1 x1,X2 x2) 
	{		
		return (Rt)p0(
			FParmSel<P1,X1,X2>::g(p1,x1,x2),
			FParmSel<P2,X1,X2>::g(p2,x1,x2),
			FParmSel<P3,X1,X2>::g(p3,x1,x2)
			);
	}
	template<typename X1,typename X2,typename X3>
	Rt operator()(X1 x1,X2 x2,X3 x3) 
	{		
		return (Rt)p0(
			FParmSel<P1,X1,X2,X3>::g(p1,x1,x2,x3),
			FParmSel<P2,X1,X2,X3>::g(p2,x1,x2,x3),
			FParmSel<P3,X1,X2,X3>::g(p3,x1,x2,x3)
			);
	}

};

template<typename Rt,typename P1,typename P2,typename P3,typename Sd,typename Rv,typename M1,typename M2>
class FBind<Rt(Rv (Sd::*)(M1,M2),P1,P2,P3)>
{
	FBind& operator=(const FBind&);
public:
	typedef Rt ret_type;
	typedef Rv (Sd::*P0)(M1,M2);
	FBind(P0 p0_,P1 p1_,P2 p2_,P3 p3_):p0(p0_),p1(p1_),p2(p2_),p3(p3_){}
	typename RealParm<P0>::type p0;typename RealParm<P1>::type p1;typename RealParm<P2>::type p2;typename RealParm<P3>::type p3;
	Rt operator()() {return (Rt)(p1->*p0)(p2,p3);}

	template<typename X1>
	Rt operator()(X1 x1) 
	{		
		return (Rt)(FParmSel<P1,X1>::g(p1,x1)->*p0)(
			FParmSel<P2,X1>::g(p2,x1),
			FParmSel<P3,X1>::g(p3,x1)
			);
	}

	template<typename X1,typename X2>
	Rt operator()(X1 x1,X2 x2) 
	{
		return (Rt)(FParmSel<P1,X1,X2>::g(p1,x1,x2)->*p0)
			(
				FParmSel<P2,X1,X2>::g(p2,x1,x2),
				FParmSel<P3,X1,X2>::g(p3,x1,x2)
			);
	}

	template<typename X1,typename X2,typename X3>
	Rt operator()(X1 x1,X2 x2,X3 x3) 
	{
		return (Rt)(FParmSel<P1,X1,X2,X3>::g(p1,x1,x2,x3)->*p0)
			(
				FParmSel<P2,X1,X2,X3>::g(p2,x1,x2,x3),
				FParmSel<P3,X1,X2,X3>::g(p3,x1,x2,x3)
			);
	}
};



template<typename Rt,typename P0,typename P1,typename P2,typename P3,typename P4>
class FBind<Rt(P0,P1,P2,P3,P4)>
{
	FBind& operator=(const FBind&);
public:
	typedef Rt ret_type;
	FBind(P0 p0_,P1 p1_,P2 p2_,P3 p3_,P4 p4_):p0(p0_),p1(p1_),p2(p2_),p3(p3_),p4(p4_){}
	typename RealParm<P0>::type p0;typename RealParm<P1>::type p1;typename RealParm<P2>::type p2;typename RealParm<P3>::type p3;
	typename RealParm<P4>::type p4;
	Rt operator()() {return (Rt)p0(p1,p2,p3,p4);}
	
	template<typename X1>
	Rt operator()(X1 x1) 
	{		
		return (Rt)p0(
			FParmSel<P1,X1>::g(p1,x1),
			FParmSel<P2,X1>::g(p2,x1),
			FParmSel<P3,X1>::g(p3,x1),
			FParmSel<P4,X1>::g(p4,x1)
			);
	}

	template<typename X1,typename X2>
	Rt operator()(X1 x1,X2 x2) 
	{		
		return (Rt)p0(
			FParmSel<P1,X1,X2>::g(p1,x1,x2),
			FParmSel<P2,X1,X2>::g(p2,x1,x2),
			FParmSel<P3,X1,X2>::g(p3,x1,x2),
			FParmSel<P4,X1,X2>::g(p4,x1,x2)
			);
	}
	template<typename X1,typename X2,typename X3>
	Rt operator()(X1 x1,X2 x2,X3 x3) 
	{		
		return (Rt)p0(
			FParmSel<P1,X1,X2,X3>::g(p1,x1,x2,x3),
			FParmSel<P2,X1,X2,X3>::g(p2,x1,x2,x3),
			FParmSel<P3,X1,X2,X3>::g(p3,x1,x2,x3),
			FParmSel<P4,X1,X2,X3>::g(p4,x1,x2,x3)
			);
	}

	template<typename X1,typename X2,typename X3,typename X4>
	Rt operator()(X1 x1,X2 x2,X3 x3,X4 x4) 
	{		
		return (Rt)p0(
			FParmSel<P1,X1,X2,X3,X4>::g(p1,x1,x2,x3,x4),
			FParmSel<P2,X1,X2,X3,X4>::g(p2,x1,x2,x3,x4),
			FParmSel<P3,X1,X2,X3,X4>::g(p3,x1,x2,x3,x4),
			FParmSel<P4,X1,X2,X3,X4>::g(p4,x1,x2,x3,x4)
			);
	}

};

template<typename Rt,typename P1,typename P2,typename P3,typename P4,typename Sd,typename Rv,typename M1,typename M2,typename M3>
class FBind<Rt(Rv (Sd::*)(M1,M2,M3),P1,P2,P3,P4)>
{
	FBind& operator=(const FBind&);
public:
	typedef Rt ret_type;
	typedef Rv (Sd::*P0)(M1,M2,M3);
	FBind(P0 p0_,P1 p1_,P2 p2_,P3 p3_,P4 p4_):p0(p0_),p1(p1_),p2(p2_),p3(p3_),p4(p4_){}
	typename RealParm<P0>::type p0;typename RealParm<P1>::type p1;typename RealParm<P2>::type p2;typename RealParm<P3>::type p3;
	typename RealParm<P4>::type p4;

	Rt operator()() {return (Rt)(p1->*p0)(p2,p3,p4);}

	template<typename X1>
	Rt operator()(X1 x1) 
	{		
		return (Rt)(FParmSel<P1,X1>::g(p1,x1)->*p0)(
			FParmSel<P2,X1>::g(p2,x1),
			FParmSel<P3,X1>::g(p3,x1),
			FParmSel<P4,X1>::g(p4,x1)
			);
	}

	template<typename X1,typename X2>
	Rt operator()(X1 x1,X2 x2) 
	{
		return (Rt)(FParmSel<P1,X1,X2>::g(p1,x1,x2)->*p0)
			(
				FParmSel<P2,X1,X2>::g(p2,x1,x2),
				FParmSel<P3,X1,X2>::g(p3,x1,x2),
				FParmSel<P4,X1,X2>::g(p4,x1,x2)
			);
	}

	template<typename X1,typename X2,typename X3>
	Rt operator()(X1 x1,X2 x2,X3 x3) 
	{
		return (Rt)(FParmSel<P1,X1,X2,X3>::g(p1,x1,x2,x3)->*p0)
			(
				FParmSel<P2,X1,X2,X3>::g(p2,x1,x2,x3),
				FParmSel<P3,X1,X2,X3>::g(p3,x1,x2,x3),
				FParmSel<P4,X1,X2,X3>::g(p4,x1,x2,x3)
			);
	}

	template<typename X1,typename X2,typename X3,typename X4>
	Rt operator()(X1 x1,X2 x2,X3 x3,X4 x4) 
	{
		return (Rt)(FParmSel<P1,X1,X2,X3,X4>::g(p1,x1,x2,x3,x4)->*p0)
			(
				FParmSel<P2,X1,X2,X3,X4>::g(p2,x1,x2,x3,x4),
				FParmSel<P3,X1,X2,X3,X4>::g(p3,x1,x2,x3,x4),
				FParmSel<P4,X1,X2,X3,X4>::g(p4,x1,x2,x3,x4)
			);
	}
};



template<typename Rt,typename P0,typename P1,typename P2,typename P3,typename P4,typename P5>
class FBind<Rt(P0,P1,P2,P3,P4,P5)>
{
	FBind& operator=(const FBind&);
public:
	typedef Rt ret_type;
	FBind(P0 p0_,P1 p1_,P2 p2_,P3 p3_,P4 p4_,P5 p5_):p0(p0_),p1(p1_),p2(p2_),p3(p3_),p4(p4_),p5(p5_){}
	typename RealParm<P0>::type p0;typename RealParm<P1>::type p1;typename RealParm<P2>::type p2;typename RealParm<P3>::type p3;
	typename RealParm<P4>::type p4;typename RealParm<P5>::type p5;
	Rt operator()() {return (Rt)p0(p1,p2,p3,p4,p5);}
	
	template<typename X1>
	Rt operator()(X1 x1) 
	{		
		return (Rt)p0(
			FParmSel<P1,X1>::g(p1,x1),
			FParmSel<P2,X1>::g(p2,x1),
			FParmSel<P3,X1>::g(p3,x1),
			FParmSel<P4,X1>::g(p4,x1),
			FParmSel<P5,X1>::g(p5,x1)
			);
	}

	template<typename X1,typename X2>
	Rt operator()(X1 x1,X2 x2) 
	{		
		return (Rt)p0(
			FParmSel<P1,X1,X2>::g(p1,x1,x2),
			FParmSel<P2,X1,X2>::g(p2,x1,x2),
			FParmSel<P3,X1,X2>::g(p3,x1,x2),
			FParmSel<P4,X1,X2>::g(p4,x1,x2),
			FParmSel<P5,X1,X2>::g(p5,x1,x2)
			);
	}
	template<typename X1,typename X2,typename X3>
	Rt operator()(X1 x1,X2 x2,X3 x3) 
	{		
		return (Rt)p0(
			FParmSel<P1,X1,X2,X3>::g(p1,x1,x2,x3),
			FParmSel<P2,X1,X2,X3>::g(p2,x1,x2,x3),
			FParmSel<P3,X1,X2,X3>::g(p3,x1,x2,x3),
			FParmSel<P4,X1,X2,X3>::g(p4,x1,x2,x3),
			FParmSel<P5,X1,X2,X3>::g(p5,x1,x2,x3)
			);
	}

	template<typename X1,typename X2,typename X3,typename X4>
	Rt operator()(X1 x1,X2 x2,X3 x3,X4 x4) 
	{		
		return (Rt)p0(
			FParmSel<P1,X1,X2,X3,X4>::g(p1,x1,x2,x3,x4),
			FParmSel<P2,X1,X2,X3,X4>::g(p2,x1,x2,x3,x4),
			FParmSel<P3,X1,X2,X3,X4>::g(p3,x1,x2,x3,x4),
			FParmSel<P4,X1,X2,X3,X4>::g(p4,x1,x2,x3,x4),
			FParmSel<P5,X1,X2,X3,X4>::g(p5,x1,x2,x3,x4)
			);
	}

	template<typename X1,typename X2,typename X3,typename X4,typename X5>
	Rt operator()(X1 x1,X2 x2,X3 x3,X4 x4,X5 x5) 
	{		
		return (Rt)p0(
			FParmSel<P1,X1,X2,X3,X4,X5>::g(p1,x1,x2,x3,x4,x5),
			FParmSel<P2,X1,X2,X3,X4,X5>::g(p2,x1,x2,x3,x4,x5),
			FParmSel<P3,X1,X2,X3,X4,X5>::g(p3,x1,x2,x3,x4,x5),
			FParmSel<P4,X1,X2,X3,X4,X5>::g(p4,x1,x2,x3,x4,x5),
			FParmSel<P5,X1,X2,X3,X4,X5>::g(p5,x1,x2,x3,x4,x5)
			);
	}


};

template<typename Rt,typename P1,typename P2,typename P3,typename P4,typename P5,typename Sd,typename Rv,typename M1,typename M2,typename M3,typename M4>
class FBind<Rt(Rv (Sd::*)(M1,M2,M3,M4),P1,P2,P3,P4,P5)>
{
	FBind& operator=(const FBind&);
public:
	typedef Rt ret_type;
	typedef Rv (Sd::*P0)(M1,M2,M3,M4);
	FBind(P0 p0_,P1 p1_,P2 p2_,P3 p3_,P4 p4_,P5 p5_):p0(p0_),p1(p1_),p2(p2_),p3(p3_),p4(p4_),p5(p5_){}
	typename RealParm<P0>::type p0;typename RealParm<P1>::type p1;typename RealParm<P2>::type p2;typename RealParm<P3>::type p3;
	typename RealParm<P4>::type p4;typename RealParm<P5>::type p5;

	Rt operator()() {return (Rt)(p1->*p0)(p2,p3,p4,p5);}

	template<typename X1>
	Rt operator()(X1 x1) 
	{		
		return (Rt)(FParmSel<P1,X1>::g(p1,x1)->*p0)(
			FParmSel<P2,X1>::g(p2,x1),
			FParmSel<P3,X1>::g(p3,x1),
			FParmSel<P4,X1>::g(p4,x1),
			FParmSel<P5,X1>::g(p5,x1)
			);
	}

	template<typename X1,typename X2>
	Rt operator()(X1 x1,X2 x2) 
	{
		return (Rt)(FParmSel<P1,X1,X2>::g(p1,x1,x2)->*p0)
			(
				FParmSel<P2,X1,X2>::g(p2,x1,x2),
				FParmSel<P3,X1,X2>::g(p3,x1,x2),
				FParmSel<P4,X1,X2>::g(p4,x1,x2),
				FParmSel<P5,X1,X2>::g(p5,x1,x2)
			);
	}

	template<typename X1,typename X2,typename X3>
	Rt operator()(X1 x1,X2 x2,X3 x3) 
	{
		return (Rt)(FParmSel<P1,X1,X2,X3>::g(p1,x1,x2,x3)->*p0)
			(
				FParmSel<P2,X1,X2,X3>::g(p2,x1,x2,x3),
				FParmSel<P3,X1,X2,X3>::g(p3,x1,x2,x3),
				FParmSel<P4,X1,X2,X3>::g(p4,x1,x2,x3),
				FParmSel<P5,X1,X2,X3>::g(p5,x1,x2,x3)
			);
	}

	template<typename X1,typename X2,typename X3,typename X4>
	Rt operator()(X1 x1,X2 x2,X3 x3,X4 x4) 
	{
		return (Rt)(FParmSel<P1,X1,X2,X3,X4>::g(p1,x1,x2,x3,x4)->*p0)
			(
				FParmSel<P2,X1,X2,X3,X4>::g(p2,x1,x2,x3,x4),
				FParmSel<P3,X1,X2,X3,X4>::g(p3,x1,x2,x3,x4),
				FParmSel<P4,X1,X2,X3,X4>::g(p4,x1,x2,x3,x4),
				FParmSel<P5,X1,X2,X3,X4>::g(p5,x1,x2,x3,x4)
			);
	}

	template<typename X1,typename X2,typename X3,typename X4,typename X5>
	Rt operator()(X1 x1,X2 x2,X3 x3,X4 x4,X5 x5) 
	{
		return (Rt)(FParmSel<P1,X1,X2,X3,X4,X5>::g(p1,x1,x2,x3,x4,x5)->*p0)
			(
				FParmSel<P2,X1,X2,X3,X4,X5>::g(p2,x1,x2,x3,x4,x5),
				FParmSel<P3,X1,X2,X3,X4,X5>::g(p3,x1,x2,x3,x4,x5),
				FParmSel<P4,X1,X2,X3,X4,X5>::g(p4,x1,x2,x3,x4,x5),
				FParmSel<P5,X1,X2,X3,X4,X5>::g(p5,x1,x2,x3,x4,x5)
			);
	}
};
VHWD_LEAVE

