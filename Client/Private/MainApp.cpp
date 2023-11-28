#include "../Default/framework.h"
#include "MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "SkyrimUI_SceneChange.h"
#include "Inventory.h"

#include "Loading_Camera.h"
#include "LoadingObj_Falmer.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	ShowCursor(false);

	g_bIsWhiteRunInit = false;
	g_bIsDungeonInit = false;
	g_bIsCastleInit = false;
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

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Bold"), TEXT("../Bin/Resource/Fonts/Bold.spriteFont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_ExtraBold"), TEXT("../Bin/Resource/Fonts/ExtraBold.spriteFont"))))
		return E_FAIL;

	// ���� ����
	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(_float _fTimeDelta)
{
	m_pGameInstance->Tick(_fTimeDelta);
}

HRESULT CMainApp::Render()
{
	//if(g_curLevel == LEVEL_LOADING || g_curLevel == LEVEL_LOGO)
	//	m_pGameInstance->Clear_BackBuffer_View(_float4(0.02f, 0.02f, 0.02f, 1.f));
	if (g_curLevel == LEVEL_TOOL)
		m_pGameInstance->Clear_BackBuffer_View(_float4(0.1f, 0.1f, 0.6f, 1.f));
	//else
	//	m_pGameInstance->Clear_BackBuffer_View(_float4(0.01f, 0.02f, 0.1f, 1.f));

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
#pragma region Texture
	// Texture
	if (FAILED(m_pGameInstance->Add_ProtoType_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cursor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/Skyrim/UI/Cursor.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ProtoType_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Black"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/Skyrim/UI/black.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ProtoType_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Gray"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/Skyrim/UI/gray.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ProtoType_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WhiteEm"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/Skyrim/UI/white_em.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ProtoType_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/Skyrim/Dissolve.png"), 1))))
		return E_FAIL;
	/* Bar */
	if (FAILED(m_pGameInstance->Add_ProtoType_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EmptyBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/Skyrim/UI/EmptyBar.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ProtoType_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SpBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/Skyrim/UI/SpBar.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ProtoType_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HpBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resource/Textures/Skyrim/UI/HpBar.png"), 1))))
		return E_FAIL;

#pragma endregion


	// Renderer
	if (FAILED(m_pGameInstance->Add_ProtoType_Component(LEVEL_STATIC, TEXT("ProtoType_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Transform
	if (FAILED(m_pGameInstance->Add_ProtoType_Component(LEVEL_STATIC, TEXT("ProtoType_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
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
