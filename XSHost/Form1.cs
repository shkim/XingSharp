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
                //m_xingApi.Request_T2101("101M6000");
                m_xingApi.SetQueryRowCount(30);
                m_xingApi.Request_ChartIndex("101M6000", "20170404", ChartMarketType.FutureOption, ChartIndexType.PriceMoveAverage, ChartPeriod.Daily);
            }
            else
            {
                LogPrint("로그인 실패 [{0}] {1}", errorCode, errorMsg);
            }
        }

        public void Xing_T2101(Xt2101 data)
        {
            LogPrint("REQUEST_DATA: {0}, 현재가={1}, 전일대비={2}, 만기일={3}, 남은날={4}", data.HangulName, data.Price, data.Sign, data.LastMonth, data.JanDateCnt);
            LogPrint("  전일종가={0}, 상한가={1}, 하한가={2}, 기준가={3}, KOSPI200지수={4}", data.JnilClose, data.UpLmtPrice, data.DnLmtPrice, data.RecPrice, data.KospiJisu);

        }

        public void Xing_ChartIndex(XChartIndex chart)
        {
            LogPrint("ChartIndex({0}): Rows={1}, MoreColumns={2}: {3}", chart.Type, chart.Items.Length, chart.ValueNames.Length, String.Join(", ", chart.ValueNames));
            foreach (XChartIndexItem item in chart.Items)
            {
                LogPrint("{0},{1}: O={2},H={3},L={4},C={5},V={6} | {7}", item.Day, item.Time, item.Open, item.High, item.Low, item.Close, item.Volume, String.Join(", ", item.IndexValues));
            }

        }
    }
}
