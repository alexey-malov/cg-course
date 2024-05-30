#pragma once

class CGDIPlusInitializer
{
public:
	CGDIPlusInitializer(void);
	~CGDIPlusInitializer(void);

private:
	CGDIPlusInitializer(CGDIPlusInitializer const&);
	CGDIPlusInitializer& operator=(CGDIPlusInitializer const&);
	ULONG_PTR m_token;
};
