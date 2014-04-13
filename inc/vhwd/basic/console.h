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

	static void ColoredWrite(int color,const String& s);
	static void ColoredWriteLine(int color,const String& s);

	static String ReadLine();

	static void Pause();

};

VHWD_LEAVE
#endif
