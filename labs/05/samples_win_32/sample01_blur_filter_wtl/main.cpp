// sample01_blur_filter_wtl.cpp : main source file for sample01_blur_filter_wtl.exe
//

#include "stdafx.h"

#include "resource.h"

#include "MainView.h"
#include "aboutdlg.h"
#include "MainFrm.h"

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;

	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

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


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpstrCmdLine,
	_In_ int       nCmdShow)
{
	CGdiplusInitializer initGdiplus;
	try
	{
		HRESULT hRes = ::CoInitialize(NULL);
		// If you are running on NT 4.0 or higher you can use the following call instead to 
		// make the EXE free threaded. This means that calls come in on a random RPC thread.
		//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
		ATLASSERT(SUCCEEDED(hRes));

		AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

		hRes = _Module.Init(NULL, hInstance);
		ATLASSERT(SUCCEEDED(hRes));

		int nRet = Run(lpstrCmdLine, nCmdShow);

		_Module.Term();
		::CoUninitialize();

		return nRet;
	}
	catch (std::runtime_error &)
	{
		return -1;
	}
}
