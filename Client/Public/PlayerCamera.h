#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "Player.h"
#include "Player_Body.h"

BEGIN(Engine)

class CTransform;
class CBone;

END

BEGIN(Client)

class CPlayerCamera final : public CCamera
{
public:
	typedef struct tagFREE_PLAYERCAMERA_DESC : public CCamera::CAMERA_DESC
	{
		CPlayer*		pPlayer = nullptr;
		_float			fMouseSensitive = 0.f;
	}FREE_PLAYERCAMERA_DESC;

private:
	CPlayerCamera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayerCamera(const CPlayerCamera& rhs);
	virtual ~CPlayerCamera() = default;

public:
	virtual HRESULT Initialize_ProtoType() override; // ����
	virtual HRESULT Initialize_Clone(void* pArg) override; // �纻
	virtual void	Tick(_float _fTimeDelta) override;
	virtual void	LateTick(_float _fTimeDelta) override;

public:
	void CameraTick_1st(_float _fTimeDelta);
	void CameraTick_3st(_float _fTimeDelta);

public:
	void Mouse_Fix();
	void Zoom(_float _fTimeDelta);

private:
	_float			m_fMouseSensitive = 0.f;
	_float			rotationSpeed = 0.1f; // ȸ�� �ӵ� (���콺 �Է¿� ���� ���� ����)

	/* �÷��̾ �������� ī�޶� �����ϱ� ���� ������ */
	CPlayer*		m_pPlayer = nullptr;
	CPlayer_Body*	m_pPlayerBody = nullptr;
	CTransform*		m_pPlayerTransform = nullptr;

	_matrix			m_matAccumulateRotX = XMMatrixIdentity();
	_matrix			m_matAccumulateRotY = XMMatrixIdentity();
	_matrix			matRot = XMMatrixIdentity();

	_float4			m_vPlayerPos = {};
	_float3			m_vRelativeCamPos = {};

	_bool			m_bIsChangeCamMode = false;

	/* ī�޶� ���� �� ī�޶� ���� ��� */
	CBone*			m_pCamBone = nullptr;
	_float4x4		m_matBodyPivot;

private:
	void	Ready_CameraDesc();

public:
	static CPlayerCamera* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

