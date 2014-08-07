#include "vhwd/scripting/executor.h"
#include "vhwd/scripting/variant_code.h"
#include "vhwd/scripting/variant_op2.h"
VHWD_ENTER

CallableCode::CallableCode()
{
	nLocal=0;
	npThis=0;
}


int CallableCode::__fun_call(Executor& ks)
{
	if(aInsts.empty())
	{
		return 0;
	}

	size_t nbp=ks.stack.vbp.back();
	size_t nsp=ks.stack.nsp;

	ks.stack.nsp+=nLocal;

	if(npThis>0)
	{
		ks.stack[nsp+npThis]=ks.stack[0];
	}

	size_t cur=0;

	for(;;)
	{
		const kvar_inst& x(aInsts[cur]);
		switch(x.inst)
		{
		case XOP_NOOP:
			break;
		case XOP2_ADD:
			ks.call2<XOP2_ADD>();
			break;
		case XOP2_SUB:
			ks.call2<XOP2_SUB>();
			break;
		case XOP2_MUL:
			ks.call2<XOP2_MUL>();
			break;
		case XOP2_DIV:
			ks.call2<XOP2_DIV>();
			break;
		case XOP2_MOD:
			ks.call2<XOP2_MOD>();
			break;
		case XOP2_POW:
			ks.call2<XOP2_POW>();
			break;
		case XOP2_DOT_MUL:
			ks.call2<XOP2_MUL>();
			break;
		case XOP2_DOT_DIV:
			ks.call2<XOP2_DIV>();
			break;

		case XOP2_GT:
			ks.call2<XOP2_GT>();
			break;
		case XOP2_LT:
			ks.call2<XOP2_LT>();
			break;
		case XOP2_GE:
			ks.call2<XOP2_GE>();
			break;
		case XOP2_LE:
			ks.call2<XOP2_LE>();
			break;
		case XOP2_NE:
			ks.call2<XOP2_NE>();
			break;
		case XOP2_EQ:
			ks.call2<XOP2_EQ>();
			break;

		case XOP2_AND:
			ks.call2<XOP2_AND>();
			break;
		case XOP2_OR:
			ks.call2<XOP2_OR>();
			break;
		case XOP2_XOR:
			ks.call2<XOP2_XOR>();
			break;
		case XOP2_BITWISE_AND:
			ks.call2<XOP2_BITWISE_AND>();
			break;
		case XOP2_BITWISE_OR:
			ks.call2<XOP2_BITWISE_OR>();
			break;
		case XOP2_BITWISE_XOR:
			ks.call2<XOP2_BITWISE_XOR>();
			break;
		case XOP1_INV:
			ks.call1<XOP1_INV>();
			break;
		case XOP1_SIN:
			ks.call1<XOP1_SIN>();
			break;
		case XOP1_COS:
			ks.call1<XOP1_COS>();
			break;
		case XOP1_TAN:
			ks.call1<XOP1_TAN>();
			break;
		case XOP1_LOG:
			ks.call1<XOP1_LOG>();
			break;
		case XOP1_LOG10:
			ks.call1<XOP1_LOG10>();
			break;
		case XOP1_BITWISE_INV:
			ks.call1<XOP1_BITWISE_INV>();
			break;
		case XOP_PUSH_NIL:
			for(int i=0;i<x.parm;i++){ks.push();}
			break;
		case XOP_GET_LLVAR:
			ks.push(aConst[x.parm]);
			break;
		case XOP_PUSH_INT_DIRECT:
			ks.push((int64_t)x.parm);
			break;
		case XOP_PUSH_BOOL:
			ks.push(x.parm!=0);
			break;
		case XOP_SPADJ:
			ks.sadj(x.parm);
			break;
		case XOP_GET_GGVAR:
			ks.push(Executor::ggvar.get(x.parm).second);
			break;
		case XOP_SET_TABLE:
			ks.set_table(aConst[x.parm].ref<String>());
			break;
		case XOP_GET_TABLE:
			ks.get_table(aConst[x.parm].ref<String>());
			break;
		case XOP_SET_INDEX:
			ks.set_index(aConst[x.parm].ref<String>());
			break;
		case XOP_GET_INDEX:
			ks.get_index(aConst[x.parm].ref<String>());
			break;
		case XOP_SET_ARRAY:
			ks.set_array();
			break;
		case XOP_GET_ARRAY:
			ks.get_array();
			break;
		case XOP_SET_PARAM:
			ks.set_local(nbp+x.parm);
			break;
		case XOP_GET_PARAM:
			ks.get_local(nbp+x.parm);
			break;
		case XOP_SET_LOCAL:
			ks.set_local(nsp+x.parm);
			break;
		case XOP_GET_STACK:
			ks.stack[ks.stack.nsp+1]=ks.stack[ks.stack.nsp+x.parm];
			ks.stack.nsp+=1;
			break;
		case XOP_SET_STACK:
			ks.stack[ks.stack.nsp+x.parm]=ks.stack[ks.stack.nsp];
			ks.stack.nsp-=1;
			break;
		case XOP_GET_LOCAL:
			ks.get_local(nsp+x.parm);
			break;
		case XOP_PROG1:
			ks.stack.vbp.push_back(ks.stack.nsp);
			break;
		case XOP_PROG2:
			ks.stack.vbp.pop_back();	
			break;
		case XOP_CALL:
			ks.callx();
			break;
		case XOP_CALL_THIS:
			if(ks.stack.nsp-ks.stack.vbp.back()!=1)
			{
				Exception::XError("invalid this call");
			}
			ks.stack[0]=ks.stack[ks.stack.vbp.back()+1];
			ks.stack.nsp=ks.stack.vbp.back();
			ks.stack.vbp.pop_back();
			ks.callx();
			break;
		case XOP_JUMP:
			cur=x.parm;
			continue;
			break;
		case XOP_JUMP_N:
			cur=PLCast<int64_t>::g(ks.stack[ks.stack.nsp--]);
			continue;
		case XOP_JUMP_TRUE:			
			if(ks.test())
			{
				cur=x.parm;
				continue;
			}
			break;
		case XOP_JUMP_FALSE:
			if(!ks.test())
			{
				cur=x.parm;
				continue;
			}
			break;
		case XOP_PMADJ:
			{
				size_t _bp=ks.stack.vbp.back();
				size_t _rc=ks.stack.nsp-_bp;
				size_t _lc=x.parm;
				if(_rc>=_lc) // rvalue count > lvalue count drop
				{
					ks.sadj(_lc-_rc);
				}
				else // fill nil
				{
					for(size_t i=_rc;i<_lc;i++)
					{
						ks.push();
					}
				}
			}
			break;
		case XOP_RETURN:
			{
				size_t _bp=ks.stack.vbp.back();
				size_t ret=ks.stack.nsp-_bp;
				ks.stack.vbp.pop_back();
				for(size_t i=1;i<=ret;i++)
				{
					ks.stack[nbp+i]=ks.stack[_bp+i];
				}
				ks.stack.nsp=nsp;
				return ret;	
			}
		case XOP_HALT:
			ks.stack.nsp=nsp;
			return 0;
		default:
			Exception::XError("unkown instruction");
		}

		++cur;
	}

	ks.stack.nsp=nsp;
	return 0;

}


VHWD_LEAVE
