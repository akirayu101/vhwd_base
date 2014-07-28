// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_BASIC_BITFLAGS__
#define __H_VHWD_BASIC_BITFLAGS__

#include "vhwd/config.h"


VHWD_ENTER


class VHWD_DLLIMPEXP BitFlags
{
public:

	BitFlags(int32_t flag=0);

	void add(int32_t flag);			// add flag  value|=flag
	void del(int32_t flag);			// del flag  value&=~flag
	bool get(int32_t flag) const;	// get flag	 return value&flag
	void set(int32_t flag,bool v);	// set flag   if(v) add(flag) else del(flag)
	void clr(int32_t flag=0);		// set flag	  value=flag;
	void inv(int32_t flag);			// inv flag   value^=flag;

	int32_t val() const;

private:
	int32_t m_nFlags;
};

inline bool operator==(const BitFlags lhs,const BitFlags rhs)
{
	return lhs.val()==rhs.val();
}
inline bool operator!=(const BitFlags lhs,const BitFlags rhs)
{
	return lhs.val()!=rhs.val();
}


VHWD_LEAVE
#endif
