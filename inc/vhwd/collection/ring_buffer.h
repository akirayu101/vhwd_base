// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_COLLECTION_RING_BUFFER__
#define __H_VHWD_COLLECTION_RING_BUFFER__

#include "vhwd/collection/collection_base.h"
#include "vhwd/basic/atomic.h"
#include "vhwd/basic/system.h"


#include <cstring>

VHWD_ENTER


class VHWD_DLLIMPEXP ringbuffer_header
{
public:
	int32_t rd_pos;
	int32_t wr_pos;
	int32_t mask;
	int32_t padding;
};

// RingBufferBase one thread read/one thread write
class VHWD_DLLIMPEXP RingBufferBase : private NonCopyable
{
public:

	RingBufferBase(){pBuffer=NULL;pHeader=NULL;}

	void rewind()
	{
		if(!pHeader)
		{
			System::LogTrace("try to rewind empty ringbuffer");
			return;
		}
		pHeader->wr_pos=pHeader->rd_pos=0;
	}

	void wr_flip(int n)
	{
		pHeader->wr_pos=(pHeader->wr_pos+n)&pHeader->mask;
	}

	void rd_flip(int n)
	{
		pHeader->rd_pos=(pHeader->rd_pos+n)&pHeader->mask;
	}

	bool packet()
	{
		unsigned short size=0;
		if(peek(&size,2)!=2)
		{
			return false;
		}
		int rd=pHeader->rd_pos;
		int wr=pHeader->wr_pos;
		int kp=(wr-rd)&pHeader->mask;

		return size<=kp;
	}


	int packet(void* p_,int n)
	{
		unsigned short size=0;
		if(peek(&size,2)!=2)
		{
			return 0;
		}
		int rd=pHeader->rd_pos;
		int wr=pHeader->wr_pos;
		int kp=(wr-rd)&pHeader->mask;

		if(size>kp)
		{
			return 0;
		}

		if(n<size)
		{
			return -size;
		}

		return recv(p_,size);
	}


	int peek(void* p_,int n)
	{
		char* p=(char*)p_;
		int rd=pHeader->rd_pos;
		int wr=pHeader->wr_pos;
		int kp=(wr-rd)&pHeader->mask;

		if(kp>n)
		{
			kp=n;
		}
		else if(kp==0)
		{
			return 0;
		}

		int kn=rd+kp-pHeader->mask-1;
		if(kn>0)
		{
			memcpy(p,pBuffer+rd,kp-kn);
			memcpy(p+kp-kn,pBuffer,kn);
		}
		else
		{
			memcpy(p,pBuffer+rd,kp);
		}

		return kp;
	}

	int rd_free()
	{
		int rd=pHeader->rd_pos;
		int wr=pHeader->wr_pos;
		int kp=(wr-rd)&pHeader->mask;
		return kp;
	}

	int recv(void* p_,int n)
	{
		char* p=(char*)p_;
		int rd=pHeader->rd_pos;
		int wr=pHeader->wr_pos;
		int kp=(wr-rd)&pHeader->mask;

		if(kp==0)
		{
			return 0;
		}
		if(kp>n)
		{
			kp=n;
		}

		int kn=rd+kp-pHeader->mask-1;
		if(kn>0)
		{
			memcpy(p,pBuffer+rd,kp-kn);
			memcpy(p+kp-kn,pBuffer,kn);
			pHeader->rd_pos=kn;
		}
		else
		{
			memcpy(p,pBuffer+rd,kp);
			pHeader->rd_pos=kn&pHeader->mask;
		}

		return kp;
	}

	int wr_free()
	{
		int rd=pHeader->rd_pos;
		int wr=pHeader->wr_pos;
		int kp=(rd-wr-1)&pHeader->mask;
		return kp;
	}

	int send(const void* p_,int n)
	{
		const char* p=(const char*)p_;

		int rd=pHeader->rd_pos;
		int wr=pHeader->wr_pos;
		int kp=(rd-wr-1)&pHeader->mask;

		if(kp>=n)
		{
			kp=n;
		}
		else
		{
			return 0;
		}

		int kn=wr+kp-pHeader->mask-1;
		if(kn>0)
		{
			memcpy(pBuffer+wr,p,kp-kn);
			memcpy(pBuffer,p+kp-kn,kn);
			pHeader->wr_pos=kn;
		}
		else
		{
			memcpy(pBuffer+wr,p,kp);
			pHeader->wr_pos=kn&pHeader->mask;
		}

		return kp;
	}

	char* pBuffer;
	ringbuffer_header* pHeader;
};


class VHWD_DLLIMPEXP RingBuffer : public RingBufferBase
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
