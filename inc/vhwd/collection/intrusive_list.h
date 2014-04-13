#ifndef __H_VHWD_COLLECTION_INTRUSIVE_LIST__
#define __H_VHWD_COLLECTION_INTRUSIVE_LIST__

#include "vhwd/config.h"

VHWD_ENTER


template<typename T>
class policy_list_next
{
public:
	static T* GetNext(T* node)
	{
		return node->next;
	}

	static void SetNext(T* node, T* next)
	{
		node->next=next;
	}

	static void DeleteNode(T* node)
	{
		delete node;
	}
};

template<typename T,typename P=policy_list_next<T> >
class intrusive_list : public NonCopyable
{
public:
	typedef T* pointer;

	intrusive_list()
	{
		m_pHead=m_pTail=NULL;
		m_nSize=0;
	}
	
	~intrusive_list()
	{
		clear();
	}

	void clear()
	{
		while(m_pHead!=NULL)
		{
			T *_tmp=m_pHead;
			m_pHead=P::GetNext(m_pHead);
			P::DeleteNode(_tmp);
		}
		m_pHead=m_pTail=NULL;
		m_nSize=0;
	}

	bool empty() const
	{
		return m_nSize==0;
	}

	void append(T *node)
	{
		wassert(node!=NULL);
		if(m_pTail==NULL)
		{
			m_pHead=m_pTail=node;
			m_nSize=1;
		}
		else
		{
			P::SetNext(m_pTail,node);
			m_pTail=node;
			m_nSize++;
		}
	}

	void insert(T *node)
	{
		wassert(node!=NULL);
		if(m_pHead==NULL)
		{
			m_pHead=m_pTail=node;
			m_nSize=1;
		}
		else
		{
			P::SetNext(node,m_pHead);
			m_pHead=node;
			m_nSize++;
		}
	}

	size_t size() const
	{
		return m_nSize;
	}

	pointer head()
	{
		return m_pHead;
	}

	pointer tail()
	{
		return m_pTail;
	}

	const pointer head() const
	{
		return m_pHead;
	}

	const pointer tail() const
	{
		return m_pTail;
	}

	class iterator
	{
	public:
		iterator(pointer p=NULL):m_ptr(p) {}

		iterator &operator++() const
		{
			if(m_ptr)
			{
				m_ptr=P::GetNext(m_ptr);
			}
			return *this;
		}

		iterator operator++(int) const
		{
			iterator old(m_ptr);
			if(m_ptr)
			{
				m_ptr=P::GetNext(m_ptr);
			}
			return old;
		}

		T &operator*()
		{
			wassert(!m_ptr);
			return *m_ptr;
		}

		const T &operator*() const
		{
			wassert(!m_ptr);
			return *m_ptr;
		}

		bool operator==(const iterator it) const
		{
			return m_ptr==it.m_ptr;
		}

		bool operator!=(const iterator it) const
		{
			return m_ptr!=it.m_ptr;
		}

	private:
		pointer m_ptr;
	};

	typedef const iterator const_iterator;

	iterator begin()
	{
		return m_pHead;
	}

	iterator end()
	{
		return NULL;
	}

	const_iterator begin() const
	{
		return m_pHead;
	}

	const_iterator end() const
	{
		return NULL;
	}

	void release()
	{
		m_pHead=m_pTail=NULL;
		m_nSize=0;
	}

	void swap(intrusive_list &o)
	{
		std::swap(m_pHead,o.m_pHead);
		std::swap(m_pTail,o.m_pTail);
		std::swap(m_nSize,o.m_nSize);
	}

private:
	pointer m_pHead;
	pointer m_pTail;
	size_t m_nSize;
};

VHWD_LEAVE

#endif
