#include "framework.h"
#include "Level_WhiteRun.h"

#include "Level_Loading.h"

#include <filesystem>
#include <fstream>
#include <commdlg.h>

#include "IMGui_Manager.h"
#include "GameInstance.h"

#include "PlayerCamera.h"
#include "Player.h"


CLevel_WhiteRun::CLevel_WhiteRun(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_WhiteRun::Initialize()
{
	// CIMGui_Manager 초기화
	CIMGui_Manager::GetInstance()->Initialize(m_pDevice, m_pContext, LEVEL_GAMEPLAY);

	if (FAILED(Ready_Light()))
		return E_FAIL;
	
	if (FAILED(Ready_Level()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Navigation_WhiteRun(TEXT("Layer_Navigation_WhiteRun"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Equip(TEXT("Layer_Equip"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_PlayerCamera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Particle(TEXT("Layer_Particle"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_WhiteRun::Tick(_float _fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_WhiteRun::LateTick(_float _fTimeDelta)
{
	SetWindowText(g_hWnd, TEXT("Current Level : WhiteRun"));

	if (GetKeyState(VK_F10) & 0x8000)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		g_curStage = STAGE_DUNGEON;
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return E_FAIL;

		Safe_Release(pGameInstance);

		int i = 0;
	}

	return S_OK;
}

void CLevel_WhiteRun::AfterRender()
{
	// ImGui 그리기
	// CIMGui_Manager::GetInstance()->Frame();
}

HRESULT CLevel_WhiteRun::Ready_Level()
{
#pragma region Static
	/* 화이트런 */
	wstring filePath = TEXT("../Bin/SaveLoad/Skyrim2");

	ifstream fileStream(filePath, ios::binary);
	if (fileStream.is_open()) {
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		pGameInstance->Object_FileLoad(fileStream, LEVEL_GAMEPLAY);

		Safe_Release(pGameInstance);

		fileStream.close();
		MessageBox(g_hWnd, L"파일을 성공적으로 불러왔습니다.", L"불러오기 완료", MB_OK);
	}
	else {
		MessageBox(g_hWnd, L"파일을 불러오는 중 오류가 발생했습니다.", L"불러오기 오류", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CLevel_WhiteRun::Ready_Light()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Light_Clear();

	LIGHT_DESC LightDesc;

	/* 방향성 광원을 추가. */
	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eLightType = LIGHT_DESC::LIGHT_DIRECTIONAL;
	LightDesc.vLightDir = _float4(1.f, -1.f, 1.f, 0.f);
	
	LightDesc.vDiffuse = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(0.2f, 0.2f, 0.2f, 1.f);
	
	if (FAILED(pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

#pragma region Light

	wstring filePath = TEXT("../Bin/SaveLoad/Light_WhiteRun");
	// 파일을 열기 모드로 열기.
	ifstream fileStream(filePath, ios::binary);
	if (fileStream.is_open()) {
		// 파일 내용을 읽기.

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		pGameInstance->Light_FileLoad(fileStream, LEVEL_GAMEPLAY);

		Safe_Release(pGameInstance);

		fileStream.close();
		MessageBox(g_hWnd, L"파일을 성공적으로 불러왔습니다.", L"불러오기 완료", MB_OK);
	}
	else {
		MessageBox(g_hWnd, L"파일을 불러오는 중 오류가 발생했습니다.", L"불러오기 오류", MB_OK | MB_ICONERROR);
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

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Weapon_IronSword"))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Glass_Boots"))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Glass_Curiass"))))
		return E_FAIL;
	
	//if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Glass_Gauntlet"))))
	//	return E_FAIL;
	
	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Glass_Helmet"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_WhiteRun::Ready_Layer_Player(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Player"))))
		return E_FAIL;

	/* 화이트런 */
	dynamic_cast<CTransform*>(pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"),
		TEXT("Player"))->Get_Component(TEXT("Com_Transform")))
	->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.f, -1.3f, 12.f, 1.f));

	dynamic_cast<CPlayer*>(pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"),
		TEXT("Player")))->Set_CurCell();

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_WhiteRun::Ready_Layer_Camera(const wstring& _strLayerTag)
{
	/* 원형객체를 복제하여 사본객체를 생성하고 레이어에 추가한다. */
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 구조체 생성
	CPlayerCamera::FREE_PLAYERCAMERA_DESC FreeCameraDesc;
	ZeroMemory(&FreeCameraDesc, sizeof FreeCameraDesc);
		
	FreeCameraDesc.pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Player")));
	FreeCameraDesc.fMouseSensitive = 0.2f;
	FreeCameraDesc.vEye = _float4(0.f, 10.f, -8.f, 1.f);
	FreeCameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	FreeCameraDesc.fFovY = XMConvertToRadians(45.f);
	FreeCameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	FreeCameraDesc.fNear = 0.2f;
	FreeCameraDesc.fFar = 500.f;
	FreeCameraDesc.fSpeedPerSec = 100.f;
	FreeCameraDesc.fRotationRadianPerSec = XMConvertToRadians(90.f);
	FreeCameraDesc.fZoomPerSec = 500.f;

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_PlayerCamera"), &FreeCameraDesc)))
		return E_FAIL;

	// 추후 카메라 추가.(전투, 1인칭)

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
	/* 원형객체를 복제하여 사본객체를 생성하고 레이어에 추가한다. */
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
