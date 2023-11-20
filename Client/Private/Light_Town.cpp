#include "Light_Town.h"

#include "GameInstance.h"

CLight_Town::CLight_Town(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CLight_Town::CLight_Town(const CLight_Town& rhs)
	: CGameObject(rhs)
{
}

HRESULT CLight_Town::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CLight_Town::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), &TransformDesc))
		return E_FAIL;

	_matrix* pMatPivot = (_matrix*)pArg;

	m_pTransformCom->Set_WorldMatrix(*pMatPivot);

	m_strName = TEXT("Light_Fire");

	Ready_Light();

	return S_OK;
}

HRESULT CLight_Town::Initialize_Clone(_uint _iLevel, const wstring& _strModelComTag, void* pArg)
{
	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), &TransformDesc))
		return E_FAIL;

	_matrix* pMatPivot = (_matrix*)pArg;

	m_pTransformCom->Set_WorldMatrix(*pMatPivot);

	m_strName = TEXT("Light_Fire");

	Ready_Light();

	return S_OK;
}

void CLight_Town::PriorityTick(_float _fTimeDelta)
{
}

void CLight_Town::Tick(_float _fTimeDelta)
{
}

void CLight_Town::LateTick(_float _fTimeDelta)
{
}

HRESULT CLight_Town::Render()
{
	return S_OK;
}

HRESULT CLight_Town::Ready_Light()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	LIGHT_DESC LightDesc;

	/* �� ���� �߰� */
	ZeroMemory(&LightDesc, sizeof(LightDesc));
	LightDesc.eLightType = LIGHT_DESC::LIGHT_POINT;
	LightDesc.vLightPos = {};
	LightDesc.fLightRange = 25.f;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 0.7f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	
	XMStoreFloat4(&LightDesc.vLightPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (FAILED(pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);
	return S_OK;
}

CLight_Town* CLight_Town::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLight_Town* pInstance = new CLight_Town(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CLight_Town");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLight_Town::Clone(void* _pArg)
{
	CLight_Town* pInstance = new CLight_Town(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CLight_Town");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight_Town::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
}
