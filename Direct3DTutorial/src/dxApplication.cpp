#include "dxApplication.h"

using namespace mini;

DxApplication::DxApplication(HINSTANCE hInstance)
	: WindowApplication(hInstance), m_device(m_window)
{ }

DxApplication::DxApplication(HINSTANCE hInstance, int width, int height, std::wstring title)
	: WindowApplication(hInstance, width, height, title), m_device(m_window)
{ }

int DxApplication::MainLoop()
{
	MSG msg{};
	do
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Update();
			Render();
			m_device.swapChain()->Present(0, 0);
		}
	} while (msg.message != WM_QUIT);
	return msg.wParam;
}

void DxApplication::Update() { };
void DxApplication::Render() { };