#include "framework.h"
#include "Monster.h"

#include "GameInstance.h"

#include "Player.h"

CMonster::CMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CMonster::CMonster(const CMonster& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonster::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CMonster::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CMonster::Initialize_Clone(_uint _iLevel, const wstring& _strModelComTag, void* pArg)
{
	return S_OK;
}

void CMonster::PriorityTick(_float _fTimeDelta)
{
}

void CMonster::Tick(_float _fTimeDelta)
{
}

void CMonster::LateTick(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>
		(pGameInstance->Find_CloneObject(LEVEL_WHITERUN, TEXT("Layer_Player"), TEXT("Player")));
	//
	//pGameInstance->Collision_AABBTransition(m_pColliderCom, dynamic_cast<CCollider*>(pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB"))));

	m_pColliderCom->IsCollision(dynamic_cast<CCollider*>(pPlayer->Get_Part(CPlayer::PART_BODY)->Get_Component(TEXT("Com_Collider_AABB"))));

	Safe_Release(pGameInstance);
}

HRESULT CMonster::Render()
{
	return S_OK;
}

HRESULT CMonster::Bind_ShaderResource()
{
	//if (FAILED(m_pTransformCom->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

	_float4x4 matWorld = m_pTransformCom->Get_WorldMatrix_Float4x4();
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &matWorld)))
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

	return S_OK;
}

HRESULT CMonster::Ready_Component()
{
	if (FAILED(__super::Add_CloneComponent(LEVEL_WHITERUN, TEXT("ProtoType_Component_Shader_VtxAnimMesh"),
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

	AABBDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_CloneComponent(LEVEL_WHITERUN, TEXT("ProtoType_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), (CComponent**)&m_pColliderCom, &AABBDesc)))
		return E_FAIL;

	m_pColliderCom->Set_OwnerObj(this);

	return S_OK;
}

void CMonster::Free()
{
	__super::Free();
}
