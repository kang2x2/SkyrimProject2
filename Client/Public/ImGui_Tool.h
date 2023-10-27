#pragma once

#include <filesystem>
#include <set>
#include <fstream>
#include <commdlg.h>

#include "Client_Defines.h"
#include "ImGui_Base.h"
#include "imgui.h"

BEGIN(Engine)

class CNavigation;

END

BEGIN(Client)

class CImGui_Tool final : public CImGui_Base
{
public:
	typedef struct tagAfterFileDesc
	{
		const char* m_strFileName;
		wstring m_strObjTag;
		wstring m_strComTag;
		_bool	m_bCheck = false;
	}TOOL_AFTER_FILEDESC;

	typedef struct tagLayOutDesc
	{
		const char*  m_strName;
		ImVec2		 m_vStartLayOutPos;
		ImVec2		 m_vEndLayOutPos;
	}TOOL_LAYOUTDESC;

	typedef struct tagToolCellDesc
	{
		_float3 fCellPos[3];
	}TOOL_CELLDESC;

private:
	CImGui_Tool();
	virtual ~CImGui_Tool() = default;

public:
	virtual HRESULT Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void	Frame();

private:
	// Mouse
	HRESULT	LayOut_Mouse();
	// main
	void	LayOut_Main();
	// Save / Load
	void	LayOut_ObjSaveLoad();
	void	LayOut_CellSaveLoad();
	// Object Create / Delete
	void	LayOut_Object_PickMode();
	// Navigation
	void	LayOut_Navigation();

	// Object Content
	void    LayOut_FBX_List();
	void    LayOut_Object_Transform();

	// FindFile
	void	FindFolder();
	void	Ready_FindFBX(const char* _FolderName);
	HRESULT Find_FileFBX(const wstring& _wStrPath);

	// Empty Folder & File
	void	Empty_Content();

	// Change Type
	void    ChangeType_Folder();
	void    ChangeType_File();

	// Custom Object
	HRESULT	Create_Object();
	HRESULT	Delete_Object();
	HRESULT	Select_Object();

	void	Key_Input(class CTransform* _pTransform);

	// Save / Load
	void    ObjFile_Save();
	void    ObjFile_Load();
	void	CellFile_Save();
	void	CellFile_Load();

	// RangeCheck(레이아웃 범위 밖에서만 피킹 가능하도록)
	void    Add_LayOut_Array(const char* _strName, ImVec2 _LayOutPos, ImVec2 _LayOutSize);
	_bool   Check_LayOut_Range(POINT _MousePos);

public:
	void    Set_ImGuiActive(_bool _isActive) { m_bActive = _isActive; }
	_bool	Get_ImGuiActive() { return m_bActive; }

private:
	// 장치
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

	// mouse
	_float3 ResultPickPos = _float3();

	_bool m_bActive = true;

	// 파일 검색 불리언
	_bool m_bFindFolder = true;
	_bool m_bFindFBX = true;

	// 파일 확장자 불리언
	_bool showFBXListShow = true;

	// 파일 배열
	set<filesystem::path> visitedDirectories;

	vector<wstring> m_vecBeforeFolderList; // 검색된 폴더 이름 저장.
	vector<const char*> m_vecAfterFolderList; // 검색된 폴더 이름 const char* 형변환.

	vector<wstring> m_vecBeforeFileList; // 변환 전 파일 이름 리스트
	vector<TOOL_AFTER_FILEDESC> m_vecAfterFileList; // 변환 후 파일 정보 리스트

	// 현재 list에서 선택된 파일 변수에 저장.
	TOOL_AFTER_FILEDESC* m_pCurFile = nullptr;
	wstring				 m_strCurLayerTag = TEXT(""); // 현재 선택된 폴더명을 레이어 태그로.
	// 선택된 오브젝트
	class CGameObject*   m_pSelectObject = nullptr;

	// Object 생성, 삭제, 선택 모드
	_bool				 m_bObjCreateMode = false;
	_bool				 m_bObjDeleteMode = false;
	_bool				 m_bObjSelectMode = false;	
	_bool				 m_bDelete = false; // gui의 모든 작업이 끝나고 객체를 삭제하기 위함.

	_float			     m_fRotValue = 0.f; // 오브젝트 회전을 위한 변수.

	// Navigation Cell 
	_bool				 m_bCellCreateMode = false;

	_uint					 m_iCellClickIdx = 0;
	TOOL_CELLDESC			 m_CellPoint;
	CNavigation*			 m_pNavigationCom = nullptr;


	// 레이아웃 범위 밖에서만 클라이언트 작업을 수행하기 위함.
	vector<TOOL_LAYOUTDESC> m_vecLayOut;

public:
	static CImGui_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

};

END