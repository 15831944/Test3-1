using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Text.RegularExpressions;
using Reaction_Editor;

namespace Auto_Complete
{
    internal partial class FrmAutoComplete : Form
    {
        public static Regex s_AutoCompleteRegex = new Regex(@"(\s|\+|^|,)\d*(\.\d+)?(?<Last>[A-Za-z0-9()\[\]]+?(?<Back>[\b]?)$)", RegexOptions.ExplicitCapture | RegexOptions.Compiled);
        public static Regex s_AutoCompleteOpeningRegex = new Regex(@"(\s|\+|^|,)\d*(\.\d+)?(?<Last>[A-Za-z0-9()\[\]]$)", RegexOptions.ExplicitCapture | RegexOptions.Compiled);

        protected int m_nLastSelect;
        protected BoxAutoComplete m_OwnerBox;
        protected bool m_bFocused = false;
        protected int m_nUseIndex = 0;

        internal FrmAutoComplete(BoxAutoComplete ownerbox)
        {
            InitializeComponent();
            OwnerBox = ownerbox;
            MainControl.SelectedIndexChanged += new EventHandler(MainControl_SelectedIndexChanged);
            MainControl.DoubleClick += new EventHandler(MainControl_DoubleClick);
        }

        void MainControl_DoubleClick(object sender, EventArgs e)
        {
            InsertText();
        }

        void MainControl_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (MainControl.SelectedIndex != -1)
                LastSelect = MainControl.SelectedIndex;
        }

        public BoxAutoComplete OwnerBox
        {
            set
            {
                m_OwnerBox = value;
                MainControl.m_OwnerBox = value;
            }
            get { return m_OwnerBox; }
        }

        public int LastSelect 
        {
            set 
            {
                m_nLastSelect = value;
            }
            get { return m_nLastSelect; }
        }

        protected Form m_ReselectForm;
        public Form ReselectForm 
        {
            set { m_ReselectForm = value; }
            get { return m_ReselectForm; }
        }

        public FrmAutoComplete()
        {
            InitializeComponent();
        }

        protected override void OnShown(EventArgs e)
        {
            //MainControl.SelectedIndex = -1;
            base.OnShown(e);
        }

        public void SetFilter(string filter)
        {
            MainControl.SelectedIndex = MainControl.FindString(filter);
        }

        public bool HotSelected
        {
            get { return MainControl.SelectedItem != null; }
        }

        public object SelectedValue
        {
            get 
            {
                if (MainControl.SelectedItem != null)
                    return MainControl.SelectedItem;
                else if (LastSelect >= 0)
                    return MainControl.Items[LastSelect];
                else return null;
            }
        }

        internal void HandleKey(KeyEventArgs e)
        {
            Messaging.SendMessage(MainControl.Handle, Messaging.WM_KEYDOWN, (IntPtr)e.KeyValue, (IntPtr)0);
        }

        public void InsertText()
        {
            if (SelectedValue != null)
            {
                Match m = s_AutoCompleteRegex.Match(m_OwnerBox.Text.Substring(0, m_OwnerBox.SelectionStart));
                m_OwnerBox.Select(m.Groups["Last"].Index, m.Groups["Last"].Length);
                m_OwnerBox.SelectedText = SelectedValue.ToString();
                if (MainControl.m_HitCounts != null)
                    MainControl.m_HitCounts[SelectedValue] = m_nUseIndex++;
            }
            this.Hide();
        }

        private void MainControl_Click(object sender, EventArgs e)
        {
            if (m_ReselectForm != null)
                m_ReselectForm.Activate();
        }

        //Doesn't intercept Alt+F4:
        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            return m_OwnerBox.DoProcessCmdKey(ref msg, keyData);
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            m_OwnerBox.DoOnClosing(e);
        }

        protected override void WndProc(ref Message m)
        {
            if (m.Msg == Messaging.WM_ACTIVATEAPP && m.WParam == (IntPtr)0)
            {
                this.Hide();
            }
            base.WndProc(ref m);
        }
    }

    internal class ListBoxEx : ListBox
    {
        internal BoxAutoComplete m_OwnerBox;
        internal Dictionary<object, int> m_HitCounts = new Dictionary<object, int>();

        public new int FindString(String s)
        {
            object lastObj = null;

            int exact = base.FindStringExact(s);
            if (exact != ListBox.NoMatches)
                return exact;

            int lastUse = -1;
            foreach (KeyValuePair<object, int> kvp in m_HitCounts)
                if (kvp.Key.ToString().ToLower().StartsWith(s.ToLower()) && kvp.Value > lastUse)
                {
                    lastObj = kvp.Key;
                    lastUse = kvp.Value;
                }
            if (lastObj != null)
                return Items.IndexOf(lastObj);
            else
                return base.FindString(s);
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Up || e.KeyCode == Keys.Down)
                base.OnKeyDown(e);
            else
                m_OwnerBox.HandleKeyDown(e);
        }

        protected override void OnKeyPress(KeyPressEventArgs e)
        {
            m_OwnerBox.HandleKeyPress(e);
        }

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            return m_OwnerBox.DoProcessCmdKey(ref msg, keyData);
        }
    }
}