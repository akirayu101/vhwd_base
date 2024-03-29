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

class VHWD_DLLIMPEXP String;

class VHWD_DLLIMPEXP Exception : public std::exception
{
public:

	Exception();
	Exception(const Exception& o);
	Exception(const char* p,bool del=true);
	Exception(const String& p);
	Exception& operator=(const Exception& o);

	~Exception() throw();

	// Throw Exception
	static void XError();
	static void XError(const String& s);
	static void XError(const char*p, bool del);

	// Throw std::bad_alloc
	static void XBadAlloc();

	static void XBadCast();
	static void XInvalidArgument();
	static void XInvalidIndex();
	static void XNotFound();
	static void XReadOnly();
	static void XTypeLocked();
	static void XBadFunctor();
	

#ifdef VHWD_WINDOWS
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
