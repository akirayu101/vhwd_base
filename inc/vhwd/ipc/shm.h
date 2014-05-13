// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_IPC_SHM__
#define __H_VHWD_IPC_SHM__

#include "vhwd/config.h"
#include "vhwd/basic/platform.h"
#include "vhwd/basic/string.h"
#include "vhwd/basic/file.h"

VHWD_ENTER

template<typename P1>
class KO_Handle_shm
{
public:

	KO_Handle_shm()
	{
		m_pAddr=NULL;
		m_nSize=0;
	}

	char* m_pAddr;
	size_t m_nSize;
	String m_sName;
	KO_Handle<P1> m_pHandle;
};


class VHWD_DLLIMPEXP SharedMem : private NonCopyable
{
public:

	SharedMem();
	~SharedMem();

	enum
	{
		FLAG_RD=1<<0,
		FLAG_WR=1<<1,
		FLAG_RW=FLAG_RD|FLAG_WR,
		FLAG_CR=1<<3,
	};

	typedef KO_Handle_shm<KO_Policy_handle> impl_type;

	// get the size of the shared memory.
	inline size_t size() const{return impl.m_nSize;}

	// get the starting address of the shared memory.
	char* addr(){return impl.m_pAddr;}
	const char* addr() const {return impl.m_pAddr;}

	// Create/Open shared memory with given name and given size.
	bool Create(const String& name_,size_t size_,int flag_=FileAccess::FLAG_RW);
	bool Open(const String& name_,size_t size_,int flag_=FileAccess::FLAG_RW);

	// mapping a file to memory, zero size_ means using the real file size, if not zero, the file will be truncated to size_.
	// if need to create a new file, size_ must be specified and FLAG_CR flag must be provided.
	bool OpenFile(const String& file_,size_t size_=0,int flag_=FileAccess::FLAG_RW);

	// just allocate memory, not mapping to any file.
	bool Alloc(size_t size,int flag_=FileAccess::FLAG_RW);

	// Close shared memory.
	void Close();

protected:
	impl_type impl;
};

VHWD_LEAVE
#endif
