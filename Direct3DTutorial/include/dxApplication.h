#pragma once

#include "windowApplication.h"
#include "dxDevice.h"
#include <DirectXMath.h>

class DxApplication : public mini::WindowApplication
{
public:
	explicit DxApplication(HINSTANCE hInstance);
	std::vector<DirectX::XMFLOAT2> CreateTriangleVertices() const;
protected:
	int MainLoop() override;
private:
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 color;
	};

	static std::vector<VertexPositionColor> CreateCubeVertices();
	static std::vector<unsigned short> CreateCubeIndices();

	void Render();
	void Update();

	DxDevice m_device;
	mini::dx_ptr<ID3D11RenderTargetView> m_backBuffer;
	mini::dx_ptr<ID3D11DepthStencilView> m_depthBuffer;
	mini::dx_ptr<ID3D11Buffer> m_vertexBuffer;
	mini::dx_ptr<ID3D11VertexShader> m_vertexShader;
	mini::dx_ptr<ID3D11Buffer> m_indexBuffer;
	mini::dx_ptr<ID3D11PixelShader> m_pixelShader;
	mini::dx_ptr<ID3D11InputLayout> m_layout;
};