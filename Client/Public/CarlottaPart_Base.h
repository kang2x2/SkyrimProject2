#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CBone;
class CModel;
class CRenderer;
class CShader;
class CTransform;

END

BEGIN(Client)

class CCarlottaPart_Base abstract : public CGameObject
{
public:
	typedef struct tagPartDesc
	{
		CGameObject* pParent;
		CTransform* pParentTransform;
	}PART_DESC;

protected:
	CCarlottaPart_Base(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCarlottaPart_Base(const CCarlottaPart_Base& rhs);
	virtual ~CCarlottaPart_Base() = default;

public:
	CBone* Get_SocketBonePtr(const char* _pBoneName);
	_float4x4 Get_SocketPivotMatrix();

	CTransform* Get_ParentTransform() { return m_pParentTransform; }
public:
	virtual HRESULT Initialize_ProtoType(); // 원본
	virtual HRESULT Initialize_Clone(void* _pArg); // 사본
	virtual void    PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

protected:
	HRESULT Compute_RenderMatrix(_fmatrix ChildMatrix);

protected:
	CModel* m_pModelCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CTransform* m_pTransformCom = nullptr;

	CGameObject* m_pParent = nullptr;
	CTransform* m_pParentTransform = nullptr;
	_float4x4		m_matWorld;

	/* 착용 장비 */
	CGameObject* m_pArmor = nullptr;
	CGameObject* m_pWeapon = nullptr;

public:
	static CCarlottaPart_Base* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg)override;
	virtual void Free()override;
};

END

