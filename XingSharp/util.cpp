#include "stdafx.h"
#include "util.h"

using namespace System;
using namespace System::Runtime::InteropServices;

#define CP_949		949

#define MAX_A2W_LENGTH		1024

WCHAR a2w_buff[MAX_A2W_LENGTH];

void* GetTempBuffer()
{
	return a2w_buff;
}

LPWSTR A2W(LPCSTR pszSrc)
{
	MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, a2w_buff, MAX_A2W_LENGTH);
	return a2w_buff;
}

LPSTR W2A(LPCWSTR pszSrc)
{
	char* buff = (char*)a2w_buff;
	WideCharToMultiByte(CP_ACP, 0, pszSrc, -1, buff, MAX_A2W_LENGTH, NULL, NULL);
	return buff;
}

LPSTR W2K(LPCWSTR pszSrc)
{
	char* buff = (char*)a2w_buff;
	WideCharToMultiByte(CP_949, 0, pszSrc, -1, buff, MAX_A2W_LENGTH, NULL, NULL);
	return buff;
}

std::wstring KRtoWide(const char* pKrStr, int klen)
{
	int lenO = klen;

	// Trim Right
	while (klen > 0)
	{
		int ch = pKrStr[klen - 1];
		if (ch == ' ' || ch == 0)
			--klen;
		else
			break;
	}

	// trash characters between '\0' and len
	for (int i = 0; i < klen; i++)
	{
		if (pKrStr[i] == '\0')
		{
			klen = i;
			break;
		}
	}

	if (klen == 0)
	{
		return L"";
	}

	//TRACE("KRtoWide(%d -> %d)\n", lenO, klen);

	int nReq = MultiByteToWideChar(CP_949, 0, pKrStr, klen, NULL, 0);
	int cbAlloc = ++nReq * sizeof(WCHAR);
	WCHAR* pBuff = (WCHAR*)_malloca(cbAlloc);
	int wlen = MultiByteToWideChar(CP_949, 0, pKrStr, klen, pBuff, nReq);
	ASSERT(wlen >= 0);
	std::wstring ret(pBuff, &pBuff[wlen]);
	_freea(pBuff);

	return ret;
}

std::string KRtoAnsi(const char* pKrStr, int len)
{
	// Trim Right
	while (len > 0)
	{
		int ch = pKrStr[len - 1];
		if (ch == ' ' || ch == 0)
			--len;
		else
			break;
	}

	if (len <= 0)
		return pKrStr;

	std::string ret(pKrStr, &pKrStr[len]);
	return ret;
}

System::String^ GetString(const char* pData, int nDataLen)
{
	return gcnew System::String(pData, 0, nDataLen);
}

std::wstring MStringToWString(System::String^ str)
{
	std::wstring ret;

	if (!System::String::IsNullOrEmpty(str))
	{
		WCHAR* pszSrc = (WCHAR*)Marshal::StringToHGlobalUni(str).ToPointer();
		ret = pszSrc;
		Marshal::FreeHGlobal(IntPtr(pszSrc));
	}

	return ret;
}

System::String^ WStringToMString(const std::wstring& str)
{
	if (str.length() == 0)
		return nullptr;

	return gcnew System::String(str.c_str(), 0, str.length());
}


static void _MakeDotNumberString(char *pBuff, const char* pData, int nDataLen, int nDotPos)
{
	ASSERT(nDataLen < 32 && nDotPos < nDataLen);

	int nPreDotLen = nDataLen - nDotPos;
	int i = 0;
	for (; i < nPreDotLen; i++)
	{
		*pBuff++ = pData[i];
	}
	*pBuff++ = '.';

	for (; i < nDataLen; i++)
	{
		*pBuff++ = pData[i];;
	}
	*pBuff++ = 0;
}

double ParseFloat(const char* pData, int nDataLen, int nDotPos)
{
	char buff[32];
	_MakeDotNumberString(buff, pData, nDataLen, nDotPos);
	return atof(buff);
}

System::String^ GetFloatString(const char* pData, int nDataLen, int nDotPos)
{
	char buff[32];
	_MakeDotNumberString(buff, pData, nDataLen, nDotPos);

	char* pStart = buff;
	while (*pStart == '0')
	{
		if (pStart[1] == '.' || pStart[1] == 0)
			break;

		pStart++;
	}

	return gcnew System::String(pStart);
}

int ParseInteger(const char* pData, int nDataLen)
{
	char buff[32];
	ASSERT(nDataLen < 32);

	memcpy(buff, pData, nDataLen);
	buff[nDataLen] = 0;

	return atoi(buff);
}

System::String^ GetIntString(const char* pData, int nDataLen)
{
	char buff[32];
	ASSERT(nDataLen < 32);

	memcpy(buff, pData, nDataLen);
	buff[nDataLen] = 0;

	char* pStart = buff;
	char* pEnd = &buff[nDataLen - 1];
	while (*pEnd == ' ' && pStart < pEnd)
	{
		*pEnd-- = 0;
	}

	bool isMinus = (*pStart == '-');
	if (isMinus)
		pStart++;

	while (*pStart == '0')
	{
		pStart++;
	}

	if (isMinus)
	{
		--pStart;
		*pStart = '-';
	}

	return (gcnew System::String(pStart));
}

void CopyStringAndFillSpace(char* pDest, int lenDest, const char* pszSource)
{
	int lenSrc = strlen(pszSource);
	int nCopy = (lenDest < lenSrc) ? lenDest : lenSrc;
	memcpy(pDest, pszSource, nCopy);
	
	while (nCopy < lenDest)
	{
		pDest[nCopy++] = ' ';
	}
}
