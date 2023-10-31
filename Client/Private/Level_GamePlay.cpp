#include "framework.h"

#include <filesystem>
#include <fstream>
#include <commdlg.h>

#include "Level_GamePlay.h"

#include "IMGui_Manager.h"
#include "GameInstance.h"

#include "PlayerCamera_Free.h"
#include "Player.h"


CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	// CIMGui_Manager 초기화
	CIMGui_Manager::GetInstance()->Initialize(m_pDevice, m_pContext, LEVEL_GAMEPLAY);

	//if (FAILED(Ready_Level()))
	//	return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_PlayerCamera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Navigation_Cleint(TEXT("Layer_Navigation_Client"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Particle(TEXT("Layer_Particle"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Tick(_float _fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_GamePlay::LateTick(_float _fTimeDelta)
{
	SetWindowText(g_hWnd, TEXT("Current Level : GamePlay"));

	return S_OK;
}

void CLevel_GamePlay::AfterRender()
{
	// ImGui 그리기
	// CIMGui_Manager::GetInstance()->Frame();
}

HRESULT CLevel_GamePlay::Ready_Level()
{
#pragma region Object
	wstring filePath = L"D:\\SkyrimProject\\Client\\Bin\\SaveLoad\\Skyrim2";

	// 파일을 열기 모드로 열기.
	ifstream fileStream(filePath, ios::binary);
	if (fileStream.is_open()) {
		// 파일 내용을 읽기.

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

HRESULT CLevel_GamePlay::Ready_Light()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	LIGHT_DESC LightDesc;

	/* 방향성 광원을 추가. */
	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eLightType = LIGHT_DESC::LIGHT_DIRECTIONAL;
	LightDesc.vLightDir = _float4(1.f, -1.f, 1.f, 0.f);
	
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	
	if (FAILED(pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	/* 점 광원 추가 */
	//ZeroMemory(&LightDesc, sizeof(LightDesc));
	//LightDesc.eLightType = LIGHT_DESC::LIGHT_POINT;
	//LightDesc.vLightPos = _float4(50.f, 1.f, 50.f, 1.f);
	//LightDesc.fLightRange = 25.f;
	//
	//LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Light(LightDesc)))
	//	return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Player"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring& _strLayerTag)
{
	/* 원형객체를 복제하여 사본객체를 생성하고 레이어에 추가한다. */
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 구조체 생성
	CPlayerCamera_Free::FREE_PLAYERCAMERA_DESC FreeCameraDesc;
	ZeroMemory(&FreeCameraDesc, sizeof FreeCameraDesc);
		
	FreeCameraDesc.pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Player")));
	FreeCameraDesc.fMouseSensitive = 0.2f;
	FreeCameraDesc.vEye = _float4(0.f, 10.f, -8.f, 1.f);
	FreeCameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	FreeCameraDesc.fFovY = XMConvertToRadians(60.f);
	FreeCameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	FreeCameraDesc.fNear = 0.2f;
	FreeCameraDesc.fFar = 1100.f;
	FreeCameraDesc.fSpeedPerSec = 100.f;
	FreeCameraDesc.fRotationRadianPerSec = XMConvertToRadians(90.f);
	FreeCameraDesc.fZoomPerSec = 500.f;

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_FreePlayerCamera"), &FreeCameraDesc)))
		return E_FAIL;

	// 추후 카메라 추가.(전투, 1인칭)

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Navigation_Cleint(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstace = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstace);

	if (FAILED(pGameInstace->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Navigation_Client"))))
		return E_FAIL;

	Safe_Release(pGameInstace);


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Sky(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstace = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstace);
	
	if (FAILED(pGameInstace->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Sky"))))
		return E_FAIL;
	
	Safe_Release(pGameInstace);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Terrain(const wstring& _strLayerTag)
{
	/* 원형객체를 복제하여 사본객체를 생성하고 레이어에 추가한다. */
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Terrain"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Particle(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Particle_Rect"))))
		return E_FAIL;

	Safe_Release(pGameInstance);
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Fail Create : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

	CIMGui_Manager::GetInstance()->DestroyInstance();
}
