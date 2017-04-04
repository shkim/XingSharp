#pragma once

using namespace System;
using namespace System::Runtime::InteropServices;

namespace XingSharp
{
	class ApiWrapper;

	public enum class ChartPeriod : char
	{
		Tick = 0,		// 틱
		Minute = 1,		// 분
		Daily = 2,		// 일
		Weekly = 3,		// 주
		Monthly = 4,	// 월
	};

	public enum class ChartMarketType : char
	{
		Stock = 1,			// 주식
		Industry = 2,		// 업종
		FutureOption = 5,	// 선물옵션
	};

	public enum class ChartIndexType : char
	{
		PriceMoveAverage,		// 가격이동평균
		EstrangementRatio,		// 이격도
		AverageTrueRange,
		MACD,
		RSI,
		OBV,
		Momentum,
		SonarMomentum,
		PriceROC,
		VolumeROC
	};

	public enum class SignAgainstYesterday : char
	{
		UpperLimit,
		Up,
		NoChange,
		Down,
		LowerLimit,

		InvalidValue
	};

	public ref class Xt2101
	{
	public:
		String^ HangulName;	// 한글명
		SignAgainstYesterday Sign; // 전일대비구분

		String^ LastMonth;	// 만기일
		int JanDateCnt;	// 잔여일

		String^ Price;	// 현재가
		String^ Change;	// 전일대비
		String^ JnilClose;	// 전일종가
		String^ UpLmtPrice;	// 상한가
		String^ DnLmtPrice;	// 하한가
		String^ RecPrice;	// 기준가
		String^ KospiJisu;	// KOSPI200지수
		// TODO: more fields
	};

	public ref class XChartIndexItem
	{
	public:
		String^ Day;		// 일자
		String^ Time;		// 시간
		String^ Open;		// 시가
		String^ High;		// 고가
		String^ Low;		// 저가
		String^ Close;		// 종가
		String^ Volume;		// 거래량

		// Value의 종류는 ValueNames 참고
		array<String^>^ IndexValues;
	};

	public ref class XChartIndex
	{
	public:
		ChartIndexType Type;
		array<String^>^ ValueNames;
		array<XChartIndexItem^>^ Items;
		bool HasError;	// 증권사 API 변경시 필드명,컬럼수가 달라질 수 있다.
	};

	public interface class IXingApiListener
	{
		void Xing_Connect(bool success, String^ errorMsg);
		void Xing_Login(bool success, String^ errorCode, String^ errorMsg);

		void Xing_T2101(Xt2101^ t2101);
		void Xing_ChartIndex(XChartIndex^ chart);
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

		void SetQueryRowCount(int cnt);
		bool Request_T2101(String^ shcode);	// 현재가 조회
		bool Request_ChartIndex(String^ shcode, String^ endDate, ChartMarketType marketType, ChartIndexType indexType, ChartPeriod period);

		void OnConnect(bool success, LPCTSTR pszMsg);
		void OnLoginSuccess();
		void OnLoginFailed(LPCSTR pszCode, LPCTSTR pszMsg);

		IXingApiListener^ m_pListener;

	private:
		ApiWrapper* m_pApiWrapper;
	};
}
