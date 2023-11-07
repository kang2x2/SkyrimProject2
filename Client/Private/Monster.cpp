#include "framework.h"
#include "Monster.h"

#include "GameInstance.h"

#include "Player.h"

CMonster::CMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCreatureObject(_pDevice, _pContext)
{
}

CMonster::CMonster(const CMonster& rhs)
	: CCreatureObject(rhs)
{
}

HRESULT CMonster::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CMonster::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CMonster::Initialize_Clone(_uint _iLevel, const wstring& _strModelComTag, void* pArg)
{
	return S_OK;
}

void CMonster::PriorityTick(_float _fTimeDelta)
{
}

void CMonster::Tick(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_pNavigationCom != nullptr)
	{
		_vector	vPosition = m_pNavigationCom->Set_OnCell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}

	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();

	Safe_Release(pGameInstance);
}

void CMonster::LateTick(_float _fTimeDelta)
{
}

HRESULT CMonster::Render()
{
#ifdef _DEBUG
	if (m_pNavigationCom != nullptr)
		m_pNavigationCom->Render();
#endif

	return S_OK;
}

HRESULT CMonster::Bind_ShaderResource()
{
	//if (FAILED(m_pTransformCom->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

	_float4x4 matWorld = m_pTransformCom->Get_WorldMatrix_Float4x4();
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &matWorld)))
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

void CMonster::Play_Animation(_bool _bIsLoop, string _strAnimationName)
{
	Set_AnimationIndex(_bIsLoop, _strAnimationName);
}

void CMonster::Set_AnimationIndex(_bool _bIsLoop, string _strAnimationName)
{
	m_pModelCom->SetUp_Animation(_bIsLoop, _strAnimationName);
}

_bool CMonster::Get_IsAnimationFin()
{
	return m_pModelCom->Get_IsAnimationFin();
}

_bool CMonster::Get_CurAnimationName(string _strAnimationName)
{
	if (!strcmp(m_pModelCom->Get_CurAnimationName().c_str(), _strAnimationName.c_str()))
		return true;

	return false;
}

_uint CMonster::Get_CurFrameIndex()
{
	return m_pModelCom->Get_CurFrameIndex();
}

HRESULT CMonster::Ready_Component()
{
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

	if (g_curLevel != LEVEL_TOOL)
	{
		/* Com_Navigation */
		CNavigation::DESC_NAVIGATION		NavigationDesc;
		NavigationDesc.iCurIndex = -1;

		if (FAILED(__super::Add_CloneComponent(LEVEL_WHITERUN, TEXT("ProtoType_Component_Navigation"),
			TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NavigationDesc)))
			return E_FAIL;

		m_pNavigationCom->Set_CurCell(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}


	return S_OK;
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pNavigationCom);
}
