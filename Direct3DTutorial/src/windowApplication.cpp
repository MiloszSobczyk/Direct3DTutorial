#include "windowApplication.h"

using namespace mini;

WindowApplication::WindowApplication(HINSTANCE hInstance, int wndWidth, int wndHeight, std::wstring wndTitle)
	: m_window(hInstance, wndWidth, wndHeight, wndTitle, this), m_hInstance(hInstance)
{ }

int WindowApplication::MainLoop()
{
	MSG msg = { nullptr };
	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return static_cast<int>(msg.wParam);
}

int WindowApplication::Run(int cmdShow)
{
	m_window.Show(cmdShow);
	return MainLoop();
}