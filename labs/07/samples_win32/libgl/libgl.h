#pragma once
#include "framework.h"

std::string LoadTextFile(const char fileName[]);
void LoadShaderSource(GLhandleARB shader, const char fileName[]);
std::string GetInfoLog(GLhandleARB object);
void CompileShader(GLhandleARB shader);
void LinkProgram(GLhandleARB program);