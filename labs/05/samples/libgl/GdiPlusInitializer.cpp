#include "pch.h"
#include "GdiPlusInitializer.h"

CGdiPlusInitializer::CGdiPlusInitializer()
{
	Gdiplus::GdiplusStartupInput input;
	Gdiplus::GdiplusStartupOutput output;

	if (Gdiplus::GdiplusStartup(&m_token, &input, &output) != Gdiplus::Ok)
	{
		// Не удалось инициализировать GDI+
		throw std::runtime_error("Failed to initialize GDI+");
	}
}

CGdiPlusInitializer::~CGdiPlusInitializer()
{
	Gdiplus::GdiplusShutdown(m_token);
}
