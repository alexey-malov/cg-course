#include "StdAfx.h"
#include "MatrixFilter.h"

using namespace Gdiplus;

CMatrixFilter::CMatrixFilter(void)
:m_matrix(1, 1.0f)
,m_matrixWidth(1)
,m_matrixHeight(1)
{
}

CMatrixFilter::~CMatrixFilter(void)
{
}

void CMatrixFilter::InitializeMatrix(size_t width, size_t height, const float * initialValues, float scale)
{
	if ((width > 0) && (width <= MAX_MATRIX_SIZE) && 
		(height > 0) && (height <= MAX_MATRIX_SIZE))
	{
		m_matrixWidth = width;
		m_matrixHeight = height;

		if (initialValues != NULL)
		{
			m_matrix.assign(&initialValues[0], &initialValues[width * height]);
		}
		else
		{
			// ������ ������� ��������� ������ � �������� �� �������� 
			m_matrix.assign(width * height, 0.0f);

			// ������������� ����������� ������� � 1
			SetMatrixValue(width / 2, height / 2, 1);
		}

		if (scale != 1.0f)
		{
			// �������� ��� �������� ������� �� ���������� ���������
			std::transform(
				m_matrix.begin(), // ������� � ������� �������� �������
				m_matrix.end(), // �� ����������
				m_matrix.begin(), // ��������� �������� � ���� �������
				std::bind1st(	
					std::multiplies<float>(), 
					scale) // bind1st ������� ������� ������� ��
					// �������� ������� std::multiplies
					// ��������� ������� �������� �������� 
					// �������� ������� �� scale
				);
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

float CMatrixFilter::GetMatrixValue(size_t row, size_t column)const
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

Gdiplus::PixelFormat CMatrixFilter::ChoosePreferableOutputPixelFormat(Gdiplus::Bitmap & bitmapData)const
{
	return bitmapData.GetPixelFormat();
}

std::auto_ptr<Gdiplus::Bitmap> CMatrixFilter::ApplyFilter(Gdiplus::Bitmap & srcBitmap)
{
	// �������������� ���������� ��� �������� ����� ������������ ��������
	int w = srcBitmap.GetWidth();
	int h = srcBitmap.GetHeight();
	size_t matrixHalfWidth = m_matrixWidth / 2;
	size_t matrixHalfHeight = m_matrixHeight / 2;

	// ������� ��������� ����������� ��� �������� ����������,
	// ����� ����������� ������� ����������� �������������� ����� 
	// ������� �������� �����������
	std::auto_ptr<Bitmap> pResult(
		new Bitmap(w, h, ChoosePreferableOutputPixelFormat(srcBitmap)));
	// ������ �� �������������� �����������
	Bitmap & dstBitmap = *pResult;

	// ���� �� ������� �����������
	for (int y0 = 0; y0 < h; ++y0)
	{
		// ���� �� �������� �����������
		for (int x0 = 0; x0 < w; ++x0)
		{
			size_t y = y0 - matrixHalfHeight;

			// ������ ��������� �������� ����� �������
			float r = 0, g = 0, b = 0, a = 0;

			// ���� �� ������� �������
			for (size_t row = 0; row < m_matrixHeight; ++row, ++y)
			{
				size_t x = x0 - matrixHalfWidth;

				// ������������ ���������� y �������� �����������
				size_t srcY = min(h - 1, max((int)y, 0));

				// ���� �� �������� �������
				for (size_t column = 0; column < m_matrixWidth; ++column, ++x)
				{
					// ������������ ���������� x �������� �����������
					size_t srcX = min(w - 1, max((int)x, 0));

					// �������� ������� ����������� �������
					float coeff = GetMatrixValue(row, column);

					// ���������� ��� ��� ���������� ���������� �����
					// ����� �������
					Color c;
					srcBitmap.GetPixel((int)srcX, (int)srcY, &c);
					r += c.GetRed() * coeff;
					g += c.GetGreen() * coeff;
					b += c.GetBlue() * coeff;
					a += c.GetAlpha() * coeff;
				}
			}

			// ������������ ���� ������� (������ �������� ����������� 
			// � ����������� �� ���� �������)
			Color srcColor;
			srcBitmap.GetPixel(x0, y0, &srcColor);
			AdjustColor(r, g, b, a, srcColor);

			// ��������� ���� ��������� ������� � ���������� ���
			// � �������� ��������� �����������
			Color dstColor(Clamp(a), Clamp(r), Clamp(g), Clamp(b));
			dstBitmap.SetPixel(x0, y0, dstColor);
		}
	}

	return pResult;
}

void CMatrixFilter::AdjustColor(float & /*r*/, float & /*g*/, float & /*b*/, float & /*a*/, Gdiplus::Color const& /*srcColor*/)const
{
	// ��������� ��� ��������� r, g, b � a ������������ �����
	// ���� ����� ����� ���� ���������� � ����������� ������
}

