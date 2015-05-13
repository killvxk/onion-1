#pragma once
#include <Windows.h>

DWORD EmbedEncryptedResource(wchar_t* pwszVehicleFile, wchar_t* pwszPayloadFile, wchar_t* pwszResourceType, wchar_t* pwszResourceName, PBYTE pEncKey, DWORD dwEncKeyLen);
DWORD ExtractEncryptedResource(wchar_t* pwszVehicleFile, wchar_t* pwszPayloadFile, wchar_t* pwszResourceType, wchar_t* pwszResourceName);
