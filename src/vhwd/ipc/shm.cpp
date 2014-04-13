#include "vhwd/ipc/shm.h"
#include "vhwd/logging/logger.h"
#include "vhwd/basic/system.h"

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
class ShareMem_detail : public SharedMem
{
public:
	typedef SharedMem::impl_type impl_type;

	static int makeflag(int flag_,int fr,int fw)
	{
		int acc=0;
		if(flag_&SharedMem::FLAG_RD)
		{
			acc|=fr;
		}
		if(flag_&SharedMem::FLAG_WR)
		{
			acc|=fw;
		}
		return acc;
	}

	static bool shm_create(impl_type& impl,const String& name_,size_t size_,int flag_)
	{

		HANDLE hMapFile;

		if(flag_&SharedMem::FLAG_CR)
		{
			union kk
			{
				DWORD64 dval;
				DWORD d[2];
			};

			kk tmp;
			tmp.dval=size_;

			hMapFile = CreateFileMapping(
				INVALID_HANDLE_VALUE,   // Use paging file - shared memory
				NULL,                   // Default security attributes
				PAGE_READWRITE,         // Allow read and write access
				tmp.d[1],               // High-order DWORD of file mapping max size
				tmp.d[0],               // Low-order DWORD of file mapping max size
				name_.empty()?NULL:name_.c_str()
				);


		}
		else
		{
			hMapFile = OpenFileMapping(
				makeflag(flag_,FILE_MAP_READ,FILE_MAP_WRITE),
				FALSE,                  // Do not inherit the name
				name_.c_str()			// File mapping name
				);
		}

		if (hMapFile == NULL)
		{
			return false;
		}

		char* pView = (char*)MapViewOfFile(
			hMapFile,               // Handle of the map object
			makeflag(flag_,FILE_MAP_READ,FILE_MAP_WRITE),					// access
			0,                      // High-order DWORD of the file offset
			0,					// Low-order DWORD of the file offset
			size_               // The number of bytes to map to view
			);

		if(!pView)
		{
			::CloseHandle(hMapFile);
			return false;
		}

		impl.m_pAddr=pView;
		impl.m_nSize=size_;
		impl.m_sName=name_;
		impl.m_pHandle.reset(hMapFile);

		return true;
	}

	static bool shm_openfile(impl_type& impl,const String& name_,size_t size_,int flag_)
	{

		KO_Handle<KO_Policy_handle> m_pExtraHandle;

		if(size_!=0 && (flag_&SharedMem::FLAG_WR)==0)
		{
			return false;
		}

		HANDLE hFile=(HANDLE)CreateFile(
			name_.c_str(),
			makeflag(flag_,GENERIC_READ,GENERIC_WRITE),
			makeflag(flag_,FILE_SHARE_READ,FILE_SHARE_WRITE),
			NULL,
			flag_&SharedMem::FLAG_CR?OPEN_ALWAYS:OPEN_EXISTING,
			NULL,
			NULL
			);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		if(size_!=0)
		{
			SetFilePointer(hFile, size_, NULL, FILE_BEGIN);
			SetEndOfFile(hFile);
		}

		m_pExtraHandle.reset(hFile);

		union kk
		{
			DWORD64 dval;
			DWORD d[2];
		};

		kk tmp;
		tmp.d[0]=::GetFileSize(hFile,&tmp.d[1]);
		if(tmp.dval<=0)
		{
			return false;
		}
		size_=tmp.dval;

		HANDLE hMapFile = CreateFileMapping(
			m_pExtraHandle,
			NULL,                   // Default security attributes
			flag_&SharedMem::FLAG_WR?PAGE_READWRITE:PAGE_READONLY,
			tmp.d[1],               // High-order DWORD of file mapping max size
			tmp.d[0],               // Low-order DWORD of file mapping max size
			NULL
			);

		if(hMapFile==NULL)
		{
			return false;
		}

		char* pView = (char*)MapViewOfFile(
			hMapFile,               // Handle of the map object
			makeflag(flag_,FILE_MAP_READ,FILE_MAP_WRITE),
			0,                      // High-order DWORD of the file offset
			0,						// Low-order DWORD of the file offset
			size_					// The number of bytes to map to view
			);

		if(!pView)
		{
			::CloseHandle(hMapFile);
			return false;
		}

		impl.m_pAddr=pView;
		impl.m_nSize=size_;
		impl.m_sName=name_;
		impl.m_pHandle.reset(hMapFile);
		return true;
	}

	static void shm_close(impl_type& impl)
	{
		if(!impl.m_pAddr)
		{
			return;
		}

		impl.m_pHandle.close();
		if(UnmapViewOfFile(impl.m_pAddr)==0)
		{
			System::CheckError("shm_close");
		}

		impl.m_pAddr=NULL;
		impl.m_nSize=0;
		impl.m_sName="";

	}

};

#else


class ShareMem_detail : public SharedMem
{
public:
	typedef SharedMem::impl_type impl_type;

	static int shm_makeflag(int flag_)
	{
		int acc=0;
		if(flag_&SharedMem::FLAG_RD)
		{
			acc|=PROT_READ;
		}
		if(flag_&SharedMem::FLAG_WR)
		{
			acc|=PROT_WRITE;
		}
		return acc;
	}

	static int shm_fileflag(int flag_)
	{
		int acc=0;

		if(flag_&SharedMem::FLAG_WR)
		{
            if(flag_&SharedMem::FLAG_RD)
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

	static bool shm_create(impl_type& impl,const String& name_,size_t size_,int flag_)
	{

		KO_Handle<KO_Policy_handle> hfd;
		int fd=-1;

		int map_type=MAP_SHARED;

		if(!name_.empty())
		{
			int oflag=shm_fileflag(flag_);
			if(flag_&SharedMem::FLAG_CR)
			{
				oflag|=O_CREAT;
			}

			fd=shm_open(name_.c_str(),oflag,0777);
			if(fd<0)
			{
				return false;
			}
			hfd.reset(fd);

			if(flag_&SharedMem::FLAG_CR)
			{
				ftruncate(fd,size_);
			}
			else
			{
				struct stat statbuf;
				if(fstat(fd,&statbuf)<0)
				{
					return false;
				}
				if((size_t)statbuf.st_size!=size_)
				{
					return false;
				}
			}
		}
		else
		{
			map_type=MAP_PRIVATE|MAP_ANONYMOUS;
		}

		void* _mem=mmap(0,size_,shm_makeflag(flag_),map_type,fd,0);
		if(_mem==MAP_FAILED)
		{
            return false;
		}

        impl.m_pAddr=(char*)_mem;
		impl.m_nSize=size_;
		impl.m_sName=name_;

		return true;
	}

	static bool shm_openfile(impl_type& impl,const String& name_,size_t size_,int flag_)
	{
		KO_Handle<KO_Policy_handle> hfd;

		int fd=::open(name_.c_str(),shm_fileflag(flag_),0777);
		if(fd<0)
		{
			if((flag_&SharedMem::FLAG_CR)==0||size_==0)
			{
				return false;
			}
			fd=::open(name_.c_str(),shm_fileflag(flag_)|O_CREAT,0777);
			if(fd<0)
			{
				return false;
			}
		}
		hfd.reset(fd);
		if(size_!=0)
		{
			ftruncate(fd,size_);
		}

		struct stat statbuf;
		if(fstat(fd,&statbuf)<0)
		{
			return false;
		}

		size_=statbuf.st_size;

		void* _mem=mmap(0,size_,shm_makeflag(flag_),MAP_SHARED,fd,0);
		if(_mem==MAP_FAILED)
		{
            return false;
		}

        impl.m_pAddr=(char*)_mem;
		impl.m_nSize=size_;
		impl.m_sName=name_;

		return true;

	}

	static void shm_close(impl_type& impl)
	{
		if(!impl.m_pAddr)
		{
			return;
		}

		munmap(impl.m_pAddr,impl.m_nSize);
		impl.m_pAddr=NULL;
		impl.m_nSize=0;

	}

};

#endif


SharedMem::SharedMem()
{

}

SharedMem::~SharedMem()
{

}

bool SharedMem::Alloc(size_t size_,int flag_)
{
	return Create("",size_,flag_);
}

bool SharedMem::Open(const String& name_,size_t size_,int flag_)
{
	ShareMem_detail::shm_close(impl);
	if(!ShareMem_detail::shm_create(impl,name_,size_,flag_))
	{
		System::CheckError("shm_create");
		return false;
	}
	else
	{
		return true;
	}
}

bool SharedMem::Create(const String& name_,size_t size_,int flag_)
{
	ShareMem_detail::shm_close(impl);
	if(!ShareMem_detail::shm_create(impl,name_,size_,flag_|FLAG_CR))
	{
		System::CheckError("shm_create");
		return false;
	}
	else
	{
		return true;
	}
}

bool SharedMem::OpenFile(const String& name_,size_t size_,int flag_)
{
	ShareMem_detail::shm_close(impl);
	if(!ShareMem_detail::shm_openfile(impl,name_,size_,flag_))
	{
		System::CheckError("shm_openfile");
		return false;
	}
	else
	{
		return true;
	}
}

void SharedMem::Close()
{
	ShareMem_detail::shm_close(impl);
}

VHWD_LEAVE
