#include "framework.h"
#include "Player_Weapon.h"

#include "GameInstance.h" 
#include "Bone.h"

#include "Weapon_IronSword.h"

CPlayer_Weapon::CPlayer_Weapon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPart_Base(_pDevice, _pContext)
{
}

CPlayer_Weapon::CPlayer_Weapon(const CPlayer_Weapon& rhs)
	: CPart_Base(rhs)
{
}

HRESULT CPlayer_Weapon::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CPlayer_Weapon::Initialize_Clone(void* _pArg)
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

	m_pTransformCom->Set_Scaling(_float3(0.01f, 0.01f, 0.01f));
	m_pTransformCom->Fix_Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.0f));
	// matInitialize = XMMatrixRotationY(XMConvertToRadians(-90.f));
	m_strName = TEXT("PlayerWeapon");

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pWeapon = pGameInstance->Find_CloneObject(LEVEL_WHITERUN, TEXT("Layer_Equip"), TEXT("IronSword"));

	Safe_Release(pGameInstance);

	return S_OK;
}

void CPlayer_Weapon::Tick(_float _fTimeDelta)
{
	/* �� ��� * (��Ĺ ���� �Ĺ��ε� ��� * ��Ĺ�� ��� * ��Ʈ�� ���� ���) */
	_float4x4 matSocketCombined = m_pSocketBone->Get_CombinedTransformationMatrix();

	_matrix		WorldMatrix = XMLoadFloat4x4(&matSocketCombined) *
		XMLoadFloat4x4(&m_matSocketPivot);

	WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
	WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
	WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);

	Compute_RenderMatrix(m_pTransformCom->Get_WorldMatrix() * WorldMatrix);
	dynamic_cast<CTransform*>(m_pWeapon->Get_Component(TEXT("Com_Transform")))->Set_WorldMatrix(XMLoadFloat4x4(&m_matWorld));
}

void CPlayer_Weapon::LateTick(_float _fTimeDelta)
{
	m_pWeapon->LateTick(_fTimeDelta);
}

HRESULT CPlayer_Weapon::Render()
{
	if (m_pWeapon != nullptr)
		m_pWeapon->Render();

	return S_OK;
}

void CPlayer_Weapon::Set_SoketBone(CBone* _pBone)
{
	Safe_Release(m_pSocketBone);
	m_pSocketBone = _pBone;
	Safe_AddRef(_pBone);
}

const char* CPlayer_Weapon::Get_SoketBoneName()
{
	return m_pSocketBone->Get_BoneName();
}

HRESULT CPlayer_Weapon::Ready_Component()
{
	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Weapon::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_matWorld)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	// ��, ���� ��İ� ī�޶��� ��ġ�� �����ش�.
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

	// ������ ���� ���꿡 �ʿ��� �����͸� ���� ��
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;

}

CPlayer_Weapon* CPlayer_Weapon::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CPlayer_Weapon* pInstance = new CPlayer_Weapon(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CPlayer_Weapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Weapon::Clone(void* _pArg)
{
	CPlayer_Weapon* pInstance = new CPlayer_Weapon(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CPlayer_Weapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Weapon::Free()
{
	__super::Free();

	Safe_Release(m_pSocketBone);
	Safe_Release(m_pParentTransform);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
