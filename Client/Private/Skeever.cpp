#include "framework.h"
#include "Skeever.h"

#include "GameInstance.h"

#include "StateManager_Skeever.h"


CSkeever::CSkeever(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CMonster(_pDevice, _pContext)
{
}

CSkeever::CSkeever(const CSkeever& rhs)
	: CMonster(rhs)
{
}

HRESULT CSkeever::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CSkeever::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CSkeever::Initialize_Clone(_uint _iLevel, const wstring& _strModelComTag, void* pArg)
{
	pMatPivot = (_matrix*)pArg;

	m_strModelComTag = _strModelComTag;

	if (FAILED(Ready_Component(_iLevel)))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	m_bHasMesh = true;
	m_bCreature = true;
	m_strName = TEXT("Skeever");

	// m_pTransformCom->Set_Speed(5.f);

	Play_Animation(true, "mtidle");

	return S_OK;

}

void CSkeever::PriorityTick(_float _fTimeDelta)
{
}

void CSkeever::Tick(_float _fTimeDelta)
{
	m_pModelCom->Play_Animation(_fTimeDelta);

	__super::Tick(_fTimeDelta);
}

void CSkeever::LateTick(_float _fTimeDelta)
{
	m_pStateManager->Late_Update();

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	__super::LateTick(_fTimeDelta);
}

HRESULT CSkeever::Render()
{
	__super::Bind_ShaderResource();

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

	__super::Render();

	return S_OK;

}

HRESULT CSkeever::Set_State(SKEEVER_STATE _eState)
{
	m_pStateManager->Set_State(_eState);

	return S_OK;
}

void CSkeever::Play_Animation(_bool _bIsLoop, string _strAnimationName)
{
	Set_AnimationIndex(_bIsLoop, _strAnimationName);
}

void CSkeever::Set_AnimationIndex(_bool _bIsLoop, string _strAnimationName)
{
	m_pModelCom->SetUp_Animation(_bIsLoop, _strAnimationName);
}

HRESULT CSkeever::Ready_Component(_uint _iLevel)
{
	if (FAILED(__super::Add_CloneComponent(_iLevel, m_strModelComTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	__super::Ready_Component();

	return S_OK;
}

HRESULT CSkeever::Ready_State()
{
	m_pStateManager = CStateManager_Skeever::Create(this, m_pTransformCom, m_pNavigationCom);

	return S_OK;
}

HRESULT CSkeever::Ready_Cell()
{
	return S_OK;
}

CSkeever* CSkeever::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSkeever* pInstance = new CSkeever(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CSkeever");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkeever::Clone(void* _pArg)
{
	CSkeever* pInstance = new CSkeever(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CSkeever");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkeever::Clone(_uint _iLevel, const wstring& _strModelComTag, void* _pArg)
{
	CSkeever* pInstance = new CSkeever(*this);

	if (FAILED(pInstance->Initialize_Clone(_iLevel, _strModelComTag, _pArg)))
	{
		MSG_BOX("Fail Clone : CSkeever");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkeever::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
}
