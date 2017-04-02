#include "Stdafx.h"
#include "XingSharp.h"
#include "apiwrap.h"
#include "util.h"
#include "XingSharp.h"
#include "IXingAPI.h"
#include "tr/t2101.h"
#include "tr/ChartIndex.h"

namespace XingSharp {

extern IXingAPI s_xingApi;

void ApiWrapper::SetQueryRowCount(int cnt)
{
	m_nChartQueryRows = cnt;

	if (m_nChartQueryRows > 500)
	{
		TRACE("Query row count should not exceed 500. (requested %d)\n", cnt);
		m_nChartQueryRows = 500;
	}
	else if (m_nChartQueryRows <= 0)
	{
		m_nChartQueryRows = 1;
	}
}

bool ApiWrapper::Request_T2101(const char* shcode)
{
	ASSERT(shcode != NULL);

	t2101InBlock inBlock;
	memset(&inBlock, ' ', sizeof(inBlock));
	CopyStringAndFillSpace(inBlock.focode, sizeof(inBlock.focode), shcode);

	int nReqId = s_xingApi.Request(m_hWnd, (LPCTSTR)"t2101",
		&inBlock, sizeof(inBlock));

	return (NULL != RegisterRequestInfo(nReqId, REQTYPE_T2101));
}

bool ApiWrapper::Request_ChartIndex(const char* shcode, const char* day8code, ChartMarketType marketType, ChartIndexType indexType, ChartPeriod period)
{
	ChartIndexInBlock inblock;
	memset(&inblock, ' ', sizeof(ChartIndexInBlock));

	switch (indexType)
	{
	case ChartIndexType::PriceMoveAverage:
		StringCchCopyA(inblock.indexname, 40, W2K(L"가격 이동평균"));
		break;
	case ChartIndexType::MACD:
		StringCchCopyA(inblock.indexname, 40, W2K(L"MACD"));
		break;
	case ChartIndexType::RSI:
		StringCchCopyA(inblock.indexname, 40, W2K(L"RSI"));
		break;

	default:
		TRACE("Invalid chart index type: %d\n", (int)indexType);
		return false;
	}

	switch (marketType)
	{
	case ChartMarketType::Stock:
		inblock.market[0] = '1';
		break;
	case ChartMarketType::Industry:
		inblock.market[0] = '2';
		break;
	case ChartMarketType::FutureOption:
		inblock.market[0] = '5';
		break;

	default:
		TRACE("Invalid chart market type: %d\n", (int)marketType);
		return false;
	}

	switch (period)
	{
	case ChartPeriod::Tick:
		inblock.period[0] = '0';
		break;
	case ChartPeriod::Minute:
		inblock.period[0] = '1';
		break;
	case ChartPeriod::Daily:
		inblock.period[0] = '2';
		break;
	case ChartPeriod::Weekly:
		inblock.period[0] = '3';
		break;
	case ChartPeriod::Monthly:
		inblock.period[0] = '4';
		break;

	default:
		TRACE("Invalid chart period type: %d\n", (int)period);
		return false;
	}

	if (lstrlenA(day8code) != 8)
	{
		TRACE("Invalid date code: '%s'\n", day8code);
		return false;
	}

	CopyStringAndFillSpace(inblock.shcode, sizeof(inblock.shcode), shcode);
	CopyStringAndFillSpace(inblock.edate, sizeof(inblock.edate), day8code); // 종료일자(일/주/월 period 해당)

	StringCchPrintfA(inblock.qrycnt, 4, "%d", m_nChartQueryRows); // 요청건수(최대 500개)
	StringCchPrintfA(inblock.ncnt, 4, "%d", 1);
	
	inblock.Isamend[0] = '1'; // 수정주가 반영
	inblock.Isgab[0] = '1'; // 갭 보정
	inblock.IsReal[0] = '0'; // 실시간 데이터 자동 등록 여부 (0:조회만, 1:실시간 자동 등록)

	int nReqId = s_xingApi.RequestService(m_hWnd, (LPCTSTR)NAME_ChartIndex, (LPCTSTR)&inblock);
	RequestInfo* pRI = RegisterRequestInfo(nReqId, REQTYPE_CHARTINDEX);
	if (pRI)
	{
		pRI->indexType = indexType;
		pRI->period = period;
		return true;
	}

	return false;
}

void ApiWrapper::OnLoginResult(WPARAM wParam, LPARAM lParam)
{
	LPCSTR pszCode = (LPCSTR)wParam;
	LPCSTR pszMsgA = (LPCSTR)lParam;

	if (!strcmp(pszCode, "0000"))
	{
		m_pOwner->OnLoginSuccess();
	}
	else
	{
		std::wstring msgW = KRtoWide(pszMsgA);
		m_pOwner->OnLoginFailed(pszCode, msgW.c_str());
	}
}

SignAgainstYesterday ParseSignAgainstYesterday(char ch)
{
	switch (ch)
	{
	case '1':
		return SignAgainstYesterday::UpperLimit;
	case '2':
		return SignAgainstYesterday::Up;
	case '3':
		return SignAgainstYesterday::NoChange;
	case '4':
		return SignAgainstYesterday::LowerLimit;
	case '5':
		return SignAgainstYesterday::Down;
	default:
		TRACE("Invalid SignAgainstYesterday: %c (%d)\n", ch, ch);
		return SignAgainstYesterday::InvalidValue;
	}
}

void ApiWrapper::Process_T2101(RequestInfo* pRI, _RECV_PACKET* pPacket)
{
	t2101OutBlock* pBlock = (t2101OutBlock*)pPacket->lpData;

	Xt2101^ ret = gcnew Xt2101();
	ret->HangulName = gcnew String(KRtoWide(pBlock->hname, sizeof(pBlock->hname)).c_str());
	ret->Sign = ParseSignAgainstYesterday(pBlock->sign[0]);

	//ret->LastMonth = gcnew String(KRtoWide(pBlock->lastmonth, sizeof(pBlock->lastmonth)).c_str());
	ret->LastMonth = gcnew String(pBlock->lastmonth, 0, sizeof(pBlock->lastmonth));
	ret->JanDateCnt = ParseInteger(pBlock->jandatecnt, sizeof(pBlock->jandatecnt));

	ret->Price = GetFloatString(pBlock->price, 6, 2);
	ret->Change = GetFloatString(pBlock->change, 6, 2);
	ret->JnilClose = GetFloatString(pBlock->jnilclose, 6, 2);
	ret->UpLmtPrice = GetFloatString(pBlock->uplmtprice, 6, 2);
	ret->DnLmtPrice = GetFloatString(pBlock->dnlmtprice, 6, 2);
	ret->RecPrice = GetFloatString(pBlock->recprice, 6, 2);
	ret->KospiJisu = GetFloatString(pBlock->kospijisu, 6, 2);

	m_pOwner->m_pListener->Xing_T2101(ret);
}

struct ChartIndexOutWrap
{
	ChartIndexOutBlock outBlock;
	char sCountOutBlock[5];	// 반복데이터 갯수
	ChartIndexOutBlock1	outBlock1[1];
};

void ApiWrapper::Process_ChartIndex(RequestInfo* pRI, _RECV_PACKET* pPacket)
{
	ChartIndexOutWrap* pWrap = (ChartIndexOutWrap*)pPacket->lpData;

	const int nColumnCount = std::stoi((KRtoAnsi(pWrap->outBlock.validdata_cnt, sizeof(pWrap->outBlock.validdata_cnt))));
	const int nMoreValueCount = nColumnCount - 7;

	bool hasError = false;

	if (KRtoWide(pWrap->outBlock1[0].date, sizeof(pWrap->outBlock1[0].date)).compare(L"일자")
	|| KRtoWide(pWrap->outBlock1[0].time, sizeof(pWrap->outBlock1[0].time)).compare(L"시간")
	|| KRtoWide(pWrap->outBlock1[0].open, sizeof(pWrap->outBlock1[0].open)).compare(L"시가")
	|| KRtoWide(pWrap->outBlock1[0].high, sizeof(pWrap->outBlock1[0].high)).compare(L"고가")
	|| KRtoWide(pWrap->outBlock1[0].low, sizeof(pWrap->outBlock1[0].low)).compare(L"저가")
	|| KRtoWide(pWrap->outBlock1[0].close, sizeof(pWrap->outBlock1[0].close)).compare(L"종가")
	|| KRtoWide(pWrap->outBlock1[0].volume, sizeof(pWrap->outBlock1[0].volume)).compare(L"거래량"))
	{
		TRACE(L"ChartIndex 기본 필드명 변경이 감지되었습니다.\n");
		hasError = true;
	}

	if (pRI->indexType == ChartIndexType::PriceMoveAverage)
	{
		// 가격 이동 평균
		if (nMoreValueCount != 5
		|| KRtoWide(pWrap->outBlock1[0].value1, sizeof(pWrap->outBlock1[0].value1)).compare(L"종가 단순")
		|| KRtoWide(pWrap->outBlock1[0].value2, sizeof(pWrap->outBlock1[0].value2)).compare(L"10")
		|| KRtoWide(pWrap->outBlock1[0].value3, sizeof(pWrap->outBlock1[0].value3)).compare(L"20")
		|| KRtoWide(pWrap->outBlock1[0].value4, sizeof(pWrap->outBlock1[0].value4)).compare(L"60")
		|| KRtoWide(pWrap->outBlock1[0].value5, sizeof(pWrap->outBlock1[0].value5)).compare(L"120"))
		{
			TRACE(L"ChartIndex 가격 이동평균 필드명 변경이 감지되었습니다.\n");
			hasError = true;
		}
	}
	else if (pRI->indexType == ChartIndexType::MACD)
	{
		// MACD
		if (nMoreValueCount != 3
		|| KRtoWide(pWrap->outBlock1[0].value1, sizeof(pWrap->outBlock1[0].value1)).compare(L"MACD Oscil")
		|| KRtoWide(pWrap->outBlock1[0].value2, sizeof(pWrap->outBlock1[0].value2)).compare(L"MACD 12,26")
		|| KRtoWide(pWrap->outBlock1[0].value3, sizeof(pWrap->outBlock1[0].value3)).compare(L"시그널 9"))
		{
			TRACE(L"ChartIndex MACD 필드명 변경이 감지되었습니다.\n");
			hasError = true;
		}
	}
	else if (pRI->indexType == ChartIndexType::RSI)
	{
		// RSI
		if (nMoreValueCount != 2
		|| KRtoWide(pWrap->outBlock1[0].value1, sizeof(pWrap->outBlock1[0].value1)).compare(L"RSI 14")
		|| KRtoWide(pWrap->outBlock1[0].value2, sizeof(pWrap->outBlock1[0].value2)).compare(L"시그널 9"))
		{
			TRACE(L"ChartIndex RSI 필드명 변경이 감지되었습니다.\n");
			hasError = true;
		}
	}
	else
	{
		TRACE(L"Unknown ChartIndex index type: %d\n", (int)pRI->indexType);
		return;
	}

	XChartIndex^ chart = gcnew XChartIndex();
	chart->Type = pRI->indexType;
	chart->HasError = hasError;

	chart->ValueNames = gcnew array<String^>(nMoreValueCount);
	chart->ValueNames[0] = gcnew String(KRtoWide(pWrap->outBlock1[0].value1, sizeof(pWrap->outBlock1[0].value1)).c_str());

	if (nMoreValueCount > 1)
		chart->ValueNames[1] = gcnew String(KRtoWide(pWrap->outBlock1[0].value2, sizeof(pWrap->outBlock1[0].value2)).c_str());

	if (nMoreValueCount > 2)
		chart->ValueNames[2] = gcnew String(KRtoWide(pWrap->outBlock1[0].value3, sizeof(pWrap->outBlock1[0].value3)).c_str());

	if (nMoreValueCount > 3)
		chart->ValueNames[3] = gcnew String(KRtoWide(pWrap->outBlock1[0].value4, sizeof(pWrap->outBlock1[0].value4)).c_str());

	if (nMoreValueCount > 4)
		chart->ValueNames[4] = gcnew String(KRtoWide(pWrap->outBlock1[0].value5, sizeof(pWrap->outBlock1[0].value5)).c_str());

	
	int nRecordCnt = std::stoi(KRtoWide(pWrap->sCountOutBlock, sizeof(pWrap->sCountOutBlock)));
	chart->Items = gcnew array<XChartIndexItem^>(nRecordCnt -1);
	for (int i=1; i<nRecordCnt; i++)
	{
		XChartIndexItem^ item = gcnew XChartIndexItem();
		chart->Items[i -1] = item;

		std::wstring pos = KRtoWide(pWrap->outBlock1[i].pos, sizeof(pWrap->outBlock1[i].pos));
		item->Day = gcnew String(KRtoWide(pWrap->outBlock1[i].date, sizeof(pWrap->outBlock1[i].date)).c_str());
		item->Time = gcnew String(KRtoWide(pWrap->outBlock1[i].time, sizeof(pWrap->outBlock1[i].time)).c_str());
		item->Open = gcnew String(KRtoWide(pWrap->outBlock1[i].open, sizeof(pWrap->outBlock1[i].open)).c_str());
		item->High = gcnew String(KRtoWide(pWrap->outBlock1[i].high, sizeof(pWrap->outBlock1[i].high)).c_str());
		item->Low = gcnew String(KRtoWide(pWrap->outBlock1[i].low, sizeof(pWrap->outBlock1[i].low)).c_str());
		item->Close = gcnew String(KRtoWide(pWrap->outBlock1[i].close, sizeof(pWrap->outBlock1[i].close)).c_str());
		item->Volume = gcnew String(KRtoWide(pWrap->outBlock1[i].volume, sizeof(pWrap->outBlock1[i].volume)).c_str());

		item->IndexValues = gcnew array<String^>(nMoreValueCount);
		item->IndexValues[0] = gcnew String(KRtoWide(pWrap->outBlock1[i].value1, sizeof(pWrap->outBlock1[i].value1)).c_str());

		if (nMoreValueCount > 1)
			item->IndexValues[1] = gcnew String(KRtoWide(pWrap->outBlock1[i].value2, sizeof(pWrap->outBlock1[i].value2)).c_str());

		if (nMoreValueCount > 2)
			item->IndexValues[2] = gcnew String(KRtoWide(pWrap->outBlock1[i].value3, sizeof(pWrap->outBlock1[i].value3)).c_str());

		if (nMoreValueCount > 3)
			item->IndexValues[3] = gcnew String(KRtoWide(pWrap->outBlock1[i].value4, sizeof(pWrap->outBlock1[i].value4)).c_str());

		if (nMoreValueCount > 4)
			item->IndexValues[4] = gcnew String(KRtoWide(pWrap->outBlock1[i].value5, sizeof(pWrap->outBlock1[i].value5)).c_str());

		/*TRACE(L"IndexValues: %s,%s,%s,%s,%s\n",
			KRtoWide(pWrap->outBlock1[i].value1, sizeof(pWrap->outBlock1[i].value1)).c_str(),
			KRtoWide(pWrap->outBlock1[i].value2, sizeof(pWrap->outBlock1[i].value2)).c_str(),
			KRtoWide(pWrap->outBlock1[i].value3, sizeof(pWrap->outBlock1[i].value3)).c_str(),
			KRtoWide(pWrap->outBlock1[i].value4, sizeof(pWrap->outBlock1[i].value4)).c_str(),
			KRtoWide(pWrap->outBlock1[i].value5, sizeof(pWrap->outBlock1[i].value5)).c_str());*/
	}

	m_pOwner->m_pListener->Xing_ChartIndex(chart);
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
			case REQTYPE_T2101:
				Process_T2101(pRI, pRpData);
				break;

			case REQTYPE_CHARTINDEX:
				Process_ChartIndex(pRI, pRpData);
				break;

			default:
				TRACE("Unknown RequestType %d (ReqID=%d)\n", pRI->nReqType, pRpData->nRqID);
			}
		}

		if (pRpData->cCont[0] == '1')
		{
			TRACE("Next data available (cCont==1), but not programmed.\n");
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

		s_xingApi.ReleaseMessageData(lParam);
	}
	else if (wParam == RELEASE_DATA)
	{
		TRACE(_T("Got RELEASE_DATA: %d"), (int)lParam);
		s_xingApi.ReleaseRequestData((int)lParam);

		auto itr = m_mapReqId2Info.find((int)lParam);
		if (itr == m_mapReqId2Info.end())
		{
			TRACE(_T("Request (ID=%d) not found in Info map\n"), lParam);
		}
		else
		{
			delete itr->second;
			m_mapReqId2Info.erase(itr);
		}
	}
	else
	{
		TRACE(_T("Unknown XM Recv wParam=%d\n"), wParam);
	}
}


} // namespace XingSharp