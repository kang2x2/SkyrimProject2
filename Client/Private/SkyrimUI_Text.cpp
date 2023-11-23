#include "framework.h"
#include "SkyrimUI_Text.h"

#include "GameInstance.h"

CSkyrimUI_Text::CSkyrimUI_Text(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CSkyrimUI(_pDevice, _pContext)
{
}

CSkyrimUI_Text::CSkyrimUI_Text(const CSkyrimUI_Text& rhs)
	: CSkyrimUI(rhs)
{
}

HRESULT CSkyrimUI_Text::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CSkyrimUI_Text::Initialize_Clone(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	// *m_strText = (_tchar&)pArg;

	UI_TEXT_DESC textDesc = (UI_TEXT_DESC&)pArg;

	*m_strText = *textDesc.strText;
	m_strFontTag = textDesc.strFontTag;
	m_fSizeX = textDesc.fSizeX;
	m_fSizeY = textDesc.fSizeY;
	m_fX = textDesc.fX;
	m_fY = textDesc.fY;
	m_vColor = textDesc.vColor;

	m_pTransformCom->Set_Scaling(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(m_fSizeX, m_fSizeY, 0.f, 1.f));

	m_bHasMesh = false;
	m_strName = TEXT("UI_Text");

	return S_OK;
}

void CSkyrimUI_Text::Tick(_float _fTimeDelta)
{
}

void CSkyrimUI_Text::LateTick(_float _fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_UI, this);
}

HRESULT CSkyrimUI_Text::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Render_Font(m_strFontTag, m_strText, _float2(m_fX, m_fY), m_vColor);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CSkyrimUI_Text::Ready_Component()
{
	// Renderer
	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	// Shader
	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	// VIBuffer
	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	// Transform
	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkyrimUI_Text::Bind_ShaderResources()
{
	/* 셰이더 전역변수로 던져야 할 값들을 전달. */
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

CSkyrimUI_Text* CSkyrimUI_Text::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSkyrimUI_Text* pInstance = new CSkyrimUI_Text(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CSkyrimUI_Text");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSkyrimUI* CSkyrimUI_Text::Clone(void* pArg)
{
	CSkyrimUI_Text* pInstance = new CSkyrimUI_Text(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX("Fail Clone : CSkyrimUI_Text");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkyrimUI_Text::Free()
{
	__super::Free();
}
