#pragma once

class CFrameBuffer;
class CRenderContext;
class CScene;

/*
	Класс CRenderer, выполняющий визуализацию сцены в буфере кадра.
	Работа по построению изображения в буфере кадра выполняется в
	отдельном потоке, дабы не замедлять работу основного приложения
*/
class CRenderer
{
public:
	CRenderer(void);
	~CRenderer(void);

	// Выполняется ли в данный момент построение изображения в буфере кадра?
	bool IsRendering()const;

	/*
		Сообщает о прогрессе выполнения работы:
			renderedChunks - количество обработанных блоков изображения
			totalChunks - общее количество блоков изображения
		Возвращаемое значение:
			true - изображение построено полностью
			false - изображение построено не полностью
	*/
	bool GetProgress(unsigned & renderedChunks, unsigned & totalChunks)const;

	/*
		Запускает фоновый поток для визуализации сцены в заданном буфере кадра
		Возвращает true, если поток был запущен и false, если поток запущен не был,
		т.к. не завершилась текущая операция по построению изображения в буфере кадра
	*/
	bool Render(CScene const& scene, CRenderContext const & context, CFrameBuffer & frameBuffer);

	/*
		Выполняет принудительную остановку фонового построения изображения.
		Данный метод следует вызывать до вызова деструкторов объектов, используемых классом CRenderer,
		если на момент их вызова выполняется построение изображения в буфере кадра
	*/
	void Stop();
private:
	/*
		Визуализация кадра, выполняемая в фоновом потоке
	*/
	void RenderFrame(CScene const& scene, CRenderContext const & context, CFrameBuffer & frameBuffer);

	// Устанавливаем потокобезопасным образом флаг о том, что идет построение изображения
	// Возвращаем true, если значение флага изменилось, и false, если нет
	bool SetRendering(bool rendering);

	// Устанавливаем флаг, сообщающий рабочему потоку о необходимости остановки
	// Возвращаем true, если значение флага изменилось, и false, если нет
	bool SetStopping(bool stopping);

	// Установлен ли флаг, сообщающий о необходимости завершения работы
	bool IsStopping()const;
private:
	// Поток, в котором выполняется построение изображения
	boost::thread m_thread;

	// Мьютекс для обеспечения доступа к переменным m_totalChunks и m_renderedChunks
	mutable boost::mutex m_mutex;

	// Идет ли в данный момент построение изображения?
	volatile boost::uint32_t m_rendering;

	// Сигнал рабочему потоку о необходимости остановить работу
	volatile boost::uint32_t m_stopping;

	// Общее количество блоков изображения (для вычисления прогресса)
	volatile boost::uint32_t m_totalChunks;

	// Количество обработанных блоков изображения (для вычисления прогресса)
	volatile boost::uint32_t m_renderedChunks;
};
