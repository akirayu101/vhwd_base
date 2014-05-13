// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_SERIALIZER__
#define __H_VHWD_SERIALIZER__



#include "vhwd/basic/object.h"
#include "vhwd/basic/pointer.h"
#include "vhwd/basic/exception.h"
#include "vhwd/collection/indexer_set.h"
#include "vhwd/collection/array.h"

#pragma push_macro("new")
#undef new
#include <complex>
#include <vector>
#include <map>
#pragma pop_macro("new")

namespace tl
{
	template<typename T>
	struct is_pod<std::complex<T> > : public value_type<true>{};
};

VHWD_ENTER

DEFINE_OBJECT_NAME(std::complex<float32_t>,"c32");
DEFINE_OBJECT_NAME(std::complex<float64_t>,"c64");

class VHWD_DLLIMPEXP SerializerReader;
class VHWD_DLLIMPEXP SerializerWriter;

class VHWD_DLLIMPEXP Serializer : private NonCopyable
{
public:

	enum
	{
		READER=1,
		WRITER=2,
	};

	Serializer(int t);
	virtual ~Serializer(){}

	bool is_reader(){return type==READER;}
	bool is_writer(){return type==WRITER;}

	virtual void errstr(const String&)=0;

	virtual bool good() const {return true;}

	virtual Serializer& tag(char ch)=0;
	virtual Serializer& tag(const char* msg)=0;

	virtual int local_version(int v)=0;

	int global_version(){return gver;}
	void global_version(int v){gver=v;}


	virtual void close(){}

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
public:

	enum
	{
		PTRTAG_NIL,		//NULL pointer
		PTRTAG_POD,		//POD pointer
		PTRTAG_CLS,		//class pointer (but NOT Object)
		PTRTAG_OBJ,		//Object and its derived class pointer
		PTRTAG_CACHED,
	};

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

	SerializerEx(int t);

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
	SerializerReader():SerializerEx(READER){}

	virtual bool recv(char* data,size_t size)=0;
	virtual void close(){}

	virtual SerializerReader& tag(char ch);
	virtual SerializerReader& tag(const char* msg);
	virtual int local_version(int v);

	template<typename T> void handle_array(T* ptr,int64_t size);
};

class VHWD_DLLIMPEXP SerializerWriter : public SerializerEx
{
public:

	SerializerWriter():SerializerEx(WRITER){}

	virtual bool send(char* data,size_t size)=0;
	virtual void close(){}

	virtual SerializerWriter& tag(char ch);
	virtual SerializerWriter& tag(const char* msg);

	virtual int local_version(int v);

	template<typename T> void handle_array(T* ptr,int64_t size);
};



template<typename T>
Serializer& operator &(Serializer& ar,T& val);

template<typename T>
SerializerReader& operator >>(SerializerReader& ar,T& val);

template<typename T>
SerializerWriter& operator <<(SerializerWriter& ar,T& val);

template<typename A,typename I,typename T>
class serial_cast
{
public:
	static void g(SerializerReader& ar,I& val_){T val;ar >> val;val_=val;}
	static void g(SerializerWriter& ar,I& val_){T val(val_);ar << val;}
};


template<typename A,typename T>
class serial_cast<A,bool,T>
{
public:
	typedef bool I;
	static void g(SerializerReader& ar,I& val_){T val;ar >> val;val_=val!=0;}
	static void g(SerializerWriter& ar,I& val_){T val(val_);ar << val;}
};



template<typename A,typename T>
class serial_pod
{
public:
	typedef T* pointer;

	static void g(SerializerReader& ar,T& val)
	{
		ar.recv((char*)&val,sizeof(T));
	}

	static void g(SerializerWriter& ar,T& val)
	{
		ar.send((char*)&val,sizeof(T));
	}

	static void g(SerializerReader& ar,pointer& val)
	{
		int32_t sval=A::PTRTAG_NIL;
		ar >> sval;
		if(sval==A::PTRTAG_NIL) {val=NULL;return;}
		if(sval!=A::PTRTAG_POD)
		{
			ar.errstr("INVALID_POINTER_TYPE");
			return;
		}

		val=new T();
		g(ar,*val);

	}

	static void g(SerializerWriter& ar,pointer& val)
	{
		int32_t sval=val==NULL?A::PTRTAG_NIL:A::PTRTAG_POD;
		ar << sval;
		if(sval==A::PTRTAG_NIL) return;
		g(ar,*val);
	}
};

template<typename A,typename T>
class serial_obj
{
public:
	typedef T* pointer;

	static void g(A& ar,Object& val)
	{
		ar.put(&val);
		val.Serialize(ar);
	}

	static void g(SerializerReader& ar,pointer& val)
	{
		int32_t sval=A::PTRTAG_NIL;
		ar >> sval;
		if(sval==A::PTRTAG_NIL) return;
		if(sval==A::PTRTAG_CACHED)
		{
			int32_t nidx(0);
			ar >> nidx;
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
		ar >> type;

		if(type=="")
		{
			ar.errstr("INVALID_OBJECT_TYPE");
			return;
		}
		Object* obj=ar.create(type);
		if(!obj)
		{
			ar.errstr("CANNOT_CREATE_OBJECT");
			return;
		}
		val=dynamic_cast<pointer>(obj);

		if(!val)
		{
			delete obj;
			ar.errstr("INCOMPATIBLE_OBJECT_TYPE");
			return;
		}

		g(ar,*val);
	}

	static void g(SerializerWriter& ar,pointer& val)
	{
		int32_t sval=val==NULL? A::PTRTAG_NIL : A::PTRTAG_OBJ;

		int32_t nidx=ar.get(val);
		if(nidx>0)
		{
			sval=A::PTRTAG_CACHED;
			ar << sval;
			ar << nidx;
			return;
		}

		ar << sval;
		if(sval==A::PTRTAG_NIL) return;


		String type=val->GetObjectName();
		ar << type;

		if(type=="")
		{
			ar.errstr("UNREGISTER_OBJECT_TYPE");
			return;
		}

		g(ar,*val);

	}
};

template<typename A,typename T,bool is_pod,bool is_obj>
class serial_helper;

template<typename A,typename T>
class serial_helper<A,T,true,false>
{
public:
	typedef T* pointer;

	static void g(A& ar,T& val)
	{
		serial_pod<A,T>::g(ar,val);
	}

	static void g(A& ar,pointer& val)
	{
		serial_pod<A,T>::g(ar,val);
	}
};


template<typename A,typename T>
class serial_helper<A,T,false,true>
{
public:
	typedef T* pointer;

	static void g(A& ar,T& val)
	{
		serial_obj<A,T>::g(ar,val);
	}

	static void g(A& ar,pointer& val)
	{
		serial_obj<A,T>::g(ar,val);
	}
};


template<typename A,typename T>
class serial_helper<A,T,false,false>
{
public:
	typedef T* pointer;

	static void g(A& ar,T& val)
	{
		val.Serialize(ar);
	}

	static void g(SerializerReader& ar,pointer& val)
	{
		int32_t sval(0);
		ar >> sval;
		if(sval==A::PTRTAG_NIL) return;
		if(sval!=A::PTRTAG_CLS)
		{
			ar.errstr("INVALID_POINTER_TYPE");
			return;
		}

		val=new T();
		g(ar,*val);

	}

	static void g(SerializerWriter& ar,pointer& val)
	{
		int32_t sval=val==NULL? A::PTRTAG_NIL : A::PTRTAG_CLS;
		ar << sval;
		if(sval==0) return;
		g(ar,*val);
	}

};

template<typename A,typename T>
class serial_helper_func : public serial_helper<A,T,tl::is_pod<T>::value,tl::is_convertible<T,Object>::value>
{
public:

};



template<typename A,typename T>
class serial_helper_func<A,T*> : public serial_helper<A,T,tl::is_pod<T>::value,tl::is_convertible<T,Object>::value>
{
public:

};



template<typename A,typename T,typename X>
class serial_helper_func<A,std::vector<T,X> >
{
public:
	typedef std::vector<T,X> type;

	static void g(SerializerReader& ar,type& val)
	{
		int64_t s=0;ar >> s;
		val.resize(s);
		if(s==0) return;
		ar.handle_array<T>(&val[0],s);
	}

	static void g(SerializerWriter& ar,type& val)
	{
		int64_t s=(int64_t)val.size();
		ar << s;
		if(s==0) return;
		ar.handle_array<T>(&val[0],s);
	}

};

template<typename A,typename K,typename T,typename X>
class serial_helper_func<A,std::map<K,T,X> >
{
public:
	typedef std::map<K,T,X> type;
	typedef typename type::iterator iterator;

	static void g(SerializerReader& ar,type& val)
	{
		int64_t _size=0;
		ar & _size;
		std::pair<K,T> pa;
		for(int i=0;i<_size;i++)
		{
			ar & pa.first;
			ar & pa.second;
			val.insert(pa);
		}
	}

	static void g(SerializerWriter& ar,type& val)
	{
		int64_t _size=(int64_t)val.size();
		ar << _size;
		for(iterator it=val.begin();it!=val.end();it++)
		{
			ar << (*it).first;
			ar << (*it).second;
			_size--;
		}
		if(_size!=0)
		{
			ar.errstr("unknown_error");
		}
	}

};
template<typename A,typename T,size_t N>
class serial_helper_func<A,T[N] >
{
public:
	typedef T type[N];

	static void g(SerializerReader& ar,type& val)
	{
		int64_t _size(0);ar >> _size;
		if(_size!=(int64_t)N) ar.errstr("invalid_array_size");
		ar.handle_array<T>(&val[0],_size);
	}

	static void g(SerializerWriter& ar,type& val)
	{
		int64_t _size=(int64_t)N;ar << _size;
		ar.handle_array<T>(&val[0],_size);
	}

};

template<typename A,typename T>
class serial_helper_func<A,LitePtrT<T> >
{
public:
	typedef LitePtrT<T> type;
	typedef T* pointer;
	static void g(SerializerReader& ar,type& val)
	{
		pointer ptr(NULL);
		ar >> ptr;
		val.reset(ptr);
	}

	static void g(SerializerWriter& ar,type& val)
	{
		pointer ptr(val.get());
		ar << ptr;
	}
};


template<typename A,typename T>
class serial_helper_func<A,DataPtrT<T> >
{
public:
	typedef DataPtrT<T> type;
	typedef T* pointer;
	static void g(SerializerReader& ar,type& val)
	{
		pointer ptr(NULL);
		ar >> ptr;
		val.reset(ptr);
	}

	static void g(SerializerWriter& ar,type& val)
	{
		pointer ptr(val.get());
		ar << ptr;
	}
};

template<typename A,typename X,typename Y>
class serial_helper_func<A,std::basic_string<char,X,Y> >
{
public:
	typedef std::basic_string<char,X,Y> type;

	static void g(SerializerReader& ar,type& val)
	{
		int64_t size=0;ar >> size;
		if(size==0) return;
		std::vector<char> vect;vect.resize(size);
		ar.handle_array<char>(&vect[0],size);
		val.assign(vect.begin(),vect.end());
	}

	static void g(SerializerWriter& ar,type& val)
	{
		int64_t size=(int64_t)val.size();ar << size;
		if(size==0) return;
		ar.handle_array<char>((char*)val.c_str(),size);
	}
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
		int64_t vals[type::MAX_DIM];
		for(int i=0;i<(int)type::MAX_DIM;i++)
		{
			ar & vals[i];
		}
		val.resize(vals[0],vals[1],vals[2],vals[3],vals[4],vals[5]);
		ar.handle_array<T>(val.data(),val.size());
	}

	static void g(SerializerWriter& ar,type& val)
	{
		int64_t vals[type::MAX_DIM];
		for(int i=0;i<(int)type::MAX_DIM;i++)
		{
			vals[i]=val.size(i);
			ar & vals[i];
		}
		ar.handle_array<T>(val.data(),val.size());
	}
};


template<typename A,typename T,typename X>
class serial_helper_func<A,arr_1t<T,X> >
{
public:
	typedef arr_1t<T,X> type;

	static void g(SerializerReader& ar,type& val)
	{
		int64_t sz(0);
		ar & sz;

		if(sz<0)
		{
			ar.errstr("invalid_array_size");
			return;
		}

		val.resize(sz);
		ar.handle_array<T>(val.data(),sz);

	}

	static void g(SerializerWriter& ar,type& val)
	{
		int64_t sz=(int64_t)val.size();
		ar & sz;
		ar.handle_array<T>(val.data(),sz);
	}
};

template<typename A> class serial_helper_func<A,bool> : public serial_cast<A,bool,int32_t>{};

template<typename A> class serial_helper_func<A,char> : public serial_pod<A,char>{};
template<typename A> class serial_helper_func<A,int32_t> : public serial_pod<A,int32_t>{};
template<typename A> class serial_helper_func<A,int64_t> : public serial_pod<A,int64_t>{};
template<typename A> class serial_helper_func<A,uint32_t> : public serial_pod<A,uint32_t>{};
template<typename A> class serial_helper_func<A,uint64_t> : public serial_pod<A,uint64_t>{};
template<typename A> class serial_helper_func<A,float32_t> : public serial_pod<A,float32_t>{};
template<typename A> class serial_helper_func<A,float64_t> : public serial_pod<A,float64_t>{};

template<typename A,typename T> class serial_helper_func<A,std::complex<T> > : public serial_pod<A,std::complex<T> >{};

template<typename A,typename T>
class serial_helper_test
{
public:
	static void g(A& ar,T& val)
	{
		serial_helper_func<A,T>::g(ar,val);
	}
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
		(void)&ar;(void)&val;
		Exception::XError("attempt_to_serialize_const");
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
Serializer& operator &(Serializer& ar,T& val)
{
	if(ar.is_reader())
	{
		return static_cast<SerializerReader&>(ar) >> val;
	}

	if(ar.is_writer())
	{
		return static_cast<SerializerWriter&>(ar) << val;
	}

	ar.errstr("INVALID_SERIALIZER");
	return ar;
}


template<typename T>
SerializerReader& operator &(SerializerReader& ar,T& val)
{
	ar >> val;
	return ar;
}

template<typename T>
SerializerWriter& operator &(SerializerWriter& ar,T& val)
{
	ar << val;
	return ar;
}

template<typename T,bool pod>
class handle_array_helper;

template<typename T>
class handle_array_helper<T,true>
{
public:

	static void g(SerializerReader& ar,T* ptr,int64_t size)
	{
		ar.recv((char*)ptr,sizeof(T)*size);
	}

	static void g(SerializerWriter& ar,T* ptr,int64_t size)
	{
		ar.send((char*)ptr,sizeof(T)*size);
	}
};


template<typename T>
class handle_array_helper<T,false>
{
public:
	static void g(SerializerReader& ar,T* ptr,int64_t s)
	{
		for(int64_t i=0;i<s;i++)
		{
			ar >> ptr[i];
		}
	}

	static void g(SerializerWriter& ar,T* ptr,int64_t s)
	{
		for(int64_t i=0;i<s;i++)
		{
			ar << ptr[i];
		}
	}
};


template<>class handle_array_helper<int32_t,false> : public handle_array_helper<int32_t,true>{};
template<>class handle_array_helper<int64_t,false> : public handle_array_helper<int64_t,true>{};
template<>class handle_array_helper<uint32_t,false> : public handle_array_helper<uint32_t,true>{};
template<>class handle_array_helper<uint64_t,false> : public handle_array_helper<uint64_t,true>{};
template<>class handle_array_helper<float32_t,false> : public handle_array_helper<float32_t,true>{};
template<>class handle_array_helper<float64_t,false> : public handle_array_helper<float64_t,true>{};

template<>class handle_array_helper<char,false> : public handle_array_helper<char,true>{};
template<typename T>class handle_array_helper<std::complex<T>,false> : public handle_array_helper<std::complex<T>,true>{};


template<typename T> void SerializerReader::handle_array(T* ptr,int64_t s)
{
	handle_array_helper<T,false>::g(*this,ptr,s);
}

template<typename T> void SerializerWriter::handle_array(T* ptr,int64_t s)
{
	handle_array_helper<T,false>::g(*this,ptr,s);
}

VHWD_LEAVE
#endif
