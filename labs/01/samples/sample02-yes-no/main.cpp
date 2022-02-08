#include <windows.h>
#include <tchar.h>

int WINAPI WinMain(
	HINSTANCE /*hInstance*/,
	HINSTANCE /*hPrevInstance*/,
	LPSTR /*lpCmdLine*/,
	int /*nCmdShow*/)
{
	if (MessageBox(NULL, _T("Do you like to study?"), _T("Yes/no example"), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		MessageBox(NULL, _T("Good boy!"), _T("Wow"), MB_OK);
	}
	else // IDNO
	{
		MessageBox(NULL, _T("You are so lazy!"), _T("I'm so sad"), MB_OK);
	}
	return 0;
}
