// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_STRING_ARGS_H__
#define __H_VHWD_STRING_ARGS_H__


#include <string>


VHWD_ENTER

class VHWD_DLLIMPEXP String;

class StringParamCast
{
public:
	template<typename G>
	static inline G g(const G& v)
	{
		return v;
	}

	static inline const char* g(const char* v)
	{
		return v;
	}
	static inline const char* g(const std::string& v)
	{
		return v.c_str();
	}
	static const char* g(const String& v);

};


#define STRING_FORMAT_PTvar(x) typename T##x
#define STRING_FORMAT_PMvar(x) const T##x & p##x
#define STRING_FORMAT_PKvar(x) StringParamCast::g(p##x)

#define STRING_FORMAT_PMList0 STRING_FORMAT_PMvar(0)
#define STRING_FORMAT_PMList1 STRING_FORMAT_PMList0,STRING_FORMAT_PMvar(1)
#define STRING_FORMAT_PMList2 STRING_FORMAT_PMList1,STRING_FORMAT_PMvar(2)
#define STRING_FORMAT_PMList3 STRING_FORMAT_PMList2,STRING_FORMAT_PMvar(3)
#define STRING_FORMAT_PMList4 STRING_FORMAT_PMList3,STRING_FORMAT_PMvar(4)
#define STRING_FORMAT_PMList5 STRING_FORMAT_PMList4,STRING_FORMAT_PMvar(5)
#define STRING_FORMAT_PMList6 STRING_FORMAT_PMList5,STRING_FORMAT_PMvar(6)
#define STRING_FORMAT_PMList7 STRING_FORMAT_PMList6,STRING_FORMAT_PMvar(7)
#define STRING_FORMAT_PMList8 STRING_FORMAT_PMList7,STRING_FORMAT_PMvar(8)
#define STRING_FORMAT_PMList9 STRING_FORMAT_PMList8,STRING_FORMAT_PMvar(9)

#define STRING_FORMAT_PKList0 STRING_FORMAT_PKvar(0)
#define STRING_FORMAT_PKList1 STRING_FORMAT_PKList0,STRING_FORMAT_PKvar(1)
#define STRING_FORMAT_PKList2 STRING_FORMAT_PKList1,STRING_FORMAT_PKvar(2)
#define STRING_FORMAT_PKList3 STRING_FORMAT_PKList2,STRING_FORMAT_PKvar(3)
#define STRING_FORMAT_PKList4 STRING_FORMAT_PKList3,STRING_FORMAT_PKvar(4)
#define STRING_FORMAT_PKList5 STRING_FORMAT_PKList4,STRING_FORMAT_PKvar(5)
#define STRING_FORMAT_PKList6 STRING_FORMAT_PKList5,STRING_FORMAT_PKvar(6)
#define STRING_FORMAT_PKList7 STRING_FORMAT_PKList6,STRING_FORMAT_PKvar(7)
#define STRING_FORMAT_PKList8 STRING_FORMAT_PKList7,STRING_FORMAT_PKvar(8)
#define STRING_FORMAT_PKList9 STRING_FORMAT_PKList8,STRING_FORMAT_PKvar(9)

#define STRING_FORMAT_PTList0 STRING_FORMAT_PTvar(0)
#define STRING_FORMAT_PTList1 STRING_FORMAT_PTList0,STRING_FORMAT_PTvar(1)
#define STRING_FORMAT_PTList2 STRING_FORMAT_PTList1,STRING_FORMAT_PTvar(2)
#define STRING_FORMAT_PTList3 STRING_FORMAT_PTList2,STRING_FORMAT_PTvar(3)
#define STRING_FORMAT_PTList4 STRING_FORMAT_PTList3,STRING_FORMAT_PTvar(4)
#define STRING_FORMAT_PTList5 STRING_FORMAT_PTList4,STRING_FORMAT_PTvar(5)
#define STRING_FORMAT_PTList6 STRING_FORMAT_PTList5,STRING_FORMAT_PTvar(6)
#define STRING_FORMAT_PTList7 STRING_FORMAT_PTList6,STRING_FORMAT_PTvar(7)
#define STRING_FORMAT_PTList8 STRING_FORMAT_PTList7,STRING_FORMAT_PTvar(8)
#define STRING_FORMAT_PTList9 STRING_FORMAT_PTList8,STRING_FORMAT_PTvar(9)

#define STRING_FORMAT_PFList0(X,Y,Z) template<STRING_FORMAT_PTList0> X(STRING_FORMAT_PMList0){Y(STRING_FORMAT_PKList0,Z);}
#define STRING_FORMAT_PFList1(X,Y,Z) template<STRING_FORMAT_PTList1> X(STRING_FORMAT_PMList1){Y(STRING_FORMAT_PKList1,Z);}
#define STRING_FORMAT_PFList2(X,Y,Z) template<STRING_FORMAT_PTList2> X(STRING_FORMAT_PMList2){Y(STRING_FORMAT_PKList2,Z);}
#define STRING_FORMAT_PFList3(X,Y,Z) template<STRING_FORMAT_PTList3> X(STRING_FORMAT_PMList3){Y(STRING_FORMAT_PKList3,Z);}
#define STRING_FORMAT_PFList4(X,Y,Z) template<STRING_FORMAT_PTList4> X(STRING_FORMAT_PMList4){Y(STRING_FORMAT_PKList4,Z);}
#define STRING_FORMAT_PFList5(X,Y,Z) template<STRING_FORMAT_PTList5> X(STRING_FORMAT_PMList5){Y(STRING_FORMAT_PKList5,Z);}
#define STRING_FORMAT_PFList6(X,Y,Z) template<STRING_FORMAT_PTList6> X(STRING_FORMAT_PMList6){Y(STRING_FORMAT_PKList6,Z);}
#define STRING_FORMAT_PFList7(X,Y,Z) template<STRING_FORMAT_PTList7> X(STRING_FORMAT_PMList7){Y(STRING_FORMAT_PKList7,Z);}
#define STRING_FORMAT_PFList8(X,Y,Z) template<STRING_FORMAT_PTList8> X(STRING_FORMAT_PMList8){Y(STRING_FORMAT_PKList8,Z);}
#define STRING_FORMAT_PFList9(X,Y,Z) template<STRING_FORMAT_PTList9> X(STRING_FORMAT_PMList9){Y(STRING_FORMAT_PKList9,Z);}


#define STRING_FORMAT_FUNCTIONS(X,Y,Z)  \
	STRING_FORMAT_PFList0(X,Y,Z);\
	STRING_FORMAT_PFList1(X,Y,Z);\
	STRING_FORMAT_PFList2(X,Y,Z);\
	STRING_FORMAT_PFList3(X,Y,Z);\
	STRING_FORMAT_PFList4(X,Y,Z);\
	STRING_FORMAT_PFList5(X,Y,Z);\
	STRING_FORMAT_PFList6(X,Y,Z);\
	STRING_FORMAT_PFList7(X,Y,Z);\
	STRING_FORMAT_PFList8(X,Y,Z);\
	STRING_FORMAT_PFList9(X,Y,Z);\


VHWD_LEAVE
#endif
