// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_SERIALIZER_STREAM__
#define __H_VHWD_SERIALIZER_STREAM__

#include"vhwd/serialization/serializer.h"
#include <fstream>

VHWD_ENTER


class VHWD_DLLIMPEXP SerializerReaderStream : public SerializerReader
{
public:
	SerializerReaderStream(std::istream& is):fs(is){}

	bool good() const {return fs.good();}

protected:
	void recv(char* data,size_t size);
	std::istream& fs;
};


class VHWD_DLLIMPEXP SerializerWriterStream : public SerializerWriter
{
public:

	SerializerWriterStream(std::ostream& is):fs(is){}
	bool good() const {return fs.good();}

protected:
	void send(char* data,size_t size);
	std::ostream& fs;
};

template<typename T>
class VHWD_DLLIMPEXP StreamSerializer;

template<>
class VHWD_DLLIMPEXP StreamSerializer<SerializerReader> : public SerializerReaderStream
{
public:
	StreamSerializer():SerializerReaderStream(fs){}

	bool open(const String &file)
	{
		clear();
		fs.open(file.c_str(),std::ios::in|std::ios::binary);
		return fs.good();
	}

	void close()
	{
		fs.close();
	}

	std::ifstream fs;
};

template<>
class VHWD_DLLIMPEXP StreamSerializer<SerializerWriter> : public SerializerWriterStream
{
public:

	StreamSerializer():SerializerWriterStream(fs){}
	std::ofstream fs;

	bool open(const String &file)
	{
		clear();
		fs.open(file.c_str(),std::ios::out|std::ios::binary);
		return fs.good();
	}

	void close()
	{
		fs.close();
	}

};



VHWD_LEAVE
#endif

