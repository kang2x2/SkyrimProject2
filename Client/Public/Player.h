#pragma once

#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)

class CRenderer;
class CTransform;
class CCamera;
class CNavigation;

END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum PARTS { PART_BODY, PART_WEAPON, PART_ARMOR, PART_HELMET, PART_HAIR, PART_END };
	enum PLAYERCAMERA { CAM_FREE, CAM_BATTLE, CAM_1ST, CAM_END };
	enum PLAYER_EQUIPSTATE { EQUIP_NONEQUIP, EQUIP_ONEHAND, EQUIP_BOW, EQUIP_MAGIC, EQUIP_END };

	enum PLAYERSTATE {
		ENEQUIP_IDLE,
		ENEQUIP_RUN_F, ENEQUIP_RUN_B, ENEQUIP_RUN_L, ENEQUIP_RUN_R,

		ONEHAND_IDLE, ONEHAND_EQUIP, ONEHAND_UNEQUIP,
		ONEHAND_RUN_F, ONEHAND_RUN_B, ONEHAND_RUN_L, ONEHAND_RUN_R,
		ONEHAND_LATTACK, ONEHAND_RATTACK, ONEHAND_PATTACK, ONEHAND_RUNPOWERATTACK,
		ONEHAND_END
	};

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
	
	_bool			Get_IsAnimationFin();
	_bool			Get_CurAnimationName(string _strAnimationName);
	_uint			Get_CurFrameIndex();
	const char*		Get_CurSocketBonName();
	void			Play_Animation(_bool _bIsLoop, string _strAnimationName);
	void			Set_SoketBone(const char* _pBoneName);

	void	Set_PlayerCam(CCamera* _pCam, PLAYERCAMERA _eCamType)
	{ 
		m_pPlayerCams[_eCamType] = _pCam;
	}

	const _vector Get_PlayerCamLook();
	// void Set_CamLook(const _vector& _vPlayerLook);

	CGameObject* Get_Part(PARTS _ePart) { return m_vecPlayerPart[_ePart]; }

	PLAYER_EQUIPSTATE Get_PlayerEquipState() { return m_eEquipState; }
	void			  Set_PlayerEquipState(PLAYER_EQUIPSTATE _eState) { m_eEquipState = _eState; }

private:
	vector<class CGameObject*>		m_vecPlayerPart;

	CRenderer*						m_pRendererCom = nullptr;
	CTransform*						m_pTransformCom = nullptr;
	CNavigation*					m_pNavigationCom = nullptr;
	CCamera*						m_pPlayerCams[CAM_END];
	class CStateManager_Player*		m_pStateManager = nullptr;

	PLAYERCAMERA					m_eCurCamMode = CAM_FREE;
	_uint							m_iAnimKeyIndex = 0;

	PLAYER_EQUIPSTATE				m_eEquipState = EQUIP_END;

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

