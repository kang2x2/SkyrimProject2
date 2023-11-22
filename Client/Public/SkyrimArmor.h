#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CModel;
class CRenderer;
class CShader;
class CTransform;
class CBone;

END

BEGIN(Client)

class CSkyrimArmor abstract : public CGameObject
{
public:
	enum ARMOR_VIEWTYPE { VIEW_3ST, VIEW_1ST, VIEW_END };

protected:
	CSkyrimArmor(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSkyrimArmor(const CSkyrimArmor& rhs);
	virtual ~CSkyrimArmor() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // ����
	virtual HRESULT Initialize_Clone(void* _pArg); // �纻
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Set_ViewType(ARMOR_VIEWTYPE _eType) { m_eViewType = _eType; }

protected:
	HRESULT Ready_Component();
	HRESULT Bind_ShaderResources();

protected:
	CModel* m_pModelComAry[VIEW_END];
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CTransform* m_pTransformCom = nullptr;

	CBone* m_pSocketBone = nullptr;
	_float4x4	m_matSocketPivot;

	ARMOR_VIEWTYPE m_eViewType = VIEW_3ST;

public:
	virtual CGameObject* Clone(void* _pArg) = 0;
	virtual void Free() override;
};

END

