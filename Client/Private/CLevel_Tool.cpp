#include "framework.h"
#include "CLevel_Tool.h"

#include "IMGui_Manager.h"
#include "GameInstance.h"

#include "Tool_Camera.h"

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
	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_FreeCamera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
		return E_FAIL;

	// CIMGui_Manager �ʱ�ȭ
	CIMGui_Manager::GetInstance()->Initialize(m_pDevice, m_pContext, LEVEL_TOOL);

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
	// ImGui �׸���
	CIMGui_Manager::GetInstance()->Frame();
}

HRESULT CLevel_Tool::Ready_Light()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	LIGHT_DESC LightDesc;

	/* ���⼺ ������ �߰�. */
	//ZeroMemory(&LightDesc, sizeof LightDesc);
	//LightDesc.eLightType = LIGHT_DESC::LIGHT_DIRECTIONAL;
	//LightDesc.vLightDir = _float4(1.f, -1.f, 1.f, 0.f);
	//
	//LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//
	//if (FAILED(pGameInstance->Add_Light(LightDesc)))
	//	return E_FAIL;

	/* �� ���� �߰� */
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

HRESULT CLevel_Tool::Ready_Layer_Camera(const wstring& _strLayerTag)
{
	/* ������ü�� �����Ͽ� �纻��ü�� �����ϰ� ���̾ �߰��Ѵ�. */
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// ����ü ����
	CTool_Camera::TOOL_CAMERA_DESC ToolCameraDesc;
	ZeroMemory(&ToolCameraDesc, sizeof ToolCameraDesc);

	ToolCameraDesc.fMouseSensitive = 0.2f;
	ToolCameraDesc.vEye = _float4(0.f, 10.f, -8.f, 1.f);
	ToolCameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	ToolCameraDesc.fFovY = XMConvertToRadians(60.f);
	ToolCameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	ToolCameraDesc.fNear = 0.2f;
	ToolCameraDesc.fFar = 600.f;
	ToolCameraDesc.fSpeedPerSec = 15.f;
	ToolCameraDesc.fRotationRadianPerSec = XMConvertToRadians(90.f);
	ToolCameraDesc.fZoomPerSec = 500.f;

	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_TOOL, _strLayerTag, TEXT("ProtoType_GameObject_ToolCamera"), &ToolCameraDesc)))
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
