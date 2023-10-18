#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int iCurIndex = -1;
	}DESC_NAVIGATION;
private:
	CNavigation(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_ProtoType(const wstring& _strNaviMeshPath);
	virtual HRESULT Initialize_Clone(void* _pArg);
	void  Update(_fmatrix _matWorld);
	_bool IsMove(_fvector _vPoint);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	static _float4x4		m_matWorld;
	_int					m_iCurIndex = -1;
	vector<class CCell*>	m_vecCell;

#ifdef _DEBUG
private:
	class CShader*			m_pShaderCom = nullptr;
#endif

private:
	HRESULT SetUp_Neighbors();

public:
	static CNavigation* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const wstring& _strNaviMeshPath);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END

