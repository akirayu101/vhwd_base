// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_BASIC_PLATFORM__
#define __H_VHWD_BASIC_PLATFORM__

#include "vhwd/config.h"
#include "vhwd/basic/atomic.h"

#ifdef _WIN32
#include "windows.h"
#endif

#include <algorithm>

VHWD_ENTER


#ifdef _WIN32
class KO_Policy_handle
{
public:
	typedef void* type;
	typedef type const_reference;
	static type invalid_value()
	{
		return NULL;
	}
	static void destroy(type& o);
};
#else
class KO_Policy_handle
{
public:
	typedef int type;
	typedef type const_reference;
	static type invalid_value()
	{
		return -1;
	}
	static void destroy(type& o);
};
#endif


template<typename P>
class KO_Handle
{
public:
	typedef typename P::type type;
	typedef typename P::const_reference const_reference;

	KO_Handle()
	{
		m_pCounter=NULL;
		m_pHandle=P::invalid_value();
	}

	KO_Handle(const KO_Handle& o)
		:m_pHandle(o.m_pHandle)
		,m_pCounter(o.m_pCounter)
	{
		if(m_pCounter)
		{
			m_pCounter->IncUseCount();
		}
	}

	KO_Handle& operator=(const KO_Handle& o)
	{
		if(m_pCounter==o.m_pCounter)
		{
			return *this;
		}
		close();

		m_pHandle=o.m_pHandle;
		m_pCounter=o.m_pCounter;

		if(m_pCounter)
		{
			m_pCounter->IncUseCount();
		}

		return *this;
	}

	void reset(const_reference v)
	{
		if(m_pCounter && m_pCounter->DecUseCount()==0)
		{
			m_pCounter=NULL;
			P::destroy(m_pHandle);
		}
		if(v!=P::invalid_value())
		{
			m_pCounter=new RefCounter();
			m_pCounter->IncUseCount();
		}
		m_pHandle=v;
	}

	bool close()
	{
		if(!m_pCounter)
		{
			return false;
		}

		if(m_pCounter->DecUseCount())
		{
			P::destroy(m_pHandle);
			m_pCounter=NULL;
			m_pHandle=P::invalid_value();
			return true;
		}
		else
		{
			m_pHandle=P::invalid_value();
			m_pCounter=NULL;
			return false;
		}
	}

	operator const_reference()
	{
		return m_pHandle;
	}

	bool ok()
	{
		return m_pCounter!=NULL;
	}

	~KO_Handle()
	{
		close();
	}

	void swap(KO_Handle& o)
	{
		std::swap(m_pHandle,o.m_pHandle);
		std::swap(m_pCounter,o.m_pCounter);
	}

protected:
	type m_pHandle;
	mutable RefCounter* m_pCounter;
};

class FileAccess
{
public:
	enum
	{
		FLAG_RD=1<<0,
		FLAG_WR=1<<1,
		FLAG_RW=FLAG_RD|FLAG_WR,
		FLAG_CR=1<<3,
		FLAG_APPEND=1<<4,
		FLAG_EX=1<<5,
	};

	static int makeflag(int flag_,int fr,int fw)
	{
		int acc=0;
		if(flag_&FileAccess::FLAG_RD)
		{
			acc|=fr;
		}
		if(flag_&FileAccess::FLAG_WR)
		{
			acc|=fw;
		}
		return acc;
	}

#ifdef _WIN32
	union LargeInteger
	{
		DWORD64 dval;
		DWORD d[2];
	};
#endif

};

VHWD_LEAVE
#endif
