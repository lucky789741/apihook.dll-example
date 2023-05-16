#include "pch.h"
#include "hook.h"
#include "logger.h"
namespace HOOK
{
	FARPROC GetCurrentPrcoessAddress = NULL;
	SIZE_T bytesWritten = 0;
	BYTE GetCurrentProcessOriginalBytes[patchSize] = {};

	HANDLE WINAPI HookedGetCurrentProcess()
	{
		return 0;
	}

	BOOL unHook()
	{
		BOOL iswritten = WriteProcessMemory((HANDLE)-1, (LPVOID)GetCurrentPrcoessAddress, GetCurrentProcessOriginalBytes, sizeof(GetCurrentProcessOriginalBytes), &bytesWritten);
		return iswritten;
	}

	BOOL hook()
	{
		//獲得GetCurrentProcess的地址
		HINSTANCE library = LoadLibraryA("kernel32.dll");
		if (library == NULL) return 0;
		GetCurrentPrcoessAddress = GetProcAddress(library, "GetCurrentProcess");
		std::string addrString = Logger::getInstance().AddressToHexString((PDWORD_PTR)GetCurrentPrcoessAddress);
		Logger::getInstance().print("修改的地址:\t"+addrString);
		//儲存原始14 byte至 GetCurrentProcessOriginalBytes
		SIZE_T bytesRead = 0;
		BOOL isreaded = ReadProcessMemory((HANDLE)-1, GetCurrentPrcoessAddress, GetCurrentProcessOriginalBytes, patchSize, &bytesRead);
		if (isreaded == NULL) return 0;
		std::string hex = Logger::getInstance().BytesInHex(GetCurrentProcessOriginalBytes, patchSize);
		Logger::getInstance().print("原始位元組:\t"+hex);
		//使用union將同一塊記憶體上下區段分別存入lo、hi
		union
		{
			void* ptr;
			struct
			{
				long lo;
				long hi;
			};
		} ptr64;
		ptr64.ptr = (void*)HookedGetCurrentProcess;

		/*
		patch patchSize byte to the GetCurrentProcess's memory
		將patchSize個byte的補丁打至GetCurrentProcess的記憶體
		push 低位
		mov [rsp+04],高位
		ret
		 */
		BYTE patch[patchSize] = { 0 };
		memcpy_s(patch, 1, "\x68", 1);//push
		memcpy_s(patch + 1, 4, (void*)&ptr64.lo, 4);//低位
		memcpy_s(patch + 5, 1, "\xC7", 1);//mov
		memcpy_s(patch + 6, 1, "\x44", 1);//rsp
		memcpy_s(patch + 7, 1, "\x24", 1);//[]
		memcpy_s(patch + 8, 1, "\x04", 1);//04
		memcpy_s(patch + 9, 4, (void*)&ptr64.hi, 4);//高位
		memcpy_s(patch + 13, 1, "\xC3", 1);//ret

		//打印
		hex = Logger::getInstance().BytesInHex(patch, patchSize);
		Logger::getInstance().print("修改位元組:\t"+hex);

		BOOL iswritten = WriteProcessMemory((HANDLE)-1, (LPVOID)GetCurrentPrcoessAddress, patch, sizeof(patch), &bytesWritten);
		return iswritten;
	}
}