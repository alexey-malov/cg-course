#include "src/OrbitApp.h"

#include <cstdlib>
#include <exception>
#include <iostream>

int main()
{
	try
	{
		OrbitApp app;
		app.Run();
	}
	catch (const std::exception& error)
	{
		std::cerr << error.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
