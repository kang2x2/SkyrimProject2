#pragma once

#include "Client_Defines.h"
#include "CreatureObject.h"

BEGIN(Engine)

class CRenderer;
class CTransform;
class CNavigation;

END

BEGIN(Client)

class CPlayer final : public CCreatureObject
{
public:
	enum PLAYER_PARTS { PART_BODY, PART_CAMERA, PART_WEAPON, PART_ARMOR, PART_HELMET, PART_HAIR, PART_END };
	enum PLAYERCAMERA { CAM_3ST, CAM_1ST, CAM_END };
	enum PLAYER_EQUIPSTATE { EQUIP_UNEQUIP, EQUIP_ONEHAND, EQUIP_BOW, EQUIP_MAGIC, EQUIP_END };

	enum PLAYERSTATE {
		UNEQUIP_IDLE,
		UNEQUIP_RUN_F, UNEQUIP_RUN_B, UNEQUIP_RUN_L, UNEQUIP_RUN_R,

		ONEHAND_IDLE, ONEHAND_EQUIP, ONEHAND_UNEQUIP,
		ONEHAND_RUN_F, ONEHAND_RUN_B, ONEHAND_RUN_L, ONEHAND_RUN_R,

		ONEHAND_LATTACK, ONEHAND_RATTACK, ONEHAND_PATTACK,

		ONEHAND_LWATTACKL, ONEHAND_LWATTACKR,
		ONEHAND_RWATTACKL, ONEHAND_RWATTACKR,
		ONEHAND_FWATTACKL, ONEHAND_FWATTACKR,
		ONEHAND_BWATTACKL, ONEHAND_BWATTACKR,
		
		// fwattackr

		ONEHAND_RUNPOWERATTACK,

		ONEHAND_BLOCK, ONEHAND_ANTICIPATE, ONEHAND_BLOCKHIT,

		ONEHAND_END
	};

public:
	typedef struct PlayerSpeedDesc
	{
		_float fSprintSpeed = 4.5f;
	}PLAYER_SPEEDDESC;

private:
	CPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // 원본
	virtual HRESULT Initialize_Clone(void* pArg); // 사본
	virtual void    PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	/* 상태 관련 */
	HRESULT Set_State(PLAYERSTATE _eState);
	PLAYERSTATE	Get_CurState() { return m_eCurState; }

	PLAYER_EQUIPSTATE Get_PlayerEquipState() { return m_eEquipState; }
	void			  Set_PlayerEquipState(PLAYER_EQUIPSTATE _eState) { m_eEquipState = _eState; }

	PLAYER_SPEEDDESC   Get_PlayerSpeedDesc() { return m_tSpeedDesc; }

	/* 애니메이션 관련 */
	_bool			Get_IsAnimationFin();
	_bool			Get_CurAnimationIsLoop();
	string			Get_CurAnimationName();
	_uint			Get_CurFrameIndex();
	const char*		Get_CurSocketBonName();
	void			Play_Animation(_bool _bIsLoop, string _strAnimationName, _uint _iChangeIndex = 0);
	void			Set_SoketBone(const char* _pBoneName);

	/* 플레이어 카메라 관련*/
	PLAYERCAMERA Get_CamMode() { return m_eCurCamMode; }
	_vector		 Get_PlayerCamLook();
	void		 Set_PlayerCam(string _strAnimationName, _vector _vPos, _uint _iChangeIndex = 0, _bool _bIsLoop = false);

	/* 파츠 관련 */
	CGameObject* Get_Part(PLAYER_PARTS _ePart) { return m_vecPlayerPart[_ePart]; }


	/* 셀 관련 */
	void			  Set_CurCell();

	/* 충돌 관련*/
	void			  CheckHit_Onehand(_uint _iSourFrame, _uint _iDestFrame);



private:
	vector<class CGameObject*>		m_vecPlayerPart;

	CRenderer*						m_pRendererCom = nullptr;
	CTransform*						m_pTransformCom = nullptr;
	CNavigation*					m_pNavigationCom[STAGE_END];
	class CStateManager_Player*		m_pStateManager = nullptr;

	PLAYERCAMERA					m_eCurCamMode = CAM_3ST;
	_uint							m_iAnimKeyIndex = 0;

	PLAYER_EQUIPSTATE				m_eEquipState = EQUIP_END;
	PLAYER_SPEEDDESC				m_tSpeedDesc;
	PLAYERSTATE						m_eCurState;

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

