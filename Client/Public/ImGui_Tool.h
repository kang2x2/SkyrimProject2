#pragma once

#include "Client_Defines.h"
#include "ImGui_Base.h"

BEGIN(Client)

class CImGui_Tool final : public CImGui_Base
{
private:
	typedef struct tagAfterFileDesc
	{
		const char* m_strFileName;
		const char* m_strFBXPath;
		wstring m_strObjTag;
		wstring m_strComTag;
		_bool	m_bCheck = false;
	}TOOL_AFTER_FILEDESC;

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
	// Object Create / Delete
	void	LayOut_Object_CreateDelete();

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

	// Create Object
	void	Create_Object();

	// Save / Load
	void    File_Save();
	void    File_Load();

public:
	void    Set_ImGuiActive(_bool _isActive) { m_bActive = _isActive; }
	_bool	Get_ImGuiActive() { return m_bActive; }

private:
	// 장치
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

	// 선택된 오브젝트
	class CGameObject* m_pObject = nullptr;

	// mouse
	_float3 ResultPickPos = _float3();

	_bool m_bActive = true;

	// 파일 검색 불리언
	_bool m_bFindFolder = true;
	_bool m_bFindFBX = true;

	// 파일 확장자 불리언
	_bool showFBXContent = true;
	_bool showDDSContent = false;
	_bool showWICContent = false;

	// 파일 배열
	vector<wstring> m_vecBeforeFolderList; // 검색된 폴더 이름 저장.
	vector<const char*> m_vecAfterFolderList; // 검색된 폴더 이름 const char* 형변환.

	vector<wstring> m_vecFilePathList; // 파일 경로 저장.
	vector<wstring> m_vecBeforeFileList; // 변환 전 파일 이름 리스트
	vector<TOOL_AFTER_FILEDESC> m_vecAfterFileList; // 변환 후 파일 정보 리스트

	// 현재 list에서 선택된 파일의 경로 저장.

public:
	static CImGui_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

};

END