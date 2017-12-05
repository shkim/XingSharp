#pragma once

using namespace System;
using namespace System::Runtime::InteropServices;

namespace XingSharp
{
	class ApiWrapper;

	public enum class ChartPeriodType : char
	{
		Tick = 0,		// 틱
		Minute = 1,		// 분
		Daily = 2,		// 일
		Weekly = 3,		// 주
		Monthly = 4,	// 월
		Yearly = 5,		// 년
	};

	public enum class ChartMarketType : char
	{
		Stock = 1,			// 주식
		Industry = 2,		// 업종
		FutureOption = 5,	// 선물옵션
	};
/*
	public enum class ChartIndexType : char
	{
		PriceMoveAverage,		// 가격이동평균
		Disparity,		// 이격도
		AverageTrueRange,
		MACD,
		RSI,
		OBV,
		Momentum,
		SonarMomentum,
		PriceROC,
		VolumeROC
	};
*/
	public ref class ChartIndexName
	{
	public:
		literal String^ PriceMoveAverage = "가격 이동평균";
		literal String^ Disparity = "이격도";
		literal String^ AverageTrueRange = "Average True Range";
		literal String^ MACD = "MACD";
		literal String^ RSI = "RSI";
		literal String^ OBV = "OBV";
		literal String^ Momentum = "Momentum";
		literal String^ SonarMomentum = "Sonar Momentum";
		literal String^ PriceROC = "Price ROC";
		literal String^ VolumeROC = "Volume ROC";
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

	public ref class XTradeAmount
	{
	public:
		String^ Sell;		// 매도
		String^ Buy;		// 매수
		String^ NetBuy;		// 순매수
		String^ Rate;		// 증감
	};

	public ref class XTraders
	{
	public:
		XTradeAmount^ Individual;	// 개인
		XTradeAmount^ Foreigner;	// 외국인
		XTradeAmount^ Institute;	// 기관계
		XTradeAmount^ StockFirm;	// 증권
		XTradeAmount^ InvestTrust;	// 투신
		XTradeAmount^ Bank;			// 은행
		XTradeAmount^ Insurance;	// 보험
		XTradeAmount^ InvestBank;	// 종금
		XTradeAmount^ Fund;			// 기금
		XTradeAmount^ Government;	// 국가
		XTradeAmount^ PrivateEquity;// 사모펀드
		XTradeAmount^ Etc;			// 기타
	};

	public ref class Xt1601
	{
	public:
		XTraders^ Kospi;
		XTraders^ Kosdaq;
		XTraders^ Future;
		XTraders^ CallOpt;
		XTraders^ PutOpt;
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

	public ref class Xt1633Item
	{
	public:
		String^ Date;
		String^ Jisu;				// KOSPI200 지수
		String^ Sign;				// 대비구분 (1:상한, 2:상승, 	3:보합, 4:하한, 5:하락)
		String^ Change;				// 대비
		String^ TotalNetBuy;		// 전체 순매수
		String^ TotalBuy;			// 전체 매수
		String^ TotalSell;			// 전체 매도
		String^ ArbitrageNetBuy;	// 차익 순매수
		String^ ArbitrageBuy;		// 차익 매수
		String^ ArbitrageSell;		// 차익 매도
		String^ NoArbitrageNetBuy;	// 비차익 순매수
		String^ NoArbitrageBuy;		// 비차익 매수
		String^ NorbitrageSell;		// 비차익 매도
		String^ Volume;				// 거래량
	};

	public ref class Xt1633
	{
	public:
		String^ ContinueDate;	// 연속조회용 날짜 (현재 사용 안함)
		array<Xt1633Item^>^ Items;
	};

	// Output of ChartIndex
	public ref class XChartIndex
	{
	public:
		int UserKey;
		int UserParam;

		array<String^>^ ValueNames;	// names of Items.IndexValues
		array<XChartIndexItem^>^ Items;
		bool HasError;	// 증권사 API 변경시 필드명,컬럼수가 달라질 수 있다.
	};

	// Input for ChartIndex
	public ref class XChartIndexParam
	{
	public:
		ChartMarketType Market;
		ChartPeriodType Period;
		String^ IndexName;
		String^ ShCode;
		String^ EndDate;	// ex: 20161231
		int QueryCount;		// max 500 (0 for default)
		int TicksOrMinutes; // N-ticks, or N-minutes (0 for default) 
		bool FixGap;
		bool AmendPrice;
		bool RegisterReal;

		// two user variables for any purpose
		int UserKey;
		int UserParam;
	};

	public interface class IXingApiListener
	{
		void Xing_Connect(bool success, String^ errorMsg);
		void Xing_Login(bool success, String^ errorCode, String^ errorMsg);
		void Xing_ErrorMessage(String^ errorMsg);

		void Xing_T2101(Xt2101^ t2101);
		void Xing_T1601(Xt1601^ t1601);
		void Xing_T1633(Xt1633^ t1633);		
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

		bool Advise_FC0(String^ futcode);
		bool Unadvise_FC0(String^ futcode);

		bool Advise_JIF();
		bool Unadvise_JIF();

		void SetQueryRowCount(int cnt);
		bool Request_T2101(String^ shcode);	// 현재가 조회
		bool Request_T1601(bool forMoney);	// 투자자별 종합 (true=금액, false=수량)
		bool Request_T1633(bool forMoney);	// 기간별 프로그램 매매추이 (거래소 금일까지 한달치 일별만 조회)
		bool Request_ChartIndex(XChartIndexParam^ param);

		void OnConnect(bool success, LPCTSTR pszMsg);
		void OnLoginSuccess();
		void OnLoginFailed(LPCSTR pszCode, LPCTSTR pszMsg);
		void OnErrorMessage(LPCWSTR pszMsg);

		IXingApiListener^ m_pListener;

	private:
		ApiWrapper* m_pApiWrapper;
	};
}
