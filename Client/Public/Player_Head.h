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

class CPlayer_Head final : public CPlayerPart_Base
{
private:
	CPlayer_Head(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayer_Head(const CPlayer_Head& rhs);
	virtual ~CPlayer_Head() = default;

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
private:
	HRESULT Ready_Component();
	HRESULT Bind_ShaderResource();

public:
	static CPlayer_Head* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

