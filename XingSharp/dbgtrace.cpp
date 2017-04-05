#include "stdafx.h"
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>

void CLR_OutputDebugStringW(const WCHAR* pszMsgW)
{
	System::Diagnostics::Debug::Write(gcnew System::String(pszMsgW));
}

void CLR_OutputDebugStringA(const CHAR* pszMsgA)
{
	System::Diagnostics::Debug::Write(gcnew System::String(pszMsgA));
}

#pragma unmanaged

void OutputDebugPrint(LPCWSTR szFormat, ...)
{
	WCHAR szBuffer[1024];

	va_list ap;
	va_start(ap, szFormat);

	StringCbVPrintfW(szBuffer, sizeof(szBuffer), szFormat, ap);

	va_end(ap);

	CLR_OutputDebugStringW(szBuffer);
}

#ifdef _DEBUG

void _TRACE(LPCWSTR szFormat, ...)
{
	WCHAR szBuffer[1024];

	va_list ap;
	va_start(ap, szFormat);
	
	StringCbVPrintfW(szBuffer, sizeof(szBuffer), szFormat, ap);

	va_end(ap);

	CLR_OutputDebugStringW(szBuffer);
}

void _TRACE(LPCSTR pszFormat, ...)
{
	CHAR szBuffer[1024];
	
	va_list ap;
	va_start(ap, pszFormat);
	
	StringCbVPrintfA(szBuffer, sizeof(szBuffer), pszFormat, ap);
	
	va_end(ap);
	
	CLR_OutputDebugStringA(szBuffer);
}

TCHAR* GetLastErrorMsg()
{
	static TCHAR buff[256];

	FormatMessage( 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		buff, sizeof(buff), NULL);

	return buff;
}

#endif
