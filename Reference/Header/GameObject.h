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
	// Ŭ������ ������ ������Ʈ���� ��� ������ �̸��� ������.
	const wstring& Get_Name() { return m_strName; }
	class CComponent* Get_Component(const wstring& _strComponentName);

protected:
	// ������ �̸��� ������ �־�� Ž���� ���� �� �� ����.
	wstring					m_strName = TEXT("");

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

