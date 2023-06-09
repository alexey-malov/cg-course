#pragma once

class CMatrixFilter
{
public:
	// ��������� ������ � ��������� ����������� � ���������� ���������
	std::auto_ptr<Gdiplus::Bitmap> ApplyFilter(
		Gdiplus::Bitmap & srcBitmap);

protected:
	enum
	{
		// ������������ ������ ������� ������� 
		// (������ ������ ������ ��� - ������������������ ����� �������)
		MAX_MATRIX_SIZE = 50,
	};

	// �������� ����������� � ���������� �������, 
	// ����� �� ���������� ������ ��� �������-�����������
	CMatrixFilter(void);
	virtual ~CMatrixFilter(void);

	// �������� ������ �������,
	// � ����� ����������� ��������� ������������� �� ���������
	void InitializeMatrix(
		size_t width, 
		size_t height, 
		const float * initialValues = NULL, 
		float scale = 1.0f);

	// ������ ������� � ������������� ������� �������
	void SetMatrixValue(size_t row, size_t column, float value);
	float GetMatrixValue(size_t row, size_t column)const
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


	// ���� ����������� �������-����������� �����������
	// ����� ����������������� ������� �������� ��� ��������� �����������
	virtual Gdiplus::PixelFormat ChoosePreferableOutputPixelFormat(
		Gdiplus::Bitmap & bitmapData)const;

	// ���� ����������� �������-����������� �����������
	// ��������� ����� ������� ����� ���������� � ���� ���������� �������
	virtual void AdjustColor(float & r, float & g, float & b, float & a, DWORD srcColor)const;

private:
	// �������� ���� � ��������� �� 0 �� 255
	inline static BYTE Clamp(float color)
	{
		return static_cast<BYTE>(
			(color < 0) ? 0 : 
			(color > 255) ? 255 :
			color
			);
	}

	inline static DWORD MakeARGB(BYTE a, BYTE r, BYTE g, BYTE b)
	{
		return (a << 24) | (r << 16) | (g << 8) | b;
	}
private:
	// ������������ ������� �������
	std::vector<float>	m_matrix;
	// ������� ������� �������
	size_t	m_matrixWidth;
	size_t	m_matrixHeight;
};
