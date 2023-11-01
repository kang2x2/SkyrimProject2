#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLoader() = default;

public:
	const wstring Get_LoadingText() const
	{
		return m_strLoadingText;
	}

	_bool Get_Finished() const
	{
		return m_bIsFinish;
	}

public:
	// ���� ������ ���� �ε��� �����ϱ� ���� �����带 �����Ѵ�.
	HRESULT Initialize(LEVELID _eNextLevel);
	_int    Loading();

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	LEVELID					m_eNextLevel = LEVEL_END;
	wstring					m_strLoadingText = TEXT("");
	_bool					m_bIsFinish = false;

	HANDLE					m_hThread = 0;
	CRITICAL_SECTION		m_Critical_Section;

private:
	HRESULT					Loading_For_Level_Zero();
	HRESULT					Loading_For_Level_Tool();
	HRESULT					Loading_For_Level_Logo();
	HRESULT					Loading_For_Level_WhiteRun();
	HRESULT					Loading_For_Level_Dungeon();

	HRESULT					Set_ProtoType_Mesh(LEVELID _eLevel);
	HRESULT					Set_ProtoType_Object();

public:
	static CLoader* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVELID eNextLevel);
	virtual void Free() override;
};

END

