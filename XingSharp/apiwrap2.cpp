#include "Stdafx.h"
#include "XingSharp.h"
#include "apiwrap.h"
#include "util.h"
#include "XingSharp.h"
#include "IXingAPI.h"
#include "tr/FC0.h"
#include "tr/t1601.h"
#include "tr/t1633.h"
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

	int nReqId = s_xingApi.Request(m_hWnd, (LPCTSTR)NAME_t2101,
		&inBlock, sizeof(inBlock));

	return (NULL != RegisterRequestInfo(nReqId, REQTYPE_T2101));
}

bool ApiWrapper::Request_T1601(bool forMoney)
{
	t1601InBlock inBlock;
	memset(&inBlock, ' ', sizeof(inBlock));

	char ch = forMoney ? '2' : '1';	// 금액(2) : 수량(1)
	inBlock.gubun1[0] = ch;
	inBlock.gubun2[0] = ch;
	inBlock.gubun3[0] = '1';	// 1:백만원, 2:억원
	inBlock.gubun4[0] = ch;

	int nReqId = s_xingApi.Request(m_hWnd, (LPCTSTR)NAME_t1601,
		&inBlock, sizeof(inBlock));

	return (NULL != RegisterRequestInfo(nReqId, REQTYPE_T1601));
}

bool ApiWrapper::Request_T1633(bool forKospi, bool forMoney, const char* pszFromDate, const char* pszToDate)
{
	t1633InBlock inBlock;
	memset(&inBlock, ' ', sizeof(inBlock));

	inBlock.gubun[0] = forKospi ? '0' : '1';
	inBlock.gubun1[0] = forMoney ? '0' : '1';
	inBlock.gubun2[0] = '0'; // 0:수치, 1:누적
	inBlock.gubun3[0] = '1'; // 1:일, 2:주, 3:월
	inBlock.gubun4[0] = '0'; // 0:default, 1:직전대비증감

	CopyStringAndFillSpace(inBlock.fdate, sizeof(inBlock.fdate), pszFromDate);
	CopyStringAndFillSpace(inBlock.tdate, sizeof(inBlock.tdate), pszToDate);

	// TODO: 연속조회셋팅
	TRACE("t1633: %s~%s\n", pszFromDate, pszToDate);
	int nReqId = s_xingApi.Request(m_hWnd, (LPCTSTR)NAME_t1633,
		&inBlock, sizeof(inBlock));

	return (NULL != RegisterRequestInfo(nReqId, REQTYPE_T1633));
}

bool ApiWrapper::Request_ChartIndex(const char* shcode, const char* day8code, const char* indexName, XChartIndexParam^ otherParam)
{
	ChartIndexInBlock inblock;
	memset(&inblock, ' ', sizeof(ChartIndexInBlock));

	switch (otherParam->Market)
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
		TRACE("Invalid chart market type: %d\n", (int)otherParam->Market);
		return false;
	}

	switch (otherParam->Period)
	{
	case ChartPeriodType::Tick:
		inblock.period[0] = '0';
		break;
	case ChartPeriodType::Minute:
		inblock.period[0] = '1';
		break;
	case ChartPeriodType::Daily:
		inblock.period[0] = '2';
		break;
	case ChartPeriodType::Weekly:
		inblock.period[0] = '3';
		break;
	case ChartPeriodType::Monthly:
		inblock.period[0] = '4';
		break;

	default:
		TRACE("Invalid chart period type: %d\n", (int)otherParam->Period);
		return false;
	}

	if (lstrlenA(day8code) != 8)
	{
		TRACE("Invalid date code: '%s'\n", day8code);
		return false;
	}

	StringCbCopyA(inblock.indexname, sizeof(inblock.indexname), indexName);

	CopyStringAndFillSpace(inblock.shcode, sizeof(inblock.shcode), shcode);
	CopyStringAndFillSpace(inblock.edate, sizeof(inblock.edate), day8code); // 종료일자(일/주/월 period 해당)

	int queryCount = otherParam->QueryCount > 0 ? otherParam->QueryCount : m_nChartQueryRows;
	StringCchPrintfA(inblock.qrycnt, 4, "%d", queryCount); // 요청건수(최대 500개)

	int numForPeriod = otherParam->TicksOrMinutes > 0 ? otherParam->TicksOrMinutes : 1;
	StringCchPrintfA(inblock.ncnt, 4, "%d", numForPeriod);
	
	inblock.Isamend[0] = otherParam->AmendPrice ? '1':'0'; // 수정주가 반영
	inblock.Isgab[0] = otherParam->FixGap ? '1':'0'; // 갭 보정
	inblock.IsReal[0] = otherParam->RegisterReal ? '1':'0'; // 실시간 데이터 자동 등록 여부 (0:조회만, 1:실시간 자동 등록)

	int nReqId = s_xingApi.RequestService(m_hWnd, (LPCTSTR)NAME_ChartIndex, (LPCTSTR)&inblock);
	RequestInfo* pRI = RegisterRequestInfo(nReqId, REQTYPE_CHARTINDEX);
	if (pRI)
	{
		pRI->strUserKey = MStringToWString(otherParam->UserKey);
		pRI->nUserParam = otherParam->UserParam;
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

// KOSPI200 선물 체결
void ApiWrapper::Process_FC0(FC0_OutBlock* pBlock)
{
	XtFC0^ ret = gcnew XtFC0();

	ret->CheTime = GetString(pBlock->chetime, sizeof(pBlock->chetime));
	ret->Sign = ParseSignAgainstYesterday(pBlock->sign[0]);
	ret->Change = GetFloatString(pBlock->change, 6, 2);
	ret->DRate = GetFloatString(pBlock->drate, 6, 2);
	ret->Price = GetFloatString(pBlock->price, 6, 2);
	ret->Open = GetFloatString(pBlock->open, 6, 2);
	ret->High = GetFloatString(pBlock->high, 6, 2);
	ret->Low = GetFloatString(pBlock->low, 6, 2);
	ret->CGubun = GetString(pBlock->cgubun, sizeof(pBlock->cgubun));
	ret->CVolume = GetIntString(pBlock->cvolume, sizeof(pBlock->cvolume));
	ret->Volume = GetIntString(pBlock->volume, sizeof(pBlock->volume));
	ret->Value = GetIntString(pBlock->value, sizeof(pBlock->value));
	ret->MdVolume = GetIntString(pBlock->mdvolume, sizeof(pBlock->mdvolume));
	ret->MdCheCnt = GetIntString(pBlock->mdchecnt, sizeof(pBlock->mdchecnt));
	ret->MsVolume = GetIntString(pBlock->msvolume, sizeof(pBlock->msvolume));
	ret->MsCheCnt = GetIntString(pBlock->mschecnt, sizeof(pBlock->mschecnt));
	ret->CPower = GetFloatString(pBlock->cpower, 9, 2);
	ret->OfferHo1 = GetFloatString(pBlock->offerho1, 6, 2);
	ret->BidHo1 = GetFloatString(pBlock->bidho1, 6, 2);
	ret->OpenYak = GetIntString(pBlock->openyak, sizeof(pBlock->openyak));
	ret->K200Jisu = GetFloatString(pBlock->k200jisu, 6, 2);
	ret->TheoryPrice = GetFloatString(pBlock->theoryprice, 6, 2);
	ret->Kasis = GetFloatString(pBlock->kasis, 6, 2);
	ret->Sbasis = GetFloatString(pBlock->sbasis, 6, 2);
	ret->Ibasis = GetFloatString(pBlock->ibasis, 6, 2);
	ret->OpenYakCha = GetIntString(pBlock->openyakcha, sizeof(pBlock->openyakcha));
	ret->JGubun = GetString(pBlock->jgubun, sizeof(pBlock->jgubun));
	ret->JnilVolume = GetIntString(pBlock->jnilvolume, sizeof(pBlock->jnilvolume));
	ret->FutCode = GetString(pBlock->futcode, sizeof(pBlock->futcode));
	
	m_pOwner->m_pListener->Xing_FC0(ret);
}

static XTraders^ _InitTradersObj()
{
	XTraders^ ret = gcnew XTraders();
	ret->Individual = gcnew XTradeAmount();
	ret->Foreigner = gcnew XTradeAmount();
	ret->Institute = gcnew XTradeAmount();
	ret->StockFirm = gcnew XTradeAmount();
	ret->InvestTrust = gcnew XTradeAmount();
	ret->Bank = gcnew XTradeAmount();
	ret->Insurance = gcnew XTradeAmount();
	ret->InvestBank = gcnew XTradeAmount();
	ret->Fund = gcnew XTradeAmount();
	ret->Government = gcnew XTradeAmount();
	ret->PrivateEquity = gcnew XTradeAmount();
	ret->Etc = gcnew XTradeAmount();
	return ret;
}

#define _SetTradeItem(_Market,_Trader,_Num)\
	m_pT1601Pend->_Market->_Trader->Buy = GetIntString(pOutBlock->ms_##_Num, sizeof(pOutBlock->ms_##_Num));\
	m_pT1601Pend->_Market->_Trader->Sell = GetIntString(pOutBlock->md_##_Num, sizeof(pOutBlock->md_##_Num));\
	m_pT1601Pend->_Market->_Trader->Rate = GetIntString(pOutBlock->rate_##_Num, sizeof(pOutBlock->rate_##_Num));\
	m_pT1601Pend->_Market->_Trader->NetBuy = GetIntString(pOutBlock->svolume_##_Num, sizeof(pOutBlock->svolume_##_Num));


// 투자자별 종합
void ApiWrapper::Process_T1601(RequestInfo* pRI, _RECV_PACKET* pPacket)
{
	TRACE("ProcessT1601: BlockName=%s\n", pPacket->szBlockName);

	if (System::Object::ReferenceEquals(m_pT1601Pend, nullptr))
		m_pT1601Pend = gcnew Xt1601();

	if (strcmp(pPacket->szBlockName, NAME_t1601OutBlock1) == 0)
	{
		t1601OutBlock1* pOutBlock = (t1601OutBlock1*)pPacket->lpData;

		/*m_pT1601Pend->Kospi = InitTradersObj();
		m_pT1601Pend->Kospi->Individual->Buy = GetIntString(pOutBlock->ms_08, sizeof(pOutBlock->ms_08));
		m_pT1601Pend->Kospi->Individual->Sell = GetIntString(pOutBlock->md_08, sizeof(pOutBlock->md_08));
		m_pT1601Pend->Kospi->Individual->Rate = GetIntString(pOutBlock->rate_08, sizeof(pOutBlock->rate_08));
		m_pT1601Pend->Kospi->Individual->NetBuy = GetIntString(pOutBlock->svolume_08, sizeof(pOutBlock->svolume_08));*/

		m_pT1601Pend->Kospi = _InitTradersObj();
		_SetTradeItem(Kospi, Individual, 08);
		_SetTradeItem(Kospi, Foreigner, 17);
		_SetTradeItem(Kospi, Institute, 18);
		_SetTradeItem(Kospi, StockFirm, 01);
		_SetTradeItem(Kospi, InvestTrust, 03);
		_SetTradeItem(Kospi, Bank, 04);
		_SetTradeItem(Kospi, Insurance, 02);
		_SetTradeItem(Kospi, InvestBank, 05);
		_SetTradeItem(Kospi, Fund, 06);
		_SetTradeItem(Kospi, Government, 11);
		_SetTradeItem(Kospi, Etc, 07);
		_SetTradeItem(Kospi, PrivateEquity, 00);
	}
	else if (strcmp(pPacket->szBlockName, NAME_t1601OutBlock2) == 0)
	{
		t1601OutBlock2* pOutBlock = (t1601OutBlock2*)pPacket->lpData;

		m_pT1601Pend->Kosdaq = _InitTradersObj();
		_SetTradeItem(Kosdaq, Individual, 08);
		_SetTradeItem(Kosdaq, Foreigner, 17);
		_SetTradeItem(Kosdaq, Institute, 18);
		_SetTradeItem(Kosdaq, StockFirm, 01);
		_SetTradeItem(Kosdaq, InvestTrust, 03);
		_SetTradeItem(Kosdaq, Bank, 04);
		_SetTradeItem(Kosdaq, Insurance, 02);
		_SetTradeItem(Kosdaq, InvestBank, 05);
		_SetTradeItem(Kosdaq, Fund, 06);
		_SetTradeItem(Kosdaq, Government, 11);
		_SetTradeItem(Kosdaq, Etc, 07);
		_SetTradeItem(Kosdaq, PrivateEquity, 00);
	}
	else if (strcmp(pPacket->szBlockName, NAME_t1601OutBlock3) == 0)
	{
		t1601OutBlock3* pOutBlock = (t1601OutBlock3*)pPacket->lpData;

		m_pT1601Pend->Future = _InitTradersObj();
		_SetTradeItem(Future, Individual, 08);
		_SetTradeItem(Future, Foreigner, 17);
		_SetTradeItem(Future, Institute, 18);
		_SetTradeItem(Future, StockFirm, 01);
		_SetTradeItem(Future, InvestTrust, 03);
		_SetTradeItem(Future, Bank, 04);
		_SetTradeItem(Future, Insurance, 02);
		_SetTradeItem(Future, InvestBank, 05);
		_SetTradeItem(Future, Fund, 06);
		_SetTradeItem(Future, Government, 11);
		_SetTradeItem(Future, Etc, 07);
		_SetTradeItem(Future, PrivateEquity, 00);
	}
	else if (strcmp(pPacket->szBlockName, NAME_t1601OutBlock4) == 0)
	{
		t1601OutBlock4* pOutBlock = (t1601OutBlock4*)pPacket->lpData;

		m_pT1601Pend->CallOpt = _InitTradersObj();
		_SetTradeItem(CallOpt, Individual, 08);
		_SetTradeItem(CallOpt, Foreigner, 17);
		_SetTradeItem(CallOpt, Institute, 18);
		_SetTradeItem(CallOpt, StockFirm, 01);
		_SetTradeItem(CallOpt, InvestTrust, 03);
		_SetTradeItem(CallOpt, Bank, 04);
		_SetTradeItem(CallOpt, Insurance, 02);
		_SetTradeItem(CallOpt, InvestBank, 05);
		_SetTradeItem(CallOpt, Fund, 06);
		_SetTradeItem(CallOpt, Government, 11);
		_SetTradeItem(CallOpt, Etc, 07);
		_SetTradeItem(CallOpt, PrivateEquity, 00);
	}
	else if (strcmp(pPacket->szBlockName, NAME_t1601OutBlock5) == 0)
	{
		t1601OutBlock5* pOutBlock = (t1601OutBlock5*)pPacket->lpData;

		m_pT1601Pend->PutOpt = _InitTradersObj();
		_SetTradeItem(PutOpt, Individual, 08);
		_SetTradeItem(PutOpt, Foreigner, 17);
		_SetTradeItem(PutOpt, Institute, 18);
		_SetTradeItem(PutOpt, StockFirm, 01);
		_SetTradeItem(PutOpt, InvestTrust, 03);
		_SetTradeItem(PutOpt, Bank, 04);
		_SetTradeItem(PutOpt, Insurance, 02);
		_SetTradeItem(PutOpt, InvestBank, 05);
		_SetTradeItem(PutOpt, Fund, 06);
		_SetTradeItem(PutOpt, Government, 11);
		_SetTradeItem(PutOpt, Etc, 07);
		_SetTradeItem(PutOpt, PrivateEquity, 00);
	}
}

void ApiWrapper::Finalize_T1601(RequestInfo* pRI)
{
	ASSERT(!System::Object::ReferenceEquals(m_pT1601Pend, nullptr));
	m_pOwner->m_pListener->Xing_T1601(m_pT1601Pend);
	m_pT1601Pend = nullptr;
}


void ApiWrapper::Process_T1633(RequestInfo* pRI, _RECV_PACKET* pPacket)
{
	if (strcmp(pPacket->szBlockName, NAME_t1633OutBlock) == 0)
	{
		t1633OutBlock* pBlock = (t1633OutBlock*)pPacket->lpData;

		ASSERT(System::Object::ReferenceEquals(m_pT1633Pend, nullptr));
		m_pT1633Pend = gcnew Xt1633();
		m_pT1633Pend->ContinueDate = gcnew String(pBlock->date, 0, sizeof(pBlock->date));

		int itemCnt = ParseInteger(pBlock->idx, sizeof(pBlock->idx));
		m_pT1633Pend->Items = gcnew array<Xt1633Item^>(itemCnt);

		TRACE("Process_T1633 date=%s, idx=%d\n", pBlock->date, itemCnt);
	}
	else if (strcmp(pPacket->szBlockName, NAME_t1633OutBlock1) == 0)
	{
		t1633OutBlock1* pBlock = (t1633OutBlock1*)pPacket->lpData;

		int nCount = pPacket->nDataLength / sizeof(t1633OutBlock1);
		TRACE("1633 count=%d, pendCnt=%d\n", nCount, m_pT1633Pend->Items->Length);
		ASSERT(nCount == m_pT1633Pend->Items->Length);

		for (int i = 0; i < nCount; i++)
		{
			Xt1633Item^ item = gcnew Xt1633Item();
			m_pT1633Pend->Items[i] = item;

			item->Date = GetString(pBlock[i].date, sizeof(pBlock->date));
			item->Jisu = GetString(pBlock[i].jisu, sizeof(pBlock->jisu));
			item->Sign = GetString(pBlock[i].sign, sizeof(pBlock->sign));
			item->Change = GetString(pBlock[i].change, sizeof(pBlock->change));

			item->TotalNetBuy = GetIntString(pBlock[i].tot3, sizeof(pBlock->tot3));
			item->TotalBuy = GetIntString(pBlock[i].tot1, sizeof(pBlock->tot1));
			item->TotalSell = GetIntString(pBlock[i].tot2, sizeof(pBlock->tot2));
			item->ArbitrageNetBuy = GetIntString(pBlock[i].cha3, sizeof(pBlock->cha3));
			item->ArbitrageBuy = GetIntString(pBlock[i].cha1, sizeof(pBlock->cha1));
			item->ArbitrageSell = GetIntString(pBlock[i].cha2, sizeof(pBlock->cha2));
			item->NoArbitrageNetBuy = GetIntString(pBlock[i].bcha3, sizeof(pBlock->bcha3));
			item->NoArbitrageBuy = GetIntString(pBlock[i].bcha1, sizeof(pBlock->bcha1));
			item->NorbitrageSell = GetIntString(pBlock[i].bcha2, sizeof(pBlock->bcha2));
			item->Volume = GetIntString(pBlock[i].volume, sizeof(pBlock->volume));
		}
	}
	else
	{
		TRACE("Unknown block name: %s\n", pPacket->szBlockName);
	}
}

void ApiWrapper::Finalize_T133(RequestInfo* pRI)
{
	ASSERT(!System::Object::ReferenceEquals(m_pT1633Pend, nullptr));
	m_pOwner->m_pListener->Xing_T1633(m_pT1633Pend);
	m_pT1633Pend = nullptr;
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
		m_pOwner->OnErrorMessage(L"ChartIndex 기본 필드명 변경이 감지되었습니다.");
		hasError = true;
	}
/*
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
			m_pOwner->OnErrorMessage(L"ChartIndex 가격 이동평균 필드명 변경이 감지되었습니다.");
			hasError = true;
		}
	}
	else if (pRI->indexType == ChartIndexType::Disparity)
	{
		// 이격도
		if (nMoreValueCount != 4
		|| KRtoWide(pWrap->outBlock1[0].value1, sizeof(pWrap->outBlock1[0].value1)).compare(0, 5, L"이격도 종")	// "이격도 종?"
		|| KRtoWide(pWrap->outBlock1[0].value2, sizeof(pWrap->outBlock1[0].value2)).compare(L"10")
		|| KRtoWide(pWrap->outBlock1[0].value3, sizeof(pWrap->outBlock1[0].value3)).compare(L"20")
		|| KRtoWide(pWrap->outBlock1[0].value4, sizeof(pWrap->outBlock1[0].value4)).compare(L"60"))
		{
			m_pOwner->OnErrorMessage(L"ChartIndex 이격도 필드명 변경이 감지되었습니다.");
			hasError = true;
		}
	}
	else if (pRI->indexType == ChartIndexType::AverageTrueRange)
	{
		// ATR
		if (nMoreValueCount != 1
		|| KRtoWide(pWrap->outBlock1[0].value1, sizeof(pWrap->outBlock1[0].value1)).compare(L"ATR 14"))
		{
			m_pOwner->OnErrorMessage(L"ChartIndex Average True Range 필드명 변경이 감지되었습니다.");
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
			m_pOwner->OnErrorMessage(L"ChartIndex MACD 필드명 변경이 감지되었습니다.");
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
			m_pOwner->OnErrorMessage(L"ChartIndex RSI 필드명 변경이 감지되었습니다.");
			hasError = true;
		}
	}
	else if (pRI->indexType == ChartIndexType::OBV)
	{
		// OBV
		if (nMoreValueCount != 2
		|| KRtoWide(pWrap->outBlock1[0].value1, sizeof(pWrap->outBlock1[0].value1)).compare(L"OBV종가")
		|| KRtoWide(pWrap->outBlock1[0].value2, sizeof(pWrap->outBlock1[0].value2)).compare(L"시그널 9"))
		{
			m_pOwner->OnErrorMessage(L"ChartIndex OBV 필드명 변경이 감지되었습니다.");
			hasError = true;
		}
	}
	else if (pRI->indexType == ChartIndexType::Momentum)
	{
		// Momentum
		if (nMoreValueCount != 2
		|| KRtoWide(pWrap->outBlock1[0].value1, sizeof(pWrap->outBlock1[0].value1)).compare(0, 9, L"Momentum ")
		|| KRtoWide(pWrap->outBlock1[0].value2, sizeof(pWrap->outBlock1[0].value2)).compare(L"시그널 9"))
		{
			m_pOwner->OnErrorMessage(L"ChartIndex Momentum 필드명 변경이 감지되었습니다.");
			hasError = true;
		}
	}
	else if (pRI->indexType == ChartIndexType::SonarMomentum)
	{
		// Sonar Momentum
		if (nMoreValueCount != 2
		|| KRtoWide(pWrap->outBlock1[0].value1, sizeof(pWrap->outBlock1[0].value1)).compare(L"Sonar 종가")
		|| KRtoWide(pWrap->outBlock1[0].value2, sizeof(pWrap->outBlock1[0].value2)).compare(0, 5, L"시그널 단"))
		{
			m_pOwner->OnErrorMessage(L"ChartIndex Sonar Momentum 필드명 변경이 감지되었습니다.");
			hasError = true;
		}
	}
	else if (pRI->indexType == ChartIndexType::PriceROC)
	{
		// Price ROC
		if (nMoreValueCount != 2
		|| KRtoWide(pWrap->outBlock1[0].value1, sizeof(pWrap->outBlock1[0].value1)).compare(L"ROC 종가 1")
		|| KRtoWide(pWrap->outBlock1[0].value2, sizeof(pWrap->outBlock1[0].value2)).compare(L"시그널 9"))
		{
			m_pOwner->OnErrorMessage(L"ChartIndex Price ROC 필드명 변경이 감지되었습니다.");
			hasError = true;
		}
	}
	else if (pRI->indexType == ChartIndexType::VolumeROC)
	{
		// Volume ROC
		if (nMoreValueCount != 2
		|| KRtoWide(pWrap->outBlock1[0].value1, sizeof(pWrap->outBlock1[0].value1)).compare(L"VolumeROC")
		|| KRtoWide(pWrap->outBlock1[0].value2, sizeof(pWrap->outBlock1[0].value2)).compare(L"시그널 9"))
		{
			m_pOwner->OnErrorMessage(L"ChartIndex Volume ROC 필드명 변경이 감지되었습니다.");
			hasError = true;
		}
	}
	else
	{
		WCHAR temp[1024];
		StringCbPrintfW(temp, sizeof(temp), L"Unknown ChartIndex index type: %d", (int)pRI->indexType);
		m_pOwner->OnErrorMessage(temp);
		return;
	}
*/
	XChartIndex^ chart = gcnew XChartIndex();
	chart->UserKey = WStringToMString(pRI->strUserKey);
	chart->UserParam = pRI->nUserParam;
	chart->HasError = hasError;
#ifdef _DEBUG
	if (chart->HasError)
		TRACE("!hasError!");
#endif

	chart->ValueNames = gcnew array<String^>(nMoreValueCount);

	if (nMoreValueCount > 0)
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

		if (nMoreValueCount > 0)
			item->IndexValues[0] = gcnew String(KRtoWide(pWrap->outBlock1[i].value1, sizeof(pWrap->outBlock1[i].value1)).c_str());

		if (nMoreValueCount > 1)
			item->IndexValues[1] = gcnew String(KRtoWide(pWrap->outBlock1[i].value2, sizeof(pWrap->outBlock1[i].value2)).c_str());

		if (nMoreValueCount > 2)
			item->IndexValues[2] = gcnew String(KRtoWide(pWrap->outBlock1[i].value3, sizeof(pWrap->outBlock1[i].value3)).c_str());

		if (nMoreValueCount > 3)
			item->IndexValues[3] = gcnew String(KRtoWide(pWrap->outBlock1[i].value4, sizeof(pWrap->outBlock1[i].value4)).c_str());

		if (nMoreValueCount > 4)
			item->IndexValues[4] = gcnew String(KRtoWide(pWrap->outBlock1[i].value5, sizeof(pWrap->outBlock1[i].value5)).c_str());

		/*TRACE(L"IndexValues: %s* %s* %s* %s* %s\n",
			KRtoWide(pWrap->outBlock1[i].value1, sizeof(pWrap->outBlock1[i].value1)).c_str(),
			KRtoWide(pWrap->outBlock1[i].value2, sizeof(pWrap->outBlock1[i].value2)).c_str(),
			KRtoWide(pWrap->outBlock1[i].value3, sizeof(pWrap->outBlock1[i].value3)).c_str(),
			KRtoWide(pWrap->outBlock1[i].value4, sizeof(pWrap->outBlock1[i].value4)).c_str(),
			KRtoWide(pWrap->outBlock1[i].value5, sizeof(pWrap->outBlock1[i].value5)).c_str());*/
	}

	m_pOwner->m_pListener->Xing_ChartIndex(chart);
}

void ApiWrapper::OnReceiveRealDataChart(WPARAM wParam, LPARAM lParam)
{
	UINT nIndexID = (UINT)wParam;
	LPRECV_REAL_PACKET pRcvData = (LPRECV_REAL_PACKET)lParam;
	ChartIndexOutBlock1 *pBlock = (ChartIndexOutBlock1 *)(pRcvData->pszData);

	auto pos = KRtoWide(pBlock->pos, sizeof(pBlock->pos));
	auto date = KRtoWide(pBlock->date, sizeof(pBlock->date));
	auto time = KRtoWide(pBlock->time, sizeof(pBlock->time));
	auto open = KRtoWide(pBlock->open, sizeof(pBlock->open));
	auto high = KRtoWide(pBlock->high, sizeof(pBlock->high));
	auto low = KRtoWide(pBlock->low, sizeof(pBlock->low));
	auto close = KRtoWide(pBlock->close, sizeof(pBlock->close));
	auto volume = KRtoWide(pBlock->volume, sizeof(pBlock->volume));
	auto value1 = KRtoWide(pBlock->value1, sizeof(pBlock->value1));
	auto value2 = KRtoWide(pBlock->value2, sizeof(pBlock->value2));
	auto value3 = KRtoWide(pBlock->value3, sizeof(pBlock->value3));
	auto value4 = KRtoWide(pBlock->value4, sizeof(pBlock->value4));
	auto value5 = KRtoWide(pBlock->value5, sizeof(pBlock->value5));

	//int nPos = _ttoi(pos.c_str());
	TRACE(_T("RDC: pos=%s, date=%s, time=%s, OHLCV=%s,%s,%s,%s,%s\n"), pos.c_str(), date.c_str(), time.c_str(), open.c_str(), high.c_str(), low.c_str(), close.c_str(), volume.c_str());
	TRACE(_T("\t\tvalues=%s,%s,%s,%s,%s\n"), value1.c_str(), value2.c_str(), value3.c_str(), value4.c_str(), value5.c_str());
}

void ApiWrapper::OnTimeoutData(WPARAM wParam, LPARAM lParam)
{
	TRACE("TODO: OnTimeoutData\n");
}

} // namespace XingSharp