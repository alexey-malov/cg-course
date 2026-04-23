// 03_twisted_object.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MyApplication.h"


CMyApplication app("Twist object with Vertex shader", 800, 600);

int _tmain(int argc, _TCHAR* argv[])
{
	glewInit();
	if (!GLEW_ARB_shader_objects)
	{
		return -1;
	}

	GLint maxVertexUniforms;
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &maxVertexUniforms);
	std::cout << "The implementation supports as many as " << maxVertexUniforms << " vertex uniforms\n";

	GLint maxFragmentUniforms;
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &maxFragmentUniforms);
	std::cout << "The implementation supports as many as " << maxFragmentUniforms << " fragment uniforms\n";

	try
	{
		app.MainLoop();
	}
	catch (std::exception const& e)
	{
		std::cout << e.what();
	}

	return 0;
}
