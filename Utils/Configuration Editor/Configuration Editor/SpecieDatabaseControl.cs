using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Text.RegularExpressions;
using System.Globalization;

namespace Configuration_Editor
{
    public partial class SpecieDatabaseControl : UserControl
    {
        public SpecieDatabaseControl()
        {
            InitializeComponent();

            LoadAtomicWts();
            SetFormulaEnables();
        }

        #region Regex's
        public static Regex EquationSplitterRegex = new Regex(
            @"^((""?(?<Equation>[^{}""]+)""?{(?<TMin>[^,{}]*),(?<TMax>[^,{}]*)})*|""?(?<GeneralEquation>[^{}""]+))""?$",
            RegexOptions.IgnorePatternWhitespace | RegexOptions.Compiled | RegexOptions.ExplicitCapture);

        static Regex AtomicWtsRegex = new Regex(@"(?<Symb>[^,\r\n]*),(?<Val>[^,\r\n]*)");

        //Allowed characters: Alphanumerics, _.*+-[]/
        protected static Regex s_AcceptableNameRegex = new Regex(@"^[A-Za-z][\w\[\]\.*+\-_/() ]*$", RegexOptions.Compiled);
        protected static Regex s_AcceptableSymbRegex = new Regex(@"^[\w\[\]\.*+\-_/]*$", RegexOptions.Compiled);

        protected static Regex s_DefinitionRegex = new Regex(@"^
             (((?<Symb>[A-Za-z]+)(?<Count>[0-9]+(/[0-9]+|\.[0-9]+)?)) |
             ((?<CustomSymb>[A-Za-z]+)\((?<Wt>[0-9]+(\.[0-9]+)?)\)))+$", RegexOptions.Compiled | RegexOptions.IgnorePatternWhitespace);

        static Regex CSVRegex = new Regex(@"\s*(?<Quotes>"")?(?<Val>(?(Quotes)[^""]*|[^,]*))(?(Quotes)(?<-Quotes>""))
            (\s*,\s*(?<Quotes>"")?(?<Val>(?(Quotes)[^""]*|[^,]*))(?(Quotes)(?<-Quotes>"")))*", RegexOptions.Compiled | RegexOptions.IgnorePatternWhitespace);
        #endregion Regex's

        #region Variables
        protected string m_StatusMessage = "";
        protected Color m_StatusColour = SystemColors.WindowText;

        string m_CurrentGraphColumn = null;

        DataTable m_SpecieDataTable;

        double m_dCurSpeciesMolWt = double.NaN;
        Dictionary<string, double> AtomicWts = new Dictionary<string, double>();

        Dictionary<string, MultiEquationDataset> m_GraphSeries = new Dictionary<string, MultiEquationDataset>();

        protected string m_sSelectedEquationColumn = "";

        Dictionary<string, Dictionary<string, FunctionValue>> availableDBFuncs = new Dictionary<string, Dictionary<string, FunctionValue>>();
        #endregion Variables

        #region Properties
        public Dictionary<string, FormatterParserProvider> Formatters;

        public string StatusMessage
        {
            get { return m_StatusMessage; }
            set
            {
                if (m_StatusMessage == value)
                    return;
                m_StatusMessage = value;
                if (StatusChanged != null)
                    StatusChanged(this, new EventArgs());
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
                if (StatusChanged != null)
                    StatusChanged(this, new EventArgs());
            }
        }

        public DataTable DataSource
        {
            get { return m_SpecieDataTable; }
            set
            {
                if (value == null)
                    return; //Windows form designer likes to set it to null.
                m_SpecieDataTable = value;
                
                errorProvider1.DataSource = m_SpecieDataTable;
            
                txtName.DataBindings.Add("Text", m_SpecieDataTable, "Name");
                txtSymbol.DataBindings.Add("Text", m_SpecieDataTable, "Compound");
                comboPhase.DataBindings.Add("Text", m_SpecieDataTable, "Phase");

                txtElementalComposition.DataBindings.Add("Text", m_SpecieDataTable, "Definition");
                comboOccurence.DataBindings.Add("Text", m_SpecieDataTable, "Occurence");
                
                Binding MinTempBinding = new Binding("Text", m_SpecieDataTable, "Ts", true);
                MinTempBinding.Format += Formatters["temperature"].Formatter; MinTempBinding.Parse += Formatters["temperature"].Parser;
                txtMinTemperature.DataBindings.Add(MinTempBinding);
                
                Binding MaxTempBinding = new Binding("Text", m_SpecieDataTable, "Te", true);
                MaxTempBinding.Format += Formatters["temperature"].Formatter; MaxTempBinding.Parse += Formatters["temperature"].Parser;
                txtMaxTemperature.DataBindings.Add(MaxTempBinding);
                
                txtSource.DataBindings.Add("Text", m_SpecieDataTable, "Reference");
                txtCheckedBy.DataBindings.Add("Text", m_SpecieDataTable, "Checked");
                txtSolvent.DataBindings.Add("Text", m_SpecieDataTable, "Solvent");

                Binding CritPressBinding = new Binding("Text", m_SpecieDataTable, "Pc", true);
                CritPressBinding.Format += Formatters["pressure"].Formatter; CritPressBinding.Parse += Formatters["pressure"].Parser;
                txtCritPressure.DataBindings.Add(CritPressBinding);

                Binding CritVolBinding = new Binding("Text", m_SpecieDataTable, "Vc", true);
                CritVolBinding.Format += Formatters["volume"].Formatter; CritVolBinding.Parse += Formatters["volume"].Parser;
                txtCritVol.DataBindings.Add(CritVolBinding);

                Binding CritTempBinding = new Binding("Text", m_SpecieDataTable, "Tc", true);
                CritTempBinding.Format += Formatters["temperature"].Formatter; CritTempBinding.Parse += Formatters["temperature"].Parser;
                txtCritTemp.DataBindings.Add(CritTempBinding);

                txtAccentricity.DataBindings.Add("Text", m_SpecieDataTable, "Ac");
                txtPhaseChange.DataBindings.Add("Text", m_SpecieDataTable, "PhaseChange");
                txtDensityCorrection.DataBindings.Add("Text", m_SpecieDataTable, "DensityCorrection");

                graph1.DataBindings.Add("MinXValue", m_SpecieDataTable, "Ts");
                graph1.DataBindings.Add("MaxXValue", m_SpecieDataTable, "Te");
                graph1.Formatters = Formatters;

                this.BindingContext[m_SpecieDataTable].PositionChanged += new EventHandler(SpecieDatabaseControl_PositionChanged);
            }
        }

        public Dictionary<string, BindingSource> UnitBindingSource
        {
            set
            {
                if (value == null) return; 
                lblCritTempUnits.DataBindings.Add("Text", value["temperature"], "Name");
                lblMaxTempUnits.DataBindings.Add("Text", value["temperature"], "Name");
                lblMinTUnits.DataBindings.Add("Text", value["temperature"], "Name");

                lblCritPressUnits.DataBindings.Add("Text", value["pressure"], "Name");
                lblCritVolUnits.DataBindings.Add("Text", value["volume"], "Name");
            }
        }
        #endregion Properties

        #region Public Functions
        public void LoadDBIni(Dictionary<string, Dictionary<string, string>> data)
        {
            tlpTempDependantRadios.RowCount = 0;
            foreach (KeyValuePair<string, Dictionary<string, string>> kvp1 in data)
            {
                if (!availableDBFuncs.ContainsKey(kvp1.Key))
                    availableDBFuncs[kvp1.Key] = new Dictionary<string, FunctionValue>();

                MultiEquationDataset graphSeries = new MultiEquationDataset();
                Color c;
                try
                {
                    c = Color.FromName(kvp1.Value["colour"].Trim('"'));
                }
                catch
                {
                    try
                    {
                        c = Color.FromArgb(int.Parse(kvp1.Value["colour"].Trim('"'), NumberStyles.HexNumber));
                    }
                    catch (Exception ex)
                    {
                        throw new Exception("Unable to parse colour associated with " + kvp1.Key, ex); 
                    }
                }

                //Read in meta data
                graphSeries.DefaultColour = c;
                graphSeries.Logarithmic = kvp1.Value.ContainsKey("logarithmic") && kvp1.Value["logarithmic"] != "0";
                graphSeries.Name = kvp1.Value.ContainsKey("friendlyname") ? kvp1.Value["friendlyname"].Trim('"') : kvp1.Key;
                if (kvp1.Value.ContainsKey("dimension"))
                    graphSeries.Units = kvp1.Value["dimension"].Trim('"');

                RadioButton rb = new RadioButton();
                rb.Name = kvp1.Key;
                graphSeries.Name = rb.Text = kvp1.Value.ContainsKey("friendlyname") ? kvp1.Value["friendlyname"].Trim('"') : kvp1.Key;
                rb.ForeColor = c;
                rb.CheckedChanged += new EventHandler(radioEntropy_CheckedChanged);
                rb.Tag = kvp1.Key;
                //tlpTempDependantRadios.RowCount++;
                //tlpTempDependantRadios.RowStyles[tlpTempDependantRadios.RowCount - 1].SizeType = SizeType.AutoSize;
                tlpTempDependantRadios.Controls.Add(rb);
                rb.Dock = DockStyle.Top;

                m_GraphSeries.Add(kvp1.Key, graphSeries);
                graph1.AddDataset(graphSeries);

                //Read in available equations.
                foreach (KeyValuePair<string, string> kvp2 in kvp1.Value)
                    if (kvp2.Key.StartsWith("eqn"))
                    {
                        Match m = CSVRegex.Match(kvp2.Value);
                        if (m.Groups["Val"].Captures.Count < 3)
                            continue;
                        List<double> defaults = new List<double>();
                        double temp;
                        for (int i = 3; i < m.Groups["Val"].Captures.Count; i++)
                            if (double.TryParse(m.Groups["Val"].Captures[i].Value, out temp))
                                defaults.Add(temp);

                        FunctionValue val = new EquationFunctionValue(m.Groups["Val"].Captures[0].Value, m.Groups["Val"].Captures[2].Value, defaults.ToArray());
                        availableDBFuncs[kvp1.Key].Add(val.Name, val);
                    }
            }
        }

        public void DatabaseChanged()
        {
            RebuildPhaseItems();
        }
        #endregion Public Functions

        #region Protected Functions
        protected void RebuildPhaseItems()
        {
            comboPhase.Items.Clear();
            foreach (DataRow r in m_SpecieDataTable.Rows)
                if (!comboPhase.Items.Contains(r["Phase"]))
                    comboPhase.Items.Add(r["Phase"]);
        }

        protected void LoadAtomicWts()
        {
            for (Match m = AtomicWtsRegex.Match(Configuration_Editor.Properties.Resources.AtmoicWts); m.Success; m = m.NextMatch())
            {
                AtomicWts[m.Groups["Symb"].Value] = double.Parse(m.Groups["Val"].Value);
            }
        }

        protected void LoadEquationsIntoGraph()
        {
            foreach (string s in m_GraphSeries.Keys)
            {
                LoadEquationsIntoGraph(s);
            }
        }

        protected void LoadEquationsIntoGraph(string col)
        {
            try
            {
                object o = m_SpecieDataTable.Rows[this.BindingContext[m_SpecieDataTable].Position][col];
                MultiEquationDataset series = m_GraphSeries[col];
                string s = "";
                if ((o is string))
                    s = (string)o;
                Match m = EquationSplitterRegex.Match(s);
                series.ClearSeries();
                if (m.Groups["GeneralEquation"].Success)
                    series.BackupSeries = new EquationGraphSeries(CreateEquationFrag(m.Groups["GeneralEquation"].Value, col), "T");

                for (int i = 0; i < m.Groups["Equation"].Captures.Count; i++)
                {
                    double TMin; double.TryParse(m.Groups["TMin"].Captures[i].Value, out TMin);
                    double TMax; double.TryParse(m.Groups["TMax"].Captures[i].Value, out TMax);

                    EquationGraphSeries EGS = new EquationGraphSeries(CreateEquationFrag(m.Groups["Equation"].Captures[i].Value, col), "T", TMin, TMax);
                    series.AddSeries(EGS);
                }
                errorProvider1.SetError(tlpTempDependantRadios.Controls[col], "");
            }
            catch (Exception ex)
            {
                errorProvider1.SetError(tlpTempDependantRadios.Controls[col], ex.Message);
                errorProvider1.SetIconPadding(tlpTempDependantRadios.Controls[col], -errorProvider1.Icon.Width);
            }
        }

        protected Dictionary<Pair<float>, string> SeperateVariousTemps(string s, out bool single)
        {
            Match m = EquationSplitterRegex.Match(s);
            Dictionary<Pair<float>, string> ret = new Dictionary<Pair<float>, string>();
            if (m.Groups["GeneralEquation"].Success || !m.Success)
            {
                single = true;
                ret.Add(new Pair<float>(0, 0), s);
            }
            else
            {
                single = false;
                for (int i = 0; i < m.Groups["Equation"].Captures.Count; i++)
                {
                    float TMin; float.TryParse(m.Groups["TMin"].Captures[i].Value, out TMin);
                    float TMax; float.TryParse(m.Groups["TMax"].Captures[i].Value, out TMax);
                    ret.Add(new Pair<float>(TMin, TMax), m.Groups["Equation"].Captures[i].Value);
                }
            }
            return ret;
        }

        protected EquationFragment CreateEquationFrag(string equation, string col)
        {
            Dictionary<string, FunctionValue> funcs = availableDBFuncs.ContainsKey(col.ToLower()) ? availableDBFuncs[col.ToLower()] : null;
            EquationFragment frag = EquationFragment.Parse(equation, funcs);
            frag.VariableValues.Add("T", 0);
            frag.VariableValues.Add("MolWt", m_dCurSpeciesMolWt);
            if (!frag.CanEvaluate())
                frag = EquationFragment.Parse("0", null);
            return frag;
        }

        protected void LoadEquationsIntoTextboxes()
        {
            if (!string.IsNullOrEmpty(m_CurrentGraphColumn))
                LoadEquationsIntoTextboxes(m_CurrentGraphColumn);
        }

        protected void LoadEquationsIntoTextboxes(string column)
        {
            string s = "";
            try
            {
                object o = m_SpecieDataTable.Rows[this.BindingContext[m_SpecieDataTable].Position][column];
                if (o is string)
                {
                    s = (string)m_SpecieDataTable.Rows[this.BindingContext[m_SpecieDataTable].Position][column];
                    Match m = EquationSplitterRegex.Match(s);
                    int start = 0;
                    if (m.Groups["GeneralEquation"].Success)
                    {
                        txtFormula0.Text = m.Groups["GeneralEquation"].Value; start++;
                        txtMinTemp0.Text = txtMaxTemp0.Text = "*";
                    }
                    int end = m.Groups["Equation"].Captures.Count + start < 4 ? m.Groups["Equation"].Captures.Count + start : 4;
                    for (int i = start; i < end; i++)
                    {
                        tlpEquations.Controls["txtFormula" + i].Text = m.Groups["Equation"].Captures[i - start].Value;
                        tlpEquations.Controls["txtMinTemp" + i].Text = m.Groups["TMin"].Captures[i - start].Value;
                        tlpEquations.Controls["txtMaxTemp" + i].Text = m.Groups["TMax"].Captures[i - start].Value;
                    }
                    for (int i = end; i < 4; i++)
                    {

                        tlpEquations.Controls["txtFormula" + i].Text = "";
                        tlpEquations.Controls["txtMinTemp" + i].Text = "";
                        tlpEquations.Controls["txtMaxTemp" + i].Text = "";
                    }
                }
                else
                    for (int i = 0; i < 4; i++)
                    {
                        tlpEquations.Controls["txtFormula" + i].Text = tlpEquations.Controls["txtMinTemp" + i].Text = tlpEquations.Controls["txtMaxTemp" + i].Text = "";
                    }
            }
            catch
            {
                for (int i = 0; i < 4; i++)
                {
                    tlpEquations.Controls["txtFormula" + i].Text = tlpEquations.Controls["txtMinTemp" + i].Text = tlpEquations.Controls["txtMaxTemp" + i].Text = "";
                }
            }
        }

        protected string CreateDBString()
        {
            StringBuilder ret = new StringBuilder();

            bool MultipleEquations = false;
            for (int i = 1; i < 4; i++)
                if (!string.IsNullOrEmpty(tlpEquations.Controls["TxtFormula" + i].Text.Trim()))
                    MultipleEquations = true;

            ret.Append(txtFormula0.Text);
            if (MultipleEquations)
            {
                ret.Append("{" + txtMinTemp0.Text + "," + txtMaxTemp0.Text + "}");
                for (int i = 1; i < 4; i++)
                {
                    if (!string.IsNullOrEmpty(tlpEquations.Controls["TxtFormula" + i].Text.Trim()))
                        ret.Append(tlpEquations.Controls["TxtFormula" + i].Text.Trim() +
                            "{" + tlpEquations.Controls["TxtMinTemp" + i].Text + "," + tlpEquations.Controls["TxtMaxTemp" + i].Text + "}");
                }
            }
            return ret.ToString();
        }

        protected void SetFormulaEnables()
        {
            int c = 3;
            while (c >= 0 && string.IsNullOrEmpty(tlpEquations.Controls["TxtFormula" + c].Text.Trim())
                && string.IsNullOrEmpty(tlpEquations.Controls["TxtMinTemp" + c].Text.Trim())
                && string.IsNullOrEmpty(tlpEquations.Controls["TxtMaxTemp" + c].Text.Trim()))
                c--;
            if (c == 3) c = 2;
            for (int i = 0; i < c + 2; i++)
            {
                tlpEquations.Controls["TxtFormula" + i].Enabled = true;
                tlpEquations.Controls["TxtMinTemp" + i].Enabled = true;
                tlpEquations.Controls["TxtMaxTemp" + i].Enabled = true;
            }
            for (int i = c + 2; i < 4; i++)
            {
                tlpEquations.Controls["TxtFormula" + i].Enabled = false;
                tlpEquations.Controls["TxtMinTemp" + i].Enabled = false;
                tlpEquations.Controls["TxtMaxTemp" + i].Enabled = false;
            }
        }

        protected void RecalculateMolWt()
        {
            try
            {
                Match m = s_DefinitionRegex.Match(txtElementalComposition.Text);
                if (!m.Success)
                {
                    m_dCurSpeciesMolWt = double.NaN; return;
                }
                double temp = 0;
                for (int i = 0; i < m.Groups["Symb"].Captures.Count; i++)
                {
                    if (!AtomicWts.ContainsKey(m.Groups["Symb"].Captures[i].Value))
                    {
                        m_dCurSpeciesMolWt = double.NaN; return;
                    }
                    double count = 1;
                    if (m.Groups["Count"].Captures[i].Length > 0)
                    {
                        if (!double.TryParse(m.Groups["Count"].Captures[i].Value, out count))
                            count = FracParse(m.Groups["Count"].Captures[i].Value);
                    }
                    temp += AtomicWts[m.Groups["Symb"].Captures[i].Value] * count;
                }
                for (int i = 0; i < m.Groups["Wt"].Captures.Count; i++)
                    temp += double.Parse(m.Groups["Wt"].Captures[i].Value);
                m_dCurSpeciesMolWt = temp;
            }
            catch { }
            UpdateGraphValues();
        }

        protected double FracParse(string s)
        {
            string[] subs = s.Split('/');
            return double.Parse(subs[0]) / double.Parse(subs[1]);
        }

        protected void UpdateGraphValues()
        {
            UpdateGraphValues(graph1.Series);
        }

        protected void UpdateGraphValues(ICollection<GraphSeries> series)
        {
            foreach (GraphSeries gs in series)
                if (gs is MultiEquationDataset)
                    UpdateGraphValues(((MultiEquationDataset)gs).SubSeries);
                else if (gs is EquationGraphSeries)
                    ((EquationGraphSeries)gs).SetVariable("MolWt", m_dCurSpeciesMolWt);
            graph1.Invalidate();
        }

        protected void UpdateDBLVIs()
        {
            if (UpdateLVIsRequired != null)
                UpdateLVIsRequired(this, new EventArgs());
        }
        #endregion Protected Functions

        #region Events
        public event EventHandler StatusChanged;

        public event EventHandler UpdateLVIsRequired;
        #endregion Events

        #region Event Handling
        void SpecieDatabaseControl_PositionChanged(object sender, EventArgs e)
        {
            LoadEquationsIntoGraph();
            LoadEquationsIntoTextboxes();
            RecalculateMolWt();
        }

        private void radioEntropy_CheckedChanged(object sender, EventArgs e)
        {
            RadioButton rSender = sender as RadioButton;
            m_CurrentGraphColumn = (string)rSender.Tag;
            m_GraphSeries[m_CurrentGraphColumn].Selected = rSender.Checked;
            if (rSender.Checked)
                LoadEquationsIntoTextboxes(m_CurrentGraphColumn);
            tlpEquations.Enabled = true;
        }

        private void txtFormula0_Validated(object sender, EventArgs e)
        {
            if (!string.IsNullOrEmpty(m_CurrentGraphColumn))
            {
                m_SpecieDataTable.Rows[this.BindingContext[m_SpecieDataTable].Position][m_CurrentGraphColumn] = CreateDBString();
                LoadEquationsIntoGraph(m_CurrentGraphColumn);
            }
        }

        private void txtName_Validating(object sender, CancelEventArgs e)
        {
            e.Cancel = !s_AcceptableNameRegex.Match(txtName.Text).Success;
            if (e.Cancel)
                errorProvider1.SetError(txtName, "Invalid Characters");
            else
                errorProvider1.SetError(txtName, "");
        }

        private void txtSymbol_Validating(object sender, CancelEventArgs e)
        {
            e.Cancel = !s_AcceptableSymbRegex.Match(txtSymbol.Text).Success;
            if (e.Cancel)
                errorProvider1.SetError(txtSymbol, "Invalid Characters");
            else
                errorProvider1.SetError(txtSymbol, "");
        }

        private void comboPhase_Validated(object sender, EventArgs e)
        {
            //Rebuild the dropdown list:
            RebuildPhaseItems();
            UpdateDBLVIs();
        }

        private void txtSymbol_Validated(object sender, EventArgs e)
        {
            UpdateDBLVIs();
        }

        private void txtName_Validated(object sender, EventArgs e)
        {
            UpdateDBLVIs();
        }

        private void txtElementalComposition_Validating(object sender, CancelEventArgs e)
        {
            Match m = s_DefinitionRegex.Match(txtElementalComposition.Text);
            if (!m.Success)
            {
                e.Cancel = true;
                errorProvider1.SetError(txtElementalComposition, "Unable To Parse Definition");
            }
            else
            {
                bool ok = true;
                foreach (Capture c in m.Groups["Symb"].Captures)
                    if (!AtomicWts.ContainsKey(c.Value))
                    {
                        errorProvider1.SetError(txtElementalComposition, "Unknown symbol " + c.Value);
                        ok = false;
                        break;
                    }
                if (ok)
                    errorProvider1.SetError(txtElementalComposition, "");
            }
        }

        private void txtElementalComposition_Validated(object sender, EventArgs e)
        {
            RecalculateMolWt();
        }

        private void txtMinTemp3_Validating(object sender, CancelEventArgs e)
        {
            double d;
            bool ok = double.TryParse(((TextBox)sender).Text, out d);
            if (!ok)
                ok = ((TextBox)sender).Text == "*" || string.IsNullOrEmpty(((TextBox)sender).Text);
            if (!ok)
            {
                e.Cancel = true;
                errorProvider1.SetError((Control)sender, "Temperature Range must be a number.");
            }
            else
                errorProvider1.SetError((Control)sender, "");
        }

        private void txtFormula0_Validating(object sender, CancelEventArgs e)
        {
            TextBox tSender = (TextBox)sender;
            tSender.Text = tSender.Text.Replace("{", "").Replace("}", ""); //Curly braces will interfere with when we try to reparse the equation.
        }

        private void txtFormula0_TextChanged(object sender, EventArgs e)
        {
            SetFormulaEnables();
        }
        #endregion Event Handling
    }
}
