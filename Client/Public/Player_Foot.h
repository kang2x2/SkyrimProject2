#pragma once

#include "Client_Defines.h"
#include "PlayerPart_Base.h"

BEGIN(Engine)

class CBone;
class CModel;
class CRenderer;
class CShader;
class CTransform;

END

BEGIN(Client)

class CPlayer_Foot final : public CPlayerPart_Base
{
private:
	CPlayer_Foot(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayer_Foot(const CPlayer_Foot& rhs);
	virtual ~CPlayer_Foot() = default;

public:
	_bool Get_IsAnimationFin();
	string Get_CurAnimationName();

public:
	virtual HRESULT Initialize_ProtoType(); // ����
	virtual HRESULT Initialize_Clone(void* pArg); // �纻
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	void		Set_AnimationIndex(_bool _bIsLoop, string _strAnimationName, _uint _iChangeIndex);
	_uint		Get_CurFrameIndex();
	_bool		Get_CurAnimationIsLoop();

	void		Change_Equip(CGameObject* _pItem);
private:
	HRESULT Ready_Component();
	HRESULT Bind_ShaderResource();

private:
	CModel* m_pBasicModel = nullptr;

public:
	static CPlayer_Foot* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

