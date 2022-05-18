#pragma once
#include "icolorbuffer.h"
#include "FrameBuffer.h"

/*
Реализация интерфейса IColorBuffer, осуществляющая графический вывод в буфер кадра, представленный
классом CFrameBuffer
*/
class CFrameBufferColorBuffer : public IColorBuffer
{
public:
	CFrameBufferColorBuffer(CFrameBuffer & frameBuffer)
		:m_frameBuffer(frameBuffer)
	{
	}

	virtual void SetPixelColor(unsigned x, unsigned y, boost::uint32_t color)
	{
		m_frameBuffer.SetPixel(x, y, color);
	}
private:
	CFrameBuffer & m_frameBuffer;
};

