#pragma once

#include "Base.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Reserve_Manager(_uint _iLevelCount);
	HRESULT Add_ProtoObject(const wstring& _strProtoTypeTag, class CGameObject* _pGameObject);
	HRESULT Add_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strProtoTypeTag, void* pArg);
	HRESULT Add_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strProtoTypeTag, const wstring& _strModelComTag, void* pArg);
	void Tick(_float _fTimeDelta);
	void LateTick(_float _fTimeDelta);
	void Clear(_uint _iLevelIndex);

public:
	class CGameObject* Find_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strName);

private:
	// 원본
	map<const wstring, class CGameObject*> m_mapProtoTypeObj;
	// 사본(레벨별로 그룹지어 레이어로 관리)
	map<const wstring, class CLayer*>*	   m_mapLayer = nullptr;
	_uint								   m_iLevelNum = 0;

private:
	class CGameObject* Find_ProtoObject(const wstring& _strPrototypeTag);
	class CLayer*	   Find_Layer(_uint _iLevelIndex, const wstring& _strLayerTag);

public:
	virtual void Free() override;
};

END

