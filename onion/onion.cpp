// onion.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "onion.h"


int wmain(int argc, wchar_t* argv[])
{

	wchar_t wszVehicleFile[MAX_PATH];
	wchar_t wszPayloadFile[MAX_PATH];
	wchar_t* pwszResourceType = L"ONION";
	PBYTE pEncKey = (PBYTE)"ASSCLOWN";
	DWORD dwEncKeyLen = strlen((char*)pEncKey);

	if (argc == 5 && _wcsicmp(L"/e", argv[1]) == 0) {

		GetFullPathName(argv[2], _countof(wszVehicleFile), wszVehicleFile, NULL);
		GetFullPathName(argv[3], _countof(wszPayloadFile), wszPayloadFile, NULL);
		EmbedEncryptedResource(wszVehicleFile, wszPayloadFile, pwszResourceType, argv[4], pEncKey, dwEncKeyLen);

	}
	else if (argc == 5 && _wcsicmp(L"/x", argv[1]) == 0) {

		GetFullPathName(argv[2], _countof(wszVehicleFile), wszVehicleFile, NULL);
		GetFullPathName(argv[3], _countof(wszPayloadFile), wszPayloadFile, NULL);
		ExtractEncryptedResource(wszVehicleFile, wszPayloadFile, pwszResourceType, argv[4], pEncKey, dwEncKeyLen);

	}
	else {

		wprintf(L"Onion 1.0a - embed and extract obfuscated PE file resources\r\n");
		wprintf(L"https://github.com/pmolchanov/onion \r\n");
		wprintf(L"\r\n");
		wprintf(L"usage: onion.exe /e | /x vehicle_file payload_file resource_name \r\n");
		wprintf(L"\r\n");
		wprintf(L"  /e  Embeds the payload_file as an encrypted resource in the vehicle_file. \r\n");
		wprintf(L"  /x  Extracts the encrypted resource from the vehicle_file into the payload_file. \r\n");
		wprintf(L"\r\n");
		wprintf(L"Example 1: onion.exe /e vehicle.exe payload.dat MyResourceName \r\n");
		wprintf(L"\r\n");
		wprintf(L"Example 2: onion.exe /x vehicle.exe payload.dat MyResourceName \r\n");

	}

	return 0;
}

