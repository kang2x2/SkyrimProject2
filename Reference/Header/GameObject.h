#pragma once

#include "Base.h"

BEGIN(Engine)

// ������Ÿ�� ����
class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext); // ����
	CGameObject(const CGameObject& rhs); // �纻
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // ����
	virtual HRESULT Initialize_Clone(void* pArg); // �纻
	virtual HRESULT Initialize_Clone(const wstring & _strModelComTag, void* pArg); // �纻
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	const wstring& Get_Name() { return m_strName; } // Ŭ������ ������ ������Ʈ���� ��� ������ �̸��� ������.
	_bool Get_IsHasMesh() { return m_bHasMesh; } // �޽ø� ������ ������Ʈ���� Ȯ���Ѵ�.
	class CComponent* Get_Component(const wstring& _strComponentName); // ������ �ִ� ������Ʈ ������ ��ȯ.

	_bool Get_IsDead() { return m_bDead; } // ������ ������Ʈ���� Ȯ���Ѵ�.
	void  Set_IsDead(_bool _bDead) { m_bDead = _bDead; } // ���� ���θ� �����Ѵ�.

	// �ڽ��� ���� ���̾� �±� ���� �� ���̾� �±� ��ȯ.
	void			Set_HasLayerTag(const wstring& _strLayerTag) { m_strHasLayerTag = _strLayerTag; }
	const wstring&  Get_HasLayerTag() { return m_strHasLayerTag; }

public: /* ���� ����� �ε带 ����ϴ� �Լ��� */
	virtual HRESULT Object_FileSave(wofstream& outFile) const;
	virtual HRESULT Object_FileLoad(std::ifstream& inFile);
	

protected:
	wstring					m_strName = TEXT(""); // ������ �̸��� ������ �־�� Ž���� ���� �� �� ����.
	wstring					m_strHasLayerTag = TEXT(""); // �ڽ��� ���� ���̾ �˰� �־�� ������ ���� �� ����.
	wstring					m_strModelComTag = TEXT(""); // �ڽ��� ������ �ִ� �� ������Ʈ�� ���� �±�
	_bool					m_bHasMesh = false; // �޽ø� ������ �ִ� ��ü���� �Ǻ��� �ʿ� �� �� ����.

	// ������ ���� ���� �ε����� �ִ� �� ���� bool ������ ���� ���õ� ���¸� ��������.
	// ���� �̸��� ������Ʈ�� ���� ���̱⿡.
	_bool					m_bDead = false;

	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

	_float			 m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4		 m_ViewMatrix, m_ProjMatrix;
protected:
	/* ������Ʈ�� �˻��� �����ϰ� �ϱ� ���� map���� ����. 
	   �ٸ� ��ü�� �� ��ü�� ������Ʈ(Transform ��)�� �����ϴ� ��찡 ����ؼ� */
	map<const wstring, class CComponent*> m_mapComponent; // ������ �ƴ϶� �纻�� �����ϳ�?

protected:
	/* ������Ʈ�� �������� �����ϰ� �־ �ش� ������ ���� ������Ʈ�� �˻��ϱ� ���� ���� �ε��� �ʿ�  */
	HRESULT Add_CloneComponent(_uint _iLevelIndex, const wstring& _strProtoTypeTag, const wstring& _strComponentTag, _Inout_ CComponent** _ppOut,  void* pArg = nullptr);
	class CComponent* Find_ProtoTypeComponent(const wstring& _strProtoTypeTag);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual CGameObject* Clone(const wstring& _strModelComTag, void* _pArg);
	virtual void Free() override;
};

END

