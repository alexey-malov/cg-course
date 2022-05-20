#pragma once

/*
	Класс CFrameBuffer, представляющий из себя 32-битный буфер кадра
	фиксированного размера
*/
class CFrameBuffer
{
public:
	CFrameBuffer(unsigned width, unsigned height);

	// Ширина буфера в пикселях
	unsigned GetWidth()const throw()
	{
		return m_width;
	}

	// Высота буфера в пикселях
	unsigned GetHeight()const throw()
	{
		return m_height;
	}

	// Очистка содержимого буфера заданным цветом
	void Clear(std::uint32_t color = 0);

	// Получение адреса начала соотв. строки пикселей (для чтения)
	const std::uint32_t * GetPixels(unsigned row = 0)const throw()
	{
		assert(row < m_height);
		return &m_pixels[size_t(row * m_width)];
	}

	// Получение адреса начала соотв. строки пикселей (для записи)
	std::uint32_t * GetPixels(unsigned row = 0)throw()
	{
		assert(row < m_height);
		return &m_pixels[size_t(row * m_width)];
	}

	// Получение цвета пикселя с заданными координатами
	std::uint32_t GetPixel(unsigned x, unsigned y)const throw()
	{
		assert(x < m_width);
		assert(y < m_height);
		return m_pixels[size_t(y * m_width + x)];
	}

	// Установка цвета пикселя с заданными координатами
	void SetPixel(unsigned x, unsigned y, std::uint32_t color) throw()
	{
		assert(x < m_width);
		assert(y < m_height);
		m_pixels[size_t(y * m_width + x)] = color;
	}

private:
	std::vector<std::uint32_t> m_pixels;
	unsigned m_width;
	unsigned m_height;
};
