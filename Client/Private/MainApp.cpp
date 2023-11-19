#include "../Default/framework.h"
#include "MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	g_bIsWhiteRunInit = false;
	g_bIsDungeonInit = false;
	g_bIsPublicInit = false;

	/* ���� �ʱ�ȭ */
	// 1. ��ġ
	GRAPHIC_DESC GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	GraphicDesc.eWinMode = GRAPHIC_DESC::WINMODE_WIN;
	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;

	// GameInstance �ʱ�ȭ
	if(FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, g_hInst, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	// ������Ʈ �ʱ�ȭ
	if (FAILED(Ready_ProtoType_Components()))
		return E_FAIL;

	// ���� ����
	if (FAILED(Open_Level(LEVEL_TOOL)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(_float _fTimeDelta)
{
	m_pGameInstance->Tick(_fTimeDelta);
}

HRESULT CMainApp::Render()
{
	if(g_curLevel == LEVEL_GAMEPLAY && g_curStage == STAGE_DUNGEON)
		m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 0.f, 1.f));
	else
		m_pGameInstance->Clear_BackBuffer_View(_float4(0.05f, 0.05f, 0.05f, 1.f));

	m_pGameInstance->Clear_DepthStencil_View();

	// �׸���
	m_pRenderer->Draw_RenderObjects();
	m_pGameInstance->AfterRender();

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID _eLevelID)
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	/* �ΰ� ���� ���� �ε� �۾� ����. */
	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, _eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_ProtoType_Components()
{
	// Renderer
	if (FAILED(m_pGameInstance->Add_ProtoType_Component(LEVEL_STATIC, TEXT("ProtoType_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Transform
	if (FAILED(m_pGameInstance->Add_ProtoType_Component(LEVEL_STATIC, TEXT("ProtoType_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Shader_VtxPosTex
	if (FAILED(m_pGameInstance->Add_ProtoType_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFile/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	// ViBuffer_Rect
	if (FAILED(m_pGameInstance->Add_ProtoType_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Safe_AddRef(m_pRenderer);

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Fail Create : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();

}
