#pragma once

#include "Base.h"

BEGIN(Engine)

// 프로토타입 패턴
class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext); // 원본
	CGameObject(const CGameObject& rhs); // 사본
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // 원본
	virtual HRESULT Initialize_Clone(void* pArg); // 사본
	virtual HRESULT Initialize_Clone(const wstring & _strModelComTag, void* pArg); // 사본
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	// 클론으로 생성된 오브젝트들은 모두 고유한 이름을 가진다.
	const wstring& Get_Name() { return m_strName; }
	class CComponent* Get_Component(const wstring& _strComponentName);

protected:
	// 고유한 이름을 가지고 있어야 탐색이 용이 할 것 같다.
	wstring					m_strName = TEXT("");

	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

	_float			 m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4		 m_ViewMatrix, m_ProjMatrix;
protected:
	/* 컴포넌트의 검색을 용이하게 하기 위해 map으로 보관. 
	   다른 객체가 현 객체의 컴포넌트(Transform 등)을 참조하는 경우가 빈번해서 */
	map<const wstring, class CComponent*> m_mapComponent; // 원본이 아니라 사본을 보관하네?

protected:
	/* 컴포넌트도 레벨별로 관리하고 있어서 해당 레벨의 원본 컴포넌트를 검색하기 위해 레벨 인덱스 필요  */
	HRESULT Add_CloneComponent(_uint _iLevelIndex, const wstring& _strProtoTypeTag, const wstring& _strComponentTag, _Inout_ CComponent** _ppOut,  void* pArg = nullptr);
	class CComponent* Find_ProtoTypeComponent(const wstring& _strProtoTypeTag);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual CGameObject* Clone(const wstring& _strModelComTag, void* _pArg);
	virtual void Free() override;
};

END

