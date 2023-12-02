#include "framework.h"
#include "LoadingObj_Falmer.h"

#include "GameInstance.h"

CLoadingObj_Falmer::CLoadingObj_Falmer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CLoadingObj_Falmer::CLoadingObj_Falmer(const CLoadingObj_Falmer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CLoadingObj_Falmer::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CLoadingObj_Falmer::Initialize_Clone(void* pArg)
{
	if (FAILED(Ready_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-0.5f, -0.5f, 0.f, 1.f));
	m_strName = TEXT("LoadingObj_Falmer");

	m_strText = TEXT("���ڵ��� ���� ��Ȱ�� ���밡 �ȸ��� ����\n�ְ� ������ٰ� �����մϴ�.\n��������� �׵��� �ٸ� ������ ���� �����������ϴ�.");

	return S_OK;
}

void CLoadingObj_Falmer::PriorityTick(_float _fTimeDelta)
{
}

void CLoadingObj_Falmer::Tick(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_long mouseMove = 0l;

	if (pGameInstance->Get_DIKeyPress(VK_LBUTTON))
	{
		if (mouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::MMS_X)) // (y���� ȸ���ϸ� x���� �����̱⿡.)
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -(mouseMove * 0.01f * _fTimeDelta));
	}
	else if (pGameInstance->Get_DIKeyPress(VK_RBUTTON))
	{
		if (mouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::MMS_Y)) // (x���� ȸ���ϸ� y���� �����̱⿡.)
		{
			if (mouseMove > 0)
				m_pTransformCom->Go_Foward(_fTimeDelta);
			else
				m_pTransformCom->Go_Backward(_fTimeDelta);
		}
	}

	Safe_Release(pGameInstance);
}

void CLoadingObj_Falmer::LateTick(_float _fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CLoadingObj_Falmer::Render()
{
	if (FAILED(Bind_ShaderResource()))
		return E_FAIL;

	// �޽� �
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Render_Font(TEXT("Font_Bold"), m_strText,
		_float2(800.f, 600.f), XMVectorSet(1.f, 1.f, 1.f, 1.f)
		, 0.f, _float2(20.f, 20.f), 0.5f);

	Safe_Release(pGameInstance);


	return S_OK;
}

HRESULT CLoadingObj_Falmer::Ready_Component(void* pArg)
{
	if (FAILED(__super::Add_CloneComponent(LEVEL_LOADING, TEXT("ProtoType_Component_Model_LoadingObj_Falmer"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_LOADING, TEXT("ProtoType_Component_Shader_VtxNonAnimMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// Transform
	CTransform::TRANSFORM_DESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fSpeedPerSec = 0.1f;
	TransformDesc.fRotationRadianPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoadingObj_Falmer::Bind_ShaderResource()
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

CLoadingObj_Falmer* CLoadingObj_Falmer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLoadingObj_Falmer* pInstance = new CLoadingObj_Falmer(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CLoadingObj_Falmer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLoadingObj_Falmer::Clone(void* _pArg)
{
	CLoadingObj_Falmer* pInstance = new CLoadingObj_Falmer(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CLoadingObj_Falmer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoadingObj_Falmer::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
