#include "framework.h"
#include "Skyrim_NPC.h"

#include "GameInstance.h"

#include "Player.h"

CSkyrim_NPC::CSkyrim_NPC(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCreatureObject(_pDevice, _pContext)
{
}

CSkyrim_NPC::CSkyrim_NPC(const CSkyrim_NPC& rhs)
	: CCreatureObject(rhs)
{
}

HRESULT CSkyrim_NPC::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CSkyrim_NPC::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CSkyrim_NPC::Initialize_Clone(_uint _iLevel, const wstring& _strModelComTag, void* pArg)
{
	return S_OK;
}

void CSkyrim_NPC::PriorityTick(_float _fTimeDelta)
{
}

void CSkyrim_NPC::Tick(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_pNavigationCom != nullptr)
	{
		_vector	vPosition = m_pNavigationCom->Set_OnCell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}

	Safe_Release(pGameInstance);
}

void CSkyrim_NPC::LateTick(_float _fTimeDelta)
{
	if (m_iHp <= 0)
		m_bDead = true;
}

HRESULT CSkyrim_NPC::Render()
{
#ifdef _DEBUG
	if (m_pNavigationCom != nullptr)
		m_pNavigationCom->Render();
#endif

	return S_OK;
}

HRESULT CSkyrim_NPC::Ready_Component()
{
	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("ProtoType_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if(pMatPivot != nullptr)
		m_pTransformCom->Set_WorldMatrix(*pMatPivot);

	if (g_curLevel != LEVEL_TOOL)
	{
		/* Com_Navigation */
		CNavigation::DESC_NAVIGATION		NavigationDesc;
		NavigationDesc.iCurIndex = 0;

		if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("ProtoType_Component_Navigation_WhiteRun"),
			TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NavigationDesc)))
			return E_FAIL;
	}

	return S_OK;
}

void CSkyrim_NPC::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pNavigationCom);
}
