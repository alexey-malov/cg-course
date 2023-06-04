#pragma once

// Фикс GDI+ для версий Windows 10 SDK ниже 2104
#define NOMINMAX

#include <Windows.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <numbers>
#include <utility>
#include <tchar.h>

// Фикс GDI+ для версий Windows 10 SDK ниже 2104
using std::max;
using std::min;

#include <gdiplus.h>
