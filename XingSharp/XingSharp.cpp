// This is the main DLL file.

#include "stdafx.h"

#include "XingSharp.h"
#include "apiwrap.h"

namespace XingSharp {

XingApi::XingApi(IXingApiListener^ pListener)
{
	m_pListener = pListener;
	m_pApiWrapper = new ApiWrapper(this);
}

bool XingApi::Create(String^ apiDir, [Out] String^% errorMsgOut)
{
	if (m_pApiWrapper != NULL && m_pApiWrapper->IsValid() != NULL)
	{
		errorMsgOut = "XingApi is already initialized.";
		return false;
	}

	TCHAR* pszApiDir = (TCHAR*)Marshal::StringToHGlobalUni(apiDir).ToPointer();
	if (pszApiDir == NULL)
	{
		errorMsgOut = "ApiDir parameter is NULL.";
		return false;
	}

	TCHAR szBuff[MAX_PATH];
	bool ret = m_pApiWrapper->Create(pszApiDir, szBuff);
	Marshal::FreeHGlobal(IntPtr(pszApiDir));
	
	return ret;
}

void XingApi::Destroy()
{
	if (m_pApiWrapper != NULL)
	{
		m_pApiWrapper->Destroy();
		delete m_pApiWrapper;
		m_pApiWrapper = NULL;
	}
}

bool XingApi::IsConnected()
{
	return m_pApiWrapper->IsConnected();
}

bool XingApi::Connect(String^ address, int port)
{
	std::string strAddress;
	if (address == nullptr)
	{
		strAddress = "hts.etrade.co.kr";
	}
	else
	{
		CHAR* pszAddress = (CHAR*)Marshal::StringToHGlobalAnsi(address).ToPointer();
		strAddress = pszAddress;
		Marshal::FreeHGlobal(IntPtr(pszAddress));
	}

	if (port <= 0)
	{
		port = 20001;
	}

	ASSERT(port < 0xFFFF && !strAddress.empty());
	return m_pApiWrapper->Connect(strAddress.c_str(), port);
}

bool XingApi::Login(String^ userId, String^ userPw, String^ certPw)
{
	ASSERT(m_pApiWrapper);

	CHAR* pszUserId = (CHAR*)Marshal::StringToHGlobalAnsi(userId).ToPointer();
	CHAR* pszUserPw = (CHAR*)Marshal::StringToHGlobalAnsi(userPw).ToPointer();
	CHAR* pszCertPw = (CHAR*)Marshal::StringToHGlobalAnsi(certPw).ToPointer();

	bool ret = m_pApiWrapper->Login(pszUserId, pszUserPw, pszCertPw);

	Marshal::FreeHGlobal(IntPtr(pszCertPw));
	Marshal::FreeHGlobal(IntPtr(pszUserPw));
	Marshal::FreeHGlobal(IntPtr(pszUserId));

	return ret;
}

void XingApi::SetQueryRowCount(int cnt)
{
	m_pApiWrapper->SetQueryRowCount(cnt);
}

bool XingApi::Request_T2101(String^ shcode)
{
	CHAR* pszShCode = (CHAR*)Marshal::StringToHGlobalAnsi(shcode).ToPointer();
	bool ret = m_pApiWrapper->Request_T2101(pszShCode);
	Marshal::FreeHGlobal(IntPtr(pszShCode));

	return ret;
}

bool XingApi::Request_ChartIndex(String^ shcode, String^ endDate, ChartMarketType marketType, ChartIndexType indexType, ChartPeriod period)
{
	CHAR* pszShCode = (CHAR*)Marshal::StringToHGlobalAnsi(shcode).ToPointer();
	CHAR* pszEndDate = (CHAR*)Marshal::StringToHGlobalAnsi(endDate).ToPointer();
	bool ret = m_pApiWrapper->Request_ChartIndex(pszShCode, pszEndDate, marketType, indexType, period);
	Marshal::FreeHGlobal(IntPtr(pszEndDate));
	Marshal::FreeHGlobal(IntPtr(pszShCode));

	return ret;
}

void XingApi::OnConnect(bool success, LPCTSTR pszMsg)
{
	m_pListener->Xing_Connect(success, gcnew String(pszMsg));
}

void XingApi::OnLoginSuccess()
{
	m_pListener->Xing_Login(true, nullptr, nullptr);
}

void XingApi::OnLoginFailed(LPCSTR pszCode, LPCTSTR pszMsg)
{
	String^ errorCode = gcnew String(pszCode);
	String^ errorMsg = gcnew String(pszMsg);

	m_pListener->Xing_Login(false, errorCode, errorMsg);
}

} // namespace XingSharp