#pragma once

#include "Client_Defines.h"
#include "ImGui_Base.h"

BEGIN(Client)

class CImGui_Tool final : public CImGui_Base
{
private:
	typedef struct tagFileDesc
	{
		const char* m_strFileName;
		_bool	m_bCheck = false;
	}TOOL_FILEDESC;

private:
	CImGui_Tool();
	virtual ~CImGui_Tool() = default;

public:
	virtual HRESULT Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void	Frame();

private:
	// Mouse
	void	LayOut_Mouse();
	// main
	void	LayOut_Object();
	// Save / Load
	void	LayOut_SaveLoad();

	// Object Content
	void    LayOut_Object_FBX();
	void    LayOut_Object_DDS();
	void    LayOut_Object_WIC();

	// FindFile
	void	FindFolder();
	void    FindFile_FBX(const char* _FolderName);

	// Empty Folder & File
	void	Empty_Content();

	// Change Type
	void    ChangeType_Folder();
	void    ChangeType_File();

	// Save / Load
	void    File_Save();
	void    File_Load();

public:
	void    Set_ImGuiActive(_bool _isActive) { m_bActive = _isActive; }
	_bool	Get_ImGuiActive() { return m_bActive; }

private:
	// 장치
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

	// mouse
	_float3 ResultPickPos = _float3();

	_bool m_bActive = true;

	// 파일 검색 불리언
	_bool m_bFindFolder = true;
	_bool m_bFindFBX	= true;

	// 파일 확장자 불리언
	_bool showFBXContent = true;
	_bool showDDSContent = false;
	_bool showWICContent = false;

	// 파일 배열
	vector<wstring> m_vecFolderList; // 검색된 폴더 이름 저장.
	vector<const char*> m_vecChangeFolderList; // 검색된 폴더 이름 const char* 형변환.

	vector<wstring> m_vecFileList;
	vector<TOOL_FILEDESC> m_vecChangeFileList;

public:
	static CImGui_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

};

END

