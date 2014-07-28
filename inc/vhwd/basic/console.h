// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_BASIC_CONSOLE__
#define __H_VHWD_BASIC_CONSOLE__

#include "vhwd/config.h"


VHWD_ENTER


class VHWD_DLLIMPEXP Console
{
public:
	enum
	{
		COLOR_R=1<<2,
		COLOR_G=1<<1,
		COLOR_B=1<<0,
		COLOR_S=1<<3,
		COLOR_D=COLOR_R|COLOR_G|COLOR_B,
	};

	static void SetColor(int color);

	static void Write(const String& s);
	static void WriteLine(const String& s);

	static void Write(const String& s,int color);
	static void WriteLine(const String& s,int color);

	static String ReadLine();

	static void Pause();

};

VHWD_LEAVE
#endif
