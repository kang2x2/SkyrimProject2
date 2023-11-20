#include "framework.h"
#include "FalmerUE_Weapon.h"

#include "GameInstance.h"

#include "Bone.h"
#include "Player.h"

CFalmerUE_Weapon::CFalmerUE_Weapon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:	CFalmerUEPart_Base(_pDevice, _pContext)
{
}

CFalmerUE_Weapon::CFalmerUE_Weapon(const CFalmerUE_Weapon& rhs)
	:	CFalmerUEPart_Base(rhs)
{
}

HRESULT CFalmerUE_Weapon::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CFalmerUE_Weapon::Initialize_Clone(void* _pArg)
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

	m_strName = TEXT("FlamerUE_Weapon");

	return S_OK;

}

void CFalmerUE_Weapon::Tick(_float _fTimeDelta)
{
	/* �� ��� * (��Ĺ ���� �Ĺ��ε� ��� * ��Ĺ�� ��� * ��Ʈ�� ���� ���) */
	_float4x4 matSocketCombined = m_pSocketBone->Get_CombinedTransformationMatrix();

	_matrix		WorldMatrix = XMLoadFloat4x4(&matSocketCombined) *
		XMLoadFloat4x4(&m_matSocketPivot);

	WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
	WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
	WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);

	Compute_RenderMatrix(m_pTransformCom->Get_WorldMatrix() * WorldMatrix);

	m_pColliderCom->Update(XMLoadFloat4x4(&m_matWorld));

}

void CFalmerUE_Weapon::LateTick(_float _fTimeDelta)
{
#ifdef _DEBUG
	m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CFalmerUE_Weapon::Render()
{
	return S_OK;
}

const char* CFalmerUE_Weapon::Get_SoketBoneName()
{
	return m_pSocketBone->Get_BoneName();
}

HRESULT CFalmerUE_Weapon::Ready_Component()
{
	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	CBounding_OBB::BOUNDING_OBB_DESC OBBDesc = {};

	OBBDesc.vExtents = _float3(10.3f, 10.7f, 10.3f);
	OBBDesc.vDegree = _float3(0.f, 0.f, 0.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("ProtoType_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &OBBDesc)))
		return E_FAIL;

	m_pColliderCom->Set_OwnerObj(m_pParent);

	return S_OK;

}

CFalmerUE_Weapon* CFalmerUE_Weapon::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CFalmerUE_Weapon* pInstance = new CFalmerUE_Weapon(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CFalmerUE_Weapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFalmerUE_Weapon::Clone(void* _pArg)
{
	CFalmerUE_Weapon* pInstance = new CFalmerUE_Weapon(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CFalmerUE_Weapon");
		Safe_Release(pInstance);
	}

	return pInstance;

}

void CFalmerUE_Weapon::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pSocketBone);
}
