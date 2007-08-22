using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Microsoft.Win32;

namespace Reaction_Editor
{
    public partial class FrmAutobalanceExtraComps : Form
    {
        private Dictionary<string, string> Sets = new Dictionary<string, string>();
        private RegistryKey regKey;

        public String SelectedList
        {
            get { return Sets[comboSet.Text]; }
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

        public void SaveToRegistry()
        {
            try
            {
                foreach (string s in regKey.GetValueNames())
                    if (s.StartsWith("Set"))
                        regKey.DeleteValue(s);
                string setNames = "";
                foreach (string s in comboSet.Items)
                {
                    setNames += s + ",";
                    regKey.SetValue("BalanceSet" + s, Sets[s]);
                }
                regKey.SetValue("SetNames", setNames);
            }
            catch { }
        }

        public FrmAutobalanceExtraComps()
        {
            InitializeComponent();
            txtCompounds.HitCounts = Program.AutocompleteHitCounts;
            lblNote.MaximumSize = this.Size;
            UpdateAutocomplete();
        }

        public FrmAutobalanceExtraComps(RegistryKey _regKey)
        {
            InitializeComponent();
            txtCompounds.HitCounts = Program.AutocompleteHitCounts;
            lblNote.MaximumSize = this.Size;
            UpdateAutocomplete();

            regKey = _regKey;
            foreach (string s in ((string)regKey.GetValue("SetNames", "")).Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries))
            {
                Sets[s] = (string)regKey.GetValue("BalanceSet" + s, "");
                comboSet.Items.Add(s);
            }
            if (comboSet.Items.Count > 0)
                comboSet.SelectedIndex = 0;
        }

        public FrmAutobalanceExtraComps(FrmAutobalanceExtraComps original)
        {
            InitializeComponent();
            txtCompounds.HitCounts = Program.AutocompleteHitCounts;
            lblNote.MaximumSize = this.Size;
            UpdateAutocomplete();

            this.regKey = original.regKey;
            this.Sets = new Dictionary<string, string>(original.Sets);
            foreach (string s in original.comboSet.Items)
                comboSet.Items.Add(s);
            comboSet.SelectedIndex = original.comboSet.SelectedIndex;
        }

        public void UpdateAutocomplete()
        {
            txtCompounds.Items.Clear();
            foreach (Compound c in Compound.CompoundList.Values)
                txtCompounds.Items.Add(c);
        }

        private void txtCompounds_TextChanged(object sender, EventArgs e)
        {
            Sets[comboSet.Text] = txtCompounds.Text;
        }

        private void RepopulateComboSets()
        {
            List<String> ToRemove = new List<string>();
            foreach (string s in comboSet.Items)
                if (!Sets.ContainsKey(s))
                    ToRemove.Add(s);
            foreach (string s in ToRemove)
                comboSet.Items.Remove(s);

            foreach (string s in Sets.Keys)
                if (!comboSet.Items.Contains(s))
                    comboSet.Items.Add(s);
        }

        private void comboSet_SelectedIndexChanged(object sender, EventArgs e)
        {
            txtCompounds.Text = Sets[comboSet.Text];
        }

        private void comboSet_Leave(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(comboSet.Text.Trim()))
                if (comboSet.Items.Count > 0)
                    comboSet.SelectedIndex = 0;
            if (!string.IsNullOrEmpty(comboSet.Text.Trim()))
            {
                if (!Sets.ContainsKey(comboSet.Text))
                {
                    Sets[comboSet.Text] = txtCompounds.Text;
                    comboSet.Items.Add(comboSet.Text);
                }
                else
                    txtCompounds.Text = Sets[comboSet.Text];
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
        }

        private void comboSet_TextChanged(object sender, EventArgs e)
        {
            txtCompounds.Enabled = !string.IsNullOrEmpty(comboSet.Text.Trim());
        }
    }
}