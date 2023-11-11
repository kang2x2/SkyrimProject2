#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)

END

BEGIN(Client)

class CCamera_Player final : public CCamera
{
public:
	typedef struct tagFREE_PLAYERCAMERA_DESC : public CCamera::CAMERA_DESC
	{
		_float			fMouseSensitive = 0.f;
	}FREE_PLAYERCAMERA_DESC;

private:
	CCamera_Player(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCamera_Player(const CCamera_Player& rhs);
	virtual ~CCamera_Player() = default;

public:
	virtual HRESULT Initialize_ProtoType() override; // 원본
	virtual HRESULT Initialize_Clone(void* pArg) override; // 사본
	void	Tick_3st(_float4x4 _matWorld, _float4 _vTargetPos, _float _fTimeDelta);
	void	Tick_1st(_float4x4 _matWorld, _float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta) override;

public:
	void Mouse_Fix();
	void Zoom(_float _fTimeDelta);

private:
	_matrix			m_matAccumulateRotX = XMMatrixIdentity();
	_matrix			m_matAccumulateRotY = XMMatrixIdentity();
	_matrix			matWorld = XMMatrixIdentity();

	_float3			m_vRelativeCamPos = {};

	_float			m_fMouseSensitive = 0.f;
	_float			rotationSpeed = 0.1f; // 회전 속도 (마우스 입력에 따라 조절 가능)

	_bool			m_bIsChangeCamMode = false;
public:
	static CCamera_Player* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};
END

