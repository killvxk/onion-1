#include "stdafx.h"
#include "onion.h"

#ifndef TRACE
#define TRACE wprintf
#endif

void XORCrypt(PBYTE pData, DWORD dwDataLen, PBYTE pEncKey, DWORD pEncKeyLen)
{
	DWORD i, j;
	for (i = 0; i < dwDataLen; i++) {
		j = i % pEncKeyLen;
		pData[i] = pData[i] ^ pEncKey[j];
	}
}

DWORD EmbedEncryptedResource(wchar_t* pwszVehicleFile, wchar_t* pwszPayloadFile, wchar_t* pwszResourceType, wchar_t* pwszResourceName, PBYTE pEncKey, DWORD dwEncKeyLen) {

	DWORD dwReturnCode = 1;
	HANDLE hPayloadFile = NULL;
	DWORD cbPayloadFileSize = 0;
	PBYTE pPayloadData = NULL;
	DWORD cbPayloadBytesRead = 0;
	HANDLE hResource = NULL;

	do {

		// Open payload file for reading
		hPayloadFile = CreateFile(pwszPayloadFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hPayloadFile == INVALID_HANDLE_VALUE) {
			TRACE(L"CreateFile error: %d", GetLastError());
			break;
		}

		// Get file size
		cbPayloadFileSize = GetFileSize(hPayloadFile, NULL);
		if (cbPayloadFileSize == INVALID_FILE_SIZE) {
			TRACE(L"GetFileSize error: %d", GetLastError());
			break;
		}

		// Allocate buffer to store: 
		//   * Encryption Key Length (4 bytes)
		//   * Encryption Key (variable size)
		//   * Encrypted Payload File (variable size)
		pPayloadData = (BYTE*)malloc(sizeof(DWORD) + dwEncKeyLen + cbPayloadFileSize);
		if (pPayloadData == NULL) {
			TRACE(L"malloc error: %d", GetLastError());
			break;
		}

		// Write the encryption key length to the payload
		memcpy(pPayloadData, &dwEncKeyLen, sizeof(DWORD));

		// Write the encryption key to the payload
		memcpy((pPayloadData + sizeof(DWORD)), pEncKey, dwEncKeyLen);

		// Read raw file bytes into memory
		if (ReadFile(hPayloadFile, (pPayloadData + sizeof(DWORD) + dwEncKeyLen), cbPayloadFileSize, &cbPayloadBytesRead, NULL) == FALSE) {
			TRACE(L"ReadFile error: %d", GetLastError());
			break;
		}

		// In-place encrypt the raw file bytes
		XORCrypt((pPayloadData + sizeof(DWORD) + dwEncKeyLen), cbPayloadBytesRead, (pPayloadData + sizeof(DWORD)), dwEncKeyLen);

		// Get a handle to the resource so we can update it
		hResource = BeginUpdateResource(pwszVehicleFile, FALSE);
		if (hResource == NULL) {
			TRACE(L"BeginUpdateResource error: %d", GetLastError());
			break;
		}

		// Update the resource
		if (UpdateResource(hResource, pwszResourceType, pwszResourceName, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), pPayloadData, (sizeof(DWORD) + dwEncKeyLen + cbPayloadBytesRead)) == FALSE) {
			TRACE(L"UpdateResource error: %d", GetLastError());
			break;
		}

		dwReturnCode = 0;

	} while (FALSE);

	// Cleanup
	if (hPayloadFile != NULL) {
		CloseHandle(hPayloadFile);
		hPayloadFile = NULL;
	}

	if (hResource != NULL) {
		EndUpdateResource(hResource, FALSE);
		hResource = NULL;
	}

	if (pPayloadData != NULL) {
		free(pPayloadData);
		pPayloadData = NULL;
	}

	return dwReturnCode;

}

DWORD ExtractEncryptedResource(wchar_t* pwszVehicleFile, wchar_t* pwszPayloadFile, wchar_t* pwszResourceType, wchar_t* pwszResourceName) {

	DWORD dwReturnCode = 1;
	HMODULE hModule = NULL;
	HRSRC hFindResource = NULL;
	DWORD dwPayloadFileSize = 0;
	HGLOBAL hLoadResource = NULL;
	PBYTE pPayloadData = NULL;
	PBYTE pPayloadDataCopy = NULL;
	HANDLE hPayloadFile = NULL;
	DWORD dwPayloadFileBytesWritten = 0;

	do {

		// Load module 
		hModule = LoadLibraryEx(pwszVehicleFile, NULL, LOAD_LIBRARY_AS_DATAFILE);
		if (hModule == NULL) {
			TRACE(L"LoadLibraryEx error: %d", GetLastError());
			break;
		}

		// Find the resource
		hFindResource = FindResource(hModule, pwszResourceName, pwszResourceType);
		if (hFindResource == NULL) {
			TRACE(L"FindResource error: %d", GetLastError());
			break;
		}

		// Get the size of the resource
		dwPayloadFileSize = SizeofResource(hModule, hFindResource);
		if (dwPayloadFileSize == 0) {
			TRACE(L"SizeofResource error: %d", GetLastError());
			break;
		}

		// Load the resource
		hLoadResource = LoadResource(hModule, hFindResource);
		if (hLoadResource == NULL) {
			TRACE(L"LoadResource error: %d", GetLastError());
			break;
		}

		// Get raw bytes of resource
		pPayloadData = (PBYTE)LockResource(hLoadResource);
		if (pPayloadData == NULL) {
			TRACE(L"LockResource error: %d", GetLastError());
			break;
		}

		// Get the encryption key length
		DWORD dwEncKeyLen = 0;
		memcpy(&dwEncKeyLen, pPayloadData, sizeof(DWORD));

		// Copy the encrypted bytes to a writeable buffer
		pPayloadDataCopy = (PBYTE)malloc(dwPayloadFileSize - sizeof(DWORD) - dwEncKeyLen);
		if (pPayloadDataCopy == NULL) {
			TRACE(L"malloc error: %d", GetLastError());
			break;
		}
		memcpy(pPayloadDataCopy, (pPayloadData + sizeof(DWORD) + dwEncKeyLen), (dwPayloadFileSize - sizeof(DWORD) - dwEncKeyLen));


		// Decrypt the bytes
		XORCrypt(pPayloadDataCopy, (dwPayloadFileSize - sizeof(DWORD) - dwEncKeyLen), (pPayloadData + sizeof(DWORD)), dwEncKeyLen);

		// Create a payload file
		hPayloadFile = CreateFile(pwszPayloadFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hPayloadFile == INVALID_HANDLE_VALUE) {
			TRACE(L"CreateFile error: %d", GetLastError());
			break;
		}

		// Write the payload out to the file
		if (WriteFile(hPayloadFile, pPayloadDataCopy, (dwPayloadFileSize - sizeof(DWORD) - dwEncKeyLen), &dwPayloadFileBytesWritten, FALSE) == FALSE) {
			TRACE(L"WriteFile error: %d", GetLastError());
			break;
		}

		dwReturnCode = 0;

	} while (FALSE);

	// Cleanup
	if (hPayloadFile != NULL) {
		CloseHandle(hPayloadFile);
		hPayloadFile = NULL;
	}

	if (pPayloadDataCopy != NULL) {
		free(pPayloadDataCopy);
		pPayloadDataCopy = NULL;
	}

	if (hModule != NULL) {
		FreeLibrary(hModule);
		hModule = NULL;
	}

	return dwReturnCode;
}
