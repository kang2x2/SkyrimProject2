#include "Renderer.h"
#include "GameObject.h"

#include "Target_Manager.h"
#include "Light_Manager.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "PipeLine.h"

CRenderer::CRenderer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
{
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pLight_Manager);
}

HRESULT CRenderer::Initialize_ProtoType()
{
	D3D11_VIEWPORT		ViewportDesc;

	_uint				iNumViewports = 1;

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	/* For. Target Diffuse */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Diffuse"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* For. Target Normal */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Normal"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	/* For. Target Depth */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Depth"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	/* For. Target Shade */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Shade"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	/* For. Target Specular*/
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Specular"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Diffuse"), 100.f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Normal"), 100.f, 300.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Depth"), 100.f, 500.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Shade"), 300.f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Specular"), 300.f, 300.f, 200.f, 200.f)))
		return E_FAIL;
#endif

	/* �� ����Ÿ�ٵ��� �׷����� ��ü�κ��� ���� ����޴´�. */
	/* For.MRT_GameObjects */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Depth"))))
		return E_FAIL;
	/* �� ����Ÿ�ٵ��� ���ӳ��� �����ϴ� �����κ��� ������ ����� ����޴´�. */
	/* For.MRT_ */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Light"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Light"), TEXT("Target_Specular"))))
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (m_pVIBuffer == nullptr)
		return E_FAIL;
	
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (m_pShader == nullptr)
		return E_FAIL;

	/* ���� ���� */
	XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());
	m_matWorld._11 = ViewportDesc.Width;
	m_matWorld._22 = ViewportDesc.Height;

	XMStoreFloat4x4(&m_matView, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matProj, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	return S_OK;
}

HRESULT CRenderer::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP _eGroup, CGameObject* _pCloneObject)
{
	// ������ ����� �߸��� �Ű������� �޾��� ��
	if (_eGroup >= RG_END)
		return E_FAIL;

	m_ltRenderObj[_eGroup].push_back(_pCloneObject);

	Safe_AddRef(_pCloneObject);

	return S_OK;
}

HRESULT CRenderer::Draw_RenderObjects()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;
	if (FAILED(Render_NonLight()))
		return E_FAIL;
	if (FAILED(Render_NonBlend()))
		return E_FAIL;
	if (FAILED(Render_LightAcc()))
		return E_FAIL;
	if (FAILED(Render_Deferred()))
		return E_FAIL;
	if (FAILED(Render_Blend()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Render_Debug()))
		return E_FAIL;
#endif

	return S_OK;
}

/* �� �����Ӹ��� �׸��� ����� �ݺ�. */
HRESULT CRenderer::Render_Priority()
{
	for (auto& iter : m_ltRenderObj[RG_PRIORITY])
	{
		if (iter != nullptr)
			iter->Render();

		Safe_Release(iter);
	}

	m_ltRenderObj[RG_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& iter : m_ltRenderObj[RG_NONLIGHT])
	{
		if (iter != nullptr)
			iter->Render();

		Safe_Release(iter);
	}

	m_ltRenderObj[RG_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	/* Diffuse + Normal + Depth */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_GameObject"))))
		return E_FAIL;

	for (auto& iter : m_ltRenderObj[RG_NONBLEND])
	{
		if (iter != nullptr)
			iter->Render();

		Safe_Release(iter);
	}

	m_ltRenderObj[RG_NONBLEND].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	/* �������� ����� �׷����� ShadeŸ���� ��ġ�� ���ε� �Ѵ�. */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Light"))))
		return E_FAIL;

	/* �簢�� ���۸� ������������ ShadeŸ���� �����ŭ �� ä���� �׸���. */
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_matWorld)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_matView)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_matProj)))
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_float4x4 matViewInv = pPipeLine->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW);
	_float4x4 matProjInv = pPipeLine->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_PROJ);
	_float4   vCamPos = pPipeLine->Get_CamPosition_Float4();
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &matViewInv)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &matProjInv)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &vCamPos, sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pPipeLine);

	if (FAILED(m_pTarget_Manager->Bind_SRV(m_pShader, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_SRV(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	m_pLight_Manager->Render(m_pShader, m_pVIBuffer);

	/* �ٽ� ��ġ�� 0��° ���Ͽ� �� ���۸� �ø���. */
	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	/* ��ǻ�� Ÿ�ٰ� ���̵� Ÿ���� ���� ���Ͽ� ����ۿ� ���������� ����. */
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_matWorld)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_matView)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_matProj)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(m_pShader, TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_SRV(m_pShader, TEXT("Target_Shade"), "g_ShadeTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_SRV(m_pShader, TEXT("Target_Specular"), "g_SpecularTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	for (auto& iter : m_ltRenderObj[RG_BLEND])
	{
		if (iter != nullptr)
			iter->Render();

		Safe_Release(iter);
	}

	m_ltRenderObj[RG_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& iter : m_ltRenderObj[RG_UI])
	{
		if (iter != nullptr)
			iter->Render();

		Safe_Release(iter);
	}

	m_ltRenderObj[RG_UI].clear();

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	for (auto& iter : m_ltRenderDebug)
	{
		iter->Render();
		Safe_Release(iter);
	}
	m_ltRenderDebug.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_matView)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_matProj)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Render(TEXT("MRT_GameObject"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Render(TEXT("MRT_Light"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}
#endif

CRenderer* CRenderer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CRenderer* pInstance = new CRenderer(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_ProtoType()))
	{
		MSG_BOX("Fail Create : CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CRenderer::Clone(void* pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);
}
