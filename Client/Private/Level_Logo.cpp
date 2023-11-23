#include "framework.h"
#include "Level_Logo.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Logo_Camera.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	//if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Layer_LogoObj(TEXT("Layer_LogoObj"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Tick(_float _fTimeDelta)
{


	return S_OK;
}

HRESULT CLevel_Logo::LateTick(_float _fTimeDelta)
{
	SetWindowText(g_hWnd, TEXT("Current Level : Logo."));

	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return E_FAIL;
		
		Safe_Release(pGameInstance);

		int i = 0;
	}

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Camera(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CLogo_Camera::LOGO_CAMERA_DESC LogoCameraDesc;
	ZeroMemory(&LogoCameraDesc, sizeof LogoCameraDesc);

	LogoCameraDesc.vEye = _float4(0.f, 0.f, -1.f, 1.f);
	LogoCameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	LogoCameraDesc.fFovY = XMConvertToRadians(60.f);
	LogoCameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	LogoCameraDesc.fNear = 0.2f;
	LogoCameraDesc.fFar = 300.f;
	LogoCameraDesc.fSpeedPerSec = 15.f;
	LogoCameraDesc.fRotationRadianPerSec = XMConvertToRadians(90.f);
	LogoCameraDesc.fZoomPerSec = 500.f;

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_LOGO, _strLayerTag, TEXT("ProtoType_GameObject_Camera_Logo"), &LogoCameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Light()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	pGameInstance->Light_Clear();
	
	LIGHT_DESC LightDesc;
	
	/* ���⼺ ������ �߰�. */
	//ZeroMemory(&LightDesc, sizeof LightDesc);
	//LightDesc.eLightType = LIGHT_DESC::LIGHT_DIRECTIONAL;
	//LightDesc.vLightDir = _float4(0.f, 0.1f, 1.f, 0.f);
	//
	//LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(0.5f, 0.5f, 0.5f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Light(LightDesc)))
	//	return E_FAIL;
	
	/* �� ���� �߰� */
	ZeroMemory(&LightDesc, sizeof(LightDesc));
	LightDesc.eLightType = LIGHT_DESC::LIGHT_POINT;
	LightDesc.vLightPos = { -0.3f, -0.1f, -1.5f, 1.f };
	LightDesc.fLightRange = 100.f;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(0.773f, 0.973f, 0.965f, 1.f);
	
	if (FAILED(pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;
	
	
	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const wstring& _strLayerTag)
{
	/* ������ü�� �����Ͽ� �纻��ü�� �����ϰ� ���̾ �߰��Ѵ�. */
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_LOGO, _strLayerTag, TEXT("ProtoType_GameObject_BackGround"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_LogoObj(const wstring& _strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_LOGO, _strLayerTag, TEXT("ProtoType_GameObject_Logo"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Fail Create : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
