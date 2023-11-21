#include "framework.h"
#include "Level_Dungeon.h"

#include "Level_Loading.h"

#include <filesystem>
#include <fstream>
#include <commdlg.h>

#include "IMGui_Manager.h"
#include "GameInstance.h"

#include "Player.h"

CLevel_Dungeon::CLevel_Dungeon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Dungeon::Initialize()
{
	if (FAILED(Ready_Layer_Equip(TEXT("Layer_Equip"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Level()))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Navigation_Dungeon(TEXT("Layer_Navigation_Dungeon"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Particle(TEXT("Layer_Particle"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Dungeon::Tick(_float _fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_Dungeon::LateTick(_float _fTimeDelta)
{
	SetWindowText(g_hWnd, TEXT("Current Level : Dungeon"));

	if (GetKeyState(VK_F11) & 0x8000)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		g_curStage = STAGE_WHITERUN;
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return E_FAIL;

		Safe_Release(pGameInstance);
	}


	return S_OK;
}

void CLevel_Dungeon::AfterRender()
{
}

HRESULT CLevel_Dungeon::Ready_Level()
{
#pragma region Static
	wstring filePath = TEXT("../Bin/SaveLoad/Dungeon1");

	// ������ ���� ���� ����.
	ifstream fileStream(filePath, ios::binary);
	if (fileStream.is_open()) {
		// ���� ������ �б�.

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		pGameInstance->Object_FileLoad(fileStream, LEVEL_GAMEPLAY);

		Safe_Release(pGameInstance);

		fileStream.close();
		MessageBox(g_hWnd, L"������ ���������� �ҷ��Խ��ϴ�.", L"�ҷ����� �Ϸ�", MB_OK);
	}
	else {
		MessageBox(g_hWnd, L"������ �ҷ����� �� ������ �߻��߽��ϴ�.", L"�ҷ����� ����", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}
#pragma endregion

#pragma region Dynamic

	filePath = TEXT("../Bin/SaveLoad/Dungeon_Monster");

	// ������ ���� ���� ����.
	ifstream fileStream2(filePath, ios::binary);
	if (fileStream2.is_open()) {
		// ���� ������ �б�.

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		
		pGameInstance->Object_FileLoad(fileStream2, LEVEL_GAMEPLAY);
		
		Safe_Release(pGameInstance);

		fileStream2.close();
		MessageBox(g_hWnd, L"������ ���������� �ҷ��Խ��ϴ�.", L"�ҷ����� �Ϸ�", MB_OK);
	}
	else {
		MessageBox(g_hWnd, L"������ �ҷ����� �� ������ �߻��߽��ϴ�.", L"�ҷ����� ����", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

#pragma endregion

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Light()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Light_Clear();

	LIGHT_DESC LightDesc;

	/* ���⼺ ������ �߰�. */
	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eLightType = LIGHT_DESC::LIGHT_DIRECTIONAL;
	LightDesc.vLightDir = _float4(1.f, -1.f, 1.f, 0.f);

	LightDesc.vDiffuse = _float4(0.05f, 0.05f, 0.05f, 1.f);
	LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vSpecular = _float4(0.2f, 0.2f, 0.2f, 1.f);
	
	if (FAILED(pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

#pragma region Light

	wstring filePath = TEXT("../Bin/SaveLoad/Light_Dungeon");
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

HRESULT CLevel_Dungeon::Ready_Layer_Equip(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Weapon_FalmerAxe"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Player(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Player"))))
		return E_FAIL;

	//dynamic_cast<CTransform*>(pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"),
	//	TEXT("Player"))->Get_Component(TEXT("Com_Transform")))
	//	->Set_State(CTransform::STATE_POSITION, XMVectorSet(51.f, 0.f, 3.f, 1.f));

	dynamic_cast<CTransform*>(pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"),
		TEXT("Player"))->Get_Component(TEXT("Com_Transform")))
		->Set_State(CTransform::STATE_POSITION, XMVectorSet(51.f, -4.f, 42.f, 1.f));

	dynamic_cast<CPlayer*>(pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"),
		TEXT("Player")))->Set_CurCell();

	Safe_Release(pGameInstance);

	return S_OK;

}

HRESULT CLevel_Dungeon::Ready_Layer_Navigation_Dungeon(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Navigation"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Sky(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstace = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstace);

	if (FAILED(pGameInstace->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Sky"))))
		return E_FAIL;

	Safe_Release(pGameInstace);

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Particle(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Particle_Rect"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Dungeon* CLevel_Dungeon::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_Dungeon* pInstance = new CLevel_Dungeon(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Fail Create : CLevel_Dungeon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Dungeon::Free()
{
	__super::Free();
}
