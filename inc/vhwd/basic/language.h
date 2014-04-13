#ifndef __H_VHWD_BASIC_LANGUAGE__
#define __H_VHWD_BASIC_LANGUAGE__

#include "vhwd/config.h"


VHWD_ENTER

class VHWD_DLLIMPEXP String;

class VHWD_DLLIMPEXP Language : public NonCopyable
{
public:
	Language();
	~Language();

	// Init with a file, po file or files that saved by Language::Save
	bool Init(const String& file);

	bool Save(const String& file);

	// Translate string
	const String& Translate(const String& msg) const;

	static Language& current();

private:
	void* impl;
};

VHWD_LEAVE
#endif
