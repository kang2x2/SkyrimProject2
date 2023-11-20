#pragma once

#include "Client_Defines.h"
#include "CarlottaPart_Base.h"

BEGIN(Engine)

class CBone;
class CModel;
class CRenderer;
class CShader;
class CCollider;
class CTransform;

END

BEGIN(Client)

class CCarlotta_Foot final : public CCarlottaPart_Base
{
private:
	CCarlotta_Foot(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCarlotta_Foot(const CCarlotta_Foot& rhs);
	virtual ~CCarlotta_Foot() = default;

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

private:
	CCollider* m_pColliderCom = nullptr;

public:
	static CCarlotta_Foot* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

