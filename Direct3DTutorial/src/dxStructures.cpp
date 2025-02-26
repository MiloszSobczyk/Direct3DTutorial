#include "dxStructures.h"

SwapChainDescription::SwapChainDescription(HWND wndHwnd, SIZE wndSize)
	: DXGI_SWAP_CHAIN_DESC{}
{
	BufferDesc.Width = wndSize.cx;
	BufferDesc.Height = wndSize.cy;
	BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	BufferDesc.RefreshRate.Denominator = 1;
	SampleDesc.Count = 1;
	BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	BufferCount = 1;
	OutputWindow = wndHwnd;
	Windowed = true;
}
