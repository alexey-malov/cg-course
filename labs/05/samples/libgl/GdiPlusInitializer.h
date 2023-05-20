#pragma once

#include "common_inc.h"

class CGdiPlusInitializer
{
public:
	CGdiPlusInitializer();

	~CGdiPlusInitializer();

private:
	ULONG_PTR m_token;
};
