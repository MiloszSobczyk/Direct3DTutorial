#include "dxApplication.h"

using namespace mini;

DxApplication::DxApplication(HINSTANCE hInstance)
	: WindowApplication(hInstance), m_device(m_window)
{ 
	ID3D11Texture2D* temp = nullptr;
	m_device.swapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&temp));
	const dx_ptr<ID3D11Texture2D> backBuffer{ temp };
	m_backBuffer = m_device.CreateRenderTargetView(backBuffer);
	SIZE wndSize = m_window.getClientSize();
	m_depthBuffer = m_device.CreateDepthStencilView(wndSize);
	ID3D11RenderTargetView* backBufferRaw = m_backBuffer.get();
	m_device.context()->OMSetRenderTargets(1, &backBufferRaw, m_depthBuffer.get());
	Viewport viewport{ wndSize };
	m_device.context()->RSSetViewports(1, &viewport);

	const auto vsBytes = DxDevice::LoadByteCode(L"vs.cso");
	const auto psBytes = DxDevice::LoadByteCode(L"ps.cso");
	m_vertexShader = m_device.CreateVertexShader(vsBytes);
	m_pixelShader = m_device.CreatePixelShader(psBytes);
	const auto vertices = CreateTriangleVertices();
	m_vertexBuffer = m_device.CreateVertexBuffer(vertices);
	std::vector<D3D11_INPUT_ELEMENT_DESC> elements = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	m_layout = m_device.CreateInputLayout(elements, vsBytes);
}

std::vector<DirectX::XMFLOAT2> DxApplication::CreateTriangleVertices() const
{
	std::vector<DirectX::XMFLOAT2> vertices = {
		DirectX::XMFLOAT2(-0.5f, -0.5f),
		DirectX::XMFLOAT2(-0.5f, 0.5f),
		DirectX::XMFLOAT2(0.5f, -0.5f)
	};

	return vertices;
}


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

void DxApplication::Render() 
{ 
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_device.context()->ClearRenderTargetView(m_backBuffer.get(), clearColor);

	m_device.context()->ClearDepthStencilView(m_depthBuffer.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_device.context()->VSSetShader(m_vertexShader.get(), nullptr, 0);
	m_device.context()->PSSetShader(m_pixelShader.get(), nullptr, 0);
	m_device.context()->IASetInputLayout(m_layout.get());
	m_device.context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ID3D11Buffer* vbs[] = { m_vertexBuffer.get() };
	UINT strides[] = { sizeof(DirectX::XMFLOAT2) };
	UINT offsets[] = { 0 };
	m_device.context()->IASetVertexBuffers(0, 1, vbs, strides, offsets);
	m_device.context()->Draw(3, 0);
};