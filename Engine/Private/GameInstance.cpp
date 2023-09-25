#include "GameInstance.h"

#include "Timer_Manager.h"
#include "Graphic_Device.h"
#include "Ray_Manager.h"
#include "Level_Manager.h"
#include "Object_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device(CGraphic_Device::GetInstance())
	, m_pTimer_Manager(CTimer_Manager::GetInstance())
	, m_pInput_Device(CInput_Device::GetInstance())
	, m_pRay_Manager(CRay_Manager::GetInstance())
	, m_pLevel_Manager(CLevel_Manager::GetInstance())
	, m_pObject_Manager(CObject_Manager::GetInstance())
	, m_pComponent_Manager(CComponent_Manager::GetInstance())
	, m_pPipeLine(CPipeLine::GetInstance())
	
{
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pRay_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pPipeLine);
}

/* GameInstance*/
HRESULT CGameInstance::Initialize_Engine(_uint _iLevelIndex, HINSTANCE hInstance, const GRAPHIC_DESC& _GraphicDesc, _Inout_ ID3D11Device** _ppDevice, _Inout_ ID3D11DeviceContext** _ppContext)
{
	// �׷��� ����̽� �ʱ�ȭ
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(_GraphicDesc.hWnd, _GraphicDesc.eWinMode, _GraphicDesc.iWinSizeX, _GraphicDesc.iWinSizeY, _ppDevice, _ppContext)))
		return E_FAIL;
	
	// �Է� ����̽� �ʱ�ȭ
	if (FAILED(m_pInput_Device->Ready_Input_Device(hInstance, _GraphicDesc.hWnd)))
		return E_FAIL;

	// ���� ����̽� �ʱ�ȭ

	// ������Ʈ �Ŵ��� Reserve
	if (FAILED(m_pObject_Manager->Reserve_Manager(_iLevelIndex)))
		return E_FAIL;

	// ������Ʈ �Ŵ��� Reserve
	if (FAILED(m_pComponent_Manager->Reserve_Manager(_iLevelIndex)))
		return E_FAIL;

	// ���������� Initialize
	if(FAILED(m_pPipeLine->Initialize()))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick(_float _fTimeDelta)
{	
	m_pInput_Device->Update_InputDev();

	m_pObject_Manager->Tick(_fTimeDelta);
	m_pLevel_Manager->Tick(_fTimeDelta);

	m_pPipeLine->Tick();

	m_pObject_Manager->LateTick(_fTimeDelta);
	m_pLevel_Manager->LateTick(_fTimeDelta);
}

void CGameInstance::AfterRender()
{
	m_pLevel_Manager->AfterRender();
}

void CGameInstance::Clear(_uint _iLevelIndex)
{
	m_pObject_Manager->Clear(_iLevelIndex);
}

/* Graphic Device */
HRESULT CGameInstance::Clear_BackBuffer_View(_float4 _vClearColor)
{
	if (m_pGraphic_Device == nullptr)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(_vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (m_pGraphic_Device == nullptr)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (m_pGraphic_Device == nullptr)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(CInput_Device::MOUSEKEYSTATE eMouse)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

/* Timer Manager */
_float CGameInstance::Compute_TimeDelta(const wstring& strTimerTag)
{
	if (m_pTimer_Manager == nullptr)
		return 0.f;

	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Add_Timer(const wstring& strTimerTag)
{
	if (m_pTimer_Manager == nullptr)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

// Ray Manager
_float3 CGameInstance::Return_WorldMousePos(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const POINT& _WinMousePos) const
{
	if (m_pRay_Manager == nullptr)
		return _float3();

	return m_pRay_Manager->Return_WorldMousePos(_pDevice, _pContext, _WinMousePos);
}

/* Level Manager */
HRESULT CGameInstance::Open_Level(_uint _iLevelIndex, CLevel* _pLevel)
{
	if (m_pLevel_Manager == nullptr)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(_iLevelIndex, _pLevel);
}

/* Object Manager */
HRESULT CGameInstance::Add_ProtoObject(const wstring& _strProtoTypeTag, CGameObject* _pGameObject)
{
	if (m_pObject_Manager == nullptr)
		return E_FAIL;

	return m_pObject_Manager->Add_ProtoObject(_strProtoTypeTag, _pGameObject);
}

HRESULT CGameInstance::Add_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strProtoTypeTag, void* pArg)
{
	if (m_pObject_Manager == nullptr)
		return E_FAIL;

	return m_pObject_Manager->Add_CloneObject(_iLevelIndex, _strLayerTag, _strProtoTypeTag, pArg);
}

/* Component Manager */
HRESULT CGameInstance::Add_ProtoType_Component(_uint _iLevelIndex, const wstring& _strProtoTypeTag, CComponent* _pProtoTypeComponent)
{
	if (m_pComponent_Manager == nullptr)
		return E_FAIL;

	return m_pComponent_Manager->Add_ProtoType_Component(_iLevelIndex, _strProtoTypeTag, _pProtoTypeComponent);
}

CComponent* CGameInstance::Clone_Component(_uint _iLevelIndex, const wstring& _strProtoTypeTag, void* _pArg)
{
	if (m_pComponent_Manager == nullptr)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(_iLevelIndex, _strProtoTypeTag, _pArg);
}

/* PipeLine */
HRESULT CGameInstance::Bind_TransformToShader(CShader* _pShader, const char* _pConstantName, CPipeLine::TRANSFORMSTATE _eState)
{
	return m_pPipeLine->Bind_TransformToShader(_pShader, _pConstantName, _eState);
}

_float4x4 CGameInstance::Get_Transform_float4x4(CPipeLine::TRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Matrix(eState);
}

_float4x4 CGameInstance::Get_Transform_float4x4_Inverse(CPipeLine::TRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_float4x4_Inverse(eState);
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Matrix_Inverse(eState);
}

_float4 CGameInstance::Get_CamPosition_Float4() const
{
	return m_pPipeLine->Get_CamPosition_Float4();
}

_vector CGameInstance::Get_CamPosition_Vector() const
{
	return m_pPipeLine->Get_CamPosition_Vector();
}

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->DestroyInstance();
	CPipeLine::GetInstance()->DestroyInstance();
	CLevel_Manager::GetInstance()->DestroyInstance();
	CObject_Manager::GetInstance()->DestroyInstance();
	CComponent_Manager::GetInstance()->DestroyInstance();
	CTimer_Manager::GetInstance()->DestroyInstance();
	CInput_Device::GetInstance()->DestroyInstance();
	CRay_Manager::GetInstance()->DestroyInstance();
	CGraphic_Device::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pRay_Manager);
	Safe_Release(m_pPipeLine);
}