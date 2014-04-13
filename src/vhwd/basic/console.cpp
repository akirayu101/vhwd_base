#include "vhwd/basic/console.h"
#include "vhwd/basic/string.h"
#include "vhwd/threading/thread_spin.h"
#include "vhwd/threading/thread_mutex.h"
#include "vhwd/threading/lockguard.h"

#include <cstdio>
#include <iostream>

VHWD_ENTER

static int g_cConsoleColor=Console::COLOR_D;

SpinLock& GetConsoleSpinLock()
{
	static StaticObjectWithoutDeletorT<SpinLock> spin_console;
	return spin_console;
}


void ConsoleDoSetColor(int color)
{
	g_cConsoleColor=color;

#ifdef _WIN32
	HANDLE hCon=GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hCon,color);
#else
	switch(color)
	{
	case 0:
		::printf("\033[22;30m");break;
	case Console::COLOR_R:
		::printf("\033[01;31m");break;
	case Console::COLOR_R|Console::COLOR_S:
		::printf("\033[22;31m");break;
	case Console::COLOR_G:
		::printf("\033[01;32m");break;
	case Console::COLOR_G|Console::COLOR_S:
		::printf("\033[22;32m");break;
	case Console::COLOR_B:
		::printf("\033[01;34m");break;
	case Console::COLOR_B|Console::COLOR_S:
		::printf("\033[22;34m");break;
	case Console::COLOR_D:
		::printf("\033[0m");break;
	default:
		::printf("\033[0m");break;
	};

#endif
}


void Console::SetColor(int color)
{
	LockGuard<SpinLock> lock(GetConsoleSpinLock());
	ConsoleDoSetColor(color);
}



void Console::Write(const String& s)
{
	LockGuard<SpinLock> lock(GetConsoleSpinLock());
	std::cout<<s;
}

void Console::WriteLine(const String& s)
{
	LockGuard<SpinLock> lock(GetConsoleSpinLock());
	std::cout<<s<<std::endl;
}


void Console::ColoredWrite(int color,const String& s)
{
	LockGuard<SpinLock> lock(GetConsoleSpinLock());
	int oldcr=g_cConsoleColor;
	ConsoleDoSetColor(color);
	std::cout<<s;
	ConsoleDoSetColor(oldcr);
}

void Console::ColoredWriteLine(int color,const String& s)
{
	LockGuard<SpinLock> lock(GetConsoleSpinLock());
	int oldcr=g_cConsoleColor;
	ConsoleDoSetColor(color);
	std::cout<<s<<std::endl;
	ConsoleDoSetColor(oldcr);
}

String Console::ReadLine()
{
	String msg;
	std::cin >> msg;
	return msg;
}


void Console::Pause()
{
#ifdef _WIN32
	system("pause");
#else
	Write("Press ENTER to continue...");
	std::cin.get();
#endif

}


VHWD_LEAVE
