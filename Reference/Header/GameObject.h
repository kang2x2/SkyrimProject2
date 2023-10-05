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
	const wstring& Get_Name() { return m_strName; } // 클론으로 생성된 오브젝트들은 모두 고유한 이름을 가진다.
	_bool Get_IsHasMesh() { return m_bHasMesh; } // 메시를 가지는 오브젝트인지 확인한다.
	class CComponent* Get_Component(const wstring& _strComponentName); // 가지고 있는 컴포넌트 정보를 반환.

	_bool Get_IsDead() { return m_bDead; } // 삭제될 오브젝트인지 확인한다.
	void  Set_IsDead(_bool _bDead) { m_bDead = _bDead; } // 삭제 여부를 설정한다.

	// 자신이 속한 레이어 태그 저장 및 레이어 태그 반환.
	void			Set_HasLayerTag(const wstring& _strLayerTag) { m_strHasLayerTag = _strLayerTag; }
	const wstring&  Get_HasLayerTag() { return m_strHasLayerTag; }

public: /* 파일 저장과 로드를 담당하는 함수들 */
	virtual HRESULT Object_FileSave(wofstream& outFile) const;
	virtual HRESULT Object_FileLoad(std::ifstream& inFile);
	

protected:
	wstring					m_strName = TEXT(""); // 고유한 이름을 가지고 있어야 탐색이 용이 할 것 같다.
	wstring					m_strHasLayerTag = TEXT(""); // 자신이 속한 레이어를 알고 있어야 삭제가 빠를 것 같다.
	wstring					m_strModelComTag = TEXT(""); // 자신이 가지고 있는 모델 컴포넌트의 원본 태그
	_bool					m_bHasMesh = false; // 메시를 가지고 있는 객체인지 판별이 필요 할 것 같다.

	// 삭제를 위해 고유 인덱스를 주는 것 보단 bool 값으로 현재 선택된 상태를 전달하자.
	// 같은 이름의 오브젝트가 많을 것이기에.
	_bool					m_bDead = false;

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

