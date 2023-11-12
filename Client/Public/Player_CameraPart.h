#pragma once

#include "Client_Defines.h"
#include "PlayerPart_Base.h"
#include "Camera_Player.h"

BEGIN(Engine)

class CTransform;
class CBone;

END

BEGIN(Client)

class CPlayer_CameraPart final : public CPlayerPart_Base
{
public:
	typedef struct tagPartDesc : public CPlayerPart_Base::PART_DESC
	{
		CBone*					pSocketBone = nullptr;
		_float4x4				matSocketPivot;
	}PLAYER_CAMERAPART_DESC;

private:
	CPlayer_CameraPart(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayer_CameraPart(const CPlayer_CameraPart& rhs);
	virtual ~CPlayer_CameraPart() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // 원본
	virtual HRESULT Initialize_Clone(void* _pArg); // 사본
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	void		Set_PivotMatrix(_float4x4 _matPivot) { m_matSocketPivot = _matPivot; }
	void		Set_SoketBone(CBone* _pBone);
	const char* Get_SoketBoneName();

	_vector		Get_PlayerCamLook() { return m_pPlayerCamera->Get_CamLook(); }

public:
	HRESULT Ready_Component();
	HRESULT Ready_Camera(void* _pArg);

private:
	class CPlayer_Body*		m_pParentBody = nullptr;
	CBone*					m_pSocketBone = nullptr;
	_float4x4				m_matSocketPivot;

	CCamera_Player*	 m_pPlayerCamera = nullptr;

public:
	static  CPlayer_CameraPart* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

