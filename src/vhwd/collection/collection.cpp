#include "vhwd/collection/detail/indexer_base.h"

#include "vhwd/collection.h"
#include "vhwd/basic/string.h"
#include "vhwd/basic/stringbuffer.h"
#include "vhwd/basic/codecvt.h"
#include "vhwd/basic/system.h"


VHWD_ENTER

RingBuffer::RingBuffer()
{
	pHeader=&_header;
	pBuffer=NULL;
}

RingBuffer::~RingBuffer()
{
	clear();
}

void RingBuffer::reset(int s)
{
	int k=sz_helper::n2p(s);
	pBuffer=(char*)malloc(k);
	if(!pBuffer)
	{
		Exception::XBadAlloc();
	}

	clear();
	_header.mask=k-1;
	_header.rd_pos=0;
	_header.wr_pos=0;

}

void RingBuffer::clear()
{
	if(pBuffer)
	{
		::free(pBuffer);
		pBuffer=NULL;
	}
}


template<typename T>
bool LinearBuffer<T>::_grow(size_type _newsize)
{
	if(pBuffer!=NULL && pBuffer!=aBuff.data())
	{
		return false;
	}

	size_type _size=sz_helper::n2p(_newsize+(_newsize>>2));
	if(_size<=_newsize) _size=_newsize<<1;

	aBuff.resize(_size);
	pBuffer=aBuff.data();
	sz_buf=aBuff.size();
	return true;	
}

template class LinearBuffer<char>;
template class LinearBuffer<unsigned char>;
//template class LinearBuffer<short>;
//template class LinearBuffer<unsigned short>;
//template class LinearBuffer<int>;
//template class LinearBuffer<unsigned int>;
template class LinearBuffer<wchar_t>;

VHWD_LEAVE
