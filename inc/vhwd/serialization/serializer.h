// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_SERIALIZER__
#define __H_VHWD_SERIALIZER__


#include "vhwd/basic/object_ex.h"
#include "vhwd/basic/bitflags.h"
#include "vhwd/basic/exception.h"
#include "vhwd/collection/indexer_set.h"
#include "vhwd/collection/indexer_map.h"
#include "vhwd/collection/arr_1t.h"
#include "vhwd/collection/arr_xt.h"
#include "vhwd/collection/bst_set.h"
#include "vhwd/collection/bst_map.h"

#pragma push_macro("new")
#undef new
#include <complex>
#include <vector>
#include <map>
#include <set>
#pragma pop_macro("new")


namespace tl
{

	template<typename T>
	struct is_pod<std::complex<T> > : public is_pod<T> {};

	template<typename X,typename Y>
	struct is_pod<std::pair<X,Y> > : public value_type<is_pod<X>::value&&is_pod<Y>::value> {};

};

VHWD_ENTER

DEFINE_OBJECT_NAME(std::complex<float32_t>,"c32");
DEFINE_OBJECT_NAME(std::complex<float64_t>,"c64");

class VHWD_DLLIMPEXP SerializerReader;
class VHWD_DLLIMPEXP SerializerWriter;

typedef uint32_t intarr_t;

class VHWD_DLLIMPEXP Serializer : private NonCopyable
{
protected:

	Serializer(int t);

public:

	enum
	{
		READER=1,
		WRITER=2,
	};

	enum
	{
		PTRTAG_NIL,		//NULL pointer
		PTRTAG_CLS,		//class pointer (but NOT Object)
		PTRTAG_POD,		//POD pointer
		PTRTAG_OBJ,		//Object and its derived class pointer
		PTRTAG_CACHED,
	};

	enum
	{
		STRCVT_UTF8	=1,	// convert string to utf8
	};

	virtual ~Serializer() {}

	bool is_reader()
	{
		return type==READER;
	}

	bool is_writer()
	{
		return type==WRITER;
	}

	virtual void errstr(const String& msg)
	{
		Exception::XError(msg.c_str());
	}

	virtual bool good() const
	{
		return true;
	}

	virtual Serializer& tag(char ch)=0;
	virtual Serializer& tag(const char* msg)=0;

	virtual int local_version(int v)=0;

	int global_version()
	{
		return gver;
	}
	void global_version(int v)
	{
		gver=v;
	}

	virtual void close() {}

	BitFlags flags;


	SerializerWriter& writer()
	{
		if(!is_writer())
		{
			errstr("NOT_WRITER");
		}
		return *(SerializerWriter*)this;
	}

	virtual SerializerReader& reader()
	{
		if(!is_reader())
		{
			errstr("NOT_READER");
		}
		return *(SerializerReader*)this;
	}

protected:
	const int32_t type;
	int32_t gver;

};



class VHWD_DLLIMPEXP serializer_cached_objects
{
public:

	serializer_cached_objects();
	~serializer_cached_objects();

	void* get(int32_t idx);

	int32_t get(Object* dat);
	int32_t put(Object* dat);
	int32_t get(void* dat);
	int32_t put(void* dat);

	void clear();

protected:
	indexer_set<void*> m_aObjects;

};


class VHWD_DLLIMPEXP SerializerEx : public Serializer
{
protected:

	SerializerEx(int t);

public:

	virtual void errstr(const String& msg)
	{
		Exception::XError(msg.c_str());
	}

	void* get(int32_t idx)
	{
		return cached_objects.get(idx);
	}

	int32_t get(Object* dat)
	{
		return cached_objects.get(dat);
	}

	int32_t put(Object* dat)
	{
		return cached_objects.put(dat);
	}

	int32_t get(void* dat)
	{
		return cached_objects.get(dat);
	}

	int32_t put(void* dat)
	{
		return cached_objects.put(dat);
	}

	void clear()
	{
		cached_objects.clear();
	}


	Object* create(const String& s)
	{
		return object_creator.Create(s);
	}

protected:

	serializer_cached_objects cached_objects;
	ObjectCreator object_creator;
};



class VHWD_DLLIMPEXP SerializerReader : public SerializerEx
{
public:
	SerializerReader():SerializerEx(READER) {}

	void checked_recv(char* data,size_t size)
	{
		size_t n=recv(data,size);
		if(n!=size)
		{
			errstr("invalid received size");
		}
	}

	virtual size_t recv(char* data,size_t size)=0;

	virtual void close() {}

	virtual SerializerReader& tag(char ch);
	virtual SerializerReader& tag(const char* msg);
	virtual int local_version(int v);

};

class VHWD_DLLIMPEXP SerializerWriter : public SerializerEx
{
public:

	SerializerWriter():SerializerEx(WRITER) {}

	virtual size_t send(const char* data,size_t size)=0;

	void checked_send(const char* data,size_t size)
	{
		size_t n=send(data,size);
		if(n!=size)
		{
			errstr("invalid received size");
		}
	}

	virtual void close() {}

	virtual SerializerWriter& tag(char ch);
	virtual SerializerWriter& tag(const char* msg);

	virtual int local_version(int v);
};

class VHWD_DLLIMPEXP SerializerDuplex : protected SerializerReader, protected SerializerWriter
{
public:
	// get writer
	SerializerWriter& writer()
	{
		return *this;
	}

	// get reader
	SerializerReader& reader()
	{
		return *this;
	}

};


template<typename T>
class serial_type
{
public:
	static const int value=tl::is_convertible<T,Object>::value?3: (tl::is_pod<T>::value?2:1);
};

template<typename T> Serializer& operator &(Serializer& ar,T& val);
template<typename T> SerializerReader& operator &(SerializerReader& ar,T& val);
template<typename T> SerializerWriter& operator &(SerializerWriter& ar,T& val);

template<typename T> SerializerReader& operator >>(Serializer& ar,T& val);
template<typename T> SerializerWriter& operator <<(Serializer& ar,T& val);
template<typename T> SerializerReader& operator >>(SerializerReader& ar,T& val);
template<typename T> SerializerWriter& operator <<(SerializerWriter& ar,T& val);


template<typename A,typename T>
class serial_helper_test;

template<typename A,typename T>
class serial_helper_func;

template<typename A,typename T,int N>
class serial_switch;

template<typename A,typename T>
class serial_pod;

template<typename A,typename I,typename T>
class serial_pod_cast : public serial_pod<A,T>
{
public:
	static const int value=0;
	typedef I type;
	static void g(SerializerReader& ar,type& val_)
	{
		T val;
		serial_pod<A,T>::g(ar,val);
		val_=val;
	}
	static void g(SerializerWriter& ar,type& val_)
	{
		T val(val_);
		serial_pod<A,T>::g(ar,val);
	}
};

template<typename A,typename T>
class serial_pod_cast<A,bool,T> : public serial_pod<A,T>
{
public:
	static const int value=0;
	typedef bool type;
	static void g(SerializerReader& ar,type& val_)
	{
		T val;
		serial_pod<A,T>::g(ar,val);
		val_=val!=0;
	}
	static void g(SerializerWriter& ar,type& val_)
	{
		T val(val_?1:0);
		serial_pod<A,T>::g(ar,val);
	}
};

template<typename A,typename T>
class serial_pod
{
public:
	static const int value=tl::is_pod<T>::value;
	static void g(SerializerReader& ar,T& val)
	{
		ar.checked_recv((char*)&val,sizeof(T));
	}
	static void g(SerializerWriter& ar,T& val)
	{
		ar.checked_send((char*)&val,sizeof(T));
	}
};

template<typename A> class serial_pod<A,bool> : public serial_pod_cast<A,bool,char> {};
template<typename A> class serial_pod<A,long> : public serial_pod_cast<A,long,int64_t> {};
template<typename A> class serial_pod<A,unsigned long> : public serial_pod_cast<A,unsigned long,int64_t> {};
template<typename A> class serial_pod<A,wchar_t> : public serial_pod_cast<A,wchar_t,int32_t> {};


template<typename A,typename T,bool>
class serial_arr_handler_impl
{
public:
	static void g(A& ar,T* ptr,intarr_t s)
	{
		for(intarr_t i=0; i<s; i++)
		{
			serial_helper_test<A,T>::g(ar,ptr[i]);
		}
	}
};


template<typename A,typename T>
class serial_arr_handler_impl<A,T,true>
{
public:
	static void g(SerializerReader& ar,T* ptr,intarr_t s)
	{
		ar.recv((char*)ptr,sizeof(T)*s);
	}

	static void g(SerializerWriter& ar,T* ptr,intarr_t s)
	{
		ar.send((char*)ptr,sizeof(T)*s);
	}
};

template<typename A,typename T>
class serial_arr_handler : public serial_arr_handler_impl<A,T,serial_pod<A,T>::value> {};


template<typename A,typename T,typename V>
class serial_arr
{
public:
	typedef V type;

	static void g(SerializerReader& ar,type& val)
	{
		intarr_t n=0;
		serial_pod<A,intarr_t>::g(ar,n);
		val.resize(n);
		if(n==0) return;
		serial_arr_handler<A,T>::g(ar,&val[0],n);
	}

	static void g(SerializerWriter& ar,type& val)
	{
		intarr_t n=(intarr_t)val.size();
		serial_pod<A,intarr_t>::g(ar,n);
		if(n==0) return;
		serial_arr_handler<A,T>::g(ar,&val[0],n);
	}
};


template<typename A,typename V>
class serial_arr<A,bool,V>
{
public:
	typedef bool T;
	typedef V type;

	static void g(SerializerReader& ar,type& val)
	{
		intarr_t n=0;
		serial_pod<A,intarr_t>::g(ar,n);
		val.resize(n);
		if(n==0) return;
		for(intarr_t i=0; i<n; i++)
		{
			bool c1;
			serial_pod<A,bool>::g(ar,c1);
			val[i]=c1;
		}
	}
	static void g(SerializerWriter& ar,type& val)
	{
		intarr_t n=(intarr_t)val.size();
		serial_pod<A,intarr_t>::g(ar,n);
		if(n==0) return;
		for(intarr_t i=0; i<n; i++)
		{
			bool c1(val[i]);
			serial_pod<A,bool>::g(ar,c1);
		}
	}

};

template<typename A,typename T,typename V>
class serial_set
{
public:
	typedef V type;
	typedef typename type::iterator iterator;
	static void g(SerializerReader& ar,type& val)
	{
		intarr_t n=0;
		T v;
		serial_pod<A,intarr_t>::g(ar,n);
		for(intarr_t i=0; i<n; i++)
		{
			ar & v;
			val.insert(v);
		}
	}

	static void g(SerializerWriter& ar,type& val)
	{
		intarr_t n=(intarr_t)val.size();
		serial_pod<A,intarr_t>::g(ar,n);
		for(iterator it=val.begin(); it!=val.end(); it++)
		{
			ar << (*it);
		}
	}

};


template<typename A,typename T,int N>
class serial_ptr
{
public:
	typedef serial_switch<A,T,N> basetype;
	typedef T* pointer;

	static void g(SerializerReader& ar,pointer& val)
	{
		int32_t sval(0);
		serial_pod<A,int32_t>::g(ar,sval);
		if(sval==A::PTRTAG_NIL)
		{
			delete val;
			val=NULL;
			return;
		}

		if(sval!=N)
		{
			ar.errstr("INVALID_POINTER_TYPE");
			return;
		}

		pointer tmp=new T();
		try
		{
			serial_helper_test<A,T>::g(ar,*tmp);
			delete val;
			val=tmp;
		}
		catch(...)
		{
			delete tmp;
			throw;
		}
	}

	static void g(SerializerWriter& ar,pointer& val)
	{
		int32_t sval=val==NULL? A::PTRTAG_NIL : N;
		serial_pod<A,int32_t>::g(ar,sval);
		if(sval==A::PTRTAG_NIL) return;
		serial_helper_test<A,T>::g(ar,*val);
	}
};


template<typename A,typename T>
class serial_ptr<A,T,3> : public serial_switch<A,T,3>
{
public:
	typedef serial_switch<A,T,3> basetype;
	typedef T* pointer;

	static void g(SerializerReader& ar,pointer& val)
	{
		int32_t sval=A::PTRTAG_NIL;
		serial_pod<A,int32_t>::g(ar,sval);

		if(sval==A::PTRTAG_NIL)
		{
			delete val;
			val=NULL;
			return;
		}

		if(sval==A::PTRTAG_CACHED)
		{
			int32_t nidx(-1);
			serial_pod<A,int32_t>::g(ar,nidx);
			val=dynamic_cast<pointer>((Object*)ar.get(nidx));
			if(!val)
			{
				ar.errstr("INVALID_CACHED_INDEX");
			}
			return;
		}
		if(sval!=A::PTRTAG_OBJ)
		{
			ar.errstr("INVALID_POINTER_TYPE");
			return;
		}
		String type;
		serial_helper_func<A,String>::g(ar,type);

		Object* obj=ar.create(type);
		if(!obj)
		{
			ar.errstr("CANNOT_CREATE_OBJECT");
			return;
		}

		pointer tmp=dynamic_cast<pointer>(obj);
		if(!tmp)
		{
			delete obj;
			ar.errstr("INCOMPATIBLE_OBJECT_TYPE");
			return;
		}

		try
		{
			basetype::g(ar,*tmp);
			delete val;
			val=tmp;
		}
		catch(...)
		{
			delete tmp;
			throw;
		}

	}

	static void g(SerializerWriter& ar,pointer& val)
	{
		int32_t sval=val==NULL? A::PTRTAG_NIL : A::PTRTAG_OBJ;
		if(sval==A::PTRTAG_NIL)
		{
			serial_pod<A,int32_t>::g(ar,sval);
			return;
		}

		int32_t nidx=ar.get(val);
		if(nidx>0)
		{
			sval=A::PTRTAG_CACHED;
			serial_pod<A,int32_t>::g(ar,sval);
			serial_pod<A,int32_t>::g(ar,nidx);
			return;
		}

		serial_pod<A,int32_t>::g(ar,sval);

		String type=val->GetObjectName();
		serial_helper_func<A,String>::g(ar,type);

		if(type=="")
		{
			ar.errstr("UNREGISTER_OBJECT_TYPE");
			return;
		}

		basetype::g(ar,*val);

	}
};



template<typename A,typename T,typename V>
class serial_ptr_holder : public serial_ptr<A,T,serial_type<T>::value>
{
public:
	typedef V type;
	typedef serial_ptr<A,T,serial_type<T>::value> basetype;

	static void g(SerializerReader& ar,type& val)
	{
		T* p(NULL);
		try
		{
			basetype::g(ar,p);
		}
		catch(...)
		{
			delete p;
			throw;
		}
		val.reset(p);
	}
	static void g(SerializerWriter& ar,type& val)
	{
		T* p(val.get());
		basetype::g(ar,p);
	}
};


template<typename A,typename T>
class serial_switch<A,T,1>
{
public:
	static void g(A& ar,T& val)
	{
		val.Serialize(ar);
	}
};

template<typename A,typename T>
class serial_switch<A,T,2> : public serial_pod<A,T> {};

template<typename A,typename T>
class serial_switch<A,T,3>
{
public:
	static void g(A& ar,Object& val)
	{
		ar.put(&val);
		val.Serialize(ar);
	}
};


template<typename A,typename T>
class serial_helper_func : public serial_switch<A,T,serial_type<T>::value>
{
public:

};

template<typename A,typename T>
class serial_helper_func<A,T*> : public serial_ptr<A,T,serial_type<T>::value>
{
public:

};


template<typename A,typename X,typename Y>
class serial_helper_func<A,std::pair<X,Y> >
{
public:
	typedef std::pair<X,Y> type;
	static void g(A& ar,type& val)
	{
		if(serial_pod<A,X>::value && serial_pod<A,Y>::value)
		{
			serial_pod<A,type>::g(ar,val);
		}
		else
		{
			ar & val.first & val.second;
		}
	}
};


template<typename A,typename T,typename X>
class serial_helper_func<A,std::vector<T,X> > : public serial_arr<A,T,std::vector<T,X> >
{
public:

};

template<typename A,typename T,typename X>
class serial_helper_func<A,arr_1t<T,X> > : public serial_arr<A,T,arr_1t<T,X> >
{
public:

};

template<typename A,typename T>
class serial_helper_func<A,StringBuffer<T> > : public serial_arr<A,T,StringBuffer<T> >
{
public:

};


template<typename A,typename K,typename T,typename X,typename Y>
class serial_helper_func<A,std::map<K,T,X,Y> > : public serial_set<A,std::pair<K,T>,std::map<K,T,X,Y> >
{
public:

};

template<typename A,typename K,typename T,typename X,typename Y>
class serial_helper_func<A,bst_map<K,T,X,Y> > : public serial_set<A,std::pair<K,T>,bst_map<K,T,X,Y> >
{
public:

};

template<typename A,typename T,typename X,typename Y>
class serial_helper_func<A,std::set<T,X,Y> > : public serial_set<A,T,std::set<T,X,Y> >
{
public:

};

template<typename A,typename T,typename X,typename Y>
class serial_helper_func<A,bst_set<T,X,Y> > : public serial_set<A,T,bst_set<T,X,Y> >
{
public:

};


template<typename A,typename T,size_t N>
class serial_helper_func<A,T[N] >
{
public:
	typedef T type[N];

	static void g(SerializerReader& ar,type& val)
	{
		intarr_t n(0);
		serial_pod<A,intarr_t>::g(ar,n);
		if(n!=(intarr_t)N) ar.errstr("invalid_array_size");
		serial_arr_handler<A,T>::g(ar,&val[0],n);
	}

	static void g(SerializerWriter& ar,type& val)
	{
		intarr_t n=(intarr_t)N;
		serial_pod<A,intarr_t>::g(ar,n);
		serial_arr_handler<A,T>::g(ar,&val[0],n);
	}

};

template<typename A,typename T>
class serial_helper_func<A,LitePtrT<T> > : public serial_ptr_holder<A,T,LitePtrT<T> >
{
public:

};

template<typename A,typename T>
class serial_helper_func<A,DataPtrT<T> > : public serial_ptr_holder<A,T,DataPtrT<T> >
{
public:

};

template<typename A,typename T,typename X,typename Y>
class serial_helper_func<A,std::basic_string<T,X,Y> > : public serial_arr<A,T,std::basic_string<T,X,Y> >
{
public:

};


template<typename A>
class VHWD_DLLIMPEXP serial_helper_func<A,String>
{
public:
	typedef String type;

	static void g(SerializerReader& ar,type& val);
	static void g(SerializerWriter& ar,type& val);
};



template<typename A,typename T,typename X>
class serial_helper_func<A,arr_xt<T,X> >
{
public:
	typedef arr_xt<T,X> type;

	static void g(SerializerReader& ar,type& val)
	{
		intarr_t d[type::MAX_DIM]= {0};
		serial_arr_handler<A,intarr_t>::g(ar,d,type::MAX_DIM);
		val.resize(d[0],d[1],d[2],d[3],d[4],d[5]);
		serial_arr_handler<A,T>::g(ar,val.data(),val.size());

	}

	static void g(SerializerWriter& ar,type& val)
	{
		intarr_t d[type::MAX_DIM];
		for(int i=0; i<(int)type::MAX_DIM; i++)
		{
			d[i]=val.size(i);
		}
		serial_arr_handler<A,intarr_t>::g(ar,d,type::MAX_DIM);
		serial_arr_handler<A,T>::g(ar,val.data(),val.size());
	}
};


template<typename A,typename T>
class serial_helper_test : public serial_helper_func<A,T>
{
public:

};

template<typename A,typename T>
class serial_helper_test<A,const T>
{
public:
	static void g(SerializerWriter& ar,const T& val)
	{
		serial_helper_func<A,T>::g(ar,const_cast<T&>(val));
	}

	static void g(SerializerReader& ar,const T& val)
	{
		(void)&val;
		ar.errstr("attempt_to_serialize_const");
	}
};


template<typename T>
SerializerReader& operator >>(SerializerReader& ar,T& val)
{
	serial_helper_test<SerializerReader,T>::g(ar,val);
	return ar;
}

template<typename T>
SerializerWriter& operator <<(SerializerWriter& ar,T& val)
{
	serial_helper_test<SerializerWriter,T>::g(ar,val);
	return ar;
}

template<typename T>
SerializerReader& operator &(SerializerReader& ar,T& val)
{
	serial_helper_test<SerializerReader,T>::g(ar,val);
	return ar;
}

template<typename T>
SerializerWriter& operator &(SerializerWriter& ar,T& val)
{
	serial_helper_test<SerializerWriter,T>::g(ar,val);
	return ar;
}

template<typename T>
Serializer& operator &(Serializer& ar,T& val)
{
	if(ar.is_reader())
	{
		serial_helper_test<SerializerReader,T>::g(static_cast<SerializerReader&>(ar),val);
		return ar;
	}

	if(ar.is_writer())
	{
		serial_helper_test<SerializerWriter,T>::g(static_cast<SerializerWriter&>(ar),val);
		return ar;
	}

	ar.errstr("INVALID_SERIALIZER");
	return ar;
}

template<typename T>
SerializerReader& operator >>(Serializer& ar_,T& val)
{
	SerializerReader& ar(ar_.reader());
	serial_helper_test<SerializerReader,T>::g(ar,val);
	return ar;
}

template<typename T>
SerializerWriter& operator <<(Serializer& ar_,T& val)
{
	SerializerWriter& ar(ar_.writer());
	serial_helper_test<SerializerWriter,T>::g(ar,val);
	return ar;
}

template<typename T>
SerializerDuplex& operator >>(SerializerDuplex& ar,T& val)
{
	ar.reader() >> val;
	return ar;
}

template<typename T>
SerializerDuplex& operator <<(SerializerDuplex& ar,T& val)
{
	ar.writer() << val;
	return ar;
}

VHWD_LEAVE
#endif
