#include "GameInstance.h"

#include "Timer_Manager.h"
#include "Graphic_Device.h"
#include "Calculator.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Light_Manager.h"
#include "Collision_Manager.h"
#include "Target_Manager.h"
#include "MyFile_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device(CGraphic_Device::GetInstance())
	, m_pTimer_Manager(CTimer_Manager::GetInstance())
	, m_pInput_Device(CInput_Device::GetInstance())
	, m_pCalculator(CCalculator::GetInstance())
	, m_pLevel_Manager(CLevel_Manager::GetInstance())
	, m_pComponent_Manager(CComponent_Manager::GetInstance())
	, m_pObject_Manager(CObject_Manager::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
	, m_pCollision_Manager(CCollision_Manager::GetInstance())
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
	, m_pPipeLine(CPipeLine::GetInstance())
	, m_pMyFile_Manager(CMyFile_Manager::GetInstance())
{
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pCalculator);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pCollision_Manager);
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pMyFile_Manager);
}

/* GameInstance*/
HRESULT CGameInstance::Initialize_Engine(_uint _iLevelIndex, HINSTANCE hInstance, const GRAPHIC_DESC& _GraphicDesc, _Inout_ ID3D11Device** _ppDevice, _Inout_ ID3D11DeviceContext** _ppContext)
{
	// 그래픽 디바이스 초기화
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(_GraphicDesc.hWnd, _GraphicDesc.eWinMode, _GraphicDesc.iWinSizeX, _GraphicDesc.iWinSizeY, _ppDevice, _ppContext)))
		return E_FAIL;
	
	// 입력 디바이스 초기화
	if (FAILED(m_pInput_Device->Ready_Input_Device(hInstance, _GraphicDesc.hWnd)))
		return E_FAIL;

	// 사운드 디바이스 초기화

	// 오브젝트 매니저 Reserve
	if (FAILED(m_pObject_Manager->Reserve_Manager(_iLevelIndex)))
		return E_FAIL;

	// 컴포넌트 매니저 Reserve
	if (FAILED(m_pComponent_Manager->Reserve_Manager(_iLevelIndex)))
		return E_FAIL;

	// 파이프라인 Initialize
	if(FAILED(m_pPipeLine->Initialize()))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick(_float _fTimeDelta)
{	
	m_pInput_Device->Update_InputDev();

	m_pObject_Manager->PriorityTick(_fTimeDelta);

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

/* For. Input Device */
_bool CGameInstance::Get_DIKeyDown(_ubyte byKeyID)
{
	if (m_pInput_Device == nullptr)
		return false;

	return m_pInput_Device->Get_DIKeyDown(byKeyID);
}

_bool CGameInstance::Get_DIKeyUp(_ubyte byKeyID)
{
	if (m_pInput_Device == nullptr)
		return false;

	return m_pInput_Device->Get_DIKeyUp(byKeyID);
}

_bool CGameInstance::Get_DIKeyPress(_ubyte byKeyID)
{
	if (m_pInput_Device == nullptr)
		return false;

	return m_pInput_Device->Get_DIKeyPress(byKeyID);
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

_bool CGameInstance::Get_DIMouseDown(CInput_Device::MOUSEKEYSTATE eMouse)
{
	if (m_pInput_Device == nullptr)
		return false;

	return m_pInput_Device->Get_DIMouseDown(eMouse);
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

// Calculator
_float3 CGameInstance::Picking_Position(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const POINT& _WinMousePos, CGameObject* _pTerrain, const _float3* _vec, _uint _iLevel)
{
	if (m_pCalculator == nullptr)
		return _float3();

	return m_pCalculator->Picking_Position(_pDevice, _pContext, _WinMousePos, _pTerrain, _vec, _iLevel);
}

CGameObject* CGameInstance::Picking_Object(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const POINT& _WinMousePos, _uint _iLevel)
{
	if (m_pCalculator == nullptr)
		return nullptr;

	return m_pCalculator->Picking_Object(_pDevice, _pContext, _WinMousePos, _iLevel);
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

HRESULT CGameInstance::Add_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strProtoTypeTag, const wstring& _strModelComTag, void* pArg)
{
	if (m_pObject_Manager == nullptr)
		return E_FAIL;

	return m_pObject_Manager->Add_CloneObject(_iLevelIndex, _strLayerTag, _strProtoTypeTag, _strModelComTag, pArg);

}

CGameObject* CGameInstance::Add_ClonePartObject(const wstring& _strPrototypeTag, void* _pArg)
{
	if (m_pObject_Manager == nullptr)
		return nullptr;

	return m_pObject_Manager->Add_ClonePartObject(_strPrototypeTag, _pArg);
}

HRESULT CGameInstance::Delete_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strName)
{
	if (m_pObject_Manager == nullptr)
		return E_FAIL;

	return m_pObject_Manager->Delete_CloneObject(_iLevelIndex, _strLayerTag, _strName);
}

CGameObject* CGameInstance::Find_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strName)
{
	if (m_pObject_Manager == nullptr)
		return nullptr;

	return m_pObject_Manager->Find_CloneObject(_iLevelIndex, _strLayerTag, _strName);
}

map<const wstring, class CLayer*>* CGameInstance::Get_CloneObjectMapAry(_uint _iLevel)
{
	if (m_pObject_Manager == nullptr)
		return nullptr;

	return m_pObject_Manager->Get_CloneObjectMapAry(_iLevel);
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

CComponent* CGameInstance::Find_ProtoType(_uint _iLevelIndex, const wstring& _strProtoTypeTag)
{
	if (m_pComponent_Manager == nullptr)
		return nullptr;

	return m_pComponent_Manager->Find_ProtoType(_iLevelIndex, _strProtoTypeTag);
}

/* Light Manager */
const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint _iLightIndex)
{
	if (m_pLight_Manager == nullptr)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(_iLightIndex);
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& _LightDesc)
{
	if (m_pLight_Manager == nullptr)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(_LightDesc);
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

/* Collision Manager */
void CGameInstance::Collision_AABBTransition(CCollider* _pCollider, CCollider* _pTargetCollider)
{
	m_pCollision_Manager->Collision_AABBTransition(_pCollider, _pTargetCollider);
}

_bool CGameInstance::Collision_ColCheck(CCollider* _pCollider, CCollider* _pTargetCollider)
{
	return m_pCollision_Manager->Collision_ColCheck(_pCollider, _pTargetCollider);
}

/* File Manager */
HRESULT CGameInstance::StaticObject_FileSave(ofstream& _outFile, _uint _iLevelIndex) const
{
	if (m_pMyFile_Manager == nullptr)
		return E_FAIL;

	return m_pMyFile_Manager->StaticObject_FileSave(_outFile, _iLevelIndex);
}

HRESULT CGameInstance::DynamicObject_FileSave(ofstream& _outFile, _uint _iLevelIndex)
{
	if (m_pMyFile_Manager == nullptr)
		return E_FAIL;

	return m_pMyFile_Manager->DynamicObject_FileSave(_outFile, _iLevelIndex);
}

HRESULT CGameInstance::Object_FileLoad(std::ifstream& _inFile, _uint _iLevelIndex)
{
	if (m_pMyFile_Manager == nullptr)
		return E_FAIL;

	return m_pMyFile_Manager->Object_FileLoad(_inFile, _iLevelIndex);
}

HRESULT CGameInstance::Cell_FileSave(ofstream& _outFile, CNavigation* _pNavigation)
{
	if (m_pMyFile_Manager == nullptr)
		return E_FAIL;

	return m_pMyFile_Manager->Cell_FileSave(_outFile, _pNavigation);
}

HRESULT CGameInstance::Cell_FileLoad(ifstream& _inFile, class CNavigation* _pNavigation)
{
	if (m_pMyFile_Manager == nullptr)
		return E_FAIL;

	return m_pMyFile_Manager->Cell_FileLoad(_inFile, _pNavigation);
}

HRESULT CGameInstance::Light_FileSave(ofstream& _outFile, _uint _iLevelIndex)
{
	if (m_pMyFile_Manager == nullptr)
		return E_FAIL;

	return m_pMyFile_Manager->Light_FileSave(_outFile, _iLevelIndex);
}

HRESULT CGameInstance::Light_FileLoad(ifstream& _inFile, _uint _iLevelIndex)
{
	if (m_pMyFile_Manager == nullptr)
		return E_FAIL;

	return m_pMyFile_Manager->Light_FileLoad(_inFile, _iLevelIndex);
}

HRESULT CGameInstance::Binary_OutFile(ofstream& _outFile, const char* _strFilePath, CModel::MODEL_TYPE _eType)
{
	if (m_pMyFile_Manager == nullptr)
		return E_FAIL;

	return m_pMyFile_Manager->Binary_OutFile(_outFile, _strFilePath, _eType);
}

CBin_AIScene* CGameInstance::Binary_InFile(const char* _strFilePath, CModel::MODEL_TYPE _eType)
{
	if (m_pMyFile_Manager == nullptr)
		return nullptr;

	return m_pMyFile_Manager->Binary_InFile(_strFilePath, _eType);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->DestroyInstance();
	CMyFile_Manager::GetInstance()->DestroyInstance();
	CPipeLine::GetInstance()->DestroyInstance();
	CTarget_Manager::GetInstance()->DestroyInstance();
	CCollision_Manager::GetInstance()->DestroyInstance();
	CLight_Manager::GetInstance()->DestroyInstance();
	CLevel_Manager::GetInstance()->DestroyInstance();
	CObject_Manager::GetInstance()->DestroyInstance();
	CComponent_Manager::GetInstance()->DestroyInstance();
	CTimer_Manager::GetInstance()->DestroyInstance();
	CInput_Device::GetInstance()->DestroyInstance();
	CCalculator::GetInstance()->DestroyInstance();
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
	Safe_Release(m_pCalculator);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pMyFile_Manager);
}
