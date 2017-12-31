#include "Stdafx.h"
#include "XingSharp.h"
#include "apiwrap.h"

#include "util.h"
#include "IXingAPI.h"
#include "tr/FC0.h"

#pragma comment(lib, "user32.lib")

#define START_WM_ID		WM_USER

namespace XingSharp {

IXingAPI s_xingApi;

LPCTSTR ApiWrapper::szClassName = _T("XingCBW");
static HINSTANCE g_hInstance;

ApiWrapper::ApiWrapper(XingApi^ pOwner)
{
	m_hWnd = NULL;
	m_pOwner = pOwner;
	m_nChartQueryRows = 100;
}

void ApiWrapper::Destroy()
{
	if (m_hWnd != NULL)
	{
		if (s_xingApi.IsConnected())
		{
			s_xingApi.Disconnect();
		}
		
		s_xingApi.Uninit();

		DestroyWindow(m_hWnd);
		m_hWnd = NULL;

		::UnregisterClass(szClassName, g_hInstance);
	}
}

bool ApiWrapper::Create(LPCTSTR pszApiDir, LPTSTR pszMsgOut)
{
	g_hInstance = GetModuleHandle(NULL);

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = g_hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szClassName;
	wcex.hIconSm = wcex.hIcon = NULL;

	if (!::RegisterClassEx(&wcex))
	{
		StringCchCopy(pszMsgOut, MAX_PATH, _T("RegisterClassEx(Xing) failed"));
		return false;
	}

	m_hWnd = CreateWindow(szClassName, _T("Xing Callback"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, g_hInstance, (void*)this);

	if (!m_hWnd)
	{
		StringCchCopy(pszMsgOut, MAX_PATH, _T("CreateWindow(Xing) failed"));
		return false;
	}

	TRACE(_T("XingApiDir: %s\n"), pszApiDir);
	BOOL bInitRet = s_xingApi.Init(pszApiDir);

	if (!bInitRet)
	{
		StringCchCopy(pszMsgOut, MAX_PATH, _T("XingApi.Init failed"));
		return false;
	}

	return true;
}

bool ApiWrapper::IsConnected()
{
	return (s_xingApi.IsConnected() != FALSE);
}

bool ApiWrapper::Connect(LPCSTR pszAddress, int nPort)
{
	if (s_xingApi.IsConnected())
	{
		m_pOwner->OnConnect(false, _T("XingApi: already connected"));
		return false;
	}

	// NOTE: casting from LPCSTR to LPCTSTR is workaround for XingApi bug
	if (!s_xingApi.Connect(m_hWnd, (LPCTSTR)pszAddress, nPort, START_WM_ID))
	{
		m_pOwner->OnConnect(false, _T("XingApi.Connect failed"));
		return false;
	}

	TRACE("XingApi.Connect succeeded\n");
	m_pOwner->OnConnect(true, NULL);
	return true;
}

bool ApiWrapper::Login(LPCSTR pszUserId, LPCSTR pszUserPw, LPCSTR pszCertPw)
{
	if (!s_xingApi.IsConnected())
	{
		m_pOwner->OnLoginFailed("X", _T("XingApi.Login: not connected"));
		return false;
	}

	if (!s_xingApi.Login(m_hWnd, pszUserId, pszUserPw, pszCertPw, 0, TRUE))
	{
		m_pOwner->OnLoginFailed("X", _T("XingApi.Login failed"));
		return false;
	}

	return true;
}

bool ApiWrapper::AdviseRealData(const char* trcode, void* pData, int nDataLen)
{
	BOOL ret = s_xingApi.AdviseRealData(m_hWnd, (LPCTSTR)trcode,
		(LPCTSTR)pData, nDataLen);

	TRACE("AdviseRealData(%s,%s): len=%d, ret=%d\n", trcode, (char*)pData, nDataLen, ret);
	return (ret != FALSE);
}

bool ApiWrapper::UnadviseRealData(const char* trcode, void* pData, int nDataLen)
{
	BOOL ret = s_xingApi.UnadviseRealData(m_hWnd, (LPCTSTR)trcode,
		(LPCTSTR)pData, nDataLen);

	return (ret != FALSE);
}

RequestInfo* ApiWrapper::RegisterRequestInfo(int nReqId, int nReqType)
{
	if (nReqId < 0)
	{
		TRACE("Request (Type=%d) failed (ID is -1)", nReqType);
		return NULL;
	}

	if (m_mapReqId2Info.find(nReqId) != m_mapReqId2Info.end())
	{
		TRACE("FATAL: ReqID %d duplicated.", nReqId);
	}

	RequestInfo* pRI = new RequestInfo();
	memset(pRI, 0, sizeof(RequestInfo));
	pRI->nReqType = nReqType;

	m_mapReqId2Info[nReqId] = pRI;
	TRACE("Request %d registered with type %d\n", nReqId, nReqType);
	return pRI;
}

void ApiWrapper::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	if (wParam == REQUEST_DATA)
	{
		RECV_PACKET* pRpData = (RECV_PACKET*)lParam;
		auto itr = m_mapReqId2Info.find(pRpData->nRqID);
		if (itr == m_mapReqId2Info.end())
		{
			TRACE("Request (ID=%d) not found in Info map\n", pRpData->nRqID);
		}
		else
		{
			RequestInfo* pRI = itr->second;
			switch (pRI->nReqType)
			{
			case REQTYPE_CHARTINDEX:
				Process_ChartIndex(pRI, pRpData);
				break;

			case REQTYPE_T1601:
				Process_T1601(pRI, pRpData);
				break;

			case REQTYPE_T1633:
				Process_T1633(pRI, pRpData);
				break;

			case REQTYPE_T2101:
				Process_T2101(pRI, pRpData);
				break;

			case REQTYPE_T9945:
				Process_T9945(pRI, pRpData);
				break;

			default:
				TRACE("Unknown RequestType %d (ReqID=%d)\n", pRI->nReqType, pRpData->nRqID);
			}

			if (pRpData->cCont[0] == '1')
			{
				TRACE("ReqType#%d Next data available (cCont==1), but not programmed.\n", pRI->nReqType);
			}
		}
	}
	else if (wParam == MESSAGE_DATA)
	{
		MSG_PACKET* pMsg = (MSG_PACKET*)lParam;

		std::wstring msg = KRtoWide((char*)pMsg->lpszMessageData, pMsg->nMsgLength);
		TRACE(_T("MESSAGE_DATA: %s\n"), msg.c_str());

		s_xingApi.ReleaseMessageData(lParam);
	}
	else if (wParam == SYSTEM_ERROR_DATA)
	{
		MSG_PACKET* pMsg = (MSG_PACKET*)lParam;

		std::wstring msg = KRtoWide((char*)pMsg->lpszMessageData, pMsg->nMsgLength);
		TRACE(_T("SYSTEM_ERROR_DATA: %s\n"), msg.c_str());
		m_pOwner->OnErrorMessage(msg.c_str());

		s_xingApi.ReleaseMessageData(lParam);
	}
	else if (wParam == RELEASE_DATA)
	{
		TRACE(_T("Got RELEASE_DATA: %d\n"), (int)lParam);
		s_xingApi.ReleaseRequestData((int)lParam);

		auto itr = m_mapReqId2Info.find((int)lParam);
		if (itr == m_mapReqId2Info.end())
		{
			TRACE(_T("Request (ID=%d) not found in Info map\n"), lParam);
		}
		else
		{
			RequestInfo* pRI = itr->second;
			switch (pRI->nReqType)
			{
			case REQTYPE_T1601:
				Finalize_T1601(pRI);
				break;
			case REQTYPE_T1633:
				Finalize_T133(pRI);
				break;
			}

			delete itr->second;
			m_mapReqId2Info.erase(itr);
		}
	}
	else
	{
		TRACE(_T("Unknown XM Recv wParam=%d\n"), wParam);
	}
}

void ApiWrapper::OnReceiveRealData(WPARAM wParam, LPARAM lParam)
{
	RECV_REAL_PACKET* pRealPacket = (RECV_REAL_PACKET*)lParam;

	if (strcmp(pRealPacket->szTrCode, NAME_FC0) == 0)
	{
		Process_FC0((FC0_OutBlock*)pRealPacket->pszData);
	}
	else
	{
		TRACE("TODO: XM_RECEIVE_REAL_DATA: %s\n", pRealPacket->szTrCode);
	}
}

void ApiWrapper::HandleMessage(UINT xmMessage, WPARAM wParam, LPARAM lParam)
{
	//TRACE("HandleMsg(%d)\n", xmMessage);
	switch (xmMessage)
	{
	case XM_DISCONNECT:
		TRACE("TODO: XM_DISCONNECT\n");
		break;

	case XM_LOGIN:
		OnLoginResult(wParam, lParam);
		break;

	case XM_LOGOUT:
		TRACE(_T("TODO: XM_LOGOUT\n"));
		//m_pListener->API_OnLogout();
		break;

	case XM_RECEIVE_DATA:
		OnReceiveData(wParam, lParam);
		break;

	case XM_RECEIVE_REAL_DATA:
		OnReceiveRealData(wParam, lParam);
		break;

	case XM_RECEIVE_REAL_DATA_CHART:
		OnReceiveRealDataChart(wParam, lParam);
		break;

	case XM_TIMEOUT_DATA:
		OnTimeoutData(wParam, lParam);
		break;

	default:
		TRACE(_T("Unhandled XM: %d\n"), xmMessage);
	}
}

LRESULT CALLBACK ApiWrapper::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static ApiWrapper* pWnd;

	if (START_WM_ID <= message && message <= (START_WM_ID + XM_RECEIVE_REAL_DATA_CHART))
	{
		pWnd->HandleMessage(message - START_WM_ID, wParam, lParam);
		return 0;
	}

	switch (message)
	{	
	case WM_CREATE:
		pWnd = (ApiWrapper*)((CREATESTRUCT*)lParam)->lpCreateParams;
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

} // namespace XingSharp