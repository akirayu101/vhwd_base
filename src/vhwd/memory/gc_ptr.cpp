
#include "vhwd/memory/gc_ptr.h"
#include "vhwd/basic.h"
#include "vhwd/logging.h"

VHWD_ENTER

gc_state::dlinkptr_type gc_state::tPtrLink;

uint32_t gc_state::nNumberOfObject=0;
uint32_t gc_state::nNumberOfObjectLast=0;

AtomicMutex gc_state::tRecursiveMutex;
gc_obj* gc_state::pLinkOfAllObject=NULL;

uint32_t gc_state::nStepingDelta=1024*128;
gc_state::addrinfo_type gc_state::tCurrentObject;

BitFlags gc_state::flags;

inline void gc_handler::mark(gc_state::queue_type& q)
{
	for(gc_state::dlinkptr_type* p=gc_state::tPtrLink._gc_next;p;p=p->_gc_next)
	{
		gc_obj* o=p->m_ptr;
		if(o)
		{
			o->_gc_tags=gc_state::TAG_MARK;
			o->_gc_mark(q);
		}
	}
}


void gc_handler::gc_nolock(bool force_marking_)
{
	ElapsedTimer tt;
	tt.tick();

	uint32_t p1=gc_state::nNumberOfObject;

	if(force_marking_)
	{
		for(gc_obj* p=gc_state::pLinkOfAllObject;p;p=p->_gc_next)
		{
			p->_gc_tags=gc_state::TAG_FLAG;
		}
	}

	gc_state::queue_type q1;
	gc_state::queue_type q2;

	gc_handler::mark(q1);

	try
	{
		while(!q1.empty())
		{
			q2.reset();
			for(gc_state::queue_type::iterator it=q1.begin();it!=q1.end();++it)
			{
				(*it)->_gc_mark(q2);
			}
			q1.swap(q2);
		}
	}
	catch(...)
	{
		System::LogError("error in gc_handler::gc_nolock");
		return;
	}

	q1.clear();
	q2.clear();

	gc_obj* p=gc_state::pLinkOfAllObject;
	gc_obj* _pLinkOfConnectedObjects=NULL;

	gc_state::pLinkOfAllObject=NULL;

	while(p)
	{
		gc_obj* x=p;
		p=p->_gc_next;;

		if(x->_gc_tags==gc_state::TAG_MARK)
		{
			x->_gc_tags=gc_state::TAG_FLAG;
			x->_gc_next=_pLinkOfConnectedObjects;
			_pLinkOfConnectedObjects=x;
			continue;
		}

		gc_handler::destroy(x);
	}

	gc_state::pLinkOfAllObject=_pLinkOfConnectedObjects;
	int p2=gc_state::nNumberOfObject;
	tt.tack();

	gc_state::nNumberOfObjectLast=gc_state::nNumberOfObject;

	if(gc_state::flags.get(gc_state::FLAG_SHOW_GC_INFO))
	{
		this_logger().LogMessage("gc: %d -> %d objects, %g sec used",p1,p2,tt.get());
	}

}

void garbage_force_collect()
{
	gc_state::tRecursiveMutex.lock();
	gc_handler::gc_nolock(true);
	gc_state::tRecursiveMutex.unlock();
}

VHWD_LEAVE
