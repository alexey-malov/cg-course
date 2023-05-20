#pragma once

// Фикс GDI+ для версий Windows 10 SDK ниже 2104
#define NOMINMAX

#include <chrono>
#include <cmath>
#include <stdexcept>
#include <tchar.h>
#include <windows.h>

#include <GL/GL.h>
#include <GL/GLU.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

// Фикс GDI+ для версий Windows 10 SDK ниже 2104
using std::min;
using std::max;

#include <gdiplus.h>
