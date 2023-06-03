#pragma once

#include "../libgl/FrameBuffer.h"
#include "../libgl/Shaders.h"
#include "../libgl/Texture.h"

class CGaussianBlurFilter
{
public:
	// Сообщаем фильтру сведения о программе, выполняющей гауссово размытие
	void SetProgram(
		GLuint program, // программа
		GLint stepLocation, // расположение переменной step
		GLint imageLocation, // расположение переменной image
		GLint coefficientsLocation, // Расположение массива коэффициентов
		size_t filterRadius); // Радиус фильтра

	// Выполняем размытие изображения в переданной текстуре
	// и возвращаем идентификатор текстурного объекта с результатом
	// Идентификатор текстуры, возвращаемый в качестве результата,
	// хранится внутри данного класса
	GLuint Blur(
		GLuint texture, // Текстурный объект
		GLfloat sigma, // Среднеквадратическое отклонение
		int width, int height, // Размеры текстуры
		bool generateMipmaps = true) const; // Необходимость генерирования mip-уровней
private:
	// Вычисляем коэффициенты размытия фильтра
	static std::vector<float> CalcGauss(size_t size, float sigma);

	// Подготавливаем вспомогательный буфер кадра для
	// обработки изображения заданного размера
	void PrepareFrameBuffer(
		int width,
		int height,
		bool generateMipmaps = true) const;

	// Рисуем текстурированный прямоугольник
	void DrawRectangle() const;

	// Выполняем настройку матриц и видового порта
	void SetupViewportAndMatrices() const;

	// Восстанавливаем параметры видового порта и матрицы
	void RestoreViewportAndMatrices() const;

	// Активизируем программу и конфигцрируем ее uniform-переменные
	void ActivateProgram(float sigma) const;

	// Выполняем размытие по горизонтали
	void BlurHorizontal(GLuint texture) const;

	// Выполняем размытие по вертикали
	void BlurVertical() const;

	// Программный объект
	CProgramHandle m_program;

	// Расположение переменной, хранящей шаг выборки данных из текстуры
	GLint m_stepLocation = -1;

	// Расположение переменной, хранящей расположение изображения фильтра
	GLint m_imageLocation = -1;

	// Расположение массива коэффициентов фильтра
	GLint m_coefficientsLocation = -1;

	// Радиус фильтра
	size_t m_filterRadius = 0;

	// Флаг, задающий необходимость генерирования mip-уровней текстуры
	mutable bool m_generateMipmaps = false;

	// Вспомогательный буфер кадра
	mutable CFrameBuffer m_frameBuffer;
	// Текстура, используемая для хранения изображения, размытое по горизонтали
	mutable CTexture2D m_colorBuffer0;
	// Текстура, используемая для хранения финального изображения
	mutable CTexture2D m_colorBuffer1;

	// Ширина и высота текстуры
	mutable unsigned m_width = 0;
	mutable unsigned m_height = 0;
};
