using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Microsoft.Win32;
using System.Text.RegularExpressions;

namespace Reaction_Editor
{
    public partial class FrmAutobalanceExtraComps : Form
    {
        private Dictionary<string, string> m_Sets = new Dictionary<string, string>();
        private RegistryKey m_RegKey;
        public static Regex s_EndsWithCommaOrNL = new Regex(@"(?<Data>.*)(,|\n)\s*$", RegexOptions.Compiled | RegexOptions.ExplicitCapture | RegexOptions.Singleline);

        #region Properties
        public String SelectedList
        {
            get { return m_Sets[comboSet.Text]; }
        }

        public String Note
        {
            get { return lblNote.Text; }
            set
            {
                lblNote.Text = value;
                lblNote.Visible = !string.IsNullOrEmpty(value);
            }
        }
        #endregion Properties

        #region Constructors
        protected void ExtraInitialise()
        {
            txtCompounds.AllowDrop = true;
            txtCompounds.DragEnter += new DragEventHandler(txtCompounds_DragEnter);
            txtCompounds.DragDrop += new DragEventHandler(txtCompounds_DragDrop);
        }

        public FrmAutobalanceExtraComps()
        {
            InitializeComponent();
            ExtraInitialise();
            txtCompounds.HitCounts = Program.AutocompleteHitCounts;
            lblNote.MaximumSize = this.Size;
            UpdateAutocomplete();
        }

        public FrmAutobalanceExtraComps(RegistryKey _regKey)
        {
            InitializeComponent(); ExtraInitialise();
            txtCompounds.HitCounts = Program.AutocompleteHitCounts;
            lblNote.MaximumSize = this.Size;
            UpdateAutocomplete();

            m_RegKey = _regKey;
            foreach (string s in ((string)m_RegKey.GetValue("SetNames", "")).Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries))
            {
                m_Sets[s] = (string)m_RegKey.GetValue("BalanceSet" + s, "");
                comboSet.Items.Add(s);
            }
            if (comboSet.Items.Count > 0)
                comboSet.SelectedIndex = 0;
        }

        public FrmAutobalanceExtraComps(FrmAutobalanceExtraComps original)
        {
            InitializeComponent(); ExtraInitialise();
            txtCompounds.HitCounts = Program.AutocompleteHitCounts;
            lblNote.MaximumSize = this.Size;
            UpdateAutocomplete();

            this.m_RegKey = original.m_RegKey;
            this.m_Sets = new Dictionary<string, string>(original.m_Sets);
            foreach (string s in original.comboSet.Items)
                comboSet.Items.Add(s);
            comboSet.SelectedIndex = original.comboSet.SelectedIndex;
        }
        #endregion Constructors

        #region Public Functions
        public void SaveToRegistry()
        {
            try
            {
                foreach (string s in m_RegKey.GetValueNames())
                    if (s.StartsWith("Set"))
                        m_RegKey.DeleteValue(s);
                string setNames = "";
                foreach (string s in comboSet.Items)
                {
                    setNames += s + ",";
                    m_RegKey.SetValue("BalanceSet" + s, m_Sets[s]);
                }
                m_RegKey.SetValue("SetNames", setNames);
            }
            catch { }
        }

        public void UpdateAutocomplete()
        {
            txtCompounds.Items.Clear();
            foreach (Compound c in Compound.CompoundList.Values)
                txtCompounds.Items.Add(c);
        }
        #endregion Public functions

        #region Overrides to provide reasonable OK / Cancel behaviour
        protected Dictionary<string, string> m_OriginalSets;
        protected object[] m_OriginalDropdown;
        protected int m_nOriginalChoice;

        public new void Show()
        {
            this.ControlBox = false;
            Backup();
            base.Show();
        }

        public new void Show(IWin32Window owner)
        {
            this.ControlBox = false;
            Backup();
            base.Show(owner);
        }

        public new DialogResult ShowDialog()
        {
            this.MdiParent = null;
            this.Hide();
            this.ControlBox = true;
            Backup();
            return base.ShowDialog();
        }

        public new DialogResult ShowDialog(IWin32Window owner)
        {
            this.MdiParent = null;
            this.Hide();
            this.ControlBox = true;
            Backup();
            return base.ShowDialog(owner);
        }

        protected void Backup()
        {
            m_OriginalSets = new Dictionary<string, string>(m_Sets);
            m_OriginalDropdown = new object[comboSet.Items.Count];
            comboSet.Items.CopyTo(m_OriginalDropdown, 0);
            m_nOriginalChoice = comboSet.SelectedIndex;
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            if (e.CloseReason == CloseReason.UserClosing)
                DoCancel();

            if (!this.Modal && e.CloseReason == CloseReason.UserClosing)
            {
                e.Cancel = true;
                this.Hide();
            }
 	        base.OnFormClosing(e);
        }

        protected void DoCancel()
        {
            m_Sets = m_OriginalSets;
            comboSet.Items.Clear();
            comboSet.Items.AddRange(m_OriginalDropdown);
            comboSet.SelectedIndex = m_nOriginalChoice;
        }
        #endregion Overrides to provide reasonable OK / Cancel behaviour

        /*protected override void OnLeave(EventArgs e)
        {
            if (this.Visible)
                this.Activate();
            base.OnLeave(e);
        }*/

        private void txtCompounds_TextChanged(object sender, EventArgs e)
        {
            m_Sets[comboSet.Text] = txtCompounds.Text;

            int oldSelectionStart = txtCompounds.SelectionStart;
            int oldSelectionLength = txtCompounds.SelectionLength;
            txtCompounds.SelectAll();
            txtCompounds.SelectionBackColor = txtCompounds.BackColor;
            txtCompounds.SelectionColor = txtCompounds.ForeColor;

            Match m = Compound.s_CompoundSeperator.Match(txtCompounds.Text);
            foreach (Capture c in m.Groups["Comp"].Captures)
            {
                if (!Compound.Contains(c.Value.Trim()))
                {
                    txtCompounds.Select(c.Index, c.Length);
                    txtCompounds.SelectionBackColor = Color.DarkRed;
                    txtCompounds.SelectionColor = Color.White;
                }
            }
            txtCompounds.Select(oldSelectionStart, oldSelectionLength);
        }

        private void RepopulateComboSets()
        {
            List<String> ToRemove = new List<string>();
            foreach (string s in comboSet.Items)
                if (!m_Sets.ContainsKey(s))
                    ToRemove.Add(s);
            foreach (string s in ToRemove)
                comboSet.Items.Remove(s);

            foreach (string s in m_Sets.Keys)
                if (!comboSet.Items.Contains(s))
                    comboSet.Items.Add(s);
        }

        private void comboSet_SelectedIndexChanged(object sender, EventArgs e)
        {
            txtCompounds.Text = m_Sets[comboSet.Text];
        }

        private void comboSet_Leave(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(comboSet.Text.Trim()))
                if (comboSet.Items.Count > 0)
                    comboSet.SelectedIndex = 0;
            if (!string.IsNullOrEmpty(comboSet.Text.Trim()))
            {
                if (!m_Sets.ContainsKey(comboSet.Text))
                {
                    m_Sets[comboSet.Text] = txtCompounds.Text;
                    comboSet.Items.Add(comboSet.Text);
                }
                else
                    txtCompounds.Text = m_Sets[comboSet.Text];
            }
        }

        private void btnDelete_Click(object sender, EventArgs e)
        {
            try { comboSet.Items.Remove(comboSet.Text); }
            catch { }
            if (comboSet.Items.Count > 0)
                comboSet.SelectedIndex = 0;
            else
                comboSet.Text = "";

        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            SaveToRegistry();
            if (!this.Modal)
                this.Hide();
        }

        private void comboSet_TextChanged(object sender, EventArgs e)
        {
            txtCompounds.Enabled = !string.IsNullOrEmpty(comboSet.Text.Trim());
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            DoCancel();
            if (!this.Modal)
                this.Hide();
        }

        void txtCompounds_DragEnter(object sender, DragEventArgs e)
        {
            RichTextBox box = (RichTextBox)sender;
            if (!e.Data.GetDataPresent(typeof(Compound)))
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            Compound newComp = (Compound)e.Data.GetData(typeof(Compound));
            //Determine if we already have the compound in the textbox:
            if (Regex.Match(box.Text, @"(^|,|\n)\s*" + Regex.Escape(newComp.Symbol) + @"\s*(,|$)", RegexOptions.ExplicitCapture).Success)
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            e.Effect = DragDropEffects.Link;
        }

        void txtCompounds_DragDrop(object sender, DragEventArgs e)
        {
            RichTextBox box = (RichTextBox)sender;
            if (!e.Data.GetDataPresent(typeof(Compound)))
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            Compound newComp = (Compound)e.Data.GetData(typeof(Compound));
            //Determine if we already have the compound in the textbox:
            if (Regex.Match(box.Text, @"(^|,|\n)\s*" + Regex.Escape(newComp.Symbol) + @"\s*(,|$)", RegexOptions.ExplicitCapture).Success)
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            e.Effect = DragDropEffects.Link;

            if (s_EndsWithCommaOrNL.Match(txtCompounds.Text).Success)
                txtCompounds.AppendText(newComp.Symbol);
            else
                txtCompounds.AppendText(", " + newComp.Symbol);
        }
    }
}