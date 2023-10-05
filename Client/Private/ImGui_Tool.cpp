#include "framework.h"
#include <iostream>
#include <filesystem>
#include <commdlg.h>
#include <fstream>

#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "ImGui_Tool.h"

#include "GameInstance.h"
#include "Model.h"

#include "VIBuffer_Grid.h"
#include "Terrain_Grid.h"

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
		LayOut_Object();
		LayOut_SaveLoad();


		if (m_bDelete)
		{
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			pGameInstance->Delete_CloneObject(LEVEL_TOOL, m_pSelectObject->Get_HasLayerTag(), m_pSelectObject->Get_Name());
			m_pSelectObject = nullptr;

			Safe_Release(pGameInstance);

			m_bDelete = false;
		}

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

	// 범위 밖에서만 수행하기 위한 레이아웃 범위 저장.
	Add_LayOut_Array(strLayOutName, ImGui::GetWindowPos(), ImGui::GetWindowSize());

	ImGui::Text("Cur Pos : X(%d) Y(%d)", MousePos.x, MousePos.y);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 레이아웃 범위 밖에서만 수행.
	if (pGameInstance->Get_DIMouseDown(CInput_Device::MKS_LBUTTON) && Check_LayOut_Range(MousePos))
	{
		CTerrain_Grid* pTerrainGrid = dynamic_cast<CTerrain_Grid*>(pGameInstance->Find_CloneObject(LEVEL_TOOL, TEXT("Layer_Terrain"), TEXT("Tool_GridTerrain")));

		CVIBuffer_Grid* pGridBuffer = dynamic_cast<CVIBuffer_Grid*>(pGameInstance->Find_ProtoType(LEVEL_TOOL, TEXT("ProtoType_Component_VIBuffer_Terrain_Grid")));
		const _float3* pTerrainVtxPos = pGridBuffer->Get_VtxPos();

		ResultPickPos = pGameInstance->Picking_Grid(m_pDevice, m_pContext, MousePos, pTerrainGrid, pTerrainVtxPos);

		if (m_bCreateMode)
			Create_Object();
		if (m_bDeleteMode)
		{
			if (!m_bDelete)
			{
				m_pSelectObject = pGameInstance->Picking_Object(m_pDevice, m_pContext, MousePos, LEVEL_TOOL);

				if (m_pSelectObject != nullptr)
				{
					m_pSelectObject->Set_IsDead(true);
					m_bDelete = true;
				}
			}

		}
		if (m_bSelectMode)
		{
			m_pSelectObject = pGameInstance->Picking_Object(m_pDevice, m_pContext, MousePos, LEVEL_TOOL);
		}

	}

	if (pGameInstance->Get_DIMouseDown(CInput_Device::MKS_RBUTTON))
	{
		if (m_bCreateMode)
			m_bCreateMode = false;

		if (m_bDeleteMode)
			m_bDeleteMode = false;

		if (m_bSelectMode)
			m_bSelectMode = false;

		if (m_pSelectObject != nullptr)
			m_pSelectObject = nullptr;
	}

	ImGui::Text("Pick Pos : X(%f) Y(%f) Z(%f)", ResultPickPos.x, ResultPickPos.y, ResultPickPos.z);

	Safe_Release(pGameInstance);

	ImGui::End();

	return S_OK;
}
void CImGui_Tool::LayOut_SaveLoad()
{
	const char* strLayOutName = "Save/Load";

	ImGui::Begin(strLayOutName);

	// 범위 밖에서만 수행하기 위한 레이아웃 범위 저장.
	Add_LayOut_Array(strLayOutName, ImGui::GetWindowPos(), ImGui::GetWindowSize());

	if (ImGui::Button("Save"))
	{
		File_Save();
	}

	ImGui::SameLine();

	if (ImGui::Button("Load"))
	{
		File_Load();
	}

	ImGui::End();
}
void CImGui_Tool::LayOut_Object_PickMode()
{
	const char* strLayOutName = "Pick Mode";

	ImGui::Begin("Pick Mode");

	// 범위 밖에서만 수행하기 위한 레이아웃 범위 저장.
	Add_LayOut_Array(strLayOutName, ImGui::GetWindowPos(), ImGui::GetWindowSize());

	if (ImGui::Button("Create"))
	{
		if (m_pCurFile != nullptr)
		{
			m_bCreateMode = true;
			m_bDeleteMode = false;
			m_bSelectMode = false;
			m_pSelectObject = nullptr;
		}
	}
	if (ImGui::Button("Delete"))
	{
		m_bCreateMode = false;
		m_bDeleteMode = true;
		m_bSelectMode = false;
		m_pSelectObject = nullptr;
	}
	if (ImGui::Button("Select"))
	{
		m_bCreateMode = false;
		m_bDeleteMode = false;
		m_bSelectMode = true;
	}

	if (m_bCreateMode)
		ImGui::Text("Cur Mode : Create");
	else if (m_bDeleteMode)
		ImGui::Text("Cur Mode : Delete");
	else if (m_bSelectMode)
		ImGui::Text("Cur Mode : Select");
	else
		ImGui::Text("Cur Mode : None");

	ImGui::End();
}

void CImGui_Tool::LayOut_Object()
{
	const char* strLayOutName = "FBX Object LayOut";

	ImGui::Begin("FBX Object LayOut");

	// 범위 밖에서만 수행하기 위한 레이아웃 범위 저장.
	Add_LayOut_Array(strLayOutName, ImGui::GetWindowPos(), ImGui::GetWindowSize());

	if (ImGui::BeginTabBar("Tabs")) // 탭 그룹 시작
	{
		if (ImGui::TabItemButton("FBX"))
		{
			m_bFindFolder = true;
			m_bFindFBX = true;

			showFBXListShow = true;
		}

		ImGui::EndTabBar(); // 탭 그룹 종료

		LayOut_FBX_List();
		LayOut_Object_Transform();
	}

	ImGui::End();
}

// 선택된 폴더명 저장하기 위한 전역변수
const char* selectedFolderName;
void CImGui_Tool::LayOut_FBX_List()
{
	if (showFBXListShow)
	{
		ImGui::Text("File List");
		ImGui::Image(nullptr, ImVec2(150, 150));

		// 한 번만 폴더 검색
		if (m_bFindFolder)
		{
			// Folder vector 모두 정리.(strdup를 사용하였기에 메모리 해제도 필수).
			Empty_Content();

			FindFolder();
			// 검색된 폴더 이름 const char*로 변환.
			ChangeType_Folder();

			selectedFolderName = m_vecAfterFolderList[0];
		}


		// 폴더명 
		if (ImGui::BeginCombo(" ", selectedFolderName))
		{
			for (size_t i = 0; i < m_vecAfterFolderList.size(); ++i)
			{
				_bool isSelected = m_vecAfterFolderList[i] == selectedFolderName;
				if (ImGui::Selectable(m_vecAfterFolderList[i], isSelected))
				{
					selectedFolderName = m_vecAfterFolderList[i];

					// File vector 모두 정리.(strdup를 사용하였기에 메모리 해제도 필수).
					for (size_t i = 0; i < m_vecAfterFileList.size(); ++i)
					{
						free((void*)m_vecAfterFileList[i].m_strFileName);
						free((void*)m_vecAfterFileList[i].m_strFBXPath);
					}
					m_vecFilePathList.clear();
					m_vecAfterFileList.clear();
					m_vecBeforeFileList.clear();
					FindFile_FBX(m_vecAfterFolderList[i]);
					ChangeType_File();
				}
			}
			ImGui::EndCombo();
		}

		// 파일 리스트
		if (ImGui::BeginListBox("##listBox", ImVec2(215, 300)))
		{
			for (size_t i = 0; i < m_vecAfterFileList.size(); ++i)
			{
				// 현재 항목이 선택되었는지 확인
				bool isSelected = m_vecAfterFileList[i].m_bCheck;

				if (ImGui::Selectable(m_vecAfterFileList[i].m_strFileName, isSelected))
				{
					// 항목을 클릭했을 때 다른 아이템들의 선택 상태를 false로 변경.
					for (size_t i = 0; i < m_vecAfterFileList.size(); ++i)
					{
						if (m_vecAfterFileList[i].m_bCheck)
						{
							m_vecAfterFileList[i].m_bCheck = false;
						}
					}

					// 현재 선택한 항목만 true로 설정.
					m_vecAfterFileList[i].m_bCheck = true;
					// 현재 선택된 파일 변수에 저장.
					m_pCurFile = &m_vecAfterFileList[i];
				}
			}
			ImGui::EndListBox();
		}
	}

	LayOut_Object_PickMode();
}

void CImGui_Tool::LayOut_Object_Transform()
{
	const char* strLayOutName = "Object Transform";

	ImGui::Begin("Object Transform");

	// 범위 밖에서만 수행하기 위한 레이아웃 범위 저장.
	Add_LayOut_Array(strLayOutName, ImGui::GetWindowPos(), ImGui::GetWindowSize());

	if (m_pSelectObject != nullptr)
	{
		Select_Object();
	}

	ImGui::End();
}

void CImGui_Tool::FindFolder()
{
	// 폴더 검색하여 findFileData에 저장.
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile(_T("../Bin/Resource/Models/Skyrim/*"), &findFileData);

	// 예외 처리
	if (hFind == INVALID_HANDLE_VALUE) {
		MSG_BOX("Fail Find : Skyrim Folder");
		return;
	}
	do {
		// 찾은게 폴더인지 검사.
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (_tcscmp(findFileData.cFileName, _T(".")) != 0 && _tcscmp(findFileData.cFileName, _T("..")) != 0) {
				// 찾은 폴더 이름 추가.
				m_vecBeforeFolderList.push_back(findFileData.cFileName);
			}
		}
	} while (FindNextFile(hFind, &findFileData) != 0);

	FindClose(hFind);
}
void CImGui_Tool::FindFile_FBX(const char* _FolderName)
{
	HANDLE fileSearch;
	WIN32_FIND_DATA wfd;
	CString musiccount;
	CString findFileName;
	CString FolderName = _FolderName;

	musiccount.Format(_T("../Bin/Resource/Models/Skyrim/") + FolderName + _T("/*.fbx"));

	// FindFirstFile 함수를 통해 검색하려고 하는 파일이 없을 경우 
	// 핸들 값은 INVALID_HANDLE_VALUE 값을 가짐
	fileSearch = FindFirstFile(musiccount, &wfd);

	// 찾는 파일이 있다면, 
	if (fileSearch != INVALID_HANDLE_VALUE)
	{
		do
		{
			// 경로 저장 위함.
			CString FolderPath = "../Bin/Resource/Models/Skyrim/"; +FolderName;

			findFileName.Format(_T("%s"), wfd.cFileName); // findFileName에 파일 이름 저장
			
			LPCTSTR lpcStr = (LPCTSTR)findFileName;
			wstring wstrName(lpcStr);

			m_vecBeforeFileList.push_back(lpcStr);

			// 파일 경로 저장
			FolderPath += "/";
			FolderPath += findFileName;

			lpcStr = (LPCTSTR)FolderPath;
			wstring wstrPath(lpcStr);

			m_vecFilePathList.push_back(lpcStr);

		} while (FindNextFile(fileSearch, &wfd));

		// 파일 찾기 핸들 값 닫기	
		FindClose(fileSearch);
	}
}

void CImGui_Tool::Empty_Content()
{
	// Folder vector 모두 정리.(strdup를 사용하였기에 메모리 해제도 필수).
	for (size_t i = 0; i < m_vecAfterFolderList.size(); ++i)
	{
		free((void*)m_vecAfterFolderList[i]);
	}
	m_vecAfterFolderList.clear();
	m_vecBeforeFolderList.clear();

	// File vector 모두 정리.(strdup를 사용하였기에 메모리 해제도 필수).
	for (size_t i = 0; i < m_vecAfterFileList.size(); ++i)
	{
		free((void*)m_vecAfterFileList[i].m_strFileName);
		free((void*)m_vecAfterFileList[i].m_strFBXPath);
	}
	m_vecFilePathList.clear();
	m_vecAfterFileList.clear();
	m_vecBeforeFileList.clear();

	m_bFindFolder = false;
}

void CImGui_Tool::ChangeType_Folder()
{
	for (size_t i = 0; i < m_vecBeforeFolderList.size(); ++i)
	{
		wstring_convert<codecvt_utf8<wchar_t>> converter;
		string tempStr = converter.to_bytes(m_vecBeforeFolderList[i]);
		m_vecAfterFolderList.push_back(strdup(tempStr.c_str())); // strdup을 사용하여 복사
		// strdup를 사용하여 복사하면 메모리도 같이 할당 됨. 나중에 꼭 해제해줘야 함.
	}

	m_vecBeforeFolderList.clear();

	m_bFindFolder = false;
}
void CImGui_Tool::ChangeType_File()
{
	for (size_t i = 0; i < m_vecBeforeFileList.size(); ++i)
	{
		wstring_convert<codecvt_utf8<wchar_t>> converter;
		string tempStr = converter.to_bytes(m_vecBeforeFileList[i]);
		string tempPath = converter.to_bytes(m_vecFilePathList[i]);

		TOOL_AFTER_FILEDESC tempFile;
		tempFile.m_bCheck = false;

		// 오브젝트 태그 문자열 자르기
		tempFile.m_strObjTag = L"ProtoType_GameObject_" + m_vecBeforeFileList[i];
		wstring wstr = tempFile.m_strObjTag;
		size_t lastDotPos = wstr.rfind(L'.'); // . 저장
		if (lastDotPos != wstring::npos) { // .이전까지 자른다.
			wstr = wstr.substr(0, lastDotPos);
		}
		tempFile.m_strObjTag = wstr;

		// 컴포넌트 태그 문자열 자르기
		tempFile.m_strComTag = L"ProtoType_Component_Model_" + m_vecBeforeFileList[i];
		wstr = tempFile.m_strComTag;
		lastDotPos = wstr.rfind(L'.'); // . 저장
		if (lastDotPos != wstring::npos) { // .이전까지 자른다.
			wstr = wstr.substr(0, lastDotPos);
		}
		tempFile.m_strComTag = wstr;

		//
		tempFile.m_strFileName = strdup(tempStr.c_str()); // strdup을 사용하여 복사
		tempFile.m_strFBXPath = strdup(tempPath.c_str()); // strdup을 사용하여 복사
		m_vecAfterFileList.push_back(tempFile); 
		// strdup를 사용하여 복사하면 메모리도 같이 할당 됨. 나중에 꼭 해제해줘야 함.
	}

	m_vecBeforeFileList.clear();

	m_bFindFBX = false;
}

void CImGui_Tool::File_Save()
{
	// 작업을 하고 나면 작업한 경로가 최종 저장 되어 추후 작업 시 문제가 됨.
	// 그러니 경로를 저장해놓고 모든 작업이 끝나면 경로를 지금 저장한 경로로 다시 초기화한다.
	TCHAR originalPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, originalPath);

	OPENFILENAME OFN;
	TCHAR filePathName[MAX_PATH] = L"";
	TCHAR lpstrFile[MAX_PATH] = L"";
	static TCHAR filter[] = L"텍스트 파일 (*.txt)\0*.txt\0모든 파일 (*.*)\0*.*\0";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = MAX_PATH;
	OFN.lpstrInitialDir = L"D:\\SkyrimProject\\Client\\Bin\\SaveLoad\\";
	OFN.Flags = OFN_OVERWRITEPROMPT; // 덮어쓰기 여부를 묻는 대화 상자를 표시

	if (GetSaveFileName(&OFN) != 0) {
		wsprintf(filePathName, L"%s 파일을 저장하겠습니까?", OFN.lpstrFile);
		MessageBox(g_hWnd, filePathName, L"저장 선택", MB_OK);

		std::wstring filePath = OFN.lpstrFile;

		// 파일 내용을 저장할 변수에 원하는 내용을 할당.

		// 파일을 쓰기 모드로 열기.
		std::wofstream fileStream(filePath);
		if (fileStream.is_open()) {
			// 파일 내용을 파일에 쓰기.
			m_pSelectObject->Object_FileSave(fileStream);

			fileStream.close();
			MessageBox(g_hWnd, L"파일이 성공적으로 저장되었습니다.", L"저장 완료", MB_OK);
		}
		else {
			MessageBox(g_hWnd, L"파일을 저장하는 중 오류가 발생했습니다.", L"저장 오류", MB_OK | MB_ICONERROR);
		}
	}

	SetCurrentDirectory(originalPath);
}
void CImGui_Tool::File_Load()
{
	// 작업을 하고 나면 작업한 경로가 최종 저장 되어 추후 작업 시 문제가 됨.
	// 그러니 경로를 저장해놓고 모든 작업이 끝나면 경로를 지금 저장한 경로로 다시 초기화한다.
	TCHAR originalPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, originalPath);

	OPENFILENAME OFN;
	TCHAR filePathName[100] = L"";
	TCHAR lpstrFile[100] = L"";
	static TCHAR filter[] = L"모든 파일\0*.*\0텍스트 파일\0*.txt\0fbx 파일\0*.fbx";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 100;
	OFN.lpstrInitialDir = L"D:\\SkyrimProject\\Client\\Bin\\SaveLoad\\";

	if (GetOpenFileName(&OFN) != 0) {
		wsprintf(filePathName, L"%s 파일을 열겠습니까?", OFN.lpstrFile);
		MessageBox(g_hWnd, filePathName, L"열기 선택", MB_OK);

		wstring filePath = OFN.lpstrFile;

		// 파일을 열기 모드로 열기.
		wifstream fileStream(filePath);
		if (fileStream.is_open()) {
			// 파일 내용을 읽기.



			fileStream.close();
			MessageBox(g_hWnd, L"파일을 성공적으로 불러왔습니다.", L"불러오기 완료", MB_OK);
		}
		else {
			MessageBox(g_hWnd, L"파일을 불러오는 중 오류가 발생했습니다.", L"불러오기 오류", MB_OK | MB_ICONERROR);
		}


		//int requiredSize = WideCharToMultiByte(CP_UTF8, 0, OFN.lpstrFile, -1, NULL, 0, NULL, NULL);
		//if (requiredSize > 0) {
		//	std::string t;
		//	t.resize(requiredSize);
		//	WideCharToMultiByte(CP_UTF8, 0, OFN.lpstrFile, -1, &t[0], requiredSize, NULL, NULL);
		//
		//	int a = 0;
		//}
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

	// 행렬 세팅
	_matrix matInitialize = XMMatrixIdentity();
	matInitialize = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(ResultPickPos.x, ResultPickPos.y, ResultPickPos.z);

	// clone object
	if (FAILED(pGameInstance->Add_CloneObject(LEVEL_TOOL, TEXT("Layer_SkyrimTerrain"),
		m_pCurFile->m_strObjTag, m_pCurFile->m_strComTag, &matInitialize)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}
HRESULT CImGui_Tool::Delete_Object()
{
	if (m_pSelectObject != nullptr && m_bDeleteMode)
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
	_float  fRotSpeed = 20.f;

	XMStoreFloat4(&objPos, pTransform->Get_State(CTransform::STATE_POSITION));

	ImGui::Text("PosX   ");
	ImGui::SameLine();
	if (ImGui::DragFloat("##PosX", &objPos.x, 1.f))
		pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&objPos));

	ImGui::Text("PosY   ");
	ImGui::SameLine();
	if (ImGui::DragFloat("##PosY", &objPos.y, 1.f))
		pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&objPos));

	ImGui::Text("PosZ   ");
	ImGui::SameLine();
	if (ImGui::DragFloat("##PosZ", &objPos.z, 1.f))
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

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	ImGui::Text("RotX   ");
	ImGui::SameLine();
	if (ImGui::DragFloat("##RotX", &fRotSpeed, 0.1f, 100.f))
	{
		pTransform->Turn(pTransform->Get_State(CTransform::STATE_UP), pGameInstance->Compute_TimeDelta(TEXT("Timer_60")), fRotSpeed);
	}

	ImGui::Text("RotY   ");
	ImGui::SameLine();
	if (ImGui::DragFloat("##RotY", &fRotSpeed, 0.1f, 100.f))
	{
		pTransform->Turn(pTransform->Get_State(CTransform::STATE_RIGHT), pGameInstance->Compute_TimeDelta(TEXT("Timer_60")), fRotSpeed);
	}

	Safe_Release(pGameInstance);

	return S_OK;

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

	// 사용했던 vector 모두 정리.(strdup를 사용하였기에 메모리 해제도 필수).
	for (size_t i = 0; i < m_vecAfterFolderList.size(); ++i)
	{
		free((void*)m_vecAfterFolderList[i]);
	}
	m_vecAfterFolderList.clear();
	m_vecBeforeFolderList.clear();

	for (size_t i = 0; i < m_vecAfterFileList.size(); ++i)
	{
		free((void*)m_vecAfterFileList[i].m_strFileName);
		free((void*)m_vecAfterFileList[i].m_strFBXPath);
	}
	m_vecFilePathList.clear();
	m_vecAfterFileList.clear();
	m_vecBeforeFileList.clear();
	// 

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}