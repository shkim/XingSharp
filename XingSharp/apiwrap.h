#pragma once

#include <vcclr.h>

using namespace System;

namespace XingSharp
{
	class ApiWrapper
	{
	public:
		ApiWrapper(XingApi^ owner);

		bool Create(LPCTSTR pszApiDir, OUT TCHAR* pszMsgOut);
		void Destroy();

		bool IsConnected();

		bool Connect(LPCSTR pszAddress, int nPort);
		bool Login(LPCSTR pszUserId, LPCSTR pszPasswd, LPCSTR pszCertPw);

		inline bool IsValid() const { return (m_hWnd != NULL); }

	private:
		void HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);

		void OnLoginResult(WPARAM wParam, LPARAM lParam);

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		static LPCTSTR szClassName;

		HWND m_hWnd;
		
		gcroot<XingApi^> m_pOwner;
	};
}