// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Target Windows XP SP3
#define _WIN32_WINNT 0x0501
#define NTDDI_VERSION 0x05010300
#include <SDKDDKVer.h>

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN   

#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>

