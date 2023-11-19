#include "framework.h"
#include "Carlotta_Body.h"

#include "GameInstance.h" 
#include "Bone.h"

#include "Weapon_IronSword.h"

#include "Layer.h"
#include "Player.h"

CCarlotta_Body::CCarlotta_Body(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCarlottaPart_Base(_pDevice, _pContext)
{
}

CCarlotta_Body::CCarlotta_Body(const CCarlotta_Body& rhs)
	: CCarlottaPart_Base(rhs)
{
}

HRESULT CCarlotta_Body::Initialize_ProtoType()
{
	return S_OK;
}

HRESULT CCarlotta_Body::Initialize_Clone(void* _pArg)
{
	if (nullptr != _pArg)
	{
		BODY_DESC* pBodyDesc = (BODY_DESC*)_pArg;

		m_pSocketBone = pBodyDesc->pSocketBone;
		Safe_AddRef(m_pSocketBone);

		m_matSocketPivot = pBodyDesc->matSocketPivot;

		if (FAILED(__super::Initialize_Clone(_pArg)))
			return E_FAIL;
	}

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scaling(_float3(0.0128f, 0.0128f, 0.0128f));
	// m_pTransformCom->Fix_Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-105.0f));
	_float4 vInitPos;
	XMStoreFloat4(&vInitPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//vInitPos.x -= 0.05f;
	//vInitPos.y -= 0.03f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vInitPos));

	m_strName = TEXT("CarlottaBody");

	return S_OK;
}

void CCarlotta_Body::Tick(_float _fTimeDelta)
{
	/* 내 행렬 * (소캣 뼈의 컴바인드 행렬 * 소캣의 행렬 * 페어런트의 월드 행렬) */
	_float4x4 matSocketCombined = m_pSocketBone->Get_CombinedTransformationMatrix();

	_matrix		WorldMatrix = XMLoadFloat4x4(&matSocketCombined) *
		XMLoadFloat4x4(&m_matSocketPivot);

	WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
	WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
	WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);

	Compute_RenderMatrix(m_pTransformCom->Get_WorldMatrix() * WorldMatrix);

	dynamic_cast<CTransform*>(m_pBody->Get_Component(TEXT("Com_Transform")))->Set_WorldMatrix(XMLoadFloat4x4(&m_matWorld));

	m_pColliderCom->Update(XMLoadFloat4x4(&m_matWorld));
}

void CCarlotta_Body::LateTick(_float _fTimeDelta)
{
	m_pBody->LateTick(_fTimeDelta);

#ifdef _DEBUG
	m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CCarlotta_Body::Render()
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

	return S_OK;
}

const char* CCarlotta_Body::Get_SoketBoneName()
{
	return m_pSocketBone->Get_BoneName();
}

HRESULT CCarlotta_Body::Ready_Component()
{
	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_CloneComponent(LEVEL_STATIC, TEXT("ProtoType_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCarlotta_Body::Bind_ShaderResource()
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

CCarlotta_Body* CCarlotta_Body::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCarlotta_Body* pInstance = new CCarlotta_Body(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CCarlotta_Body");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCarlotta_Body::Clone(void* _pArg)
{
	CCarlotta_Body* pInstance = new CCarlotta_Body(*this);

	if (FAILED(pInstance->Initialize_Clone(_pArg)))
	{
		MSG_BOX("Fail Clone : CCarlotta_Body");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCarlotta_Body::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pSocketBone);
}
