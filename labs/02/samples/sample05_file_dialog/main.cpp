#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#pragma warning(push)
#pragma warning(disable : 4458)
#include <gdiplus.h>
#pragma warning(pop)
#include <memory>
#include <stdexcept>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include "resource.h"

TCHAR const CLASS_NAME[] = _T("MainWndClass");
TCHAR const WINDOW_TITLE[] = _T("File dialog example");

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int MainLoop(HWND hMainWindow);
bool RegisterWndClass(HINSTANCE hInstance);
HWND CreateMainWindow(HINSTANCE hInstance);

HINSTANCE g_hInstance = NULL;

using namespace Gdiplus;
using namespace std;

auto_ptr<Bitmap> g_pBitmap;

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


int WINAPI WinMain(
				   HINSTANCE hInstance,
				   HINSTANCE /*hPrevInstance*/,
				   LPSTR /*lpCmdLine*/,
				   int nCmdShow)
{
	g_hInstance = hInstance;
	// Выполняем инициализацию GDI+
	try
	{
		CGdiplusInitializer initializer;

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
		int result = MainLoop(hMainWindow);

		g_pBitmap.release();

		return result;
	}
	catch (std::runtime_error &)
	{
		return 2;
	}
}

HWND CreateMainWindow(HINSTANCE hInstance)
{
	HMENU hMainMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MAIN_MENU));

	HWND hMainWindow = CreateWindowEx(
		0,
		CLASS_NAME,
		WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		hMainMenu,
		hInstance,
		NULL);

	return hMainWindow;
}

int MainLoop(HWND hMainWindow)
{

	HACCEL accel = LoadAccelerators(g_hInstance, MAKEINTRESOURCE(IDR_MAIN_MENU));

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
			// Пытаемся обработать сообщение как сообщение от нажатия клавиш
			// быстрого доступа
			if (!TranslateAccelerator(hMainWindow, accel, &msg))
			{
				// Это не сообщение о нажатии клавиш быстрого доступа
				// обрабатываем сообщение стандартным образом

				// Если это сообщение о нажатии виртуальной клавиши,
				// то добавляем в очередь сообщений сообщения, несущие информацию о
				// коде вводимого пользователем символа
				TranslateMessage(&msg);
				// передаем сообщение в соответствующую оконную процедуру
				DispatchMessage(&msg);
			}
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

	if (g_pBitmap.get() != NULL)
	{
		Graphics g(dc);
		g.DrawImage(g_pBitmap.get(), 0, 0);
	}

	EndPaint(hwnd, &ps);
}

void InitFileNameStructure(HWND hwndOwner, OPENFILENAME * pOpenFileName, TCHAR * pFileName, DWORD maxFileName)
{
	ZeroMemory(pOpenFileName, sizeof(OPENFILENAME));
	
	pOpenFileName->lStructSize = sizeof(OPENFILENAME);
	pOpenFileName->hwndOwner = hwndOwner;
	pOpenFileName->hInstance = g_hInstance;
	pOpenFileName->nMaxFile = maxFileName;
	pOpenFileName->lpstrFile = pFileName;
	pOpenFileName->lpstrFilter = 
		_T("Images (BMP, PNG, JPG, TIFF)\0*.bmp;*.png;*.jpg;*.tif\0")
		_T("All files\0*.*\0")
		_T("\0");

}

void OnOpenFile(HWND hwnd, UINT /*codeNotify*/)
{
	OPENFILENAME ofn;
	TCHAR fileName[MAX_PATH + 1] = _T("");
	InitFileNameStructure(hwnd, &ofn, fileName, MAX_PATH);
	
	if (GetOpenFileName(&ofn))
	{
		Image img(ofn.lpstrFile);

		if (img.GetLastStatus() == Ok)
		{

			g_pBitmap = auto_ptr<Bitmap>(new Bitmap(img.GetWidth(), img.GetHeight(), PixelFormat32bppARGB));

			Graphics g(g_pBitmap.get());
			g.DrawImage(&img, 0, 0);

			InvalidateRect(hwnd, NULL, TRUE);
		}
	}
}

/*
Возвращаем расширение файла (все, что после последней точки в имени)
Строго говоря, для ситуаций вроде c:\some_folder.ext\someFile-without-extension
будет работать неправильно, но для примера сойдет
*/
std::wstring GetFileExtension(std::wstring const& fileName)
{
	size_t dotPos = fileName.find_last_of(L'.');
	if (dotPos != std::wstring::npos)
	{
		return fileName.substr(dotPos + 1);
	}
	else
	{
		return std::wstring();
	}
}

// приводим строку к нижнему регистру
std::wstring WStringToLower(std::wstring const& str)
{
	std::wstring result(str);

	std::transform(result.begin(), result.end(), result.begin(), [](wchar_t ch) { return static_cast<wchar_t>(std::tolower(ch)); });

	return result;
}

CLSID GetEncoderCLSID(std::wstring const& fileExtension)
{
	// Приводим разрешение к виду "*.разрешение"
	std::wstring extensionMask = L"*." + WStringToLower(fileExtension) + L";";

	// Запрашиваем у GDI+ количество кодировщиков изображений
	// и размер блока данных для хранения их описания
	UINT numEncoders;
	UINT encodersSize;
	GetImageEncodersSize(&numEncoders, &encodersSize);

	// Выделяем буфер для хранения информации о кодировщиках
	std::vector<BYTE> encodersBuffer(encodersSize);

	// Запрашиваем у GDI+ информацию обо всех кодировщиков
	ImageCodecInfo* pInstalledCodecs = reinterpret_cast<ImageCodecInfo *>(&encodersBuffer[0]);
	GetImageEncoders(numEncoders, encodersSize, pInstalledCodecs);

	// ищем подходящий кодировщик изображений
	for (unsigned i = 0; i < numEncoders; ++i)
	{
		ImageCodecInfo & codec = pInstalledCodecs[i];

		// получаем расширения файлов, поддерживаемых данным кодировщиком
		// в формате: *.jpg;*.jpe;*.jpeg
		std::wstring extensions = WStringToLower(codec.FilenameExtension) + L";";

		// Если в списке расширений содержится маска расширения файла
		// то кодек считается найденным
		if (extensions.find(extensionMask) != wstring::npos)
		{
			return codec.Clsid;
		}
	}

	// не нашли подходящий кодировщик, возвращаем нулевой идентификатор
	return CLSID_NULL;
}

void SaveBitmap(Bitmap & bitmap, std::wstring const& fileName, int quality = 0)
{	
	// получаем расширение выходного файла
	std::wstring fileExtension = GetFileExtension(fileName);

	// Получаем идентификатор по расширению файла
	CLSID codecId = GetEncoderCLSID(fileExtension);

	// Если вернули CLSID_NULL (кодек не найден), то выходим
	if (IsEqualCLSID(codecId, CLSID_NULL))
	{
		return;
	}

	// заполняем параметры кодировщика
	EncoderParameters params;
	params.Count = 1;	// у нас только один параметр (степень компресии 0-100)

	// заполняем характеристики параметра качество сжатия
	EncoderParameter & param0 = params.Parameter[0];

	LONG qualityParam = quality;

	param0.Guid = EncoderCompression;	// идентификатор параметра "компрессия"
	param0.NumberOfValues = 1;			// в массиве параметров содержится одно значение
	param0.Type = EncoderParameterValueTypeLong; // тип значений LONG
	param0.Value = &qualityParam;		// адрес массива параметров

	// сохраняем изображение с использованием подобранного кодировщика
	// и параметра Quality (на практике используется только в JPEG-е)
	bitmap.Save(fileName.c_str(), &codecId, &params);
}

void OnSaveFile(HWND hwnd, UINT /*codeNotify*/)
{
	// Если изображение не открыто, то его и сохранить нельзя
	if (!g_pBitmap.get())
	{
		return;
	}

	OPENFILENAME ofn;
	TCHAR fileName[MAX_PATH + 1] = _T("");
	InitFileNameStructure(hwnd, &ofn, fileName, MAX_PATH);

	if (GetSaveFileName(&ofn))
	{
		SaveBitmap(*g_pBitmap, fileName, 75);
	}
}

void OnExit(HWND hwnd, UINT /*codeNotify*/)
{
	DestroyWindow(hwnd);
}

void OnCommand(HWND hwnd, int id, HWND /*hwndCtl*/, UINT codeNotify)
{
	switch (id)
	{
	case ID_FILE_OPEN_IMAGE:
		OnOpenFile(hwnd, codeNotify);
		break;
	case ID_FILE_SAVE_IMAGE_AS:
		OnSaveFile(hwnd, codeNotify);
		break;
	case ID_FILE_EXIT:
		OnExit(hwnd, codeNotify);
		break;
	}
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
		HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
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

