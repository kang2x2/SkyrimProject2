#include "framework.h"
#include <iostream>

#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "ImGui_Tool.h"

#include "GameInstance.h"
#include "Model.h"

#include "VIBuffer_Grid.h"
#include "Terrain_Grid.h"
#include "Light_Point.h"

// ���� ���õ� Light Name �����ϱ� ���� ���� ����
string selectedLightName;

CImGui_Tool::CImGui_Tool()
{
}

HRESULT CImGui_Tool::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	m_pDevice = _pDevice;
	m_pContext = _pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsLight();
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	m_pNavigationCom = CNavigation::Create(m_pDevice, m_pContext);

	return S_OK;
}

void CImGui_Tool::Frame()
{
	if (m_bActive)
	{
		ImGui_ImplWin32_NewFrame();
		ImGui_ImplDX11_NewFrame();
		ImGui::NewFrame();

		LayOut_Mouse();
		LayOut_Main();
		LayOut_Light();
		LayOut_ObjSaveLoad();
		LayOut_CellSaveLoad();
		LayOut_Navigation();

		if (m_bDead)
		{
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			pGameInstance->Delete_CloneObject(LEVEL_TOOL, m_pSelectObject->Get_ObjFileDesc().m_strLayerTag, m_pSelectObject->Get_Name());
			m_pSelectObject = nullptr;

			Safe_Release(pGameInstance);

			m_bDead = false;
		}

		if (m_bLightDead)
		{
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			pGameInstance->Delete_Light(dynamic_cast<CLight_Point*>(m_pSelectLight)->Get_LightIndex());

			m_pSelectLight->Set_IsReadyDead(true);
			//pGameInstance->Delete_CloneObject(LEVEL_TOOL, m_pSelectLight->Get_ObjFileDesc().m_strLayerTag, m_pSelectLight->Get_Name());
			//m_pSelectLight = nullptr;

			Safe_Release(pGameInstance);

			m_bLightDead = false;
		}
	
		m_pNavigationCom->Update();
#ifdef _DEBUG
		m_pNavigationCom->Render();
#endif
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}

HRESULT CImGui_Tool::LayOut_Mouse()
{
	POINT MousePos = {};
	GetCursorPos(&MousePos);
	ScreenToClient(g_hWnd, &MousePos);

	const char* strLayOutName = "Mouse Pos";

	ImGui::Begin(strLayOutName);

	// ���� �ۿ����� �����ϱ� ���� ���̾ƿ� ���� ����.
	Add_LayOut_Array(strLayOutName, ImGui::GetWindowPos(), ImGui::GetWindowSize());

	ImGui::Text("Cur Pos : X(%d) Y(%d)", MousePos.x, MousePos.y);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// ���̾ƿ� ���� �ۿ����� ����.
	if (pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON) && Check_LayOut_Range(MousePos))
	{
#pragma region Object
		CTerrain_Grid* pTerrainGrid = dynamic_cast<CTerrain_Grid*>(pGameInstance->Find_CloneObject(LEVEL_TOOL, TEXT("Layer_Terrain"), TEXT("Tool_GridTerrain")));

		CVIBuffer_Grid* pGridBuffer = dynamic_cast<CVIBuffer_Grid*>(pGameInstance->Find_ProtoType(LEVEL_TOOL, TEXT("ProtoType_Component_VIBuffer_Terrain_Grid")));
		const _float3* pTerrainVtxPos = pGridBuffer->Get_VtxPos();

		ResultPickPos = pGameInstance->Picking_Position(m_pDevice, m_pContext, MousePos, pTerrainGrid, pTerrainVtxPos, LEVEL_TOOL);

		if (m_bObjCreateMode)
			Create_Object();
		if (m_bObjDeleteMode)
		{
			if (!m_bDead)
			{
				m_pSelectObject = pGameInstance->Picking_Object(m_pDevice, m_pContext, MousePos, LEVEL_TOOL);

				if (m_pSelectObject != nullptr)
				{
					m_pSelectObject->Set_IsDead(true);
					m_bDead = true;
				}
			}

		}
		if (m_bObjSelectMode)
		{
			m_pSelectObject = pGameInstance->Picking_Object(m_pDevice, m_pContext, MousePos, LEVEL_TOOL);
		}
#pragma endregion

#pragma region Light

		if (m_bLightCreateMode)
			Create_Light();
		if (m_bLightDeleteMode)
		{
			if (!m_bLightDead)
			{
				m_pSelectLight = pGameInstance->Picking_Light(m_pDevice, m_pContext, MousePos, LEVEL_TOOL);

				if (m_pSelectLight != nullptr)
				{
					m_pSelectLight->Set_IsDead(true);
					m_bLightDead = true;
				}
			}
		}
		if (m_bLightSelectMode)
			m_pSelectLight = pGameInstance->Picking_Light(m_pDevice, m_pContext, MousePos, LEVEL_TOOL);

#pragma endregion

#pragma region Navigation

		if (m_bCellCreateMode)
		{
			// �� �����ض�.
			m_CellPoint.fCellPos[m_iCellClickIdx] = ResultPickPos;
			m_iCellClickIdx += 1;

			// 3�� ������� ��ƶ�.
			if (m_iCellClickIdx > 2)
			{
				m_pNavigationCom->Add_Cell(m_CellPoint.fCellPos);
				m_iCellClickIdx = 0;
			}
		}

		

#pragma endregion

	}

	if (pGameInstance->Get_DIMouseDown(CInput_Device::MKS_RBUTTON))
	{
			m_bObjCreateMode = false;
			m_bObjDeleteMode = false;
			m_bObjSelectMode = false;

			m_bLightCreateMode = false;
			m_bLightDeleteMode = false;
			m_bLightSelectMode = false;

			m_pSelectObject = nullptr;
			m_pSelectLight = nullptr;

			m_bCellCreateMode = false;

			m_iCellClickIdx = 0;
			m_fRotValue = 0.f;
	}

	ImGui::Text("Pick Pos : X(%f) Y(%f) Z(%f)", ResultPickPos.x, ResultPickPos.y, ResultPickPos.z);

	Safe_Release(pGameInstance);

	ImGui::End();

	return S_OK;
}
void CImGui_Tool::LayOut_ObjSaveLoad()
{
	const char* strLayOutName = "StaticObj Save/Load";

	ImGui::Begin(strLayOutName);

	// ���� �ۿ����� �����ϱ� ���� ���̾ƿ� ���� ����.
	Add_LayOut_Array(strLayOutName, ImGui::GetWindowPos(), ImGui::GetWindowSize());

	if (ImGui::Button("Static Save"))
	{
		ObjFile_Save(SAVETYPE_STATIC);
	}

	ImGui::SameLine();

	if (ImGui::Button("Dynamic Save"))
	{
		ObjFile_Save(SAVETYPE_DYNAMIC);
	}

	ImGui::SameLine();

	if (ImGui::Button("Load"))
	{
		ObjFile_Load();
	}

	ImGui::End();
}

void CImGui_Tool::LayOut_CellSaveLoad()
{
	const char* strLayOutName = "Cell Save/Load";

	ImGui::Begin(strLayOutName);

	// ���� �ۿ����� �����ϱ� ���� ���̾ƿ� ���� ����.
	Add_LayOut_Array(strLayOutName, ImGui::GetWindowPos(), ImGui::GetWindowSize());

	if (ImGui::Button("Save"))
	{
		CellFile_Save();
	}

	ImGui::SameLine();

	if (ImGui::Button("Load"))
	{
		CellFile_Load();
	}
	ImGui::End();
}

void CImGui_Tool::LayOut_Object_PickMode()
{
	const char* strLayOutName = "Pick Mode";

	ImGui::Begin("Pick Mode");

	// ���� �ۿ����� �����ϱ� ���� ���̾ƿ� ���� ����.
	Add_LayOut_Array(strLayOutName, ImGui::GetWindowPos(), ImGui::GetWindowSize());

	if (ImGui::Button("Create"))
	{
		//if (m_pCurFile != nullptr)
		//{
			m_bObjCreateMode = true;
			m_bObjDeleteMode = false;
			m_bObjSelectMode = false;

			m_bLightCreateMode = false;
			m_bLightDeleteMode = false;
			m_bLightSelectMode = false;

			m_pSelectObject = nullptr;

			m_bCellCreateMode = false;
		//}
	}
	if (ImGui::Button("Delete"))
	{
		m_bObjCreateMode = false;
		m_bObjDeleteMode = true;
		m_bObjSelectMode = false;

		m_bLightCreateMode = false;
		m_bLightDeleteMode = false;
		m_bLightSelectMode = false;

		m_pSelectObject = nullptr;

		m_bCellCreateMode = false;
	}
	if (ImGui::Button("Select"))
	{
		m_bObjCreateMode = false;
		m_bObjDeleteMode = false;
		m_bObjSelectMode = true;

		m_bLightCreateMode = false;
		m_bLightDeleteMode = false;
		m_bLightSelectMode = false;

		m_bCellCreateMode = false;
	}

	if (m_bObjCreateMode)
		ImGui::Text("Cur Mode : Create");
	else if (m_bObjDeleteMode)
		ImGui::Text("Cur Mode : Delete");
	else if (m_bObjSelectMode)
		ImGui::Text("Cur Mode : Select");
	else
		ImGui::Text("Cur Mode : None");

	ImGui::End();
}

void CImGui_Tool::LayOut_Navigation()
{
	const char* strLayOutName = "Navigation Mode";

	ImGui::Begin("Navigation Mode");

	// ���� �ۿ����� �����ϱ� ���� ���̾ƿ� ���� ����.
	Add_LayOut_Array(strLayOutName, ImGui::GetWindowPos(), ImGui::GetWindowSize());

	if (ImGui::Button("Create"))
	{
		m_bCellCreateMode = true;
		m_pSelectObject = nullptr;

		m_bObjCreateMode = false;
		m_bObjDeleteMode = false;
		m_bObjSelectMode = false;

		m_bLightCreateMode = false;
		m_bLightDeleteMode = false;
		m_bLightSelectMode = false;

		m_iCellClickIdx = 0;
	}
	if (ImGui::Button("Delete"))
	{
		//m_bCellCreateMode = false;
		//m_pSelectObject = nullptr;
		//
		//m_bObjCreateMode = false;
		//m_bObjDeleteMode = false;
		//m_bObjSelectMode = false;

		m_iCellClickIdx = 0;
		if (m_pNavigationCom->Get_VecCell().size() > 0)
		{
			m_pNavigationCom->Cell_PopBack();
		}
	}

	if (m_bCellCreateMode)
		ImGui::Text("Cur Mode : Create");
	else
		ImGui::Text("Cur Mode : None");

	ImGui::End();

}

void CImGui_Tool::LayOut_Main()
{
	const char* strLayOutName = "FBX Object LayOut";

	ImGui::Begin("FBX Object LayOut");

	// ���� �ۿ����� �����ϱ� ���� ���̾ƿ� ���� ����.
	Add_LayOut_Array(strLayOutName, ImGui::GetWindowPos(), ImGui::GetWindowSize());

	if (ImGui::BeginTabBar("Tabs")) // �� �׷� ����
	{
		if (ImGui::TabItemButton("FBX"))
		{
			m_bFindFolder = true;
			m_bFindFBX = true;

			showFBXListShow = true;
			showOBJListShow = false;
		}

		if (ImGui::TabItemButton("OBJ"))
		{
			m_bFindObj = true;

			showOBJListShow = true;
			showFBXListShow = false;
		}

		ImGui::EndTabBar(); // �� �׷� ����

		LayOut_FBX_List();
		LayOut_Obj_List();
		LayOut_Object_Transform();
	}

	ImGui::End();
}

// ���õ� ������ �����ϱ� ���� ��������
const char* selectedFolderName;
void CImGui_Tool::LayOut_FBX_List()
{
	if (showFBXListShow)
	{
		ImGui::Text("File List");
		ImGui::Image(nullptr, ImVec2(150, 150));

		// �� ���� ���� �˻�
		if (m_bFindFolder)
		{
			// wstring ���� �迭 ����
			m_vecBeforeFileDesc.clear();

			// Folder vector ��� ����.(strdup�� ����Ͽ��⿡ �޸� ������ �ʼ�).
			Empty_Content();

			FindFolder();
			// �˻��� ���� �̸� const char*�� ��ȯ.
			ChangeType_Folder();

			selectedFolderName = m_vecAfterFolderDesc[0].strFolderName;
		}


		// ������ 
		if (ImGui::BeginCombo(" ", selectedFolderName))
		{
			for (size_t i = 0; i < m_vecAfterFolderDesc.size(); ++i)
			{
				_bool isSelected = m_vecAfterFolderDesc[i].strFolderName == selectedFolderName;
				if (ImGui::Selectable(m_vecAfterFolderDesc[i].strFolderName, isSelected))
				{
					selectedFolderName = m_vecAfterFolderDesc[i].strFolderName; 
					// Ŭ�� �� ����� ���̾� �±�
					m_strCurLayerTag = TEXT("Layer_") + m_vecBeforeFileDesc[i].strFolderName;

					// File vector ��� ����.(strdup�� ����Ͽ��⿡ �޸� ������ �ʼ�).
					for (size_t i = 0; i < m_vecAfterFileDesc.size(); ++i)
					{
						free((void*)m_vecAfterFileDesc[i].m_strFileName);
					}
					m_vecAfterFileDesc.clear();
					m_vecBeforeFileList.clear();
					Ready_FindFBX(m_vecAfterFolderDesc[i]);
					ChangeType_File();
				}
			}
			ImGui::EndCombo();
		}

		// ���� ����Ʈ
		if (ImGui::BeginListBox("##listBox", ImVec2(215, 300)))
		{
			for (size_t i = 0; i < m_vecAfterFileDesc.size(); ++i)
			{
				// ���� �׸��� ���õǾ����� Ȯ��
				bool isSelected = m_vecAfterFileDesc[i].m_bCheck;

				if (ImGui::Selectable(m_vecAfterFileDesc[i].m_strFileName, isSelected))
				{
					// �׸��� Ŭ������ �� �ٸ� �����۵��� ���� ���¸� false�� ����.
					for (size_t i = 0; i < m_vecAfterFileDesc.size(); ++i)
					{
						if (m_vecAfterFileDesc[i].m_bCheck)
						{
							m_vecAfterFileDesc[i].m_bCheck = false;
						}
					}

					// ���� ������ �׸� true�� ����.
					m_vecAfterFileDesc[i].m_bCheck = true;
					// ���� ���õ� ���� ������ ����.
					m_pCurFile = m_vecAfterFileDesc[i];
				}
			}
			ImGui::EndListBox();
		}
	}

	LayOut_Object_PickMode();
}

void CImGui_Tool::LayOut_Obj_List()
{
	if (showOBJListShow)
	{
		ImGui::Text("File List");
		ImGui::Image(nullptr, ImVec2(150, 150));

		// �� ���� ������Ʈ �˻�
		if (m_bFindObj)
		{
			m_vecProtoObjDesc.clear();

			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			map<const wstring, CGameObject*> pTempMap = pGameInstance->Get_ProtoObjectMapAry();

			for (auto iter = pTempMap.begin(); iter != pTempMap.end(); ++iter)
			{
				if (iter->second->Get_LayerTag() == TEXT("ProtoType_NPC"))
				{
					TOOL_PROTO_OBJDESC tempDesc;
					/* �̸� �����ڵ� -> ��Ƽ����Ʈ ��ȯ */
					wstring tempName = iter->second->Get_Name();
					tempDesc.m_strFileName.assign(tempName.begin(), tempName.end());
					tempDesc.m_strObjTag = iter->first;
					tempDesc.m_bCheck = false;
					m_vecProtoObjDesc.push_back(tempDesc);
				}
			}

			Safe_Release(pGameInstance);

			m_bFindObj = false;
		}

		// ���� ����Ʈ
		if (ImGui::BeginListBox("##listBox", ImVec2(215, 300)))
		{
			for (size_t i = 0; i < m_vecProtoObjDesc.size(); ++i)
			{
				// ���� �׸��� ���õǾ����� Ȯ��
				bool isSelected = m_vecProtoObjDesc[i].m_bCheck;

				if (ImGui::Selectable(m_vecProtoObjDesc[i].m_strFileName.c_str(), isSelected))
				{
					// �׸��� Ŭ������ �� �ٸ� �����۵��� ���� ���¸� false�� ����.
					for (size_t i = 0; i < m_vecProtoObjDesc.size(); ++i)
					{
						if (m_vecProtoObjDesc[i].m_bCheck)
						{
							m_vecProtoObjDesc[i].m_bCheck = false;
						}
					}

					// ���� ������ �׸� true�� ����.
					m_vecProtoObjDesc[i].m_bCheck = true;
					// ���� ���õ� ���� ������ ����.
					//if (m_pCurFile == nullptr)
					//	m_pCurFile = new TOOL_AFTER_FILEDESC;

					m_strCurLayerTag.assign(m_vecProtoObjDesc[i].m_strFileName.begin(), m_vecProtoObjDesc[i].m_strFileName.end());
					m_pCurFile.m_strObjTag = m_vecProtoObjDesc[i].m_strObjTag;
					m_pCurFile.m_strComTag = m_vecProtoObjDesc[i].m_strComTag;
				}
			}
			ImGui::EndListBox();
		}
	}
}

void CImGui_Tool::LayOut_Object_Transform()
{
	const char* strLayOutName = "Object Transform";

	ImGui::Begin("Object Transform");

	// ���� �ۿ����� �����ϱ� ���� ���̾ƿ� ���� ����.
	Add_LayOut_Array(strLayOutName, ImGui::GetWindowPos(), ImGui::GetWindowSize());

	if (m_pSelectObject != nullptr)
	{
		Select_Object();
	}

	ImGui::End();
}

void CImGui_Tool::LayOut_Light()
{
	const char* strLayOutName = "Light Object LayOut";

	ImGui::Begin("Light Object LayOut");

	// ���� �ۿ����� �����ϱ� ���� ���̾ƿ� ���� ����.
	Add_LayOut_Array(strLayOutName, ImGui::GetWindowPos(), ImGui::GetWindowSize());

	//if (ImGui::BeginCombo(" ", selectedLightName.c_str()))
	//{
	//	for (size_t i = 0; i < TLIGHT_END; ++i)
	//	{
	//		_bool isSelected = m_lightNameAry[i] == selectedLightName;
	//		if (ImGui::Selectable(m_lightNameAry[i].c_str(), isSelected))
	//		{
	//			selectedLightName = m_lightNameAry[i].c_str();
	//			// Ŭ�� �� ����� ���̾� �±�
	//			m_strCurLayerTag = TEXT("Layer_LightObject");
	//		}
	//	}
	//
	//	ImGui::EndCombo();
	//}

	if (ImGui::Button("Create"))
	{
		m_bCellCreateMode = false;
		m_pSelectObject = nullptr;

		m_bObjCreateMode = false;
		m_bObjDeleteMode = false;
		m_bObjSelectMode = false;

		m_bLightCreateMode = true;
		m_bLightDeleteMode = false;
		m_bLightSelectMode = false;

		m_iCellClickIdx = 0;
	}

	ImGui::SameLine();

	if (ImGui::Button("Delete"))
	{
		m_bCellCreateMode = false;
		m_pSelectObject = nullptr;

		m_bObjCreateMode = false;
		m_bObjDeleteMode = false;
		m_bObjSelectMode = false;

		m_bLightCreateMode = false;
		m_bLightDeleteMode = true;
		m_bLightSelectMode = false;

		m_iCellClickIdx = 0;

	}

	if (ImGui::Button("Select"))
	{
		m_bCellCreateMode = false;
		m_pSelectObject = nullptr;

		m_bObjCreateMode = false;
		m_bObjDeleteMode = false;
		m_bObjSelectMode = false;

		m_bLightCreateMode = false;
		m_bLightDeleteMode = false;
		m_bLightSelectMode = true;

		m_iCellClickIdx = 0;
	}

	if(m_pSelectLight != nullptr)
		Select_Light();

	if (ImGui::Button("Save"))
	{
		LightFile_Save();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		LightFile_Load();
	}


	ImGui::End();
}

void CImGui_Tool::FindFolder()
{
	// ���� �˻��Ͽ� findFileData�� ����.
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile(_T("../Bin/Resource/BinaryFBX/NonAnim/*"), &findFileData);

	// ���� ó��
	if (hFind == INVALID_HANDLE_VALUE) {
		MSG_BOX("Fail Find : Skyrim Folder");
		return;
	}
	do {
		// ã���� �������� �˻�.
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (_tcscmp(findFileData.cFileName, _T(".")) != 0 && _tcscmp(findFileData.cFileName, _T("..")) != 0) {
				// ã�� ���� �̸� �߰�.
				TOOL_BEFORE_FOLDERDESC tFileDesc;
				tFileDesc.strFolderName = findFileData.cFileName;
				tFileDesc.strType = TEXT("NonAnim");
				m_vecBeforeFileDesc.push_back(tFileDesc);
			}
		}
	} while (FindNextFile(hFind, &findFileData) != 0);

	FindClose(hFind);

	////////////////////////////////////

	hFind = FindFirstFile(_T("../Bin/Resource/BinaryFBX/Anim/*"), &findFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		MSG_BOX("Fail Find : Skyrim Folder");
		return;
	}
	do {
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (_tcscmp(findFileData.cFileName, _T(".")) != 0 && _tcscmp(findFileData.cFileName, _T("..")) != 0) {
				TOOL_BEFORE_FOLDERDESC tFileDesc;
				tFileDesc.strFolderName = findFileData.cFileName;
				tFileDesc.strType = TEXT("Anim");
				m_vecBeforeFileDesc.push_back(tFileDesc);
			}
		}
	} while (FindNextFile(hFind, &findFileData) != 0);

	FindClose(hFind);
}
void CImGui_Tool::Ready_FindFBX(TOOL_AFTER_FOLDERDESC _tFileDesc)
{
	wstring wStrFindPath;
	CString FolderName = _tFileDesc.strFolderName;
	
	if(_tFileDesc.strType == TEXT("NonAnim"))
		wStrFindPath = TEXT("../Bin/Resource/BinaryFBX/NonAnim/") + FolderName + TEXT("/");
	else if(_tFileDesc.strType == TEXT("Anim"))
		wStrFindPath = TEXT("../Bin/Resource/BinaryFBX/Anim/") + FolderName + TEXT("/");

	Find_FileFBX(wStrFindPath);
}

HRESULT CImGui_Tool::Find_FileFBX(const wstring& _wStrPath)
{
	if (visitedDirectories.count(_wStrPath) > 0)
		return S_OK;

	visitedDirectories.insert(_wStrPath);

	for (const auto& entry : filesystem::directory_iterator(_wStrPath))
	{
		if (filesystem::is_directory(entry))
		{
			Find_FileFBX(entry.path());
		}
		else if (filesystem::is_regular_file(entry))
		{
			if (TEXT(".bin") == entry.path().extension())
			{
				m_vecBeforeFileList.push_back(entry.path().stem());
			}
		}
	}

	return S_OK;
}

void CImGui_Tool::Empty_Content()
{
	// Folder vector ��� ����.(strdup�� ����Ͽ��⿡ �޸� ������ �ʼ�).
	for (size_t i = 0; i < m_vecAfterFolderDesc.size(); ++i)
	{
		free((void*)m_vecAfterFolderDesc[i].strFolderName);
	}
	m_vecAfterFolderDesc.clear();
	m_vecBeforeFileDesc.clear();

	// File vector ��� ����.(strdup�� ����Ͽ��⿡ �޸� ������ �ʼ�).
	for (size_t i = 0; i < m_vecAfterFileDesc.size(); ++i)
	{
		free((void*)m_vecAfterFileDesc[i].m_strFileName);
	}
	m_vecAfterFileDesc.clear();
	m_vecBeforeFileList.clear();

	m_bFindFolder = false;
}

void CImGui_Tool::ChangeType_Folder()
{
	for (size_t i = 0; i < m_vecBeforeFileDesc.size(); ++i)
	{
		wstring_convert<codecvt_utf8<wchar_t>> converter;
		string tempStr = converter.to_bytes(m_vecBeforeFileDesc[i].strFolderName);
		
		TOOL_AFTER_FOLDERDESC tFolderDesc;
		tFolderDesc.strFolderName = strdup(tempStr.c_str());
		tFolderDesc.strType = m_vecBeforeFileDesc[i].strType;

		m_vecAfterFolderDesc.push_back(tFolderDesc); // strdup�� ����Ͽ� ����
		// strdup�� ����Ͽ� �����ϸ� �޸𸮵� ���� �Ҵ� ��. ���߿� �� ��������� ��.
	}

	m_bFindFolder = false;
}
void CImGui_Tool::ChangeType_File()
{
	for (size_t i = 0; i < m_vecBeforeFileList.size(); ++i)
	{
		wstring_convert<codecvt_utf8<wchar_t>> converter;
		string tempStr = converter.to_bytes(m_vecBeforeFileList[i]);

		TOOL_AFTER_FILEDESC tempFile;
		tempFile.m_bCheck = false;

		// ������Ʈ �±� ���ڿ� �ڸ���
		tempFile.m_strObjTag = L"ProtoType_GameObject_" + m_vecBeforeFileList[i];
		wstring wstr = tempFile.m_strObjTag;
		size_t lastDotPos = wstr.rfind(L'.'); // . ����
		if (lastDotPos != wstring::npos) { // .�������� �ڸ���.
			wstr = wstr.substr(0, lastDotPos);
		}
		tempFile.m_strObjTag = wstr;

		// ������Ʈ �±� ���ڿ� �ڸ���
		tempFile.m_strComTag = L"ProtoType_Component_Model_" + m_vecBeforeFileList[i];
		wstr = tempFile.m_strComTag;
		lastDotPos = wstr.rfind(L'.'); // . ����
		if (lastDotPos != wstring::npos) { // .�������� �ڸ���.
			wstr = wstr.substr(0, lastDotPos);
		}
		tempFile.m_strComTag = wstr;

		//
		tempFile.m_strFileName = strdup(tempStr.c_str()); // strdup�� ����Ͽ� ����
		m_vecAfterFileDesc.push_back(tempFile); 
		// strdup�� ����Ͽ� �����ϸ� �޸𸮵� ���� �Ҵ� ��. ���߿� �� ��������� ��.
	}

	m_vecBeforeFileList.clear();

	m_bFindFBX = false;
}

void CImGui_Tool::ObjFile_Save(SAVE_OBJTYPE _eType)
{
	// �۾��� �ϰ� ���� �۾��� ��ΰ� ���� ���� �Ǿ� ���� �۾� �� ������ ��.
	// �׷��� ��θ� �����س��� ��� �۾��� ������ ��θ� ���� ������ ��η� �ٽ� �ʱ�ȭ�Ѵ�.
	TCHAR originalPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, originalPath);

	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	TCHAR lpstrFile[MAX_PATH] = L"";
	static TCHAR filter[] = L"�ؽ�Ʈ ���� (*.txt)\0*.txt\0��� ���� (*.*)\0*.*\0";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = MAX_PATH;
	OFN.lpstrInitialDir = L"../Bin/SaveLoad/";;
	OFN.Flags = OFN_OVERWRITEPROMPT; // ����� ���θ� ���� ��ȭ ���ڸ� ǥ��

	if (GetSaveFileName(&OFN) != 0) {
		wsprintf(filePathName, L"%s ������ �����ϰڽ��ϱ�?", OFN.lpstrFile);
		MessageBox(g_hWnd, filePathName, L"���� ����", MB_OK);

		wstring filePath = OFN.lpstrFile;

		// ���� ������ ������ ������ ���ϴ� ������ �Ҵ�.

		// ������ ���� ���� ����.
		ofstream fileStream(filePath, ios::binary);
		if (fileStream.is_open()) {
			// ���� ������ ���Ͽ� ����.

			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			if(_eType == SAVETYPE_STATIC)
				pGameInstance->StaticObject_FileSave(fileStream, LEVEL_TOOL);
			else if(_eType == SAVETYPE_DYNAMIC)
				pGameInstance->DynamicObject_FileSave(fileStream, LEVEL_TOOL);

			Safe_Release(pGameInstance);

			fileStream.close();
			MessageBox(g_hWnd, L"������ ���������� ����Ǿ����ϴ�.", L"���� �Ϸ�", MB_OK);
		}
		else {
			MessageBox(g_hWnd, L"������ �����ϴ� �� ������ �߻��߽��ϴ�.", L"���� ����", MB_OK | MB_ICONERROR);
		}
	}

	SetCurrentDirectory(originalPath);
}
void CImGui_Tool::ObjFile_Load()
{
	// �۾��� �ϰ� ���� �۾��� ��ΰ� ���� ���� �Ǿ� ���� �۾� �� ������ ��.
	// �׷��� ��θ� �����س��� ��� �۾��� ������ ��θ� ���� ������ ��η� �ٽ� �ʱ�ȭ�Ѵ�.
	TCHAR originalPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, originalPath);

	OPENFILENAME OFN;
	TCHAR filePathName[100] = L"";
	TCHAR lpstrFile[100] = L"";
	static TCHAR filter[] = L"��� ����\0*.*\0�ؽ�Ʈ ����\0*.txt\0fbx ����\0*.fbx";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 100;
	OFN.lpstrInitialDir = L"../Bin/SaveLoad/";

	if (GetOpenFileName(&OFN) != 0) {
		wsprintf(filePathName, L"%s ������ ���ڽ��ϱ�?", OFN.lpstrFile);
		MessageBox(g_hWnd, filePathName, L"���� ����", MB_OK);

		wstring filePath = OFN.lpstrFile;

		// ������ ���� ���� ����.
		ifstream fileStream(filePath, ios::binary);
		if (fileStream.is_open()) {
			// ���� ������ �б�.

			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			pGameInstance->Object_FileLoad(fileStream ,LEVEL_TOOL);

			Safe_Release(pGameInstance);

			fileStream.close();
			MessageBox(g_hWnd, L"������ ���������� �ҷ��Խ��ϴ�.", L"�ҷ����� �Ϸ�", MB_OK);
		}
		else {
			MessageBox(g_hWnd, L"������ �ҷ����� �� ������ �߻��߽��ϴ�.", L"�ҷ����� ����", MB_OK | MB_ICONERROR);
		}
	}

	SetCurrentDirectory(originalPath);
}

void CImGui_Tool::CellFile_Save()
{
	// �۾��� �ϰ� ���� �۾��� ��ΰ� ���� ���� �Ǿ� ���� �۾� �� ������ ��.
	// �׷��� ��θ� �����س��� ��� �۾��� ������ ��θ� ���� ������ ��η� �ٽ� �ʱ�ȭ�Ѵ�.
	TCHAR originalPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, originalPath);

	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	TCHAR lpstrFile[MAX_PATH] = L"";
	static TCHAR filter[] = L"�ؽ�Ʈ ���� (*.txt)\0*.txt\0��� ���� (*.*)\0*.*\0";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = MAX_PATH;
	OFN.lpstrInitialDir = L"../Bin/SaveLoad/";;
	OFN.Flags = OFN_OVERWRITEPROMPT; // ����� ���θ� ���� ��ȭ ���ڸ� ǥ��

	if (GetSaveFileName(&OFN) != 0) {
		wsprintf(filePathName, L"%s ������ �����ϰڽ��ϱ�?", OFN.lpstrFile);
		MessageBox(g_hWnd, filePathName, L"���� ����", MB_OK);

		wstring filePath = OFN.lpstrFile;

		// ���� ������ ������ ������ ���ϴ� ������ �Ҵ�.

		// ������ ���� ���� ����.
		ofstream fileStream(filePath, ios::binary);
		if (fileStream.is_open()) {
			// ���� ������ ���Ͽ� ����.

			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			pGameInstance->Cell_FileSave(fileStream, m_pNavigationCom);

			Safe_Release(pGameInstance);

			fileStream.close();
			MessageBox(g_hWnd, L"������ ���������� ����Ǿ����ϴ�.", L"���� �Ϸ�", MB_OK);
		}
		else {
			MessageBox(g_hWnd, L"������ �����ϴ� �� ������ �߻��߽��ϴ�.", L"���� ����", MB_OK | MB_ICONERROR);
		}
	}

	SetCurrentDirectory(originalPath);
}
void CImGui_Tool::CellFile_Load()
{
	// �۾��� �ϰ� ���� �۾��� ��ΰ� ���� ���� �Ǿ� ���� �۾� �� ������ ��.
// �׷��� ��θ� �����س��� ��� �۾��� ������ ��θ� ���� ������ ��η� �ٽ� �ʱ�ȭ�Ѵ�.
	TCHAR originalPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, originalPath);

	OPENFILENAME OFN;
	TCHAR filePathName[100] = L"";
	TCHAR lpstrFile[100] = L"";
	static TCHAR filter[] = L"��� ����\0*.*\0�ؽ�Ʈ ����\0*.txt\0fbx ����\0*.fbx";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 100;
	OFN.lpstrInitialDir = L"../Bin/SaveLoad/";
	// L"D:\\SkyrimProject\\Client\\Bin\\SaveLoad\\"

	if (GetOpenFileName(&OFN) != 0) {
		wsprintf(filePathName, L"%s ������ ���ڽ��ϱ�?", OFN.lpstrFile);
		MessageBox(g_hWnd, filePathName, L"���� ����", MB_OK);

		wstring filePath = OFN.lpstrFile;

		// ������ ���� ���� ����.
		ifstream fileStream(filePath, ios::binary);
		if (fileStream.is_open()) {
			// ���� ������ �б�.

			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			pGameInstance->Cell_FileLoad(fileStream, m_pNavigationCom);

			Safe_Release(pGameInstance);

			fileStream.close();
			MessageBox(g_hWnd, L"������ ���������� �ҷ��Խ��ϴ�.", L"�ҷ����� �Ϸ�", MB_OK);
		}
		else {
			MessageBox(g_hWnd, L"������ �ҷ����� �� ������ �߻��߽��ϴ�.", L"�ҷ����� ����", MB_OK | MB_ICONERROR);
		}
	}

	SetCurrentDirectory(originalPath);
}

void CImGui_Tool::LightFile_Save()
{
	TCHAR originalPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, originalPath);

	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	TCHAR lpstrFile[MAX_PATH] = L"";
	static TCHAR filter[] = L"�ؽ�Ʈ ���� (*.txt)\0*.txt\0��� ���� (*.*)\0*.*\0";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = MAX_PATH;
	OFN.lpstrInitialDir = L"../Bin/SaveLoad/";;
	OFN.Flags = OFN_OVERWRITEPROMPT; // ����� ���θ� ���� ��ȭ ���ڸ� ǥ��

	if (GetSaveFileName(&OFN) != 0) {
		wsprintf(filePathName, L"%s ������ �����ϰڽ��ϱ�?", OFN.lpstrFile);
		MessageBox(g_hWnd, filePathName, L"���� ����", MB_OK);

		wstring filePath = OFN.lpstrFile;

		ofstream fileStream(filePath, ios::binary);
		if (fileStream.is_open()) {
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			pGameInstance->Light_FileSave(fileStream, LEVEL_TOOL);

			Safe_Release(pGameInstance);

			fileStream.close();
			MessageBox(g_hWnd, L"������ ���������� ����Ǿ����ϴ�.", L"���� �Ϸ�", MB_OK);
		}
		else {
			MessageBox(g_hWnd, L"������ �����ϴ� �� ������ �߻��߽��ϴ�.", L"���� ����", MB_OK | MB_ICONERROR);
		}
	}

	SetCurrentDirectory(originalPath);
}
void CImGui_Tool::LightFile_Load()
{
	TCHAR originalPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, originalPath);

	OPENFILENAME OFN;
	TCHAR filePathName[100] = L"";
	TCHAR lpstrFile[100] = L"";
	static TCHAR filter[] = L"��� ����\0*.*\0�ؽ�Ʈ ����\0*.txt\0fbx ����\0*.fbx";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 100;
	OFN.lpstrInitialDir = L"../Bin/SaveLoad/";

	if (GetOpenFileName(&OFN) != 0) {
		wsprintf(filePathName, L"%s ������ ���ڽ��ϱ�?", OFN.lpstrFile);
		MessageBox(g_hWnd, filePathName, L"���� ����", MB_OK);

		wstring filePath = OFN.lpstrFile;

		// ������ ���� ���� ����.
		ifstream fileStream(filePath, ios::binary);
		if (fileStream.is_open()) {
			// ���� ������ �б�.

			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			pGameInstance->Light_FileLoad(fileStream, LEVEL_TOOL);

			Safe_Release(pGameInstance);

			fileStream.close();
			MessageBox(g_hWnd, L"������ ���������� �ҷ��Խ��ϴ�.", L"�ҷ����� �Ϸ�", MB_OK);
		}
		else {
			MessageBox(g_hWnd, L"������ �ҷ����� �� ������ �߻��߽��ϴ�.", L"�ҷ����� ����", MB_OK | MB_ICONERROR);
		}
	}

	SetCurrentDirectory(originalPath);

}

void CImGui_Tool::Add_LayOut_Array(const char* _strName, ImVec2 _LayOutPos, ImVec2 _LayOutSize)
{
	_bool m_bDupliName = false;

	TOOL_LAYOUTDESC tempLayOutDesc;
	tempLayOutDesc.m_strName = _strName;
	tempLayOutDesc.m_vStartLayOutPos = _LayOutPos;
	tempLayOutDesc.m_vEndLayOutPos = { tempLayOutDesc.m_vStartLayOutPos.x + _LayOutSize.x ,
									   tempLayOutDesc.m_vStartLayOutPos.y + _LayOutSize.y };

	for (size_t i = 0; i < m_vecLayOut.size(); ++i)
	{
		if (m_vecLayOut[i].m_strName == tempLayOutDesc.m_strName)
		{
			m_bDupliName = true;
			return;
		}
	}

	if (!m_bDupliName) m_vecLayOut.push_back(tempLayOutDesc);
}
_bool CImGui_Tool::Check_LayOut_Range(POINT _MousePos)
{
	for (size_t i = 0; i < m_vecLayOut.size(); ++i)
	{
		if(m_vecLayOut[i].m_vStartLayOutPos.x <= _MousePos.x && m_vecLayOut[i].m_vEndLayOutPos.x >= _MousePos.x &&
		   m_vecLayOut[i].m_vStartLayOutPos.y <= _MousePos.y && m_vecLayOut[i].m_vEndLayOutPos.y >= _MousePos.y)
		{
			return false;
		}
	}

	return true;
}

HRESULT CImGui_Tool::Create_Object()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// ��� ����
	_matrix matInitialize = XMMatrixIdentity();
	matInitialize = XMMatrixTranslation(ResultPickPos.x, ResultPickPos.y, ResultPickPos.z);

	// clone object
	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_TOOL, m_strCurLayerTag,
		m_pCurFile.m_strObjTag, m_pCurFile.m_strComTag, &matInitialize)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}
HRESULT CImGui_Tool::Delete_Object()
{
	if (m_pSelectObject != nullptr && m_bObjDeleteMode)
	{
		Safe_Release(m_pSelectObject);
		m_pSelectObject = nullptr;
	}

	return S_OK;
}
HRESULT CImGui_Tool::Select_Object()
{
	char buffer[256];
	_bool bIsChangeValue = false;

	CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectObject->Get_Component(TEXT("Com_Transform")));
	_float4 objPos;
	_float3 objScale = pTransform->Get_Scaled();
	_float3 vRotation = pTransform->Get_Rotated();

	XMStoreFloat4(&objPos, pTransform->Get_State(CTransform::STATE_POSITION));

	Key_Input(pTransform);

	string strObjName = "";
	strObjName.assign(m_pSelectObject->Get_Name().begin(), m_pSelectObject->Get_Name().end());

	ImGui::Text(strObjName.c_str());

	ImGui::Text("PosX   ");
	ImGui::SameLine();
	if (ImGui::DragFloat("##PosX", &objPos.x, 0.1f))
		pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&objPos));

	ImGui::Text("PosY   ");
	ImGui::SameLine();
	if (ImGui::DragFloat("##PosY", &objPos.y, 0.1f))
		pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&objPos));

	ImGui::Text("PosZ   ");
	ImGui::SameLine();
	if (ImGui::DragFloat("##PosZ", &objPos.z, 0.1f))
		pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&objPos));

	ImGui::Text("ScaleX ");
	ImGui::SameLine();
	if (ImGui::DragFloat("##ScaleX", &objScale.x, 0.1f, 0.1f, 100.f))
		pTransform->Set_Scaling(objScale);

	ImGui::Text("ScaleY ");
	ImGui::SameLine();
	if (ImGui::DragFloat("##ScaleY", &objScale.y, 0.1f, 0.1f, 100.f))
		pTransform->Set_Scaling(objScale);

	ImGui::Text("ScaleZ ");
	ImGui::SameLine();
	if (ImGui::DragFloat("##ScaleZ", &objScale.z, 0.1f, 0.1f, 100.f))
		pTransform->Set_Scaling(objScale);

	_float  fRotSpeed = 0.f;

	// ImGui�� ��ü������ ���콺�� �Է� ���¸� Ȯ���ϴ� �Լ��� �ֳ�?
	if (ImGui::IsMouseDragging(0, 0.0f)) // ���� ��ư�� ���ȴ��� Ȯ���Ѵܴ�.
	{
		ImVec2 dragDelta = ImGui::GetMouseDragDelta(0); // �巡�� ������� �Ѵ�.
		if (dragDelta.x > 0.0f) { // ������ �巡��
			fRotSpeed += 1.f;
		}
		else if (dragDelta.x < 0.0f) { // ���� �巡��
			fRotSpeed -= 1.f;
		}
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	ImGui::Text("RotX   ");
	ImGui::SameLine();
	if (ImGui::DragFloat("##RotX", &vRotation.x, 0.01f, 100.f))
	{
		pTransform->Turn(pTransform->Get_State(CTransform::STATE_UP), pGameInstance->Compute_TimeDelta(TEXT("Timer_60")), fRotSpeed);
	}

	ImGui::Text("RotY   ");
	ImGui::SameLine();
	if (ImGui::DragFloat("##RotY", &vRotation.y, 0.01f, 100.f))
	{
		pTransform->Turn(pTransform->Get_State(CTransform::STATE_RIGHT), pGameInstance->Compute_TimeDelta(TEXT("Timer_60")), fRotSpeed);
	}

	ImGui::Text("RotZ   ");
	ImGui::SameLine();
	if (ImGui::DragFloat("##RotZ", &vRotation.z, 0.01f, 100.f))
	{
		pTransform->Turn(pTransform->Get_State(CTransform::STATE_LOOK), pGameInstance->Compute_TimeDelta(TEXT("Timer_60")), fRotSpeed -15.f);
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CImGui_Tool::Create_Light()
{
	if (m_bLightCreateMode)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		// ��� ����
		_matrix matInitialize = XMMatrixIdentity();
		matInitialize = XMMatrixTranslation(ResultPickPos.x, ResultPickPos.y, ResultPickPos.z);

		//wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		//wstring changTypeName = converter.from_bytes(selectedLightName);

		// clone light
		if (FAILED(pGameInstance->Add_CloneObject(LEVEL_TOOL, TEXT("Layer_LightObject"),
			TEXT("ProtoType_GameObject_Light_Point"), &matInitialize)))
			return E_FAIL;

		Safe_Release(pGameInstance);
	}

	return S_OK;
}

void CImGui_Tool::Select_Light()
{
	char buffer[256];
	_bool bIsChangeValue = false;

	CTransform* pTransform = dynamic_cast<CTransform*>(m_pSelectLight->Get_Component(TEXT("Com_Transform")));
	CLight_Point* pLight = dynamic_cast<CLight_Point*>(m_pSelectLight);
	LIGHT_DESC* pLightDesc = pLight->Get_LightDesc();
	_float4 vLightPos;

	XMStoreFloat4(&vLightPos, pTransform->Get_State(CTransform::STATE_POSITION));

	string strLightName = "";
	strLightName.assign(m_pSelectLight->Get_Name().begin(), m_pSelectLight->Get_Name().end());

	ImGui::Text(strLightName.c_str());

	ImGui::Text("Position   ");
	ImGui::SameLine();
	if (ImGui::DragFloat3("##PosX", &pLightDesc->vLightPos.x, 0.1f))
	{
		pLight->Set_LightDesc(*pLightDesc);
		pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pLightDesc->vLightPos));
	}

	ImGui::Text("Diffuse   ");
	ImGui::SameLine();
	if (ImGui::ColorEdit3("##DiffuseX", &pLightDesc->vDiffuse.x, ImGuiColorEditFlags_Float))
		pLight->Set_LightDesc(*pLightDesc);

	ImGui::Text("Ambient  ");
	ImGui::SameLine();
	if (ImGui::ColorEdit3("##AmbientX", &pLightDesc->vAmbient.x, ImGuiColorEditFlags_Float))
		pLight->Set_LightDesc(*pLightDesc);

	ImGui::Text("Specular  ");
	ImGui::SameLine();
	if (ImGui::ColorEdit3("##SpecularX", &pLightDesc->vSpecular.x, ImGuiColorEditFlags_Float))
		pLight->Set_LightDesc(*pLightDesc);

	ImGui::Text("Range  ");
	ImGui::SameLine();
	if (ImGui::DragFloat("##Range", &pLightDesc->fLightRange, 0.1f, 0.1f, 100.f))
		pLight->Set_LightDesc(*pLightDesc);

}

void CImGui_Tool::Key_Input(class CTransform* _pTransform)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	_float4 objPos;
	_float3 objScale = _pTransform->Get_Scaled();

	XMStoreFloat4(&objPos, _pTransform->Get_State(CTransform::STATE_POSITION));

	/* x */
	if (!pGameInstance->Get_DIKeyPress(VK_LCONTROL) && pGameInstance->Get_DIKeyDown('X'))
	{
		objPos.x += 1.f;
		_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&objPos));
	}
	else if (pGameInstance->Get_DIKeyPress(VK_LCONTROL) && pGameInstance->Get_DIKeyDown('X'))
	{
		objPos.x -= 1.f;
		_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&objPos));
	}

	/* y */
	if (!pGameInstance->Get_DIKeyPress(VK_LCONTROL) && pGameInstance->Get_DIKeyDown('Y'))
	{
		objPos.y += 1.f;
		_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&objPos));
	}
	else if (pGameInstance->Get_DIKeyPress(VK_LCONTROL) && pGameInstance->Get_DIKeyDown('Y'))
	{
		objPos.y -= 1.f;
		_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&objPos));
	}

	/* z */
	if (!pGameInstance->Get_DIKeyPress(VK_LCONTROL) && pGameInstance->Get_DIKeyDown('Z'))
	{
		objPos.z += 1.f;
		_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&objPos));
	}
	else if (pGameInstance->Get_DIKeyPress(VK_LCONTROL) && pGameInstance->Get_DIKeyDown('Z'))
	{
		objPos.z -= 1.f;
		_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&objPos));
	}

	_pTransform->Get_State(CTransform::STATE_UP);

	/* x ȸ�� */
	if (!pGameInstance->Get_DIKeyPress(VK_LCONTROL) && pGameInstance->Get_DIKeyDown('R'))
	{
		m_fRotValue += 11.25f;
		_pTransform->Fix_Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fRotValue));
	}

	else if (pGameInstance->Get_DIKeyPress(VK_LCONTROL) && pGameInstance->Get_DIKeyDown('R'))
	{
		m_fRotValue -= 11.25f;
		_pTransform->Fix_Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fRotValue));
	}

	else if (pGameInstance->Get_DIKeyPress(VK_LCONTROL) && pGameInstance->Get_DIKeyDown('Q'))
	{
		m_fRotValue -= 90.f;
		_pTransform->Fix_Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(m_fRotValue));
	}


	Safe_Release(pGameInstance);
}

CImGui_Tool* CImGui_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CImGui_Tool* pInstance = new CImGui_Tool();

	if (FAILED(pInstance->Initialize(_pDevice, _pContext)))
	{
		MSG_BOX("Fail Create : CImGui_Tool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImGui_Tool::Free()
{
	__super::Free();

	// ����ߴ� vector ��� ����.(strdup�� ����Ͽ��⿡ �޸� ������ �ʼ�).
	for (size_t i = 0; i < m_vecAfterFolderDesc.size(); ++i)
	{
		free((void*)m_vecAfterFolderDesc[i].strFolderName);
	}
	m_vecAfterFolderDesc.clear();
	m_vecBeforeFileDesc.clear();

	for (size_t i = 0; i < m_vecAfterFileDesc.size(); ++i)
	{
		free((void*)m_vecAfterFileDesc[i].m_strFileName);
	}
	m_vecAfterFileDesc.clear();
	m_vecBeforeFileList.clear();
	// 

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pNavigationCom);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}