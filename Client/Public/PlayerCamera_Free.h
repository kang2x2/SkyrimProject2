#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "Player.h"

BEGIN(Engine)

class CTransform;

END

BEGIN(Client)

class CPlayerCamera_Free final : public CCamera
{
public:
	typedef struct tagFREE_PLAYERCAMERA_DESC : public CCamera::CAMERA_DESC
	{
		CPlayer*		pPlayer = nullptr;
		_float			fMouseSensitive = 0.f;
	}FREE_PLAYERCAMERA_DESC;

private:
	CPlayerCamera_Free(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayerCamera_Free(const CPlayerCamera_Free& rhs);
	virtual ~CPlayerCamera_Free() = default;

public:
	virtual HRESULT Initialize_ProtoType() override; // ����
	virtual HRESULT Initialize_Clone(void* pArg) override; // �纻
	virtual void	Tick(_float _fTimeDelta) override;
	virtual void	LateTick(_float _fTimeDelta) override;

public:


private:
	_float			m_fMouseSensitive = 0.f;
	_float			rotationSpeed = 0.1f; // ȸ�� �ӵ� (���콺 �Է¿� ���� ���� ����)

	/* �÷��̾ �������� ī�޶� �����ϱ� ���� ������ */
	CPlayer*		m_pPlayer = nullptr;
	CTransform*		m_pPlayerTransform = nullptr;

	_matrix			m_matAccumulateRotX = XMMatrixIdentity();
	_matrix			m_matAccumulateRotY = XMMatrixIdentity();
	_matrix			matRot = XMMatrixIdentity();

	_float4			m_vPlayerPos = {};
	_float3			m_vRelativeCamPos = { 0.f, 4.f, -3.f };

public:
	static CPlayerCamera_Free* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

