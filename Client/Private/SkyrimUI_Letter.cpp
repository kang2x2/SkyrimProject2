#include "framework.h"
#include "SkyrimUI_Letter.h"

#include "GameInstance.h"

CSkyrimUI_Letter::CSkyrimUI_Letter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CSkyrimUI(_pDevice, _pContext)
{
}

CSkyrimUI_Letter::CSkyrimUI_Letter(const CSkyrimUI_Letter& rhs)
	: CSkyrimUI(rhs)
{
}

HRESULT CSkyrimUI_Letter::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CSkyrimUI_Letter::Initialize_Clone(void* _pArg)
{
	__super::Initialize_Clone(_pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	UI_InitPos((_float)(g_iWinSizeX / 2.f), (_float)(g_iWinSizeY / 2.f));

	m_fSizeX = g_iWinSizeX * 0.5f;
	m_fSizeY = g_iWinSizeY * 1.f;

	m_pTransformCom->Set_Scaling(_float3(m_fSizeX , m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX, m_fY, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	m_strText = TEXT("ȭ��Ʈ�� ���ϼ��ο��� �뷮��\n�������� �߰ߵƴ�.\n�� �Ҽ��� �͵��� ���� ����\n�������� �𸣳� �� ������ ����\n�ڸ��ؾ� �Ѵ�.\n���ϼ��θ� ������ �شٸ�\n�� �߱׷����� �̸��� �ɰ�\n������ ������ ���� ���� ����Ѵ�.");

	m_strName = TEXT("UI_Letter");

	return S_OK;

}

void CSkyrimUI_Letter::PriorityTick(_float _fTimeDelta)
{
}

void CSkyrimUI_Letter::Tick(_float _fTimeDelta)
{
	m_fWaitTime += _fTimeDelta;

	if (m_fWaitTime > 2.f)
		g_bIsPause = true;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyDown(VK_SPACE))
	{
		m_bReadyDead = true;
		g_bIsPause = false;
	}

	Safe_Release(pGameInstance);
}

void CSkyrimUI_Letter::LateTick(_float _fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_UI_0, this);
}

HRESULT CSkyrimUI_Letter::Render()
{
	if (m_fWaitTime > 2.f)
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pVIBufferCom->Render();

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		pGameInstance->Render_Font(TEXT("Font_Bold"), m_strText,
			_float2(420.f, 120.f), { 0.1f, 0.1f, 0.1f, 1.f }
		, 0.f, _float2(0.f, 0.f), 0.7f);

		pGameInstance->Render_Font(TEXT("Font_Bold"), TEXT("���� ������ �߱׷���"),
			_float2(420.f, 550.f), { 0.1f, 0.1f, 0.1f, 1.f }
		, 0.f, _float2(0.f, 0.f), 0.7f);

		Safe_Release(pGameInstance);
	}

	return S_OK;
}

HRESULT CSkyrimUI_Letter::Ready_Component()
{
	// Renderer
	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// Transform
	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// Shader
	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// VIBuffer
	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// Texture
	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("Prototype_Component_Texture_Letter"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkyrimUI_Letter::Bind_ShaderResources()
{
	/* ���̴� ���������� ������ �� ������ ����. */
	if (FAILED(m_pTransformCom->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CSkyrimUI_Letter* CSkyrimUI_Letter::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSkyrimUI_Letter* pInstance = new CSkyrimUI_Letter(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CSkyrimUI_Letter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSkyrimUI* CSkyrimUI_Letter::Clone(void* _pArg)
{
	CSkyrimUI_Letter* pInstance = new CSkyrimUI_Letter(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CSkyrimUI_Letter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkyrimUI_Letter::Free()
{
	__super::Free();
}
