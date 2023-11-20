#include "framework.h"
#include "DGPlaceableObj.h"

#include "GameInstance.h"

CDGPlaceableObj::CDGPlaceableObj(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CDGPlaceableObj::CDGPlaceableObj(const CDGPlaceableObj& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDGPlaceableObj::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CDGPlaceableObj::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CDGPlaceableObj::Initialize_Clone(_uint _iLevel, const wstring& _strModelComTag, void* pArg)
{
	_matrix* pMatPivot = (_matrix*)pArg;

	m_strModelComTag = _strModelComTag;

	if (FAILED(Ready_Component(_iLevel)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(*pMatPivot);

	m_bHasMesh = true;
	m_strName = TEXT("DGPlaceableObj");

	return S_OK;

}

void CDGPlaceableObj::Tick(_float _fTimeDelta)
{

}

void CDGPlaceableObj::LateTick(_float _fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CDGPlaceableObj::Render()
{
	if (FAILED(Bind_ShaderResource()))
		return E_FAIL;

	// 메시 몇개
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;

}

HRESULT CDGPlaceableObj::Ready_Component(_uint _iLevel)
{
	if (FAILED(__super::Add_CloneComponent(_iLevel, m_strModelComTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(g_curLevel, TEXT("ProtoType_Component_Shader_VtxNonAnimMesh"),
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

HRESULT CDGPlaceableObj::Bind_ShaderResource()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	// 뷰, 투영 행렬과 카메라의 위치을 던져준다.
	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ViewMatrix", CPipeLine::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CDGPlaceableObj* CDGPlaceableObj::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CDGPlaceableObj* pInstance = new CDGPlaceableObj(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CDGPlaceableObj");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDGPlaceableObj::Clone(void* _pArg)
{
	CDGPlaceableObj* pInstance = new CDGPlaceableObj(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CDGPlaceableObj");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDGPlaceableObj::Clone(_uint _iLevel, const wstring& _strModelComTag, void* _pArg)
{
	CDGPlaceableObj* pInstance = new CDGPlaceableObj(*this);

	if (FAILED(pInstance->Initialize_Clone(_iLevel, _strModelComTag, _pArg)))
	{
		MSG_BOX("Fail Clone : CDGPlaceableObj");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDGPlaceableObj::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
