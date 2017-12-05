using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using XingSharp;

namespace XSHost
{
    public partial class Form1 : Form, IXingApiListener
    {
        private XingApi m_xingApi;

        public Form1()
        {
            InitializeComponent();

            m_xingApi = new XingApi(this);
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            LogPrint("XingApi 초기화를 시도합니다.");

            string errorMsg;
            if (m_xingApi.Create(@"C:\eBEST\XingAPI", out errorMsg))
            {
                LogPrint("XingApi 생성에 성공하였습니다.");
                m_xingApi.Connect(null, 0);
            }
            else
            {
                LogPrint("XingApi 생성 실패: {0}", errorMsg);
            }
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            m_xingApi.Destroy();
        }

        public void LogPrint(String fmt, params object[] args)
        {
            string msg = String.Format(fmt, args);

            Invoke(new MethodInvoker(() =>
            {
                tbLogs.SelectionStart = tbLogs.TextLength;
                tbLogs.SelectionLength = 0;
                tbLogs.AppendText(msg + Environment.NewLine);
                tbLogs.ScrollToCaret();
            }));
        }

        public void Xing_Connect(bool success, String errorMsg)
        {
            if (success)
            {
                String userId, userPw, certPw;
                try
                {
                    // connected, read credentials from file
                    string text = System.IO.File.ReadAllText(@"xingidpw.txt");
                    string[] tok = text.Split('\n');
                    userId = tok[0].Trim();
                    userPw = tok[1].Trim();
                    certPw = tok[2].Trim();
                }
                catch (Exception)
                {
                    userId = "userid";
                    userPw = "passwd";
                    certPw = "certpw";
                }

                LogPrint("로그인을 시도합니다.");
                Debug.WriteLine("UserID={0},UserPW={1},CertPw={2}", userId, userPw, certPw);
                m_xingApi.Login(userId, userPw, certPw);
            }
            else
            {
                LogPrint("서버 연결에 실패하였습니다.");
            }
        }

        public void Xing_Login(bool success, String errorCode, string errorMsg)
        {
            if (success)
            {
                LogPrint("로그인에 성공하였습니다.");

                //Req_ChartIndex();            
                //m_xingApi.Unadvise_FC0("101M6000");                
                //m_xingApi.Advise_JIF();
                m_xingApi.Request_T1633(true);
            }
            else
            {
                LogPrint("로그인 실패 [{0}] {1}", errorCode, errorMsg);
            }
        }

        public void Xing_ErrorMessage(String errorMsg)
        {
            LogPrint("에러메세지: {0}", errorMsg);
        }

        public void Xing_T1601(Xt1601 data)
        {
            LogPrint("선물: 개인:{0},{1}, 외국인:{2},{3}", data.Kospi.Individual.Buy, data.Kospi.Individual.Sell, data.Kospi.Foreigner.Buy, data.Kospi.Foreigner.Sell);
        }

        public void Xing_T1633(Xt1633 data)
        {
            foreach (Xt1633Item item in data.Items)
            {
                LogPrint("{0}: 전체:{1}, 차익:{2}, 비차익:{3}, 지수:{4}, 거래량:{5}, Sign:{6}, 대비:{7}", item.Date, item.TotalNetBuy, item.ArbitrageNetBuy, item.NoArbitrageNetBuy, item.Jisu, item.Volume, item.Sign, item.Change);
            }
        }

        public void Xing_T2101(Xt2101 data)
        {
            LogPrint("REQUEST_DATA: {0}, 현재가={1}, 전일대비={2}, 만기일={3}, 남은날={4}", data.HangulName, data.Price, data.Sign, data.LastMonth, data.JanDateCnt);
            LogPrint("  전일종가={0}, 상한가={1}, 하한가={2}, 기준가={3}, KOSPI200지수={4}", data.JnilClose, data.UpLmtPrice, data.DnLmtPrice, data.RecPrice, data.KospiJisu);

        }

        public void Xing_ChartIndex(XChartIndex chart)
        {
            LogPrint("ChartIndex({0},{1}): NumRows={2}, MoreColumns={3}: {4}", chart.UserKey, chart.UserParam, chart.Items.Length, chart.ValueNames.Length, String.Join(", ", chart.ValueNames));
            foreach (XChartIndexItem item in chart.Items)
            {
                LogPrint("{0},{1}: O={2},H={3},L={4},C={5},V={6} | {7}", item.Day, item.Time, item.Open, item.High, item.Low, item.Close, item.Volume, String.Join(", ", item.IndexValues));
            }

        }

        private void Req_ChartIndex()
        {
            XChartIndexParam param = new XChartIndexParam();
            param.Market = ChartMarketType.FutureOption;
            param.Period = ChartPeriodType.Monthly;
            param.TicksOrMinutes = 6;
            param.IndexName = ChartIndexName.PriceMoveAverage;
            param.ShCode = "101M6000";
            param.EndDate = "20170417";
            param.QueryCount = 30;
            param.FixGap = true;
            param.AmendPrice = true;
            param.RegisterReal = true;
            param.UserKey = 111;
            param.UserParam = 999;

            m_xingApi.Request_ChartIndex(param);
        }

        private void menuRealFC0_Click(object sender, EventArgs e)
        {
            m_xingApi.Advise_FC0("101M6000");
        }

        private void menuT1601_Click(object sender, EventArgs e)
        {
            m_xingApi.Request_T1601(true);
        }

        private void menuT2101_Click(object sender, EventArgs e)
        {
            m_xingApi.Request_T2101("101M6000");
        }
    }
}
