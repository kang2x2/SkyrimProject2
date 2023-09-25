#include "framework.h"
#include <filesystem>
#include <commdlg.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "ImGui_Tool.h"

#include "GameInstance.h"

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

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}

void CImGui_Tool::LayOut_Mouse()
{
	POINT MousePos = {};
	GetCursorPos(&MousePos);
	ScreenToClient(g_hWnd, &MousePos);

	ImGui::Begin("Mouse Pos");

	ImGui::Text("Cur Pos : X(%d) Y(%d)", MousePos.x, MousePos.y);
	
	if (GetKeyState(MK_LBUTTON) & 0x8000)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		ResultPickPos = pGameInstance->Return_WorldMousePos(m_pDevice, m_pContext, MousePos);

		Safe_Release(pGameInstance);
	}

	ImGui::Text("Pick Pos : X(%f) Y(%f) Z(%f)", ResultPickPos.x, ResultPickPos.y, ResultPickPos.z);

	ImGui::End();
}

void CImGui_Tool::LayOut_SaveLoad()
{
	ImGui::Begin("Save/Load");

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

void CImGui_Tool::LayOut_Object()
{
	ImGui::Begin("FBX Object LayOut");

	if (ImGui::BeginTabBar("Tabs")) // 탭 그룹 시작
	{
		if (ImGui::TabItemButton("FBX"))
		{
			m_bFindFolder = true;
			m_bFindFBX	  = true;

			showFBXContent = true;
			showDDSContent = false;
			showWICContent = false;
		}
		if (ImGui::TabItemButton("DDS"))
		{
			m_bFindFolder = true;

			showFBXContent = false;
			showDDSContent = true;
			showWICContent = false;
		}
		if (ImGui::TabItemButton("WIC"))
		{
			m_bFindFolder = true;

			showFBXContent = false;
			showDDSContent = false;
			showWICContent = true;
		}

		ImGui::EndTabBar(); // 탭 그룹 종료

		LayOut_Object_FBX();
		LayOut_Object_DDS();
		LayOut_Object_WIC();
	}

	ImGui::End();
}

// 선택된 폴더명 저장하기 위한 전역변수
const char* selectedFolderName;
void CImGui_Tool::LayOut_Object_FBX()
{
	if (showFBXContent)
	{
		ImGui::Text("FBX");

		ImGui::Image(nullptr, ImVec2(150, 150));

		// 한 번만 폴더 검색
		if (m_bFindFolder)
		{
			// Folder vector 모두 정리.(strdup를 사용하였기에 메모리 해제도 필수).
			Empty_Content();

			FindFolder();
			// 검색된 폴더 이름 const char*로 변환.
			ChangeType_Folder();

			selectedFolderName = m_vecChangeFolderList[0];
		}
		
		
		// 폴더명 
		if (ImGui::BeginCombo(" ", selectedFolderName))
		{
			for (size_t i = 0; i < m_vecChangeFolderList.size(); ++i)
			{
				_bool isSelected = m_vecChangeFolderList[i] == selectedFolderName;
				if (ImGui::Selectable(m_vecChangeFolderList[i], isSelected))
				{
					selectedFolderName = m_vecChangeFolderList[i];

					// File vector 모두 정리.(strdup를 사용하였기에 메모리 해제도 필수).
					for (size_t i = 0; i < m_vecChangeFileList.size(); ++i)
					{
						free((void*)m_vecChangeFileList[i].m_strFileName);
					}
					m_vecChangeFileList.clear();
					m_vecFileList.clear();
					FindFile_FBX(m_vecChangeFolderList[i]);
					ChangeType_File();
				}
			}
			ImGui::EndCombo();
		}

		// 파일 리스트
		if (ImGui::BeginListBox("##listBox", ImVec2(215, 300)))
		{
			for (size_t i = 0; i < m_vecChangeFileList.size(); ++i)
			{
				// 현재 항목이 선택되었는지 확인
				bool isSelected = m_vecChangeFileList[i].m_bCheck;

				if (ImGui::Selectable(m_vecChangeFileList[i].m_strFileName, isSelected))
				{
					// 항목을 클릭했을 때 다른 아이템들의 선택 상태를 false로 변경.
					for (size_t i = 0; i < m_vecChangeFileList.size(); ++i)
					{
						if (m_vecChangeFileList[i].m_bCheck)
						{
							m_vecChangeFileList[i].m_bCheck = false;
						}
					}

					// 현재 선택한 항목만 true로 설정.
					m_vecChangeFileList[i].m_bCheck = true;
				}
			}
			ImGui::EndListBox();
		}
	}
}
void CImGui_Tool::LayOut_Object_DDS()
{
	if (showDDSContent)
	{
		ImGui::Text("DDS");
		if (m_bFindFolder)
			Empty_Content();
	}
}
void CImGui_Tool::LayOut_Object_WIC()
{
	if (showWICContent)
	{
		ImGui::Text("WIC");

		if (m_bFindFolder)
			Empty_Content();
	}
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
				m_vecFolderList.push_back(findFileData.cFileName);
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
			findFileName.Format(_T("%s"), wfd.cFileName);
			// findFileName에 파일 이름 저장

			// 여기에서 findFileName을 사용하여 원하는 작업 수행
			LPCTSTR lpcStr = (LPCTSTR)findFileName;

			wstring wstr(lpcStr);

			m_vecFileList.push_back(lpcStr);

		} while (FindNextFile(fileSearch, &wfd));

		// 파일 찾기 핸들 값 닫기	
		FindClose(fileSearch);
	}
}

void CImGui_Tool::Empty_Content()
{
	// Folder vector 모두 정리.(strdup를 사용하였기에 메모리 해제도 필수).
	for (size_t i = 0; i < m_vecChangeFolderList.size(); ++i)
	{
		free((void*)m_vecChangeFolderList[i]);
	}
	m_vecChangeFolderList.clear();
	m_vecFolderList.clear();

	// File vector 모두 정리.(strdup를 사용하였기에 메모리 해제도 필수).
	for (size_t i = 0; i < m_vecChangeFileList.size(); ++i)
	{
		free((void*)m_vecChangeFileList[i].m_strFileName);
	}
	m_vecChangeFileList.clear();
	m_vecFileList.clear();

	m_bFindFolder = false;
}

void CImGui_Tool::ChangeType_Folder()
{
	for (size_t i = 0; i < m_vecFolderList.size(); ++i)
	{
		wstring_convert<codecvt_utf8<wchar_t>> converter;
		string tempStr = converter.to_bytes(m_vecFolderList[i]);
		m_vecChangeFolderList.push_back(strdup(tempStr.c_str())); // strdup을 사용하여 복사
		// strdup를 사용하여 복사하면 메모리도 같이 할당 됨. 나중에 꼭 해제해줘야 함.
	}

	m_vecFolderList.clear();

	m_bFindFolder = false;
}
void CImGui_Tool::ChangeType_File()
{
	for (size_t i = 0; i < m_vecFileList.size(); ++i)
	{
		wstring_convert<codecvt_utf8<wchar_t>> converter;
		string tempStr = converter.to_bytes(m_vecFileList[i]);

		TOOL_FILEDESC tempFile;
		tempFile.m_bCheck = false;
		tempFile.m_strFileName = strdup(tempStr.c_str());
		m_vecChangeFileList.push_back(tempFile); // strdup을 사용하여 복사
		// strdup를 사용하여 복사하면 메모리도 같이 할당 됨. 나중에 꼭 해제해줘야 함.
	}

	m_vecFileList.clear();

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
		std::string filePathUTF8(filePath.begin(), filePath.end()); // UTF-8로 변환
		int a = 0;
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

		int requiredSize = WideCharToMultiByte(CP_UTF8, 0, OFN.lpstrFile, -1, NULL, 0, NULL, NULL);
		if (requiredSize > 0) {
			std::string t;
			t.resize(requiredSize);
			WideCharToMultiByte(CP_UTF8, 0, OFN.lpstrFile, -1, &t[0], requiredSize, NULL, NULL);

			int a = 0;
		}
	}

	SetCurrentDirectory(originalPath);
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
	for (size_t i = 0; i < m_vecChangeFolderList.size(); ++i)
	{
		free((void*)m_vecChangeFolderList[i]);
	}
	m_vecChangeFolderList.clear();
	m_vecFolderList.clear();

	for (size_t i = 0; i < m_vecChangeFileList.size(); ++i)
	{
		free((void*)m_vecChangeFileList[i].m_strFileName);
	}
	m_vecChangeFileList.clear();
	m_vecFileList.clear();
	// 

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}
