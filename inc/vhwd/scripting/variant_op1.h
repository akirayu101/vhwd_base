#ifndef __H_VHWD_SCRIPTING_VARIANT_OP1__
#define __H_VHWD_SCRIPTING_VARIANT_OP1__

#include "vhwd/scripting/variant_op.h"
#include "vhwd/scripting/executor.h"

VHWD_ENTER


template<typename P>
class kvar_handler1
{
public:
	typedef typename P::type type;

	static inline type g(const Variant& v)
	{
		return g((Variant&)v);
	}

	static inline type g(Variant& v)
	{
		switch(v.flag())
		{
		case 0: return P::template rebind<0>::g(v);
		case 1: return P::template rebind<1>::g(v);
		case 2: return P::template rebind<2>::g(v);
		case 3: return P::template rebind<3>::g(v);
		case 4: return P::template rebind<4>::g(v);
		case 5: return P::template rebind<5>::g(v);
		case 6: return P::template rebind<6>::g(v);
		case 7: return P::template rebind<7>::g(v);
		case 8: return P::template rebind<8>::g(v);
		case 9: return P::template rebind<9>::g(v);
		case 10: return P::template rebind<10>::g(v);
		case 11: return P::template rebind<11>::g(v);
		case 12: return P::template rebind<12>::g(v);
		case 13: return P::template rebind<13>::g(v);
		case 14: return P::template rebind<14>::g(v);
		case 15: return P::template rebind<15>::g(v);
		default: return P::invalid_type();
		};
	}


	template<typename T>
	static inline type g1(T& v,int f)
	{
		switch(f)
		{
		case 0: return P::template rebind<0>::g(v);
		case 1: return P::template rebind<1>::g(v);
		case 2: return P::template rebind<2>::g(v);
		case 3: return P::template rebind<3>::g(v);
		case 4: return P::template rebind<4>::g(v);
		case 5: return P::template rebind<5>::g(v);
		case 6: return P::template rebind<6>::g(v);
		case 7: return P::template rebind<7>::g(v);
		case 8: return P::template rebind<8>::g(v);
		case 9: return P::template rebind<9>::g(v);
		case 10: return P::template rebind<10>::g(v);
		case 11: return P::template rebind<11>::g(v);
		case 12: return P::template rebind<12>::g(v);
		case 13: return P::template rebind<13>::g(v);
		case 14: return P::template rebind<14>::g(v);
		case 15: return P::template rebind<15>::g(v);
		default: return P::invalid_type();
		};
	}


	template<typename T1,typename T2>
	static inline type g2(T1& v1,T2& v2,int f)
	{
		switch(f)
		{
		case 0: return P::template rebind<0>::g(v1,v2);
		case 1: return P::template rebind<1>::g(v1,v2);
		case 2: return P::template rebind<2>::g(v1,v2);
		case 3: return P::template rebind<3>::g(v1,v2);
		case 4: return P::template rebind<4>::g(v1,v2);
		case 5: return P::template rebind<5>::g(v1,v2);
		case 6: return P::template rebind<6>::g(v1,v2);
		case 7: return P::template rebind<7>::g(v1,v2);
		case 8: return P::template rebind<8>::g(v1,v2);
		case 9: return P::template rebind<9>::g(v1,v2);
		case 10: return P::template rebind<10>::g(v1,v2);
		case 11: return P::template rebind<11>::g(v1,v2);
		case 12: return P::template rebind<12>::g(v1,v2);
		case 13: return P::template rebind<13>::g(v1,v2);
		case 14: return P::template rebind<14>::g(v1,v2);
		case 15: return P::template rebind<15>::g(v1,v2);
		default: return P::invalid_type();
		};
	}
};


template<typename T>
class kvar_policy_cast
{
public:
	typedef T type;

	static inline type invalid_type()
	{
		Exception::XBadCast();
		return type();
	}

	template<typename X>
	static inline X& k(X& v)
	{
		return v;
	}

	static inline CallableData* k(const CallableData*& v)
	{
		return v;
	}

	template<int N>
	class rebind
	{
	public:
		static inline type g(Variant& v)
		{
			return kvar_cast<type>::g(k(kvar_type<N>::raw((kvar_base&)(v))));
		}
	};
};


template<typename T>
class PLCast : public kvar_cast<T>
{
public:
	typedef kvar_policy_cast<T> P;
	typedef typename P::type type;
	using kvar_cast<T>::g;

	static inline type g(Variant& v)
	{
		return kvar_handler1<P>::g(v);
	}
};



template<typename P>
class PL1_arr
{
public:

	template<typename X>
	static inline void g(Variant& z,const X& x)
	{
		typedef typename P::template rebind<X>::type R;
		z.reset<R>(P::g(x));
	}

	template<typename X>
	static inline void g(Variant& z,const arr_xt<X>& x)
	{
		typedef typename P::template rebind<X>::type R;
		arr_xt<R> r;r.resize(x.size(0),x.size(1),x.size(2),x.size(3),x.size(4),x.size(5));
		for(size_t i=0;i<x.size();i++)
		{
			r(i)=P::g(x(i));
		}
		kvar_flag<arr_xt<R> >::unique(z).swap(r);
	}

};



template<typename X,typename P,bool V>
class PL1_sel
{
public:
	static inline void g(Executor&)
	{
		Exception::XBadCast();
	}
};

template<typename X,typename P>
class PL1_sel<X,P,true>
{
public:
	static inline void g(Executor& ks)
	{
		Variant& lhs(ks.get(0));
		PL1_arr<P>::g(lhs,kvar_flag<X>::raw(lhs));
	}
};

template<typename P>
class PL1_opx_num
{
public:
	typedef void type;

	static inline type invalid_type()
	{
		Exception::XBadCast();
		return type();
	}

	template<int N1>
	class rebind : public PL1_sel<typename kvar_type<N1>::type,P,kvar_opx_num<N1,0>::valid>{};

};

class PL1_sin : public PL1_num2
{
public:
	static inline double g(double x)
	{
		return ::sin(x);
	}
	static inline std::complex<double> g(const std::complex<double>& x)
	{
		return std::sin(x);
	}
};

class PL1_cos : public PL1_num2
{
public:
	static inline double g(double x)
	{
		return ::cos(x);
	}

	static inline std::complex<double> g(const std::complex<double>& x)
	{
		return std::cos(x);
	}
};

class PL1_tan : public PL1_num2
{
public:
	static inline double g(double x)
	{
		return ::tan(x);
	}

	static inline std::complex<double> g(const std::complex<double>& x)
	{
		return std::tan(x);
	}
};

class PL1_log : public PL1_num2
{
public:
	static inline double g(double x)
	{
		return ::log(x);
	}

	static inline std::complex<double> g(const std::complex<double>& x)
	{
		return std::log(x);
	}
};


class PL1_log10 : public PL1_num2
{
public:
	static inline double g(double x)
	{
		return ::log10(x);
	}

	static inline std::complex<double> g(const std::complex<double>& x)
	{
		return std::log10(x);
	}
};


class PL1_inv : public PL1_num1
{
public:

	static inline int64_t g(bool x)
	{
		return x?-1:0;
	}

	static inline int64_t g(int64_t x)
	{
		return -x;
	}

	static inline double g(double x)
	{
		return -x;
	}

	static inline std::complex<double> g(const std::complex<double>& x)
	{
		return -x;
	}
};

class PL1_not
{
public:

	template<typename T>
	class rebind{public:typedef bool type;};

	template<typename T>
	static bool g(T x)
	{
		return !(bool)x;
	}

	static bool g(double x)
	{
		return x==0.0;
	}

	static bool g(int64_t x)
	{
		return x==0;
	}

	static bool g(const std::complex<double>& x)
	{
		return x.real()==0&&x.imag()==0;
	}
};

class PL1_bw_inv
{
public:
	template<typename X>
	class rebind{public:typedef int64_t type;};

	template<typename X>
	static int64_t g(const X& x)
	{
		return ~PLCast<int64_t>::g(x);
	}
};

template<typename T>
class PL1_rebind_no_complex : public PL1_num1::rebind<T>
{
public:

};

template<typename T>
class PL1_rebind_no_complex<std::complex<T> >
{
public:
	typedef T type;

};


class PL1_scr
{
public:
	template<typename X>
	class rebind : public PL1_rebind_no_complex<X>{};
};

class PL1_conj
{
public:
	template<typename X>
	class rebind{public:typedef X type;};

	template<typename T>
	static T g(const T& x){return x;}

	template<typename T>
	static std::complex<T> g(const std::complex<T>& x){return std::complex<T>(x.real(),-x.imag());}

};

class PL1_real : public PL1_scr
{
public:
	template<typename T>
	static typename rebind<T>::type g(T v)
	{
		return v;
	}

	template<typename T>
	static typename rebind<T>::type g(const std::complex<T>& v)
	{
		return v.real();
	}

};

class PL1_imag : public PL1_scr
{
public:
	template<typename T>
	static typename rebind<T>::type g(T v)
	{
		return T(0);
	}

	template<typename T>
	static typename rebind<T>::type g(const std::complex<T>& v)
	{
		return v.imag();
	}

};

class PL1_abs : public PL1_scr
{
public:

	static int64_t k(bool v){return v?-1:0;}

	template<typename T>
	static T k(T v){return v<0?-v:v;}

	template<typename T>
	static T k(const std::complex<T>& v){return ::sqrt(v.real()*v.real()+v.imag()*v.imag());}

	template<typename T>
	static typename rebind<T>::type g(T v)
	{
		return k(v);
	}
};


class PL1_sqrt : public PL1_num1
{
public:

	template<typename T>
	static typename rebind<T>::type k(T v)
	{
		return ::sqrt((double)v);
	}

	template<typename T>
	static std::complex<T> k(const std::complex<T>& v)
	{
		return std::sqrt(v);
	}

	template<typename T>
	static typename rebind<T>::type g(T v)
	{
		return k(v);
	}
};



template<typename T>
T Executor::eval(const String &s)
{
	if(!execute("ans=("+s+")")){Exception::XError("eval failed");}
	return PLCast<T>::g(table["ans"]);
}

template<typename T>
bool Executor::eval(const String& s,T& v)
{
	if(!execute("ans=("+s+")")){return false;}
	try
	{
		v=PLCast<T>::g(table["ans"]);
		return true;
	}
	catch(...)
	{
		return false;
	}
}

inline bool Executor::test()
{
	return PLCast<bool>::g(stack[stack.nsp--]);
}

VHWD_LEAVE
#endif
