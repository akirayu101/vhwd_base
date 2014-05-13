// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_BASIC_EXCEPTION__
#define __H_VHWD_BASIC_EXCEPTION__

#include "vhwd/config.h"
#include <stdexcept>

VHWD_ENTER

class VHWD_DLLIMPEXP Exception : public std::exception
{
public:

	Exception();
	Exception(const Exception& o);
	Exception(const char* p,bool del=true);
	Exception(const String& p);
	const Exception& operator=(const Exception& o);

	~Exception() throw();

	// Throw Exception
	static void XError();
	static void XError(const char*p, bool del=true);

	// Throw std::bad_alloc
	static void XBadAlloc();

	static void XBadCast();
	static void XInvalidArgument();
	static void XInvalidIndex();
	static void XNotFound();
	static void XReadOnly();
	static void XTypeLocked();
	static void XBadFactor();
	

#ifdef _WIN32
	const char* what() const throw();
#else
	const char* what() throw();
#endif

protected:
	const char* m_sWhat;
	bool m_bDelete;
};


VHWD_LEAVE
#endif
