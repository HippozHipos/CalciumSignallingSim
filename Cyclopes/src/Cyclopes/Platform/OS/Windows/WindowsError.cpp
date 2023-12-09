#include "Cycpch.h"
#include "WindowsError.h"

namespace cyc {

	CycWString TranslateWin32ErrorCodeW(HRESULT hr)
	{
		wchar_t* msgBuffer = nullptr;
		DWORD nMsgLen = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			hr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPWSTR>(&msgBuffer), 0, nullptr
		);

		if (nMsgLen == 0)
		{
			return L"Undefined Error Code";
		}

		CycWString error = msgBuffer;
		LocalFree(msgBuffer);
		return error;
	}

	CycString TranslateWin32ErrorCodeA(HRESULT hr)
	{
		char* msgBuffer = nullptr;
		DWORD nMsgLen = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			hr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPSTR>(&msgBuffer), 0, nullptr
		);

		if (nMsgLen == 0)
		{
			return "Undefined Error Code";
		}

		CycString error = msgBuffer;
		LocalFree(msgBuffer);
		return error;
	}

}
