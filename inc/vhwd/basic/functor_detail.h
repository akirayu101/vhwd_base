// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#include "vhwd/config.h"

VHWD_ENTER

typedef tl::nulltype nil_t;

template<typename T>
class functor_reference
{
	const functor_reference& operator=(const functor_reference&);
public:
	T& ref;
	functor_reference(T& f):ref(f) {}
	inline operator T&()
	{
		return ref;
	}
};

template<typename T>
class functor_pointer : public FakePtrT<T>
{
public:
	functor_pointer(T* p=NULL):FakePtrT<T>(p) {}

	inline operator T*()
	{
		return this->get();
	}
	inline operator const T*() const
	{
		return this->get();
	}
};

template<typename T>
inline functor_reference<T> mk_ref(T& v)
{
	return functor_reference<T>(v);
}

template<typename T>
inline functor_reference<const T> mk_cref(const T& v)
{
	return functor_reference<const T>(v);
}


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
	typedef nil_t P1;
	typedef nil_t P2;
	typedef nil_t P3;
	typedef nil_t P4;
	typedef nil_t P5;
	typedef nil_t P6;
	typedef nil_t P7;
	typedef nil_t P8;

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
	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,nil_t)
	{
		return ft(k1,k2,k3,k4,k5,k6,k7);
	}

	template<typename K1,typename K2,typename K3,typename K4,typename K5,typename K6>
	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,nil_t,nil_t)
	{
		return ft(k1,k2,k3,k4,k5,k6);
	}

	template<typename K1,typename K2,typename K3,typename K4,typename K5>
	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,nil_t,nil_t,nil_t)
	{
		return ft(k1,k2,k3,k4,k5);
	}

	template<typename K1,typename K2,typename K3,typename K4>
	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,nil_t,nil_t,nil_t,nil_t)
	{
		return ft(k1,k2,k3,k4);
	}

	template<typename K1,typename K2,typename K3>
	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,nil_t,nil_t,nil_t,nil_t,nil_t)
	{
		return ft(k1,k2,k3);
	}

	template<typename K1,typename K2>
	static inline Rt g(Ft ft,K1 k1,K2 k2,nil_t,nil_t,nil_t,nil_t,nil_t,nil_t)
	{
		return ft(k1,k2);
	}

	template<typename K1>
	static inline Rt g(Ft ft,K1 k1,nil_t,nil_t,nil_t,nil_t,nil_t,nil_t,nil_t)
	{
		return ft(k1);
	}

	static inline Rt g(Ft ft,nil_t,nil_t,nil_t,nil_t,nil_t,nil_t,nil_t,nil_t)
	{
		return ft();
	}
};

template<typename Rv,typename Rt>
class FunctorImpl<Rv,Rt(*)()> : public BindImplBase
{
public:
	typedef nil_t K1;
	typedef nil_t K2;
	typedef nil_t K3;
	typedef nil_t K4;
	typedef nil_t K5;
	typedef nil_t K6;
	typedef nil_t K7;
	typedef nil_t K8;

	typedef Rt return_type;
	typedef Rt(*Ft)();

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;
		(void)&k2;
		(void)&k3;
		(void)&k4;
		(void)&k5;
		(void)&k6;
		(void)&k7;
		(void)&k8;
		return ft();
	}
};

template<typename Rv,typename Rt,typename K1>
class FunctorImpl<Rv,Rt(*)(K1)> : public BindImplBase
{
public:

	typedef nil_t K2;
	typedef nil_t K3;
	typedef nil_t K4;
	typedef nil_t K5;
	typedef nil_t K6;
	typedef nil_t K7;
	typedef nil_t K8;

	typedef Rt return_type;
	typedef Rt(*Ft)(K1);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;
		(void)&k2;
		(void)&k3;
		(void)&k4;
		(void)&k5;
		(void)&k6;
		(void)&k7;
		(void)&k8;
		return ft(k1);
	}
};

template<typename Rv,typename Rt,typename Sd>
class FunctorImpl<Rv,Rt(Sd::*)()> : public BindImplBase
{
public:

	typedef Sd* K1;
	typedef nil_t K2;
	typedef nil_t K3;
	typedef nil_t K4;
	typedef nil_t K5;
	typedef nil_t K6;
	typedef nil_t K7;
	typedef nil_t K8;

	typedef Rt return_type;
	typedef Rt(Sd::*Ft)();

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;
		(void)&k2;
		(void)&k3;
		(void)&k4;
		(void)&k5;
		(void)&k6;
		(void)&k7;
		(void)&k8;
		return (k1->*ft)();

	}
};

template<typename Rv,typename Rt,typename K1,typename K2>
class FunctorImpl<Rv,Rt(*)(K1,K2)> : public BindImplBase
{
public:

	typedef nil_t K3;
	typedef nil_t K4;
	typedef nil_t K5;
	typedef nil_t K6;
	typedef nil_t K7;
	typedef nil_t K8;

	typedef Rt return_type;
	typedef Rt(*Ft)(K1,K2);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;
		(void)&k2;
		(void)&k3;
		(void)&k4;
		(void)&k5;
		(void)&k6;
		(void)&k7;
		(void)&k8;
		return ft(k1,k2);
	}

};


template<typename Rv,typename Rt,typename Sd,typename K2>
class FunctorImpl<Rv,Rt(Sd::*)(K2)> : public BindImplBase
{
public:

	typedef Sd* K1;
	typedef nil_t K3;
	typedef nil_t K4;
	typedef nil_t K5;
	typedef nil_t K6;
	typedef nil_t K7;
	typedef nil_t K8;

	typedef Rt return_type;
	typedef Rt(Sd::*Ft)(K2);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;
		(void)&k2;
		(void)&k3;
		(void)&k4;
		(void)&k5;
		(void)&k6;
		(void)&k7;
		(void)&k8;
		return (k1->*ft)(k2);
	}
};

template<typename Rv,typename Rt,typename K1,typename K2,typename K3>
class FunctorImpl<Rv,Rt(*)(K1,K2,K3)> : public BindImplBase
{
public:

	typedef nil_t K4;
	typedef nil_t K5;
	typedef nil_t K6;
	typedef nil_t K7;
	typedef nil_t K8;

	typedef Rt return_type;
	typedef Rt(*Ft)(K1,K2,K3);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;
		(void)&k2;
		(void)&k3;
		(void)&k4;
		(void)&k5;
		(void)&k6;
		(void)&k7;
		(void)&k8;
		return ft(k1,k2,k3);
	}

};


template<typename Rv,typename Rt,typename Sd,typename K2,typename K3>
class FunctorImpl<Rv,Rt(Sd::*)(K2,K3)> : public BindImplBase
{
public:

	typedef Sd* K1;
	typedef nil_t K4;
	typedef nil_t K5;
	typedef nil_t K6;
	typedef nil_t K7;
	typedef nil_t K8;

	typedef Rt return_type;
	typedef Rt(Sd::*Ft)(K2,K3);


	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;
		(void)&k2;
		(void)&k3;
		(void)&k4;
		(void)&k5;
		(void)&k6;
		(void)&k7;
		(void)&k8;
		return (k1->*ft)(k2,k3);
	}
};



template<typename Rv,typename Rt,typename K1,typename K2,typename K3,typename K4>
class FunctorImpl<Rv,Rt(*)(K1,K2,K3,K4)> : public BindImplBase
{
public:


	typedef nil_t K5;
	typedef nil_t K6;
	typedef nil_t K7;
	typedef nil_t K8;

	typedef Rt return_type;
	typedef Rt(*Ft)(K1,K2,K3,K4);


	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;
		(void)&k2;
		(void)&k3;
		(void)&k4;
		(void)&k5;
		(void)&k6;
		(void)&k7;
		(void)&k8;
		return ft(k1,k2,k3,k4);
	}

};


template<typename Rv,typename Rt,typename Sd,typename K2,typename K3,typename K4>
class FunctorImpl<Rv,Rt(Sd::*)(K2,K3,K4)> : public BindImplBase
{
public:

	typedef Sd* K1;

	typedef nil_t K5;
	typedef nil_t K6;
	typedef nil_t K7;
	typedef nil_t K8;

	typedef Rt return_type;
	typedef Rt(Sd::*Ft)(K2,K3,K4);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;
		(void)&k2;
		(void)&k3;
		(void)&k4;
		(void)&k5;
		(void)&k6;
		(void)&k7;
		(void)&k8;
		return (k1->*ft)(k2,k3,k4);
	}
};



template<typename Rv,typename Rt,typename K1,typename K2,typename K3,typename K4,typename K5>
class FunctorImpl<Rv,Rt(*)(K1,K2,K3,K4,K5)> : public BindImplBase
{
public:


	typedef nil_t K6;
	typedef nil_t K7;
	typedef nil_t K8;

	typedef Rt return_type;
	typedef Rt(*Ft)(K1,K2,K3,K4,K5);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;
		(void)&k2;
		(void)&k3;
		(void)&k4;
		(void)&k5;
		(void)&k6;
		(void)&k7;
		(void)&k8;
		return ft(k1,k2,k3,k4,k5);
	}

};


template<typename Rv,typename Rt,typename Sd,typename K2,typename K3,typename K4,typename K5>
class FunctorImpl<Rv,Rt(Sd::*)(K2,K3,K4,K5)> : public BindImplBase
{
public:

	typedef Sd* K1;

	typedef nil_t K6;
	typedef nil_t K7;
	typedef nil_t K8;

	typedef Rt return_type;
	typedef Rt(Sd::*Ft)(K2,K3,K4,K5);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;
		(void)&k2;
		(void)&k3;
		(void)&k4;
		(void)&k5;
		(void)&k6;
		(void)&k7;
		(void)&k8;
		return (k1->*ft)(k2,k3,k4,k5);
	}
};


template<typename Rv,typename Rt,typename K1,typename K2,typename K3,typename K4,typename K5,typename K6>
class FunctorImpl<Rv,Rt(*)(K1,K2,K3,K4,K5,K6)> : public BindImplBase
{
public:


	typedef nil_t K7;
	typedef nil_t K8;

	typedef Rt return_type;
	typedef Rt(*Ft)(K1,K2,K3,K4,K5,K6);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;
		(void)&k2;
		(void)&k3;
		(void)&k4;
		(void)&k5;
		(void)&k6;
		(void)&k7;
		(void)&k8;
		return ft(k1,k2,k3,k4,k5,k6);
	}

};


template<typename Rv,typename Rt,typename Sd,typename K2,typename K3,typename K4,typename K5,typename K6>
class FunctorImpl<Rv,Rt(Sd::*)(K2,K3,K4,K5,K6)> : public BindImplBase
{
public:

	typedef Sd* K1;

	typedef nil_t K7;
	typedef nil_t K8;

	typedef Rt return_type;
	typedef Rt(Sd::*Ft)(K2,K3,K4,K5,K6);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;
		(void)&k2;
		(void)&k3;
		(void)&k4;
		(void)&k5;
		(void)&k6;
		(void)&k7;
		(void)&k8;
		return (k1->*ft)(k2,k3,k4,k5,k6);
	}
};



template<typename Rv,typename Rt,typename K1,typename K2,typename K3,typename K4,typename K5,typename K6,typename K7>
class FunctorImpl<Rv,Rt(*)(K1,K2,K3,K4,K5,K6,K7)> : public BindImplBase
{
public:


	typedef nil_t K8;

	typedef Rt return_type;
	typedef Rt(*Ft)(K1,K2,K3,K4,K5,K6,K7);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;
		(void)&k2;
		(void)&k3;
		(void)&k4;
		(void)&k5;
		(void)&k6;
		(void)&k7;
		(void)&k8;
		return ft(k1,k2,k3,k4,k5,k6,k7);
	}

};


template<typename Rv,typename Rt,typename Sd,typename K2,typename K3,typename K4,typename K5,typename K6,typename K7>
class FunctorImpl<Rv,Rt(Sd::*)(K2,K3,K4,K5,K6,K7)> : public BindImplBase
{
public:

	typedef Sd* K1;

	typedef nil_t K8;

	typedef Rt return_type;
	typedef Rt(Sd::*Ft)(K2,K3,K4,K5,K6,K7);

	static inline Rt g(Ft ft,K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)
	{
		(void)&k1;
		(void)&k2;
		(void)&k3;
		(void)&k4;
		(void)&k5;
		(void)&k6;
		(void)&k7;
		(void)&k8;
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
		(void)&k1;
		(void)&k2;
		(void)&k3;
		(void)&k4;
		(void)&k5;
		(void)&k6;
		(void)&k7;
		(void)&k8;
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
		(void)&k1;
		(void)&k2;
		(void)&k3;
		(void)&k4;
		(void)&k5;
		(void)&k6;
		(void)&k7;
		(void)&k8;
		return (k1->*ft)(k2,k3,k4,k5,k6,k7,k8);
	}
};

template<typename K0,typename K1=nil_t,typename K2=nil_t,typename K3=nil_t,typename K4=nil_t,typename K5=nil_t,typename K6=nil_t,typename K7=nil_t,typename K8=nil_t>
class ParamDispatch
{
public:

	static inline K0 g(K0 k0,K1 k1=K1(),K2 k2=K2(),K3 k3=K3(),K4 k4=K4(),K5 k5=K5(),K6 k6=K6(),K7 k7=K7(),K8 k8=K8())
	{
		(void)&k0;
		(void)&k1;
		(void)&k2;
		(void)&k3;
		(void)&k4;
		(void)&k5;
		(void)&k6;
		(void)&k7;
		(void)&k8;
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
		return basetype::g(\
			tl::tuple_at<tuple_type,0>::g(data),\
			tl::tuple_at<tuple_type,1>::g(data),\
			tl::tuple_at<tuple_type,2>::g(data),\
			tl::tuple_at<tuple_type,3>::g(data),\
			tl::tuple_at<tuple_type,4>::g(data),\
			tl::tuple_at<tuple_type,5>::g(data),\
			tl::tuple_at<tuple_type,6>::g(data),\
			tl::tuple_at<tuple_type,7>::g(data),\
			tl::tuple_at<tuple_type,8>::g(data)\
		);\
	}\


#define BINDIMPL_OPERATOR1() BINDIMPL_OPERATOR0()\
	template<typename K1>\
	inline Rt operator()(K1 k1)\
	{\
		return basetype::g(tl::tuple_at<tuple_type,0>::g(data),\
						   ParamDispatch<P1,K1>::g(tl::tuple_at<tuple_type,1>::g(data),k1),\
						   ParamDispatch<P2,K1>::g(tl::tuple_at<tuple_type,2>::g(data),k1),\
						   ParamDispatch<P3,K1>::g(tl::tuple_at<tuple_type,3>::g(data),k1),\
						   ParamDispatch<P4,K1>::g(tl::tuple_at<tuple_type,4>::g(data),k1),\
						   ParamDispatch<P5,K1>::g(tl::tuple_at<tuple_type,5>::g(data),k1),\
						   ParamDispatch<P6,K1>::g(tl::tuple_at<tuple_type,6>::g(data),k1),\
						   ParamDispatch<P7,K1>::g(tl::tuple_at<tuple_type,7>::g(data),k1),\
						   ParamDispatch<P8,K1>::g(tl::tuple_at<tuple_type,8>::g(data),k1)\
						  );\
	}

#define BINDIMPL_OPERATOR2() BINDIMPL_OPERATOR1()\
	template<typename K1,typename K2>\
	inline Rt operator()(K1 k1,K2 k2)\
	{\
		return basetype::g(tl::tuple_at<tuple_type,0>::g(data),\
						   ParamDispatch<P1,K1,K2>::g(tl::tuple_at<tuple_type,1>::g(data),k1,k2),\
						   ParamDispatch<P2,K1,K2>::g(tl::tuple_at<tuple_type,2>::g(data),k1,k2),\
						   ParamDispatch<P3,K1,K2>::g(tl::tuple_at<tuple_type,3>::g(data),k1,k2),\
						   ParamDispatch<P4,K1,K2>::g(tl::tuple_at<tuple_type,4>::g(data),k1,k2),\
						   ParamDispatch<P5,K1,K2>::g(tl::tuple_at<tuple_type,5>::g(data),k1,k2),\
						   ParamDispatch<P6,K1,K2>::g(tl::tuple_at<tuple_type,6>::g(data),k1,k2),\
						   ParamDispatch<P7,K1,K2>::g(tl::tuple_at<tuple_type,7>::g(data),k1,k2),\
						   ParamDispatch<P8,K1,K2>::g(tl::tuple_at<tuple_type,8>::g(data),k1,k2)\
						  );\
	}

#define BINDIMPL_OPERATOR3() BINDIMPL_OPERATOR2()\
	template<typename K1,typename K2,typename K3>\
	inline Rt operator()(K1 k1,K2 k2,K3 k3)\
	{\
		return basetype::g(tl::tuple_at<tuple_type,0>::g(data),\
						   ParamDispatch<P1,K1,K2,K3>::g(tl::tuple_at<tuple_type,1>::g(data),k1,k2,k3),\
						   ParamDispatch<P2,K1,K2,K3>::g(tl::tuple_at<tuple_type,2>::g(data),k1,k2,k3),\
						   ParamDispatch<P3,K1,K2,K3>::g(tl::tuple_at<tuple_type,3>::g(data),k1,k2,k3),\
						   ParamDispatch<P4,K1,K2,K3>::g(tl::tuple_at<tuple_type,4>::g(data),k1,k2,k3),\
						   ParamDispatch<P5,K1,K2,K3>::g(tl::tuple_at<tuple_type,5>::g(data),k1,k2,k3),\
						   ParamDispatch<P6,K1,K2,K3>::g(tl::tuple_at<tuple_type,6>::g(data),k1,k2,k3),\
						   ParamDispatch<P7,K1,K2,K3>::g(tl::tuple_at<tuple_type,7>::g(data),k1,k2,k3),\
						   ParamDispatch<P8,K1,K2,K3>::g(tl::tuple_at<tuple_type,8>::g(data),k1,k2,k3)\
						  );\
	}

#define BINDIMPL_OPERATOR4() BINDIMPL_OPERATOR3()\
	template<typename K1,typename K2,typename K3,typename K4>\
	inline Rt operator()(K1 k1,K2 k2,K3 k3,K4 k4)\
	{\
		return basetype::g(tl::tuple_at<tuple_type,0>::g(data),\
						   ParamDispatch<P1,K1,K2,K3,K4>::g(tl::tuple_at<tuple_type,1>::g(data),k1,k2,k3,k4),\
						   ParamDispatch<P2,K1,K2,K3,K4>::g(tl::tuple_at<tuple_type,2>::g(data),k1,k2,k3,k4),\
						   ParamDispatch<P3,K1,K2,K3,K4>::g(tl::tuple_at<tuple_type,3>::g(data),k1,k2,k3,k4),\
						   ParamDispatch<P4,K1,K2,K3,K4>::g(tl::tuple_at<tuple_type,4>::g(data),k1,k2,k3,k4),\
						   ParamDispatch<P5,K1,K2,K3,K4>::g(tl::tuple_at<tuple_type,5>::g(data),k1,k2,k3,k4),\
						   ParamDispatch<P6,K1,K2,K3,K4>::g(tl::tuple_at<tuple_type,6>::g(data),k1,k2,k3,k4),\
						   ParamDispatch<P7,K1,K2,K3,K4>::g(tl::tuple_at<tuple_type,7>::g(data),k1,k2,k3,k4),\
						   ParamDispatch<P8,K1,K2,K3,K4>::g(tl::tuple_at<tuple_type,8>::g(data),k1,k2,k3,k4)\
						  );\
	}

#define BINDIMPL_OPERATOR5() BINDIMPL_OPERATOR4()\
	template<typename K1,typename K2,typename K3,typename K4,typename K5>\
	inline Rt operator()(K1 k1,K2 k2,K3 k3,K4 k4,K5 k5)\
	{\
		return basetype::g(tl::tuple_at<tuple_type,0>::g(data),\
						   ParamDispatch<P1,K1,K2,K3,K4,K5>::g(tl::tuple_at<tuple_type,1>::g(data),k1,k2,k3,k4,k5),\
						   ParamDispatch<P2,K1,K2,K3,K4,K5>::g(tl::tuple_at<tuple_type,2>::g(data),k1,k2,k3,k4,k5),\
						   ParamDispatch<P3,K1,K2,K3,K4,K5>::g(tl::tuple_at<tuple_type,3>::g(data),k1,k2,k3,k4,k5),\
						   ParamDispatch<P4,K1,K2,K3,K4,K5>::g(tl::tuple_at<tuple_type,4>::g(data),k1,k2,k3,k4,k5),\
						   ParamDispatch<P5,K1,K2,K3,K4,K5>::g(tl::tuple_at<tuple_type,5>::g(data),k1,k2,k3,k4,k5),\
						   ParamDispatch<P6,K1,K2,K3,K4,K5>::g(tl::tuple_at<tuple_type,6>::g(data),k1,k2,k3,k4,k5),\
						   ParamDispatch<P7,K1,K2,K3,K4,K5>::g(tl::tuple_at<tuple_type,7>::g(data),k1,k2,k3,k4,k5),\
						   ParamDispatch<P8,K1,K2,K3,K4,K5>::g(tl::tuple_at<tuple_type,8>::g(data),k1,k2,k3,k4,k5)\
						  );\
	}

#define BINDIMPL_OPERATOR6() BINDIMPL_OPERATOR5()\
	template<typename K1,typename K2,typename K3,typename K4,typename K5,typename K6>\
	inline Rt operator()(K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6)\
	{\
		return basetype::g(tl::tuple_at<tuple_type,0>::g(data),\
						   ParamDispatch<P1,K1,K2,K3,K4,K5,K6>::g(tl::tuple_at<tuple_type,1>::g(data),k1,k2,k3,k4,k5,k6),\
						   ParamDispatch<P2,K1,K2,K3,K4,K5,K6>::g(tl::tuple_at<tuple_type,2>::g(data),k1,k2,k3,k4,k5,k6),\
						   ParamDispatch<P3,K1,K2,K3,K4,K5,K6>::g(tl::tuple_at<tuple_type,3>::g(data),k1,k2,k3,k4,k5,k6),\
						   ParamDispatch<P4,K1,K2,K3,K4,K5,K6>::g(tl::tuple_at<tuple_type,4>::g(data),k1,k2,k3,k4,k5,k6),\
						   ParamDispatch<P5,K1,K2,K3,K4,K5,K6>::g(tl::tuple_at<tuple_type,5>::g(data),k1,k2,k3,k4,k5,k6),\
						   ParamDispatch<P6,K1,K2,K3,K4,K5,K6>::g(tl::tuple_at<tuple_type,6>::g(data),k1,k2,k3,k4,k5,k6),\
						   ParamDispatch<P7,K1,K2,K3,K4,K5,K6>::g(tl::tuple_at<tuple_type,7>::g(data),k1,k2,k3,k4,k5,k6),\
						   ParamDispatch<P8,K1,K2,K3,K4,K5,K6>::g(tl::tuple_at<tuple_type,8>::g(data),k1,k2,k3,k4,k5,k6)\
						  );\
	}

#define BINDIMPL_OPERATOR7() BINDIMPL_OPERATOR6()\
	template<typename K1,typename K2,typename K3,typename K4,typename K5,typename K6,typename K7>\
	inline Rt operator()(K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7)\
	{\
		return basetype::g(tl::tuple_at<tuple_type,0>::g(data),\
						   ParamDispatch<P1,K1,K2,K3,K4,K5,K6,K7>::g(tl::tuple_at<tuple_type,1>::g(data),k1,k2,k3,k4,k5,k6,k7),\
						   ParamDispatch<P2,K1,K2,K3,K4,K5,K6,K7>::g(tl::tuple_at<tuple_type,2>::g(data),k1,k2,k3,k4,k5,k6,k7),\
						   ParamDispatch<P3,K1,K2,K3,K4,K5,K6,K7>::g(tl::tuple_at<tuple_type,3>::g(data),k1,k2,k3,k4,k5,k6,k7),\
						   ParamDispatch<P4,K1,K2,K3,K4,K5,K6,K7>::g(tl::tuple_at<tuple_type,4>::g(data),k1,k2,k3,k4,k5,k6,k7),\
						   ParamDispatch<P5,K1,K2,K3,K4,K5,K6,K7>::g(tl::tuple_at<tuple_type,5>::g(data),k1,k2,k3,k4,k5,k6,k7),\
						   ParamDispatch<P6,K1,K2,K3,K4,K5,K6,K7>::g(tl::tuple_at<tuple_type,6>::g(data),k1,k2,k3,k4,k5,k6,k7),\
						   ParamDispatch<P7,K1,K2,K3,K4,K5,K6,K7>::g(tl::tuple_at<tuple_type,7>::g(data),k1,k2,k3,k4,k5,k6,k7),\
						   ParamDispatch<P8,K1,K2,K3,K4,K5,K6,K7>::g(tl::tuple_at<tuple_type,8>::g(data),k1,k2,k3,k4,k5,k6,k7)\
						  );\
	}

#define BINDIMPL_OPERATOR8() BINDIMPL_OPERATOR7()\
	template<typename K1,typename K2,typename K3,typename K4,typename K5,typename K6,typename K7,typename K8>\
	inline Rt operator()(K1 k1,K2 k2,K3 k3,K4 k4,K5 k5,K6 k6,K7 k7,K8 k8)\
	{\
		return basetype::g(tl::tuple_at<tuple_type,0>::g(data),\
						   ParamDispatch<P1,K1,K2,K3,K4,K5,K6,K7,K8>::g(tl::tuple_at<tuple_type,1>::g(data),k1,k2,k3,k4,k5,k6,k7,k8),\
						   ParamDispatch<P2,K1,K2,K3,K4,K5,K6,K7,K8>::g(tl::tuple_at<tuple_type,2>::g(data),k1,k2,k3,k4,k5,k6,k7,k8),\
						   ParamDispatch<P3,K1,K2,K3,K4,K5,K6,K7,K8>::g(tl::tuple_at<tuple_type,3>::g(data),k1,k2,k3,k4,k5,k6,k7,k8),\
						   ParamDispatch<P4,K1,K2,K3,K4,K5,K6,K7,K8>::g(tl::tuple_at<tuple_type,4>::g(data),k1,k2,k3,k4,k5,k6,k7,k8),\
						   ParamDispatch<P5,K1,K2,K3,K4,K5,K6,K7,K8>::g(tl::tuple_at<tuple_type,5>::g(data),k1,k2,k3,k4,k5,k6,k7,k8),\
						   ParamDispatch<P6,K1,K2,K3,K4,K5,K6,K7,K8>::g(tl::tuple_at<tuple_type,6>::g(data),k1,k2,k3,k4,k5,k6,k7,k8),\
						   ParamDispatch<P7,K1,K2,K3,K4,K5,K6,K7,K8>::g(tl::tuple_at<tuple_type,7>::g(data),k1,k2,k3,k4,k5,k6,k7,k8),\
						   ParamDispatch<P8,K1,K2,K3,K4,K5,K6,K7,K8>::g(tl::tuple_at<tuple_type,8>::g(data),k1,k2,k3,k4,k5,k6,k7,k8)\
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

	typedef tl::tuple<P0> tuple_type;
	tuple_type data;


	BindImpl(
		P0 p0_
	)
		:data(p0_)
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


	typedef tl::tuple<P0,P1,P2,P3,P4,P5,P6,P7,P8> tuple_type;
	tuple_type data;

	BindImpl(
		P0 p0_,P1 p1_
	)
		:data(p0_,p1_)
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


	typedef tl::tuple<P0,P1,P2,P3,P4,P5,P6,P7,P8> tuple_type;
	tuple_type data;

	BindImpl(
		P0 p0_,P1 p1_,P2 p2_
	)
		:data(p0_,p1_,p2_)
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

	typedef tl::tuple<P0,P1,P2,P3,P4,P5,P6,P7,P8> tuple_type;
	tuple_type data;

	BindImpl(
		P0 p0_,P1 p1_,P2 p2_,P3 p3_
	)
		:data(p0_,p1_,p2_,p3_)
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


	typedef tl::tuple<P0,P1,P2,P3,P4,P5,P6,P7,P8> tuple_type;
	tuple_type data;


	BindImpl(
		P0 p0_,P1 p1_,P2 p2_,P3 p3_,P4 p4_
	)
		:data(p0_,p1_,p2_,p3_,p4_)
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

	typedef tl::tuple<P0,P1,P2,P3,P4,P5,P6,P7,P8> tuple_type;
	tuple_type data;

	BindImpl(
		P0 p0_,P1 p1_,P2 p2_,P3 p3_,P4 p4_,P5 p5_
	)
		:data(p0_,p1_,p2_,p3_,p4_,p5_)
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


	typedef tl::tuple<P0,P1,P2,P3,P4,P5,P6,P7,P8> tuple_type;
	tuple_type data;

	BindImpl(
		P0 p0_,P1 p1_,P2 p2_,P3 p3_,P4 p4_,P5 p5_,P6 p6_
	)
		:data(p0_,p1_,p2_,p3_,p4_,p5_,p6_)
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


	typedef tl::tuple<P0,P1,P2,P3,P4,P5,P6,P7,P8> tuple_type;
	tuple_type data;

	BindImpl(
		P0 p0_,P1 p1_,P2 p2_,P3 p3_,P4 p4_,P5 p5_,P6 p6_,P7 p7_
	)
		:data(p0_,p1_,p2_,p3_,p4_,p5_,p6_,p7_)
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

	typedef tl::tuple<P0,P1,P2,P3,P4,P5,P6,P7,P8> tuple_type;
	tuple_type data;

	BindImpl(
		P0 p0_,P1 p1_,P2 p2_,P3 p3_,P4 p4_,P5 p5_,P6 p6_,P7 p7_,P8 p8_
	)
		:data(p0_,p1_,p2_,p3_,p4_,p5_,p6_,p7_,p8_)
	{

	}


	BINDIMPL_OPERATOR8();

};


VHWD_LEAVE

