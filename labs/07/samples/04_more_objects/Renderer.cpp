#include "stdafx.h"
#include "Renderer.h"
#include "FrameBuffer.h"
#include "FrameBufferColorBuffer.h"
#include "RenderContext.h"

using std::mutex;

CRenderer::~CRenderer(void)
{
	// Останавливаем работу фонового потока, если он еще не закончился
	Stop();
}

// Выполняется ли в данный момент построение изображения в буфере кадра?
bool CRenderer::IsRendering() const
{
	// Считываем потокобезопасным образом значение переменной m_rendering
	return m_rendering != 0;
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
void CRenderer::RenderFrame(CScene const& scene, CRenderContext const& context, CFrameBuffer& frameBuffer)
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

	CFrameBufferColorBuffer colorBuffer(frameBuffer);

	// Количество строк буфера кадра, отрсовываемых за один вызов RenderRectangle
	const int numScanLinesPerRenderCycle = 10;

	// Пробегаем все строки буфера кадра
	// При включенной поддержке OpenMP итерации цикла по строкам изображения
	// будут выполняться в параллельных потоках
#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic)
#endif
	for (int y = 0; y < height; y += numScanLinesPerRenderCycle)
	{
		IColorBuffer& buffer = colorBuffer;
		// Визуализируем методом трассировки лучей сразу несколько строк буфера кадра
		int numScanlines = numScanLinesPerRenderCycle;
		if (y + numScanlines > height)
		{
			numScanlines = height - y;
		}

		// Цикл по строкам выполняется только, если поступил запрос от пользователя
		// об остановке построения изображения
		// Инструкцию break для выхода из цикла здесь использовать нельзя (ограничение OpenMP)
		if (!IsStopping())
		{
			context.RenderRectangle(scene, 0, y, width, numScanlines, buffer);

			// Увеличиваем потокобезопасным образом количество обработанных блоков (строк) изображения
			while (numScanlines-- != 0)
			{
				++m_renderedChunks;
			}
		}
	}

	// Сбрасываем флаг остановки
	SetStopping(false);
	// Сообщаем об окончании построения изображения
	SetRendering(false);
}

// Запускает визуализацию сцены в буфере кадра в фоновом потоке
// Возвращает false, если еще не была завершена работа ранее запущенного потока
bool CRenderer::Render(CScene const& scene, CRenderContext const& context, CFrameBuffer& frameBuffer)
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
		std::ref(scene), // Ссылка на scene
		std::ref(context), // Ссылка на context
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
