#include <string>
#include <Windows.h>
#include <shobjidl.h>
#include <codecvt>

namespace DataGraph::utils
{
bool openFile(std::string& filePath)
{
	//  CREATE FILE OBJECT INSTANCE
	HRESULT f_SysHr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(f_SysHr))
		return FALSE;

	// CREATE FileOpenDialog OBJECT
	IFileOpenDialog* f_FileSystem = nullptr;
	f_SysHr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&f_FileSystem));
	if (FAILED(f_SysHr))
	{
		CoUninitialize();
		return FALSE;
	}
	COMDLG_FILTERSPEC fileTypes[] = {
		 {L"Image Files", L"*.bmp;*.jpg;*.png"},
	};
	f_SysHr = f_FileSystem->SetFileTypes(ARRAYSIZE(fileTypes), fileTypes);
	if (FAILED(f_SysHr))
	{
		f_FileSystem->Release();
		CoUninitialize();
		return FALSE;
	}

	DWORD dwOptions;
	f_SysHr = f_FileSystem->GetOptions(&dwOptions);
	if (FAILED(f_SysHr))
	{
		f_FileSystem->Release();
		CoUninitialize();
		return FALSE;
	}

	f_SysHr = f_FileSystem->SetOptions(dwOptions | FOS_FORCEFILESYSTEM);
	if (FAILED(f_SysHr))
	{
		f_FileSystem->Release();
		CoUninitialize();
		return FALSE;
	}

	//  SHOW OPEN FILE DIALOG WINDOW
	f_SysHr = f_FileSystem->Show(NULL);
	if (FAILED(f_SysHr))
	{
		f_FileSystem->Release();
		CoUninitialize();
		return FALSE;
	}


	// Get the selected file name.
	IShellItem* pItem;
	f_SysHr = f_FileSystem->GetResult(&pItem);
	if (FAILED(f_SysHr))
	{
		f_FileSystem->Release();
		CoUninitialize();
		return FALSE;
	}

	LPWSTR pszFilePath;
	f_SysHr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
	if (FAILED(f_SysHr))
	{
		pItem->Release();
		f_FileSystem->Release();
		CoUninitialize();
		return FALSE;
	}

	// Check the file size.
	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFileEx(pszFilePath, FindExInfoStandard, &findData, FindExSearchNameMatch, NULL, 0);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		ULARGE_INTEGER fileSize{};
		fileSize.LowPart = findData.nFileSizeLow;
		fileSize.HighPart = findData.nFileSizeHigh;
		if (fileSize.QuadPart > 2 * 1024 * 1024)  // 100 MB
		{
			FindClose(hFind);
			CoTaskMemFree(pszFilePath);
			pItem->Release();
			f_FileSystem->Release();
			CoUninitialize();

			return FALSE;
		}

		FindClose(hFind);
	}

	//  FORMAT AND STORE THE FILE PATH
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;
	filePath = convert.to_bytes(pszFilePath);

	//  SUCCESS, CLEAN UP
	CoTaskMemFree(pszFilePath);
	pItem->Release();
	f_FileSystem->Release();
	CoUninitialize();

	return TRUE;
}

}  // namespace DataGraph::utils
