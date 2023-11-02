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
	XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());

	pMatPivot = (_matrix*)pArg;

	XMStoreFloat4x4(&m_matWorld, *pMatPivot);

	m_strModelComTag = _strModelComTag;

	if (FAILED(Ready_Component(_iLevel)))
		return E_FAIL;

	if (FAILED(Ready_State()))
		return E_FAIL;

	m_bHasMesh = true;
	m_strName = TEXT("Skeever");

	m_pTransformCom->Set_Speed(5.f);

	Play_Animation(true, "mt_idle");

	return S_OK;

}

void CSkeever::PriorityTick(_float _fTimeDelta)
{
}

void CSkeever::Tick(_float _fTimeDelta)
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

void CSkeever::LateTick(_float _fTimeDelta)
{
	m_pStateManager->Late_Update();

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CSkeever::Render()
{
	if (FAILED(Bind_ShaderResource()))
		return E_FAIL;

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
	if(m_pColliderCom != nullptr)
		m_pColliderCom->Render();
#endif

	return S_OK;

}

HRESULT CSkeever::Set_State(SKEEVERSTATE _eState)
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

	if (FAILED(__super::Add_CloneComponent(LEVEL_WHITERUN, TEXT("ProtoType_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(*pMatPivot);

	if (_iLevel != LEVEL_TOOL)
	{
		CBounding_AABB::BOUNDING_AABB_DESC AABBDesc = {};

		AABBDesc.vExtents = _float3(0.5f, 1.5f, 2.0f);
		AABBDesc.vCenter = _float3(0.f, 0.f, 0.f);

		if (FAILED(__super::Add_CloneComponent(LEVEL_WHITERUN, TEXT("ProtoType_Component_Collider_AABB"),
			TEXT("Com_Collider_AABB"), (CComponent**)&m_pColliderCom, &AABBDesc)))
			return E_FAIL;

		/* Com_Navigation */
		CNavigation::DESC_NAVIGATION		NavigationDesc;
		NavigationDesc.iCurIndex = 0;

		if (FAILED(__super::Add_CloneComponent(LEVEL_WHITERUN, TEXT("ProtoType_Component_Navigation"),
			TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NavigationDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CSkeever::Ready_State()
{
	// m_pStateManager = CStateManager_Player::Create(this, m_pTransformCom, m_pNavigationCom);

	return S_OK;
}

HRESULT CSkeever::Ready_Cell()
{
	return S_OK;
}

HRESULT CSkeever::Bind_ShaderResource()
{
	//if (FAILED(m_pTransformCom->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_matWorld)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	// 뷰, 투영 행렬과 카메라의 위치를 던져준다.
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

	// 나머지 조명 연산에 필요한 데이터를 던져 줌
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pGameInstance);

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

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pNavigationCom);

}
