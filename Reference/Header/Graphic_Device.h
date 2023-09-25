#pragma once

#include "Base.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{
	DECLARE_SINGLETON(CGraphic_Device)

private:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:
	// �׷��� ����̽� �ʱ�ȭ
	HRESULT Ready_Graphic_Device(HWND _hWnd, GRAPHIC_DESC::WINMODE _eWinMode, _uint _iWinCX, _uint _iWinCY,
		_Inout_ ID3D11Device** _pDevice, _Inout_ ID3D11DeviceContext** _pContext);

	// ����� Ŭ���� 
	HRESULT Clear_BackBuffer_View(_float4 _vClearColor);

	// ����, ���Ľ� ���� Ŭ����
	HRESULT Clear_DepthStencil_View();

	// �ĸ� ���� ���� ���۷� ��ü (����� ȭ�鿡 ���� �����ֱ�)
	HRESULT Present();

private:
	// �޸� �Ҵ� ����. Vertex Index Buffer, TextureLoad�� ���� Com��ü�� ������ ���õ� ������ �ô´�. 
	ID3D11Device*			m_pDevice = nullptr;
	// ����� �����ϰų� �׸��� ����. 
	// (���ε��۾�, �������۸� SetStreamSource(), SetIndices(), SetTransform(), SetTexture())
	ID3D11DeviceContext*	m_pContext = nullptr;
	// �ĸ�, ���� ���� ��ü�ϸ鼭 ȭ�鿡 �����ִ� ����.
	IDXGISwapChain*			m_pSwapChain = nullptr;

	/* ���� 9���� ����ϴ� Com��ü : IDirect3DTexture9 */
	/* ID3D11Texture2D : �ؽ��ĸ� ǥ���ϴ� ������ü Ÿ���̴�.
	why? �뵵�� �´� ���������� ����ϱ����� �ؽ��İ�ü�� ����������.  */
	

	// ����Ÿ�ٿ����� ���� �� �ִ� �ؽ��� Ÿ��.
	ID3D11RenderTargetView* m_pBackBufferRTV	 = nullptr;
	// ���̽��ٽǹ��۷μ� ���� �� �ִ� Ÿ��.
	ID3D11DepthStencilView* m_pDepthStencilView  = nullptr;
	// ���̴��� ���޵� �� �ִ� �ؽ�óŸ��.
	// ID3D11ShaderResourceView 

private:
	/* ����ü���� �ʼ������� ����۰� �ʿ��ϴ�. 
	   �׷��⿡ ����۸� �����ϱ� ���� ������ �޾ƾ� �Ѵ�.
	   ��, ����ü���� �����ߴٴ� ���� ����۸� �����ߴٴ� ���̴�. */
	HRESULT Ready_SwapChain(HWND _hWnd, GRAPHIC_DESC::WINMODE _eWinMode, _uint _iWinCX, _uint _iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilRenderTargetView(_uint _iWinCX, _uint _iWinCY);

public:
	virtual void Free() override;
};

END
