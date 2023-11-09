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
	typedef struct tagBeforeFolderDesc
	{
		wstring strFolderName;
		wstring strType;
	}TOOL_BEFORE_FOLDERDESC;

	typedef struct tagAfterFolderDesc
	{
		const char* strFolderName;
		wstring strType;
	}TOOL_AFTER_FOLDERDESC;

	typedef struct tagAfterFileDesc
	{
		const char* m_strFileName;
		wstring m_strObjTag;
		wstring m_strComTag = TEXT("");
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

public:
	enum SAVE_OBJTYPE { SAVETYPE_STATIC, SAVETYPE_DYNAMIC, SAVETYPE_END };
	enum TOOL_LIGHT_TYPE { TLIGHT_TOWN, TLIGHT_FIRE, TLIGHT_END };
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

	// Light Content
	void	LayOut_Light();

	// FindFile
	void	FindFolder();
	void	Ready_FindFBX(TOOL_AFTER_FOLDERDESC _tFileDesc);
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

	// Custom Light
	HRESULT Create_Light();
	HRESULT Delete_Light();

	void	Key_Input(class CTransform* _pTransform);

	// Save / Load
	void    ObjFile_Save(SAVE_OBJTYPE _eType);
	void    ObjFile_Load();
	void	CellFile_Save();
	void	CellFile_Load();
	void	LightFile_Save();
	void	LightFile_Load();

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

	vector<TOOL_BEFORE_FOLDERDESC> m_vecBeforeFileDesc; // �˻��� ���� �̸� ����.
	vector<TOOL_AFTER_FOLDERDESC> m_vecAfterFolderDesc; // �˻��� ���� �̸� const char* ����ȯ.

	vector<wstring> m_vecBeforeFileList; // ��ȯ �� ���� �̸� ����Ʈ
	vector<TOOL_AFTER_FILEDESC> m_vecAfterFileDesc; // ��ȯ �� ���� ���� ����Ʈ

	// ���� list���� ���õ� ���� ������ ����.
	TOOL_AFTER_FILEDESC* m_pCurFile = nullptr;
	wstring				 m_strCurLayerTag = TEXT(""); // ���� ���õ� �������� ���̾� �±׷�.
	// ���õ� ������Ʈ
	class CGameObject*   m_pSelectObject = nullptr;

	// Light ����, ���� ���� ���
	_bool				 m_bLightCreateMode = false;
	_bool				 m_bLightDeleteMode = false;

	// Object ����, ����, ���� ���
	_bool				 m_bObjCreateMode = false;
	_bool				 m_bObjDeleteMode = false;
	_bool				 m_bObjSelectMode = false;	
	_bool				 m_bDelete = false; // gui�� ��� �۾��� ������ ��ü�� �����ϱ� ����.

	_float			     m_fRotValue = 0.f; // ������Ʈ ȸ���� ���� ����.

	// Navigation Cell 
	_bool				 m_bCellCreateMode = false;

	_uint					 m_iCellClickIdx = 0;
	TOOL_CELLDESC			 m_CellPoint;
	CNavigation*			 m_pNavigationCom = nullptr;


	// Light
	string m_lightNameAry[TLIGHT_END] = { "Town", "Fire" };

	// ���̾ƿ� ���� �ۿ����� Ŭ���̾�Ʈ �۾��� �����ϱ� ����.
	vector<TOOL_LAYOUTDESC> m_vecLayOut;

public:
	static CImGui_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

};

END