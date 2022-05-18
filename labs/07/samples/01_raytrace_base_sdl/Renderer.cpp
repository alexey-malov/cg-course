#include "stdafx.h"
#include "Renderer.h"
#include "FrameBuffer.h"

using namespace boost::interprocess::ipcdetail;
using boost::mutex;

CRenderer::CRenderer(void)
:m_rendering(0)			// Изначальное состояние: "не рисуем"
,m_stopping(0)			// Не останавливаемся
,m_totalChunks(0)		// Общее количество обрабатываемых блоков изображения
,m_renderedChunks(0)	// Количество обработанных блоков изображения
{
}

CRenderer::~CRenderer(void)
{
	// Останавливаем работу фонового потока, если он еще не закончился
	Stop();
}

// Выполняется ли в данный момент построение изображения в буфере кадра?
bool CRenderer::IsRendering()const
{
	// Считываем потокобезопасным образом значение переменной m_rendering
	return atomic_read32(const_cast<volatile boost::uint32_t*>(&m_rendering)) != 0;
}

// Установлен ли флаг, сообщающий о необходимости завершения работы
bool CRenderer::IsStopping()const
{
	// Считываем потокобезопасным образом значение переменной m_stopping
	return atomic_read32(const_cast<volatile boost::uint32_t *>(&m_stopping)) != 0;
}

bool CRenderer::SetRendering(bool rendering)
{
	/*
	Атомарное (в рамках нескольких потоков) выполнение следующщей 
	последовательности операций:
		wasRendering = m_rendering != 0;
		if (m_rendering == !rendering)
		{
			m_rendering = rendering;
		}
	*/
	bool wasRendering = atomic_cas32(&m_rendering, rendering, !rendering) != 0;

	// Сообщаем, произошло ли изменение значения флага
	return wasRendering != rendering;
}

bool CRenderer::SetStopping(bool stopping)
{
	/*
	Атомарное (в рамках нескольких потоков) выполнение следующщей 
	последовательности операций:
		wasStopping = wasStopping != 0;
		if (m_stopping == !stopping)
		{
			m_stopping = stopping;
		}
	*/
	bool wasStopping = atomic_cas32(&m_stopping, stopping, !stopping) != 0;

	// Сообщаем, произошло ли изменение значения флага
	return wasStopping != stopping;
}

bool CRenderer::GetProgress(unsigned & renderedChunks, unsigned & totalChunks)const
{
	// Захватываем мьютекс на время работы данного метода
	mutex::scoped_lock lock(m_mutex);

	// Получаем потокобезопасным образом значения переменных
	// m_renderedChunks и m_totalChunks
	renderedChunks = atomic_read32(const_cast<volatile boost::uint32_t*>(&m_renderedChunks));
	totalChunks = atomic_read32(const_cast<volatile boost::uint32_t*>(&m_totalChunks));;

	// Сообщаем, все ли блоки изображения были обработаны
	return (totalChunks > 0) && (renderedChunks == totalChunks);
}

/*
Выполняет основную работу по построению изображения в буфере кадра
*/
void CRenderer::RenderFrame(CFrameBuffer & frameBuffer)
{
	// Запоминаем ширину и высоту буфера кадра, чтобы каждый раз не вызывать
	// методы класса CFrameBuffer
	const int width = frameBuffer.GetWidth();
	const int height = frameBuffer.GetHeight();

	/*
	Задаем общее количество блоков изображения 
	Под блоком изображения здесь понимается 1 строка буфера кадра
	*/
	atomic_write32(&m_totalChunks, height);

	// Пробегаем все строки буфера кадра
	// При включенной поддержке OpenMP итерации цикла по строкам изображения
	// будут выполняться в параллельных потоках
#ifdef _OPENMP
	#pragma omp parallel for schedule(dynamic)
#endif
	for (int y = 0; y < height; ++y)
	{
		// Адрес y-й строки изображения
		boost::uint32_t * rowPixels = frameBuffer.GetPixels(y);

		// Цикл по x выполняется только, если не было запроса
		// от пользователя об остановке построения изображения
		// Инструкцию break для выхода из цикла здесь использовать
		// нельзя (ограничение OpenMP)
		if (!IsStopping())
		{
			// Пробегаем все пиксели в строке
			for (int x = 0; x < width; ++x)
			{
				// Вычисляем цвет текущего пикселя и записываем его в буфер кадра
				rowPixels[size_t(x)] = CalculatePixelColor(x, y, width, height);
			}

			// Увеличиваем потокобезопасным образом количество обработанных блоков (строк) изображения
			atomic_inc32(&m_renderedChunks);	//-V1206
		}
	}

	// Сбрасываем флаг остановки
	SetStopping(false);
	// Сообщаем об окончании построения изображения
	SetRendering(false);
}

// Запускает визуализацию сцены в буфере кадра в фоновом потоке
// Возвращает false, если еще не была завершена работа ранее запущенного потока
bool CRenderer::Render(CFrameBuffer & frameBuffer)
{
	// Пытаемся перейти в режим рендеринга
	if (!SetRendering(true))
	{
		// В данный момент еще идет построение изображения в параллельном потоке
		return false;
	}

	// Блокируем доступ к общим (для фонового и основного потока) данным класса
	// вплоть до завершения работа метода Render
	mutex::scoped_lock lock(m_mutex);

	// Очищаем буфер кадра
	frameBuffer.Clear();

	// Сбрасываем количество обработанных и общее количество блоков изображения
	// сигнализируя о том, что еще ничего не сделано	
	atomic_write32(&m_totalChunks, 0);
	atomic_write32(&m_renderedChunks, 0);

	// Сбрасываем запрос на остановку построения изображения
	if (SetStopping(false))
	{
		// Если еще до запуска рабочего потока пришел запрос на остановку,
		// выходим, сбрасывая флаг "идет построение изображения"
		SetRendering(false);
		return false;
	}

	// Запускаем метод RenderFrame в параллельном потоке, передавая ему
	// ссылку на объект frameBuffer
	m_thread = boost::thread(&CRenderer::RenderFrame, this, boost::ref(frameBuffer));

	// Выходим, сообщая о том, что процесс построения изображения запущен
	return true;
}

void CRenderer::Stop()
{
	// Если происходит построение изображения
	if (IsRendering())
	{
		// Сообщаем потоку, выполняющему построение изображения, о необходимости
		// завершить работу
		SetStopping(true);

		// Дожидаемся окончания работы рабочего потока
		m_thread.join();

		// Сбрасываем флаг остановки, если поток завершил свою работу до вызова SetStopping(true)
		SetStopping(false);
	}
}

// Вычисляем цвет пикселя буфера кадра
boost::uint32_t CRenderer::CalculatePixelColor(int x, int y, unsigned frameWidth, unsigned frameHeight)const
{
	// Пока здесь у нас заглушка - вычисляется цвет точки фрактала Мандельброта
	// (см. Википедию)

	// В дальнейшем этот метод будет отправной точкой для прослеживания
	// пути луча, проходящего через данный пиксель, вглубь сцены

	double x0 = 2.0 * x / frameWidth - 1.5;
	double y0 = 2.0 * y / frameHeight - 1.0;

	// Оптимизация: проверяем, не попадает ли текущая точка в область главной кардиоиды
	double rho = sqrt((x0 - 0.25) * (x0 - 0.25) + y0 * y0);
	double theta = atan2(y0, x0 - 0.25);
	double rhoC = 0.5 - 0.5 * cos(theta);
	if (rho <= rhoC)
	{
		return 0x000000;
	}

	double re = 0, im = 0;

	// Подсчет количества итераций "улетания точки Zi в бесконечность"
	int iterCount = 10000;
	// Итеративное вычисление последовательности Z(i+1) = (Z(i))^2 + C, где
	//	Z(0) = 0 + 0*i
	//	C = x0 + y0*i
	while ((iterCount > 0) && (re * re + im * im < 1e18))
	{
		double re1 = re * re - im * im + x0;
		im = 2 * re * im + y0;
		re = re1;
		--iterCount;
	}

	// Формирование цвета на основе количества итераций
	boost::uint8_t r = static_cast<boost::uint8_t>((iterCount / 3) & 0xff);
	boost::uint8_t g = static_cast<boost::uint8_t>(iterCount & 0xff);
	boost::uint8_t b = static_cast<boost::uint8_t>((iterCount / 2) & 0xff);
	boost::uint8_t a = 0xff;
	return (a << 24) | (r << 16) | (g << 8) | b;
}
