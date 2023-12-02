#include "framework.h"
#include "Carlotta_Foot.h"

#include "GameInstance.h"
#include "Layer.h"

#include "Player.h"

CCarlotta_Foot::CCarlotta_Foot(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCarlottaPart_Base(_pDevice, _pContext)
{

}

CCarlotta_Foot::CCarlotta_Foot(const CCarlotta_Foot& rhs)
	: CCarlottaPart_Base(rhs)
{

}

HRESULT CCarlotta_Foot::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CCarlotta_Foot::Initialize_Clone(void* _pArg)
{
	if (FAILED(__super::Initialize_Clone(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	//CGameInstance* pGameInstance = CGameInstance::GetInstance();
	//Safe_AddRef(pGameInstance);
	//
	//pGameInstance->Add_CloneObject(g_curLevel, TEXT("Temp"), TEXT("ProtoType_GameObject_BootsF_Merchant01"));
	//
	//CGameObject* tempObject = pGameInstance->Find_CloneObject(g_curLevel, TEXT("Temp"), TEXT("BootsF_Merchant01"));
	//
	//m_pModelCom->SwapDesc_Armor(
	//	dynamic_cast<CModel*>(tempObject->Get_Component(TEXT("Com_3stModel"))));
	//
	//Safe_Release(pGameInstance);

	m_strName = TEXT("Carlotta_Foot");

	return S_OK;
}

void CCarlotta_Foot::Tick(_float _fTimeDelta)
{
	m_pModelCom->Play_Animation(_fTimeDelta);

	Compute_RenderMatrix(m_pTransformCom->Get_WorldMatrix());
}

void CCarlotta_Foot::LateTick(_float _fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CCarlotta_Foot::Render()
{
	if (FAILED(Bind_ShaderResource()))
		return E_FAIL;

	// �޽� �
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

	return S_OK;
}

void CCarlotta_Foot::Set_AnimationIndex(_bool _bIsLoop, string _strAnimationName, _uint _iChangeIndex, _bool _bIsReset, _bool _bIsQuickChange)
{
	m_pModelCom->SetUp_Animation(_bIsLoop, _strAnimationName, _iChangeIndex, _bIsReset, _bIsQuickChange);
}

_uint CCarlotta_Foot::Get_CurFrameIndex()
{
	return m_pModelCom->Get_CurFrameIndex();
}

_bool CCarlotta_Foot::Get_CurAnimationIsLoop()
{
	return m_pModelCom->Get_CurAnimationIsLoop();
}

_bool CCarlotta_Foot::Get_IsAnimationFin()
{
	return m_pModelCom->Get_IsAnimationFin();
}

string CCarlotta_Foot::Get_CurAnimationName()
{
	return m_pModelCom->Get_CurAnimationName().c_str();
}


HRESULT CCarlotta_Foot::Ready_Component()
{
	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("ProtoType_Component_Model_Carlotta_Foot"),
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

HRESULT CCarlotta_Foot::Bind_ShaderResource()
{
	//if (FAILED(m_pTransformCom->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_matWorld)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// ��, ���� ��İ� ī�޶��� ��ġ�� �����ش�.
	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ViewMatrix", CPipeLine::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CCarlotta_Foot* CCarlotta_Foot::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCarlotta_Foot* pInstance = new CCarlotta_Foot(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CCarlotta_Foot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCarlotta_Foot::Clone(void* _pArg)
{
	CCarlotta_Foot* pInstance = new CCarlotta_Foot(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CCarlotta_Foot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCarlotta_Foot::Free()
{
	__super::Free();
}
