#pragma once

#include "windowApplication.h"
#include "dxDevice.h"

class DxApplication : public mini::WindowApplication
{
public:
	explicit DxApplication(HINSTANCE hInstance);
protected:
	int MainLoop() override;
private:
	void Render();
	void Update();

	DxDevice m_device;
	mini::dx_ptr<ID3D11RenderTargetView> m_backBuffer;
};