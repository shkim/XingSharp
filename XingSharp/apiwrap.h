#pragma once

#include <vcclr.h>

using namespace System;

struct _RECV_PACKET;

namespace XingSharp
{
	enum
	{
		REQTYPE_T2101 = 1,
		REQTYPE_CHARTINDEX,
		REQTYPE_TODO
	};

	struct RequestInfo
	{
		int nReqType;
		int nDataReceived;

		ChartIndexType indexType;
		ChartPeriod period;
	};

	class ApiWrapper
	{
	public:
		ApiWrapper(XingApi^ owner);

		bool Create(LPCTSTR pszApiDir, OUT TCHAR* pszMsgOut);
		void Destroy();

		bool IsConnected();

		bool Connect(LPCSTR pszAddress, int nPort);
		bool Login(LPCSTR pszUserId, LPCSTR pszPasswd, LPCSTR pszCertPw);

		void SetQueryRowCount(int cnt);
		bool Request_T2101(const char* shcode);
		bool Request_ChartIndex(const char* shcode, const char* day8code, ChartMarketType marketType, ChartIndexType indexType, ChartPeriod period);

		inline bool IsValid() const { return (m_hWnd != NULL); }

	private:
		void HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);
		
		RequestInfo* RegisterRequestInfo(int nReqId, int nReqType);
		void OnLoginResult(WPARAM wParam, LPARAM lParam);
		void OnReceiveData(WPARAM wParam, LPARAM lParam);
		void Process_T2101(RequestInfo* pRI, _RECV_PACKET* pPacket);
		void Process_ChartIndex(RequestInfo* pRI, _RECV_PACKET* pPacket);

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		static LPCTSTR szClassName;

		HWND m_hWnd;
		int m_nChartQueryRows;
		
		gcroot<XingApi^> m_pOwner;

		std::map<int, RequestInfo*> m_mapReqId2Info;
	};
}