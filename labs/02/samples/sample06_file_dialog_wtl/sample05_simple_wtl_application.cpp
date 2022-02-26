// sample05_simple_wtl_application.cpp : main source file for sample05_simple_wtl_application.exe
//

#include "stdafx.h"

#include "resource.h"

#include "sample05_simple_wtl_applicationView.h"
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

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	try
	{
		// Инициализируем GDI+
		CGdiplusInitializer initializer;

		HRESULT hRes = ::CoInitialize(NULL);
	// If you are running on NT 4.0 or higher you can use the following call instead to 
	// make the EXE free threaded. This means that calls come in on a random RPC thread.
	//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
		ATLASSERT(SUCCEEDED(hRes));

		// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
		::DefWindowProc(NULL, 0, 0, 0L);

		AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

		hRes = _Module.Init(NULL, hInstance);
		ATLASSERT(SUCCEEDED(hRes));

		int nRet = Run(lpstrCmdLine, nCmdShow);

		_Module.Term();
		::CoUninitialize();

		return nRet;
	}
	catch (std::runtime_error & )
	{
		return -1;
	}
}
