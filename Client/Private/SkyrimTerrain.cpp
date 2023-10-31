#include "framework.h"
#include "SkyrimTerrain.h"

#include "GameInstance.h"
#include "Mesh.h"

CSkyrimTerrain::CSkyrimTerrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CSkyrimTerrain::CSkyrimTerrain(const CSkyrimTerrain& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkyrimTerrain::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CSkyrimTerrain::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CSkyrimTerrain::Initialize_Clone(_uint _iLevel, const wstring& _strModelComTag, void* pArg)
{
	pMatPivot = (_matrix*)pArg;

	m_strModelComTag = _strModelComTag;

	if (FAILED(Ready_Component(_iLevel)))
		return E_FAIL;

	m_bHasMesh = true;
	m_strName = TEXT("SkyrimTerrain");

	return S_OK;
}

void CSkyrimTerrain::PriorityTick(_float _fTimeDelta)
{
	// m_pNavigationCom->Update();
}

void CSkyrimTerrain::Tick(_float _fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CSkyrimTerrain::LateTick(_float _fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CSkyrimTerrain::Render()
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

#ifdef _DEBUG
	// m_pColliderCom->Render();
#endif

	return S_OK;
}

HRESULT CSkyrimTerrain::Ready_Component(_uint _iLevel)
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

	m_pTransformCom->Set_WorldMatrix(*pMatPivot);

	CBounding_AABB::BOUNDING_AABB_DESC AABBDesc = {};

	AABBDesc.vExtents = _float3(0.5f, 1.5f, 2.0f);
	AABBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_CloneComponent(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), (CComponent**)&m_pColliderCom, &AABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkyrimTerrain::Ready_Cell()
{
	//vector<CMesh*> vecMesh = m_pModelCom->Get_VecMesh();
	//
	//for (size_t i = 0; i < vecMesh.size(); ++i)
	//{
	//	// 정점과 인덱스 정보를 얻어옴.
	//	const _float3* pPos = vecMesh[i]->Get_VtxPos();
	//	const vector<_ulong>& indices = vecMesh[i]->Get_Indices();
	//
	//	// 메시의 각 삼각형에 대해 교차 검사 수행
	//	for (size_t idx = 0; idx < indices.size(); idx += 3)
	//	{
	//		_float3 vIntersectionPos;
	//
	//		_ulong idx0 = indices[idx];
	//		_ulong idx1 = indices[idx + 1];
	//		_ulong idx2 = indices[idx + 2];
	//
	//		// 각 삼각형을 구성하는 세 개의 정점을 가져옴
	//		_float3 vertex0 = pPos[idx0];
	//		_float3 vertex1 = pPos[idx1];
	//		_float3 vertex2 = pPos[idx2];
	//
	//		_float3 vectex[3];
	//		vectex[0] = pPos[idx0];
	//		vectex[1] = pPos[idx1];
	//		vectex[2] = pPos[idx2];
	//
	//		m_pNavigationCom->Add_Cell(vectex);
	//	}
	//}

	return S_OK;
}

HRESULT CSkyrimTerrain::Bind_ShaderResource()
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

CSkyrimTerrain* CSkyrimTerrain::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSkyrimTerrain* pInstance = new CSkyrimTerrain(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CSkyrimTerrain ProtoType");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkyrimTerrain::Clone(void* _pArg)
{
	CSkyrimTerrain* pInstance = new CSkyrimTerrain(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CSkyrimTerrain Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkyrimTerrain::Clone(_uint _iLevel, const wstring& _strModelComTag, void* _pArg)
{
	CSkyrimTerrain* pInstance = new CSkyrimTerrain(*this);
	if (FAILED(pInstance->Initialize_Clone(_iLevel, _strModelComTag, _pArg)))
	{
		MSG_BOX("Fail Clone : CSkyrimTerrain Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkyrimTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pNavigationCom);
}
