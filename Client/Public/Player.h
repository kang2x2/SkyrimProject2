#pragma once

#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)

class CTransform;
class CCamera;

END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum PARTS { PART_BODY, PART_WEAPON, PART_HAIR, PART_END };
	enum PLAYERCAMERA { CAM_FREE, CAM_BATTLE, CAM_1ST, CAM_END };
private:
	CPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // 원본
	virtual HRESULT Initialize_Clone(void* pArg); // 사본
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT Set_State(PLAYERSTATE _eState);
	
	_bool   Get_IsAnimationFin();
	void	Play_Animation(_bool _bIsLoop, string _strAnimationName);
	
	void	Set_PlayerCam(CCamera* _pCam, PLAYERCAMERA _eCamType)
	{ 
		m_pPlayerCams[_eCamType] = _pCam;
	}

	const _vector& Get_PlayerCamLook();
	// void Set_CamLook(const _vector& _vPlayerLook);

private:
	vector<class CGameObject*>		m_vecPlayerPart;

	CTransform*						m_pTransformCom = nullptr;
	CCamera*						m_pPlayerCams[CAM_END];
	class CStateManager_Player*		m_pStateManager = nullptr;

	PLAYERCAMERA					m_eCurCamMode = CAM_FREE;
	_uint							m_iAnimKeyIndex = 0;



private:
	HRESULT Ready_Part();
	HRESULT Ready_Component();
	HRESULT Ready_State();

	HRESULT Bind_ShaderResource();

public:
	static CPlayer* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

