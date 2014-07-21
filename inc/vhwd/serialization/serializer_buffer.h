// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_SERIALIZER_BUFFER__
#define __H_VHWD_SERIALIZER_BUFFER__

#include "vhwd/serialization/serializer.h"
#include "vhwd/collection/linear_buffer.h"


VHWD_ENTER



class VHWD_DLLIMPEXP SerializerBuffer : public SerializerDuplex
{
public:

	SerializerBuffer();

	// skip data, return true if gptr()==gend();
	bool skip(); 

	// assign external buffer
	void assign(char* pbuf,size_t size);

	// allocate buffer
	void alloc(size_t bufsize);


	char* gbeg(){return lbuf.gbeg();} // buffer begin
	char* gptr(){return lbuf.gptr();} // get position begin
	char* gend(){return lbuf.gend();} // get position end or put position begin
	char* last(){return lbuf.last();}

protected:

	void send(char* data,size_t size);
	void recv(char* data,size_t size);

	LinearBuffer<char> lbuf;

};


VHWD_LEAVE
#endif

