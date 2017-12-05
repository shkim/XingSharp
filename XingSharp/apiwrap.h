#pragma once

#include <vcclr.h>

using namespace System;

struct _RECV_PACKET;
struct _FC0_OutBlock;

namespace XingSharp
{
	enum
	{
		REQTYPE_CHARTINDEX =1,
		REQTYPE_T1601,
		REQTYPE_T1633,
		REQTYPE_T2101,
		_REQTYPE_TODO
	};

	struct RequestInfo
	{
		int nReqType;
		int nDataReceived;

		int nUserKey;
		int nUserParam;
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

		bool AdviseRealData(const char* trcode, void* pData, int nDataLen);
		bool UnadviseRealData(const char* trcode, void* pData, int nDataLen);

		void SetQueryRowCount(int cnt);
		bool Request_T2101(const char* shcode);
		bool Request_T1601(bool forMoney);
		bool Request_T1633(bool forKospi, bool forMoney, const char* fromDate, const char* toDate);
		bool Request_ChartIndex(const char* shcode, const char* day8code, const char* indexName, XChartIndexParam^ otherParam);

		inline bool IsValid() const { return (m_hWnd != NULL); }

	private:
		void HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);
		
		RequestInfo* RegisterRequestInfo(int nReqId, int nReqType);
		void OnLoginResult(WPARAM wParam, LPARAM lParam);
		void OnReceiveData(WPARAM wParam, LPARAM lParam);
		void OnReceiveRealData(WPARAM wParam, LPARAM lParam);
		void OnReceiveRealDataChart(WPARAM wParam, LPARAM lParam);
		void OnTimeoutData(WPARAM wParam, LPARAM lParam);

		void Process_FC0(_FC0_OutBlock* pOutBlock);
		
		void Process_T1601(RequestInfo* pRI, _RECV_PACKET* pPacket);
		void Finalize_T1601(RequestInfo* pRI);
		
		void Process_T1633(RequestInfo* pRI, _RECV_PACKET* pPacket);
		void Finalize_T133(RequestInfo* pRI); 
		
		void Process_T2101(RequestInfo* pRI, _RECV_PACKET* pPacket);
		void Process_ChartIndex(RequestInfo* pRI, _RECV_PACKET* pPacket);

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		static LPCTSTR szClassName;

		HWND m_hWnd;
		int m_nChartQueryRows;
		
		gcroot<XingApi^> m_pOwner;
		gcroot<Xt1601^> m_pT1601Pend;
		gcroot<Xt1633^> m_pT1633Pend;

		std::map<int, RequestInfo*> m_mapReqId2Info;
	};
}