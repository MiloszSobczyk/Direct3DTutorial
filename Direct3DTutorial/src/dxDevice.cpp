#include "dxDevice.h"
#include "exceptions.h"

DxDevice::DxDevice(const mini::Window& window)
{
	SwapChainDescription desc{ window.getHandle(), window.getClientSize() };
	ID3D11Device* d = nullptr;
	ID3D11DeviceContext* dc = nullptr;
	IDXGISwapChain* sc = nullptr;
	auto hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 
		D3D11_CREATE_DEVICE_DEBUG, nullptr, 0, D3D11_SDK_VERSION, &desc, &sc, &d, nullptr, &dc);
	m_device.reset(d);
	m_context.reset(dc);
	m_swapChain.reset(sc);
	if (FAILED(hr))
		THROW_WINAPI;
}

mini::dx_ptr<ID3D11RenderTargetView> DxDevice::CreateRenderTargetView(const mini::dx_ptr<ID3D11Texture2D>& texture) const
{
	ID3D11RenderTargetView* temp = nullptr;
	auto hr = m_device->CreateRenderTargetView(texture.get(), nullptr, &temp);
	mini::dx_ptr<ID3D11RenderTargetView> result{ temp };
	if (FAILED(hr))
		THROW_WINAPI;

	return result;
}
