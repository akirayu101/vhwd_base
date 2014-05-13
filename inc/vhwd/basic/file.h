// Copyright 2014, Wenda han.  All rights reserved.
// https://github.com/vhwd/vhwd_base
//
/// Use of this source code is governed by Apache License
// that can be found in the License file.
// Author: Wenda Han.

#ifndef __H_VHWD_BASIC_FILE__
#define __H_VHWD_BASIC_FILE__

#include "vhwd/basic/string.h"
#include "vhwd/basic/platform.h"


VHWD_ENTER



class VHWD_DLLIMPEXP File
{
public:

	enum
	{
		FILEPOS_BEG,
		FILEPOS_CUR,
		FILEPOS_END
	};

	typedef KO_Handle<KO_Policy_handle> impl_type;

	typedef int64_t pos_t;

	static bool Rename(const String& oldname_,const String& newname_);
	static bool Remove(const String& filename_);

	File();
	File(const String& filename_,int op=FileAccess::FLAG_RD);
	~File();

	bool Open(const String& filename_,int op=FileAccess::FLAG_RD);
	void Close();

	int64_t Size();

	int32_t Read(char* buf,size_t len);
	int32_t Write(const char* buf,size_t len);

	int64_t Seek(int64_t pos,int t);
	int64_t Tell();

	void Rewind();

	void Flush();
	void Truncate(size_t size_);

	bool Eof();

	void swap(impl_type& o){impl.swap(o);}
	void swap(File& o){impl.swap(o.impl);}

	impl_type::type native_handle(){return impl;}

	bool Ok(){return impl.ok();}

private:

	impl_type impl;
};


VHWD_LEAVE
#endif
