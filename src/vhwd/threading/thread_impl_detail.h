#ifdef _MSC_VER
#include "thread_impl_windows.h"
typedef vhwd::ThreadImpl_windows ThreadImpl_detail;
#else
#include "thread_impl_pthread.h"
typedef vhwd::ThreadImpl_pthread ThreadImpl_detail;
#endif

