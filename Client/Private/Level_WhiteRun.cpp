#include "framework.h"
#include "Level_WhiteRun.h"

#include "Level_Loading.h"

#include <filesystem>
#include <fstream>
#include <commdlg.h>

#include "IMGui_Manager.h"
#include "GameInstance.h"

#include "SceneChangeCol.h"
#include "Player.h"
#include "Inventory.h"


CLevel_WhiteRun::CLevel_WhiteRun(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_WhiteRun::Initialize()
{
	// CIMGui_Manager �ʱ�ȭ
	CIMGui_Manager::GetInstance()->Initialize(m_pDevice, m_pContext, LEVEL_GAMEPLAY);

	if (FAILED(Ready_Cursor(TEXT("Layer_Cursor"))))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Equip(TEXT("Layer_Equip"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Inventory(TEXT("Layer_Inventory"))))
		return E_FAIL;

	if (FAILED(Ready_Level()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Navigation_WhiteRun(TEXT("Layer_Navigation_WhiteRun"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Particle(TEXT("Layer_Particle"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_WhiteRun::Tick(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Clear_BackBuffer_View(_float4(0.01f, 0.02f, 0.1f, 1.f));

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_WhiteRun::LateTick(_float _fTimeDelta)
{
	SetWindowText(g_hWnd, TEXT("Current Level : WhiteRun"));

	//if (GetKeyState(VK_F10) & 0x8000)
	//{
	//	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	//	Safe_AddRef(pGameInstance);
	//
	//	g_curStage = STAGE_DUNGEON;
	//	if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
	//		return E_FAIL;
	//
	//	Safe_Release(pGameInstance);
	//
	//	int i = 0;
	//}
	if (m_bIsChangeScene)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
	
		g_curStage = (STAGEID)m_iChangeStageIdx;
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, (LEVELID)m_iChangeLevelIdx))))
			return E_FAIL;
	
		m_bIsChangeScene = false;
		Safe_Release(pGameInstance);
	}

	return S_OK;
}

void CLevel_WhiteRun::AfterRender()
{
	// ImGui �׸���
	// CIMGui_Manager::GetInstance()->Frame();
}

HRESULT CLevel_WhiteRun::Ready_Cursor(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Cursor"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_WhiteRun::Ready_Level()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CSceneChangeCol::tagSceneChangeColDesc tempDesc;
	tempDesc.eDestStage = STAGE_DUNGEON;
	tempDesc.vPos = { 3.f, 7.f, 64.f, 1.f };
	tempDesc.strDestText = TEXT("���ϼ���");
	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_SceneChangeCol"), TEXT("ProtoType_GameObject_SceneChangeCol"), &tempDesc)))
		return E_FAIL;

#pragma region Static
	/* ȭ��Ʈ�� */
	// wstring filePath = TEXT("../Bin/SaveLoad/Skyrim3");
	wstring filePath = TEXT("../Bin/SaveLoad/testMap");

	ifstream fileStream(filePath, ios::binary);
	if (fileStream.is_open()) {

		pGameInstance->Object_FileLoad(fileStream, LEVEL_GAMEPLAY);

		fileStream.close();
		MessageBox(g_hWnd, L"������ ���������� �ҷ��Խ��ϴ�.", L"�ҷ����� �Ϸ�", MB_OK);
	}
	else {
		MessageBox(g_hWnd, L"������ �ҷ����� �� ������ �߻��߽��ϴ�.", L"�ҷ����� ����", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}
#pragma endregion

#pragma region Dynamic

	//filePath = TEXT("../Bin/SaveLoad/Outfit_NPC");
	filePath = TEXT("../Bin/SaveLoad/testMonster2");
	//
	ifstream fileStream2(filePath, ios::binary);
	if (fileStream2.is_open()) {
	
		pGameInstance->Object_FileLoad(fileStream2, LEVEL_GAMEPLAY);
	
		fileStream2.close();
		MessageBox(g_hWnd, L"������ ���������� �ҷ��Խ��ϴ�.", L"�ҷ����� �Ϸ�", MB_OK);
	}
	else {
		MessageBox(g_hWnd, L"������ �ҷ����� �� ������ �߻��߽��ϴ�.", L"�ҷ����� ����", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

#pragma endregion

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_WhiteRun::Ready_Light()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Light_Clear();

	LIGHT_DESC LightDesc;

	/* ���⼺ ������ �߰�. */
	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eLightType = LIGHT_DESC::LIGHT_DIRECTIONAL;
	LightDesc.vLightDir = _float4(1.f, -1.f, 1.f, 0.f);
	
	//LightDesc.vDiffuse = _float4(0.01f, 0.01f, 0.01f, 1.f);
	LightDesc.vDiffuse = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vSpecular = _float4(0.1f, 0.1f, 0.1f, 1.f);
	
	if (FAILED(pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

#pragma region Light

	// wstring filePath = TEXT("../Bin/SaveLoad/Light_WhiteRun");
	wstring filePath = TEXT("../Bin/SaveLoad/testLight");
	// ������ ���� ���� ����.
	ifstream fileStream(filePath, ios::binary);
	if (fileStream.is_open()) {
		// ���� ������ �б�.
	
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
	
		pGameInstance->Light_FileLoad(fileStream, LEVEL_GAMEPLAY);
	
		Safe_Release(pGameInstance);
	
		fileStream.close();
		MessageBox(g_hWnd, L"������ ���������� �ҷ��Խ��ϴ�.", L"�ҷ����� �Ϸ�", MB_OK);
	}
	else {
		MessageBox(g_hWnd, L"������ �ҷ����� �� ������ �߻��߽��ϴ�.", L"�ҷ����� ����", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

#pragma endregion

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_WhiteRun::Ready_Layer_Equip(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_WhiteRun::Ready_Layer_Player(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Player"))))
		return E_FAIL;

	/* ȭ��Ʈ�� */
	//dynamic_cast<CTransform*>(pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"),
	//	TEXT("Player"))->Get_Component(TEXT("Com_Transform")))
	//->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.f, -1.3f, 12.f, 1.f));
	dynamic_cast<CTransform*>(pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"),
		TEXT("Player"))->Get_Component(TEXT("Com_Transform")))
	->Set_State(CTransform::STATE_POSITION, XMVectorSet(-2.f, -1.f, 12.f, 1.f));

	dynamic_cast<CPlayer*>(pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"),
		TEXT("Player")))->Set_CurCell();

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_WhiteRun::Ready_Layer_Inventory(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* Inventory */
	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Player_Inventory"), TEXT("ProtoType_GameObject_UI_Inventory"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_WhiteRun::Ready_Layer_Navigation_WhiteRun(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstace = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstace);

	if (FAILED(pGameInstace->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Navigation"))))
		return E_FAIL;

	Safe_Release(pGameInstace);


	return S_OK;
}

HRESULT CLevel_WhiteRun::Ready_Layer_Sky(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstace = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstace);
	
	if (FAILED(pGameInstace->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Sky"))))
		return E_FAIL;
	
	Safe_Release(pGameInstace);

	return S_OK;
}

HRESULT CLevel_WhiteRun::Ready_Layer_Terrain(const wstring& _strLayerTag)
{
	/* ������ü�� �����Ͽ� �纻��ü�� �����ϰ� ���̾ �߰��Ѵ�. */
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Terrain"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_WhiteRun::Ready_Layer_Particle(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Particle_Rect"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_WhiteRun::Ready_Layer_Effect(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_BloodFlare"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_BloodSpot"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_WhiteRun* CLevel_WhiteRun::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_WhiteRun* pInstance = new CLevel_WhiteRun(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Fail Create : CLevel_WhiteRun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_WhiteRun::Free()
{
	__super::Free();

	CIMGui_Manager::GetInstance()->DestroyInstance();
}
