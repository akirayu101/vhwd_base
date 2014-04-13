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
	bool get(int32_t flag) const;	// get flag
	void set(int32_t flag,bool v);	// set flag   if(v) add(flag) else del(flag)
	void clr(int32_t flag=0);		// set flag	  value=flag;
	void inv(int32_t flag);			// inv flag   value^=flag;

	int32_t val() const;

private:
	int32_t m_flags;
};

VHWD_LEAVE
#endif
