#include "stdafx.h"
#include "FrameBuffer.h"

CFrameBuffer::CFrameBuffer(unsigned width, unsigned height)
:m_pixels(width * height)
,m_width(width)
,m_height(height)
{
}

void CFrameBuffer::Clear(boost::uint32_t color)
{
	std::fill(m_pixels.begin(), m_pixels.end(), color);
}
