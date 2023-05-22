#pragma once

// Фикс GDI+ для версий Windows 10 SDK ниже 2104
#define NOMINMAX

#include <chrono>
#include <cmath>
#include <numbers>
#include <stdexcept>
#include <string>
#include <tchar.h>
#include <windows.h>

#include <GL/GL.h>
#include <GL/GLU.h>

// Фикс GDI+ для версий Windows 10 SDK ниже 2104
using std::max;
using std::min;

#include <gdiplus.h>
