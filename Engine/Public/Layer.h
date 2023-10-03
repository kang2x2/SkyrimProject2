#pragma once

#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_CloneObject(class CGameObject* _pGameObject);
	HRESULT Delete_CloneObject(const wstring& _strName);
	void    Tick(_float _fTimeDelta);
	void    LateTick(_float _fTimeDelta);

public:
	CGameObject* Find_CloneObject(const wstring& _strName);
	list<class CGameObject*> Get_ObjList() { return m_ltCloneObj; }

private:
	list<class CGameObject*> m_ltCloneObj;

public:
	static CLayer*  Create();
	virtual void	Free() override;
};

END

