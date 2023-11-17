#include "framework.h"
#include "Weapon_FalmerAxe.h"

#include "GameInstance.h"
#include "Bone.h"

CWeapon_FalmerAxe::CWeapon_FalmerAxe(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CSkyrimWeapon(_pDevice, _pContext)
{
}

CWeapon_FalmerAxe::CWeapon_FalmerAxe(const CWeapon_FalmerAxe& rhs)
	: CSkyrimWeapon(rhs)
{
}

HRESULT CWeapon_FalmerAxe::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CWeapon_FalmerAxe::Initialize_Clone(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scaling(_float3(0.01f, 0.01f, 0.01f));
	m_pTransformCom->Fix_Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.0f));
	// matInitialize = XMMatrixRotationY(XMConvertToRadians(-90.f));
	m_strName = TEXT("FalmerAxe");

	return S_OK;
}

void CWeapon_FalmerAxe::Tick(_float _fTimeDelta)
{
}

void CWeapon_FalmerAxe::LateTick(_float _fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CWeapon_FalmerAxe::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	// �޽� �
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CWeapon_FalmerAxe::Ready_Component()
{
	if (FAILED(__super::Add_CloneComponent(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Model_Weapon_FalmerAxe"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Shader_VtxNonAnimMesh"),
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

HRESULT CWeapon_FalmerAxe::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	// ��, ���� ����� �����ش�.
	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ViewMatrix", CPipeLine::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CWeapon_FalmerAxe* CWeapon_FalmerAxe::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CWeapon_FalmerAxe* pInstance = new CWeapon_FalmerAxe(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CWeapon_FalmerAxe");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon_FalmerAxe::Clone(void* _pArg)
{
	CWeapon_FalmerAxe* pInstance = new CWeapon_FalmerAxe(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CWeapon_FalmerAxe");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_FalmerAxe::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);

}
