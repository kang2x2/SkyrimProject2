#include "framework.h"
#include "BackGround.h"

#include "GameInstance.h"

CBackGround::CBackGround(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CBackGround::CBackGround(const CGameObject& rhs)
	: CGameObject(rhs)
{

}

HRESULT CBackGround::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CBackGround::Initialize_Clone(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.5f;

	m_pTransformCom->Set_Scaling(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	m_bHasMesh = false;
	m_strName = TEXT("Logo_BackGround");

	return S_OK;
}

void CBackGround::Tick(_float _fTimeDelta)
{
}

void CBackGround::LateTick(_float _fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_UI_0, this);
}

HRESULT CBackGround::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CBackGround::Ready_Component()
{
	/* �θ�� ������Ʈ�� �Ű��� ���� ������ ��ƿ�. */
	
	// Renderer
	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// Shader
	if(FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// VIBuffer
	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// Texture
	if (FAILED(__super::Add_CloneComponent(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// Transform
	CTransform::TRANSFORM_DESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationRadianPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CBackGround::Bind_ShaderResources()
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
	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_Textures")))
		return E_FAIL;

	return S_OK;
}

CBackGround* CBackGround::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBackGround* pInstance = new CBackGround(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CBackGround_ProtoType");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBackGround::Clone(void* pArg)
{
	CBackGround* pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX("Fail Create : CBackGround_Clone");
		Safe_Release(pInstance);
	}

	return pInstance;

}

void CBackGround::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
}
