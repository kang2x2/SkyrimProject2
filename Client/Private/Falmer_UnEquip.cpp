#include "framework.h"
#include "Falmer_UnEquip.h"

#include "GameInstance.h"

#include "StateManager_FalmerUE.h"

CFalmer_UnEquip::CFalmer_UnEquip(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CMonster(_pDevice, _pContext)
{
	// state manager 만들자. 그리고 로더에 원본 추가하자.
}

CFalmer_UnEquip::CFalmer_UnEquip(const CFalmer_UnEquip& rhs)
	: CMonster(rhs)
{
}

HRESULT CFalmer_UnEquip::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CFalmer_UnEquip::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CFalmer_UnEquip::Initialize_Clone(_uint _iLevel, const wstring& _strModelComTag, void* pArg)
{
	pMatPivot = (_matrix*)pArg;

	m_strModelComTag = _strModelComTag;

	if (FAILED(Ready_Component(_iLevel)))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	m_bHasMesh = true;
	m_bCreature = true;
	m_strName = TEXT("Falmer_UnEquip");

	m_pTransformCom->Set_Speed(5.f);

	Play_Animation(true, "mt_idle");

	return S_OK;

}

void CFalmer_UnEquip::PriorityTick(_float _fTimeDelta)
{
}

void CFalmer_UnEquip::Tick(_float _fTimeDelta)
{
	m_pModelCom->Play_Animation(_fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_pNavigationCom != nullptr)
	{
		_vector	vPosition = m_pNavigationCom->Set_OnCell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}

	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	m_pColliderCom->Update(matWorld);

	Safe_Release(pGameInstance);
}

void CFalmer_UnEquip::LateTick(_float _fTimeDelta)
{
	m_pStateManager->Late_Update();

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	__super::LateTick(_fTimeDelta);
}

HRESULT CFalmer_UnEquip::Render()
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

#ifdef _DEBUG
	/* 콜라이더를 그 때도 오리지널을 그리는 게 아니라 행렬을 곱해놓은 aabb를 그린다. */
	if (m_pColliderCom != nullptr)
		m_pColliderCom->Render();
#endif

	return S_OK;

}

HRESULT CFalmer_UnEquip::Set_State(SKEEVERSTATE _eState)
{
	m_pStateManager->Set_State(_eState);

	return S_OK;
}

void CFalmer_UnEquip::Play_Animation(_bool _bIsLoop, string _strAnimationName)
{
	Set_AnimationIndex(_bIsLoop, _strAnimationName);
}

void CFalmer_UnEquip::Set_AnimationIndex(_bool _bIsLoop, string _strAnimationName)
{
	m_pModelCom->SetUp_Animation(_bIsLoop, _strAnimationName);
}

HRESULT CFalmer_UnEquip::Ready_Component(_uint _iLevel)
{
	if (FAILED(__super::Add_CloneComponent(_iLevel, m_strModelComTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	__super::Ready_Component();

	if (_iLevel != LEVEL_TOOL)
	{
		/* Com_Navigation */
		CNavigation::DESC_NAVIGATION		NavigationDesc;
		NavigationDesc.iCurIndex = 0;

		if (FAILED(__super::Add_CloneComponent(LEVEL_WHITERUN, TEXT("ProtoType_Component_Navigation"),
			TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NavigationDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CFalmer_UnEquip::Ready_State()
{
	m_pStateManager = CStateManager_FalmerUE::Create(this, m_pTransformCom, m_pNavigationCom);

	return S_OK;
}

HRESULT CFalmer_UnEquip::Ready_Cell()
{
	return S_OK;
}

CFalmer_UnEquip* CFalmer_UnEquip::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CFalmer_UnEquip* pInstance = new CFalmer_UnEquip(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CFalmer_UnEquip");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFalmer_UnEquip::Clone(void* _pArg)
{
	CFalmer_UnEquip* pInstance = new CFalmer_UnEquip(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CFalmer_UnEquip");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFalmer_UnEquip::Clone(_uint _iLevel, const wstring& _strModelComTag, void* _pArg)
{
	CFalmer_UnEquip* pInstance = new CFalmer_UnEquip(*this);

	if (FAILED(pInstance->Initialize_Clone(_iLevel, _strModelComTag, _pArg)))
	{
		MSG_BOX("Fail Clone : CFalmer_UnEquip");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFalmer_UnEquip::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pNavigationCom);

}
