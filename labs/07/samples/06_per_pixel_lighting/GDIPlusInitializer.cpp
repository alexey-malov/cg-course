#include "StdAfx.h"
#include "GDIPlusInitializer.h"

CGDIPlusInitializer::CGDIPlusInitializer(void)
{
	Gdiplus::GdiplusStartupInput input;
	Gdiplus::GdiplusStartupOutput output;

	if (Gdiplus::GdiplusStartup(&m_token, &input, &output) != Gdiplus::Ok)
	{
		// Не удалось инициализировать GDI+
		throw std::runtime_error("Failed to initialize GDI+");
	}
}

CGDIPlusInitializer::~CGDIPlusInitializer(void)
{
	Gdiplus::GdiplusShutdown(m_token);
}
