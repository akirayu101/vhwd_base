#include "vhwd/basic/console.h"
#include "vhwd/basic/string.h"
#include "vhwd/basic/lockguard.h"
#include "vhwd/basic/atomic.h"

#ifdef VHWD_WINDOWS
#include "windows.h"
#endif

#include <cstdio>
#include <iostream>

VHWD_ENTER

int g_cConsoleColor=Console::COLOR_D;
AtomicSpin g_tSpinConsole;

void ConsoleDoSetColor(int color)
{
	g_cConsoleColor=color;

#ifdef VHWD_WINDOWS
	HANDLE hCon=GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hCon,color);
#else
	switch(color)
	{
	case 0:
		::printf("\033[22;30m");
		break;
	case Console::COLOR_R:
		::printf("\033[01;31m");
		break;
	case Console::COLOR_R|Console::COLOR_S:
		::printf("\033[22;31m");
		break;
	case Console::COLOR_G:
		::printf("\033[01;32m");
		break;
	case Console::COLOR_G|Console::COLOR_S:
		::printf("\033[22;32m");
		break;
	case Console::COLOR_B:
		::printf("\033[01;34m");
		break;
	case Console::COLOR_B|Console::COLOR_S:
		::printf("\033[22;34m");
		break;
	case Console::COLOR_D:
		::printf("\033[0m");
		break;
	default:
		::printf("\033[0m");
		break;
	};

#endif
}


void Console::SetColor(int color)
{
	LockGuard<AtomicSpin> lock(g_tSpinConsole);
	ConsoleDoSetColor(color);
}



void Console::Write(const String& s)
{
	LockGuard<AtomicSpin> lock(g_tSpinConsole);
	std::cout<<s.c_str();
}

void Console::WriteLine(const String& s)
{
	LockGuard<AtomicSpin> lock(g_tSpinConsole);
	std::cout<<s.c_str()<<std::endl;
}


void Console::Write(const String& s,int color)
{
	LockGuard<AtomicSpin> lock(g_tSpinConsole);
	int oldcr=g_cConsoleColor;
	ConsoleDoSetColor(color);
	std::cout<<s.c_str();
	ConsoleDoSetColor(oldcr);
}

void Console::WriteLine(const String& s,int color)
{
	LockGuard<AtomicSpin> lock(g_tSpinConsole);
	int oldcr=g_cConsoleColor;
	ConsoleDoSetColor(color);
	std::cout<<s.c_str()<<std::endl;
	ConsoleDoSetColor(oldcr);
}

String Console::ReadLine()
{
	std::string msg;
	std::cin >> msg;
	return msg.c_str();
}


void Console::Pause()
{
#ifdef VHWD_WINDOWS
	system("pause");
#else
	Write("Press ENTER to continue...");
	std::cin.get();
#endif

}


VHWD_LEAVE
