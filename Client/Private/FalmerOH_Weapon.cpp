#include "framework.h"
#include "FalmerOH_Weapon.h"

#include "GameInstance.h"

#include "Bone.h"
#include "Player.h"

CFalmerOH_Weapon::CFalmerOH_Weapon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CFalmerOHPart_Base(_pDevice, _pContext)
{
}

CFalmerOH_Weapon::CFalmerOH_Weapon(const CFalmerOH_Weapon& rhs)
	: CFalmerOHPart_Base(rhs)
{
}

HRESULT CFalmerOH_Weapon::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CFalmerOH_Weapon::Initialize_Clone(void* _pArg)
{
	if (nullptr != _pArg)
	{
		WEAPON_DESC* pWeaponDesc = (WEAPON_DESC*)_pArg;

		m_pSocketBone = pWeaponDesc->pSocketBone;
		Safe_AddRef(m_pSocketBone);

		m_matSocketPivot = pWeaponDesc->matSocketPivot;

		if (FAILED(__super::Initialize_Clone(_pArg)))
			return E_FAIL;
	}

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scaling(_float3(0.0128f, 0.0128f, 0.0128f));
	m_pTransformCom->Fix_Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-105.0f));
	_float4 vInitPos;
	XMStoreFloat4(&vInitPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	vInitPos.x -= 0.05f;
	vInitPos.y -= 0.03f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vInitPos));

	m_strName = TEXT("FlamerOH_Weapon");

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pWeapon = pGameInstance->Find_CloneObject(g_curLevel, TEXT("Layer_Equip"), TEXT("FalmerAxe"));

	Safe_Release(pGameInstance);

	return S_OK;
}

void CFalmerOH_Weapon::Tick(_float _fTimeDelta)
{	/* 내 행렬 * (소캣 뼈의 컴바인드 행렬 * 소캣의 행렬 * 페어런트의 월드 행렬) */
	_float4x4 matSocketCombined = m_pSocketBone->Get_CombinedTransformationMatrix();

	_matrix		WorldMatrix = XMLoadFloat4x4(&matSocketCombined) *
		XMLoadFloat4x4(&m_matSocketPivot);

	WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
	WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
	WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);

	Compute_RenderMatrix(m_pTransformCom->Get_WorldMatrix() * WorldMatrix);

	dynamic_cast<CTransform*>(m_pWeapon->Get_Component(TEXT("Com_Transform")))->Set_WorldMatrix(XMLoadFloat4x4(&m_matWorld));

	m_pColliderCom->Update(XMLoadFloat4x4(&m_matWorld));
}

void CFalmerOH_Weapon::LateTick(_float _fTimeDelta)
{
#ifdef _DEBUG
	m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CFalmerOH_Weapon::Render()
{
	return S_OK;
}

const char* CFalmerOH_Weapon::Get_SoketBoneName()
{
	return m_pSocketBone->Get_BoneName();
}

void CFalmerOH_Weapon::Set_SoketBone(CBone* _pSocketBone)
{
	Safe_Release(m_pSocketBone);
	m_pSocketBone = _pSocketBone;
	Safe_AddRef(_pSocketBone);
}

HRESULT CFalmerOH_Weapon::Ready_Component()
{
	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	CBounding_OBB::BOUNDING_OBB_DESC OBBDesc = {};

	OBBDesc.vExtents = _float3(7.f, 7.f, 25.f);
	OBBDesc.vDegree = _float3(0.f, 0.f, 0.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, OBBDesc.vExtents.z / 2.f);

	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("ProtoType_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &OBBDesc)))
		return E_FAIL;

	m_pColliderCom->Set_OwnerObj(m_pParent);

	return S_OK;
}

CFalmerOH_Weapon* CFalmerOH_Weapon::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CFalmerOH_Weapon* pInstance = new CFalmerOH_Weapon(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CFalmerOH_Weapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFalmerOH_Weapon::Clone(void* _pArg)
{
	CFalmerOH_Weapon* pInstance = new CFalmerOH_Weapon(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CFalmerOH_Weapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFalmerOH_Weapon::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pSocketBone);

}
