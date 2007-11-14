using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Text.RegularExpressions;
using System.Collections;

namespace Configuration_Editor
{
    public partial class ProjectVectorControl : UserControl
    {
        #region Constructors
        public ProjectVectorControl()
        {
            InitializeComponent();
        }

        public void SetupSpecies()
        {
            this.SuspendLayout();
            pnlActive.Size = pnlBlank.Size;
            foreach (TabPage tab in tcIDE.TabPages)
            {
                foreach (Control c in tab.Controls)
                {
                    c.Parent = tcIDE.Parent;
                    c.Visible = false;
                    c.Dock = DockStyle.Fill;
                    c.Visible = false;
                }
            }
            pnlBlank.Visible = true;
            tcIDE.Parent = null;
            tcIDE.Dispose();

            ImageList lst = new ImageList();
            lst.ColorDepth = ColorDepth.Depth32Bit;
            lstProjectVector.SmallImageList = lst;

            lst.Images.Add("Calculation", Configuration_Editor.Properties.Resources.FunctionHS);
            lst.Images.Add("PageBreak", Configuration_Editor.Properties.Resources.InsertTabControlHS);
            lst.Images.Add("Attribute", Configuration_Editor.Properties.Resources.PropertiesHS);
            lst.Images.Add("Text", Configuration_Editor.Properties.Resources.TextboxHS);
            lst.Images.Add("Sum", Configuration_Editor.Properties.Resources.Sum);

            //tcSpecies.TabPages.Remove(tabSpecieDatabase);

            AddSumItems();

            this.ResumeLayout();
        }

        #endregion Constructors

        #region Varliables
        ProjectVectorItem m_CurrentItem = null;
        FrmAddSelector m_AddSelector = new FrmAddSelector();

        ListView lstDBSpecies = null;
        DataTable m_SpecieDataTable = null;

        string m_StatusMessage;
        Color m_StatusColour;

        protected PVIOrderer m_Sorter = new PVIOrderer();

        protected Dictionary<string, BindingSource> m_UnitBindingSource;

        protected ConvertEventHandler m_TempFormatter;
        protected ConvertEventHandler m_TempParser;
        #endregion Variables

        #region Propreties
        public ConvertEventHandler TempParser
        {
            get { return m_TempParser; }
            set
            {
                m_TempParser = value;
                foreach (ListViewItem lvi in lstProjectVector.Items)
                    if (lvi.Tag is ProjectSpecie)
                        ((ProjectSpecie)lvi.Tag).TempParser = value;
            }
        }

        public ConvertEventHandler TempFormatter
        {
            get { return m_TempFormatter; }
            set
            {
                m_TempFormatter = value;
                foreach (ListViewItem lvi in lstProjectVector.Items)
                    if (lvi.Tag is ProjectSpecie)
                        ((ProjectSpecie)lvi.Tag).TempFormatter = value;
            }
        }

        public PVIOrderer Sorter
        {
            get { return m_Sorter; }
            set { m_Sorter = value; }
        }

        public ListView ListDBSpecies
        {
            get { return lstDBSpecies; }
            set { lstDBSpecies = value; }
        }

        public DataTable SpecieDataTable
        {
            get { return m_SpecieDataTable; }
            set { m_SpecieDataTable = value; }
        }

        public string StatusMessage
        {
            get { return m_StatusMessage; }
            set
            {
                if (m_StatusMessage == value)
                    return;

                m_StatusMessage = value;
                if (StatusMessageChanged != null)
                    StatusMessageChanged(this, new EventArgs());
            }
        }

        public Color StatusColour
        {
            get { return m_StatusColour; }
            set
            {
                if (m_StatusColour == value)
                    return;
                m_StatusColour = value;
                if (StatusMessageChanged != null)
                    StatusMessageChanged(this, new EventArgs());
            }
        }

        public Dictionary<string, BindingSource> UnitBindingSource
        {
            get { return m_UnitBindingSource; }
            set
            {
                if (value == null)
                    return;
                m_UnitBindingSource = value;
                Binding grpTempRangeBinding = new Binding("Text", value["temperature"], "Name", true, DataSourceUpdateMode.Never);
                grpTempRangeBinding.Format += new ConvertEventHandler(grpTempRangeBinding_Format);
                grpTempRange.DataBindings.Add(grpTempRangeBinding);
                m_UnitBindingSource["temperature"].CurrentChanged += new EventHandler(m_TempBindingSource_CurrentChanged);
            }
        }

        void m_TempBindingSource_CurrentChanged(object sender, EventArgs e)
        {
            UpdateProjectLVIs();
            if (m_CurrentItem is ProjectSpecie)
                LoadItem(m_CurrentItem);
        }

        void grpTempRangeBinding_Format(object sender, ConvertEventArgs e)
        {
            if (e.DesiredType == typeof(string))
                e.Value = "Temperature Range (" + e.Value + ")";
        }
        #endregion Properties

        #region Events
        public event EventHandler StatusMessageChanged;
        #endregion Events

        #region Public Functions
        public void Sort()
        {
            List<ProjectVectorItem> items = new List<ProjectVectorItem>();
            foreach (ListViewItem lvi in lstProjectVector.Items)
                items.Add((ProjectVectorItem)lvi.Tag);

            items = m_Sorter.Sort(items);

            lstProjectVector.BeginUpdate();
            lstProjectVector.Items.Clear();
            foreach (ProjectVectorItem i in items)
                lstProjectVector.Items.Add(i.LVI);
            lstProjectVector.EndUpdate();
        }

        public bool TransferSpecies()
        {
            DataRow[] values = new DataRow[lstDBSpecies.SelectedItems.Count];
            int i = 0;
            foreach (ListViewItem lvi in lstDBSpecies.SelectedItems)
                values[i++] = (DataRow)lvi.Tag;

            return AddSpecies(values, -1);
        }

        public void AddToCalculation(string s)
        {
            if (m_CurrentItem is ProjectCalculation)
                txtCalculation.Text += s;
        }

        public void RemoveSpecies(DataRow r)
        {
            ArrayList projectItems = new ArrayList(lstProjectVector.Items);
            foreach (ListViewItem projLVI in projectItems)
                if (projLVI.Tag is ProjectSpecie && ((ProjectSpecie)projLVI.Tag).SpDataRow == r)
                    lstProjectVector.Items.Remove(projLVI);
        }
        
        //Called after a symbol is changed in the database. If the selected compound is in the project specie list, updates that compound & rechecks calculations (Does not refactor calculations).
        public void UpdateProjectLVIs()
        {
            bool changed = false;
            try
            {
                DataRow curRow = ((DataRowView)this.BindingContext[m_SpecieDataTable].Current).Row;
                foreach (ListViewItem lvi in lstProjectVector.Items)
                    if (lvi.Tag is ProjectSpecie &&
                        ((ProjectSpecie)lvi.Tag).SpDataRow == curRow)
                    {
                        changed = true;
                        ((ProjectSpecie)lvi.Tag).UpdateLVI();
                    }
            }
            catch { }
            if (changed)
                CheckCalculations();
        }

        private static Regex SpecieRegex = new Regex(@"^\s*S\d+\s*$", RegexOptions.Compiled | RegexOptions.IgnoreCase);
        public void LoadSpecies(Dictionary<string, string> RelevantContents, List<string> Errors)
        {
            lstProjectVector.BeginUpdate();
            lstProjectVector.Items.Clear();
            List<string> OrderedKeys = new List<string>(RelevantContents.Keys);
            OrderedKeys.Sort();
            foreach (string s in OrderedKeys)
            {
                if (!SpecieRegex.Match(s).Success)
                    continue;
                try
                {
                    ProjectVectorItem item = ProjectVectorItem.Parse(RelevantContents[s], m_SpecieDataTable, Errors);
                    item.Changed += new EventHandler(item_Changed);
                    lstProjectVector.Items.Add(item.LVI);
                }
                catch
                {
                    Errors.Add("Unalbe to Parse: '" + s + " = " + RelevantContents[s] + "'");
                }
            }
            AddSumItems();
            lstProjectVector.EndUpdate();
            CheckCalculations();
        }

        public string SaveString()
        {
            StringBuilder sb = new StringBuilder();
            int i = 0;
            foreach (ListViewItem lvi in lstProjectVector.Items)
            {
                try
                {
                    sb.AppendLine("S" + i++.ToString("0000") + "=" + ((ProjectVectorItem)lvi.Tag).ToSaveString());
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.ToString());
                }
            }
            return sb.ToString();
        }
        #endregion Public Functions

        #region Protected Functions

        protected void AddSumItems()
        {
            lstProjectVector.BeginUpdate();
            bool totalContained = false, liquidContained = false, solidContained = false, vapourContained = false;
            foreach (ListViewItem lvi in lstProjectVector.Items)
                if (lvi.Tag is ProjectSum)
                    switch (((ProjectSum)lvi.Tag).Phase)
                    {
                        case Phase.Gas:
                            vapourContained = true; break;
                        case Phase.Liquid:
                            liquidContained = true; break;
                        case Phase.Solid:
                            solidContained = true; break;
                        case Phase.Total:
                            totalContained = true; break;
                    }
            if (!totalContained) lstProjectVector.Items.Add(new ProjectSum(Phase.Total).LVI);
            if (!solidContained) lstProjectVector.Items.Add(new ProjectSum(Phase.Solid).LVI);
            if (!liquidContained) lstProjectVector.Items.Add(new ProjectSum(Phase.Liquid).LVI);
            if (!vapourContained) lstProjectVector.Items.Add(new ProjectSum(Phase.Gas).LVI);
            lstProjectVector.EndUpdate();
        }

        protected ListViewItem FindLVIBelow(Point p)
        {
            int nClosestVal = int.MaxValue;
            ListViewItem closestLVI = null;
            foreach (ListViewItem lvi in lstProjectVector.Items)
                if (lvi.Bounds.Bottom > p.Y &&
                    lvi.Bounds.Bottom - p.Y < nClosestVal)
                {
                    nClosestVal = lvi.Bounds.Bottom - p.Y;
                    closestLVI = lvi;
                }
            return closestLVI;
        }

        protected void LoadItem(ProjectVectorItem item)
        {
            this.SuspendLayout();
            m_CurrentItem = item;

            grpSpecie.Visible = grpTextValue.Visible = grpCalculation.Visible = pnlBlank.Visible = false;

            if (item == null)
            {
                pnlBlank.Visible = true;
            }
            else if (item.GetType() == typeof(ProjectSpecie))
            {
                grpSpecie.Visible = true;
                UpdateTemps();
                chkIdeal.Checked = ((ProjectSpecie)item).Ideal;
                chkIdeal.Enabled = ((ProjectSpecie)item).Phase == Phase.Gas;
                grpSpecie.Text = item.Name;
            }
            else if (item.GetType() == typeof(ProjectCalculation))
            {
                grpCalculation.Visible = true;
                txtCalculation.Text = item.Value;
                txtCalcDesc.Text = ((ProjectCalculation)item).Desc;
                txtCalcSymbol.Text = item.Name;
            }
            else if (item.GetType() == typeof(ProjectText))
            {
                grpTextValue.Visible = true;
                txtText.Text = item.Value;
            }
            else if (item.GetType() == typeof(ProjectPage))
            {
                grpTextValue.Visible = true;
                txtText.Text = item.Value;
            }
            else if (item.GetType() == typeof(ProjectAttribute))
            {
                grpAttribute.Visible = true;
                txtAttributeName.Text = item.Name;
                txtAttDimension.Text = ((ProjectAttribute)item).Cnv;
                txtAttParent.Text = ((ProjectAttribute)item).Parent;
                comboAttributeType.SelectedIndex = (int)((ProjectAttribute)item).AttributeType;
            }
            this.ResumeLayout();
        }

        protected void UpdateStatusMessage()
        {
            if (m_CurrentItem != null)
            {
                StatusMessage = m_CurrentItem.Valid ? "Status: Ok" :
                    "Status: Invalid. " + m_CurrentItem.StatusDetails;
                StatusColour = m_CurrentItem.Valid ? SystemColors.WindowText : Color.Red;
            }
            else
            {
                StatusMessage = "";
                StatusColour = SystemColors.WindowText;
            }
        }

        protected void SetBtnEnables()
        {
            if (lstProjectVector.SelectedItems.Count == 0)
                btnRemove.Enabled = btnMoveDown.Enabled = btnMoveUp.Enabled = false;
            else
            {
                bool canRemove = false;
                foreach (ListViewItem lvi in lstProjectVector.SelectedItems)
                    if (!(lvi.Tag is ProjectSum))
                        canRemove = true;
                btnRemove.Enabled = canRemove;
                btnMoveUp.Enabled = lstProjectVector.SelectedIndices[0] != 0;
                btnMoveDown.Enabled = lstProjectVector.SelectedIndices[lstProjectVector.SelectedIndices.Count - 1] < lstProjectVector.Items.Count - 1;
            }
        }

        protected void UpdateTemps()
        {
            txtDefinedMinTemp.Text = ((ProjectSpecie)m_CurrentItem).MinDefinedTemp.ToString("0.00");
            txtDefinedMaxTemp.Text = ((ProjectSpecie)m_CurrentItem).MaxDefinedTemp.ToString("0.00");
            numGreaterTempTolerance.Value = (decimal)((ProjectSpecie)m_CurrentItem).ExtraAboveT;
            numLowerTempTolerance.Value = (decimal)((ProjectSpecie)m_CurrentItem).ExtraBelowT;
            txtAllowedMinTemp.Text = ((ProjectSpecie)m_CurrentItem).MinAllowedTemp.ToString("0.00");
            txtAllowedMaxTemp.Text = ((ProjectSpecie)m_CurrentItem).MaxAllowedTemp.ToString("0.00");
        }

        List<string> GetAvailableVariables()
        {
            List<string> availableVariables = new List<string>();
            foreach (ListViewItem lvi in lstProjectVector.Items)
                if (lvi.Tag is ProjectSpecie)
                    availableVariables.Add("[" + ((ProjectSpecie)lvi.Tag).Symbol + "]");
                else if (lvi.Tag is ProjectAttribute)
                    availableVariables.Add("[" + ((ProjectAttribute)lvi.Tag).Name + "]");
                else if (lvi.Tag is ProjectCalculation)
                    availableVariables.Add("[" + ((ProjectCalculation)lvi.Tag).Desc + "]");
            return availableVariables;
        }
        
        protected void HighlightText(RichTextBox box, int start, int length)
        {
            box.Select(start, length);
            box.SelectionBackColor = Color.DarkRed;
            box.SelectionColor = Color.White;
        }

        protected void CheckCalculations() //Also checks Attributes.
        {
            List<string> availableVariables = GetAvailableVariables();
            List<string> availableParents = GetAvailableParents();
            foreach (ListViewItem lvi in lstProjectVector.Items)
                if (lvi.Tag is ProjectCalculation)
                {
                    ProjectCalculation calc = (ProjectCalculation)lvi.Tag;
                    calc.Valid = true;
                    try
                    {
                        EquationFragment frag = EquationFragment.Parse(calc.Value, new Dictionary<string, FunctionValue>());
                        foreach (string s in frag.ReqVariableNames)
                            if (!availableVariables.Contains(s))
                            {
                                calc.Valid = false;
                                break;
                            }
                    }
                    catch
                    {
                        calc.Valid = false;
                    }
                }
                else if (lvi.Tag is ProjectAttribute)
                {
                    ProjectAttribute att = (ProjectAttribute)lvi.Tag;
                    if (!availableParents.Contains(att.Parent))
                    {
                        att.StatusDetails = "Parent not found.";
                        att.Valid = false;
                    }
                }

            List<object> temp = new List<object>();
            foreach (object o in txtCalculation.Items)
                temp.Add(o);
            foreach (string s in temp)
            {
                if (!availableVariables.Contains(s))
                    txtCalculation.Items.Remove(s);
            }
            foreach (string s in availableVariables)
                if (!txtCalculation.Items.Contains(s))
                    txtCalculation.Items.Add(s);
        }

        List<string> GetAvailableParents()
        {
            List<string> ret = new List<string>();
            foreach (ListViewItem lvi in lstProjectVector.Items)
                if (lvi.Tag is ProjectSpecie)
                    ret.Add(((ProjectSpecie)lvi.Tag).Symbol);
                else if (lvi.Tag is ProjectCalculation)
                    ret.Add(((ProjectCalculation)lvi.Tag).Desc);
            return ret;
        }

        protected bool AddSpecies(DataRow[] values, int insertionIndex)
        {
            bool ret = false;
            lstProjectVector.BeginUpdate();
            List<ListViewItem> lvisToSelect = new List<ListViewItem>();
            foreach (DataRow r in values)
            {
                ListViewItem alreadyItem = AlreadyContained(r);
                if (alreadyItem == null)
                {
                    ProjectSpecie specie = new ProjectSpecie(r);
                    specie.Changed += new EventHandler(item_Changed);
                    specie.TempFormatter = m_TempFormatter;
                    specie.TempParser = m_TempParser;

                    lvisToSelect.Add(specie.LVI);

                    if (insertionIndex < 0)
                        lstProjectVector.Items.Add(specie.LVI);
                    else
                        lstProjectVector.Items.Insert(insertionIndex++, specie.LVI);
                    ret = true;
                }
                else
                    lvisToSelect.Add(alreadyItem);
            }
            if (ret == true)
            {
                lstProjectVector.SelectedItems.Clear();
                foreach (ListViewItem lvi in lvisToSelect)
                    lvi.Selected = true;
                CheckCalculations();
            }
            lstProjectVector.EndUpdate();
            return ret;
        }

        protected ListViewItem AlreadyContained(DataRow s)
        {
            foreach (ListViewItem lvi in lstProjectVector.Items)
                if (lvi.Tag is ProjectSpecie)
                    if (((ProjectSpecie)lvi.Tag).SpDataRow == s)
                        return lvi;
            return null;
        }

        protected void DoAddInterface()
        {
            if (m_AddSelector.ShowDialog() == DialogResult.Cancel) return;

            lstProjectVector.SelectedItems.Clear(); //Say what?
            ProjectVectorItem item = null;
            if (m_AddSelector.radioAttribute.Checked)
            {
                item = new ProjectAttribute();
                item.Name = GetNextName("Att"); item.Valid = false;
            }
            else if (m_AddSelector.radioCalculation.Checked)
            {
                item = new ProjectCalculation();
                item.Name = GetNextName("Calc"); item.Valid = false;
            }
            else if (m_AddSelector.radioLabel.Checked)
            {
                item = new ProjectText();
                item.Name = GetNextName("Label");
            }
            else if (m_AddSelector.radioPageBreak.Checked)
            {
                item = new ProjectPage();
                item.Name = GetNextName("Page");
            }

            if (lstProjectVector.SelectedItems.Count == 0)
                lstProjectVector.Items.Add(item.LVI);
            else
                lstProjectVector.Items.Insert(lstProjectVector.SelectedIndices[0], item.LVI);

            item.Changed += new EventHandler(item_Changed);
            item.LVI.Selected = true;
        }
        #endregion Protected Functions

        #region Event Handling
        protected void PreventCommas(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == ',')
                e.Handled = true;
        }

        private void FireCheckCalculations(object sender, EventArgs e)
        {
            CheckCalculations();
        }

        private void lstProjectVector_DragDrop(object sender, DragEventArgs e)
        {
            try
            {
                lstProjectVector.BeginUpdate();
                ListViewItem lvi = FindLVIBelow(lstProjectVector.PointToClient(new Point(e.X, e.Y)));
                int insertionIndex = lstProjectVector.Items.Count;
                if (lvi != null)
                    insertionIndex = lvi.Index;


                if (e.Data.GetDataPresent(typeof(SpecieDrag)))
                {
                    lstProjectVector.SelectedItems.Clear();
                    DataRow[] values = ((SpecieDrag)e.Data.GetData(typeof(SpecieDrag))).Value;
                    AddSpecies(values, insertionIndex);
                }
                else if (e.Data.GetDataPresent(typeof(ProjectVectorItemDrag)))
                {
                    lstProjectVector.SelectedItems.Clear();
                    ProjectVectorItemDrag item = (ProjectVectorItemDrag)e.Data.GetData(typeof(ProjectVectorItemDrag));
                    for (int i = 0; i < item.items.Length; i++)
                    {
                        if (item.items[i].LVI.Index < i)
                            insertionIndex--;
                        lstProjectVector.Items.Remove(item.items[i].LVI);
                    }
                    for (int i = 0; i < item.items.Length; i++)
                    {
                        lstProjectVector.Items.Insert(insertionIndex++, item.items[i].LVI);
                        item.items[i].LVI.Selected = true;
                    }
                }
            }
            finally
            {
                lstProjectVector.EndUpdate();
            }
        }

        private void lstProjectVector_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(typeof(SpecieDrag)))
                e.Effect = DragDropEffects.Link;
        }

        private void lstProjectVector_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lstProjectVector.SelectedItems.Count == 1)
                LoadItem((ProjectVectorItem)lstProjectVector.SelectedItems[0].Tag);
            else
                LoadItem(null);

            UpdateStatusMessage();

            SetBtnEnables();
        }

        private void numLowerTempTolerance_ValueChanged(object sender, EventArgs e)
        {
            if (m_CurrentItem is ProjectSpecie)
            {
                ((ProjectSpecie)m_CurrentItem).ExtraBelowT = (double)numLowerTempTolerance.Value;
                UpdateTemps();
            }
        }

        private void numGreaterTempTolerance_ValueChanged(object sender, EventArgs e)
        {
            if (m_CurrentItem is ProjectSpecie)
            {
                ((ProjectSpecie)m_CurrentItem).ExtraAboveT = (double)numGreaterTempTolerance.Value;
                UpdateTemps();
            }
        }

        protected string GetNextName(string start)
        {
            List<string> RelevantItems = new List<string>();
            foreach (ListViewItem lvi in lstProjectVector.Items)
                if (((ProjectVectorItem)lvi.Tag).Name.StartsWith(start))
                    RelevantItems.Add(((ProjectVectorItem)lvi.Tag).Name);

            int i = 1;
            while (RelevantItems.Contains(start + i))
                i++;
            return start + i;
        }

        private void btnAdd_Click(object sender, EventArgs e)
        {
            if (lstDBSpecies.Focused && TransferSpecies())
                return;

            DoAddInterface();
        }

        void item_Changed(object sender, EventArgs e)
        {
            UpdateStatusMessage();
        }

        private void txtCalculation_TextChanged(object sender, EventArgs e)
        {
            List<string> availableVariables = GetAvailableVariables();
            int oldSelectionStart = txtCalculation.SelectionStart;
            int oldSelectionLength = txtCalculation.SelectionLength;
            txtCalculation.SelectAll();
            txtCalculation.SelectionBackColor = SystemColors.Window;
            txtCalculation.SelectionColor = SystemColors.WindowText;
            if (m_CurrentItem is ProjectCalculation)
            {
                m_CurrentItem.Value = txtCalculation.Text;

                try
                {
                    EquationFragment frag = EquationFragment.Parse(txtCalculation.Text, new Dictionary<string, FunctionValue>());
                    bool eqnOk = true;
                    List<string> invalidNames = new List<string>();
                    foreach (string s in frag.ReqVariableNames)
                    {
                        bool ok = availableVariables.Contains(s);
                        if (!ok)
                        {
                            int i = txtCalculation.Text.IndexOf(s, 0);
                            while (i >= 0)
                            {
                                HighlightText(txtCalculation, i, s.Length);
                                i = txtCalculation.Text.IndexOf(s, i + 1);
                            }
                            invalidNames.Add(s);
                        }
                        eqnOk = eqnOk && ok;
                    }

                    if (!eqnOk)
                    {
                        StringBuilder statusDetails = new StringBuilder("Unrecognised Variables: ");
                        foreach (string s in invalidNames)
                            statusDetails.Append("'" + s + "', ");
                        m_CurrentItem.StatusDetails = statusDetails.ToString().Substring(0, statusDetails.Length - 2);
                    }
                    ((ProjectCalculation)m_CurrentItem).Valid = eqnOk;

                }
                catch (Function.FunctionNotFoundException ex)
                {
                    Regex r = new Regex(@"(?<=\b\d*)(?<Func>" + Regex.Escape(ex.FunctionName) + @")\(");
                    for (Match m = r.Match(txtCalculation.Text); m.Success; m = m.NextMatch())
                        HighlightText(txtCalculation, m.Groups["Func"].Index, m.Groups["Func"].Length);
                    m_CurrentItem.Valid = false;
                    m_CurrentItem.StatusDetails = "Unrecognised Function Found";
                }
                catch (Exception ex)
                {
                    int temp = 0;
                    Match vbm = EquationFragment.VariableBracketsRegex.Match(txtCalculation.Text);
                    while (temp < 2)
                    {
                        temp++;
                        if (vbm.Groups["Opening"].Captures.Count > 0) //We have too many opening brackets...
                            foreach (Capture c in vbm.Groups["Opening"].Captures)
                            {
                                HighlightText(txtCalculation, c.Index, c.Length);
                                temp++;
                            }
                        if (vbm.Groups["ErronousClosing"].Captures.Count > 0)
                            foreach (Capture c in vbm.Groups["ErronousClosing"].Captures)
                            {
                                HighlightText(txtCalculation, c.Index, c.Length);
                                temp++;
                            }
                        vbm = EquationFragment.BracketsRegex.Match(txtCalculation.Text);
                    }
                    m_CurrentItem.Valid = false;
                    m_CurrentItem.StatusDetails = "Equation unpareseable (" + ex.Message + ")";
                }
            }
            txtCalculation.Select(oldSelectionStart, oldSelectionLength);
        }

        private void txtText_TextChanged(object sender, EventArgs e)
        {
            if (m_CurrentItem != null)
                m_CurrentItem.Value = txtText.Text;
        }

        private void chkIdeal_CheckedChanged(object sender, EventArgs e)
        {
            if (m_CurrentItem is ProjectSpecie)
                ((ProjectSpecie)m_CurrentItem).Ideal = chkIdeal.Checked;
        }

        private void txtAttributeName_TextChanged(object sender, EventArgs e)
        {
            if (m_CurrentItem is ProjectAttribute)
                m_CurrentItem.Name = txtAttributeName.Text;
        }

        private void comboAttributeType_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (m_CurrentItem is ProjectAttribute)
                ((ProjectAttribute)m_CurrentItem).AttributeType = (AttributeType)comboAttributeType.SelectedIndex;
        }

        private void lstProjectVector_ItemDrag(object sender, ItemDragEventArgs e)
        {
            ProjectVectorItemDrag drag = new ProjectVectorItemDrag();
            drag.items = new ProjectVectorItem[lstProjectVector.SelectedItems.Count];
            for (int i = 0; i < lstProjectVector.SelectedItems.Count; i++)
            {
                drag.items[i] = (ProjectVectorItem)lstProjectVector.SelectedItems[i].Tag;
            }
            DoDragDrop(drag, DragDropEffects.Move);
        }

        private void lstProjectVector_DragOver(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(typeof(ProjectVectorItemDrag)))
                e.Effect = DragDropEffects.Move;
        }

        private void btnRemove_Click(object sender, EventArgs e)
        {
            ListViewItem[] selectedItems = new ListViewItem[lstProjectVector.SelectedItems.Count];
            lstProjectVector.SelectedItems.CopyTo(selectedItems, 0);
            lstProjectVector.BeginUpdate();
            foreach (ListViewItem lvi in selectedItems)
                if (lvi.Tag is ProjectSum)
                    continue;
                else
                    lstProjectVector.Items.Remove(lvi);
            CheckCalculations();
            lstProjectVector.EndUpdate();
        }

        private void btnMoveUp_Click(object sender, EventArgs e)
        {
            //Rather than move everything up, we're going to move one down.
            if (lstProjectVector.SelectedItems.Count == 0 || lstProjectVector.SelectedItems[0].Index == 0)
                return;
            ListViewItem lvi = lstProjectVector.Items[lstProjectVector.SelectedItems[0].Index - 1];
            int moveTo = lstProjectVector.SelectedItems[lstProjectVector.SelectedItems.Count - 1].Index;
            lvi.Remove();
            lstProjectVector.Items.Insert(moveTo, lvi);
            SetBtnEnables();
        }

        private void btnMoveDown_Click(object sender, EventArgs e)
        {
            //Rather than move everything down, we're going to move one up.
            if (lstProjectVector.SelectedItems.Count == 0 || lstProjectVector.SelectedItems[lstProjectVector.SelectedItems.Count - 1].Index == lstProjectVector.Items.Count)
                return;
            ListViewItem lvi = lstProjectVector.Items[lstProjectVector.SelectedItems[lstProjectVector.SelectedItems.Count - 1].Index + 1];
            lvi.Remove();
            int moveTo = lstProjectVector.SelectedItems[0].Index;
            lstProjectVector.Items.Insert(moveTo, lvi);
            SetBtnEnables();
        }

        private void txtCalcDesc_TextChanged(object sender, EventArgs e)
        {
            if (m_CurrentItem is ProjectCalculation)
                ((ProjectCalculation)m_CurrentItem).Desc = txtCalcDesc.Text;
        }

        private void lstProjectVector_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Delete)
                btnRemove_Click(sender, e);
        }

        private void txtCalcSymbol_TextChanged(object sender, EventArgs e)
        {
            if (m_CurrentItem is ProjectCalculation)
                m_CurrentItem.Name = txtCalcSymbol.Text;
        }

        private void txtAttParent_TextChanged(object sender, EventArgs e)
        {
            if (m_CurrentItem is ProjectAttribute)
            {
                ((ProjectAttribute)m_CurrentItem).Parent = txtAttParent.Text;
                if (string.IsNullOrEmpty(txtAttParent.Text) || !GetAvailableParents().Contains(txtAttParent.Text))
                {
                    m_CurrentItem.Valid = false;
                    m_CurrentItem.StatusDetails = "Parent Not Found.";
                }
                else
                    m_CurrentItem.Valid = true;
            }
        }

        private void txtAttDimension_TextChanged(object sender, EventArgs e)
        {
            if (m_CurrentItem is ProjectAttribute)
                ((ProjectAttribute)m_CurrentItem).Cnv = txtAttDimension.Text;
        }

        private void contextMenuStrip1_Opening(object sender, CancelEventArgs e)
        {
            menuRemove.Enabled = lstProjectVector.SelectedItems.Count > 0;
        }

        private void menuAdd_Click(object sender, EventArgs e)
        {
            DoAddInterface();
        }

        private void sortToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Sort();
        }
        #endregion Event Handling


    }

    public class PVIOrderer
    {
        public bool m_bByPhase;
        public bool m_bPreservePageAndLabelLocations;
        public bool m_bPreserveCalculationLocations;
        public bool m_bAscending;
        public List<Phase> m_PhaseOrder = new List<Phase>();
        static List<Type> SpecieOrder = new List<Type>(new Type[] {
            typeof(ProjectSum),
            typeof(ProjectCalculation),
            typeof(ProjectAttribute),
            typeof(ProjectText),
            typeof(ProjectSpecie),
            typeof(ProjectPage)});

        public PVIOrderer()
        {
        }

        /* Logic: We are going to look through the source list, and break it into sub lists.
         * We will start with a list of preserved items. And the reordered items will be the item
         * which exactly follows the corresponding list 
         * */
        public List<ProjectVectorItem> Sort(List<ProjectVectorItem> Source)
        {
            //New Code:
            ProjectSpecie.CachePhase = true;
            List<ProjectVectorItem> PreservedItems = new List<ProjectVectorItem>();
            List<List<ProjectVectorItem>> ReOrderedItems = new List<List<ProjectVectorItem>>();

            List<ProjectVectorItem> currentReorder = new List<ProjectVectorItem>();
            ReOrderedItems.Add(currentReorder);
            foreach (ProjectVectorItem pvi in Source)
            {
                if (m_bByPhase && !IsPhaseLabel(pvi.Value) && pvi is ProjectText)
                    continue;
                else if ((m_bPreservePageAndLabelLocations &&
                    (pvi is ProjectText || pvi is ProjectPage)) ||
                    (m_bPreserveCalculationLocations &&
                    (pvi is ProjectCalculation)))
                {
                    PreservedItems.Add(pvi);
                    ReOrderedItems.Add(currentReorder = new List<ProjectVectorItem>());
                }
                else
                    currentReorder.Add(pvi);
            }
            ProjectSpecie.SortByPhase = m_bByPhase;
            foreach (List<ProjectVectorItem> lst in ReOrderedItems)
                lst.Sort(new Comparison<ProjectVectorItem>(SimpleComparison));

            List<ProjectVectorItem> intermediate = new List<ProjectVectorItem>(Source.Count);
            for (int i = 0; i < PreservedItems.Count; i++)
            {
                intermediate.AddRange(ReOrderedItems[i]);
                intermediate.Add(PreservedItems[i]);
            }
            intermediate.AddRange(ReOrderedItems[ReOrderedItems.Count - 1]);

            List<ProjectVectorItem> ret = new List<ProjectVectorItem>();

            Phase lastPhase = (Phase)(-1);
            foreach (ProjectVectorItem pvi in intermediate)
            {
                if (m_bByPhase && pvi is ProjectText &&
                    IsPhaseLabel(pvi.Value))
                    continue; //Don't add these labels.
                if (m_bByPhase && pvi is ProjectSpecie && ((ProjectSpecie)pvi).Phase != lastPhase)
                {
                    lastPhase = ((ProjectSpecie)pvi).Phase;
                    ProjectText txt = new ProjectText();
                    txt.Value = lastPhase.ToString();
                    ret.Add(txt);
                }
                if (m_bPreservePageAndLabelLocations || !(pvi is ProjectPage))
                    ret.Add(pvi);
            }

            ProjectSpecie.CachePhase = false;

            return ret;
        }

        private bool IsPhaseLabel(string s)
        {
            return (s.ToLower() == "solid" || s.ToLower() == "solids" ||
                    s.ToLower() == "liquid" || s.ToLower() == "liquids" ||
                    s.ToLower() == "gas" || s.ToLower() == "gasses" ||
                    s.ToLower() == "vapour" || s.ToLower() == "vapours" ||
                    s.ToLower() == "vapor" || s.ToLower() == "vapors");
        }

        private int SimpleComparison(ProjectVectorItem i1, ProjectVectorItem i2)
        {
            if (i1.GetType() == i2.GetType())
                return i1.CompareTo(i2);

            return SpecieOrder.IndexOf(i1.GetType()).CompareTo(SpecieOrder.IndexOf(i2.GetType()));
        }

        public PVIOrderer Clone()
        {
            PVIOrderer ret = (PVIOrderer)this.MemberwiseClone();
            ret.m_PhaseOrder = new List<Phase>(this.m_PhaseOrder);
            return ret;
        }
    }

}
