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

	// �޽� �
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

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

	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("ProtoType_Component_Shader_VtxNonAnimMesh"),
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

	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("ProtoType_Component_Collider_AABB"),
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
	//	// ������ �ε��� ������ ����.
	//	const _float3* pPos = vecMesh[i]->Get_VtxPos();
	//	const vector<_ulong>& indices = vecMesh[i]->Get_Indices();
	//
	//	// �޽��� �� �ﰢ���� ���� ���� �˻� ����
	//	for (size_t idx = 0; idx < indices.size(); idx += 3)
	//	{
	//		_float3 vIntersectionPos;
	//
	//		_ulong idx0 = indices[idx];
	//		_ulong idx1 = indices[idx + 1];
	//		_ulong idx2 = indices[idx + 2];
	//
	//		// �� �ﰢ���� �����ϴ� �� ���� ������ ������
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
	// ��, ���� ����� �����ش�.
	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ViewMatrix", CPipeLine::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;

	Safe_Release(pGameInstance);

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
