// sample03_1_custom_window_with_class.cpp : Defines the entry point for the application.
//

#include "sample03_1_custom_window_with_class.h"
#include "Finally.h"
#include "framework.h"
#include <cassert>
#include <random>
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

class Brush
{
public:
	Brush() = default;
	explicit Brush(COLORREF color)
		: m_handle(CreateSolidBrush(color))
	{
		if (!m_handle)
		{
			throw std::runtime_error("Failed to create solid brush");
		}
	}

	Brush(const Brush&) = delete;
	Brush& operator=(const Brush&) = delete;

	Brush(Brush&& other)
		: m_handle(std::exchange(other.m_handle, nullptr))
	{
	}

	Brush& operator=(Brush&& rhs) noexcept
	{
		assert(this != &rhs);
		if (m_handle)
		{
			DeleteObject(m_handle);
		}
		m_handle = std::exchange(rhs.m_handle, nullptr);
		return *this;
	}

	~Brush()
	{
		if (m_handle)
		{
			DeleteObject(m_handle);
		}
	}

	void Delete() noexcept
	{
		assert(m_handle);
		DeleteObject(m_handle);
		m_handle = nullptr;
	}

	operator HBRUSH() const noexcept
	{
		return m_handle;
	}

	explicit operator bool() const noexcept
	{
		return m_handle != nullptr;
	}

private:
	HBRUSH m_handle = nullptr;
};

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
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance,
			/* lpParam= */ this);
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
				wcex.lpfnWndProc = WndProcThunk;
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

	// Переходник оконной процедуры. Он должен определить по дескриптору окна
	// адрес связанного с окном C++-класса и передать управление оконной процедуре класса
	static LRESULT CALLBACK WndProcThunk(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		WindowImpl* thisPtr = nullptr;
		if (msg == WM_NCCREATE)
		{
			// Извлекаем адрес экземпляра окна из поля lpCreateParams
			auto createParams = reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams;
			thisPtr = static_cast<WindowImpl*>(createParams);
			thisPtr->Attach(wnd);
			// Помещаем адрес экземпляра окна в поле USERDATA
			SetWindowLongPtr(wnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createParams));
		}
		if ((thisPtr = thisPtr
					? thisPtr
					: reinterpret_cast<WindowImpl*>(GetWindowLongPtr(wnd, GWLP_USERDATA))))
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
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT rc;
		GetClientRect(hWnd, &rc);

		auto releaseDC = util::Finally([hWnd, &ps] {
			EndPaint(hWnd, &ps);
		});

		auto restoreBrush = util::Finally([this, hdc, oldBrush = SelectObject(hdc, m_brush)] {
			SelectObject(hdc, oldBrush);
		});

		Ellipse(hdc, 0, 0, rc.right, rc.bottom);
	}

	void OnDestroy(HWND)
	{
		PostQuitMessage(0);
	}

	BOOL OnCreate(HWND /*hwnd*/, LPCREATESTRUCT /*lpCreateStruct*/)
	{
		UpdateBrush();
		return TRUE;
	}

	void OnLButtonUp(HWND hwnd, int /*x*/, int /*y*/, UINT /*keyFlags*/)
	{
		UpdateBrush();
		InvalidateRect(hwnd, /* rect= */ nullptr, /* erase= */ TRUE);
	}

	LRESULT ThisWndProc(UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		switch (msg)
		{
			HANDLE_MSG(*this, WM_PAINT, OnPaint);
			HANDLE_MSG(*this, WM_COMMAND, OnCommand);
			HANDLE_MSG(*this, WM_DESTROY, OnDestroy);
			HANDLE_MSG(*this, WM_CREATE, OnCreate);
			HANDLE_MSG(*this, WM_LBUTTONUP, OnLButtonUp);
		}

		return ::DefWindowProc(*this, msg, wParam, lParam);
	}

	void UpdateBrush()
	{
		std::uniform_int_distribution<unsigned> dist(0, 255);
		m_brush = Brush(RGB(dist(m_gen), dist(m_gen), dist(m_gen)));
	}

	static inline std::mt19937 m_gen;
	Brush m_brush;
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
