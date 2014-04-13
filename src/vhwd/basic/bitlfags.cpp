#include "vhwd/basic/bitflags.h"

VHWD_ENTER

BitFlags::BitFlags(int32_t f)
{
	m_flags=f;
}

void BitFlags::add(int32_t f)
{
	m_flags|=f;
}

int32_t BitFlags::val() const
{
	return m_flags;
}
void BitFlags::del(int32_t f)
{
	m_flags&=~f;
}

bool BitFlags::get(int32_t f) const
{
	return (m_flags&f)!=0;
}

void BitFlags::clr(int32_t f)
{
	m_flags=f;
}

void BitFlags::inv(int32_t f)
{
	m_flags=(m_flags^f);
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
