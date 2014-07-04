#include "vhwd/basic/bitflags.h"

VHWD_ENTER

BitFlags::BitFlags(int32_t f)
{
	m_nFlags=f;
}

void BitFlags::add(int32_t f)
{
	m_nFlags|=f;
}

int32_t BitFlags::val() const
{
	return m_nFlags;
}
void BitFlags::del(int32_t f)
{
	m_nFlags&=~f;
}

bool BitFlags::get(int32_t f) const
{
	return (m_nFlags&f)!=0;
}

void BitFlags::clr(int32_t f)
{
	m_nFlags=f;
}

void BitFlags::inv(int32_t f)
{
	m_nFlags=(m_nFlags^f);
}

void BitFlags::set(int32_t f,bool v)
{
	if(v)
	{
		add(f);
	}
	else
	{
		del(f);
	}
}

VHWD_LEAVE
