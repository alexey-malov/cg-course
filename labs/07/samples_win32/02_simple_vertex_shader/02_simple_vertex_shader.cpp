// 02_simple_vertex_shader.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "Application.h"
#include "MyApplication.h"
#include <windows.h>

int main(int argc, char* argv[])
{

	try
	{
		CMyApplication app(&argc, argv, 640, 480, "Simple vertex shader");
		app.Init();
		app.Run();
	}
	catch (std::exception& e)
	{
		std::cout << "%s\n" << e.what();
	}

	return 0;
}
