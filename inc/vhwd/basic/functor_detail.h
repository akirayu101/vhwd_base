// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#include "vhwd/config.h"

VHWD_ENTER

template<typename T>
class functor_reference
{
	const functor_reference& operator=(const functor_reference&);
public:
	T& ref;
	functor_reference(T& f):ref(f){}
	inline operator T&(){return ref;}
};

template<typename T>
class functor_pointer : public FakePtrT<T>
{
public:
	functor_pointer(T* p=NULL):FakePtrT<T>(p){}

	operator T*() {return this->get();}
	operator const T*() const {return this->get();}
};

template<typename T>
inline functor_reference<T> mk_ref(T& v){return functor_reference<T>(v);}

template<typename T>
inline functor_reference<const T> mk_cref(const T& v){return functor_reference<const T>(v);}


template<typename T>
class ParamRealImpl
{
public:
	typedef T type;
};

template<typename T>
class ParamRealImpl<T*>
{
public:
	typedef functor_pointer<T> type;
};

template<typename T>
class ParamRealImpl<functor_reference<T> >
{
public:
	typedef T& type;
};

template<typename T>
class ParamRealImpl<functor_reference<const T> >
{
public:
	typedef const T& type;
};


class BindImplBase
{
public:
	typedef tl::nulltype P1;
	typedef tl::nulltype P2;
	typedef tl::nulltype P3;
	typedef tl::nulltype P4;
	typedef tl::nulltype P5;
	typedef tl::nulltype P6;
	typedef tl::nulltype P7;
	typedef tl::nulltype P8;

	static tl::nulltype p1,p2,p3,p4,p5,p6,p7,p8;

};

template<typename Rv,typename X0>
class FunctorImpl : public BindImplBase
{
public:
	typedef Rv Rt;
	typedef typename ParamRealImpl<X0>::type P0;

	typedef P0& Ft;


	template<typename K1,typename K2,typename K3,typename K4,typename K5,typename K6,typename K7,typename K8>
	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		return ft(k1,k2,k3,k4,k5,k6,k7,k8);
	}

	template<typename K1,typename K2,typename K3,typename K4,typename K5,typename K6,typename K7>
	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,tl::nulltype)
	{
		return ft(k1,k2,k3,k4,k5,k6,k7);
	}

	template<typename K1,typename K2,typename K3,typename K4,typename K5,typename K6>
	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,tl::nulltype,tl::nulltype)
	{
		return ft(k1,k2,k3,k4,k5,k6);
	}

	template<typename K1,typename K2,typename K3,typename K4,typename K5>
	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,tl::nulltype,tl::nulltype,tl::nulltype)
	{
		return ft(k1,k2,k3,k4,k5);
	}

	template<typename K1,typename K2,typename K3,typename K4>
	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,tl::nulltype,tl::nulltype,tl::nulltype,tl::nulltype)
	{
		return ft(k1,k2,k3,k4);
	}

	template<typename K1,typename K2,typename K3>
	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,tl::nulltype,tl::nulltype,tl::nulltype,tl::nulltype,tl::nulltype)
	{
		return ft(k1,k2,k3);
	}

	template<typename K1,typename K2>
	static inline Rt g(Ft ft,K1 k1,K2 k2,tl::nulltype,tl::nulltype,tl::nulltype,tl::nulltype,tl::nulltype,tl::nulltype)
	{
		return ft(k1,k2);
	}

	template<typename K1>
	static inline Rt g(Ft ft,K1 k1,tl::nulltype,tl::nulltype,tl::nulltype,tl::nulltype,tl::nulltype,tl::nulltype,tl::nulltype)
	{
		return ft(k1);
	}

	static inline Rt g(Ft ft,tl::nulltype,tl::nulltype,tl::nulltype,tl::nulltype,tl::nulltype,tl::nulltype,tl::nulltype,tl::nulltype)
	{
		return ft();
	}
};

template<typename Rv,typename Rt>
class FunctorImpl<Rv,Rt(*)()> : public BindImplBase
{
public:
	typedef tl::nulltype K1;
	typedef tl::nulltype K2;
	typedef tl::nulltype K3;
	typedef tl::nulltype K4;
	typedef tl::nulltype K5;
	typedef tl::nulltype K6;
	typedef tl::nulltype K7;
	typedef tl::nulltype K8;

	typedef Rt return_type;
	typedef Rt(*Ft)();

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;(void)&k2;(void)&k3;(void)&k4;
		(void)&k5;(void)&k6;(void)&k7;(void)&k8;
		return ft();
	}
};

template<typename Rv,typename Rt,typename K1>
class FunctorImpl<Rv,Rt(*)(K1)> : public BindImplBase
{
public:

	typedef tl::nulltype K2;
	typedef tl::nulltype K3;
	typedef tl::nulltype K4;
	typedef tl::nulltype K5;
	typedef tl::nulltype K6;
	typedef tl::nulltype K7;
	typedef tl::nulltype K8;

	typedef Rt return_type;
	typedef Rt(*Ft)(K1);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;(void)&k2;(void)&k3;(void)&k4;
		(void)&k5;(void)&k6;(void)&k7;(void)&k8;
		return ft(k1);
	}
};

template<typename Rv,typename Rt,typename Sd>
class FunctorImpl<Rv,Rt(Sd::*)()> : public BindImplBase
{
public:

	typedef Sd* K1;
	typedef tl::nulltype K2;
	typedef tl::nulltype K3;
	typedef tl::nulltype K4;
	typedef tl::nulltype K5;
	typedef tl::nulltype K6;
	typedef tl::nulltype K7;
	typedef tl::nulltype K8;

	typedef Rt return_type;
	typedef Rt(Sd::*Ft)();

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;(void)&k2;(void)&k3;(void)&k4;
		(void)&k5;(void)&k6;(void)&k7;(void)&k8;
		return (k1->*ft)();

	}
};

template<typename Rv,typename Rt,typename K1,typename K2>
class FunctorImpl<Rv,Rt(*)(K1,K2)> : public BindImplBase
{
public:

	typedef tl::nulltype K3;
	typedef tl::nulltype K4;
	typedef tl::nulltype K5;
	typedef tl::nulltype K6;
	typedef tl::nulltype K7;
	typedef tl::nulltype K8;

	typedef Rt return_type;
	typedef Rt(*Ft)(K1,K2);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;(void)&k2;(void)&k3;(void)&k4;
		(void)&k5;(void)&k6;(void)&k7;(void)&k8;
		return ft(k1,k2);
	}

};


template<typename Rv,typename Rt,typename Sd,typename K2>
class FunctorImpl<Rv,Rt(Sd::*)(K2)> : public BindImplBase
{
public:

	typedef Sd* K1;
	typedef tl::nulltype K3;
	typedef tl::nulltype K4;
	typedef tl::nulltype K5;
	typedef tl::nulltype K6;
	typedef tl::nulltype K7;
	typedef tl::nulltype K8;

	typedef Rt return_type;
	typedef Rt(Sd::*Ft)(K2);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;(void)&k2;(void)&k3;(void)&k4;
		(void)&k5;(void)&k6;(void)&k7;(void)&k8;
		return (k1->*ft)(k2);
	}
};

template<typename Rv,typename Rt,typename K1,typename K2,typename K3>
class FunctorImpl<Rv,Rt(*)(K1,K2,K3)> : public BindImplBase
{
public:

	typedef tl::nulltype K4;
	typedef tl::nulltype K5;
	typedef tl::nulltype K6;
	typedef tl::nulltype K7;
	typedef tl::nulltype K8;

	typedef Rt return_type;
	typedef Rt(*Ft)(K1,K2,K3);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;(void)&k2;(void)&k3;(void)&k4;
		(void)&k5;(void)&k6;(void)&k7;(void)&k8;
		return ft(k1,k2,k3);
	}

};


template<typename Rv,typename Rt,typename Sd,typename K2,typename K3>
class FunctorImpl<Rv,Rt(Sd::*)(K2,K3)> : public BindImplBase
{
public:

	typedef Sd* K1;
	typedef tl::nulltype K4;
	typedef tl::nulltype K5;
	typedef tl::nulltype K6;
	typedef tl::nulltype K7;
	typedef tl::nulltype K8;

	typedef Rt return_type;
	typedef Rt(Sd::*Ft)(K2,K3);


	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;(void)&k2;(void)&k3;(void)&k4;
		(void)&k5;(void)&k6;(void)&k7;(void)&k8;
		return (k1->*ft)(k2,k3);
	}
};



template<typename Rv,typename Rt,typename K1,typename K2,typename K3,typename K4>
class FunctorImpl<Rv,Rt(*)(K1,K2,K3,K4)> : public BindImplBase
{
public:


	typedef tl::nulltype K5;
	typedef tl::nulltype K6;
	typedef tl::nulltype K7;
	typedef tl::nulltype K8;

	typedef Rt return_type;
	typedef Rt(*Ft)(K1,K2,K3,K4);


	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;(void)&k2;(void)&k3;(void)&k4;
		(void)&k5;(void)&k6;(void)&k7;(void)&k8;
		return ft(k1,k2,k3,k4);
	}

};


template<typename Rv,typename Rt,typename Sd,typename K2,typename K3,typename K4>
class FunctorImpl<Rv,Rt(Sd::*)(K2,K3,K4)> : public BindImplBase
{
public:

	typedef Sd* K1;

	typedef tl::nulltype K5;
	typedef tl::nulltype K6;
	typedef tl::nulltype K7;
	typedef tl::nulltype K8;

	typedef Rt return_type;
	typedef Rt(Sd::*Ft)(K2,K3,K4);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;(void)&k2;(void)&k3;(void)&k4;
		(void)&k5;(void)&k6;(void)&k7;(void)&k8;
		return (k1->*ft)(k2,k3,k4);
	}
};



template<typename Rv,typename Rt,typename K1,typename K2,typename K3,typename K4,typename K5>
class FunctorImpl<Rv,Rt(*)(K1,K2,K3,K4,K5)> : public BindImplBase
{
public:


	typedef tl::nulltype K6;
	typedef tl::nulltype K7;
	typedef tl::nulltype K8;

	typedef Rt return_type;
	typedef Rt(*Ft)(K1,K2,K3,K4,K5);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;(void)&k2;(void)&k3;(void)&k4;
		(void)&k5;(void)&k6;(void)&k7;(void)&k8;
		return ft(k1,k2,k3,k4,k5);
	}

};


template<typename Rv,typename Rt,typename Sd,typename K2,typename K3,typename K4,typename K5>
class FunctorImpl<Rv,Rt(Sd::*)(K2,K3,K4,K5)> : public BindImplBase
{
public:

	typedef Sd* K1;

	typedef tl::nulltype K6;
	typedef tl::nulltype K7;
	typedef tl::nulltype K8;

	typedef Rt return_type;
	typedef Rt(Sd::*Ft)(K2,K3,K4,K5);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;(void)&k2;(void)&k3;(void)&k4;
		(void)&k5;(void)&k6;(void)&k7;(void)&k8;
		return (k1->*ft)(k2,k3,k4,k5);
	}
};


template<typename Rv,typename Rt,typename K1,typename K2,typename K3,typename K4,typename K5,typename K6>
class FunctorImpl<Rv,Rt(*)(K1,K2,K3,K4,K5,K6)> : public BindImplBase
{
public:


	typedef tl::nulltype K7;
	typedef tl::nulltype K8;

	typedef Rt return_type;
	typedef Rt(*Ft)(K1,K2,K3,K4,K5,K6);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;(void)&k2;(void)&k3;(void)&k4;
		(void)&k5;(void)&k6;(void)&k7;(void)&k8;
		return ft(k1,k2,k3,k4,k5,k6);
	}

};


template<typename Rv,typename Rt,typename Sd,typename K2,typename K3,typename K4,typename K5,typename K6>
class FunctorImpl<Rv,Rt(Sd::*)(K2,K3,K4,K5,K6)> : public BindImplBase
{
public:

	typedef Sd* K1;

	typedef tl::nulltype K7;
	typedef tl::nulltype K8;

	typedef Rt return_type;
	typedef Rt(Sd::*Ft)(K2,K3,K4,K5,K6);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;(void)&k2;(void)&k3;(void)&k4;
		(void)&k5;(void)&k6;(void)&k7;(void)&k8;
		return (k1->*ft)(k2,k3,k4,k5,k6);
	}
};



template<typename Rv,typename Rt,typename K1,typename K2,typename K3,typename K4,typename K5,typename K6,typename K7>
class FunctorImpl<Rv,Rt(*)(K1,K2,K3,K4,K5,K6,K7)> : public BindImplBase
{
public:


	typedef tl::nulltype K8;

	typedef Rt return_type;
	typedef Rt(*Ft)(K1,K2,K3,K4,K5,K6,K7);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;(void)&k2;(void)&k3;(void)&k4;
		(void)&k5;(void)&k6;(void)&k7;(void)&k8;
		return ft(k1,k2,k3,k4,k5,k6,k7);
	}

};


template<typename Rv,typename Rt,typename Sd,typename K2,typename K3,typename K4,typename K5,typename K6,typename K7>
class FunctorImpl<Rv,Rt(Sd::*)(K2,K3,K4,K5,K6,K7)> : public BindImplBase
{
public:

	typedef Sd* K1;

	typedef tl::nulltype K8;

	typedef Rt return_type;
	typedef Rt(Sd::*Ft)(K2,K3,K4,K5,K6,K7);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;(void)&k2;(void)&k3;(void)&k4;
		(void)&k5;(void)&k6;(void)&k7;(void)&k8;
		return (k1->*ft)(k2,k3,k4,k5,k6,k7);
	}
};


template<typename Rv,typename Rt,typename K1,typename K2,typename K3,typename K4,typename K5,typename K6,typename K7,typename K8>
class FunctorImpl<Rv,Rt(*)(K1,K2,K3,K4,K5,K6,K7,K8)> : public BindImplBase
{
public:


	typedef Rt return_type;
	typedef Rt(*Ft)(K1,K2,K3,K4,K5,K6,K7,K8);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;(void)&k2;(void)&k3;(void)&k4;
		(void)&k5;(void)&k6;(void)&k7;(void)&k8;
		return ft(k1,k2,k3,k4,k5,k6,k7,k8);
	}

};


template<typename Rv,typename Rt,typename Sd,typename K2,typename K3,typename K4,typename K5,typename K6,typename K7,typename K8>
class FunctorImpl<Rv,Rt(Sd::*)(K2,K3,K4,K5,K6,K7,K8)> : public BindImplBase
{
public:

	typedef Sd* K1;


	typedef Rt return_type;
	typedef Rt(Sd::*Ft)(K2,K3,K4,K5,K6,K7,K8);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;(void)&k2;(void)&k3;(void)&k4;
		(void)&k5;(void)&k6;(void)&k7;(void)&k8;
		return (k1->*ft)(k2,k3,k4,k5,k6,k7,k8);
	}
};

template<typename K0,typename K1=tl::nulltype,typename K2=tl::nulltype,typename K3=tl::nulltype,typename K4=tl::nulltype,typename K5=tl::nulltype,typename K6=tl::nulltype,typename K7=tl::nulltype,typename K8=tl::nulltype>
class ParamDispatch
{
public:

	static inline K0 g(K0 k0,K1 k1=K1(),K2 k2=K2(),K3 k3=K3(),K4 k4=K4(),K5 k5=K5(),K6 k6=K6(),K7 k7=K7(),K8 k8=K8())
	{
		(void)&k0;
		(void)&k1;(void)&k2;(void)&k3;(void)&k4;
		(void)&k5;(void)&k6;(void)&k7;(void)&k8;
		return k0;
	}
};

#define PARM_SELECTION(X) \
template<typename K1,typename K2,typename K3,typename K4,typename K5,typename K6,typename K7,typename K8>\
class ParamDispatch<tl::int2type<X>,K1,K2,K3,K4,K5,K6,K7,K8>\
{\
public:\
	typedef tl::int2type<X> K0;\
	static inline K##X g(K0 k0,K1 k1=K1(),K2 k2=K2(),K3 k3=K3(),K4 k4=K4(),K5 k5=K5(),K6 k6=K6(),K7 k7=K7(),K8 k8=K8())\
	{\
		(void)&k0;\
		(void)&k1;(void)&k2;(void)&k3;(void)&k4;\
		(void)&k5;(void)&k6;(void)&k7;(void)&k8;\
		return k##X;\
	}\
};

PARM_SELECTION(1)
PARM_SELECTION(2)
PARM_SELECTION(3)
PARM_SELECTION(4)
PARM_SELECTION(5)
PARM_SELECTION(6)
PARM_SELECTION(7)
PARM_SELECTION(8)



#define BINDIMPL_OPERATOR0()\
inline Rt operator()()\
{\
	return basetype::g(p0,p1,p2,p3,p4,p5,p6,p7,p8);\
}\


#define BINDIMPL_OPERATOR1() BINDIMPL_OPERATOR0()\
template<typename K1>\
inline Rt operator()(K1 k1)\
{\
	return basetype::g(p0,\
		ParamDispatch<P1,K1>::g(p1,k1),\
		ParamDispatch<P2,K1>::g(p2,k1),\
		ParamDispatch<P3,K1>::g(p3,k1),\
		ParamDispatch<P4,K1>::g(p4,k1),\
		ParamDispatch<P5,K1>::g(p5,k1),\
		ParamDispatch<P6,K1>::g(p6,k1),\
		ParamDispatch<P7,K1>::g(p7,k1),\
		ParamDispatch<P8,K1>::g(p8,k1)\
		);\
}

#define BINDIMPL_OPERATOR2() BINDIMPL_OPERATOR1()\
template<typename K1,typename K2>\
inline Rt operator()(K1 k1,K2 k2)\
{\
	return basetype::g(p0,\
		ParamDispatch<P1,K1,K2>::g(p1,k1,k2),\
		ParamDispatch<P2,K1,K2>::g(p2,k1,k2),\
		ParamDispatch<P3,K1,K2>::g(p3,k1,k2),\
		ParamDispatch<P4,K1,K2>::g(p4,k1,k2),\
		ParamDispatch<P5,K1,K2>::g(p5,k1,k2),\
		ParamDispatch<P6,K1,K2>::g(p6,k1,k2),\
		ParamDispatch<P7,K1,K2>::g(p7,k1,k2),\
		ParamDispatch<P8,K1,K2>::g(p8,k1,k2)\
		);\
}

#define BINDIMPL_OPERATOR3() BINDIMPL_OPERATOR2()\
template<typename K1,typename K2,typename K3>\
inline Rt operator()(K1 k1,K2 k2,K3 k3)\
{\
	return basetype::g(p0,\
		ParamDispatch<P1,K1,K2,K3>::g(p1,k1,k2,k3),\
		ParamDispatch<P2,K1,K2,K3>::g(p2,k1,k2,k3),\
		ParamDispatch<P3,K1,K2,K3>::g(p3,k1,k2,k3),\
		ParamDispatch<P4,K1,K2,K3>::g(p4,k1,k2,k3),\
		ParamDispatch<P5,K1,K2,K3>::g(p5,k1,k2,k3),\
		ParamDispatch<P6,K1,K2,K3>::g(p6,k1,k2,k3),\
		ParamDispatch<P7,K1,K2,K3>::g(p7,k1,k2,k3),\
		ParamDispatch<P8,K1,K2,K3>::g(p8,k1,k2,k3)\
		);\
}

#define BINDIMPL_OPERATOR4() BINDIMPL_OPERATOR3()\
template<typename K1,typename K2,typename K3,typename K4>\
inline Rt operator()(K1 k1,K2 k2,K3 k3,K4 k4)\
{\
	return basetype::g(p0,\
		ParamDispatch<P1,K1,K2,K3,K4>::g(p1,k1,k2,k3,k4),\
		ParamDispatch<P2,K1,K2,K3,K4>::g(p2,k1,k2,k3,k4),\
		ParamDispatch<P3,K1,K2,K3,K4>::g(p3,k1,k2,k3,k4),\
		ParamDispatch<P4,K1,K2,K3,K4>::g(p4,k1,k2,k3,k4),\
		ParamDispatch<P5,K1,K2,K3,K4>::g(p5,k1,k2,k3,k4),\
		ParamDispatch<P6,K1,K2,K3,K4>::g(p6,k1,k2,k3,k4),\
		ParamDispatch<P7,K1,K2,K3,K4>::g(p7,k1,k2,k3,k4),\
		ParamDispatch<P8,K1,K2,K3,K4>::g(p8,k1,k2,k3,k4)\
		);\
}

#define BINDIMPL_OPERATOR5() BINDIMPL_OPERATOR4()\
template<typename K1,typename K2,typename K3,typename K4,typename K5>\
inline Rt operator()(K1 k1,K2 k2,K3 k3,K4 k4,K5 k5)\
{\
	return basetype::g(p0,\
		ParamDispatch<P1,K1,K2,K3,K4,K5>::g(p1,k1,k2,k3,k4,k5),\
		ParamDispatch<P2,K1,K2,K3,K4,K5>::g(p2,k1,k2,k3,k4,k5),\
		ParamDispatch<P3,K1,K2,K3,K4,K5>::g(p3,k1,k2,k3,k4,k5),\
		ParamDispatch<P4,K1,K2,K3,K4,K5>::g(p4,k1,k2,k3,k4,k5),\
		ParamDispatch<P5,K1,K2,K3,K4,K5>::g(p5,k1,k2,k3,k4,k5),\
		ParamDispatch<P6,K1,K2,K3,K4,K5>::g(p6,k1,k2,k3,k4,k5),\
		ParamDispatch<P7,K1,K2,K3,K4,K5>::g(p7,k1,k2,k3,k4,k5),\
		ParamDispatch<P8,K1,K2,K3,K4,K5>::g(p8,k1,k2,k3,k4,k5)\
		);\
}

#define BINDIMPL_OPERATOR6() BINDIMPL_OPERATOR5()\
template<typename K1,typename K2,typename K3,typename K4,typename K5,typename K6>\
inline Rt operator()(K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6)\
{\
	return basetype::g(p0,\
		ParamDispatch<P1,K1,K2,K3,K4,K5,K6>::g(p1,k1,k2,k3,k4,k5,k6),\
		ParamDispatch<P2,K1,K2,K3,K4,K5,K6>::g(p2,k1,k2,k3,k4,k5,k6),\
		ParamDispatch<P3,K1,K2,K3,K4,K5,K6>::g(p3,k1,k2,k3,k4,k5,k6),\
		ParamDispatch<P4,K1,K2,K3,K4,K5,K6>::g(p4,k1,k2,k3,k4,k5,k6),\
		ParamDispatch<P5,K1,K2,K3,K4,K5,K6>::g(p5,k1,k2,k3,k4,k5,k6),\
		ParamDispatch<P6,K1,K2,K3,K4,K5,K6>::g(p6,k1,k2,k3,k4,k5,k6),\
		ParamDispatch<P7,K1,K2,K3,K4,K5,K6>::g(p7,k1,k2,k3,k4,k5,k6),\
		ParamDispatch<P8,K1,K2,K3,K4,K5,K6>::g(p8,k1,k2,k3,k4,k5,k6)\
		);\
}

#define BINDIMPL_OPERATOR7() BINDIMPL_OPERATOR6()\
template<typename K1,typename K2,typename K3,typename K4,typename K5,typename K6,typename K7>\
inline Rt operator()(K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7)\
{\
	return basetype::g(p0,\
		ParamDispatch<P1,K1,K2,K3,K4,K5,K6,K7>::g(p1,k1,k2,k3,k4,k5,k6,k7),\
		ParamDispatch<P2,K1,K2,K3,K4,K5,K6,K7>::g(p2,k1,k2,k3,k4,k5,k6,k7),\
		ParamDispatch<P3,K1,K2,K3,K4,K5,K6,K7>::g(p3,k1,k2,k3,k4,k5,k6,k7),\
		ParamDispatch<P4,K1,K2,K3,K4,K5,K6,K7>::g(p4,k1,k2,k3,k4,k5,k6,k7),\
		ParamDispatch<P5,K1,K2,K3,K4,K5,K6,K7>::g(p5,k1,k2,k3,k4,k5,k6,k7),\
		ParamDispatch<P6,K1,K2,K3,K4,K5,K6,K7>::g(p6,k1,k2,k3,k4,k5,k6,k7),\
		ParamDispatch<P7,K1,K2,K3,K4,K5,K6,K7>::g(p7,k1,k2,k3,k4,k5,k6,k7),\
		ParamDispatch<P8,K1,K2,K3,K4,K5,K6,K7>::g(p8,k1,k2,k3,k4,k5,k6,k7)\
		);\
}

#define BINDIMPL_OPERATOR8() BINDIMPL_OPERATOR7()\
template<typename K1,typename K2,typename K3,typename K4,typename K5,typename K6,typename K7,typename K8>\
inline Rt operator()(K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)\
{\
	return basetype::g(p0,\
		ParamDispatch<P1,K1,K2,K3,K4,K5,K6,K7,K8>::g(p1,k1,k2,k3,k4,k5,k6,k7,k8),\
		ParamDispatch<P2,K1,K2,K3,K4,K5,K6,K7,K8>::g(p2,k1,k2,k3,k4,k5,k6,k7,k8),\
		ParamDispatch<P3,K1,K2,K3,K4,K5,K6,K7,K8>::g(p3,k1,k2,k3,k4,k5,k6,k7,k8),\
		ParamDispatch<P4,K1,K2,K3,K4,K5,K6,K7,K8>::g(p4,k1,k2,k3,k4,k5,k6,k7,k8),\
		ParamDispatch<P5,K1,K2,K3,K4,K5,K6,K7,K8>::g(p5,k1,k2,k3,k4,k5,k6,k7,k8),\
		ParamDispatch<P6,K1,K2,K3,K4,K5,K6,K7,K8>::g(p6,k1,k2,k3,k4,k5,k6,k7,k8),\
		ParamDispatch<P7,K1,K2,K3,K4,K5,K6,K7,K8>::g(p7,k1,k2,k3,k4,k5,k6,k7,k8),\
		ParamDispatch<P8,K1,K2,K3,K4,K5,K6,K7,K8>::g(p8,k1,k2,k3,k4,k5,k6,k7,k8)\
		);\
}

template<typename Ft>
class BindImpl;


template<typename Rv,typename X0>
class BindImpl<Rv(X0)> : public FunctorImpl<Rv,X0>
{
public:

	typedef FunctorImpl<Rv,X0> basetype;
	typedef Rv Rt;

	typedef typename ParamRealImpl<X0>::type P0;
	typedef typename basetype::P1 P1;
	typedef typename basetype::P2 P2;
	typedef typename basetype::P3 P3;
	typedef typename basetype::P4 P4;
	typedef typename basetype::P5 P5;
	typedef typename basetype::P6 P6;
	typedef typename basetype::P7 P7;
	typedef typename basetype::P8 P8;

	P0 p0;
	using basetype::p1;
	using basetype::p2;
	using basetype::p3;
	using basetype::p4;
	using basetype::p5;
	using basetype::p6;
	using basetype::p7;
	using basetype::p8;


	BindImpl(
		P0 p0_
		)
		:p0(p0_)
	{
	}


	BINDIMPL_OPERATOR0();

};

template<typename Rv,typename X0,typename X1>
class BindImpl<Rv(X0,X1)> : public FunctorImpl<Rv,X0>
{
public:

	typedef FunctorImpl<Rv,X0> basetype;
	typedef Rv Rt;

	typedef typename ParamRealImpl<X0>::type P0;
	typedef typename ParamRealImpl<X1>::type P1;
	typedef typename basetype::P2 P2;
	typedef typename basetype::P3 P3;
	typedef typename basetype::P4 P4;
	typedef typename basetype::P5 P5;
	typedef typename basetype::P6 P6;
	typedef typename basetype::P7 P7;
	typedef typename basetype::P8 P8;

	P0 p0;P1 p1;
	using basetype::p2;
	using basetype::p3;
	using basetype::p4;
	using basetype::p5;
	using basetype::p6;
	using basetype::p7;
	using basetype::p8;

	BindImpl(
		P0 p0_,P1 p1_
		)
		:p0(p0_)
		,p1(p1_)
	{
	}

	BINDIMPL_OPERATOR1();
};


template<typename Rv,typename X0,typename X1,typename X2>
class BindImpl<Rv(X0,X1,X2)> : public FunctorImpl<Rv,X0>
{
public:

	typedef FunctorImpl<Rv,X0> basetype;
	typedef Rv Rt;

	typedef typename ParamRealImpl<X0>::type P0;
	typedef typename ParamRealImpl<X1>::type P1;
	typedef typename ParamRealImpl<X2>::type P2;
	typedef typename basetype::P3 P3;
	typedef typename basetype::P4 P4;
	typedef typename basetype::P5 P5;
	typedef typename basetype::P6 P6;
	typedef typename basetype::P7 P7;
	typedef typename basetype::P8 P8;

	P0 p0;P1 p1;P2 p2;
	using basetype::p3;
	using basetype::p4;
	using basetype::p5;
	using basetype::p6;
	using basetype::p7;
	using basetype::p8;

	BindImpl(
		P0 p0_,P1 p1_,P2 p2_
		)
		:p0(p0_),p1(p1_),p2(p2_)
	{
	}

	BINDIMPL_OPERATOR2();
};


template<typename Rv,typename X0,typename X1,typename X2,typename X3>
class BindImpl<Rv(X0,X1,X2,X3)> : public FunctorImpl<Rv,X0>
{
public:

	typedef FunctorImpl<Rv,X0> basetype;
	typedef Rv Rt;


	typedef typename ParamRealImpl<X0>::type P0;
	typedef typename ParamRealImpl<X1>::type P1;
	typedef typename ParamRealImpl<X2>::type P2;
	typedef typename ParamRealImpl<X3>::type P3;
	typedef typename basetype::P4 P4;
	typedef typename basetype::P5 P5;
	typedef typename basetype::P6 P6;
	typedef typename basetype::P7 P7;
	typedef typename basetype::P8 P8;

	P0 p0;P1 p1;P2 p2;P3 p3;
	using basetype::p4;
	using basetype::p5;
	using basetype::p6;
	using basetype::p7;
	using basetype::p8;

	BindImpl(
		P0 p0_,P1 p1_,P2 p2_,P3 p3_
		)
		:p0(p0_),p1(p1_),p2(p2_),p3(p3_)
	{
	}

	BINDIMPL_OPERATOR3();
};


template<typename Rv,typename X0,typename X1,typename X2,typename X3,typename X4>
class BindImpl<Rv(X0,X1,X2,X3,X4)> : public FunctorImpl<Rv,X0>
{
public:

	typedef FunctorImpl<Rv,X0> basetype;
	typedef Rv Rt;


	typedef typename ParamRealImpl<X0>::type P0;
	typedef typename ParamRealImpl<X1>::type P1;
	typedef typename ParamRealImpl<X2>::type P2;
	typedef typename ParamRealImpl<X3>::type P3;
	typedef typename ParamRealImpl<X4>::type P4;
	typedef typename basetype::P5 P5;
	typedef typename basetype::P6 P6;
	typedef typename basetype::P7 P7;
	typedef typename basetype::P8 P8;

	P0 p0;P1 p1;P2 p2;P3 p3;P4 p4;
	using basetype::p5;
	using basetype::p6;
	using basetype::p7;
	using basetype::p8;

	BindImpl(
		P0 p0_,P1 p1_,P2 p2_,P3 p3_,P4 p4_
		)
		:p0(p0_),p1(p1_),p2(p2_),p3(p3_),p4(p4_)
	{

	}

	BINDIMPL_OPERATOR4();


};


template<typename Rv,typename X0,typename X1,typename X2,typename X3,typename X4,typename X5>
class BindImpl<Rv(X0,X1,X2,X3,X4,X5)> : public FunctorImpl<Rv,X0>
{
public:

	typedef FunctorImpl<Rv,X0> basetype;
	typedef Rv Rt;

	typedef typename ParamRealImpl<X0>::type P0;
	typedef typename ParamRealImpl<X1>::type P1;
	typedef typename ParamRealImpl<X2>::type P2;
	typedef typename ParamRealImpl<X3>::type P3;
	typedef typename ParamRealImpl<X4>::type P4;
	typedef typename ParamRealImpl<X5>::type P5;
	typedef typename basetype::P6 P6;
	typedef typename basetype::P7 P7;
	typedef typename basetype::P8 P8;

	P0 p0;P1 p1;P2 p2;P3 p3;P4 p4;P5 p5;
	using basetype::p6;
	using basetype::p7;
	using basetype::p8;

	BindImpl(
		P0 p0_,P1 p1_,P2 p2_,P3 p3_,P4 p4_,P5 p5_
		)
		:p0(p0_),p1(p1_),p2(p2_),p3(p3_),p4(p4_),p5(p5_)
	{

	}

	BINDIMPL_OPERATOR5();
};


template<typename Rv,typename X0,typename X1,typename X2,typename X3,typename X4,typename X5,typename X6>
class BindImpl<Rv(X0,X1,X2,X3,X4,X5,X6)> : public FunctorImpl<Rv,X0>
{
public:

	typedef FunctorImpl<Rv,X0> basetype;
	typedef Rv Rt;

	typedef typename ParamRealImpl<X0>::type P0;
	typedef typename ParamRealImpl<X1>::type P1;
	typedef typename ParamRealImpl<X2>::type P2;
	typedef typename ParamRealImpl<X3>::type P3;
	typedef typename ParamRealImpl<X4>::type P4;
	typedef typename ParamRealImpl<X5>::type P5;
	typedef typename ParamRealImpl<X6>::type P6;
	typedef typename basetype::P7 P7;
	typedef typename basetype::P8 P8;

	P0 p0;P1 p1;P2 p2;P3 p3;P4 p4;P5 p5;P6 p6;
	using basetype::p7;
	using basetype::p8;

	BindImpl(
		P0 p0_,P1 p1_,P2 p2_,P3 p3_,P4 p4_,P5 p5_,P6 p6_
		)
		:p0(p0_),p1(p1_),p2(p2_),p3(p3_),p4(p4_),p5(p5_),p6(p6_)
	{

	}

	BINDIMPL_OPERATOR6();

};


template<typename Rv,typename X0,typename X1,typename X2,typename X3,typename X4,typename X5,typename X6,typename X7>
class BindImpl<Rv(X0,X1,X2,X3,X4,X5,X6,X7)> : public FunctorImpl<Rv,X0>
{
public:

	typedef FunctorImpl<Rv,X0> basetype;
	typedef Rv Rt;

	typedef typename ParamRealImpl<X0>::type P0;
	typedef typename ParamRealImpl<X1>::type P1;
	typedef typename ParamRealImpl<X2>::type P2;
	typedef typename ParamRealImpl<X3>::type P3;
	typedef typename ParamRealImpl<X4>::type P4;
	typedef typename ParamRealImpl<X5>::type P5;
	typedef typename ParamRealImpl<X6>::type P6;
	typedef typename ParamRealImpl<X7>::type P7;
	typedef typename basetype::P8 P8;

	P0 p0;P1 p1;P2 p2;P3 p3;P4 p4;P5 p5;P6 p6;P7 p7;
	using basetype::p8;

	BindImpl(
		P0 p0_,P1 p1_,P2 p2_,P3 p3_,P4 p4_,P5 p5_,P6 p6_,P7 p7_
		)
		:p0(p0_),p1(p1_),p2(p2_),p3(p3_),p4(p4_),p5(p5_),p6(p6_),p7(p7_)
	{

	}


	BINDIMPL_OPERATOR7();

};


template<typename Rv,typename X0,typename X1,typename X2,typename X3,typename X4,typename X5,typename X6,typename X7,typename X8>
class BindImpl<Rv(X0,X1,X2,X3,X4,X5,X6,X7,X8)> : public FunctorImpl<Rv,X0>
{
public:

	typedef FunctorImpl<Rv,X0> basetype;
	typedef Rv Rt;

	typedef typename ParamRealImpl<X0>::type P0;
	typedef typename ParamRealImpl<X1>::type P1;
	typedef typename ParamRealImpl<X2>::type P2;
	typedef typename ParamRealImpl<X3>::type P3;
	typedef typename ParamRealImpl<X4>::type P4;
	typedef typename ParamRealImpl<X5>::type P5;
	typedef typename ParamRealImpl<X6>::type P6;
	typedef typename ParamRealImpl<X7>::type P7;
	typedef typename ParamRealImpl<X8>::type P8;

	P0 p0;P1 p1;P2 p2;P3 p3;P4 p4;P5 p5;P6 p6;P7 p7;P8 p8;

	BindImpl(
		P0 p0_,P1 p1_,P2 p2_,P3 p3_,P4 p4_,P5 p5_,P6 p6_,P7 p7_,P8 p8_
		)
		:p0(p0_),p1(p1_),p2(p2_),p3(p3_),p4(p4_),p5(p5_),p6(p6_),p7(p7_),p8(p8_)
	{

	}

	BINDIMPL_OPERATOR8();

};


VHWD_LEAVE

