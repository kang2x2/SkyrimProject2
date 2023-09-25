#include "framework.h"
#include "CLevel_Tool.h"

#include "IMGui_Manager.h"
#include "GameInstance.h"

#include "Free_Camera.h"

CLevel_Tool::CLevel_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

CLevel_Tool::CLevel_Tool(const CLevel_Tool& rhs)
	: CLevel(rhs)
{
}

HRESULT CLevel_Tool::Initialize()
{
	// CIMGui_Manager 초기화
	CIMGui_Manager::GetInstance()->Initialize(m_pDevice, m_pContext, LEVEL_TOOL);

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_FreeCamera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tool::Tick(_float _fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_Tool::LateTick(_float _fTimeDelta)
{
	SetWindowText(g_hWnd, TEXT("Current Level : Tool."));

	return S_OK;
}

void CLevel_Tool::AfterRender()
{
	// ImGui 그리기
	CIMGui_Manager::GetInstance()->Frame();
}

HRESULT CLevel_Tool::Ready_Layer_Camera(const wstring& _strLayerTag)
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
	FreeCameraDesc.fFar = 600.f;
	FreeCameraDesc.fSpeedPerSec = 40.f;
	FreeCameraDesc.fRotationRadianPerSec = XMConvertToRadians(90.f);

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_TOOL, _strLayerTag, TEXT("ProtoType_GameObject_ToolCamera"), &FreeCameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;

}

HRESULT CLevel_Tool::Ready_Layer_Terrain(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_TOOL, _strLayerTag, TEXT("ProtoType_GameObject_GridTerrain"))))
		return E_FAIL;

	Safe_Release(pGameInstance);


	return S_OK;
}

CLevel_Tool* CLevel_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_Tool* pInstance = new CLevel_Tool(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Fail Create : CLevel_Tool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Tool::Free()
{
	__super::Free();

	CIMGui_Manager::GetInstance()->DestroyInstance();
}
