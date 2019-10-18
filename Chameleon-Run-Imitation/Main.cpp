#include "stdafx.h"
#include "Game.h"

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	ba::Game tester;

	if (tester.Init(hInstance))
	{
		tester.Run();
		tester.Release();
	}
}
