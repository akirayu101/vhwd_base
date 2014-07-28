#include "vhwd/basic/file.h"
#include <cstdio>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <cerrno>
#include <unistd.h>
#endif

VHWD_ENTER


#ifdef _WIN32

String fn_encode(const String& oldname_)
{
	return oldname_;
}


bool File::Open(const String& filename_,int flag_)
{
	String fn=fn_encode(filename_);
	Close();

	HANDLE hFile=(HANDLE)CreateFileA(
					 fn.c_str(),
					 FileAccess::makeflag(flag_,GENERIC_READ,GENERIC_WRITE),
					 FileAccess::makeflag(flag_,FILE_SHARE_READ,FILE_SHARE_WRITE),
					 NULL,
					 flag_&FileAccess::FLAG_CR?OPEN_ALWAYS:OPEN_EXISTING,
					 NULL,
					 NULL
				 );


	if (hFile == INVALID_HANDLE_VALUE)
	{
		m_bGood=false;
		return false;
	}

	m_bGood=true;

	impl.reset(hFile);

	if(flag_&FileAccess::FLAG_APPEND)
	{
		Seek(0,FILEPOS_END);
	}


	return true;
}

int64_t File::Size()
{
	FileAccess::LargeInteger tmp;
	tmp.d[0]=::GetFileSize(impl,&tmp.d[1]);
	return tmp.dval;
}

int32_t File::Read(char* buf,size_t len)
{
	DWORD nRead(0);
	if(::ReadFile(impl,buf,len,&nRead,NULL)==FALSE)
	{
		m_bGood=false;
		System::CheckError("File::Read Error");
		return -1;
	}
	return nRead;
}

int32_t File::Write(const char* buf,size_t len)
{
	DWORD nWrite(0);
	if(::WriteFile(impl,buf,len,&nWrite,NULL)==FALSE)
	{
		m_bGood=false;
		System::CheckError("File::Write Error");
		return -1;
	}
	return nWrite;
}

bool File::Eof()
{
	return Size()==Tell();
}

int64_t File::Tell()
{
	return Seek(0,FILEPOS_CUR);
}

int64_t File::Seek(int64_t pos,int t)
{
	LARGE_INTEGER li;
	li.QuadPart=pos;
	li.LowPart = SetFilePointer (impl,
								 li.LowPart,
								 &li.HighPart,
								 t);

	if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
	{
		return -1;
	}

	return li.QuadPart;
}


void File::Rewind()
{
	Seek(0,FILEPOS_BEG);
}

void File::Flush()
{
	::FlushFileBuffers(impl);
}

void File::Truncate(size_t s)
{
	Seek(s,FILEPOS_BEG);
	SetEndOfFile(impl);
}

#else


String fn_encode(const String& oldname_)
{
	return oldname_;
}

static int shm_fileflag(int flag_)
{
	int acc=0;

	if(flag_&FileAccess::FLAG_WR)
	{
		if(flag_&FileAccess::FLAG_RD)
		{
			acc|=O_RDWR;
		}
		else
		{
			acc|=O_WRONLY;
		}
	}

	return acc;
}

bool File::Open(const String& filename_,int flag_)
{
	String filename=fn_encode(filename_);

	int fd=::open(filename.c_str(),shm_fileflag(flag_),0777);
	if(fd<0)
	{
		if((flag_&FileAccess::FLAG_CR)==0)
		{
			m_bGood=false;
			return false;
		}
		fd=::open(filename.c_str(),shm_fileflag(flag_)|O_CREAT,0777);
		if(fd<0)
		{
			m_bGood=false;
			return false;
		}
	}

	m_bGood=true;
	impl.reset(fd);

	if(flag_&FileAccess::FLAG_APPEND)
	{
		Seek(0,FILEPOS_END);
	}

	return true;
}

int64_t File::Size()
{
	struct stat statbuf;
	if(fstat(impl,&statbuf)<0)
	{
		return -1;
	}

	return statbuf.st_size;
}



int32_t File::Read(char* buf,size_t len)
{
	int nLen= ::read(impl,buf,len);
	if(nLen<0)
	{
		m_bGood=false;
	}
	return nLen;
}

int32_t File::Write(const char* buf,size_t len)
{
	int nLen=::write(impl,buf,len);
	if(nLen<0)
	{
		m_bGood=false;
	}
	return nLen;
}

bool File::Eof()
{
	return Size()==Tell();
}

int64_t File::Tell()
{
	return Seek(0,FILEPOS_CUR);
}

int64_t File::Seek(int64_t pos,int t)
{
	return lseek(impl,pos,t);
}


void File::Rewind()
{
	Seek(0,FILEPOS_BEG);
}

void File::Flush()
{
	::fsync(impl);
}

void File::Truncate(size_t size_)
{
	Seek(size_,FILEPOS_BEG);
	ftruncate(impl,size_);
}

#endif

File::File()
{
	m_bGood=true;
}

File::File(const String& filename_,int op)
{
	m_bGood=true;
	Open(filename_,op);
}

File::~File()
{
	Close();
}

void File::Close()
{
	impl.close();
}

bool File::Rename(const String& oldname_,const String& newname_)
{
	String oldname=fn_encode(oldname_);
	String newname=fn_encode(newname_);

	int bRet=::rename(oldname.c_str(),newname.c_str());
	return bRet==0;
}

bool File::Remove(const String& filename_)
{
	String fn=fn_encode(filename_);
	int bRet=::remove(fn.c_str());
	return bRet==0;
}


VHWD_LEAVE
