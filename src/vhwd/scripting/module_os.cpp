
#include "module.h"

VHWD_ENTER


class VHWD_DLLIMPEXP CallableTimePoint : public CallableValueT<TimePoint>
{
public:
	CallableTimePoint(){value=Clock::now();}


	virtual bool ToValue(String& v) const 
	{
		TimeDetail td(value);
		v=td.Format();
		return true;
	}

	virtual bool ToValue(int64_t& v) const 
	{
		v=value.val;
		return true;
	}

	virtual bool ToValue(double& v) const 
	{
		v=value.val;
		return true;
	}

};


class VHWD_DLLIMPEXP CallableTimeSpan : public CallableValueT<TimeSpan>
{
public:
	CallableTimeSpan(){}
	CallableTimeSpan(const TimeSpan& ts){value=ts;}

	virtual bool ToValue(int64_t& v) const 
	{
		v=value/TimeSpan::Seconds(1);
		return true;
	}

	virtual bool ToValue(double& v) const 
	{
		v=value/TimeSpan::Seconds(1);
		return true;
	}

};

class VHWD_DLLIMPEXP CallableFunctionTime : public CallableFunction
{
public:
	int __fun_call(Executor& ks)
	{
		int nbp=ks.stack.vbp.back();
		ks.stack[nbp+1].kptr(new CallableTimePoint);
		return 1;
	}
};

class VHWD_DLLIMPEXP CallableFunctionTimeDiff : public CallableFunction
{
public:
	int __fun_call(Executor& ks)
	{
		int nbp=ks.stack.vbp.back();
		int pmc=ks.stack.nsp-nbp;
		if(pmc!=2)
		{
			return -1;
		}

		TimeSpan ts(PLCast<int64_t>::g(ks.get(0))-PLCast<int64_t>::g(ks.get(-1)));
		ks.stack[nbp+1].reset<double>(ts/TimeSpan::Seconds(1));
		return 1;
	}
};

void init_scripting_module_os()
{
	kvar_table& ggvar(Executor::ggvar);
	kvar_table& os(ggvar["os"].ref<kvar_table>());
	os["time"].kptr(new CallableFunctionTime);
	os["timespan"].kptr(new CallableFunctionTimeDiff);
}

VHWD_LEAVE


