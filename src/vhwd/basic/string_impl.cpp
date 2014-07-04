#include "string_impl.h"

VHWD_ENTER

void GBKTable_init();
StringPool::StringPool():pool(MyPool::current())
{
	GBKTable_init();
}

VHWD_LEAVE
