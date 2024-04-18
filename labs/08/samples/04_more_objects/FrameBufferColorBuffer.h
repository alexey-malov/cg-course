#pragma once
#include "FrameBuffer.h"
#include "icolorbuffer.h"

/*
Реализация интерфейса IColorBuffer, осуществляющая графический вывод в буфер кадра, представленный
классом CFrameBuffer
*/
class CFrameBufferColorBuffer : public IColorBuffer
{
public:
	explicit CFrameBufferColorBuffer(CFrameBuffer& frameBuffer)
		: m_frameBuffer(frameBuffer)
	{
	}

	void SetPixelColor(unsigned x, unsigned y, std::uint32_t color) override
	{
		m_frameBuffer.SetPixel(x, y, color);
	}

private:
	CFrameBuffer& m_frameBuffer;
};
