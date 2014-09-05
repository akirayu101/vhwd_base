#ifndef __H_VHWD_SCRIPTING_VARIANT_OP__
#define __H_VHWD_SCRIPTING_VARIANT_OP__

#include "vhwd/scripting/variant.h"
//#include "vhwd/scripting/executor.h"

VHWD_ENTER

typedef tl::mk_typelist<bool,int64_t,double,std::complex<double> >::type kvar_scalar_type;

class Executor;

template<int N1,int N2=0>
class kvar_opx_num
{
public:
	static const int value1=N1;
	static const int value2=N2;
	static const int scalar1=N1&Variant::V_SCRMASK;
	static const int scalar2=N2&Variant::V_SCRMASK;
	static const int amask=(N1&Variant::V_ARRMASK)||(N2&Variant::V_ARRMASK);
	static const int scalar=scalar1>scalar2?scalar1:scalar2;
	static const int value=((scalar>0?scalar:1)&Variant::V_SCRMASK)+(amask?Variant::V_ARRMASK:0);
	static const bool valid=scalar<=Variant::V_COMPLEX&&scalar2!=Variant::V_COMPOUND;
};


class PL1_num1
{
public:
	template<typename T1>
	class rebind
	{
	public:
		static const int id1=kvar_scalar_type::template id<T1>::value;
		static const int id0=id1>1?id1:1;
		typedef typename kvar_scalar_type::template at<id0>::type type;
	};
};

class PL1_num2
{
public:
	template<typename T1>
	class rebind
	{
	public:
		static const int id1=kvar_scalar_type::template id<T1>::value;
		static const int id0=id1>2?id1:2;
		typedef typename kvar_scalar_type::template at<id0>::type type;
	};
};

VHWD_LEAVE
#endif
