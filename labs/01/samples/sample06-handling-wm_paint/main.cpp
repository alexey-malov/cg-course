#include <stdexcept>
#include <tchar.h>
#include <windows.h>
#include <windowsx.h>

TCHAR const CLASS_NAME[] = _T("MainWndClass");
TCHAR const WINDOW_TITLE[] = _T("My first window");

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int MainLoop();
bool RegisterWndClass(HINSTANCE hInstance);
HWND CreateMainWindow(HINSTANCE hInstance);

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE /*hPrevInstance*/,
	LPSTR /*lpCmdLine*/,
	int nCmdShow)
{
	// Регистрируем класс главного окна
	if (!RegisterWndClass(hInstance))
	{
		return 1;
	}

	// Создаем главное окно приложения
	HWND hMainWindow = CreateMainWindow(hInstance);
	if (hMainWindow == NULL)
	{
		return 1;
	}

	// Показываем главное окно приложения
	ShowWindow(hMainWindow, nCmdShow);
	UpdateWindow(hMainWindow);

	// Запускаем цикл выборки сообщений, пока не получим
	// сигнал о завершении приложения
	return MainLoop();
}

HWND CreateMainWindow(HINSTANCE hInstance)
{
	HWND hMainWindow = CreateWindowEx(
		0,
		CLASS_NAME,
		WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

	return hMainWindow;
}

int MainLoop()
{
	MSG msg;
	BOOL res;
	while ((res = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (res == -1)
		{
			// произошла ошибка - нужно обработать ее и, вероятно,
			// завершить работу приложения
		}
		else
		{
			// Если это сообщение о нажатии виртуальной клавиши,
			// то добавляем в очередь сообщений сообщения, несущие информацию о
			// коде вводимого пользователем символа
			TranslateMessage(&msg);
			// передаем сообщение в соответствующую оконную процедуру
			DispatchMessage(&msg);
		}
	}

	// сюда мы попадем только в том случае извлечения сообщения WM_QUIT
	// msg.wParam содержит код возврата, помещенный при помощи функции PostQuitMessage()
	return msg.wParam;
}

void OnDestroy(HWND /*hWnd*/)
{
	PostQuitMessage(0);
}

// Инкапсулирует вызов BeginPaint/EndPaint
class PaintDC
{
public:
	PaintDC(HWND hWnd)
		: m_hWnd(m_hWnd)
		, m_dc(BeginPaint(hWnd, &m_ps))
	{
		if (!m_dc)
		{
			throw std::runtime_error("BeginPaint failed");
		}
	}

	PaintDC(const PaintDC&) = delete;
	PaintDC& operator=(const PaintDC&) = delete;

	operator HDC() const noexcept
	{
		return m_dc;
	}

	~PaintDC()
	{
		EndPaint(m_hWnd, &m_ps);
	}

private:
	HWND m_hWnd;
	PAINTSTRUCT m_ps;
	HDC m_dc;
};

void OnPaint(HWND hwnd)
{
	PaintDC dc(hwnd);

	// создаем красное перо
	HPEN pen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));

	// создаем желтую кисть
	LOGBRUSH brushInfo;
	brushInfo.lbStyle = BS_SOLID;
	brushInfo.lbColor = RGB(255, 255, 0);
	brushInfo.lbHatch = 0;
	HBRUSH brush = CreateBrushIndirect(&brushInfo);

	// выбираем перо и кисть в контексте устройства, запоминая ранее выбранные объекты
	HPEN oldPen = SelectPen(dc, pen);
	HBRUSH oldBrush = SelectBrush(dc, brush);

	Ellipse(dc, // дескриптор контекста устройства
		100, 50, // x и y верхнего левого угла ограничивающего прямоугольника
		250, 150 // x и y нижнего правого угла ограничивающего прямоугольника
	);

	// выбираем ранее сохраненные объекты GDI в контексте устройства
	// при этом pen и brush автоматически становятся в нем не выбраны
	SelectPen(dc, oldPen);
	SelectBrush(dc, oldBrush);

	// объекты GDI можно удалять если они не выбраны ни в одном из контекстов
	DeletePen(pen);
	DeleteBrush(brush);
}

LRESULT CALLBACK WindowProc(
	HWND hwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (uMsg)
	{
		HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool RegisterWndClass(HINSTANCE hInstance)
{
	WNDCLASSEX wndClass = {
		sizeof(wndClass), //UINT cbSize;
		CS_HREDRAW | CS_VREDRAW, //UINT style;
		&WindowProc, //WNDPROC lpfnWndProc;
		0, //int cbClsExtra;
		0, //int cbWndExtra;
		hInstance, //HINSTANCE hInstance;
		NULL, //HICON hIcon;
		LoadCursor(NULL, IDC_ARROW), //HCURSOR hCursor;
		(HBRUSH)(COLOR_BTNFACE + 1), //HBRUSH hbrBackground;
		NULL, //LPCTSTR lpszMenuName;
		CLASS_NAME, //LPCTSTR lpszClassName;
		NULL, //HICON hIconSm;
	};

	return RegisterClassEx(&wndClass) != FALSE;
}
