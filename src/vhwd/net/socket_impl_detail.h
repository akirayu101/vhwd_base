#ifdef _WIN32

#include "socket_impl_windows.h"
typedef vhwd::SocketImpl_windows socket_detail;

#else

#include "socket_impl_linux.h"
typedef vhwd::SocketImpl_linux socket_detail;

#endif

