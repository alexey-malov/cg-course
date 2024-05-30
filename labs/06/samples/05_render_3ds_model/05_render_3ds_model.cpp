// 05_render_3ds_model.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MyApplication.h"


class CGdiplusInitializer
{
public:
	CGdiplusInitializer()
	{
		Gdiplus::GdiplusStartupInput input;
		Gdiplus::GdiplusStartupOutput output;

		if (Gdiplus::GdiplusStartup(&m_token, &input, &output) != Gdiplus::Ok)
		{
			// Не удалось инициализировать GDI+
			throw std::runtime_error("Failed to initialize GDI+");
		}
	}

	~CGdiplusInitializer()
	{
		Gdiplus::GdiplusShutdown(m_token);
	}
private:
	ULONG_PTR m_token;
};

CMyApplication app("3ds model rendering", 800, 600);
CGdiplusInitializer gdiplusInitializer;

int _tmain(int /*argc*/, _TCHAR* /*argv*/[])
{
	glewInit();
	try
	{
		app.MainLoop();
	}
	catch (std::exception const& e)
	{
		std::cout << e.what();
	}
	return 0;
}
