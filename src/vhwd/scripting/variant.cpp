#include "vhwd/scripting/variant.h"
#include "vhwd/scripting/variant_op1.h"
#include "vhwd/scripting/variant_op2.h"
#include "vhwd/scripting/executor.h"

VHWD_ENTER

CallableIterator* CallableData::GetIterator(int)
{
	return new CallableIterator;
}

int CallableIterator::__fun_call(Executor& ks)
{
	ks.stack[ks.stack.vbp.back()+1].reset<bool>(false);
	return 1;
}

Variant& Variant::operator=(const Variant& o)
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


void Variant::kptr(CallableData* p)
{
	if(p) p->IncRef();
	clear();
	data.pVal=p;

}


CallableData* Variant::kptr()
{
	if(bptr())
	{
		return data.pVal;
	}
	return NULL;
}

const CallableData* Variant::kptr() const
{
	if(bptr())
	{
		return data.pVal;
	}
	return NULL;
}

void Variant::swap(Variant& o)
{
	std::swap(o.data,data);
}


void kvar_base::clear()
{
	if(bptr() && pVal)
	{
		pVal->DecRef();
	}
	flag=Variant::V_OBJECT;
	pVal=NULL;
}

void Variant::clear()
{
	data.clear();
}

Variant::Variant(const Variant&o):data(o.data)
{
	if(bptr() && data.pVal)
	{
		data.pVal->IncRef();
	}
}


class kvar_policy_equal
{
public:
	typedef bool type;

	static type invalid_type()
	{
		return false;
	}

	template<int N>
	class rebind
	{
	public:
		static type g(const Variant& v1,const Variant& v2)
		{
			kvar_base& lhs((kvar_base&)v1);
			kvar_base& rhs((kvar_base&)v2);
			if(N==7)
			{
				return lhs.pVal==rhs.pVal||(lhs.pVal&&lhs.pVal->IsEqualTo(rhs.pVal));
			}
			else
			{
				return kvar_type<N>::raw((kvar_base&)(lhs))== kvar_type<N>::raw((kvar_base&)(rhs));
			}
		}
	};
};

bool Variant::operator==(const Variant& v2) const
{
	const Variant& v1(*this);
	if(v1.flag()!=v2.flag()) return false;
	return kvar_handler1<kvar_policy_equal>::g2(v1,v2,v1.flag());
}

bool Variant::operator!=(const Variant& v2) const
{
	const Variant& v1(*this);
	return !(v1==v2);
}


class kvar_policy_hash
{
public:
	typedef uint32_t type;

	static type invalid_type()
	{
		return type(-1);
	}

	template<int N>
	class rebind
	{
	public:
		static type g(Variant& v)
		{
			typedef typename kvar_type<N>::type rawtype;
			hash_t<rawtype> h;return h(kvar_type<N>::raw(v));
		}
	};
};


uint32_t hash_t<Variant>::operator()(const Variant& v)
{
	return kvar_handler1<kvar_policy_hash>::g(v);
}



VHWD_LEAVE
