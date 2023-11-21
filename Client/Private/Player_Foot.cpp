#include "framework.h"
#include "Player_Foot.h"

#include "GameInstance.h"
#include "Layer.h"

#include "Player.h"

CPlayer_Foot::CPlayer_Foot(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPlayerPart_Base(_pDevice, _pContext)
{

}

CPlayer_Foot::CPlayer_Foot(const CPlayer_Foot& rhs)
	: CPlayerPart_Base(rhs)
{

}

HRESULT CPlayer_Foot::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CPlayer_Foot::Initialize_Clone(void* _pArg)
{
	if (FAILED(__super::Initialize_Clone(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_strName = TEXT("Player_Foot");

	return S_OK;
}

void CPlayer_Foot::Tick(_float _fTimeDelta)
{
	m_pModelCom->Play_Animation(_fTimeDelta);

	Compute_RenderMatrix(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer_Foot::LateTick(_float _fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CPlayer_Foot::Render()
{
	if (dynamic_cast<CPlayer*>(m_pParent)->Get_CamMode() == CPlayer::CAM_3ST)
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
			if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;
		}
	}
	return S_OK;
}

void CPlayer_Foot::Set_AnimationIndex(_bool _bIsLoop, string _strAnimationName, _uint _iChangeIndex)
{
	m_pModelCom->SetUp_Animation(_bIsLoop, _strAnimationName, _iChangeIndex);
}

_uint CPlayer_Foot::Get_CurFrameIndex()
{
	return m_pModelCom->Get_CurFrameIndex();
}

_bool CPlayer_Foot::Get_CurAnimationIsLoop()
{
	return m_pModelCom->Get_CurAnimationIsLoop();
}

_bool CPlayer_Foot::Get_IsAnimationFin()
{
	return m_pModelCom->Get_IsAnimationFin();
}

string CPlayer_Foot::Get_CurAnimationName()
{
	return m_pModelCom->Get_CurAnimationName().c_str();
}


HRESULT CPlayer_Foot::Ready_Component()
{
	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("ProtoType_Component_Model_PlayerNude_Foot"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("ProtoType_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Foot::Bind_ShaderResource()
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

	Safe_Release(pGameInstance);

	return S_OK;
}

CPlayer_Foot* CPlayer_Foot::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CPlayer_Foot* pInstance = new CPlayer_Foot(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CPlayer_Foot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Foot::Clone(void* _pArg)
{
	CPlayer_Foot* pInstance = new CPlayer_Foot(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CPlayer_Foot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Foot::Free()
{
	__super::Free();
}
