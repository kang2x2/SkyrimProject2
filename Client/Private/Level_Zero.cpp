#include "framework.h"

#include <filesystem>
#include <fstream>
#include <commdlg.h>

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

		Start_Binary();

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

void CLevel_Zero::Start_Binary()
{
	// ���� �˻��Ͽ� findFileData�� ����.
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile(_T("../Bin/Resource/Models/Skyrim/*"), &findFileData);

	// ���� ó��
	if (hFind == INVALID_HANDLE_VALUE) {
		MSG_BOX("Fail Find : Skyrim Folder");
		return;
	}
	do {
		// ã���� �������� �˻�.
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (_tcscmp(findFileData.cFileName, _T(".")) != 0 && _tcscmp(findFileData.cFileName, _T("..")) != 0) {
			
				Find_FBX(findFileData.cFileName);

			}
		}
	} while (FindNextFile(hFind, &findFileData) != 0);

	FindClose(hFind);
}

HRESULT CLevel_Zero::Find_FBX(CString _cStrFolderName)
{
	HANDLE fileSearch;
	WIN32_FIND_DATA wfd;
	CString musiccount;
	CString cStrPBXPath;
	CString cStrFolderName = _cStrFolderName;
	CModel::MODEL_TYPE eModelType = CModel::TYPE_END;


	/* Anim, NonAnim�� ���� �� txt ������ ���� ã�´�. */
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

	/* FBX ã�� */
	musiccount.Format(_T("../Bin/Resource/Models/Skyrim/") + cStrFolderName + _T("/*.fbx"));
	fileSearch = FindFirstFile(musiccount, &wfd);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// ã�� ������ �ִٸ�, 
	if (fileSearch != INVALID_HANDLE_VALUE)
	{
		do
		{
			/* Ȯ���� �ڸ��� */
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


			/* CString -> const char* : FBX ��� const char*�� ��ȯ. (strFBXPath) */
			cStrPBXPath = "../Bin/Resource/Models/Skyrim/" + cStrFolderName + "/" + CString(wfd.cFileName);
			const wchar_t* wStrModelPath = cStrPBXPath;
				// wchar_t �迭�� UTF-8�� ��ȯ
			int utf8Length = WideCharToMultiByte(CP_UTF8, 0, wStrModelPath, -1, nullptr, 0, nullptr, nullptr);
			string strFBXPath(utf8Length, '\0');
			WideCharToMultiByte(CP_UTF8, 0, wStrModelPath, -1, &strFBXPath[0], utf8Length, nullptr, nullptr);

			/* ���̺� ��� ���� */
			CString cStrSavePath = "../Bin/Resource/BinaryFBX/";

			if (eModelType == CModel::TYPE_ANIM)
				cStrSavePath += "Anim/";
			else if(eModelType == CModel::TYPE_NONANIM)
				cStrSavePath += "NonAnim/";

			cStrSavePath += cStrFolderName;
			cStrSavePath += "/";
			cStrSavePath += cStrFBXName;

			/* FBX ���̳ʸ�ȭ ���� */
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
