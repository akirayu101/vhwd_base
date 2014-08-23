// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_COLLECTION_RING_BUFFER__
#define __H_VHWD_COLLECTION_RING_BUFFER__

#include "vhwd/collection/detail/collection_base.h"
#include "vhwd/basic/atomic.h"
#include "vhwd/basic/system.h"
#include <cstring>

VHWD_ENTER


class VHWD_DLLIMPEXP ringbuffer_header
{
public:
	int32_t rd_pos;
	int32_t wr_pos;
	int32_t rb_mask;
	int32_t padding;
};

// RingBufferBase one thread read/one thread write
class VHWD_DLLIMPEXP RingBufferBase
{
public:

	RingBufferBase()
	{
		pBuffer=NULL;
		pHeader=NULL;
	}

	void rewind();

	void wr_flip(int n)
	{
		pHeader->wr_pos=(pHeader->wr_pos+n)&pHeader->rb_mask;
	}
	void rd_flip(int n)
	{
		pHeader->rd_pos=(pHeader->rd_pos+n)&pHeader->rb_mask;
	}

	bool packet();
	int packet(void* p_,int n);

	int peek(void* p_,int n);
	int recv(void* p_,int n);
	int send(const void* p_,int n);

	int rd_free();
	int wr_free();


	char* pBuffer;
	ringbuffer_header* pHeader;
};


class VHWD_DLLIMPEXP RingBuffer : public RingBufferBase, private NonCopyable
{
public:

	RingBuffer();
	~RingBuffer();

	void reset(int s);
	void clear();

private:
	ringbuffer_header _header;
};

VHWD_LEAVE

#endif
