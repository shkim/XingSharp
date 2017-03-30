#pragma once

using namespace System;
using namespace System::Runtime::InteropServices;

namespace XingSharp
{
	class ApiWrapper;

	public interface class IXingApiListener
	{
		void Xing_Connect(bool success, String^ errorMsg);
		void Xing_Login(bool success, String^ errorCode, String^ errorMsg);
	};

	public ref class XingApi
	{
	public:
		XingApi(IXingApiListener^ pListener);
		
		bool Create(String^ apiDir, [Out]String^% errorMsgOut); // errorMsgOut is valid only if Create() failed
		void Destroy();

		bool IsConnected();

		bool Connect(String^ address, int port);
		bool Login(String^ userId, String^ userPw, String^ certPw);
		//bool Logout();

		void OnConnect(bool success, LPCTSTR pszMsg);
		void OnLoginSuccess();
		void OnLoginFailed(LPCSTR pszCode, LPCTSTR pszMsg);

	private:
		IXingApiListener^ m_pListener;
		ApiWrapper* m_pApiWrapper;
	};
}
