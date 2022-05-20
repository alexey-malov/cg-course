#include "stdafx.h"
#include "Renderer.h"
#include "FrameBuffer.h"

using std::mutex;

CRenderer::~CRenderer(void)
{
	// Останавливаем работу фонового потока, если он еще не закончился
	Stop();
}

// Выполняется ли в данный момент построение изображения в буфере кадра?
bool CRenderer::IsRendering() const
{
	return m_rendering;
}

// Установлен ли флаг, сообщающий о необходимости завершения работы
bool CRenderer::IsStopping() const
{
	// Считываем потокобезопасным образом значение переменной m_stopping
	return m_stopping;
}

bool CRenderer::SetRendering(bool rendering)
{
	bool expected = !rendering;
	return m_rendering.compare_exchange_strong(expected, rendering);
}

bool CRenderer::SetStopping(bool stopping)
{
	bool expected = !stopping;
	return m_stopping.compare_exchange_strong(expected, stopping);
}

bool CRenderer::GetProgress(unsigned& renderedChunks, unsigned& totalChunks) const
{
	// Захватываем мьютекс на время работы данного метода
	std::lock_guard lock(m_mutex);

	// Получаем потокобезопасным образом значения переменных
	// m_renderedChunks и m_totalChunks
	renderedChunks = m_renderedChunks;
	totalChunks = m_totalChunks;

	// Сообщаем, все ли блоки изображения были обработаны
	return (totalChunks > 0) && (renderedChunks == totalChunks);
}

/*
Выполняет основную работу по построению изображения в буфере кадра
*/
void CRenderer::RenderFrame(CFrameBuffer& frameBuffer)
{
	// Запоминаем ширину и высоту буфера кадра, чтобы каждый раз не вызывать
	// методы класса CFrameBuffer
	const int width = frameBuffer.GetWidth();
	const int height = frameBuffer.GetHeight();

	/*
	Задаем общее количество блоков изображения 
	Под блоком изображения здесь понимается 1 строка буфера кадра
	*/
	m_totalChunks = height;

	// Пробегаем все строки буфера кадра
	// При включенной поддержке OpenMP итерации цикла по строкам изображения
	// будут выполняться в параллельных потоках
#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic)
#endif
	for (int y = 0; y < height; ++y)
	{
		std::uint32_t * rowPixels = nullptr;

		// Синхронизируем доступ к frameBuffer из вспомогательных потоков
#ifdef _OPENMP
		#pragma omp critical
#endif
		{
			// Получаем адрес начала y-й строки в буфере кадра
			rowPixels = frameBuffer.GetPixels(y);
		}

		// Цикл по строкам выполняется только, если поступил запрос от пользователя
		// об остановке построения изображения
		// Инструкцию break для выхода из цикла здесь использовать нельзя (ограничение OpenMP)
		if (!IsStopping())
		{
			// Пробегаем все пиксели в строке
			for (int x = 0; x < width; ++x)
			{
				// Вычисляем цвет текущего пикселя и записываем его в буфер кадра
				rowPixels[size_t(x)] = CalculatePixelColor(x, y, width, height);
			}

			++m_renderedChunks;
		}
	}

	// Сбрасываем флаг остановки
	SetStopping(false);
	// Сообщаем об окончании построения изображения
	SetRendering(false);
}

// Запускает визуализацию сцены в буфере кадра в фоновом потоке
// Возвращает false, если еще не была завершена работа ранее запущенного потока
bool CRenderer::Render(CFrameBuffer& frameBuffer)
{
	// Пытаемся перейти в режим рендеринга
	if (!SetRendering(true))
	{
		// В данный момент еще идет построение изображения в параллельном потоке
		return false;
	}

	// Блокируем доступ к общим (для фонового и основного потока) данным класса
	// вплоть до завершения работа метода Render
	std::lock_guard lock(m_mutex);

	// Очищаем буфер кадра
	frameBuffer.Clear();

	// Сбрасываем количество обработанных и общее количество блоков изображения
	// сигнализируя о том, что еще ничего не сделано
	m_totalChunks = 0;
	m_renderedChunks = 0;

	// Сбрасываем запрос на остановку построения изображения
	if (SetStopping(false))
	{
		// Если еще до запуска рабочего потока пришел запрос на остановку,
		// выходим, сбрасывая флаг "идет построение изображения"
		SetRendering(false);
		return false;
	}

	// Запускаем метод RenderFrame в параллельном потоке, передавая ему
	// необходимый набор параметров
	m_thread = std::thread(
		&CRenderer::RenderFrame, // Адрес метода RenderFrame
		this, // Указатель this
		std::ref(frameBuffer)); // Ссылка на frameBuffer

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
uint32_t CRenderer::CalculatePixelColor(int x, int y, unsigned frameWidth, unsigned frameHeight)const
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
	uint8_t r = static_cast<uint8_t>((iterCount / 3) & 0xff);
	uint8_t g = static_cast<uint8_t>(iterCount & 0xff);
	uint8_t b = static_cast<uint8_t>((iterCount / 2) & 0xff);
	uint8_t a = 0xff;
	return (a << 24) | (r << 16) | (g << 8) | b;
}
