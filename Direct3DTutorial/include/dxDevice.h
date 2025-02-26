#pragma once

#include "dxptr.h"
#include "window.h"
#include "dxStructures.h"

class DxDevice
{
public:
	explicit DxDevice(const mini::Window& window);
	mini::dx_ptr<ID3D11RenderTargetView> CreateRenderTargetView(const mini::dx_ptr<ID3D11Texture2D>& texture) const;
	const mini::dx_ptr<ID3D11DeviceContext>& context() const { return m_context; }
	const mini::dx_ptr<IDXGISwapChain>& swapChain() const { return m_swapChain; }
	ID3D11Device* operator->() const { return m_device.get(); }
private:
	mini::dx_ptr<ID3D11Device> m_device;
	mini::dx_ptr<ID3D11DeviceContext> m_context;
	mini::dx_ptr<IDXGISwapChain> m_swapChain;
};