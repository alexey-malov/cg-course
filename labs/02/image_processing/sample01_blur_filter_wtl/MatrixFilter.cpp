#include "StdAfx.h"
#include "MatrixFilter.h"

using namespace Gdiplus;

CMatrixFilter::CMatrixFilter(void)
	: m_matrix(1, 1.0f)
	, m_matrixWidth(1)
	, m_matrixHeight(1)
{
}

CMatrixFilter::~CMatrixFilter(void)
{
}

void CMatrixFilter::InitializeMatrix(size_t width, size_t height, const float* initialValues, float scale)
{
	if ((width > 0) && (width <= MAX_MATRIX_SIZE) && (height > 0) && (height <= MAX_MATRIX_SIZE))
	{
		m_matrixWidth = width;
		m_matrixHeight = height;

		if (initialValues != NULL)
		{
			m_matrix.assign(&initialValues[0], &initialValues[width * height]);
		}
		else
		{
			// задаем матрице начальный размер и обнуляем ее элементы
			m_matrix.assign(width * height, 0.0f);

			// Устанавливаем центральный элемент в 1
			SetMatrixValue(width / 2, height / 2, 1);
		}

		if (scale != 1.0f)
		{
			// умножаем все элементы матрицы на масштабный множитель
			std::transform(
				m_matrix.begin(), // начиная с первого элемента матрицы
				m_matrix.end(), // до последнего
				m_matrix.begin(), // результат помещаем в саму матрицу
				[scale](float value) { return scale * value; });
		}
	}
	else
	{
		throw std::out_of_range("Matrix dimensions is out of range");
	}
}

void CMatrixFilter::SetMatrixValue(size_t row, size_t column, float value)
{
	if ((row < m_matrixHeight) && (column < m_matrixWidth))
	{
		m_matrix[row * m_matrixWidth + column] = value;
	}
	else
	{
		throw std::invalid_argument("Row or Column index is out of range");
	}
}

float CMatrixFilter::GetMatrixValue(size_t row, size_t column) const
{
	if ((row < m_matrixHeight) && (column < m_matrixWidth))
	{
		return m_matrix[row * m_matrixWidth + column];
	}
	else
	{
		throw std::invalid_argument("Row or Column index is out of range");
	}
}

Gdiplus::PixelFormat CMatrixFilter::ChoosePreferableOutputPixelFormat(Gdiplus::Bitmap& bitmapData) const
{
	return bitmapData.GetPixelFormat();
}

std::unique_ptr<Gdiplus::Bitmap> CMatrixFilter::ApplyFilter(Gdiplus::Bitmap& srcBitmap)
{
	// Инициализируем переменные для хранения часто используемых значений
	int w = srcBitmap.GetWidth();
	int h = srcBitmap.GetHeight();
	size_t matrixHalfWidth = m_matrixWidth / 2;
	size_t matrixHalfHeight = m_matrixHeight / 2;

	// создаем растровое изображение для хранения результата,
	// давая возможность классам наследникам переопределить выбор
	// формата пикселей изображения
	std::unique_ptr<Bitmap> pResult(
		new Bitmap(w, h, ChoosePreferableOutputPixelFormat(srcBitmap)));
	// Ссылка на результирующее изображение
	Bitmap& dstBitmap = *pResult;

	// цикл по строкам изображения
	for (int y0 = 0; y0 < h; ++y0)
	{
		// цикл по столбцам изображения
		for (int x0 = 0; x0 < w; ++x0)
		{
			size_t y = y0 - matrixHalfHeight;

			// задаем начальное значение цвета пикселя
			float r = 0, g = 0, b = 0, a = 0;

			// цикл по строкам матрицы
			for (size_t row = 0; row < m_matrixHeight; ++row, ++y)
			{
				size_t x = x0 - matrixHalfWidth;

				// ограничиваем координату y областью изображения
				size_t srcY = min(h - 1, max((int)y, 0));

				// цикл по столбцам матрицы
				for (size_t column = 0; column < m_matrixWidth; ++column, ++x)
				{
					// ограничиваем координату x областью изображения
					size_t srcX = min(w - 1, max((int)x, 0));

					// получаем текущий коэффициент матрицы
					float coeff = GetMatrixValue(row, column);

					// используем его для обновления взвешенной суммы
					// цвета пикселя
					Color c;
					srcBitmap.GetPixel((int)srcX, (int)srcY, &c);
					r += c.GetRed() * coeff;
					g += c.GetGreen() * coeff;
					b += c.GetBlue() * coeff;
					a += c.GetAlpha() * coeff;
				}
			}

			// корректируем цвет пикселя (данная операция реализуется
			// в зависимости от типа фильтра)
			Color srcColor;
			srcBitmap.GetPixel(x0, y0, &srcColor);
			AdjustColor(r, g, b, a, srcColor);

			// Формируем цвет выходного пикселя и записываем его
			// в выходное растровое изображение
			Color dstColor(Clamp(a), Clamp(r), Clamp(g), Clamp(b));
			dstBitmap.SetPixel(x0, y0, dstColor);
		}
	}

	return pResult;
}

void CMatrixFilter::AdjustColor(float& /*r*/, float& /*g*/, float& /*b*/, float& /*a*/, Gdiplus::Color const& /*srcColor*/) const
{
	// оставляем без изменения r, g, b и a составляющие цвета
	// этот метод может быть перегружен в производном классе
}
