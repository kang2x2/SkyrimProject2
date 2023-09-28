#pragma once

#include "Component_Manager.h"
#include "PipeLine.h"
#include "Input_Device.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.GameInstance */
	HRESULT Initialize_Engine(_uint _iLevelIndex, HINSTANCE hInstance, const GRAPHIC_DESC& _GraphicDesc, _Inout_ ID3D11Device** _ppDevice, _Inout_ ID3D11DeviceContext** _ppContext);
	void	Tick(_float _fTimeDelta);
	void	AfterRender();
	void    Clear(_uint _iLevelIndex);

public : /* For.Graphic_Device */
	HRESULT Clear_BackBuffer_View(_float4 _vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

public: /* For. InputDevice */
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(CInput_Device::MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseState);

public : /* For.Timer_Manager */
	_float	Compute_TimeDelta(const wstring & strTimerTag);
	HRESULT Add_Timer(const wstring & strTimerTag);

public: /* For.Calculator */
	_float3 Return_TransPos(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const POINT & _WinMousePos, class CGameObject* _pTerrain, const _float3 * _vec) const;

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint _iLevelIndex, class CLevel* _pLevel);

public: /* For.Object_Manager */
	HRESULT Add_ProtoObject(const wstring& _strProtoTypeTag, class CGameObject* _pGameObject);
	HRESULT Add_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strProtoTypeTag, void* pArg = nullptr);
	class CGameObject* Find_CloneObject(_uint _iLevelIndex, const wstring & _strLayerTag, const wstring & _strName);


public: /* For.Component_Manager */
	HRESULT Add_ProtoType_Component(_uint _iLevelIndex, const wstring & _strProtoTypeTag, class CComponent* _pProtoTypeComponent);
	class CComponent* Clone_Component(_uint _iLevelIndex, const wstring & _strProtoTypeTag, void* _pArg = nullptr);
	class CComponent* Find_ProtoType(_uint _iLevelIndex, const wstring & _strProtoTypeTag);

public: /* For.Light_Manager */
	const LIGHT_DESC* Get_LightDesc(_uint _iLightIndex);
	HRESULT Add_Light(const LIGHT_DESC & _LightDesc);

public: /* For. PipeLine */
	HRESULT Bind_TransformToShader(class CShader* _pShader, const char* _pConstantName, CPipeLine::TRANSFORMSTATE _eState);
	_float4x4 Get_Transform_float4x4(CPipeLine::TRANSFORMSTATE eState) const;
	_matrix Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE eState) const;
	_float4x4 Get_Transform_float4x4_Inverse(CPipeLine::TRANSFORMSTATE eState) const;
	_matrix Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORMSTATE eState) const;
	_float4 Get_CamPosition_Float4()const;
	_vector Get_CamPosition_Vector()const;

private:
	class CGraphic_Device*		m_pGraphic_Device = nullptr;
	class CInput_Device*		m_pInput_Device = nullptr;
	class CTimer_Manager*		m_pTimer_Manager = nullptr;
	class CCalculator*			m_pCalculator = nullptr;
	class CLevel_Manager*		m_pLevel_Manager = nullptr;
	class CObject_Manager*		m_pObject_Manager = nullptr;
	class CComponent_Manager*	m_pComponent_Manager = nullptr;
	class CLight_Manager*		m_pLight_Manager = nullptr;
	class CPipeLine*		    m_pPipeLine = nullptr;

public:
	static void Release_Engine();
	virtual void Free() override;
};

END
