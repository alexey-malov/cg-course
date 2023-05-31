#include "pch.h"
#include "Color4d.h"

void Color4d::SetOpenGLColor() const
{
	glColor4f(r, g, b, a);
}
