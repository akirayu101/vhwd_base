#ifndef __H_VHWD_SCRIPTING_VARIANT_OP2__
#define __H_VHWD_SCRIPTING_VARIANT_OP2__

#include "vhwd/scripting/variant_op1.h"

VHWD_ENTER


template<typename P>
class kvar_handler2
{
public:
	typedef typename P::type type;

	static const int M=Variant::V_FLAGMOVE;

	static type g(const Variant& v1,const Variant& v2)
	{
		switch(kvar_helper_base::f2(v1,v2))
		{
		case (0<<M)+0: return P::template rebind<0,0>::g(v1,v2);case (0<<M)+1: return P::template rebind<0,1>::g(v1,v2);case (0<<M)+2: return P::template rebind<0,2>::g(v1,v2);case (0<<M)+3: return P::template rebind<0,3>::g(v1,v2);
		case (0<<M)+4: return P::template rebind<0,4>::g(v1,v2);case (0<<M)+5: return P::template rebind<0,5>::g(v1,v2);case (0<<M)+6: return P::template rebind<0,6>::g(v1,v2);case (0<<M)+7: return P::template rebind<0,7>::g(v1,v2);
		case (0<<M)+8: return P::template rebind<0,8>::g(v1,v2);case (0<<M)+9: return P::template rebind<0,9>::g(v1,v2);case (0<<M)+10: return P::template rebind<0,10>::g(v1,v2);case (0<<M)+11: return P::template rebind<0,11>::g(v1,v2);
		case (0<<M)+12: return P::template rebind<0,12>::g(v1,v2);case (0<<M)+13: return P::template rebind<0,13>::g(v1,v2);case (0<<M)+14: return P::template rebind<0,14>::g(v1,v2);case (0<<M)+15: return P::template rebind<0,15>::g(v1,v2);

		case (1<<M)+0: return P::template rebind<1,0>::g(v1,v2);case (1<<M)+1: return P::template rebind<1,1>::g(v1,v2);case (1<<M)+2: return P::template rebind<1,2>::g(v1,v2);case (1<<M)+3: return P::template rebind<1,3>::g(v1,v2);
		case (1<<M)+4: return P::template rebind<1,4>::g(v1,v2);case (1<<M)+5: return P::template rebind<1,5>::g(v1,v2);case (1<<M)+6: return P::template rebind<1,6>::g(v1,v2);case (1<<M)+7: return P::template rebind<1,7>::g(v1,v2);
		case (1<<M)+8: return P::template rebind<1,8>::g(v1,v2);case (1<<M)+9: return P::template rebind<1,9>::g(v1,v2);case (1<<M)+10: return P::template rebind<1,10>::g(v1,v2);case (1<<M)+11: return P::template rebind<1,11>::g(v1,v2);
		case (1<<M)+12: return P::template rebind<1,12>::g(v1,v2);case (1<<M)+13: return P::template rebind<1,13>::g(v1,v2);case (1<<M)+14: return P::template rebind<1,14>::g(v1,v2);case (1<<M)+15: return P::template rebind<1,15>::g(v1,v2);

		case (2<<M)+0: return P::template rebind<2,0>::g(v1,v2);case (2<<M)+1: return P::template rebind<2,1>::g(v1,v2);case (2<<M)+2: return P::template rebind<2,2>::g(v1,v2);case (2<<M)+3: return P::template rebind<2,3>::g(v1,v2);
		case (2<<M)+4: return P::template rebind<2,4>::g(v1,v2);case (2<<M)+5: return P::template rebind<2,5>::g(v1,v2);case (2<<M)+6: return P::template rebind<2,6>::g(v1,v2);case (2<<M)+7: return P::template rebind<2,7>::g(v1,v2);
		case (2<<M)+8: return P::template rebind<2,8>::g(v1,v2);case (2<<M)+9: return P::template rebind<2,9>::g(v1,v2);case (2<<M)+10: return P::template rebind<2,10>::g(v1,v2);case (2<<M)+11: return P::template rebind<2,11>::g(v1,v2);
		case (2<<M)+12: return P::template rebind<2,12>::g(v1,v2);case (2<<M)+13: return P::template rebind<2,13>::g(v1,v2);case (2<<M)+14: return P::template rebind<2,14>::g(v1,v2);case (2<<M)+15: return P::template rebind<2,15>::g(v1,v2);

		case (3<<M)+0: return P::template rebind<3,0>::g(v1,v2);case (3<<M)+1: return P::template rebind<3,1>::g(v1,v2);case (3<<M)+2: return P::template rebind<3,2>::g(v1,v2);case (3<<M)+3: return P::template rebind<3,3>::g(v1,v2);
		case (3<<M)+4: return P::template rebind<3,4>::g(v1,v2);case (3<<M)+5: return P::template rebind<3,5>::g(v1,v2);case (3<<M)+6: return P::template rebind<3,6>::g(v1,v2);case (3<<M)+7: return P::template rebind<3,7>::g(v1,v2);
		case (3<<M)+8: return P::template rebind<3,8>::g(v1,v2);case (3<<M)+9: return P::template rebind<3,9>::g(v1,v2);case (3<<M)+10: return P::template rebind<3,10>::g(v1,v2);case (3<<M)+11: return P::template rebind<3,11>::g(v1,v2);
		case (3<<M)+12: return P::template rebind<3,12>::g(v1,v2);case (3<<M)+13: return P::template rebind<3,13>::g(v1,v2);case (3<<M)+14: return P::template rebind<3,14>::g(v1,v2);case (3<<M)+15: return P::template rebind<3,15>::g(v1,v2);

		case (4<<M)+0: return P::template rebind<4,0>::g(v1,v2);case (4<<M)+1: return P::template rebind<4,1>::g(v1,v2);case (4<<M)+2: return P::template rebind<4,2>::g(v1,v2);case (4<<M)+3: return P::template rebind<4,3>::g(v1,v2);
		case (4<<M)+4: return P::template rebind<4,4>::g(v1,v2);case (4<<M)+5: return P::template rebind<4,5>::g(v1,v2);case (4<<M)+6: return P::template rebind<4,6>::g(v1,v2);case (4<<M)+7: return P::template rebind<4,7>::g(v1,v2);
		case (4<<M)+8: return P::template rebind<4,8>::g(v1,v2);case (4<<M)+9: return P::template rebind<4,9>::g(v1,v2);case (4<<M)+10: return P::template rebind<4,10>::g(v1,v2);case (4<<M)+11: return P::template rebind<4,11>::g(v1,v2);
		case (4<<M)+12: return P::template rebind<4,12>::g(v1,v2);case (4<<M)+13: return P::template rebind<4,13>::g(v1,v2);case (4<<M)+14: return P::template rebind<4,14>::g(v1,v2);case (4<<M)+15: return P::template rebind<4,15>::g(v1,v2);

		case (5<<M)+0: return P::template rebind<5,0>::g(v1,v2);case (5<<M)+1: return P::template rebind<5,1>::g(v1,v2);case (5<<M)+2: return P::template rebind<5,2>::g(v1,v2);case (5<<M)+3: return P::template rebind<5,3>::g(v1,v2);
		case (5<<M)+4: return P::template rebind<5,4>::g(v1,v2);case (5<<M)+5: return P::template rebind<5,5>::g(v1,v2);case (5<<M)+6: return P::template rebind<5,6>::g(v1,v2);case (5<<M)+7: return P::template rebind<5,7>::g(v1,v2);
		case (5<<M)+8: return P::template rebind<5,8>::g(v1,v2);case (5<<M)+9: return P::template rebind<5,9>::g(v1,v2);case (5<<M)+10: return P::template rebind<5,10>::g(v1,v2);case (5<<M)+11: return P::template rebind<5,11>::g(v1,v2);
		case (5<<M)+12: return P::template rebind<5,12>::g(v1,v2);case (5<<M)+13: return P::template rebind<5,13>::g(v1,v2);case (5<<M)+14: return P::template rebind<5,14>::g(v1,v2);case (5<<M)+15: return P::template rebind<5,15>::g(v1,v2);

		case (6<<M)+0: return P::template rebind<6,0>::g(v1,v2);case (6<<M)+1: return P::template rebind<6,1>::g(v1,v2);case (6<<M)+2: return P::template rebind<6,2>::g(v1,v2);case (6<<M)+3: return P::template rebind<6,3>::g(v1,v2);
		case (6<<M)+4: return P::template rebind<6,4>::g(v1,v2);case (6<<M)+5: return P::template rebind<6,5>::g(v1,v2);case (6<<M)+6: return P::template rebind<6,6>::g(v1,v2);case (6<<M)+7: return P::template rebind<6,7>::g(v1,v2);
		case (6<<M)+8: return P::template rebind<6,8>::g(v1,v2);case (6<<M)+9: return P::template rebind<6,9>::g(v1,v2);case (6<<M)+10: return P::template rebind<6,10>::g(v1,v2);case (6<<M)+11: return P::template rebind<6,11>::g(v1,v2);
		case (6<<M)+12: return P::template rebind<6,12>::g(v1,v2);case (6<<M)+13: return P::template rebind<6,13>::g(v1,v2);case (6<<M)+14: return P::template rebind<6,14>::g(v1,v2);case (6<<M)+15: return P::template rebind<6,15>::g(v1,v2);

		case (7<<M)+0: return P::template rebind<7,0>::g(v1,v2);case (7<<M)+1: return P::template rebind<7,1>::g(v1,v2);case (7<<M)+2: return P::template rebind<7,2>::g(v1,v2);case (7<<M)+3: return P::template rebind<7,3>::g(v1,v2);
		case (7<<M)+4: return P::template rebind<7,4>::g(v1,v2);case (7<<M)+5: return P::template rebind<7,5>::g(v1,v2);case (7<<M)+6: return P::template rebind<7,6>::g(v1,v2);case (7<<M)+7: return P::template rebind<7,7>::g(v1,v2);
		case (7<<M)+8: return P::template rebind<7,8>::g(v1,v2);case (7<<M)+9: return P::template rebind<7,9>::g(v1,v2);case (7<<M)+10: return P::template rebind<7,10>::g(v1,v2);case (7<<M)+11: return P::template rebind<7,11>::g(v1,v2);
		case (7<<M)+12: return P::template rebind<7,12>::g(v1,v2);case (7<<M)+13: return P::template rebind<7,13>::g(v1,v2);case (7<<M)+14: return P::template rebind<7,14>::g(v1,v2);case (7<<M)+15: return P::template rebind<7,15>::g(v1,v2);


		case ((0+8)<<M)+0: return P::template rebind<(0+8),0>::g(v1,v2);case ((0+8)<<M)+1: return P::template rebind<(0+8),1>::g(v1,v2);case ((0+8)<<M)+2: return P::template rebind<(0+8),2>::g(v1,v2);case ((0+8)<<M)+3: return P::template rebind<(0+8),3>::g(v1,v2);
		case ((0+8)<<M)+4: return P::template rebind<(0+8),4>::g(v1,v2);case ((0+8)<<M)+5: return P::template rebind<(0+8),5>::g(v1,v2);case ((0+8)<<M)+6: return P::template rebind<(0+8),6>::g(v1,v2);case ((0+8)<<M)+7: return P::template rebind<(0+8),7>::g(v1,v2);
		case ((0+8)<<M)+8: return P::template rebind<(0+8),8>::g(v1,v2);case ((0+8)<<M)+9: return P::template rebind<(0+8),9>::g(v1,v2);case ((0+8)<<M)+10: return P::template rebind<(0+8),10>::g(v1,v2);case ((0+8)<<M)+11: return P::template rebind<(0+8),11>::g(v1,v2);
		case ((0+8)<<M)+12: return P::template rebind<(0+8),12>::g(v1,v2);case ((0+8)<<M)+13: return P::template rebind<(0+8),13>::g(v1,v2);case ((0+8)<<M)+14: return P::template rebind<(0+8),14>::g(v1,v2);case ((0+8)<<M)+15: return P::template rebind<(0+8),15>::g(v1,v2);

		case ((1+8)<<M)+0: return P::template rebind<(1+8),0>::g(v1,v2);case ((1+8)<<M)+1: return P::template rebind<(1+8),1>::g(v1,v2);case ((1+8)<<M)+2: return P::template rebind<(1+8),2>::g(v1,v2);case ((1+8)<<M)+3: return P::template rebind<(1+8),3>::g(v1,v2);
		case ((1+8)<<M)+4: return P::template rebind<(1+8),4>::g(v1,v2);case ((1+8)<<M)+5: return P::template rebind<(1+8),5>::g(v1,v2);case ((1+8)<<M)+6: return P::template rebind<(1+8),6>::g(v1,v2);case ((1+8)<<M)+7: return P::template rebind<(1+8),7>::g(v1,v2);
		case ((1+8)<<M)+8: return P::template rebind<(1+8),8>::g(v1,v2);case ((1+8)<<M)+9: return P::template rebind<(1+8),9>::g(v1,v2);case ((1+8)<<M)+10: return P::template rebind<(1+8),10>::g(v1,v2);case ((1+8)<<M)+11: return P::template rebind<(1+8),11>::g(v1,v2);
		case ((1+8)<<M)+12: return P::template rebind<(1+8),12>::g(v1,v2);case ((1+8)<<M)+13: return P::template rebind<(1+8),13>::g(v1,v2);case ((1+8)<<M)+14: return P::template rebind<(1+8),14>::g(v1,v2);case ((1+8)<<M)+15: return P::template rebind<(1+8),15>::g(v1,v2);

		case ((2+8)<<M)+0: return P::template rebind<(2+8),0>::g(v1,v2);case ((2+8)<<M)+1: return P::template rebind<(2+8),1>::g(v1,v2);case ((2+8)<<M)+2: return P::template rebind<(2+8),2>::g(v1,v2);case ((2+8)<<M)+3: return P::template rebind<(2+8),3>::g(v1,v2);
		case ((2+8)<<M)+4: return P::template rebind<(2+8),4>::g(v1,v2);case ((2+8)<<M)+5: return P::template rebind<(2+8),5>::g(v1,v2);case ((2+8)<<M)+6: return P::template rebind<(2+8),6>::g(v1,v2);case ((2+8)<<M)+7: return P::template rebind<(2+8),7>::g(v1,v2);
		case ((2+8)<<M)+8: return P::template rebind<(2+8),8>::g(v1,v2);case ((2+8)<<M)+9: return P::template rebind<(2+8),9>::g(v1,v2);case ((2+8)<<M)+10: return P::template rebind<(2+8),10>::g(v1,v2);case ((2+8)<<M)+11: return P::template rebind<(2+8),11>::g(v1,v2);
		case ((2+8)<<M)+12: return P::template rebind<(2+8),12>::g(v1,v2);case ((2+8)<<M)+13: return P::template rebind<(2+8),13>::g(v1,v2);case ((2+8)<<M)+14: return P::template rebind<(2+8),14>::g(v1,v2);case ((2+8)<<M)+15: return P::template rebind<(2+8),15>::g(v1,v2);

		case ((3+8)<<M)+0: return P::template rebind<(3+8),0>::g(v1,v2);case ((3+8)<<M)+1: return P::template rebind<(3+8),1>::g(v1,v2);case ((3+8)<<M)+2: return P::template rebind<(3+8),2>::g(v1,v2);case ((3+8)<<M)+3: return P::template rebind<(3+8),3>::g(v1,v2);
		case ((3+8)<<M)+4: return P::template rebind<(3+8),4>::g(v1,v2);case ((3+8)<<M)+5: return P::template rebind<(3+8),5>::g(v1,v2);case ((3+8)<<M)+6: return P::template rebind<(3+8),6>::g(v1,v2);case ((3+8)<<M)+7: return P::template rebind<(3+8),7>::g(v1,v2);
		case ((3+8)<<M)+8: return P::template rebind<(3+8),8>::g(v1,v2);case ((3+8)<<M)+9: return P::template rebind<(3+8),9>::g(v1,v2);case ((3+8)<<M)+10: return P::template rebind<(3+8),10>::g(v1,v2);case ((3+8)<<M)+11: return P::template rebind<(3+8),11>::g(v1,v2);
		case ((3+8)<<M)+12: return P::template rebind<(3+8),12>::g(v1,v2);case ((3+8)<<M)+13: return P::template rebind<(3+8),13>::g(v1,v2);case ((3+8)<<M)+14: return P::template rebind<(3+8),14>::g(v1,v2);case ((3+8)<<M)+15: return P::template rebind<(3+8),15>::g(v1,v2);

		case ((4+8)<<M)+0: return P::template rebind<(4+8),0>::g(v1,v2);case ((4+8)<<M)+1: return P::template rebind<(4+8),1>::g(v1,v2);case ((4+8)<<M)+2: return P::template rebind<(4+8),2>::g(v1,v2);case ((4+8)<<M)+3: return P::template rebind<(4+8),3>::g(v1,v2);
		case ((4+8)<<M)+4: return P::template rebind<(4+8),4>::g(v1,v2);case ((4+8)<<M)+5: return P::template rebind<(4+8),5>::g(v1,v2);case ((4+8)<<M)+6: return P::template rebind<(4+8),6>::g(v1,v2);case ((4+8)<<M)+7: return P::template rebind<(4+8),7>::g(v1,v2);
		case ((4+8)<<M)+8: return P::template rebind<(4+8),8>::g(v1,v2);case ((4+8)<<M)+9: return P::template rebind<(4+8),9>::g(v1,v2);case ((4+8)<<M)+10: return P::template rebind<(4+8),10>::g(v1,v2);case ((4+8)<<M)+11: return P::template rebind<(4+8),11>::g(v1,v2);
		case ((4+8)<<M)+12: return P::template rebind<(4+8),12>::g(v1,v2);case ((4+8)<<M)+13: return P::template rebind<(4+8),13>::g(v1,v2);case ((4+8)<<M)+14: return P::template rebind<(4+8),14>::g(v1,v2);case ((4+8)<<M)+15: return P::template rebind<(4+8),15>::g(v1,v2);

		case ((5+8)<<M)+0: return P::template rebind<(5+8),0>::g(v1,v2);case ((5+8)<<M)+1: return P::template rebind<(5+8),1>::g(v1,v2);case ((5+8)<<M)+2: return P::template rebind<(5+8),2>::g(v1,v2);case ((5+8)<<M)+3: return P::template rebind<(5+8),3>::g(v1,v2);
		case ((5+8)<<M)+4: return P::template rebind<(5+8),4>::g(v1,v2);case ((5+8)<<M)+5: return P::template rebind<(5+8),5>::g(v1,v2);case ((5+8)<<M)+6: return P::template rebind<(5+8),6>::g(v1,v2);case ((5+8)<<M)+7: return P::template rebind<(5+8),7>::g(v1,v2);
		case ((5+8)<<M)+8: return P::template rebind<(5+8),8>::g(v1,v2);case ((5+8)<<M)+9: return P::template rebind<(5+8),9>::g(v1,v2);case ((5+8)<<M)+10: return P::template rebind<(5+8),10>::g(v1,v2);case ((5+8)<<M)+11: return P::template rebind<(5+8),11>::g(v1,v2);
		case ((5+8)<<M)+12: return P::template rebind<(5+8),12>::g(v1,v2);case ((5+8)<<M)+13: return P::template rebind<(5+8),13>::g(v1,v2);case ((5+8)<<M)+14: return P::template rebind<(5+8),14>::g(v1,v2);case ((5+8)<<M)+15: return P::template rebind<(5+8),15>::g(v1,v2);

		case ((6+8)<<M)+0: return P::template rebind<(6+8),0>::g(v1,v2);case ((6+8)<<M)+1: return P::template rebind<(6+8),1>::g(v1,v2);case ((6+8)<<M)+2: return P::template rebind<(6+8),2>::g(v1,v2);case ((6+8)<<M)+3: return P::template rebind<(6+8),3>::g(v1,v2);
		case ((6+8)<<M)+4: return P::template rebind<(6+8),4>::g(v1,v2);case ((6+8)<<M)+5: return P::template rebind<(6+8),5>::g(v1,v2);case ((6+8)<<M)+6: return P::template rebind<(6+8),6>::g(v1,v2);case ((6+8)<<M)+7: return P::template rebind<(6+8),7>::g(v1,v2);
		case ((6+8)<<M)+8: return P::template rebind<(6+8),8>::g(v1,v2);case ((6+8)<<M)+9: return P::template rebind<(6+8),9>::g(v1,v2);case ((6+8)<<M)+10: return P::template rebind<(6+8),10>::g(v1,v2);case ((6+8)<<M)+11: return P::template rebind<(6+8),11>::g(v1,v2);
		case ((6+8)<<M)+12: return P::template rebind<(6+8),12>::g(v1,v2);case ((6+8)<<M)+13: return P::template rebind<(6+8),13>::g(v1,v2);case ((6+8)<<M)+14: return P::template rebind<(6+8),14>::g(v1,v2);case ((6+8)<<M)+15: return P::template rebind<(6+8),15>::g(v1,v2);

		case ((7+8)<<M)+0: return P::template rebind<(7+8),0>::g(v1,v2);case ((7+8)<<M)+1: return P::template rebind<(7+8),1>::g(v1,v2);case ((7+8)<<M)+2: return P::template rebind<(7+8),2>::g(v1,v2);case ((7+8)<<M)+3: return P::template rebind<(7+8),3>::g(v1,v2);
		case ((7+8)<<M)+4: return P::template rebind<(7+8),4>::g(v1,v2);case ((7+8)<<M)+5: return P::template rebind<(7+8),5>::g(v1,v2);case ((7+8)<<M)+6: return P::template rebind<(7+8),6>::g(v1,v2);case ((7+8)<<M)+7: return P::template rebind<(7+8),7>::g(v1,v2);
		case ((7+8)<<M)+8: return P::template rebind<(7+8),8>::g(v1,v2);case ((7+8)<<M)+9: return P::template rebind<(7+8),9>::g(v1,v2);case ((7+8)<<M)+10: return P::template rebind<(7+8),10>::g(v1,v2);case ((7+8)<<M)+11: return P::template rebind<(7+8),11>::g(v1,v2);
		case ((7+8)<<M)+12: return P::template rebind<(7+8),12>::g(v1,v2);case ((7+8)<<M)+13: return P::template rebind<(7+8),13>::g(v1,v2);case ((7+8)<<M)+14: return P::template rebind<(7+8),14>::g(v1,v2);case ((7+8)<<M)+15: return P::template rebind<(7+8),15>::g(v1,v2);

		default: return P::invalid_type();

		};
	}

	template<typename T1>
	static type g1(T1& v1,int flag)
	{
		switch(flag)
		{
		case (0<<M)+0: return P::template rebind<0,0>::g(v1);case (0<<M)+1: return P::template rebind<0,1>::g(v1);case (0<<M)+2: return P::template rebind<0,2>::g(v1);case (0<<M)+3: return P::template rebind<0,3>::g(v1);
		case (0<<M)+4: return P::template rebind<0,4>::g(v1);case (0<<M)+5: return P::template rebind<0,5>::g(v1);case (0<<M)+6: return P::template rebind<0,6>::g(v1);case (0<<M)+7: return P::template rebind<0,7>::g(v1);
		case (0<<M)+8: return P::template rebind<0,8>::g(v1);case (0<<M)+9: return P::template rebind<0,9>::g(v1);case (0<<M)+10: return P::template rebind<0,10>::g(v1);case (0<<M)+11: return P::template rebind<0,11>::g(v1);
		case (0<<M)+12: return P::template rebind<0,12>::g(v1);case (0<<M)+13: return P::template rebind<0,13>::g(v1);case (0<<M)+14: return P::template rebind<0,14>::g(v1);case (0<<M)+15: return P::template rebind<0,15>::g(v1);

		case (1<<M)+0: return P::template rebind<1,0>::g(v1);case (1<<M)+1: return P::template rebind<1,1>::g(v1);case (1<<M)+2: return P::template rebind<1,2>::g(v1);case (1<<M)+3: return P::template rebind<1,3>::g(v1);
		case (1<<M)+4: return P::template rebind<1,4>::g(v1);case (1<<M)+5: return P::template rebind<1,5>::g(v1);case (1<<M)+6: return P::template rebind<1,6>::g(v1);case (1<<M)+7: return P::template rebind<1,7>::g(v1);
		case (1<<M)+8: return P::template rebind<1,8>::g(v1);case (1<<M)+9: return P::template rebind<1,9>::g(v1);case (1<<M)+10: return P::template rebind<1,10>::g(v1);case (1<<M)+11: return P::template rebind<1,11>::g(v1);
		case (1<<M)+12: return P::template rebind<1,12>::g(v1);case (1<<M)+13: return P::template rebind<1,13>::g(v1);case (1<<M)+14: return P::template rebind<1,14>::g(v1);case (1<<M)+15: return P::template rebind<1,15>::g(v1);

		case (2<<M)+0: return P::template rebind<2,0>::g(v1);case (2<<M)+1: return P::template rebind<2,1>::g(v1);case (2<<M)+2: return P::template rebind<2,2>::g(v1);case (2<<M)+3: return P::template rebind<2,3>::g(v1);
		case (2<<M)+4: return P::template rebind<2,4>::g(v1);case (2<<M)+5: return P::template rebind<2,5>::g(v1);case (2<<M)+6: return P::template rebind<2,6>::g(v1);case (2<<M)+7: return P::template rebind<2,7>::g(v1);
		case (2<<M)+8: return P::template rebind<2,8>::g(v1);case (2<<M)+9: return P::template rebind<2,9>::g(v1);case (2<<M)+10: return P::template rebind<2,10>::g(v1);case (2<<M)+11: return P::template rebind<2,11>::g(v1);
		case (2<<M)+12: return P::template rebind<2,12>::g(v1);case (2<<M)+13: return P::template rebind<2,13>::g(v1);case (2<<M)+14: return P::template rebind<2,14>::g(v1);case (2<<M)+15: return P::template rebind<2,15>::g(v1);

		case (3<<M)+0: return P::template rebind<3,0>::g(v1);case (3<<M)+1: return P::template rebind<3,1>::g(v1);case (3<<M)+2: return P::template rebind<3,2>::g(v1);case (3<<M)+3: return P::template rebind<3,3>::g(v1);
		case (3<<M)+4: return P::template rebind<3,4>::g(v1);case (3<<M)+5: return P::template rebind<3,5>::g(v1);case (3<<M)+6: return P::template rebind<3,6>::g(v1);case (3<<M)+7: return P::template rebind<3,7>::g(v1);
		case (3<<M)+8: return P::template rebind<3,8>::g(v1);case (3<<M)+9: return P::template rebind<3,9>::g(v1);case (3<<M)+10: return P::template rebind<3,10>::g(v1);case (3<<M)+11: return P::template rebind<3,11>::g(v1);
		case (3<<M)+12: return P::template rebind<3,12>::g(v1);case (3<<M)+13: return P::template rebind<3,13>::g(v1);case (3<<M)+14: return P::template rebind<3,14>::g(v1);case (3<<M)+15: return P::template rebind<3,15>::g(v1);

		case (4<<M)+0: return P::template rebind<4,0>::g(v1);case (4<<M)+1: return P::template rebind<4,1>::g(v1);case (4<<M)+2: return P::template rebind<4,2>::g(v1);case (4<<M)+3: return P::template rebind<4,3>::g(v1);
		case (4<<M)+4: return P::template rebind<4,4>::g(v1);case (4<<M)+5: return P::template rebind<4,5>::g(v1);case (4<<M)+6: return P::template rebind<4,6>::g(v1);case (4<<M)+7: return P::template rebind<4,7>::g(v1);
		case (4<<M)+8: return P::template rebind<4,8>::g(v1);case (4<<M)+9: return P::template rebind<4,9>::g(v1);case (4<<M)+10: return P::template rebind<4,10>::g(v1);case (4<<M)+11: return P::template rebind<4,11>::g(v1);
		case (4<<M)+12: return P::template rebind<4,12>::g(v1);case (4<<M)+13: return P::template rebind<4,13>::g(v1);case (4<<M)+14: return P::template rebind<4,14>::g(v1);case (4<<M)+15: return P::template rebind<4,15>::g(v1);

		case (5<<M)+0: return P::template rebind<5,0>::g(v1);case (5<<M)+1: return P::template rebind<5,1>::g(v1);case (5<<M)+2: return P::template rebind<5,2>::g(v1);case (5<<M)+3: return P::template rebind<5,3>::g(v1);
		case (5<<M)+4: return P::template rebind<5,4>::g(v1);case (5<<M)+5: return P::template rebind<5,5>::g(v1);case (5<<M)+6: return P::template rebind<5,6>::g(v1);case (5<<M)+7: return P::template rebind<5,7>::g(v1);
		case (5<<M)+8: return P::template rebind<5,8>::g(v1);case (5<<M)+9: return P::template rebind<5,9>::g(v1);case (5<<M)+10: return P::template rebind<5,10>::g(v1);case (5<<M)+11: return P::template rebind<5,11>::g(v1);
		case (5<<M)+12: return P::template rebind<5,12>::g(v1);case (5<<M)+13: return P::template rebind<5,13>::g(v1);case (5<<M)+14: return P::template rebind<5,14>::g(v1);case (5<<M)+15: return P::template rebind<5,15>::g(v1);

		case (6<<M)+0: return P::template rebind<6,0>::g(v1);case (6<<M)+1: return P::template rebind<6,1>::g(v1);case (6<<M)+2: return P::template rebind<6,2>::g(v1);case (6<<M)+3: return P::template rebind<6,3>::g(v1);
		case (6<<M)+4: return P::template rebind<6,4>::g(v1);case (6<<M)+5: return P::template rebind<6,5>::g(v1);case (6<<M)+6: return P::template rebind<6,6>::g(v1);case (6<<M)+7: return P::template rebind<6,7>::g(v1);
		case (6<<M)+8: return P::template rebind<6,8>::g(v1);case (6<<M)+9: return P::template rebind<6,9>::g(v1);case (6<<M)+10: return P::template rebind<6,10>::g(v1);case (6<<M)+11: return P::template rebind<6,11>::g(v1);
		case (6<<M)+12: return P::template rebind<6,12>::g(v1);case (6<<M)+13: return P::template rebind<6,13>::g(v1);case (6<<M)+14: return P::template rebind<6,14>::g(v1);case (6<<M)+15: return P::template rebind<6,15>::g(v1);

		case (7<<M)+0: return P::template rebind<7,0>::g(v1);case (7<<M)+1: return P::template rebind<7,1>::g(v1);case (7<<M)+2: return P::template rebind<7,2>::g(v1);case (7<<M)+3: return P::template rebind<7,3>::g(v1);
		case (7<<M)+4: return P::template rebind<7,4>::g(v1);case (7<<M)+5: return P::template rebind<7,5>::g(v1);case (7<<M)+6: return P::template rebind<7,6>::g(v1);case (7<<M)+7: return P::template rebind<7,7>::g(v1);
		case (7<<M)+8: return P::template rebind<7,8>::g(v1);case (7<<M)+9: return P::template rebind<7,9>::g(v1);case (7<<M)+10: return P::template rebind<7,10>::g(v1);case (7<<M)+11: return P::template rebind<7,11>::g(v1);
		case (7<<M)+12: return P::template rebind<7,12>::g(v1);case (7<<M)+13: return P::template rebind<7,13>::g(v1);case (7<<M)+14: return P::template rebind<7,14>::g(v1);case (7<<M)+15: return P::template rebind<7,15>::g(v1);


		case ((0+8)<<M)+0: return P::template rebind<(0+8),0>::g(v1);case ((0+8)<<M)+1: return P::template rebind<(0+8),1>::g(v1);case ((0+8)<<M)+2: return P::template rebind<(0+8),2>::g(v1);case ((0+8)<<M)+3: return P::template rebind<(0+8),3>::g(v1);
		case ((0+8)<<M)+4: return P::template rebind<(0+8),4>::g(v1);case ((0+8)<<M)+5: return P::template rebind<(0+8),5>::g(v1);case ((0+8)<<M)+6: return P::template rebind<(0+8),6>::g(v1);case ((0+8)<<M)+7: return P::template rebind<(0+8),7>::g(v1);
		case ((0+8)<<M)+8: return P::template rebind<(0+8),8>::g(v1);case ((0+8)<<M)+9: return P::template rebind<(0+8),9>::g(v1);case ((0+8)<<M)+10: return P::template rebind<(0+8),10>::g(v1);case ((0+8)<<M)+11: return P::template rebind<(0+8),11>::g(v1);
		case ((0+8)<<M)+12: return P::template rebind<(0+8),12>::g(v1);case ((0+8)<<M)+13: return P::template rebind<(0+8),13>::g(v1);case ((0+8)<<M)+14: return P::template rebind<(0+8),14>::g(v1);case ((0+8)<<M)+15: return P::template rebind<(0+8),15>::g(v1);

		case ((1+8)<<M)+0: return P::template rebind<(1+8),0>::g(v1);case ((1+8)<<M)+1: return P::template rebind<(1+8),1>::g(v1);case ((1+8)<<M)+2: return P::template rebind<(1+8),2>::g(v1);case ((1+8)<<M)+3: return P::template rebind<(1+8),3>::g(v1);
		case ((1+8)<<M)+4: return P::template rebind<(1+8),4>::g(v1);case ((1+8)<<M)+5: return P::template rebind<(1+8),5>::g(v1);case ((1+8)<<M)+6: return P::template rebind<(1+8),6>::g(v1);case ((1+8)<<M)+7: return P::template rebind<(1+8),7>::g(v1);
		case ((1+8)<<M)+8: return P::template rebind<(1+8),8>::g(v1);case ((1+8)<<M)+9: return P::template rebind<(1+8),9>::g(v1);case ((1+8)<<M)+10: return P::template rebind<(1+8),10>::g(v1);case ((1+8)<<M)+11: return P::template rebind<(1+8),11>::g(v1);
		case ((1+8)<<M)+12: return P::template rebind<(1+8),12>::g(v1);case ((1+8)<<M)+13: return P::template rebind<(1+8),13>::g(v1);case ((1+8)<<M)+14: return P::template rebind<(1+8),14>::g(v1);case ((1+8)<<M)+15: return P::template rebind<(1+8),15>::g(v1);

		case ((2+8)<<M)+0: return P::template rebind<(2+8),0>::g(v1);case ((2+8)<<M)+1: return P::template rebind<(2+8),1>::g(v1);case ((2+8)<<M)+2: return P::template rebind<(2+8),2>::g(v1);case ((2+8)<<M)+3: return P::template rebind<(2+8),3>::g(v1);
		case ((2+8)<<M)+4: return P::template rebind<(2+8),4>::g(v1);case ((2+8)<<M)+5: return P::template rebind<(2+8),5>::g(v1);case ((2+8)<<M)+6: return P::template rebind<(2+8),6>::g(v1);case ((2+8)<<M)+7: return P::template rebind<(2+8),7>::g(v1);
		case ((2+8)<<M)+8: return P::template rebind<(2+8),8>::g(v1);case ((2+8)<<M)+9: return P::template rebind<(2+8),9>::g(v1);case ((2+8)<<M)+10: return P::template rebind<(2+8),10>::g(v1);case ((2+8)<<M)+11: return P::template rebind<(2+8),11>::g(v1);
		case ((2+8)<<M)+12: return P::template rebind<(2+8),12>::g(v1);case ((2+8)<<M)+13: return P::template rebind<(2+8),13>::g(v1);case ((2+8)<<M)+14: return P::template rebind<(2+8),14>::g(v1);case ((2+8)<<M)+15: return P::template rebind<(2+8),15>::g(v1);

		case ((3+8)<<M)+0: return P::template rebind<(3+8),0>::g(v1);case ((3+8)<<M)+1: return P::template rebind<(3+8),1>::g(v1);case ((3+8)<<M)+2: return P::template rebind<(3+8),2>::g(v1);case ((3+8)<<M)+3: return P::template rebind<(3+8),3>::g(v1);
		case ((3+8)<<M)+4: return P::template rebind<(3+8),4>::g(v1);case ((3+8)<<M)+5: return P::template rebind<(3+8),5>::g(v1);case ((3+8)<<M)+6: return P::template rebind<(3+8),6>::g(v1);case ((3+8)<<M)+7: return P::template rebind<(3+8),7>::g(v1);
		case ((3+8)<<M)+8: return P::template rebind<(3+8),8>::g(v1);case ((3+8)<<M)+9: return P::template rebind<(3+8),9>::g(v1);case ((3+8)<<M)+10: return P::template rebind<(3+8),10>::g(v1);case ((3+8)<<M)+11: return P::template rebind<(3+8),11>::g(v1);
		case ((3+8)<<M)+12: return P::template rebind<(3+8),12>::g(v1);case ((3+8)<<M)+13: return P::template rebind<(3+8),13>::g(v1);case ((3+8)<<M)+14: return P::template rebind<(3+8),14>::g(v1);case ((3+8)<<M)+15: return P::template rebind<(3+8),15>::g(v1);

		case ((4+8)<<M)+0: return P::template rebind<(4+8),0>::g(v1);case ((4+8)<<M)+1: return P::template rebind<(4+8),1>::g(v1);case ((4+8)<<M)+2: return P::template rebind<(4+8),2>::g(v1);case ((4+8)<<M)+3: return P::template rebind<(4+8),3>::g(v1);
		case ((4+8)<<M)+4: return P::template rebind<(4+8),4>::g(v1);case ((4+8)<<M)+5: return P::template rebind<(4+8),5>::g(v1);case ((4+8)<<M)+6: return P::template rebind<(4+8),6>::g(v1);case ((4+8)<<M)+7: return P::template rebind<(4+8),7>::g(v1);
		case ((4+8)<<M)+8: return P::template rebind<(4+8),8>::g(v1);case ((4+8)<<M)+9: return P::template rebind<(4+8),9>::g(v1);case ((4+8)<<M)+10: return P::template rebind<(4+8),10>::g(v1);case ((4+8)<<M)+11: return P::template rebind<(4+8),11>::g(v1);
		case ((4+8)<<M)+12: return P::template rebind<(4+8),12>::g(v1);case ((4+8)<<M)+13: return P::template rebind<(4+8),13>::g(v1);case ((4+8)<<M)+14: return P::template rebind<(4+8),14>::g(v1);case ((4+8)<<M)+15: return P::template rebind<(4+8),15>::g(v1);

		case ((5+8)<<M)+0: return P::template rebind<(5+8),0>::g(v1);case ((5+8)<<M)+1: return P::template rebind<(5+8),1>::g(v1);case ((5+8)<<M)+2: return P::template rebind<(5+8),2>::g(v1);case ((5+8)<<M)+3: return P::template rebind<(5+8),3>::g(v1);
		case ((5+8)<<M)+4: return P::template rebind<(5+8),4>::g(v1);case ((5+8)<<M)+5: return P::template rebind<(5+8),5>::g(v1);case ((5+8)<<M)+6: return P::template rebind<(5+8),6>::g(v1);case ((5+8)<<M)+7: return P::template rebind<(5+8),7>::g(v1);
		case ((5+8)<<M)+8: return P::template rebind<(5+8),8>::g(v1);case ((5+8)<<M)+9: return P::template rebind<(5+8),9>::g(v1);case ((5+8)<<M)+10: return P::template rebind<(5+8),10>::g(v1);case ((5+8)<<M)+11: return P::template rebind<(5+8),11>::g(v1);
		case ((5+8)<<M)+12: return P::template rebind<(5+8),12>::g(v1);case ((5+8)<<M)+13: return P::template rebind<(5+8),13>::g(v1);case ((5+8)<<M)+14: return P::template rebind<(5+8),14>::g(v1);case ((5+8)<<M)+15: return P::template rebind<(5+8),15>::g(v1);

		case ((6+8)<<M)+0: return P::template rebind<(6+8),0>::g(v1);case ((6+8)<<M)+1: return P::template rebind<(6+8),1>::g(v1);case ((6+8)<<M)+2: return P::template rebind<(6+8),2>::g(v1);case ((6+8)<<M)+3: return P::template rebind<(6+8),3>::g(v1);
		case ((6+8)<<M)+4: return P::template rebind<(6+8),4>::g(v1);case ((6+8)<<M)+5: return P::template rebind<(6+8),5>::g(v1);case ((6+8)<<M)+6: return P::template rebind<(6+8),6>::g(v1);case ((6+8)<<M)+7: return P::template rebind<(6+8),7>::g(v1);
		case ((6+8)<<M)+8: return P::template rebind<(6+8),8>::g(v1);case ((6+8)<<M)+9: return P::template rebind<(6+8),9>::g(v1);case ((6+8)<<M)+10: return P::template rebind<(6+8),10>::g(v1);case ((6+8)<<M)+11: return P::template rebind<(6+8),11>::g(v1);
		case ((6+8)<<M)+12: return P::template rebind<(6+8),12>::g(v1);case ((6+8)<<M)+13: return P::template rebind<(6+8),13>::g(v1);case ((6+8)<<M)+14: return P::template rebind<(6+8),14>::g(v1);case ((6+8)<<M)+15: return P::template rebind<(6+8),15>::g(v1);

		case ((7+8)<<M)+0: return P::template rebind<(7+8),0>::g(v1);case ((7+8)<<M)+1: return P::template rebind<(7+8),1>::g(v1);case ((7+8)<<M)+2: return P::template rebind<(7+8),2>::g(v1);case ((7+8)<<M)+3: return P::template rebind<(7+8),3>::g(v1);
		case ((7+8)<<M)+4: return P::template rebind<(7+8),4>::g(v1);case ((7+8)<<M)+5: return P::template rebind<(7+8),5>::g(v1);case ((7+8)<<M)+6: return P::template rebind<(7+8),6>::g(v1);case ((7+8)<<M)+7: return P::template rebind<(7+8),7>::g(v1);
		case ((7+8)<<M)+8: return P::template rebind<(7+8),8>::g(v1);case ((7+8)<<M)+9: return P::template rebind<(7+8),9>::g(v1);case ((7+8)<<M)+10: return P::template rebind<(7+8),10>::g(v1);case ((7+8)<<M)+11: return P::template rebind<(7+8),11>::g(v1);
		case ((7+8)<<M)+12: return P::template rebind<(7+8),12>::g(v1);case ((7+8)<<M)+13: return P::template rebind<(7+8),13>::g(v1);case ((7+8)<<M)+14: return P::template rebind<(7+8),14>::g(v1);case ((7+8)<<M)+15: return P::template rebind<(7+8),15>::g(v1);

		default: return P::invalid_type();

		};
	}

	template<typename T1,typename T2>
	static type g2(T1& v1,T2& v2,int flag)
	{
		switch(flag)
		{
		case (0<<M)+0: return P::template rebind<0,0>::g(v1,v2);case (0<<M)+1: return P::template rebind<0,1>::g(v1,v2);case (0<<M)+2: return P::template rebind<0,2>::g(v1,v2);case (0<<M)+3: return P::template rebind<0,3>::g(v1,v2);
		case (0<<M)+4: return P::template rebind<0,4>::g(v1,v2);case (0<<M)+5: return P::template rebind<0,5>::g(v1,v2);case (0<<M)+6: return P::template rebind<0,6>::g(v1,v2);case (0<<M)+7: return P::template rebind<0,7>::g(v1,v2);
		case (0<<M)+8: return P::template rebind<0,8>::g(v1,v2);case (0<<M)+9: return P::template rebind<0,9>::g(v1,v2);case (0<<M)+10: return P::template rebind<0,10>::g(v1,v2);case (0<<M)+11: return P::template rebind<0,11>::g(v1,v2);
		case (0<<M)+12: return P::template rebind<0,12>::g(v1,v2);case (0<<M)+13: return P::template rebind<0,13>::g(v1,v2);case (0<<M)+14: return P::template rebind<0,14>::g(v1,v2);case (0<<M)+15: return P::template rebind<0,15>::g(v1,v2);

		case (1<<M)+0: return P::template rebind<1,0>::g(v1,v2);case (1<<M)+1: return P::template rebind<1,1>::g(v1,v2);case (1<<M)+2: return P::template rebind<1,2>::g(v1,v2);case (1<<M)+3: return P::template rebind<1,3>::g(v1,v2);
		case (1<<M)+4: return P::template rebind<1,4>::g(v1,v2);case (1<<M)+5: return P::template rebind<1,5>::g(v1,v2);case (1<<M)+6: return P::template rebind<1,6>::g(v1,v2);case (1<<M)+7: return P::template rebind<1,7>::g(v1,v2);
		case (1<<M)+8: return P::template rebind<1,8>::g(v1,v2);case (1<<M)+9: return P::template rebind<1,9>::g(v1,v2);case (1<<M)+10: return P::template rebind<1,10>::g(v1,v2);case (1<<M)+11: return P::template rebind<1,11>::g(v1,v2);
		case (1<<M)+12: return P::template rebind<1,12>::g(v1,v2);case (1<<M)+13: return P::template rebind<1,13>::g(v1,v2);case (1<<M)+14: return P::template rebind<1,14>::g(v1,v2);case (1<<M)+15: return P::template rebind<1,15>::g(v1,v2);

		case (2<<M)+0: return P::template rebind<2,0>::g(v1,v2);case (2<<M)+1: return P::template rebind<2,1>::g(v1,v2);case (2<<M)+2: return P::template rebind<2,2>::g(v1,v2);case (2<<M)+3: return P::template rebind<2,3>::g(v1,v2);
		case (2<<M)+4: return P::template rebind<2,4>::g(v1,v2);case (2<<M)+5: return P::template rebind<2,5>::g(v1,v2);case (2<<M)+6: return P::template rebind<2,6>::g(v1,v2);case (2<<M)+7: return P::template rebind<2,7>::g(v1,v2);
		case (2<<M)+8: return P::template rebind<2,8>::g(v1,v2);case (2<<M)+9: return P::template rebind<2,9>::g(v1,v2);case (2<<M)+10: return P::template rebind<2,10>::g(v1,v2);case (2<<M)+11: return P::template rebind<2,11>::g(v1,v2);
		case (2<<M)+12: return P::template rebind<2,12>::g(v1,v2);case (2<<M)+13: return P::template rebind<2,13>::g(v1,v2);case (2<<M)+14: return P::template rebind<2,14>::g(v1,v2);case (2<<M)+15: return P::template rebind<2,15>::g(v1,v2);

		case (3<<M)+0: return P::template rebind<3,0>::g(v1,v2);case (3<<M)+1: return P::template rebind<3,1>::g(v1,v2);case (3<<M)+2: return P::template rebind<3,2>::g(v1,v2);case (3<<M)+3: return P::template rebind<3,3>::g(v1,v2);
		case (3<<M)+4: return P::template rebind<3,4>::g(v1,v2);case (3<<M)+5: return P::template rebind<3,5>::g(v1,v2);case (3<<M)+6: return P::template rebind<3,6>::g(v1,v2);case (3<<M)+7: return P::template rebind<3,7>::g(v1,v2);
		case (3<<M)+8: return P::template rebind<3,8>::g(v1,v2);case (3<<M)+9: return P::template rebind<3,9>::g(v1,v2);case (3<<M)+10: return P::template rebind<3,10>::g(v1,v2);case (3<<M)+11: return P::template rebind<3,11>::g(v1,v2);
		case (3<<M)+12: return P::template rebind<3,12>::g(v1,v2);case (3<<M)+13: return P::template rebind<3,13>::g(v1,v2);case (3<<M)+14: return P::template rebind<3,14>::g(v1,v2);case (3<<M)+15: return P::template rebind<3,15>::g(v1,v2);

		case (4<<M)+0: return P::template rebind<4,0>::g(v1,v2);case (4<<M)+1: return P::template rebind<4,1>::g(v1,v2);case (4<<M)+2: return P::template rebind<4,2>::g(v1,v2);case (4<<M)+3: return P::template rebind<4,3>::g(v1,v2);
		case (4<<M)+4: return P::template rebind<4,4>::g(v1,v2);case (4<<M)+5: return P::template rebind<4,5>::g(v1,v2);case (4<<M)+6: return P::template rebind<4,6>::g(v1,v2);case (4<<M)+7: return P::template rebind<4,7>::g(v1,v2);
		case (4<<M)+8: return P::template rebind<4,8>::g(v1,v2);case (4<<M)+9: return P::template rebind<4,9>::g(v1,v2);case (4<<M)+10: return P::template rebind<4,10>::g(v1,v2);case (4<<M)+11: return P::template rebind<4,11>::g(v1,v2);
		case (4<<M)+12: return P::template rebind<4,12>::g(v1,v2);case (4<<M)+13: return P::template rebind<4,13>::g(v1,v2);case (4<<M)+14: return P::template rebind<4,14>::g(v1,v2);case (4<<M)+15: return P::template rebind<4,15>::g(v1,v2);

		case (5<<M)+0: return P::template rebind<5,0>::g(v1,v2);case (5<<M)+1: return P::template rebind<5,1>::g(v1,v2);case (5<<M)+2: return P::template rebind<5,2>::g(v1,v2);case (5<<M)+3: return P::template rebind<5,3>::g(v1,v2);
		case (5<<M)+4: return P::template rebind<5,4>::g(v1,v2);case (5<<M)+5: return P::template rebind<5,5>::g(v1,v2);case (5<<M)+6: return P::template rebind<5,6>::g(v1,v2);case (5<<M)+7: return P::template rebind<5,7>::g(v1,v2);
		case (5<<M)+8: return P::template rebind<5,8>::g(v1,v2);case (5<<M)+9: return P::template rebind<5,9>::g(v1,v2);case (5<<M)+10: return P::template rebind<5,10>::g(v1,v2);case (5<<M)+11: return P::template rebind<5,11>::g(v1,v2);
		case (5<<M)+12: return P::template rebind<5,12>::g(v1,v2);case (5<<M)+13: return P::template rebind<5,13>::g(v1,v2);case (5<<M)+14: return P::template rebind<5,14>::g(v1,v2);case (5<<M)+15: return P::template rebind<5,15>::g(v1,v2);

		case (6<<M)+0: return P::template rebind<6,0>::g(v1,v2);case (6<<M)+1: return P::template rebind<6,1>::g(v1,v2);case (6<<M)+2: return P::template rebind<6,2>::g(v1,v2);case (6<<M)+3: return P::template rebind<6,3>::g(v1,v2);
		case (6<<M)+4: return P::template rebind<6,4>::g(v1,v2);case (6<<M)+5: return P::template rebind<6,5>::g(v1,v2);case (6<<M)+6: return P::template rebind<6,6>::g(v1,v2);case (6<<M)+7: return P::template rebind<6,7>::g(v1,v2);
		case (6<<M)+8: return P::template rebind<6,8>::g(v1,v2);case (6<<M)+9: return P::template rebind<6,9>::g(v1,v2);case (6<<M)+10: return P::template rebind<6,10>::g(v1,v2);case (6<<M)+11: return P::template rebind<6,11>::g(v1,v2);
		case (6<<M)+12: return P::template rebind<6,12>::g(v1,v2);case (6<<M)+13: return P::template rebind<6,13>::g(v1,v2);case (6<<M)+14: return P::template rebind<6,14>::g(v1,v2);case (6<<M)+15: return P::template rebind<6,15>::g(v1,v2);

		case (7<<M)+0: return P::template rebind<7,0>::g(v1,v2);case (7<<M)+1: return P::template rebind<7,1>::g(v1,v2);case (7<<M)+2: return P::template rebind<7,2>::g(v1,v2);case (7<<M)+3: return P::template rebind<7,3>::g(v1,v2);
		case (7<<M)+4: return P::template rebind<7,4>::g(v1,v2);case (7<<M)+5: return P::template rebind<7,5>::g(v1,v2);case (7<<M)+6: return P::template rebind<7,6>::g(v1,v2);case (7<<M)+7: return P::template rebind<7,7>::g(v1,v2);
		case (7<<M)+8: return P::template rebind<7,8>::g(v1,v2);case (7<<M)+9: return P::template rebind<7,9>::g(v1,v2);case (7<<M)+10: return P::template rebind<7,10>::g(v1,v2);case (7<<M)+11: return P::template rebind<7,11>::g(v1,v2);
		case (7<<M)+12: return P::template rebind<7,12>::g(v1,v2);case (7<<M)+13: return P::template rebind<7,13>::g(v1,v2);case (7<<M)+14: return P::template rebind<7,14>::g(v1,v2);case (7<<M)+15: return P::template rebind<7,15>::g(v1,v2);


		case ((0+8)<<M)+0: return P::template rebind<(0+8),0>::g(v1,v2);case ((0+8)<<M)+1: return P::template rebind<(0+8),1>::g(v1,v2);case ((0+8)<<M)+2: return P::template rebind<(0+8),2>::g(v1,v2);case ((0+8)<<M)+3: return P::template rebind<(0+8),3>::g(v1,v2);
		case ((0+8)<<M)+4: return P::template rebind<(0+8),4>::g(v1,v2);case ((0+8)<<M)+5: return P::template rebind<(0+8),5>::g(v1,v2);case ((0+8)<<M)+6: return P::template rebind<(0+8),6>::g(v1,v2);case ((0+8)<<M)+7: return P::template rebind<(0+8),7>::g(v1,v2);
		case ((0+8)<<M)+8: return P::template rebind<(0+8),8>::g(v1,v2);case ((0+8)<<M)+9: return P::template rebind<(0+8),9>::g(v1,v2);case ((0+8)<<M)+10: return P::template rebind<(0+8),10>::g(v1,v2);case ((0+8)<<M)+11: return P::template rebind<(0+8),11>::g(v1,v2);
		case ((0+8)<<M)+12: return P::template rebind<(0+8),12>::g(v1,v2);case ((0+8)<<M)+13: return P::template rebind<(0+8),13>::g(v1,v2);case ((0+8)<<M)+14: return P::template rebind<(0+8),14>::g(v1,v2);case ((0+8)<<M)+15: return P::template rebind<(0+8),15>::g(v1,v2);

		case ((1+8)<<M)+0: return P::template rebind<(1+8),0>::g(v1,v2);case ((1+8)<<M)+1: return P::template rebind<(1+8),1>::g(v1,v2);case ((1+8)<<M)+2: return P::template rebind<(1+8),2>::g(v1,v2);case ((1+8)<<M)+3: return P::template rebind<(1+8),3>::g(v1,v2);
		case ((1+8)<<M)+4: return P::template rebind<(1+8),4>::g(v1,v2);case ((1+8)<<M)+5: return P::template rebind<(1+8),5>::g(v1,v2);case ((1+8)<<M)+6: return P::template rebind<(1+8),6>::g(v1,v2);case ((1+8)<<M)+7: return P::template rebind<(1+8),7>::g(v1,v2);
		case ((1+8)<<M)+8: return P::template rebind<(1+8),8>::g(v1,v2);case ((1+8)<<M)+9: return P::template rebind<(1+8),9>::g(v1,v2);case ((1+8)<<M)+10: return P::template rebind<(1+8),10>::g(v1,v2);case ((1+8)<<M)+11: return P::template rebind<(1+8),11>::g(v1,v2);
		case ((1+8)<<M)+12: return P::template rebind<(1+8),12>::g(v1,v2);case ((1+8)<<M)+13: return P::template rebind<(1+8),13>::g(v1,v2);case ((1+8)<<M)+14: return P::template rebind<(1+8),14>::g(v1,v2);case ((1+8)<<M)+15: return P::template rebind<(1+8),15>::g(v1,v2);

		case ((2+8)<<M)+0: return P::template rebind<(2+8),0>::g(v1,v2);case ((2+8)<<M)+1: return P::template rebind<(2+8),1>::g(v1,v2);case ((2+8)<<M)+2: return P::template rebind<(2+8),2>::g(v1,v2);case ((2+8)<<M)+3: return P::template rebind<(2+8),3>::g(v1,v2);
		case ((2+8)<<M)+4: return P::template rebind<(2+8),4>::g(v1,v2);case ((2+8)<<M)+5: return P::template rebind<(2+8),5>::g(v1,v2);case ((2+8)<<M)+6: return P::template rebind<(2+8),6>::g(v1,v2);case ((2+8)<<M)+7: return P::template rebind<(2+8),7>::g(v1,v2);
		case ((2+8)<<M)+8: return P::template rebind<(2+8),8>::g(v1,v2);case ((2+8)<<M)+9: return P::template rebind<(2+8),9>::g(v1,v2);case ((2+8)<<M)+10: return P::template rebind<(2+8),10>::g(v1,v2);case ((2+8)<<M)+11: return P::template rebind<(2+8),11>::g(v1,v2);
		case ((2+8)<<M)+12: return P::template rebind<(2+8),12>::g(v1,v2);case ((2+8)<<M)+13: return P::template rebind<(2+8),13>::g(v1,v2);case ((2+8)<<M)+14: return P::template rebind<(2+8),14>::g(v1,v2);case ((2+8)<<M)+15: return P::template rebind<(2+8),15>::g(v1,v2);

		case ((3+8)<<M)+0: return P::template rebind<(3+8),0>::g(v1,v2);case ((3+8)<<M)+1: return P::template rebind<(3+8),1>::g(v1,v2);case ((3+8)<<M)+2: return P::template rebind<(3+8),2>::g(v1,v2);case ((3+8)<<M)+3: return P::template rebind<(3+8),3>::g(v1,v2);
		case ((3+8)<<M)+4: return P::template rebind<(3+8),4>::g(v1,v2);case ((3+8)<<M)+5: return P::template rebind<(3+8),5>::g(v1,v2);case ((3+8)<<M)+6: return P::template rebind<(3+8),6>::g(v1,v2);case ((3+8)<<M)+7: return P::template rebind<(3+8),7>::g(v1,v2);
		case ((3+8)<<M)+8: return P::template rebind<(3+8),8>::g(v1,v2);case ((3+8)<<M)+9: return P::template rebind<(3+8),9>::g(v1,v2);case ((3+8)<<M)+10: return P::template rebind<(3+8),10>::g(v1,v2);case ((3+8)<<M)+11: return P::template rebind<(3+8),11>::g(v1,v2);
		case ((3+8)<<M)+12: return P::template rebind<(3+8),12>::g(v1,v2);case ((3+8)<<M)+13: return P::template rebind<(3+8),13>::g(v1,v2);case ((3+8)<<M)+14: return P::template rebind<(3+8),14>::g(v1,v2);case ((3+8)<<M)+15: return P::template rebind<(3+8),15>::g(v1,v2);

		case ((4+8)<<M)+0: return P::template rebind<(4+8),0>::g(v1,v2);case ((4+8)<<M)+1: return P::template rebind<(4+8),1>::g(v1,v2);case ((4+8)<<M)+2: return P::template rebind<(4+8),2>::g(v1,v2);case ((4+8)<<M)+3: return P::template rebind<(4+8),3>::g(v1,v2);
		case ((4+8)<<M)+4: return P::template rebind<(4+8),4>::g(v1,v2);case ((4+8)<<M)+5: return P::template rebind<(4+8),5>::g(v1,v2);case ((4+8)<<M)+6: return P::template rebind<(4+8),6>::g(v1,v2);case ((4+8)<<M)+7: return P::template rebind<(4+8),7>::g(v1,v2);
		case ((4+8)<<M)+8: return P::template rebind<(4+8),8>::g(v1,v2);case ((4+8)<<M)+9: return P::template rebind<(4+8),9>::g(v1,v2);case ((4+8)<<M)+10: return P::template rebind<(4+8),10>::g(v1,v2);case ((4+8)<<M)+11: return P::template rebind<(4+8),11>::g(v1,v2);
		case ((4+8)<<M)+12: return P::template rebind<(4+8),12>::g(v1,v2);case ((4+8)<<M)+13: return P::template rebind<(4+8),13>::g(v1,v2);case ((4+8)<<M)+14: return P::template rebind<(4+8),14>::g(v1,v2);case ((4+8)<<M)+15: return P::template rebind<(4+8),15>::g(v1,v2);

		case ((5+8)<<M)+0: return P::template rebind<(5+8),0>::g(v1,v2);case ((5+8)<<M)+1: return P::template rebind<(5+8),1>::g(v1,v2);case ((5+8)<<M)+2: return P::template rebind<(5+8),2>::g(v1,v2);case ((5+8)<<M)+3: return P::template rebind<(5+8),3>::g(v1,v2);
		case ((5+8)<<M)+4: return P::template rebind<(5+8),4>::g(v1,v2);case ((5+8)<<M)+5: return P::template rebind<(5+8),5>::g(v1,v2);case ((5+8)<<M)+6: return P::template rebind<(5+8),6>::g(v1,v2);case ((5+8)<<M)+7: return P::template rebind<(5+8),7>::g(v1,v2);
		case ((5+8)<<M)+8: return P::template rebind<(5+8),8>::g(v1,v2);case ((5+8)<<M)+9: return P::template rebind<(5+8),9>::g(v1,v2);case ((5+8)<<M)+10: return P::template rebind<(5+8),10>::g(v1,v2);case ((5+8)<<M)+11: return P::template rebind<(5+8),11>::g(v1,v2);
		case ((5+8)<<M)+12: return P::template rebind<(5+8),12>::g(v1,v2);case ((5+8)<<M)+13: return P::template rebind<(5+8),13>::g(v1,v2);case ((5+8)<<M)+14: return P::template rebind<(5+8),14>::g(v1,v2);case ((5+8)<<M)+15: return P::template rebind<(5+8),15>::g(v1,v2);

		case ((6+8)<<M)+0: return P::template rebind<(6+8),0>::g(v1,v2);case ((6+8)<<M)+1: return P::template rebind<(6+8),1>::g(v1,v2);case ((6+8)<<M)+2: return P::template rebind<(6+8),2>::g(v1,v2);case ((6+8)<<M)+3: return P::template rebind<(6+8),3>::g(v1,v2);
		case ((6+8)<<M)+4: return P::template rebind<(6+8),4>::g(v1,v2);case ((6+8)<<M)+5: return P::template rebind<(6+8),5>::g(v1,v2);case ((6+8)<<M)+6: return P::template rebind<(6+8),6>::g(v1,v2);case ((6+8)<<M)+7: return P::template rebind<(6+8),7>::g(v1,v2);
		case ((6+8)<<M)+8: return P::template rebind<(6+8),8>::g(v1,v2);case ((6+8)<<M)+9: return P::template rebind<(6+8),9>::g(v1,v2);case ((6+8)<<M)+10: return P::template rebind<(6+8),10>::g(v1,v2);case ((6+8)<<M)+11: return P::template rebind<(6+8),11>::g(v1,v2);
		case ((6+8)<<M)+12: return P::template rebind<(6+8),12>::g(v1,v2);case ((6+8)<<M)+13: return P::template rebind<(6+8),13>::g(v1,v2);case ((6+8)<<M)+14: return P::template rebind<(6+8),14>::g(v1,v2);case ((6+8)<<M)+15: return P::template rebind<(6+8),15>::g(v1,v2);

		case ((7+8)<<M)+0: return P::template rebind<(7+8),0>::g(v1,v2);case ((7+8)<<M)+1: return P::template rebind<(7+8),1>::g(v1,v2);case ((7+8)<<M)+2: return P::template rebind<(7+8),2>::g(v1,v2);case ((7+8)<<M)+3: return P::template rebind<(7+8),3>::g(v1,v2);
		case ((7+8)<<M)+4: return P::template rebind<(7+8),4>::g(v1,v2);case ((7+8)<<M)+5: return P::template rebind<(7+8),5>::g(v1,v2);case ((7+8)<<M)+6: return P::template rebind<(7+8),6>::g(v1,v2);case ((7+8)<<M)+7: return P::template rebind<(7+8),7>::g(v1,v2);
		case ((7+8)<<M)+8: return P::template rebind<(7+8),8>::g(v1,v2);case ((7+8)<<M)+9: return P::template rebind<(7+8),9>::g(v1,v2);case ((7+8)<<M)+10: return P::template rebind<(7+8),10>::g(v1,v2);case ((7+8)<<M)+11: return P::template rebind<(7+8),11>::g(v1,v2);
		case ((7+8)<<M)+12: return P::template rebind<(7+8),12>::g(v1,v2);case ((7+8)<<M)+13: return P::template rebind<(7+8),13>::g(v1,v2);case ((7+8)<<M)+14: return P::template rebind<(7+8),14>::g(v1,v2);case ((7+8)<<M)+15: return P::template rebind<(7+8),15>::g(v1,v2);

		default: return P::invalid_type();

		};
	}

};



template<typename P>
class PL2_arr
{
public:

	template<typename X,typename Y>
	static void g(Variant& z,const X& x,const Y& y)
	{
		typedef typename P::template rebind<X,Y>::type R;
		z.reset<R>(P::g(x,y));
	}

	template<typename X,typename Y>
	static void g(Variant& z,const arr_xt<X>& x,Y y)
	{
		typedef typename P::template rebind<X,Y>::type R;
		arr_xt<R> r;r.resize(x.size(0),x.size(1),x.size(2),x.size(3),x.size(4),x.size(5));
		for(size_t i=0;i<x.size();i++)
		{
			r(i)=P::g(x(i),y);
		}
		kvar_flag<arr_xt<R> >::unique(z).swap(r);
	}

	template<typename X,typename Y>
	static void g(Variant& z,X y,const arr_xt<Y>& x)
	{
		typedef typename P::template rebind<X,Y>::type R;
		arr_xt<R> r;r.resize(x.size(0),x.size(1),x.size(2),x.size(3),x.size(4),x.size(5));
		for(size_t i=0;i<x.size();i++)
		{
			r(i)=P::g(y,x(i));
		}
		kvar_flag<arr_xt<R> >::unique(z).swap(r);
	}

	template<typename X,typename Y>
	static void g(Variant& z,const arr_xt<X>& x,const arr_xt<Y>& y)
	{
		typedef typename P::template rebind<X,Y>::type R;
		if(x.size()!=y.size())
		{
			Exception::XInvalidIndex();
		}

		arr_xt<R> r;r.resize(x.size(0),x.size(1),x.size(2),x.size(3),x.size(4),x.size(5));
		for(size_t i=0;i<x.size();i++)
		{
			r(i)=P::g(x(i),y(i));
		}
		kvar_flag<arr_xt<R> >::unique(z).swap(r);
	}
};


template<typename X,typename Y,typename P,bool V>
class PL2_sel
{
public:
	static void g(Executor&)
	{
		Exception::XBadCast();
	}
};

template<typename X,typename Y,typename P>
class PL2_sel<X,Y,P,true>
{
public:
	static void g(Executor& ks)
	{
		Variant& lhs(ks.get(-1));
		Variant& rhs(ks.get(0));

		PL2_arr<P>::g(lhs,kvar_flag<X>::raw(lhs),kvar_flag<Y>::raw(rhs));
	}
};



template<typename P>
class PL2_opx_num
{
public:
	typedef void type;

	static type invalid_type()
	{
		Exception::XBadCast();
	}

	template<int N1,int N2>
	class rebind : public PL2_sel<typename kvar_type<N1>::type,typename kvar_type<N2>::type,P,kvar_opx_num<N1,N2>::valid>{};

};



class PL2_bool
{
public:
	template<typename T1,typename T2>
	class rebind
	{
	public:
		typedef bool type;

	};
};


class PL2_num1
{
public:
	template<typename T1,typename T2>
	class rebind
	{
	public:
		static const int id1=kvar_scalar_type::template id<T1>::value;
		static const int id2=kvar_scalar_type::template id<T2>::value;
		static const int id0=id1>id2?id1:(id2<1?1:id2);
		typedef typename kvar_scalar_type::template at<id0>::type type;
	};
};

class PL2_num2
{
public:
	template<typename T1,typename T2>
	class rebind
	{
	public:
		static const int id1=kvar_scalar_type::template id<T1>::value;
		static const int id2=kvar_scalar_type::template id<T2>::value;
		static const int id0=id1>id2?id1:(id2<2?2:id2);
		typedef typename kvar_scalar_type::template at<id0>::type type;
	};
};

class PL2_rel
{
public:

	template<typename T>
	const T& cast(const T& v){return v;}

	double cast(std::complex<double>& v)
	{
		if(v.imag()!=0.0)
		{
			Exception::XBadCast();
		}
		return v.real();
	}

	template<typename T1,typename T2>
	class rebind
	{
	public:
		static const int id1=kvar_scalar_type::template id<T1>::value;
		static const int id2=kvar_scalar_type::template id<T2>::value;
		static const int id0=id1>id2?id1:id2;
		static const int id3=(id0==3)?2:id0;

		typedef bool type;
		typedef typename kvar_scalar_type::template at<id0>::type elem;
		typedef typename kvar_scalar_type::template at<id3>::type fake;

		template<typename X>
		static fake cast(const X& v)
		{
			return fake(v);
		}

		static fake cast(const std::complex<double>& v)
		{
			if(v.imag()!=0.0)
			{
				Exception::XBadCast();
			}
			return v.real();
		}
	};
};

class PL2_add : public PL2_num1
{
public:
	template<typename T1,typename T2>
	static typename rebind<T1,T2>::type g(const T1& t1,const T2& t2)
	{
		return typename rebind<T1,T2>::type(t1)+typename rebind<T1,T2>::type(t2);
	}
};

class PL2_sub : public PL2_num1
{
public:
	template<typename T1,typename T2>
	static typename rebind<T1,T2>::type g(const T1& t1,const T2& t2)
	{
		return typename rebind<T1,T2>::type(t1)-typename rebind<T1,T2>::type(t2);
	}
};

class PL2_mul : public PL2_num1
{
public:
	template<typename T1,typename T2>
	static typename rebind<T1,T2>::type g(const T1& t1,const T2& t2)
	{
		return typename rebind<T1,T2>::type(t1)* typename rebind<T1,T2>::type(t2);
	}
};


class PL2_div : public PL2_num1
{
public:
	template<typename T1,typename T2>
	static typename rebind<T1,T2>::type g(const T1& t1,const T2& t2)
	{
		return typename rebind<T1,T2>::type(t1)/ typename rebind<T1,T2>::type(t2);
	}
};


class PL1_cpx
{
public:
	template<typename T1>
	class rebind
	{
	public:
		typedef std::complex<double> type;
	};

	template<typename T1>
	static std::complex<double> g(const T1& t1)
	{
		return std::complex<double>(t1);
	}

	template<typename T1>
	static std::complex<double> g(const std::complex<T1>& t1)
	{
		return std::complex<double>(t1.real(),t1.imag());
	}

};


class PL2_cpx
{
public:
	template<typename T1,typename T2>
	class rebind
	{
	public:
		typedef std::complex<double> type;
	};

	template<typename T1>
	static double k(T1 v){return (double)v;}

	template<typename T1>
	static double k(const std::complex<T1>& v)
	{
		if(v.imag()!=T1(0))
		{
			Exception::XBadCast();
		}
		return k(v.real());
	}


	template<typename T1,typename T2>
	static std::complex<double> g(const T1& t1,const T2& t2)
	{
		return std::complex<double>(k(t1),k(t2));
	}
};


class PL2_eq : public PL2_rel
{
public:

	template<typename T1,typename T2>
	static bool g(const T1& t1,const T2& t2)
	{
		return typename PL2_num1::rebind<T1,T2>::type(t1)==typename PL2_num1::rebind<T1,T2>::type(t2);
	}
};



class PL2_mod : public PL2_num1
{
public:


	static int64_t k(int64_t t1,int64_t t2)
	{
		return t1%t2;
	}

	static double k(double t1,double t2)
	{
		return t1-double((int64_t)(t1/t2))*t2;
	}

	static std::complex<double> k(std::complex<double> t1,std::complex<double> t2)
	{
		Exception::XBadCast();
		return 0;
	}


	template<typename T1,typename T2>
	static typename rebind<T1,T2>::type g(const T1& t1,const T2& t2)
	{
		typedef typename rebind<T1,T2>::type type;
		return k((type)t1,(type)t2);
	}
};


class PL2_pow : public PL2_num1
{
public:

	static int64_t k(int64_t t1,int64_t t2)
	{
		return ::pow((double)t1,(int)t2);
	}

	static double k(double t1,double t2)
	{
		return ::pow(t1,t2);
	}


	static std::complex<double> k(const std::complex<double>& t1,int64_t t2)
	{
		return std::pow(t1,(int)t2);
	}

	template<typename X>
	static std::complex<double> k(const std::complex<double>& t1,X t2)
	{
		return std::pow(t1,t2);
	}


	template<typename T1,typename T2>
	static typename rebind<T1,T2>::type g(const T1& t1,const T2& t2)
	{
		typedef typename rebind<T1,T2>::type type;
		return k((type)t1,(type)t2);
	}

	template<typename T1,typename T2>
	static std::complex<double> g(const std::complex<T1>& t1,const T2& t2)
	{
		return k((std::complex<double>)t1,t2);
	}
};


class PL2_ne : public PL2_rel
{
public:
	template<typename T1,typename T2>
	static bool g(const T1& t1,const T2& t2)
	{
		return typename PL2_num1::rebind<T1,T2>::type(t1)!=typename PL2_num1::rebind<T1,T2>::type(t2);
	}
};


class PL2_gt : public PL2_rel
{
public:
	template<typename T1,typename T2>
	static bool g(const T1& t1,const T2& t2)
	{
		return rebind<T1,T2>::cast(t1)>rebind<T1,T2>::cast(t2);
	}
};

class PL2_ge : public PL2_rel
{
public:
	template<typename T1,typename T2>
	static bool g(const T1& t1,const T2& t2)
	{
		return rebind<T1,T2>::cast(t1)>=rebind<T1,T2>::cast(t2);
	}
};


class PL2_lt : public PL2_rel
{
public:
	template<typename T1,typename T2>
	static bool g(const T1& t1,const T2& t2)
	{
		return rebind<T1,T2>::cast(t1)<rebind<T1,T2>::cast(t2);
	}
};

class PL2_le : public PL2_rel
{
public:
	template<typename T1,typename T2>
	static bool g(const T1& t1,const T2& t2)
	{
		return rebind<T1,T2>::cast(t1)<rebind<T1,T2>::cast(t2);
	}
};

class PL2_and : public PL2_bool
{
public:
	template<typename T1,typename T2>
	static bool g(const T1& t1,const T2& t2)
	{
		return PLCast<bool>::g(t1) && PLCast<bool>::g(t2);
	}
};

class PL2_or : public PL2_bool
{
public:
	template<typename T1,typename T2>
	static bool g(const T1& t1,const T2& t2)
	{
		return PLCast<bool>::g(t1)|| PLCast<bool>::g(t2);
	}
};

class PL2_xor : public PL2_bool
{
public:
	template<typename T1,typename T2>
	static bool g(const T1& t1,const T2& t2)
	{
		return PLCast<bool>::g(t1)^PLCast<bool>::g(t2);
	}
};


class PL2_bitwise
{
public:
	template<typename X,typename Y>
	class rebind
	{
	public:
		typedef int64_t type;
	};
};


class PL2_bw_and : public PL2_bitwise
{
public:
	template<typename T1,typename T2>
	static int64_t g(const T1& t1,const T2& t2)
	{
		return PLCast<int64_t>::g(t1)&PLCast<int64_t>::g(t2);
	}
};

class PL2_bw_or : public PL2_bitwise
{
public:
	template<typename T1,typename T2>
	static int64_t g(const T1& t1,const T2& t2)
	{
		return PLCast<int64_t>::g(t1)|PLCast<int64_t>::g(t2);
	}
};


class PL2_bw_xor : public PL2_bitwise
{
public:
	template<typename T1,typename T2>
	static int64_t g(const T1& t1,const T2& t2)
	{
		return PLCast<int64_t>::g(t1)^PLCast<int64_t>::g(t2);
	}
};



VHWD_LEAVE
#endif
