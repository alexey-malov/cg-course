#pragma once
#include <windows.h>

class CGDIPlusInitializer
{
public:
	CGDIPlusInitializer();
	~CGDIPlusInitializer();

	CGDIPlusInitializer(CGDIPlusInitializer const&) = delete;
	CGDIPlusInitializer& operator=(CGDIPlusInitializer const&) = delete;

private:
	ULONG_PTR m_token;
};
