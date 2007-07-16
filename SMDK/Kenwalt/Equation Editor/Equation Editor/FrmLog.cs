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
            Assembly asm = Assembly.GetExecutingAssembly();
            ResourceReader rr = new ResourceReader(asm.GetManifestResourceStream(typeof(FrmMain), "Icons.resources"));
            //rm.BaseName
            listView1.SmallImageList = new ImageList();
            foreach (DictionaryEntry de in rr)
            {
                Console.WriteLine("Key: " + de.Key);
                Console.WriteLine("Value " + de.Value);
                listView1.SmallImageList.Images.Add((String)de.Key, (Icon) de.Value);
            }
        }

        #region ILog Members

        protected Stack<MessageSource> sourceStack = new Stack<MessageSource>();
        protected void AddMessage(string msg, MessageType msgType, MessageSource src)
        {
            string imageKey = "";
            switch (msgType)
            {
                case MessageType.Error:
                    imageKey = "Error_ico";
                    break;
                case MessageType.Note:
                    imageKey = "Info_ico";
                    break;
                case MessageType.Warning:
                    imageKey = "Warning_ico";
                    break;
            }
            string source = "Global";
            if (src != null)
                source = src.Source;
            ListViewItem lvi = new ListViewItem(new string[] { source, msg }, imageKey);
            lvi.Tag = src;
            listView1.Items.Add(lvi);
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