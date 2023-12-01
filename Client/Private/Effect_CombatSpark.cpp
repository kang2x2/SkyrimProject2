#include "framework.h"
#include "Effect_CombatSpark.h"

#include "GameInstance.h"

#include "Player.h"

CEffect_CombatSpark::CEffect_CombatSpark(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CEffect_CombatSpark::CEffect_CombatSpark(const CEffect_CombatSpark& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_CombatSpark::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CEffect_CombatSpark::Initialize_Clone(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	_vector vPos = *(_vector*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	return S_OK;
}

HRESULT CEffect_CombatSpark::Initialize_Clone(_uint _iLevel, const wstring& _strModelComTag, void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

void CEffect_CombatSpark::PriorityTick(_float _fTimeDelta)
{

}

void CEffect_CombatSpark::Tick(_float _fTimeDelta)
{
	m_fDeleteTime += _fTimeDelta;

	if (m_fDeleteTime >= 0.8f)
		m_bReadyDead = true;

	m_pVIBufferCom->Update_Spark(_fTimeDelta);
}

void CEffect_CombatSpark::LateTick(_float _fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CEffect_CombatSpark::Render()
{
	if (FAILED(Bind_ShaderResource()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CEffect_CombatSpark::Ready_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("ProtoType_Component_Shader_Rect_Instance"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("ProtoType_Component_VIBuffer_Rect_Instance"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* Com_Texture*/
	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("Prototype_Component_Texture_SparkEffect"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_CombatSpark::Bind_ShaderResource()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ViewMatrix", CPipeLine::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	return S_OK;
}

CEffect_CombatSpark* CEffect_CombatSpark::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CEffect_CombatSpark* pInstance = new CEffect_CombatSpark(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CEffect_CombatSpark");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_CombatSpark::Clone(void* _pArg)
{
	CEffect_CombatSpark* pInstance = new CEffect_CombatSpark(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CEffect_CombatSpark");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_CombatSpark::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
