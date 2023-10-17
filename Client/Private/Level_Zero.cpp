#include "framework.h"

#include "Level_Zero.h"

#include "GameInstance.h"
#include "Model.h"

CLevel_Zero::CLevel_Zero(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Zero::Initialize()
{
	return S_OK;
}

HRESULT CLevel_Zero::Tick(_float _fTimeDelta)
{
	if (!m_bIsBinarying && GetKeyState('Q') & 0x8000)
	{
		m_bIsBinarying = true;
		SetWindowText(g_hWnd, TEXT("Bynarying..."));

		FindFBX(TEXT("../Bin/Resource/Models/Skyrim/"));

		SetWindowText(g_hWnd, TEXT("Bynary Complete!"));
	}

	return S_OK;
}

HRESULT CLevel_Zero::LateTick(_float _fTimeDelta)
{
	if(!m_bIsBinarying)
		SetWindowText(g_hWnd, TEXT("Current Level : Zero."));

	return S_OK;
}

HRESULT CLevel_Zero::FindFBX(const wstring& _wStrFolderPath)
{
	if (visitedDirectories.count(_wStrFolderPath) > 0)
		return S_OK;

	visitedDirectories.insert(_wStrFolderPath);

	for (const auto& entry : filesystem::directory_iterator(_wStrFolderPath))
	{
		if (filesystem::is_directory(entry))
		{
			FindFBX(entry.path());
		}
		else if (filesystem::is_regular_file(entry))
		{
			if (fileExtension == entry.path().extension() || fileExtension2 == entry.path().extension())
			{
				if (FAILED(ThrowFBX(entry.path())))
					return E_FAIL;
			}
		}
	}
}

HRESULT CLevel_Zero::ThrowFBX(const wstring& _wStrFolderPath)
{
	HANDLE fileSearch;
	WIN32_FIND_DATA wfd;
	const wstring& cStrPBXPath = _wStrFolderPath;
	CModel::MODEL_TYPE eModelType;

	fileSearch = FindFirstFile(cStrPBXPath.c_str(), &wfd);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// ã�� ������ �ִٸ�, 
	if (fileSearch != INVALID_HANDLE_VALUE)
	{
		// Anim, NonAnim �Ǻ��ϱ�
		wstring animDir = L"/Anim\\";
		wstring nonAnimDir = L"/NonAnim\\";

		size_t animPos = cStrPBXPath.find(animDir);
		size_t nonAnimPos = cStrPBXPath.find(nonAnimDir);

		if (animPos != wstring::npos) {
			wstring extractedPath = cStrPBXPath.substr(animPos + animDir.length());
			eModelType = CModel::TYPE_ANIM;
		}
		else if (nonAnimPos != wstring::npos) {
			wstring extractedPath = cStrPBXPath.substr(nonAnimPos + nonAnimDir.length());
			eModelType = CModel::TYPE_NONANIM;
		}
		else
		{
			MSG_BOX("Anim, NonAnim Ȯ�� �Ұ�.");
			return E_FAIL;
		}

		/* ���̺� ��� ���� */
		string StrSavePath = "../Bin/Resource/BinaryFBX/";

		wstring originalPath = cStrPBXPath;
		wstring pathToRemove = TEXT("../Bin/Resource/Models/Skyrim/");

		size_t pos = originalPath.find(pathToRemove);

		if (pos != wstring::npos) {
			originalPath.erase(pos, pathToRemove.length());
		}

		wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
		// FBX ��� ��ȯ
		string strPath = converter.to_bytes(cStrPBXPath);
		// �߶� ���� ��� ��ȯ 
		string  ConvertPathToRemove = converter.to_bytes(originalPath);

		// �߶� ���� ��� ���� ���� ��ο� ��ġ��.
		StrSavePath += ConvertPathToRemove;

		// Ȯ���� �ڸ���
		size_t lastDotPos = StrSavePath.find_last_of('.');
		if (lastDotPos != string::npos) {
			// Ȯ���� ����
			StrSavePath.substr(lastDotPos + 1);
			// Ȯ���� ������ ���� �̸� ����
			StrSavePath = StrSavePath.substr(0, lastDotPos);
		}
		else {
			MSG_BOX("Ȯ���ڰ� ���ų� Ȯ�� �Ұ�.");
			return E_FAIL;
		}

		// Ȯ���� �߰�
		StrSavePath += ".bin";

		/* ���� ��� ����(��ü ������ ���� ���� �����ϱ� ����) */
		/* ���ϸ� �ڸ��� ���������� ����� */
		string StrPath = filesystem::path(StrSavePath).remove_filename().string(); 
		filesystem::path directory = filesystem::path(StrPath).parent_path();

		// ������ �������� �ʴ� ���, ������ ����
		if (!filesystem::exists(directory)) {
			/* ���� ó��*/
			if (!filesystem::create_directories(directory)) {
				MSG_BOX("���� ���� ����.");
				return E_FAIL;
			}
		}

		/* FBX ���̳ʸ�ȭ ���� */
		ofstream fileStream(StrSavePath, ios::binary);
		if (fileStream.is_open())
		{
			pGameInstance->Binary_OutFile(fileStream, strPath.c_str(), eModelType);

			fileStream.close();
		}
		else
			MessageBox(g_hWnd, L"������ �����ϴ� �� ������ �߻��߽��ϴ�.", L"���� ����", MB_OK | MB_ICONERROR);

		Safe_Release(pGameInstance);
		// ���� ã�� �ڵ� �� �ݱ�	
		FindClose(fileSearch);
	}

	return S_OK;
}

CLevel_Zero* CLevel_Zero::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_Zero* pInstance = new CLevel_Zero(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Fail Create : CLevel_Zero");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Zero::Free()
{
	__super::Free();
}



/* HRESULT CLevel_Zero::ThrowFBX(CString _cStrFolderName)
{
	HANDLE fileSearch;
	WIN32_FIND_DATA wfd;
	CString musiccount;
	CString cStrPBXPath;
	CString cStrFolderName = _cStrFolderName;
	CModel::MODEL_TYPE eModelType = CModel::TYPE_END;

	// �ϴ� �� �������� anim, nonanim���� ������ �ξ���.
	// ���ٿͼ� ��� ��� ã���� Ȯ������

	// Anim, NonAnim�� ���� �� txt ������ ���� ã�´�.
musiccount.Format(_T("../Bin/Resource/Models/Skyrim/") + cStrFolderName + _T("/*.txt"));

// FindFirstFile �Լ��� ���� �˻��Ϸ��� �ϴ� ������ ���� ��� 
// �ڵ� ���� INVALID_HANDLE_VALUE ���� ����
fileSearch = FindFirstFile(musiccount, &wfd);

if (fileSearch != INVALID_HANDLE_VALUE)
{
	if (_tcscmp(wfd.cFileName, _T("00.NonAnim.txt")) == 0)
		eModelType = CModel::TYPE_NONANIM;

	else if (_tcscmp(wfd.cFileName, _T("00.Anim.txt")) == 0)
		eModelType = CModel::TYPE_ANIM;

	else
	{
		MSG_BOX("�ش��ϴ� txt ���� �� ã��.");
		return E_FAIL;
	}
}
else
{
	MSG_BOX("Fail Find : Type.Txt");
	return E_FAIL;
}

// FBX ã��
musiccount.Format(_T("../Bin/Resource/Models/Skyrim/") + cStrFolderName + _T("/*.fbx"));
fileSearch = FindFirstFile(musiccount, &wfd);

CGameInstance* pGameInstance = CGameInstance::GetInstance();
Safe_AddRef(pGameInstance);

// ã�� ������ �ִٸ�, 
if (fileSearch != INVALID_HANDLE_VALUE)
{
	do
	{
		// Ȯ���� �ڸ���
		CString cStrFBXName = CString(wfd.cFileName);
		int lastDotPos = cStrFBXName.ReverseFind('.');
		if (lastDotPos != -1) {
			CString Extension = cStrFBXName.Right(cStrFBXName.GetLength() - lastDotPos - 1);
			cStrFBXName = cStrFBXName.Left(lastDotPos);
		}
		else
		{
			MSG_BOX("Ȯ���ڸ� ã�� �� ����. ");
			return E_FAIL;
		}


		// CString -> const char* : FBX ��� const char*�� ��ȯ. (strFBXPath)
		cStrPBXPath = "../Bin/Resource/Models/Skyrim/" + cStrFolderName + "/" + CString(wfd.cFileName) + ".bin";
		const wchar_t* wStrModelPath = cStrPBXPath;
		// wchar_t �迭�� UTF-8�� ��ȯ
		int utf8Length = WideCharToMultiByte(CP_UTF8, 0, wStrModelPath, -1, nullptr, 0, nullptr, nullptr);
		string strFBXPath(utf8Length, '\0');
		WideCharToMultiByte(CP_UTF8, 0, wStrModelPath, -1, &strFBXPath[0], utf8Length, nullptr, nullptr);

		// ���̺� ��� ���� 
		CString cStrSavePath = "../Bin/Resource/BinaryFBX/";

		if (eModelType == CModel::TYPE_ANIM)
			cStrSavePath += "Anim/";
		else if (eModelType == CModel::TYPE_NONANIM)
			cStrSavePath += "NonAnim/";

		cStrSavePath += cStrFolderName;
		cStrSavePath += "/";
		cStrSavePath += cStrFBXName;

		// FBX ���̳ʸ�ȭ ���� 
		ofstream fileStream(cStrSavePath, ios::binary);
		if (fileStream.is_open())
		{
			pGameInstance->Binary_OutFile(fileStream, strFBXPath.c_str(), eModelType);

			fileStream.close();
		}
		else
			MessageBox(g_hWnd, L"������ �����ϴ� �� ������ �߻��߽��ϴ�.", L"���� ����", MB_OK | MB_ICONERROR);

	} while (FindNextFile(fileSearch, &wfd));

	Safe_Release(pGameInstance);
	// ���� ã�� �ڵ� �� �ݱ�	
	FindClose(fileSearch);
}

return S_OK;
} */