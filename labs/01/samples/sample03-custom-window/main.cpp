#include <tchar.h>
#include <windows.h>

const TCHAR CLASS_NAME[] = L"MainWndClass";
const TCHAR WINDOW_TITLE[] = L"My first window";

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
	// –егистрируем класс главного окна
	if (!RegisterWndClass(hInstance))
	{
		return 1;
	}

	// —оздаем главное окно приложени€
	HWND hMainWindow = CreateMainWindow(hInstance);
	if (hMainWindow == NULL)
	{
		return 1;
	}

	// ѕоказываем главное окно приложени€
	ShowWindow(hMainWindow, nCmdShow);
	UpdateWindow(hMainWindow);

	// «апускаем цикл выборки сообщений, пока не получим
	// сигнал о завершении приложени€
	return MainLoop();
}

HWND CreateMainWindow(HINSTANCE hInstance)
{
	HWND hMainWindow = CreateWindowEx(
		0, // DWORD dwExStyle;
		CLASS_NAME,
		WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr,
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
			// произошла ошибка - нужно обработать ее и, веро€тно,
			// завершить работу приложени€
			break;
		}
		else
		{
			// ≈сли это сообщение о нажатии виртуальной клавиши,
			// то добавл€ем в очередь сообщений сообщени€, несущие информацию о
			// коде вводимого пользователем символа
			TranslateMessage(&msg);
			// передаем сообщение в соответствующую оконную процедуру
			DispatchMessage(&msg);
		}
	}

	// сюда мы попадем только в том случае извлечени€ сообщени€ WM_QUIT
	// msg.wParam содержит код возврата, помещенный при помощи функции PostQuitMessage()
	return msg.wParam;
}

LRESULT CALLBACK WindowProc(
	HWND hwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		// ѕомещаем сообщение WM_QUIT в очередь сообщений текущего потока
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
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
		reinterpret_cast<HBRUSH>(COLOR_WINDOW), //HBRUSH hbrBackground;
		NULL, //LPCTSTR lpszMenuName;
		CLASS_NAME, //LPCTSTR lpszClassName;
		NULL, //HICON hIconSm;
	};

	return RegisterClassEx(&wndClass) != FALSE;
}
