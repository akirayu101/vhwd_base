#ifndef __H_VHWD_FACTOR__
#define __H_VHWD_FACTOR__

#include "vhwd/basic/exception.h"
#include "vhwd/basic/pointer.h"
#include "vhwd/basic/factor_detail.h"

VHWD_ENTER

extern VHWD_DLLIMPEXP tl::int2type<1> _1;
extern VHWD_DLLIMPEXP tl::int2type<2> _2;
extern VHWD_DLLIMPEXP tl::int2type<3> _3;
extern VHWD_DLLIMPEXP tl::int2type<4> _4;
extern VHWD_DLLIMPEXP tl::int2type<5> _5;
extern VHWD_DLLIMPEXP tl::int2type<6> _6;

//Bind functions, return type must be provided explicitly.
//hbind<return_type>::g(function,parameter1,parameter2,...,parametern);
//sample usage:
//double fma(double,double,double);
//auto f0=hbind<double>::g(fma,1.0,2.0,3.0); f0();
//auto f1=hbind<double>::g(fma,1.0,_1,3.0); f1(2.0);
//auto f2=hbind<double>::g(fma,1.0,_2,_1); f2(3.0,2.0);
//auto f3=hbind<double>::g(fma,_2,_3,_1); f3(3.0,1.0,2.0);
//auto f4=hbind<void>::g(f2,3.0,2.0); f4();

// Factor f0 to f4 all call fma(1.0,2.0,3.0);

template<typename Rt>
class hbind
{
public:

	template<typename P0>
	static FBind<Rt(P0)> g(P0 p0)
	{
		return FBind<Rt(P0)>(p0);
	}

	template<typename P0,typename P1>
	static FBind<Rt(P0,P1)> g(P0 p0,P1 p1)
	{
		return FBind<Rt(P0,P1)>(p0,p1);
	}

	template<typename P0,typename P1,typename P2>
	static FBind<Rt(P0,P1,P2)> g(P0 p0,P1 p1,P2 p2)
	{
		return FBind<Rt(P0,P1,P2)>(p0,p1,p2);
	}

	template<typename P0,typename P1,typename P2,typename P3>
	static FBind<Rt(P0,P1,P2,P3)> g(P0 p0,P1 p1,P2 p2,P3 p3)
	{
		return FBind<Rt(P0,P1,P2,P3)>(p0,p1,p2,p3);
	}

	template<typename P0,typename P1,typename P2,typename P3,typename P4>
	static FBind<Rt(P0,P1,P2,P3,P4)> g(P0 p0,P1 p1,P2 p2,P3 p3,P4 p4)
	{
		return FBind<Rt(P0,P1,P2,P3,P4)>(p0,p1,p2,p3,p4);
	}

	template<typename P0,typename P1,typename P2,typename P3,typename P4,typename P5>
	static FBind<Rt(P0,P1,P2,P3,P4,P5)> g(P0 p0,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5)
	{
		return FBind<Rt(P0,P1,P2,P3,P4,P5)>(p0,p1,p2,p3,p4,p5);
	}
};


template<typename T>
class Factor;


template<typename T>
class Factor<T()>
{
public:

	class Invoker: public ObjectData
	{
		const Invoker& operator=(const Invoker&);
	public:
		virtual T Invoke()=0;
	};

	void reset(Invoker* p){invoker.reset(p);}
	void clear(){invoker.reset(NULL);}

	template<typename P0>
	void reset(P0 p0)
	{
		class KInvoker : public Invoker
		{
		public:
			FBind<T(P0)> impl;
			KInvoker(P0 p0):impl(p0){}
			virtual T Invoke(){return impl();}
		};
		invoker.reset(new KInvoker(p0));
	}

	template<typename P0,typename P1>
	void reset(P0 p0,P1 p1)
	{
		class KInvoker : public Invoker
		{
		public:
			FBind<T(P0,P1)> impl;
			KInvoker(P0 p0,P1 p1):impl(p0,p1){}
			virtual T Invoke(){return impl();}
		};
		invoker.reset(new KInvoker(p0,p1));
	}

	template<typename P0,typename P1,typename P2>
	void reset(P0 p0,P1 p1,P2 p2)
	{
		class KInvoker : public Invoker
		{
		public:
			FBind<T(P0,P1,P2)> impl;
			KInvoker(P0 p0,P1 p1,P2 p2):impl(p0,p1,p2){}
			virtual T Invoke(){return impl();}
		};
		invoker.reset(new KInvoker(p0,p1,p2));
	}

	template<typename P0,typename P1,typename P2,typename P3>
	void reset(P0 p0,P1 p1,P2 p2, P3 p3)
	{
		class KInvoker : public Invoker
		{
		public:
			FBind<T(P0,P1,P2,P3)> impl;
			KInvoker(P0 p0,P1 p1,P2 p2,P3 p3):impl(p0,p1,p2,p3){}
			virtual T Invoke(){return impl();}
		};
		invoker.reset(new KInvoker(p0,p1,p2,p3));
	}

	template<typename P0,typename P1,typename P2,typename P3,typename P4>
	void reset(P0 p0,P1 p1,P2 p2,P3 p3,P4 p4)
	{
		class KInvoker : public Invoker
		{
		public:
			FBind<T(P0,P1,P2,P3,P4)> impl;
			KInvoker(P0 p0,P1 p1,P2 p2,P3 p3,P4 p4):impl(p0,p1,p2,p3,p4){}
			virtual T Invoke(){return impl();}
		};
		invoker.reset(new KInvoker(p0,p1,p2,p3,p4));
	}

	template<typename P0,typename P1,typename P2,typename P3,typename P4,typename P5>
	void reset(P0 p0,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5)
	{
		class KInvoker : public Invoker
		{
		public:
			FBind<T(P0,P1,P2,P3,P4,P5)> impl;
			KInvoker(P0 p0,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5):impl(p0,p1,p2,p3,p4,p5){}
			virtual T Invoke(){return impl();}
		};
		invoker.reset(new KInvoker(p0,p1,p2,p3,p4,p5));
	}

	inline T operator()(){if(!invoker) Exception::XBadFactor();return invoker->Invoke();}
	inline operator bool() const{return invoker.get()!=NULL;}

private:
	DataPtrT<Invoker> invoker;
};

template<typename T,typename X>
class Factor<T(X)>
{
public:

	class Invoker: public ObjectData
	{
		const Invoker& operator=(const Invoker&);
	public:
		virtual T Invoke(X)=0;
	};

	void reset(Invoker* p){invoker.reset(p);}
	void clear(){invoker.reset(NULL);}

	template<typename P0,typename P1>
	void reset(P0 p0,P1 p1)
	{
		class KInvoker : public Invoker
		{
		public:
			FBind<T(P0,P1)> impl;
			KInvoker(P0 p0,P1 p1):impl(p0,p1){}
			virtual T Invoke(X x){return impl(x);}
		};
		invoker.reset(new KInvoker(p0,p1));
	}

	template<typename P0,typename P1,typename P2>
	void reset(P0 p0,P1 p1,P2 p2)
	{
		class KInvoker : public Invoker
		{
		public:
			FBind<T(P0,P1,P2)> impl;
			KInvoker(P0 p0,P1 p1,P2 p2):impl(p0,p1,p2){}
			virtual T Invoke(X x){return impl(x);}
		};
		invoker.reset(new KInvoker(p0,p1,p2));
	}

	template<typename P0,typename P1,typename P2,typename P3>
	void reset(P0 p0,P1 p1,P2 p2,P3 p3)
	{
		class KInvoker : public Invoker
		{
		public:
			FBind<T(P0,P1,P2,P3)> impl;
			KInvoker(P0 p0,P1 p1,P2 p2,P3 p3):impl(p0,p1,p2,p3){}
			virtual T Invoke(X x){return impl(x);}
		};
		invoker.reset(new KInvoker(p0,p1,p2,p3));
	}

	template<typename P0,typename P1,typename P2,typename P3,typename P4>
	void reset(P0 p0,P1 p1,P2 p2,P3 p3,P4 p4)
	{
		class KInvoker : public Invoker
		{
		public:
			FBind<T(P0,P1,P2,P3,P4)> impl;
			KInvoker(P0 p0,P1 p1,P2 p2,P3 p3,P4 p4):impl(p0,p1,p2,p3,p4){}
			virtual T Invoke(X x){return impl(x);}
		};
		invoker.reset(new KInvoker(p0,p1,p2,p3,p4));
	}

	template<typename P0,typename P1,typename P2,typename P3,typename P4,typename P5>
	void reset(P0 p0,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5)
	{
		class KInvoker : public Invoker
		{
		public:
			FBind<T(P0,P1,P2,P3,P4,P5)> impl;
			KInvoker(P0 p0,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5):impl(p0,p1,p2,p3,p4,p5){}
			virtual T Invoke(X x){return impl(x);}
		};
		invoker.reset(new KInvoker(p0,p1,p2,p3,p4,p5));
	}

	inline T operator()(X x){if(!invoker) Exception::XBadFactor();return invoker->Invoke(x);}
	inline operator bool() const{return invoker.get()!=NULL;}

private:
	DataPtrT<Invoker> invoker;
};


template<typename T,typename X,typename Y>
class Factor<T(X,Y)>
{
public:

	class Invoker: public ObjectData
	{
		const Invoker& operator=(const Invoker&);
	public:
		virtual T Invoke(X,Y)=0;
	};

	void reset(Invoker* p){invoker.reset(p);}
	void clear(){invoker.reset(NULL);}


	template<typename P0,typename P1,typename P2>
	void reset(P0 p0,P1 p1,P2 p2)
	{
		class KInvoker : public Invoker
		{
		public:
			FBind<T(P0,P1,P2)> impl;
			KInvoker(P0 p0,P1 p1,P2 p2):impl(p0,p1,p2){}
			virtual T Invoke(X x,Y y){return impl(x,y);}
		};
		invoker.reset(new KInvoker(p0,p1,p2));
	}

	template<typename P0,typename P1,typename P2,typename P3>
	void reset(P0 p0,P1 p1,P2 p2,P3 p3)
	{
		class KInvoker : public Invoker
		{
		public:
			FBind<T(P0,P1,P2,P3)> impl;
			KInvoker(P0 p0,P1 p1,P2 p2,P3 p3):impl(p0,p1,p2,p3){}
			virtual T Invoke(X x,Y y){return impl(x,y);}
		};
		invoker.reset(new KInvoker(p0,p1,p2,p3));
	}

	template<typename P0,typename P1,typename P2,typename P3,typename P4>
	void reset(P0 p0,P1 p1,P2 p2,P3 p3,P4 p4)
	{
		class KInvoker : public Invoker
		{
		public:
			FBind<T(P0,P1,P2,P3,P4)> impl;
			KInvoker(P0 p0,P1 p1,P2 p2,P3 p3,P4 p4):impl(p0,p1,p2,p3,p4){}
			virtual T Invoke(X x,Y y){return impl(x,y);}
		};
		invoker.reset(new KInvoker(p0,p1,p2,p3,p4));
	}


	template<typename P0,typename P1,typename P2,typename P3,typename P4,typename P5>
	void reset(P0 p0,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5)
	{
		class KInvoker : public Invoker
		{
		public:
			FBind<T(P0,P1,P2,P3,P4,P5)> impl;
			KInvoker(P0 p0,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5):impl(p0,p1,p2,p3,p4,p5){}
			virtual T Invoke(X x,Y y){return impl(x,y);}
		};
		invoker.reset(new KInvoker(p0,p1,p2,p3,p4,p5));
	}

	inline T operator()(X x,Y y){if(!invoker) Exception::XBadFactor();return invoker->Invoke(x,y);}
	inline operator bool() const{return invoker.get()!=NULL;}

private:
	DataPtrT<Invoker> invoker;
};

VHWD_LEAVE

#endif
