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
            this.menuRealFC0 = new System.Windows.Forms.ToolStripMenuItem();
            this.menuT1601 = new System.Windows.Forms.ToolStripMenuItem();
            this.menuT2101 = new System.Windows.Forms.ToolStripMenuItem();
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
            this.tbLogs.Size = new System.Drawing.Size(770, 290);
            this.tbLogs.TabIndex = 0;
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.xingToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(770, 24);
            this.menuStrip1.TabIndex = 1;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // xingToolStripMenuItem
            // 
            this.xingToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuRealFC0,
            this.menuT1601,
            this.menuT2101});
            this.xingToolStripMenuItem.Name = "xingToolStripMenuItem";
            this.xingToolStripMenuItem.Size = new System.Drawing.Size(43, 20);
            this.xingToolStripMenuItem.Text = "&Xing";
            // 
            // menuRealFC0
            // 
            this.menuRealFC0.Name = "menuRealFC0";
            this.menuRealFC0.Size = new System.Drawing.Size(187, 22);
            this.menuRealFC0.Text = "실시간 FC0 선물체결";
            this.menuRealFC0.Click += new System.EventHandler(this.menuRealFC0_Click);
            // 
            // menuT1601
            // 
            this.menuT1601.Name = "menuT1601";
            this.menuT1601.Size = new System.Drawing.Size(187, 22);
            this.menuT1601.Text = "T1601 투자자";
            this.menuT1601.Click += new System.EventHandler(this.menuT1601_Click);
            // 
            // menuT2101
            // 
            this.menuT2101.Name = "menuT2101";
            this.menuT2101.Size = new System.Drawing.Size(187, 22);
            this.menuT2101.Text = "T2101 선옵 현재가";
            this.menuT2101.Click += new System.EventHandler(this.menuT2101_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(770, 314);
            this.Controls.Add(this.tbLogs);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form1";
            this.Text = "XingSharp Test";
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
        private System.Windows.Forms.ToolStripMenuItem menuRealFC0;
        private System.Windows.Forms.ToolStripMenuItem menuT1601;
        private System.Windows.Forms.ToolStripMenuItem menuT2101;
    }
}

