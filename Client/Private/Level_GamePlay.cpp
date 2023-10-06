#include "framework.h"
#include "Level_GamePlay.h"

#include "IMGui_Manager.h"
#include "GameInstance.h"

#include "Free_Camera.h"


CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	// CIMGui_Manager 초기화
	CIMGui_Manager::GetInstance()->Initialize(m_pDevice, m_pContext, LEVEL_GAMEPLAY);

	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Playher(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain_Grid"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_FreeCamera"))))
		return E_FAIL;



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
	CIMGui_Manager::GetInstance()->Frame();
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

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring& _strLayerTag)
{
	/* 원형객체를 복제하여 사본객체를 생성하고 레이어에 추가한다. */
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 구조체 생성
	CFree_Camera::FREE_CAMERA_DESC FreeCameraDesc;
	ZeroMemory(&FreeCameraDesc, sizeof FreeCameraDesc);
	
	FreeCameraDesc.fMouseSensitive = 0.2f;
	FreeCameraDesc.vEye = _float4(0.f, 10.f, -8.f, 1.f);
	FreeCameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	FreeCameraDesc.fFovY = XMConvertToRadians(60.f);
	FreeCameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	FreeCameraDesc.fNear = 0.2f;
	FreeCameraDesc.fFar = 1100.f;
	FreeCameraDesc.fSpeedPerSec = 80.f;
	FreeCameraDesc.fRotationRadianPerSec = XMConvertToRadians(90.f);

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_FreeCamera"), &FreeCameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

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

HRESULT CLevel_GamePlay::Ready_Layer_Playher(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("ProtoType_GameObject_Player"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
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
