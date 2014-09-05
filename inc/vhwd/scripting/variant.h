#ifndef __H_VHWD_SCRIPTING_VARIANT__
#define __H_VHWD_SCRIPTING_VARIANT__

#include "vhwd/basic.h"
#include "vhwd/collection.h"
#include <complex>

VHWD_ENTER

template<typename T> class hash_t<std::complex<T> > : public hash_pod<std::complex<T> > {};

template<typename X,typename Y> class hash_t<std::pair<X,Y> >
{
public:
	uint32_t operator()(const std::pair<X,Y> & o)
	{
		hash_t<X> h1;hash_t<Y> h2;
		return h1(o.first)^h2(o.second);
	}
};


class VHWD_DLLIMPEXP Executor;
class VHWD_DLLIMPEXP CallableData;
class VHWD_DLLIMPEXP CallableClass;
class VHWD_DLLIMPEXP CallableMetatable;
class VHWD_DLLIMPEXP CallableFunction;
class VHWD_DLLIMPEXP CallableValue;
class VHWD_DLLIMPEXP CallableIterator;

template<typename T> class VHWD_DLLIMPEXP CallableValueT;


class VHWD_DLLIMPEXP kvar_base
{
public:
	union
	{
		CallableData* pVal;
		int64_t iVal;
		double fVal;
		bool bVal;;
		char _i8val[8];
	};

	char _pad[4];
	int32_t flag;

	bool bptr() const{return flag>2;}
	void clear();
};

class VHWD_DLLIMPEXP Variant
{
public:

	enum
	{
		V_BOOL		=0,
		V_INT		=1,
		V_DOUBLE	=2,
		V_COMPLEX	=3,
		V_COMPOUND	=4,
		V_STRING	=5,
		V_TABLE		=6,
		V_OBJECT	=7,		// flag=V_OBJECT and pVal=0 means nil
		V_SCRMASK	=7,
		V_ARRMASK	=8,
		V_TYPEMASK	=15,
		V_FLAGMOVE	=4,

	};

	inline Variant(){data.flag=V_OBJECT;data.pVal=NULL;}
	Variant(const Variant&o);
	Variant& operator=(const Variant& o);

	~Variant(){clear();}

	void clear();

	template<typename T>
	void reset(const T& value);

	template<typename T>
	void reset();

	template<typename T>
	bool get(T& value);

	template<typename T>
	bool set(const T& value);

	template<typename T>
	T& ref();	// set type to T if not T, return reference of internal value

	template<typename T>
	T* ptr();	// return NULL if not T


	bool bptr() const{return data.flag>Variant::V_DOUBLE;}

	CallableData* kptr();
	void kptr(CallableData* p);
	const CallableData* kptr() const;

	void swap(Variant& o);

	int flag() const{return data.flag;}

	operator kvar_base&(){return data;}
	operator const kvar_base&() const {return data;}

	bool operator==(const Variant& v2) const;
	bool operator!=(const Variant& v2) const;

//protected:
	kvar_base data;
};



class kvar_table : public indexer_map<String,Variant>
{
public:
	bool operator==(const  kvar_table& v2) const;
	bool operator!=(const  kvar_table& v2) const;
};


template<>
class hash_t<kvar_table>
{
public:
	uint32_t operator()(const kvar_table& o);
};




class VHWD_DLLIMPEXP CallableData : public ObjectData
{
public:

	virtual int __getindex(Executor&,const String&); // top.s
	virtual int __setindex(Executor&,const String&); // top.s=val
	virtual int __getarray(Executor&); // top[s]
	virtual int __setarray(Executor&); // top[s]=val
	virtual int __fun_call(Executor&); // top(...)
	virtual int __new_item(Executor&); // new top(...)

	virtual uint32_t hashcode() const
	{
		hash_pod<void*> h;return h((void*)this);;
	}


	virtual bool IsEqualTo(const CallableData* d) const {return this==d;}

	virtual CallableValueT<kvar_table>* ToTable() {return NULL;}
	virtual CallableClass* ToClass() {return NULL;}
	virtual CallableMetatable* GetMetaTable(){return NULL;}
	virtual CallableIterator* GetIterator(int);

	virtual String GetType() const {return "object";}


	virtual bool ToValue(String& s) const;
	virtual bool ToValue(int64_t&) const;
	virtual bool ToValue(double&) const;

	bool ToValue(int32_t& v) const;
	bool ToValue(float& v) const;
	bool ToValue(size_t& v) const;


};

class VHWD_DLLIMPEXP CallableIterator : public CallableData
{
public:
	int __fun_call(Executor& ks);
};

class VHWD_DLLIMPEXP CallableFunction : public CallableData
{
public:

	virtual String GetType() const {return "function";}

	virtual bool ToValue(String& s) const {s=m_sName;return true;}


	bool CheckParamCount(Executor& ks,int pm);

	enum
	{
		CF_READONLY	=1<<0,
	};

	virtual int __getindex(Executor&,const String&); // top.s
	virtual int __setindex(Executor&,const String&); // top.s=val

	String m_sName;
	BitFlags flags;
};

class CallableValue : public CallableData
{
public:
	virtual int __getindex(Executor&,const String&);
	virtual String GetType() const {return "value";}
};

template<typename T>
class CallableValueT : public CallableValue
{
public:
	typedef T type;
	type value;
	CallableValueT(){}
	CallableValueT(const type& v):value(v){}

	virtual uint32_t hashcode() const
	{
		hash_t<type> h;
		return h(value);
	}

	virtual bool IsEqualTo(const CallableData* d) const
	{
		if(this==d) return true;
		const CallableValueT<type>* q=dynamic_cast<const CallableValueT<type>*>(d);
		return q && value==q->value;
	}

};



template<>
class CallableValueT<kvar_table> : public CallableValue
{
public:
	typedef kvar_table type;

	type value;
	CallableValueT(){}
	CallableValueT(const type& v):value(v){}

	virtual int __getindex(Executor& ks,const String& s);
	virtual int __setindex(Executor& ks,const String& s);

	virtual int __getarray(Executor& ks);
	virtual int __setarray(Executor& ks);


	virtual uint32_t hashcode() const;

	virtual bool IsEqualTo(const CallableData* d) const;

	virtual CallableValueT<kvar_table>* ToTable() {return this;}
	virtual CallableClass* ToClass() {return NULL;}

	virtual CallableIterator* GetIterator(int n);


	virtual String GetType() const {return "table";}

};


template<typename T>
class CallableValueT<arr_xt<T> > : public CallableValue
{
public:

	typedef arr_xt<T> type;

	type value;
	CallableValueT(){}
	CallableValueT(const type& v):value(v){}

	virtual uint32_t hashcode()
	{
		hash_t<type> h;return h(value);
	}

	virtual bool IsEqualTo(const CallableData* d) const
	{
		if(this==d) return true;
		const CallableValueT<type>* q=dynamic_cast<const CallableValueT<type>*>(d);
		return q && value==q->value;
	}

	virtual int __getindex(Executor& ks,const String& s);
	virtual int __setindex(Executor& ks,const String& s);

	virtual int __getarray(Executor& ks);
	virtual int __setarray(Executor& ks);

	virtual CallableIterator* GetIterator(int);

	virtual String GetType() const {return "array";}

};

class kvar_compound
{
public:
	int32_t type;
	BitFlags flags;
	int64_t val[8];

	bool operator==(const kvar_compound& o) const
	{
		return memcmp(this,&o,sizeof(kvar_compound))==0;
	}
	bool operator!=(const kvar_compound& o) const
	{
		return memcmp(this,&o,sizeof(kvar_compound))!=0;
	}
};



template<>
class hash_t<kvar_compound> : public hash_pod<kvar_compound>
{
public:

};



template<typename T,int N>
class kvar_type_raw
{
public:
	typedef T type;
	typedef T elemtype;

	static const int flag=N;

	static inline type& raw(kvar_base& v)
	{
		return (type&)v;
	}

	static inline void reset(kvar_base& v,const type& t)
	{
		if(v.bptr() && v.pVal)
		{
			v.pVal->DecRef();
		}
		v.flag=flag;
		raw(v)=t;
	}

	static inline type& ref(kvar_base& v)
	{
		if(v.flag!=flag)
		{
			reset(v,type());
		}
		return raw(v);
	}

	static inline type& unique(kvar_base& v)
	{
		return ref(v);
	}

	static inline type* ptr(kvar_base& v)
	{
		if(v.flag==flag)
		{
			return &raw(v);
		}
		return NULL;
	}

};


template<typename T,int N>
class kvar_type_ptr
{
public:
	typedef T type;
	typedef T elemtype;

	static const int flag=N;

	static type& raw(kvar_base& v)
	{
		return static_cast<CallableValueT<T>*>(v.pVal)->value;
	}

	static void reset(kvar_base& v,const type& t)
	{
		v.clear();
		v.pVal=new CallableValueT<T>(t);
		v.pVal->IncRef();
		v.flag=flag;
	}

	static type& ref(kvar_base& v)
	{
		if(v.flag!=flag)
		{
			reset(v,type());
		}
		return raw(v);
	}

	static type& unique(kvar_base& v)
	{
		if(v.flag!=flag||v.pVal->GetRef()>1)
		{
			reset(v,type());
		}
		return raw(v);
	}

	static type* ptr(kvar_base& v)
	{
		if(v.flag==flag)
		{
			return &raw(v);
		}
		return NULL;
	}
};


template<int N>
class kvar_type : public kvar_type_ptr<arr_xt<typename kvar_type<N-Variant::V_ARRMASK>::elemtype>,N>
{
public:

};

template<>
class kvar_type<Variant::V_BOOL> : public kvar_type_raw<bool,Variant::V_BOOL>
{
public:

};

template<>
class kvar_type<Variant::V_INT> : public kvar_type_raw<int64_t,Variant::V_INT>
{
public:

};

template<>
class kvar_type<Variant::V_DOUBLE> : public kvar_type_raw<double,Variant::V_DOUBLE>
{
public:

};


template<>
class kvar_type<Variant::V_COMPOUND> :  public kvar_type_ptr<kvar_compound,Variant::V_COMPOUND>
{
public:

};

template<>
class kvar_type<Variant::V_COMPLEX> : public kvar_type_ptr<std::complex<double>,Variant::V_COMPLEX>
{
public:

};


template<>
class kvar_type<Variant::V_STRING> :  public kvar_type_ptr<String,Variant::V_STRING>
{
public:

};


template<>
class kvar_type<Variant::V_TABLE> :  public kvar_type_ptr<kvar_table,Variant::V_TABLE>
{
public:

};

template<>
class kvar_type<Variant::V_OBJECT> :  public kvar_type_raw<CallableData*,Variant::V_OBJECT>
{
public:

	typedef CallableData* type;
	typedef DataPtrT<CallableData> elemtype;

	static const int flag=Variant::V_OBJECT;

	static type& raw(kvar_base& v)
	{
		return (type&)v;
	}

	static void reset(kvar_base& v,const type& t)
	{
		v.clear();
		v.flag=flag;
		ObjectData::locked_reset(raw(v),t);
	}

	static type& ref(kvar_base& v)
	{
		if(v.flag!=flag)
		{
			reset(v,type());
		}
		return raw(v);
	}

	static type& unique(kvar_base& v)
	{
		return ref(v);
	}

};


template<typename T>
class kvar_flag_miss : public kvar_type<Variant::V_OBJECT>
{
public:

	typedef T type;
	typedef T elemtype;
	static const int flag=Variant::V_OBJECT;

	static type& raw(kvar_base& v)
	{
		return static_cast<CallableValueT<type>*>(v.pVal)->value;
	}

	static type& ref(kvar_base& v)
	{
		if(v.flag==flag)
		{
			CallableValueT<type>* p=dynamic_cast<CallableValueT<type>*>(v.pVal);
			if(p) return p->value;
		}
		reset(v,type());
		return raw(v);
	}

	static type& unique(kvar_base& v)
	{
		if(v.flag==flag)
		{
			CallableValueT<type>* p=dynamic_cast<CallableValueT<type>*>(v.pVal);
			if(p && p->GetRef()==1) return p->value;
		}
		reset(v,type());
		return raw(v);
	}

	static type* ptr(kvar_base& v)
	{
		if(v.flag==flag)
		{
			CallableValueT<type>* p=dynamic_cast<CallableValueT<type>*>(v.pVal);
			if(p) return &p->value;
		}
		return NULL;
	}

	static void reset(kvar_base& v,const type& t)
	{
		v.clear();
		v.pVal=new CallableValueT<T>(t);
		v.pVal->IncRef();
	}

};

template<typename T,int N>
class kvar_flag_test
{
public:
	static const int flag=tl::is_same_type<T,typename kvar_type<N>::type>::value?N:-1;
	static const int fsum=flag>=0?flag:kvar_flag_test<T,N-1>::fsum;
};


template<typename T>
class kvar_flag_test<T,0>
{
public:
	static const int flag=tl::is_same_type<T,typename kvar_type<0>::type>::value?0:-1;
	static const int fsum=flag;
};


template<typename T,int N>
class kvar_flag_switch : public kvar_type<N>
{
public:

};


template<typename T>
class kvar_flag_switch<T,-1> : public kvar_flag_miss<T>
{
public:

};


template<typename T>
class kvar_flag : public kvar_flag_switch<T,kvar_flag_test<T,(Variant::V_ARRMASK<<1)-1>::fsum>
{
public:

};

template<typename T>
inline void Variant::reset(const T& value)
{
	kvar_flag<T>::reset(data,value);
}

template<typename T>
inline void Variant::reset()
{
	clear();ref<T>();
}

template<typename T>
inline T& Variant::ref()
{
	return kvar_flag<T>::ref(data);
}

template<typename T>
inline T* Variant::ptr()
{
	return kvar_flag<T>::ptr(data);
}

template<typename T>
inline bool Variant::get(T& value)
{
	T* p=kvar_flag<T>::ptr(data);
	if(!p) return false;
	value=*p;
	return true;
}

template<typename T>
inline bool Variant::set(const T& value)
{
	T* p=kvar_flag<T>::ptr(data);
	if(!p) return false;
	*p=value;
	return true;
}


template<typename T>
class kvar_cast;

template<>
class kvar_cast<bool>
{
public:
	typedef bool type;
	template<typename O>
	static type g(const O&){Exception::XBadCast();return type();}
	static type g(const type& v){return v;}
	static type g(int32_t v){return v!=0;}
	static type g(int64_t v){return v!=0;}
	static type g(float v){return v!=0;}
	static type g(double v){return v!=0;}
	static type g(const String& v){return !v.empty();}
	static type g(CallableData* d){return d!=NULL;}

	template<typename T>
	static type g(const std::complex<T>& o)
	{
		if(o.imag()!=T(0))
		{
			Exception::XBadCast();
		}
		return g(o.real());
	}

	template<typename O>
	static type g(const arr_xt<O>& v)
	{
		if(v.size()==1) return g(v(0));
		Exception::XBadCast();
		return type();
	}

};


template<>
class kvar_cast<kvar_compound>
{
public:
	typedef kvar_compound type;

	template<typename O>
	static type g(const O&){Exception::XBadCast();return type();}

};

template<>
class kvar_cast<kvar_table>
{
public:
	typedef kvar_table type;

	template<typename O>
	static type g(const O&){Exception::XBadCast();return type();}

};


template<>
class kvar_cast<DataPtrT<CallableData> >
{
public:
	typedef DataPtrT<CallableData> type;

	template<typename O>
	static type g(const O&){Exception::XBadCast();return type();}

};



template<typename T>
class kvar_cast_number
{
public:
	typedef T type;
	template<typename O>
	static type g(const O&){Exception::XBadCast();return type();}
	static type g(bool v){return (type)v;}
	static type g(int32_t v){return (type)v;}
	static type g(int64_t v){return (type)v;}
	static type g(size_t v){return (type)v;}
	static type g(float v){return (type)v;}
	static type g(double v){return (type)v;}

	static type g(CallableData* v)
	{
		type n(0);
		if(!v) return n;
		if(!v->ToValue(n))
		{
			Exception::XBadCast();
		}
		return n;
	}

	static type g(const String& v)
	{
		if(tl::is_same_type<type,double>::value||tl::is_same_type<type,float>::value)
		{
			double k;if(v.ToNumber(&k)) return (type)k;
			Exception::XBadCast();return type();
		}
		else
		{
			int64_t k;if(v.ToNumber(&k)) return (type)k;
			Exception::XBadCast();return type();
		}
	}

	template<typename O>
	static type g(const arr_xt<O>& v)
	{
		if(v.size()==1) return g(v(0));
		Exception::XBadCast();
		return type();
	}

	template<typename O>
	static type g(const std::complex<O>& v)
	{
		if(v.imag()==type(0)) return g(v.real());
		Exception::XBadCast();
		return type();
	}

};

template<> class kvar_cast<size_t> : public kvar_cast_number<size_t>{};
template<> class kvar_cast<int32_t> : public kvar_cast_number<int32_t>{};
template<> class kvar_cast<int64_t> : public kvar_cast_number<int64_t>{};
template<> class kvar_cast<float> : public kvar_cast_number<float>{};
template<> class kvar_cast<double> : public kvar_cast_number<double>{};

template<typename T>
class kvar_cast<std::complex<T> >
{
public:
	typedef std::complex<T> type;
	template<typename O>
	static type g(const O&){Exception::XBadCast();return type();}

	static type g(bool v){return type(v,0);}
	static type g(int32_t v){return type(v,0);}
	static type g(int64_t v){return type(v,0);}
	static type g(float v){return type(v,0);}
	static type g(double v){return type(v,0);}
	static const type& g(const type& v){return v;}

	template<typename O>
	static type g(const arr_xt<O>& v)
	{
		if(v.size()==1) return g(v(0));
		Exception::XBadCast();
		return type();
	}

};

template<typename T>
class kvar_cast<arr_xt<T> >
{
public:
	typedef arr_xt<T> type;
	template<typename O>
	static type g(const O&){Exception::XBadCast();return type();}

	static type g(bool v){type t;t.resize(1);t(0)=kvar_cast<T>::g(v);return t;}
	static type g(int32_t v){type t;t.resize(1);t(0)=kvar_cast<T>::g(v);return t;}
	static type g(int64_t v){type t;t.resize(1);t(0)=kvar_cast<T>::g(v);return t;}
	static type g(float v){type t;t.resize(1);t(0)=kvar_cast<T>::g(v);return t;}
	static type g(double v){type t;t.resize(1);t(0)=kvar_cast<T>::g(v);return t;}

	static const type& g(const type& v){return v;}

	template<typename O>
	static type g(const arr_xt<O>& v)
	{
		type t;
		t.resize(v.size(0),v.size(1),v.size(2),v.size(3),v.size(4),v.size(5));
		for(size_t k5=0;k5<v.size(5);k5++)
		for(size_t k4=0;k4<v.size(4);k4++)
		for(size_t k3=0;k3<v.size(3);k3++)
		for(size_t k2=0;k2<v.size(2);k2++)
		for(size_t k1=0;k1<v.size(1);k1++)
		for(size_t k0=0;k0<v.size(0);k0++)
			t(k0,k1,k2,k3,k4,k5)=kvar_cast<T>::g(v(k0,k1,k2,k3,k4,k5));

		return t;
	}

};


template<>
class kvar_cast<String>
{
public:
	typedef String type;
	template<typename O>
	static type g(const O&){Exception::XBadCast();return type();}

	static const type& g(const type& v){return v;}
	static type g(bool v){return v?"true":"false";}
	static type g(int32_t v){type t;t<<v;return t;}
	static type g(int64_t v){type t;t<<v;return t;}
	static type g(float v){type t;t<<v;return t;}
	static type g(double v){type t;t<<v;return t;}

	static type g(CallableData* v)
	{
		type s;if(!v) return "nil";
		v->ToValue(s);
		return s;
	}

	template<typename O>
	static type g(const arr_xt<O>& v)
	{
		if(v.size()==1) return g(v(0));
		Exception::XBadCast();
		return type();
	}

};

template<>
class hash_t<CallableData*>
{
public:
	uint32_t operator()(CallableData* d);
};


template<>
class hash_t<DataPtrT<CallableData> >
{
public:
	uint32_t operator()(const DataPtrT<CallableData>& o);
};


template<>
class hash_t<Variant>
{
public:
	uint32_t operator()(const Variant& v);
};




inline Variant::Variant(const Variant&o):data(o.data)
{
	if(bptr() && data.pVal)
	{
		data.pVal->IncRef();
	}
}

inline Variant& Variant::operator=(const Variant& o)
{
	if(o.bptr() && o.data.pVal)
	{
		o.data.pVal->IncRef();
	}

	if(bptr() && data.pVal)
	{
		data.pVal->DecRef();
	}

	data=o.data;

	return *this;
}


inline void Variant::swap(Variant& o)
{
	std::swap(o.data,data);
}

inline void Variant::kptr(CallableData* p)
{
	if(p) p->IncRef();
	clear();
	data.pVal=p;
}


inline CallableData* Variant::kptr()
{
	if(bptr())
	{
		return data.pVal;
	}
	return NULL;
}

inline const CallableData* Variant::kptr() const
{
	if(bptr())
	{
		return data.pVal;
	}
	return NULL;
}


inline void kvar_base::clear()
{
	if(bptr() && pVal)
	{
		pVal->DecRef();
	}
	flag=Variant::V_OBJECT;
	pVal=NULL;
}

inline void Variant::clear()
{
	data.clear();
}

VHWD_LEAVE
#endif
