#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#pragma warning(push)
#pragma warning(disable : 4458)
#include <gdiplus.h>
#pragma warning(pop)
#include <memory>
#include <stdexcept>

TCHAR const CLASS_NAME[] = _T("MainWndClass");
TCHAR const WINDOW_TITLE[] = _T("Drawing raster images");

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int MainLoop();
bool RegisterWndClass(HINSTANCE hInstance);
HWND CreateMainWindow(HINSTANCE hInstance);

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

Gdiplus::Bitmap * g_pBitmap = NULL;

int WINAPI WinMain(
				   HINSTANCE hInstance,
				   HINSTANCE /*hPrevInstance*/,
				   LPSTR /*lpCmdLine*/,
				   int nCmdShow)
{
	// Выполняем инициализацию GDI+
	try
	{
		CGdiplusInitializer initializer;

		// Создаем объект Image в динамической памяти и сохраняем 
		// указатель на него в умном указателе std::auto_ptr
		std::auto_ptr<Gdiplus::Image> pImage(new Gdiplus::Image(L"car.jpg"));

		// Создаем растровое изображение соответствующего размера 
		// с форматом пикселей RGBA 32 bit
		Gdiplus::Bitmap bmp(pImage->GetWidth(), pImage->GetHeight(), PixelFormat32bppARGB);
		
		// Создаем объект Graphics дли рисования в растре bmp
		Gdiplus::Graphics g(&bmp);

		// Рисуем исходное изображение внутрь созданного нами растра
		g.DrawImage(pImage.get(), 0, 0);

		// удаляем объект Image
		pImage.release();

		// Сохраняем указатель на раст в глобальной переменной
		// для функции OnPaint
		g_pBitmap = &bmp;

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
	catch (std::runtime_error &)
	{
		return 2;
	}
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

void OnPaint(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC dc = BeginPaint(hwnd, &ps);

	// Создаем объект Graphics
	Gdiplus::Graphics g(dc);

	// Рисуем изображение в Graphics
	g.DrawImage(g_pBitmap, 20, 20);

	EndPaint(hwnd, &ps);
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
	WNDCLASSEX wndClass =
	{
		sizeof(wndClass),	//UINT cbSize;
		CS_HREDRAW | CS_VREDRAW, //UINT style;
		&WindowProc,		//WNDPROC lpfnWndProc;
		0,					//int cbClsExtra;
		0,					//int cbWndExtra;
		hInstance,			//HINSTANCE hInstance;
		NULL,				//HICON hIcon;
		LoadCursor(NULL, IDC_ARROW), //HCURSOR hCursor;
		(HBRUSH)(COLOR_BTNFACE + 1), //HBRUSH hbrBackground;
		NULL,				//LPCTSTR lpszMenuName;
		CLASS_NAME,			//LPCTSTR lpszClassName;
		NULL,				//HICON hIconSm;
	};

	return RegisterClassEx(&wndClass) != FALSE;
}

