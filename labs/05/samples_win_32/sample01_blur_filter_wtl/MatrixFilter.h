#pragma once

class CMatrixFilter
{
public:
	// Применяем фильтр к исходному изображению и возвращаем результат
	std::auto_ptr<Gdiplus::Bitmap> ApplyFilter(
		Gdiplus::Bitmap & srcBitmap);

protected:
	enum
	{
		// максимальный размер матрицы фильтра 
		// (больше делать смысла нет - производительность будет никакая)
		MAX_MATRIX_SIZE = 50,
	};

	// Защищаем конструктор и деструктор матрицы, 
	// делая их доступными только для классов-наследников
	CMatrixFilter(void);
	virtual ~CMatrixFilter(void);

	// Изменяет размер матрицы,
	// а также опционально выполняет инициализацию ее элементов
	void InitializeMatrix(
		size_t width, 
		size_t height, 
		const float * initialValues = NULL, 
		float scale = 1.0f);

	// Методы доступа к коэффициентам матрицы свертки
	void SetMatrixValue(size_t row, size_t column, float value);
	float GetMatrixValue(size_t row, size_t column)const;

	// Даем возможность классам-наследникам осуществить
	// выбор предпочтительного формата пикселей для выходного изображения
	virtual Gdiplus::PixelFormat ChoosePreferableOutputPixelFormat(
		Gdiplus::Bitmap & bitmapData)const;

	// Даем возможность классам-наследникам осуществить
	// обработку цвета пикселя после применения к нему матричного фильтра
	virtual void AdjustColor(float & r, float & g, float & b, float & a, Gdiplus::Color const& srcColor)const;
private:
	// приводим цвет к диапазону от 0 до 255
	inline static BYTE Clamp(float color)
	{
		return static_cast<BYTE>(
			(color < 0) ? 0 : 
			(color > 255) ? 255 :
			color
			);
	}
private:
	// коэффициенты матрицы фильтра
	std::vector<float>	m_matrix;
	// размеры матрицы фильтра
	size_t	m_matrixWidth;
	size_t	m_matrixHeight;
};
