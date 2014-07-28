// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_IPC_PIPE__
#define __H_VHWD_IPC_PIPE__

#include "vhwd/config.h"
#include "vhwd/basic/platform.h"
#include "vhwd/basic/string.h"

VHWD_ENTER

class pipe_detail;

class VHWD_DLLIMPEXP Pipe
{
public:
	class impl_type : public KO_Handle<KO_Policy_handle>
	{
	public:
		KO_Handle<KO_Policy_handle> serv;
		String name;
	};

	Pipe();
	~Pipe();

	int Send(const char* buf,int len);
	int Recv(char* buf,int len);

	void Close();

	static bool LinkPipe(Pipe& reader,Pipe& writer,int buflen=0);

protected:
	impl_type impl;

};

class VHWD_DLLIMPEXP NamedPipe : public Pipe
{
public:
	friend class pipe_detail;

	NamedPipe() {}

// server
	bool Create(const String& name);
	bool Accept();
	void Disconnect();

//client
	bool Connect(const String& name);
	bool Connect(const String& name,int t);

};

VHWD_LEAVE

#endif
