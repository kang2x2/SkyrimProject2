#include "framework.h"
#include "Inventory_ItemCategory.h"

#include "GameInstance.h"

CInventory_ItemCategory::CInventory_ItemCategory(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CInventory_PartBase(_pDevice, _pContext)
{
}

CInventory_ItemCategory::CInventory_ItemCategory(const CInventory_ItemCategory& rhs)
	: CInventory_PartBase(rhs)
{
}

HRESULT CInventory_ItemCategory::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CInventory_ItemCategory::Initialize_Clone(void* _pArg)
{
	if (FAILED(__super::Initialize_Clone(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_strName = TEXT("Inventory_Category");

	return S_OK;
}

void CInventory_ItemCategory::PriorityTick(_float _fTimeDelta)
{
}

void CInventory_ItemCategory::Tick(_float _fTimeDelta)
{
}

void CInventory_ItemCategory::LateTick(_float _fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_UI, this);
}

HRESULT CInventory_ItemCategory::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	__super::Render();

	return S_OK;
}

HRESULT CInventory_ItemCategory::Ready_Component()
{
	return S_OK;
}

HRESULT CInventory_ItemCategory::Bind_ShaderResources()
{
	if(FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

CInventory_ItemCategory* CInventory_ItemCategory::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CInventory_ItemCategory* pInstance = new CInventory_ItemCategory(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CInventory_ItemCategory");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInventory_ItemCategory::Clone(void* _pArg)
{
	CInventory_ItemCategory* pInstance = new CInventory_ItemCategory(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone: CInventory_ItemCategory");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInventory_ItemCategory::Free()
{
	__super::Free();
}
