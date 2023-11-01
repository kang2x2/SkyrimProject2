#include "framework.h"
#include "Navigation_Client.h"

#include "Renderer.h"
#include "Navigation.h" 

CNavigation_Client::CNavigation_Client(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CNavigation_Client::CNavigation_Client(const CNavigation_Client& rhs)
	: CGameObject(rhs)
{
}

HRESULT CNavigation_Client::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CNavigation_Client::Initialize_Clone(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNavigation_Client::Initialize_Clone(_uint _iLevel, const wstring& _strModelComTag, void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

void CNavigation_Client::PriorityTick(_float _fTimeDelta)
{
	m_pNavigationCom->Update();
}

void CNavigation_Client::Tick(_float _fTimeDelta)
{
}

void CNavigation_Client::LateTick(_float _fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);
}

HRESULT CNavigation_Client::Render()
{
#ifdef _DEBUG
	m_pNavigationCom->Render();
#endif

	return S_OK;
}

HRESULT CNavigation_Client::Ready_Component()
{
	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_WHITERUN, TEXT("ProtoType_Component_Navigation"),
		TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom)))
			return E_FAIL;

	return S_OK;
}

CNavigation_Client* CNavigation_Client::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CNavigation_Client* pInstance = new CNavigation_Client(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CNavigation_Client");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNavigation_Client::Clone(void* _pArg)
{
	CNavigation_Client* pInstance = new CNavigation_Client(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CNavigation_Client");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation_Client::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pNavigationCom);
}
