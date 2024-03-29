

#include "vhwd/basic/system.h"
#include "vhwd/basic/platform.h"
#include "vhwd/basic/atomic.h"
#include "vhwd/basic/functor.h"

#ifdef VHWD_WINDOWS
#include <windows.h>
#endif

VHWD_ENTER

void OnAssertFailure(const char* msg,const char* file,long line)
{
	vhwd::System::LogTrace("assert_failure: %s at %s:%d\n",msg,file,line);
	vhwd::System::DebugBreak();
}


VHWD_DLLIMPEXP tl::int2type<1> _1;
VHWD_DLLIMPEXP tl::int2type<2> _2;
VHWD_DLLIMPEXP tl::int2type<3> _3;
VHWD_DLLIMPEXP tl::int2type<4> _4;
VHWD_DLLIMPEXP tl::int2type<5> _5;
VHWD_DLLIMPEXP tl::int2type<6> _6;


#ifdef VHWD_WINDOWS
void KO_Policy_handle::destroy(type& o)
{
	::CloseHandle(o);
}
#else
void KO_Policy_handle::destroy(type& o)
{
	::close(o);
}
#endif




VHWD_LEAVE
