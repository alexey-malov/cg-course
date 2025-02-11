// sample03_1_custom_window_with_class.cpp : Defines the entry point for the application.
//

#include "sample03_1_custom_window_with_class.h"
#include "Finally.h"
#include "framework.h"
#include <stdexcept>
#include <windowsx.h>

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

class Window
{
public:
	operator HWND() const noexcept
	{
		return m_hWnd;
	}

	HWND Attach(HWND newWnd) noexcept
	{
		return std::exchange(m_hWnd, newWnd);
	}

private:
	HWND m_hWnd = nullptr;
};

template <typename Base = Window>
class WindowImpl : public Base
{
	static constexpr WCHAR szWindowClass[] = L"MyWindow"; // the main window class name
	HINSTANCE m_hInstance = nullptr;

public:
	WindowImpl() = default;

	WindowImpl(const WindowImpl&) = delete;
	WindowImpl& operator=(const WindowImpl&) = delete;

	void Create(HINSTANCE hInstance, const WCHAR* title)
	{
		m_hInstance = hInstance;
		RegisterWindowClassOnce(hInstance);

		HWND hWnd = CreateWindowW(szWindowClass, title, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, this);
		if (!hWnd)
		{
			throw std::runtime_error("Failed to create window");
		}
	}

private:
	static void RegisterWindowClassOnce(HINSTANCE hInstance)
	{
		struct ClassRegistrator
		{
			ClassRegistrator(HINSTANCE hInstance)
			{
				WNDCLASSEXW wcex;

				wcex.cbSize = sizeof(WNDCLASSEX);

				wcex.style = CS_HREDRAW | CS_VREDRAW;
				wcex.lpfnWndProc = WndProc;
				wcex.cbClsExtra = 0;
				wcex.cbWndExtra = 0;
				wcex.hInstance = hInstance;
				wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SAMPLE031CUSTOMWINDOWWITHCLASS));
				wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
				wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
				wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SAMPLE031CUSTOMWINDOWWITHCLASS);
				wcex.lpszClassName = szWindowClass;
				wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

				if (!RegisterClassExW(&wcex))
				{
					throw std::runtime_error("Failed to register class");
				}
			}
		};

		static ClassRegistrator registrator{ hInstance };
	}

	static LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		WindowImpl* thisPtr = nullptr;
		if (msg == WM_NCCREATE)
		{
			auto createParams = reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams;
			thisPtr = static_cast<WindowImpl*>(createParams);
			thisPtr->Attach(wnd);
			SetWindowLongPtr(wnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createParams));
		}
		if ((thisPtr = thisPtr ? thisPtr : reinterpret_cast<WindowImpl*>(GetWindowLongPtr(wnd, GWLP_USERDATA))))
		{
			return thisPtr->ThisWndProc(msg, wParam, lParam);
		}
		return ::DefWindowProc(wnd, msg, wParam, lParam);
	}

	void OnCommand(HWND hWnd, int id, HWND /*hwndCtl*/, UINT /*codeNotify*/)
	{
		switch (id)
		{
		case IDM_ABOUT:
			DialogBox(m_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		}
	}

	void OnPaint(HWND hWnd)
	{
		PAINTSTRUCT ps;
		[[maybe_unused]] HDC hdc = BeginPaint(hWnd, &ps);
		auto releaseDC = util::Finally([hWnd, &ps] {
			EndPaint(hWnd, &ps);
		});
	}

	void OnDestroy(HWND)
	{
		PostQuitMessage(0);
	}

	LRESULT ThisWndProc(UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		switch (msg)
		{
			HANDLE_MSG(*this, WM_COMMAND, OnCommand);
			HANDLE_MSG(*this, WM_PAINT, OnPaint);
			HANDLE_MSG(*this, WM_DESTROY, OnDestroy);
		}

		return ::DefWindowProc(*this, msg, wParam, lParam);
	}
};

// Forward declarations of functions included in this code module:
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	WindowImpl wnd;
	wnd.Create(hInstance, L"Window title");

	ShowWindow(wnd, nCmdShow);
	UpdateWindow(wnd);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SAMPLE031CUSTOMWINDOWWITHCLASS));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}
