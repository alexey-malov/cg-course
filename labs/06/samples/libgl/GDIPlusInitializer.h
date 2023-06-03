#pragma once

class CGDIPlusInitializer
{
public:
	CGDIPlusInitializer();
	~CGDIPlusInitializer();

private:
	CGDIPlusInitializer(CGDIPlusInitializer const&);
	CGDIPlusInitializer& operator=(CGDIPlusInitializer const&);
	ULONG_PTR m_token;
};
