#pragma once

// Фикс GDI+ для версий Windows 10 SDK ниже 2104
#define NOMINMAX



#include <windows.h>
#include <assert.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <stdexcept>

// Фикс GDI+ для версий Windows 10 SDK ниже 2104
using std::max;
using std::min;

#include <gdiplus.h>
