#ifndef __H_VHWD_SCRIPTING_EXECUTOR__
#define __H_VHWD_SCRIPTING_EXECUTOR__

#include "vhwd/scripting/variant.h"

#include "vhwd/scripting/instruction.h"

VHWD_ENTER



class kvar_stack
{
public:

	arr_1t<Variant> tbl;
	arr_1t<size_t> vbp;
	intptr_t nsp;

	kvar_stack();

	void swap(kvar_stack& o);

	inline Variant& operator[](size_t n){return tbl[n];}

};


class Executor
{
public:

	friend class CallableCode;

	static kvar_table ggvar;
	kvar_table table;
	kvar_stack stack;

	Executor();

	bool execute(const String& s);

	inline void push(){stack[++stack.nsp].clear();}
	inline void push(Variant& o){stack[++stack.nsp]=o;}
	inline void push(const kvar_table& t){stack[++stack.nsp].reset<kvar_table>(t);}
	inline void push(bool s){stack[++stack.nsp].reset<bool>(s);}
	inline void push(int32_t s){stack[++stack.nsp].reset<int64_t>(s);}
	inline void push(int64_t s){stack[++stack.nsp].reset<int64_t>(s);}
	inline void push(double s){stack[++stack.nsp].reset<double>(s);}
	inline void push(const char* s){stack[++stack.nsp].reset<String>(s);}
	inline void push(const String& s){stack[++stack.nsp].reset<String>(s);}
	inline void push(CallableData* d){stack[++stack.nsp].kptr(d);}

	inline void popq(Variant& o){o=stack[stack.nsp--];}
	inline void popq(){--stack.nsp;}

	inline void sadj(int n){stack.nsp+=n;}

	Variant& get(int n){return stack[stack.nsp+n];}
	Variant& top();

	template<int N>
	void call1();

	template<int N>
	void call2();

	void calln(int n);

	void get_index(const String& s);
	void set_index(const String& s);
	void get_table(const String& s){push(table[s]);}
	void set_table(const String& s){popq(table[s]);}
	void get_table(size_t n){push(table.get(n).second);}
	void set_table(size_t n){popq(table.get(n).second);}
	void get_ggvar(const String& s){push(ggvar[s]);}

	void get_array(int n);
	void set_array(int n);

	void kerror(const String& s);

	template<typename T>
	T eval(const String &s);

	template<typename T>
	bool eval(const String& s,T& v);

	Logger logger;

protected:

	bool test();
	void get_array();
	void set_array();
	void get_local(int n);
	void set_local(int n);
	void get_ggvar(int n){push(ggvar.get(n).second);}


	void callx();

	CallableData* kptr1();
	CallableData* kptr2();

};


class kvar_helper_base
{
public:
	static int f1(Executor& ks)
	{
		return ks.top().flag();
	}

	static int f2(Executor& ks)
	{
		return (ks.get(-1).flag()<<Variant::V_FLAGMOVE)+ks.top().flag();
	}

	static int f1(const Variant& v1)
	{
		return v1.flag();
	}

	static int f2(const Variant& v1,const Variant& v2)
	{
		return (v1.flag()<<Variant::V_FLAGMOVE)+v2.flag();
	}
};

template<int N>
class kvar_helper : public kvar_helper_base
{
public:
	static void g1(Executor& ks);
	static void g2(Executor& ks);
};


template<int N>
inline void Executor::call2()
{
	kvar_helper<N>::g2(*this);
	popq();
}

template<int N>
inline void Executor::call1()
{
	kvar_helper<N>::g1(*this);
}


VHWD_LEAVE
#endif
