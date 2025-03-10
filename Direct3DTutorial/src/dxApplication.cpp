#include "dxApplication.h"
#include <iostream>

using namespace mini;
using namespace DirectX;

DxApplication::DxApplication(HINSTANCE hInstance)
	: WindowApplication(hInstance), m_device(m_window), m_rotationAngle(0.f)
{
	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_lastTime);
	XMStoreFloat4x4(&m_staticModelMtx, XMMatrixTranslation(-5.f, 0.0f, 0.0f));

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
	//const auto vertices = CreateTriangleVertices();
	//m_vertexBuffer = m_device.CreateVertexBuffer(vertices);
	//std::vector<D3D11_INPUT_ELEMENT_DESC> elements = {
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	//};
	//m_layout = m_device.CreateInputLayout(elements, vsBytes);

	const auto vertices = CreateCubeVertices();
	m_vertexBuffer = m_device.CreateVertexBuffer(vertices);
	const auto indices = CreateCubeIndices();
	m_indexBuffer = m_device.CreateIndexBuffer(indices);
	std::vector<D3D11_INPUT_ELEMENT_DESC> elements{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		offsetof(VertexPositionColor, color),
		D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	m_layout = m_device.CreateInputLayout(elements, vsBytes);

	XMStoreFloat4x4(&m_modelMtx, XMMatrixIdentity());
	XMStoreFloat4x4(&m_viewMtx,
		XMMatrixRotationX(XMConvertToRadians(-30)) *
		XMMatrixTranslation(0.0f, 0.0f, 10.0f));
	XMStoreFloat4x4(&m_projMtx, XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45),
		static_cast<float>(wndSize.cx) / wndSize.cy,
		0.1f, 100.0f));
	m_cbMVP = m_device.CreateConstantBuffer<XMFLOAT4X4>();
}

std::vector<DirectX::XMFLOAT2> DxApplication::CreateTriangleVertices() const
{
	std::vector<DirectX::XMFLOAT2> vertices = {
		DirectX::XMFLOAT2(-0.5f, -0.5f),
		DirectX::XMFLOAT2(-0.5f, 0.5f),
		DirectX::XMFLOAT2(0.5f, -0.5f),
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

void DxApplication::Update()
{
	CalculateRotationMatrix();

	D3D11_MAPPED_SUBRESOURCE res;
	m_device.context()->Map(m_cbMVP.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	XMMATRIX mvp = XMLoadFloat4x4(&m_modelMtx) * XMLoadFloat4x4(&m_viewMtx) * XMLoadFloat4x4(&m_projMtx);
	memcpy(res.pData, &mvp, sizeof(XMMATRIX));
	m_device.context()->Unmap(m_cbMVP.get(), 0);
}

void DxApplication::CalculateRotationMatrix()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);

	double deltaTime = static_cast<double>(currentTime.QuadPart - m_lastTime.QuadPart) / m_frequency.QuadPart;
	m_lastTime = currentTime;

	constexpr float angularSpeed = XM_PI / 4.0f;
	m_rotationAngle += angularSpeed * static_cast<float>(deltaTime);

	XMMATRIX rotationMatrix = XMMatrixRotationY(m_rotationAngle);
	XMStoreFloat4x4(&m_modelMtx, rotationMatrix);
}

std::vector<DxApplication::VertexPositionColor> DxApplication::CreateCubeVertices()
{
	return {
		// Red
		{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
		{ { +0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
		{ { +0.5f, +0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
		{ { -0.5f, +0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
		// Green
		{ { -0.5f, -0.5f, +0.5f }, { 0.0f, 1.0f, 0.0f } },
		{ { +0.5f, -0.5f, +0.5f }, { 0.0f, 1.0f, 0.0f } },
		{ { +0.5f, +0.5f, +0.5f }, { 0.0f, 1.0f, 0.0f } },
		{ { -0.5f, +0.5f, +0.5f }, { 0.0f, 1.0f, 0.0f } },
		// Blue
		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f } },
		{ { +0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f } },
		{ { +0.5f, -0.5f, +0.5f }, { 0.0f, 0.0f, 1.0f } },
		{ { -0.5f, -0.5f, +0.5f }, { 0.0f, 0.0f, 1.0f } },
		// Yellow
		{ { -0.5f, +0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f } },
		{ { +0.5f, +0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f } },
		{ { +0.5f, +0.5f, +0.5f }, { 1.0f, 1.0f, 0.0f } },
		{ { -0.5f, +0.5f, +0.5f }, { 1.0f, 1.0f, 0.0f } },
		// Magenta
		{ { +0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 1.0f } },
		{ { +0.5f, +0.5f, -0.5f }, { 1.0f, 0.0f, 1.0f } },
		{ { +0.5f, +0.5f, +0.5f }, { 1.0f, 0.0f, 1.0f } },
		{ { +0.5f, -0.5f, +0.5f }, { 1.0f, 0.0f, 1.0f } },
		// Cyan
		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f } },
		{ { -0.5f, +0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f } },
		{ { -0.5f, +0.5f, +0.5f }, { 0.0f, 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, +0.5f }, { 0.0f, 1.0f, 1.0f } },
	};
}

std::vector<unsigned short> DxApplication::CreateCubeIndices()
{
	return {
		// Front face
		0, 2, 1,  0, 3, 2,
		// Back face
		4, 5, 6,  4, 6, 7,
		// Bottom face
		8, 9, 10,  8, 10, 11,
		// Top face
		12, 14, 13,  12, 15, 14,
		// Right face
		16, 17, 18,  16, 18, 19,
		// Left face
		20, 22, 21,  20, 23, 22,
	};
}



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
	UINT strides[] = { sizeof(VertexPositionColor) };
	UINT offsets[] = { 0 };

	m_device.context()->IASetVertexBuffers(0, 1, vbs, strides, offsets);
	m_device.context()->IASetIndexBuffer(m_indexBuffer.get(), DXGI_FORMAT_R16_UINT, 0);
	ID3D11Buffer* cbs[] = { m_cbMVP.get() };
	m_device.context()->VSSetConstantBuffers(0, 1, cbs);

	D3D11_MAPPED_SUBRESOURCE res;
	m_device.context()->Map(m_cbMVP.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	XMMATRIX mvp = XMLoadFloat4x4(&m_modelMtx) * XMLoadFloat4x4(&m_viewMtx) * XMLoadFloat4x4(&m_projMtx);
	memcpy(res.pData, &mvp, sizeof(XMMATRIX));
	m_device.context()->Unmap(m_cbMVP.get(), 0);

	m_device.context()->DrawIndexed(36, 0, 0);

	m_device.context()->Map(m_cbMVP.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	XMMATRIX staticMvp = XMLoadFloat4x4(&m_staticModelMtx) * XMLoadFloat4x4(&m_viewMtx) * XMLoadFloat4x4(&m_projMtx);
	memcpy(res.pData, &staticMvp, sizeof(XMMATRIX));
	m_device.context()->Unmap(m_cbMVP.get(), 0);

	m_device.context()->DrawIndexed(36, 0, 0);
}

bool DxApplication::ProcessMessage(WindowMessage& msg)
{
	static bool rotating = false;
	static bool zooming = false;
	static POINT lastMousePos;
	static float cameraAngleX = -30.0f;
	static float cameraDistance = 10.0f;

	switch (msg.message)
	{
	case WM_LBUTTONDOWN:
		rotating = true;
		lastMousePos = { LOWORD(msg.lParam), HIWORD(msg.lParam) };
		return true;

	case WM_RBUTTONDOWN:
		zooming = true;
		lastMousePos = { LOWORD(msg.lParam), HIWORD(msg.lParam) };
		return true;

	case WM_MOUSEMOVE:
		if (rotating || zooming)
		{
			POINT currentMousePos = { LOWORD(msg.lParam), HIWORD(msg.lParam) };
			int deltaY = currentMousePos.y - lastMousePos.y;

			if (rotating)
			{
				cameraAngleX += deltaY * 0.5f;
				cameraAngleX = max(-90.0f, min(90.0f, cameraAngleX));
			}
			else if (zooming)
			{
				cameraDistance += deltaY * 0.1f;
				cameraDistance = max(1.0f, min(50.0f, cameraDistance));
			}

			XMMATRIX viewMatrix = XMMatrixRotationX(XMConvertToRadians(cameraAngleX)) *
				XMMatrixTranslation(0.0f, 0.0f, cameraDistance);
			XMStoreFloat4x4(&m_viewMtx, viewMatrix);

			lastMousePos = currentMousePos;
		}
		return true;

	case WM_LBUTTONUP:
		rotating = false;
		return true;

	case WM_RBUTTONUP:
		zooming = false;
		return true;
	}
	return false;
}
