// sample03_glut_application_oop.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "MyApplication.h"

// Создаем экземпляр нашего приложения
CMyApplication app("test");

int _tmain(int argc, _TCHAR* argv[])
{
	// и запускаем его
	app.MainLoop();

	return 0;
}
