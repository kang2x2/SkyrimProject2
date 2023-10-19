#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CFree_Camera final : public CCamera
{
public:
	typedef struct tagFree_Camera_Desc : public CCamera::CAMERA_DESC
	{
		_float			fMouseSensitive = 0.f;
	}FREE_CAMERA_DESC;

private:
	CFree_Camera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFree_Camera(const CFree_Camera& rhs);
	virtual ~CFree_Camera() = default;

public:
	virtual HRESULT Initialize_ProtoType() override; // 원본
	virtual HRESULT Initialize_Clone(void* pArg) override; // 사본
	virtual void	Tick(_float _fTimeDelta) override;
	virtual void	LateTick(_float _fTimeDelta) override;

private:
	_float			m_fMouseSensitive = 0.f;

	_float4			PlayerPos = {};
	_float			m_fRotationX = 0.f;
	_float			m_fRotationY = 0.f;
	_float			m_fCurrotationX = 0.f;
	_float			m_fCurrotationY = 0.f;
	_float			rotationSpeed = 0.2f; // 회전 속도 (마우스 입력에 따라 조절 가능)


public:
	static CFree_Camera* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

