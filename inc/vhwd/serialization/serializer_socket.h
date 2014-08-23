// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_SERIALIZER_SOCKET__
#define __H_VHWD_SERIALIZER_SOCKET__

#include "vhwd/serialization/serializer.h"
#include "vhwd/net/ipaddress.h"
#include "vhwd/net/socket.h"
#include "vhwd/collection/linear_buffer.h"

VHWD_ENTER




// Usage:
// SerializerSocket ar;
//
// //reader
// if(ar.rd_sync()) // recv data from socket and store to recv buffer.
// {
//	 ar.reader() & obj & ...; // read data from recv buffer.
//	 // call ar.rd_test() to test read position reach the end of recv buffer.
// }
//
// //writer
// ar.writer() & obj & ...; write data to send buffer;
// if(ar.wr_sync()) // send buffer to socket
// {
//
// }
// //call ar.wr_skip() to clear send buffer;
//
class VHWD_DLLIMPEXP SerializerSocket : public SerializerDuplex
{
public:

	SerializerSocket();

	// get writer
	SerializerWriter& writer()
	{
		return *this;
	}

	// get reader
	SerializerReader& reader()
	{
		return *this;
	}


	void close()
	{
		sock.Close();
	}

	// read data from socket and store to buffer.
	bool rd_sync();

	// test if read position reach the end of the buffer
	bool rd_test();


	// send buffer to socket and clear buffer.
	bool wr_sync();

	// clear buffer
	void wr_skip();

	Socket sock;

protected:
	LinearBuffer<char> lbuf_rd;
	LinearBuffer<char> lbuf_wr;

	size_t recv(char* data,size_t size_);
	size_t send(const char* data,size_t size_);

};






VHWD_LEAVE
#endif

