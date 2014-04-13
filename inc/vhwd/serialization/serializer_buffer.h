#ifndef __H_VHWD_SERIALIZER_BUFFER__
#define __H_VHWD_SERIALIZER_BUFFER__

#include "vhwd/serialization/serializer.h"
#include "vhwd/net/ipaddress.h"
#include "vhwd/net/socket.h"

VHWD_ENTER



class VHWD_DLLIMPEXP SerializerBuffer : protected SerializerReader, protected SerializerWriter
{
public:

	SerializerBuffer();

	// skip data, return true if gptr()==gend();
	bool skip(); 

	// assign external buffer
	void assign(void* pbuf,size_t size);

	// allocate buffer
	void alloc(size_t bufsize);

	SerializerWriter& writer(){return *this;}
	SerializerReader& reader(){return *this;}

	char* gbeg(){return m_pBuff;} // buffer begin
	char* gptr(){return m_pBuff+m_nGpos;} // get position begin
	char* gend(){return m_pBuff+m_nGend;} // get position end or put position begin

protected:

	bool _grow(size_t _newsize);

	bool send(char* data,size_t size);
	bool recv(char* data,size_t size);

	char* m_pBuff;
	size_t m_nLast; // buffer size
	size_t m_nGpos; // get position begin
	size_t m_nGend; // get position end or put position begin

	arr_1t<char> m_aBuff;

};



VHWD_LEAVE
#endif

