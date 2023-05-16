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
		//��oGetCurrentProcess���a�}
		HINSTANCE library = LoadLibraryA("kernel32.dll");
		if (library == NULL) return 0;
		GetCurrentPrcoessAddress = GetProcAddress(library, "GetCurrentProcess");
		std::string addrString = Logger::getInstance().AddressToHexString((PDWORD_PTR)GetCurrentPrcoessAddress);
		Logger::getInstance().print("�ק諸�a�}:\t"+addrString);
		//�x�s��l14 byte�� GetCurrentProcessOriginalBytes
		SIZE_T bytesRead = 0;
		BOOL isreaded = ReadProcessMemory((HANDLE)-1, GetCurrentPrcoessAddress, GetCurrentProcessOriginalBytes, patchSize, &bytesRead);
		if (isreaded == NULL) return 0;
		std::string hex = Logger::getInstance().BytesInHex(GetCurrentProcessOriginalBytes, patchSize);
		Logger::getInstance().print("��l�줸��:\t"+hex);
		//�ϥ�union�N�P�@���O����W�U�Ϭq���O�s�Jlo�Bhi
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
		�NpatchSize��byte���ɤB����GetCurrentProcess���O����
		push �C��
		mov [rsp+04],����
		ret
		 */
		BYTE patch[patchSize] = { 0 };
		memcpy_s(patch, 1, "\x68", 1);//push
		memcpy_s(patch + 1, 4, (void*)&ptr64.lo, 4);//�C��
		memcpy_s(patch + 5, 1, "\xC7", 1);//mov
		memcpy_s(patch + 6, 1, "\x44", 1);//rsp
		memcpy_s(patch + 7, 1, "\x24", 1);//[]
		memcpy_s(patch + 8, 1, "\x04", 1);//04
		memcpy_s(patch + 9, 4, (void*)&ptr64.hi, 4);//����
		memcpy_s(patch + 13, 1, "\xC3", 1);//ret

		//���L
		hex = Logger::getInstance().BytesInHex(patch, patchSize);
		Logger::getInstance().print("�ק�줸��:\t"+hex);

		BOOL iswritten = WriteProcessMemory((HANDLE)-1, (LPVOID)GetCurrentPrcoessAddress, patch, sizeof(patch), &bytesWritten);
		return iswritten;
	}
}