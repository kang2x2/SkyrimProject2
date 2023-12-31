#include "framework.h"
#include "TorsoM_Blades.h"

#include "GameInstance.h"

CTorsoM_Blades::CTorsoM_Blades(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CSkyrimArmor(_pDevice, _pContext)
{
}

CTorsoM_Blades::CTorsoM_Blades(const CTorsoM_Blades& rhs)
	: CSkyrimArmor(rhs)
{
}

HRESULT CTorsoM_Blades::Initialize_ProtoType()
{
	__super::Initialize_ProtoType();

	return S_OK;
}

HRESULT CTorsoM_Blades::Initialize_Clone(void* _pArg)
{
	__super::Initialize_Clone(_pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scaling(_float3(0.01f, 0.01f, 0.01f));
	// m_pTransformCom->Fix_Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-90.0f));
	// matInitialize = XMMatrixRotationY(XMConvertToRadians(-90.f));
	m_strName = TEXT("블레이즈 아머(남)");
	m_ePartType = ARMOR_TORSO;

	return S_OK;
}

void CTorsoM_Blades::Tick(_float _fTimeDelta)
{
}

void CTorsoM_Blades::LateTick(_float _fTimeDelta)
{
	__super::LateTick(_fTimeDelta);
}

HRESULT CTorsoM_Blades::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CTorsoM_Blades::Ready_Component()
{
	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("ProtoType_Component_Model_TorsoM_Blades"),
		TEXT("Com_3stModel"), (CComponent**)&m_pModelComAry[VIEW_3ST])))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("ProtoType_Component_Model_1stTorsoM_Blades"),
		TEXT("Com_1stModel"), (CComponent**)&m_pModelComAry[VIEW_1ST])))
		return E_FAIL;

	__super::Ready_Component();

	return S_OK;
}

CTorsoM_Blades* CTorsoM_Blades::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CTorsoM_Blades* pInstance = new CTorsoM_Blades(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CTorsoM_Blades");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTorsoM_Blades::Clone(void* _pArg)
{
	CTorsoM_Blades* pInstance = new CTorsoM_Blades(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CTorsoM_Blades");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTorsoM_Blades::Free()
{
	__super::Free();
}

