using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Reflection;
using System.Resources;
using System.Collections;

namespace Reaction_Editor
{
    public partial class FrmLog : Form, ILog
    {
        public FrmLog()
        {
            InitializeComponent();
            //rm.BaseName
            listView1.SmallImageList = Program.Images;
        }

        #region ILog Members
        protected bool m_bActive = true;
        protected Stack<MessageSource> sourceStack = new Stack<MessageSource>();
        protected void AddMessage(string msg, MessageType msgType, MessageSource src)
        {
            if (!m_bActive) return;
            string imageKey = "";
            switch (msgType)
            {
                case MessageType.Error:
                    imageKey = "Error";
                    break;
                case MessageType.Note:
                    imageKey = "Info";
                    break;
                case MessageType.Warning:
                    imageKey = "Warning";
                    break;
            }
            string source = "Global";
            if (src != null)
                source = src.Source;
            ListViewItem lvi = new ListViewItem(new string[] { source, msg }, imageKey);
            lvi.Tag = src;
            listView1.Items.Add(lvi);
            listView1.EnsureVisible(lvi.Index);
            //listView1.AutoScrollOffset = new Point(0);//(lvi.Position);
        }

        public void Message(string msg, MessageType msgType, MessageSource src)
        {
            AddMessage(msg, msgType, src);
        }

        public void Message(string msg, MessageType msgType)
        {
            MessageSource src = null;
            if (sourceStack.Count != 0)
                src = sourceStack.Peek();
            AddMessage(msg, msgType, src);
        }

        public void SetSource(MessageSource src)
        {
            sourceStack.Push(src);
        }

        public void RemoveSource()
        {
            sourceStack.Pop();
        }

        public bool Active
        {
            get { return m_bActive; }
            set { m_bActive = value; }
        }
        #endregion

        private void FrmLog_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (e.CloseReason == CloseReason.UserClosing)
            {
                e.Cancel = true;
                this.WindowState = FormWindowState.Minimized;
            }
        }

        private void listView1_DoubleClick(object sender, EventArgs e)
        {
            if (listView1.SelectedItems.Count != 0 &&
                listView1.SelectedItems[0].Tag != null)
            {
                ((MessageSource)listView1.SelectedItems[0].Tag).DoCallback();
            }
        }
    }
}