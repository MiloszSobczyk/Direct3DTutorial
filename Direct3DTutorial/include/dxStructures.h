#pragma once
#include <d3d11.h>

struct SwapChainDescription : DXGI_SWAP_CHAIN_DESC 
{
	SwapChainDescription(HWND wndHwnd, SIZE wndSize);
};