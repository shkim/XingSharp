#include "Stdafx.h"
#include "XingSharp.h"
#include "apiwrap.h"

#include "util.h"
#include "IXingAPI.h"

#pragma comment(lib, "user32.lib")

#define START_WM_ID		WM_USER

namespace XingSharp {

static IXingAPI s_xingApi;

LPCTSTR ApiWrapper::szClassName = _T("XingCBW");
static HINSTANCE g_hInstance;

ApiWrapper::ApiWrapper(XingApi^ pOwner)
{
	m_hWnd = NULL;
	m_pOwner = pOwner;
}

void ApiWrapper::Destroy()
{
	if (m_hWnd != NULL)
	{
		if (s_xingApi.IsConnected())
		{
			s_xingApi.Disconnect();
		}
		
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

void ApiWrapper::HandleMessage(UINT xmMessage, WPARAM wParam, LPARAM lParam)
{
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
		//OnReceiveData(wParam, lParam);
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