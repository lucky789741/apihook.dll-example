#pragma once
#include <Windows.h>
namespace HOOK
{
	CONST WORD patchSize = 14;
	HANDLE WINAPI HookedGetCurrentProcess();
	BOOL unHook();
	BOOL hook();
};