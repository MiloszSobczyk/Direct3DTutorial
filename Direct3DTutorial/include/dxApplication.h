#pragma once

#include "windowApplication.h"
#include "dxDevice.h"

class DxApplication : public mini::WindowApplication
{
public:
	explicit DxApplication(HINSTANCE hInstance);
	explicit DxApplication(HINSTANCE hInstance, int width, int height, std::wstring title);
protected:
	int MainLoop() override;
private:
	void Render();
	void Update();

	DxDevice m_device;
};