#include "framework.h"
#include "Building.h"


#include "GameInstance.h"

CBuilding::CBuilding(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CBuilding::CBuilding(const CBuilding& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBuilding::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CBuilding::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CBuilding::Initialize_Clone(_uint _iLevel, const wstring& _strModelComTag, void* pArg)
{
	_matrix* pMatPivot = (_matrix*)pArg;

	m_strModelComTag = _strModelComTag;

	if (FAILED(Ready_Component(_iLevel)))
		return E_FAIL;

	// 받아온 행렬의 정보를 저장 후 세팅
	//_float4x4 _matInit;
	//XMStoreFloat4x4(&_matInit, (*pMatPivot));
	//_float3 vScale = { _matInit._11, _matInit._22, _matInit._33 };
	//
	// _vector vPos = pMatPivot->r[3];
	//
	//m_pTransformCom->Set_Scaling(vScale);
	// m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->Set_WorldMatrix(*pMatPivot);
	// 메시의 정점 정보도 같이 업데이트 해준다.
	m_pModelCom->Update_VI(*pMatPivot);

	m_bHasMesh = true;
	m_strName = TEXT("SkyrimBuilding");

	return S_OK;
}

void CBuilding::Tick(_float _fTimeDelta)
{
}

void CBuilding::LateTick(_float _fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CBuilding::Render()
{
	if (FAILED(Bind_ShaderResource()))
		return E_FAIL;

	// 메시 몇개
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBuilding::Ready_Component(_uint _iLevel)
{
	if (FAILED(__super::Add_CloneComponent(_iLevel, m_strModelComTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Shader_VtxNonAnimMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBuilding::Bind_ShaderResource()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	// 뷰, 투영 행렬과 카메라의 위치를 던져준다.
	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ViewMatrix", CPipeLine::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;

	_float4 vCamPosition = pGameInstance->Get_CamPosition_Float4();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (pLightDesc == nullptr)
		return E_FAIL;

	_uint		iPassIndex = 0;

	if (pLightDesc->eLightType == LIGHT_DESC::LIGHT_DIRECTIONAL)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vLightDir, sizeof(_float4))))
			return E_FAIL;
		iPassIndex = 0;
	}
	else if (pLightDesc->eLightType == LIGHT_DESC::LIGHT_POINT)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightPos", &pLightDesc->vLightPos, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fLightRange", &pLightDesc->fLightRange, sizeof(_float))))
			return E_FAIL;
		iPassIndex = 1;
	}

	// 나머지 조명 연산에 필요한 데이터를 던져 줌
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	m_pShaderCom->Begin(iPassIndex);

	return S_OK;

}

CBuilding* CBuilding::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBuilding* pInstance = new CBuilding(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CBuilding ProtoType");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBuilding::Clone(void* _pArg)
{
	CBuilding* pInstance = new CBuilding(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CBuilding Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBuilding::Clone(_uint _iLevel, const wstring& _strModelComTag, void* _pArg)
{
	CBuilding* pInstance = new CBuilding(*this);

	if (FAILED(pInstance->Initialize_Clone(_iLevel, _strModelComTag, _pArg)))
	{
		MSG_BOX("Fail Clone : CBuilding Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBuilding::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
