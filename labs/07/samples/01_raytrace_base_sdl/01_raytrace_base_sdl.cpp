// 01_raytrace_base_sdl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Renderer.h"
#include "FrameBuffer.h"
#include <SDL.h>

using namespace boost::interprocess::ipcdetail;

class CApplication
{
public:
	CApplication()
		:m_frameBuffer(800, 600)
		,m_pMainSurface(NULL)
		,m_timerId(NULL)
		,m_mainSurfaceUpdated(0)
	{
		// Инициализация SDL (таймер и видео)
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
		SDL_WM_SetCaption("Raytracing example", NULL);

		// Создаем главное окно приложения и сохраняем указатель
		// на поверхность, связанную с ним
		m_pMainSurface = SDL_SetVideoMode(800, 600, 32, SDL_SWSURFACE | SDL_DOUBLEBUF);	//-V112
	}

	// Запуск главного цикла приложения
	void MainLoop()
	{
		// Инициализация приложения
		Initialize();

		// Обновляем изначальное содержимое окна
		UpdateMainSurface();

		// Цикл обработки сообщений, продолжающийся пока не будет
		// получен запрос на завершение работы
		SDL_Event evt;
		while (SDL_WaitEvent(&evt) && evt.type != SDL_QUIT)
		{
			switch (evt.type)
			{
			case SDL_VIDEOEXPOSE:
				// Обновляем содержимое главного окна,
				// если оно нуждается в перерисовке
				UpdateMainSurface();
				break;
			}
		}

		// Деинициализация приложения
		Uninitialize();
	}

	~CApplication()
	{
		// Завершаем работу всех подсистем SDL
		SDL_Quit();
	}

private:

	void Initialize()
	{
		// Запускаем построение изображения и таймер обновления экрана
		m_renderer.Render(m_frameBuffer);
		m_timerId = SDL_AddTimer(500, &TimerCallback, this);
	}

	void Uninitialize()
	{
		// Останавливаем таймер обновления экрана и построение изображения
		SDL_RemoveTimer(m_timerId);
		m_renderer.Stop();
	}

	// Обновление содержимого окна приложения
	void UpdateMainSurface()
	{
		// Копируем содержимое буфера кадра в теневой буфер окна приложения
		CopyFrameBufferToSDLSurface();

		// Обмениваем теневой буфер и видимый, делая видимым текущее состояние буфера кадра
		SDL_Flip(m_pMainSurface);

		// Устанавливаем флаг: "Поверхность обновлена"
		atomic_write32(&m_mainSurfaceUpdated, 1);
	}

	// Копирование буфера кадра в область главного окна
	void CopyFrameBufferToSDLSurface()
	{
		// Получаем доступ к пикселям поверхности окна приложения
		SDL_LockSurface(m_pMainSurface);

		// В нашем приложении для простоты поддерживается только формат 32 бита на пиксель
		const SDL_PixelFormat * pixelFormat = m_pMainSurface->format;
		if (pixelFormat->BitsPerPixel == 32)
		{
			// Получаем смещения битов красной, зеленой, синей и альфа составляющих
			const Uint8 rShift = pixelFormat->Rshift;
			const Uint8 gShift = pixelFormat->Gshift;
			const Uint8 bShift = pixelFormat->Bshift;
			const Uint8 aShift = pixelFormat->Ashift;
			// Маска альфа-канала (может быть равна нулю, если поверхность не имеет альфа канала)
			const Uint32 aMask = pixelFormat->Amask;

			// Размеры буфера кадра
			const unsigned h = m_frameBuffer.GetHeight();
			const unsigned w = m_frameBuffer.GetWidth();

			// Адрес начала пиксельных данных поверхности
			Uint8 * pixels = reinterpret_cast<Uint8*>(m_pMainSurface->pixels);
			for (unsigned y = 0; 
				y < h; 
				++y, 
				pixels += m_pMainSurface->pitch)	// перемещаемся на адрес следующей строки в конце каждой итерации цикла
			{
				// Получаем адреса начала текущей строки в буфере кадра и поверхости окна
				boost::uint32_t const * srcLine = m_frameBuffer.GetPixels(y);
				Uint32 * dstLine = reinterpret_cast<Uint32*>(pixels);

				// Совпадает ли формат пикселей поверхности с форматом
				// ARGB или XRGB, используемом в буфере кадра?
				if (bShift == 0 && gShift == 8 && rShift == 16)
				{
					// Совпадает, просто копируем пиксели строки как они есть
					memcpy(dstLine, srcLine, w * sizeof(Uint32));
				}
				else
				{
					// Формат не совпадает, придется конвертировать цвет пикселя
					// из формата буфера кадра в формат пикселей поверхности
					for (unsigned x = 0; x < w; ++x)
					{
						boost::uint32_t srcColor = srcLine[x];	//-V108
						Uint32 dstColor =
							((srcColor & 0xff) << bShift) |
							(((srcColor >> 8) & 0xff) << gShift) |
							(((srcColor >> 16) & 0xff) << rShift) |
							((((srcColor >> 24) ) << aShift) & aMask);
						dstLine[x] = dstColor;	//-V108
					}
				}
			}
		}

		// Завершение прямого доступа к пикселям поверхности
		SDL_UnlockSurface(m_pMainSurface);
	}

	// Обработчик таймера, вызываемый SDL
	static Uint32 SDLCALL TimerCallback(Uint32 interval, void *param)
	{
		// Перенаправляем вызов таймера в метод класса OnTimer
		CApplication * pMyApp = reinterpret_cast<CApplication*>(param);
		return pMyApp->OnTimer(interval);
	}

	// Обработчик таймера SDL объекта CApplication
	Uint32 OnTimer(Uint32 interval)
	{
		// Проверяем, завершено ли построение изображения
		unsigned renderedChunks = 0;
		unsigned totalChunks = 0;
		if (m_renderer.GetProgress(renderedChunks, totalChunks))
		{
			// Изображение построено, обработчик вернет значение 0 
			// для остановки таймера
			interval = 0;
		}

		// В любом случае помечаем окно как требующее обновления
		InvalidateMainSurface();

		// Выходим из обработчика, сообщая SDL о необходимости повторить
		// вызов данного обработчика через указанное количество миллисекунд
		// Возврат нулевого значения из обработчика приведет к отключению
		// данного таймера
		return interval;
	}

	// Пометка содержимого окна, как нуждающейся в перерисовке
	void InvalidateMainSurface()
	{
		// Окно нуждается в перерисовке, если оно было обновлено
		bool redrawIsNeeded = atomic_read32(&m_mainSurfaceUpdated) == 1;

		if (redrawIsNeeded)
		{
			// Сбрасываем флаг "Поверхность обновлена"
			atomic_write32(&m_mainSurfaceUpdated, 0);

			// Добавляем событие о необходимости перересовки окна
			// в очередь событий SDL
			SDL_Event evt;
			evt.type = SDL_VIDEOEXPOSE;
			SDL_PushEvent(&evt);
		}
	}
private:
	// Буфер кадра
	CFrameBuffer m_frameBuffer;
	// Визуализатор
	CRenderer m_renderer;
	// Поверхность окна приложения
	SDL_Surface * m_pMainSurface;
	// Идентификатор SDL-таймера
	SDL_TimerID m_timerId;
	// Обновлена ли поверхность окна приложения (1 - да, 0 - нет)
	boost::uint32_t m_mainSurfaceUpdated;
};


#ifdef __cplusplus
extern "C"
#endif
int main(int /*argc*/, char* /*argv*/[])
{
	CApplication app;
	app.MainLoop();
	return 0;
}

