#include "src/HelloTriangleApp.h"

#include <cstdlib>
#include <exception>
#include <iostream>

int main()
{
	try
	{
		HelloTriangleApp app;
		app.Run();
	}
	catch (const std::exception& error)
	{
		std::cerr << error.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
