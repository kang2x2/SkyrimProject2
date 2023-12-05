#pragma once

#include "Base.h"

BEGIN(Engine)

// ������Ÿ�� ����
class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	// ���̾�� ����� ���Լ��� ���� �������
	typedef struct tagObjFileDesc
	{
		wstring		m_strLayerTag;
		wstring		m_strProtoObjTag;
		wstring		m_strProtoModelComTag;
	}FILE_OBJDESC;

	// ���Ѵ�� ����Ʈ ���� ���⿡
	typedef struct tagLightFileDesc
	{
		LIGHT_DESC lightDesc;
	}LIGHT_OBJDESC;

protected:
	CGameObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext); // ����
	CGameObject(const CGameObject& rhs); // �纻
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_ProtoType(); // ����
	virtual HRESULT Initialize_Clone(void* pArg); // �纻
	virtual HRESULT Initialize_Clone(_uint _iLevel, const wstring & _strModelComTag, void* pArg); // �纻
	virtual void    PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	const wstring& Get_Name() { return m_strName; } // Ŭ������ ������ ������Ʈ���� ��� ������ �̸��� ������.
	const wstring& Get_LayerTag() { return m_strLayerTag; } // Ŭ������ ������ ������Ʈ���� ��� ���̾� �̸��� ������.
	_bool Get_IsHasMesh() { return m_bHasMesh; } // �޽ø� ������ ������Ʈ���� Ȯ���Ѵ�.
	_bool Get_IsHasPart() { return m_bHasPart; } // ������ �������� �ִ� ������Ʈ���� Ȯ���Ѵ�.
	_bool Get_IsCreature() { return m_bCreature; } // ����ü����.
	class CComponent* Get_Component(const wstring& _strComponentName); // ������ �ִ� ������Ʈ ������ ��ȯ.

	_bool Get_IsDead() { return m_bDead; } // ������ ������Ʈ���� Ȯ���Ѵ�.
	_bool Get_IsReadyDead() { return m_bReadyDead; } // ���� �غ�� ������Ʈ���� Ȯ���Ѵ�.
	void  Set_IsDead(_bool _bDead) { m_bDead = _bDead; } // ���� ���θ� �����Ѵ�.
	void  Set_IsReadyDead(_bool _bReadyDead) { m_bReadyDead = _bReadyDead; } // ���� �غ� ���θ� �����Ѵ�.

	// ������Ʈ �ڽ��� ���� ���� ����ü Get, Set
	void			Set_ObjFileDesc(const wstring& _strLayerTag, const wstring& _strProtoObjTag, const wstring& _strProtoModelComTag = TEXT(""));
	FILE_OBJDESC    Get_ObjFileDesc() { return m_tObjFileDesc; }

	void			 Set_LightFileDesc(LIGHT_DESC _lightDesc)
	{
		m_tLightFileDesc.lightDesc = _lightDesc;
	}
	LIGHT_OBJDESC    Get_LightFileDesc() { return m_tLightFileDesc; }


	_float			  Get_AnimationSpeed() { return m_fAnimationSpeed; }
	void			  Set_AnimationSpeed(_float _fSpeed) { m_fAnimationSpeed = _fSpeed; }


	_bool  Get_IsMaintain() { return m_bIsMaintain; }

protected:
	wstring					m_strName = TEXT(""); // ������ �̸��� ������ �־�� Ž���� ���� �� �� ����.
	wstring					m_strModelComTag = TEXT(""); // �ڽ��� ������ �ִ� �� ������Ʈ�� ���� �±�
	wstring					m_strLayerTag = TEXT(""); // �ڽ��� ������ �ִ� ���̾� �±�.
	_bool					m_bHasMesh = false; // �޽ø� ������ �ִ� ��ü���� �Ǻ��� �ʿ� �� �� ����.
	_bool					m_bHasPart = false; // ������ �������� �ִ���? �׷��ٸ� �˻��� �� ������ �˻��ؾ� �Ѵ�.
	_bool					m_bCreature = false; // ����ü����.

	// ������ ���� ���� �ε����� �ִ� �� ���� bool ������ ���� ���õ� ���¸� ��������.
	// ���� �̸��� ������Ʈ�� ���� ���̱⿡.
	_bool					m_bReadyDead = false;
	_bool					m_bDead = false;
	_bool					m_bIsMaintain = false; // �� ��ȯ�ǵ� �������� ���� �͵�.
	

	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

	_float			 m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4		 m_ViewMatrix, m_ProjMatrix;

	// ���� ������� ���� ����� ����
	FILE_OBJDESC      m_tObjFileDesc;
	LIGHT_OBJDESC     m_tLightFileDesc;

	// �ִϸ��̼� ��� �ӵ�.
	_float							m_fAnimationSpeed = 1.f;

protected:
	/* ������Ʈ�� �˻��� �����ϰ� �ϱ� ���� map���� ����. 
	   �ٸ� ��ü�� �� ��ü�� ������Ʈ(Transform ��)�� �����ϴ� ��찡 ����ؼ� */
	map<const wstring, class CComponent*> m_mapComponent; // ������ �ƴ϶� �纻�� �����ϳ�?

protected:
	/* ������Ʈ�� �������� �����ϰ� �־ �ش� ������ ���� ������Ʈ�� �˻��ϱ� ���� ���� �ε��� �ʿ�  */
	HRESULT Add_CloneComponent(_uint _iLevelIndex, const wstring& _strProtoTypeTag, const wstring& _strComponentTag, _Inout_ CComponent** _ppOut, void* pArg = nullptr);
	
	/*  ������ �ʿ��� ������Ʈ�� ������ ��� �θ𿡰� �״�� �������ֱ� ����.
		����� ������ ��ŷ�� ���� ���.*/
	HRESULT Add_CloneComponent(const wstring& _strComponentTag, CComponent* _pCopyTargetCom, _Inout_ CComponent** _ppOut);
	class CComponent* Find_ProtoTypeComponent(const wstring& _strProtoTypeTag);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual CGameObject* Clone(_uint _iLevel, const wstring& _strModelComTag, void* _pArg);
	virtual CGameObject* LightClone(_uint _iLevel, const wstring& _strModelComTag, void* _pArg, LIGHT_DESC* _LightDesc);
	virtual void Free() override;
};

END

