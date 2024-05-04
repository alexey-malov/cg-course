#include "stdafx.h"
#include "ViewPort.h"

CViewPort::CViewPort(unsigned left, unsigned top, unsigned width, unsigned height) noexcept
:m_left(left)
,m_top(top)
,m_width(width)
,m_height(height)
{
}

CViewPort::CViewPort() noexcept
:m_left(0)
,m_top(0)
,m_width(0)
,m_height(0)
{

}