#include "pch.h"
#include "GDIPlusInitializer.h"

CGDIPlusInitializer::CGDIPlusInitializer()
{
	Gdiplus::GdiplusStartupInput input;
	Gdiplus::GdiplusStartupOutput output;

	if (Gdiplus::GdiplusStartup(&m_token, &input, &output) != Gdiplus::Ok)
	{
		// Не удалось инициализировать GDI+
		throw std::runtime_error("Failed to initialize GDI+");
	}
}

CGDIPlusInitializer::~CGDIPlusInitializer()
{
	Gdiplus::GdiplusShutdown(m_token);
}
