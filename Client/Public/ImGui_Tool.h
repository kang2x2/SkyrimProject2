#pragma once

#include <filesystem>
#include <set>
#include <fstream>
#include <commdlg.h>

#include "Client_Defines.h"
#include "ImGui_Base.h"
#include "imgui.h"

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
	void	LayOut_Object();
	// Save / Load
	void	LayOut_SaveLoad();
	// Object Create / Delete
	void	LayOut_Object_PickMode();

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

	// Create Object
	HRESULT	Create_Object();
	HRESULT	Delete_Object();
	HRESULT	Select_Object();

	// Save / Load
	void    File_Save();
	void    File_Load();

	// RangeCheck(���̾ƿ� ���� �ۿ����� ��ŷ �����ϵ���)
	void    Add_LayOut_Array(const char* _strName, ImVec2 _LayOutPos, ImVec2 _LayOutSize);
	_bool   Check_LayOut_Range(POINT _MousePos);

public:
	void    Set_ImGuiActive(_bool _isActive) { m_bActive = _isActive; }
	_bool	Get_ImGuiActive() { return m_bActive; }

private:
	// ��ġ
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

	// mouse
	_float3 ResultPickPos = _float3();

	_bool m_bActive = true;

	// ���� �˻� �Ҹ���
	_bool m_bFindFolder = true;
	_bool m_bFindFBX = true;

	// ���� Ȯ���� �Ҹ���
	_bool showFBXListShow = true;

	// ���� �迭
	set<filesystem::path> visitedDirectories;

	vector<wstring> m_vecBeforeFolderList; // �˻��� ���� �̸� ����.
	vector<const char*> m_vecAfterFolderList; // �˻��� ���� �̸� const char* ����ȯ.

	vector<wstring> m_vecBeforeFileList; // ��ȯ �� ���� �̸� ����Ʈ
	vector<TOOL_AFTER_FILEDESC> m_vecAfterFileList; // ��ȯ �� ���� ���� ����Ʈ

	// ���� list���� ���õ� ���� ������ ����.
	TOOL_AFTER_FILEDESC* m_pCurFile = nullptr;
	wstring				 m_strCurLayerTag = TEXT(""); // ���� ���õ� �������� ���̾� �±׷�.
	// ���õ� ������Ʈ
	class CGameObject*   m_pSelectObject = nullptr;

	// ����, ����, ���� ���
	_bool				 m_bCreateMode = false;
	_bool				 m_bDeleteMode = false;
	_bool				 m_bSelectMode = false;	
	_bool				 m_bDelete = false; // gui�� ��� �۾��� ������ ��ü�� �����ϱ� ����.

	// ���̾ƿ� ���� �ۿ����� Ŭ���̾�Ʈ �۾��� �����ϱ� ����.
	vector<TOOL_LAYOUTDESC> m_vecLayOut;

public:
	static CImGui_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

};

END