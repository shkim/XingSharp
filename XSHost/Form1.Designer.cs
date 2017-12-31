namespace XSHost
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.tbLogs = new System.Windows.Forms.TextBox();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.xingToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aPIToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.차트ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripSeparator();
            this.menuRealFC0 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripSeparator();
            this.menuT1601 = new System.Windows.Forms.ToolStripMenuItem();
            this.t1633프로그램매매추이ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.금액ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.거래량ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menuT2101 = new System.Windows.Forms.ToolStripMenuItem();
            this.menuT9945 = new System.Windows.Forms.ToolStripMenuItem();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tbLogs
            // 
            this.tbLogs.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbLogs.Location = new System.Drawing.Point(0, 24);
            this.tbLogs.Multiline = true;
            this.tbLogs.Name = "tbLogs";
            this.tbLogs.ReadOnly = true;
            this.tbLogs.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbLogs.Size = new System.Drawing.Size(660, 316);
            this.tbLogs.TabIndex = 0;
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.xingToolStripMenuItem,
            this.aPIToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Padding = new System.Windows.Forms.Padding(5, 2, 0, 2);
            this.menuStrip1.Size = new System.Drawing.Size(660, 24);
            this.menuStrip1.TabIndex = 1;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // xingToolStripMenuItem
            // 
            this.xingToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem1,
            this.exitToolStripMenuItem});
            this.xingToolStripMenuItem.Name = "xingToolStripMenuItem";
            this.xingToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.xingToolStripMenuItem.Text = "&File";
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(89, 6);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(92, 22);
            this.exitToolStripMenuItem.Text = "E&xit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.menuExit_Click);
            // 
            // aPIToolStripMenuItem
            // 
            this.aPIToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.차트ToolStripMenuItem,
            this.toolStripMenuItem2,
            this.menuRealFC0,
            this.toolStripMenuItem3,
            this.menuT1601,
            this.t1633프로그램매매추이ToolStripMenuItem,
            this.menuT2101,
            this.menuT9945});
            this.aPIToolStripMenuItem.Name = "aPIToolStripMenuItem";
            this.aPIToolStripMenuItem.Size = new System.Drawing.Size(60, 20);
            this.aPIToolStripMenuItem.Text = "API Call";
            // 
            // 차트ToolStripMenuItem
            // 
            this.차트ToolStripMenuItem.Name = "차트ToolStripMenuItem";
            this.차트ToolStripMenuItem.Size = new System.Drawing.Size(210, 22);
            this.차트ToolStripMenuItem.Text = "Chart";
            this.차트ToolStripMenuItem.Click += new System.EventHandler(this.menuChart_Click);
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.Name = "toolStripMenuItem2";
            this.toolStripMenuItem2.Size = new System.Drawing.Size(207, 6);
            // 
            // menuRealFC0
            // 
            this.menuRealFC0.Name = "menuRealFC0";
            this.menuRealFC0.Size = new System.Drawing.Size(210, 22);
            this.menuRealFC0.Text = "FC0 실시간 선물체결";
            this.menuRealFC0.Click += new System.EventHandler(this.menuRealFC0_Click);
            // 
            // toolStripMenuItem3
            // 
            this.toolStripMenuItem3.Name = "toolStripMenuItem3";
            this.toolStripMenuItem3.Size = new System.Drawing.Size(207, 6);
            // 
            // menuT1601
            // 
            this.menuT1601.Name = "menuT1601";
            this.menuT1601.Size = new System.Drawing.Size(210, 22);
            this.menuT1601.Text = "T1601 투자자";
            this.menuT1601.Click += new System.EventHandler(this.menuT1601_Click);
            // 
            // t1633프로그램매매추이ToolStripMenuItem
            // 
            this.t1633프로그램매매추이ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.금액ToolStripMenuItem,
            this.거래량ToolStripMenuItem});
            this.t1633프로그램매매추이ToolStripMenuItem.Name = "t1633프로그램매매추이ToolStripMenuItem";
            this.t1633프로그램매매추이ToolStripMenuItem.Size = new System.Drawing.Size(210, 22);
            this.t1633프로그램매매추이ToolStripMenuItem.Text = "T1633 프로그램 매매 추이";
            // 
            // 금액ToolStripMenuItem
            // 
            this.금액ToolStripMenuItem.Name = "금액ToolStripMenuItem";
            this.금액ToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.금액ToolStripMenuItem.Text = "금액";
            this.금액ToolStripMenuItem.Click += new System.EventHandler(this.menuT1633Money_Click);
            // 
            // 거래량ToolStripMenuItem
            // 
            this.거래량ToolStripMenuItem.Name = "거래량ToolStripMenuItem";
            this.거래량ToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.거래량ToolStripMenuItem.Text = "수량";
            this.거래량ToolStripMenuItem.Click += new System.EventHandler(this.menuT1633Volume_Click);
            // 
            // menuT2101
            // 
            this.menuT2101.Name = "menuT2101";
            this.menuT2101.Size = new System.Drawing.Size(210, 22);
            this.menuT2101.Text = "T2101 선옵 현재가";
            this.menuT2101.Click += new System.EventHandler(this.menuT2101_Click);
            // 
            // menuT9945
            // 
            this.menuT9945.Name = "menuT9945";
            this.menuT9945.Size = new System.Drawing.Size(210, 22);
            this.menuT9945.Text = "T9945 종목마스터 조회";
            this.menuT9945.Click += new System.EventHandler(this.menuT9945_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(660, 340);
            this.Controls.Add(this.tbLogs);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form1";
            this.Text = "XingSharp API Test";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_FormClosed);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox tbLogs;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem xingToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aPIToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 차트ToolStripMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripMenuItem1;
		private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem menuT1601;
		private System.Windows.Forms.ToolStripMenuItem menuRealFC0;
		private System.Windows.Forms.ToolStripMenuItem menuT2101;
		private System.Windows.Forms.ToolStripSeparator toolStripMenuItem2;
		private System.Windows.Forms.ToolStripSeparator toolStripMenuItem3;
		private System.Windows.Forms.ToolStripMenuItem t1633프로그램매매추이ToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem 금액ToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem 거래량ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem menuT9945;
    }
}

