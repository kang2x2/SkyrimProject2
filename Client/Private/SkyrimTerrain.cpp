#include "framework.h"
#include "SkyrimTerrain.h"

#include "GameInstance.h"

CSkyrimTerrain::CSkyrimTerrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CSkyrimTerrain::CSkyrimTerrain(const CSkyrimTerrain& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkyrimTerrain::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CSkyrimTerrain::Initialize_Clone(void* pArg)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_strName = TEXT("SkyrimTerrain");

	return S_OK;
}

void CSkyrimTerrain::Tick(_float _fTimeDelta)
{
}

void CSkyrimTerrain::LateTick(_float _fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CSkyrimTerrain::Render()
{
	if (FAILED(Bind_ShaderResource()))
		return E_FAIL;

	// 메시 몇개
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}


	return S_OK;
}

HRESULT CSkyrimTerrain::Ready_Component()
{
	if (FAILED(__super::Add_CloneComponent(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Model_SkyrimTerrain"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_GAMEPLAY, TEXT("ProtoType_Component_Shader_VtxMesh"),
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

HRESULT CSkyrimTerrain::Bind_ShaderResource()
{
	return S_OK;
}

CSkyrimTerrain* CSkyrimTerrain::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSkyrimTerrain* pInstance = new CSkyrimTerrain(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CSkyrimTerrain ProtoType");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkyrimTerrain::Clone(void* _pArg)
{
	CSkyrimTerrain* pInstance = new CSkyrimTerrain(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CSkyrimTerrain Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkyrimTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
