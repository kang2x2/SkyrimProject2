#include "framework.h"
#include "Player_Body.h"

#include "GameInstance.h"
#include "Layer.h"

CPlayer_Body::CPlayer_Body(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPart_Base(_pDevice, _pContext)
{
}

CPlayer_Body::CPlayer_Body(const CPlayer_Body& rhs)
	: CPart_Base(rhs)
{
}

HRESULT CPlayer_Body::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CPlayer_Body::Initialize_Clone(void* _pArg)
{
	if (FAILED(__super::Initialize_Clone(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	// m_pTransformCom->Set_Scaling(_float3(0.0013f, 0.0013f, 0.0013f));
	m_strName = TEXT("Player_Body");

	return S_OK;
}

void CPlayer_Body::Tick(_float _fTimeDelta)
{
	m_pModelCom->Play_Animation(_fTimeDelta);

	Compute_RenderMatrix(m_pTransformCom->Get_WorldMatrix());

	/* aabb오리지널 바운딩 * 행렬을 해서 실제 충돌하기위한 데이터(aabb)에게 전달한다.*/
	m_pColliderCom->Update(XMLoadFloat4x4(&m_matWorld));
}

void CPlayer_Body::LateTick(_float _fTimeDelta)
{
#ifdef _DEBUG
	m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	map<const wstring, class CLayer*>* pLayerMapAry = pGameInstance->Get_CloneObjectMapAry(LEVEL_GAMEPLAY);

	for (auto Layer = pLayerMapAry->begin(); Layer != pLayerMapAry->end(); ++Layer)
	{
		list<CGameObject*> ltbjList = Layer->second->Get_ObjList();

		for (auto obj : ltbjList)
		{
			if (obj->Get_IsCreature())
			{
				pGameInstance->Collision_AABBTransition(m_pColliderCom,
					dynamic_cast<CCollider*>(obj->Get_Component(TEXT("Com_Collider_AABB"))));
			}
		}
	}


	Safe_Release(pGameInstance);
}

HRESULT CPlayer_Body::Render()
{
	if (FAILED(Bind_ShaderResource()))
		return E_FAIL;

	// 메시 몇개
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_BondMatrices(m_pShaderCom, i, "g_BoneMatrices")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CPlayer_Body::Set_AnimationIndex(_bool _bIsLoop, string _strAnimationName)
{
	m_pModelCom->SetUp_Animation(_bIsLoop, _strAnimationName);
}

_uint CPlayer_Body::Get_CurFrameIndex()
{
	return m_pModelCom->Get_CurFrameIndex();
}

_bool CPlayer_Body::Get_IsAnimationFin()
{
	return m_pModelCom->Get_IsAnimationFin();
}

_bool CPlayer_Body::Get_CurAnimationName(string _strAnimationName)
{
	if (!strcmp(m_pModelCom->Get_CurAnimationName().c_str(), _strAnimationName.c_str()))
		return true;

	return false;
}


HRESULT CPlayer_Body::Ready_Component()
{
	if (FAILED(__super::Add_CloneComponent(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Model_Player_Body"),
		TEXT("Com_3stModel"), (CComponent**)&m_p3stModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Model_Player_1stBody"),
		TEXT("Com_1stModel"), (CComponent**)&m_p1stModelCom)))
		return E_FAIL;

	m_pModelCom = m_p3stModelCom;

	if (FAILED(__super::Add_CloneComponent(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC AABBDesc = {};

	AABBDesc.vExtents = _float3(0.3f, 0.7f, 0.3f );
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_CloneComponent(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), (CComponent**)&m_pColliderCom, &AABBDesc)))
		return E_FAIL;

	m_pColliderCom->Set_OwnerObj(m_pParent);

	return S_OK;
}

HRESULT CPlayer_Body::Bind_ShaderResource()
{
	//if (FAILED(m_pTransformCom->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_matWorld)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 뷰, 투영 행렬과 카메라의 위치를 던져준다.
	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ViewMatrix", CPipeLine::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CPlayer_Body* CPlayer_Body::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CPlayer_Body* pInstance = new CPlayer_Body(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CPlayer_Body");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Body::Clone(void* _pArg)
{
	CPlayer_Body* pInstance = new CPlayer_Body(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CPlayer_Body");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Body::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pParentTransform);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);

	// 지울 때 누수나 에러 나는지 확인.
	if(m_p1stModelCom != nullptr)
		Safe_Release(m_p1stModelCom);
	if (m_p3stModelCom != nullptr)
		Safe_Release(m_p3stModelCom);

}
