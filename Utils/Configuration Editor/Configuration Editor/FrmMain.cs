using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.OleDb;
using System.Collections;
using System.Text.RegularExpressions;
using System.IO;
using System.Data.Common;

namespace Configuration_Editor
{
    public partial class FrmMain : Form
    {
        public static string FileVersion = "114";

        #region Variables
        ProjectVectorItem m_CurrentItem = null;
        FrmAddSelector m_AddSelector = new FrmAddSelector();
        FrmSortOptions m_SortOptions = new FrmSortOptions();
        PVIOrderer m_Sorter = new PVIOrderer();

        DataTable m_SpecieDataTable = new DataTable("Species");
        Dictionary<string, MultiEquationDataset> m_GraphSeries = new Dictionary<string, MultiEquationDataset>();

        FileStream configFile;
        #endregion Variables
        
        #region Constructors
        public FrmMain()
        {
            InitializeComponent();
            SetupSpecies();
            SetupMisc();
            SetupSpDbEditor();
        }

        private void SetupSpDbEditor()
        {
            #region Database stuff:
            m_SpecieDataTable = new DataTable("Species");
            m_SpecieDataTable.Columns.Add("Name", typeof(String));
            m_SpecieDataTable.Columns.Add("Compound", typeof(String));
            m_SpecieDataTable.Columns.Add("Phase", typeof(String));
            m_SpecieDataTable.Columns.Add("Definition", typeof(String));
            m_SpecieDataTable.Columns.Add("Occurence", typeof(String));
            m_SpecieDataTable.Columns.Add("Checked", typeof(String));
            m_SpecieDataTable.Columns.Add("Ts", typeof(Single));
            m_SpecieDataTable.Columns.Add("Te", typeof(Single));
            m_SpecieDataTable.Columns.Add("Density", typeof(String));
            m_SpecieDataTable.Columns.Add("Solvent", typeof(String));
            m_SpecieDataTable.Columns.Add("DensityCorrection", typeof(String));
            m_SpecieDataTable.Columns.Add("VPCorrection", typeof(String));
            m_SpecieDataTable.Columns.Add("Hf25", typeof(String));
            m_SpecieDataTable.Columns.Add("S25", typeof(String));
            m_SpecieDataTable.Columns.Add("Cp", typeof(String));
            m_SpecieDataTable.Columns.Add("VP", typeof(String));
            m_SpecieDataTable.Columns.Add("Pc", typeof(String));
            m_SpecieDataTable.Columns.Add("Tc", typeof(String));
            m_SpecieDataTable.Columns.Add("Vc", typeof(String));
            m_SpecieDataTable.Columns.Add("Ac", typeof(String));
            m_SpecieDataTable.Columns.Add("Dissociation", typeof(String));
            m_SpecieDataTable.Columns.Add("Reference", typeof(String));
            #endregion Database stuff

            foreach (Control c in pnlTempDependantRadios.Controls)
                if (c is RadioButton)
                {
                    MultiEquationDataset equations = new MultiEquationDataset();
                    equations.DefaultColour = c.ForeColor;
                    m_GraphSeries.Add((string)c.Tag, equations);
                    graph1.AddDataset(equations);
                }
            
            txtName.DataBindings.Add("Text", m_SpecieDataTable, "Name");
            txtSymbol.DataBindings.Add("Text", m_SpecieDataTable, "Compound");
            txtPhase.DataBindings.Add("Text", m_SpecieDataTable, "Phase");

            txtElementalComposition.DataBindings.Add("Text", m_SpecieDataTable, "Definition");
            comboOccurence.DataBindings.Add("Text", m_SpecieDataTable, "Occurence");
            txtMinTemperature.DataBindings.Add("Text", m_SpecieDataTable, "Ts", true);
            txtMaxTemperature.DataBindings.Add("Text", m_SpecieDataTable, "Te", true);
            txtSource.DataBindings.Add("Text", m_SpecieDataTable, "Reference");
            txtCheckedBy.DataBindings.Add("Text", m_SpecieDataTable, "Checked");
            txtSolvent.DataBindings.Add("Text", m_SpecieDataTable, "Solvent");
            txtCritPressure.DataBindings.Add("Text", m_SpecieDataTable, "Pc");
            txtCritVol.DataBindings.Add("Text", m_SpecieDataTable, "Vc");
            txtCritTemp.DataBindings.Add("Text", m_SpecieDataTable, "Tc");
            txtAccentricity.DataBindings.Add("Text", m_SpecieDataTable, "Ac");
        }

        private void SetupMisc()
        {
            comboDefaultSolution.Items.AddRange(new object[] { "ProBal", "Dynamic" });
            comboDefaultSolution.SelectedItem = "ProBal";

            object[] SurgeItems = new object[] { "Direct", "Buffered", "Inline" };
            comboProBalSurge.Items.AddRange(SurgeItems);
            comboDynamicSurge.Items.AddRange(SurgeItems);
            comboMaxCompSurge.Items.AddRange(SurgeItems);
            comboProBalSurge.SelectedItem = "Direct";
            comboDynamicSurge.SelectedItem = "Buffered";
            comboMaxCompSurge.SelectedItem = "Buffered";

            object[] FCItems = new object[] { "Transfer", "Simple", "Linear" };
            comboProbalFlow.Items.AddRange(FCItems);
            comboDynamicFlow.Items.AddRange(FCItems);
            comboMaxCompFlow.Items.AddRange(FCItems);
            comboProbalFlow.SelectedItem = "Transfer";
            comboDynamicFlow.SelectedItem = "Simple";
            comboMaxCompFlow.SelectedItem = "Linear";

            object[] HCItems = new object[] { "None", "Reduced", "Rigorous" };
            comboProBalHeat.Items.AddRange(HCItems);
            comboDynamicHeat.Items.AddRange(HCItems);
            comboMaxCompHeat.Items.AddRange(HCItems);
            comboProBalHeat.SelectedItem = comboDynamicHeat.SelectedItem = comboMaxCompHeat.SelectedItem = "Rigorous";

            txtStT.Text = "20"; txtNT.Text = "0"; txtMinT.Text = "0"; txtMaxT.Text = "1000";
            txtStP.Text = "101.325"; txtNP.Text = "101.325"; txtMinP.Text = "50"; txtMaxP.Text = "350";

            comboSpecieModel.Items.Add("Standard");
            comboSpecieModel.SelectedItem = "Standard";
        }

        private void SetupSpecies()
        {
            this.SuspendLayout();
            pnlActive.Size = pnlBlank.Size;
            foreach (TabPage tab in tcIDE.TabPages)
            {
                foreach (Control c in tab.Controls)
                {
                    c.Parent = tcIDE.Parent;
                    c.Visible = false;
                    Console.WriteLine(c.ToString() + " Moved to " + tcIDE.Parent.ToString());
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

            m_SortOptions.Sorter = m_Sorter;

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

        #region File Operations
        protected void SaveAs()
        {
            try
            {
                if (dlgSaveConfig.ShowDialog(this) != DialogResult.OK)
                    return;
                if (configFile != null) try { configFile.Close(); }
                    catch { }
                configFile = new FileStream(dlgSaveConfig.FileName, FileMode.Create, FileAccess.ReadWrite, FileShare.Read);
                if (configFile != null)
                    Save();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        protected void Save()
        {
            if (configFile == null)
                SaveAs();

            StreamWriter sw = null;
            try
            {
                configFile.SetLength(0);
                sw = new StreamWriter(configFile);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                return;
            }

            try
            {
                sw.WriteLine("[General]");
                sw.WriteLine("PrjFileVersion=" + FileVersion);
                sw.WriteLine("TagMonitor=0"); //TODO: Check what is happening
                sw.WriteLine("UseStdFns4H2O=1"); //TODO: Check what this is.
                sw.WriteLine("Std_Temp=" + txtStT.Text);
                sw.WriteLine("Std_Press=" + txtStP.Text);
                sw.WriteLine("Norm_Temp=" + txtNT.Text);
                sw.WriteLine("Norm_Press=" + txtNP.Text);
                sw.WriteLine("Minimum_Temp=" + txtMinT.Text);
                sw.WriteLine("Minimum_Press=" + txtMinP.Text);
                sw.WriteLine("Maximum_Temp=" + txtMaxT.Text);
                sw.WriteLine("Maximum_Press=" + txtMaxP.Text);
                sw.WriteLine("Atmospheric_Press=101.287, -11.83e-3, 0.4793e-6"); //TODO: Check what this is
                sw.WriteLine("Default_SpModel=" + comboSpecieModel.Text);
                sw.WriteLine("H2O_As_Aqueous=" + (chkH2OAqueous.Checked ? "1" : "0"));

                sw.WriteLine("[ModelDLLs]");
                //TODO: Model DLLs

                sw.WriteLine("[HelpDLLs]");
                //TODO: Find out what these are, then do 'em.

                sw.WriteLine("[Species]");
                int i = 0;
                foreach (ListViewItem lvi in lstProjectVector.Items)
                {
                    try
                    {
                        sw.WriteLine("S" + i.ToString("0000") + "=" + ((ProjectVectorItem)lvi.Tag).ToSaveString());
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show(ex.ToString());
                    }
                }

                sw.WriteLine("[PhaseNames]");
                //TODO: Phase Names. Actually, what the hell are these things?

                sw.WriteLine("[Selectable]");
                //TODO: Models.

                sw.WriteLine("[Modes]");
                sw.WriteLine("Default_NetMode=" + comboDefaultSolution.Text);
                sw.WriteLine("ProBal_Allowed=" + (chkProbalAllowed.Checked ? "1" : "0"));
                sw.WriteLine("Probal_NodeMode=" + comboProBalSurge.Text);
                sw.WriteLine("Probal_FlowMode=" + comboProbalFlow.Text);
                sw.WriteLine("Probal_HeatMode=" + comboProBalHeat.Text);

                sw.WriteLine("Dynamic_Allowed=" + (chkDynamicAllowed.Checked ? "1" : "0"));
                sw.WriteLine("Dynamic_NodeMode=" + comboDynamicSurge.Text);
                sw.WriteLine("Dynamic_FlowMode=" + comboDynamicFlow.Text);
                sw.WriteLine("Dynamic_HeatMode=" + comboDynamicHeat.Text);

                sw.WriteLine("Maximum_NodeMode=" + comboMaxCompSurge.Text);
                sw.WriteLine("Maximum_FlowMode=" + comboMaxCompFlow.Text);
                sw.WriteLine("Maximum_HeatMode=" + comboMaxCompHeat.Text);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            finally
            {
                if (sw != null)
                    try
                    {
                        sw.Flush();
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show(ex.Message);
                    }
            }
        }

        protected void LoadFile(string filename)
        {
            if (configFile != null)
                try { configFile.Close(); }
                catch { }

            try
            {
                lstProjectVector.Items.Clear();
                configFile = new FileStream(filename, FileMode.Open, FileAccess.ReadWrite, FileShare.Read, 8192, FileOptions.SequentialScan);

                string contents = new StreamReader(configFile).ReadToEnd();

                Dictionary<string, Dictionary<string, string>> data = ReadIni(contents);
                if (data.ContainsKey("general"))
                {
                    Dictionary<string, string> General = data["general"];
                    txtStT.Text = General["std_temp"];
                    txtStP.Text = General["std_press"];
                    txtNT.Text = General["norm_temp"];
                    txtNP.Text = General["norm_press"];
                    txtMinT.Text = General["minimum_temp"];
                    txtMaxT.Text = General["maximum_temp"];
                    txtMinP.Text = General["minimum_press"];
                    txtMaxP.Text = General["maximum_press"];
                    comboSpecieModel.Text = General["default_spmodel"];
                    chkH2OAqueous.Checked = General.ContainsKey("h2o_as_aqueous") && General["h2o_as_aqueous"] != "0";
                }

                if (data.ContainsKey("modeldlls"))
                {
                    //TODO: Model DLLs
                }

                if (data.ContainsKey("helpdlls"))
                {
                    //TODO: Help DLLs
                }

                if (data.ContainsKey("species"))
                {
                    lstProjectVector.BeginUpdate();
                    foreach (string s in data["species"].Values)
                        lstProjectVector.Items.Add(ProjectVectorItem.Parse(s, m_SpecieDataTable).LVI);
                    lstProjectVector.EndUpdate();
                    AddSumItems();
                }

                if (data.ContainsKey("phasenames"))
                {
                    //TODO: Phase Names
                }

                if (data.ContainsKey("selectable"))
                {
                    //TODO: Selectable
                }

                if (data.ContainsKey("modes"))
                {
                    Dictionary<string, string> Modes = data["modes"];
                    comboDefaultSolution.Text = Modes["default_netmode"];
                    chkProbalAllowed.Checked = (!Modes.ContainsKey("probal_allowed")) || Modes["probal_allowed"] != "0";
                    comboProBalSurge.Text = Modes["probal_nodemode"];
                    comboProbalFlow.Text = Modes["probal_flowmode"];
                    comboProBalHeat.Text = Modes["probal_heatmode"];

                    chkDynamicAllowed.Checked = (!Modes.ContainsKey("dynamic_allowed")) || Modes["dynamic_allowed"] != "0";
                    comboDynamicSurge.Text = Modes["dynamic_nodemode"];
                    comboDynamicFlow.Text = Modes["dynamic_flowmode"];
                    comboDynamicHeat.Text = Modes["dynamic_heatmode"];

                    comboMaxCompSurge.Text = Modes["maximum_nodemode"];
                    comboMaxCompFlow.Text = Modes["maximum_flowmode"];
                    comboMaxCompHeat.Text = Modes["maximum_heatmode"];
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        protected static Regex iniRegex1 = new Regex(@"(^\[(?<Title>\w*)\](?<Values>.*?))*\Z", 
            RegexOptions.Singleline | RegexOptions.Compiled | RegexOptions.Multiline); //This'll be kinda slow with that .*?
        protected static Regex iniRegex2 = new Regex(@"^(?<Variable>[^\s=]*)=(?<Value>.*)",
            RegexOptions.Multiline | RegexOptions.Compiled);

        //Work in pure lowercase.
        protected Dictionary<string, Dictionary<string, string>> ReadIni(string contents)
        {
            Match tier1Match = iniRegex1.Match(contents);
            Dictionary<string, Dictionary<string, string>> ret = new Dictionary<string,Dictionary<string,string>>();
            for (int i = 0; i < tier1Match.Groups["Title"].Captures.Count; i++)
            {
                if (!ret.ContainsKey(tier1Match.Groups["Title"].Captures[i].Value.ToLower()))
                    ret.Add(tier1Match.Groups["Title"].Captures[i].Value.ToLower(), new Dictionary<string, string>());
                Dictionary<string, string> current = ret[tier1Match.Groups["Title"].Captures[i].Value.ToLower()];

                for (Match m = iniRegex2.Match(tier1Match.Groups["Values"].Captures[i].Value); m.Success; m = m.NextMatch())
                    current[m.Groups["Variable"].Value.ToLower()] = m.Groups["Value"].Value.Trim();
            }
            return ret;
        }
        #endregion File Operations

        #region Specie database stuff
        protected void OpenDatabase(string filename)
        {
            //Program.SpecieDatabase = new Database(filename, "Species");
            OleDbConnection conn = new OleDbConnection(@"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + filename);
            //conn.Open();

            OleDbDataAdapter da = new OleDbDataAdapter("SELECT * FROM Species", conn);
            //da.FillSchema(m_SpecieDataTable, SchemaType.Mapped);

            DataTableMapping mapping = da.TableMappings.Add("Table", "Species");
            mapping.ColumnMappings.Add("Rho", "Density");
            mapping.ColumnMappings.Add("dHf", "Hf25");
            mapping.ColumnMappings.Add("S�298", "S25");

            da.Fill(m_SpecieDataTable);

            MergeRows();

            UpdateFilter();
        }

        protected string m_sSelectedEquationColumn = "Density";
        protected static Regex EquationSplitterRegex = new Regex(
            @"^(((?<Equation>[^{}]+){(?<TMin>\d+(\.\d+)?|\.\d+),(?<TMax>\d+(\.\d+)?|\.\d+)})*|(?<GeneralEquation>[^{}]+))$",
            RegexOptions.IgnorePatternWhitespace | RegexOptions.Compiled | RegexOptions.ExplicitCapture);
        protected void LoadEquationsIntoGraph()
        {
            try
            {
                foreach (string s in m_GraphSeries.Keys)
                {
                    LoadEquationsIntoGraph(s);
                }
            }
            catch { }
        }

        protected Dictionary<Pair<float>, string> SeperateVariousTemps(string s, out bool single)
        {
            Match m = EquationSplitterRegex.Match(s);
            Dictionary<Pair<float>, string> ret = new Dictionary<Pair<float>,string>();
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

        protected void LoadEquationsIntoGraph(string col)
        {
            object o = m_SpecieDataTable.Rows[this.BindingContext[m_SpecieDataTable].Position][col];
            MultiEquationDataset series = m_GraphSeries[col];
            string s = "";
            try { s = (string)o; }
            catch { }
            Match m = EquationSplitterRegex.Match(s);
            series.ClearSeries();
            if (m.Groups["GeneralEquation"].Success)
                series.BackupSeries = new PolyEquationSeries(m.Groups["GeneralEquation"].Value);

            for (int i = 0; i < m.Groups["Equation"].Captures.Count; i++)
            {
                double TMin; double.TryParse(m.Groups["TMin"].Captures[i].Value, out TMin);
                double TMax; double.TryParse(m.Groups["TMax"].Captures[i].Value, out TMax);
                series.AddSeries(new PolyEquationSeries(m.Groups["Equation"].Captures[i].Value, TMin, TMax));
            }
        }

        protected void LoadEquationsIntoTextboxes()
        {
            foreach (Control c in pnlTempDependantRadios.Controls)
                if (c is RadioButton && ((RadioButton)c).Checked)
                    LoadEquationsIntoTextboxes((string)c.Tag);
        }

        protected void LoadEquationsIntoTextboxes(string column)
        {
            string s = "";
            try
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
            
            ret.Append(txtFormula0.Text );
            double tmin, tmax;
            if( double.TryParse(txtMinTemp0.Text, out tmin) && double.TryParse(txtMaxTemp0.Text, out tmax))
            ret.Append( "{" + txtMinTemp0.Text + "," + txtMaxTemp0.Text + "}");
            for (int i = 1; i < 3; i++)
            {
                if (!string.IsNullOrEmpty(tlpEquations.Controls["TxtFormula" + i].Text.Trim()))
                    ret.Append(tlpEquations.Controls["TxtFormula" + i].Text.Trim() +
                        "{" + tlpEquations.Controls["TxtMinTemp" + i].Text + "," + tlpEquations.Controls["TxtMaxTemp" + i].Text + "}");
            }
            return ret.ToString();
        }

        protected void MergeRows()
        {
            DataColumn Ts = m_SpecieDataTable.Columns["Ts"];
            DataColumn Te = m_SpecieDataTable.Columns["Te"];
            Dictionary<string, List<DataRow>> Compounds = new Dictionary<string, List<DataRow>>();
            foreach (DataRow r in m_SpecieDataTable.Rows)
            {
                string symbol = r["Compound"] + "(" + r["Phase"] + ")";
                if (!Compounds.ContainsKey(symbol))
                    Compounds.Add(symbol, new List<DataRow>());
                Compounds[symbol].Add(r);
            }
            foreach (KeyValuePair<string, List<DataRow>> kvp in Compounds)
            {
                //For the sake of efficiency, we will merely edit the first row in place:
                //If there is only one row in the set, we needn't make any changes
                if (kvp.Value.Count == 1) continue;
                DataRow resultantRow = kvp.Value[0];

                #region Figure out if the data is the same in all rows, and change the resultant appropriately
                foreach (DataColumn c in m_SpecieDataTable.Columns)
                {
                    if (c.DataType != typeof(String)) //|| c.ColumnName == "Ts" || c.ColumnName == "Te") //Ts and Te are floats, anyway...
                        continue;
                    Dictionary<string, List<Pair<float>>> values = new Dictionary<string, List<Pair<float>>>(); //s1 will be min, s2 max
                    foreach (DataRow r in kvp.Value)
                        if (!r.IsNull(c))
                        {
                            //We should actually run it through our range seperator first.
                            bool single;
                            Dictionary<Pair<float>, string> seperated = SeperateVariousTemps((string)r[c], out single);
                            if (single)
                            {
                                if (!values.ContainsKey((string)r[c]))
                                    values.Add((string)r[c], new List<Pair<float>>());
                                values[(string)r[c]].Add(new Pair<float>((float)r[Ts], (float)r[Te]));
                            }
                            else
                            {
                                foreach (KeyValuePair<Pair<float>, string> varTemps in seperated)
                                {
                                    if (!values.ContainsKey(varTemps.Value))
                                        values.Add(varTemps.Value, new List<Pair<float>>());
                                    values[varTemps.Value].Add(varTemps.Key);
                                }
                            }
                                
                        }

                    //Join sets:
                    foreach (List<Pair<float>> lst in values.Values)
                        for (int i = lst.Count - 1; i > 0; i--)
                            for (int j = i - 1; j >= 0; i++)
                            {
                                if(lst[i].s1 <= lst[j].s2 && lst[i].s2 >= lst[j].s1)
                                {
                                    lst[j].s1 = Math.Min(lst[i].s1, lst[j].s1);
                                    lst[j].s2 = Math.Max(lst[i].s2, lst[j].s2);
                                    lst.RemoveAt(i);
                                    break;
                                }
                            }

                    bool req = false;
                    foreach (List<Pair<float>> lst in values.Values)
                        if (lst.Count > 1)
                        {
                            req = true;
                            break;
                        }
                    if (values.Count > 1 || req)
                    {
                        List<Pair<float>> sortedValues = new List<Pair<float>>();
                        foreach (List<Pair<float>> lst in values.Values)
                            sortedValues.AddRange(lst);
                        sortedValues.Sort((Comparison<Pair<float>>)delegate(Pair<float> p1, Pair<float> p2) { return p1.s1.CompareTo(p2.s1); });

                        Dictionary<Pair<float>, string> ReverseDictionary = new Dictionary<Pair<float>, string>();
                        foreach (KeyValuePair<string, List<Pair<float>>> kvp2 in values)
                            foreach (Pair<float> p in kvp2.Value)
                                if (!ReverseDictionary.ContainsKey(p))
                                    ReverseDictionary.Add(p, kvp2.Key);
                                else
                                { } //We should log the error, or prompt the user for input.

                        StringBuilder sb = new StringBuilder();
                        foreach (Pair<float> p in sortedValues)
                            sb.Append(ReverseDictionary[p] + "{" + p.s1 + "," + p.s2 + "}");
                        resultantRow[c] = sb.ToString();
                    }
                }
                #endregion Figure out if the data is the same in all rows, and change the resultant appropriately
                
                float minTemp = (float)resultantRow[Ts];
                float maxTemp = (float)resultantRow[Te];
                foreach (DataRow r in kvp.Value)
                {
                    if ((float)r[Ts] < minTemp)
                        minTemp = (float)r[Ts];
                    if ((float)r[Te] > maxTemp)
                        maxTemp = (float)r[Te];
                }
                resultantRow[Ts] = minTemp; resultantRow[Te] = maxTemp;

                foreach (DataRow r in kvp.Value)
                    if (r != resultantRow)
                        m_SpecieDataTable.Rows.Remove(r);
            }
        }
        #endregion Specie DB stuff

        #region Project Vector Stuff
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
                txtAttributeName.Text = item.Value;
                comboAttributeType.SelectedIndex = (int)((ProjectAttribute)item).AttributeType;
            }
            this.ResumeLayout();
        }

        protected void UpdateTemps()
        {
            DataRow r = ((ProjectSpecie)m_CurrentItem).SpDataRow;
            if (r == null) return;
            float Ts = (float)r["Ts"], Te = (float)r["Te"];
            txtDefinedMinTemp.Text = Ts.ToString();
            txtDefinedMaxTemp.Text = Te.ToString();
            numGreaterTempTolerance.Value = (decimal)((ProjectSpecie)m_CurrentItem).ExtraAboveT;
            numLowerTempTolerance.Value = (decimal)((ProjectSpecie)m_CurrentItem).ExtraBelowT;
            txtAllowedMinTemp.Text = ((ProjectSpecie)m_CurrentItem).MinAllowedTemp.ToString();
            txtAllowedMaxTemp.Text = ((ProjectSpecie)m_CurrentItem).MaxAllowedTemp.ToString();
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

        protected void Sort()
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

        protected bool TransferSpecies()
        {
            DataRow[] values = new DataRow[lstDBSpecies.SelectedItems.Count];
            int i = 0;
            foreach (ListViewItem lvi in lstDBSpecies.SelectedItems)
                values[i++] = (DataRow)lvi.Tag;
            
            return AddSpecies(values, -1);
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
        #endregion Project Vector Stuff

        protected void UpdateFilter()
        {
            lstDBSpecies.BeginUpdate();
            lstDBSpecies.Items.Clear();
            if (m_SpecieDataTable != null)
            {
                string[] filterTokens = txtFilter.Text.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
                List<DataRow> toAdd = new List<DataRow>();
                foreach (DataRow r in m_SpecieDataTable.Rows)
                {
                    string symb = (string)r["Compound"] + "(" + (string)r["Phase"] + ")";
                    bool passesFilter = true;
                    foreach (string s in filterTokens)
                        if (!symb.Contains(s))
                            passesFilter = false;
                    if (passesFilter)
                        toAdd.Add(r);
                }
                toAdd.Sort(new Comparison<DataRow>(DataRowComparison));
                foreach (DataRow r in toAdd)
                {
                    string symb = (string)r["Compound"] + "(" + (string)r["Phase"] + ")";
                    ListViewItem lvi = new ListViewItem(new string[] { symb, (string)r["Name"] });
                    lvi.Tag = r;
                    lstDBSpecies.Items.Add(lvi);
                }
            }
            lstDBSpecies.EndUpdate();
        }

        private int DataRowComparison(DataRow r1, DataRow r2)
        {
            if (menuSortSpDBPhase.Checked && (string)r1["Phase"] != (string)r2["Phase"])
                return ((string)r1["Phase"]).CompareTo((string)r2["Phase"]);
            else if (menuSortSpDBAlph.Checked)
                return ((string)r1["Compound"] + "(" + r1["Phase"] + ")").CompareTo((string)r2["Compound"] + "(" + r2["Phase"] + ")");
            else
                return 0;
        }

        private void menuOpenDatabase_Click(object sender, EventArgs e)
        {
            if (dlgOpenDB.ShowDialog() == DialogResult.Cancel)
                return;

            OpenDatabase(dlgOpenDB.FileName);
        }

        private void lstDBSpecies_ItemDrag(object sender, ItemDragEventArgs e)
        {
            SpecieDrag drag = new SpecieDrag(new DataRow[lstDBSpecies.SelectedItems.Count]);
            for (int i = 0; i < lstDBSpecies.SelectedItems.Count; i++)
                drag.Value[i] = (DataRow)lstDBSpecies.SelectedItems[i].Tag;
            DoDragDrop(drag, DragDropEffects.Link);
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

        private void btnAdd_Click(object sender, EventArgs e)
        {
            if (lstDBSpecies.Focused && TransferSpecies())
                return;
            if (m_AddSelector.ShowDialog() == DialogResult.Cancel) return;
            lstProjectVector.SelectedItems.Clear();
            ProjectVectorItem item = null;
            if (m_AddSelector.radioAttribute.Checked)
                item = new ProjectAttribute();
            else if (m_AddSelector.radioCalculation.Checked)
                item = new ProjectCalculation();
            else if (m_AddSelector.radioLabel.Checked)
                item = new ProjectText();
            else if (m_AddSelector.radioPageBreak.Checked)
                item = new ProjectPage();

            if (lstProjectVector.SelectedItems.Count == 0)
                lstProjectVector.Items.Add(item.LVI);
            else
                lstProjectVector.Items.Insert(lstProjectVector.SelectedIndices[0], item.LVI);

            item.LVI.Selected = true;
        }

        private void txtCalculation_TextChanged(object sender, EventArgs e)
        {
            if (m_CurrentItem is ProjectCalculation)
                m_CurrentItem.Value = txtCalculation.Text;
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
                m_CurrentItem.Value = txtAttributeName.Text;
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

        private void lstDBSpecies_ItemActivate(object sender, EventArgs e)
        {
            MessageBox.Show("Not Implemented");
        }

        private void txtFilter_TextChanged(object sender, EventArgs e)
        {
            UpdateFilter();
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

        private void MenuSort_Click(object sender, EventArgs e)
        {
            Sort();
        }

        private void menuAdvancedSort_Click(object sender, EventArgs e)
        {
            PVIOrderer oldSorter = m_Sorter.Clone();
            if (m_SortOptions.ShowDialog(this) == DialogResult.OK)
                Sort();
            else
            {
                m_Sorter = oldSorter;
                m_SortOptions.Sorter = oldSorter;
            }
        }

        private void txtCalcName_TextChanged(object sender, EventArgs e)
        {
            if (m_CurrentItem is ProjectCalculation)
                m_CurrentItem.Name = txtCalcName.Text;
        }

        private void lstProjectVector_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Delete)
                btnRemove_Click(sender, e);
        }

        private void lstDBSpecies_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lstDBSpecies.SelectedItems.Count > 0)
            {
                this.BindingContext[m_SpecieDataTable].Position = m_SpecieDataTable.Rows.IndexOf((DataRow)lstDBSpecies.SelectedItems[0].Tag);
                LoadEquationsIntoGraph();
                LoadEquationsIntoTextboxes();
            }
            else
            {
                //We need to disable the speciedatabase panel.
            }
        }

        private void txtCalcSymbol_TextChanged(object sender, EventArgs e)
        {
            if (m_CurrentItem is ProjectCalculation)
                ((ProjectCalculation)m_CurrentItem).Symbol = txtCalcSymbol.Text;
            
        }

        private void txtMinTemperature_TextChanged(object sender, EventArgs e)
        {
            try
            {
                graph1.MinXValue = double.Parse(txtMinTemperature.Text);
            }
            catch { }
        }

        private void txtMaxTemperature_TextChanged(object sender, EventArgs e)
        {
            try { graph1.MaxXValue = double.Parse(txtMaxTemperature.Text); }
            catch { }
        }

        private void radioEntropy_CheckedChanged(object sender, EventArgs e)
        {
            RadioButton rSender = sender as RadioButton;
            m_GraphSeries[(string)rSender.Tag].Selected = rSender.Checked;
            LoadEquationsIntoTextboxes((string)rSender.Tag);
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (dlgOpenConfig.ShowDialog() == DialogResult.OK)
                LoadFile(dlgOpenConfig.FileName);
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Save();
        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveAs();
        }

        private void txtFormula0_Leave(object sender, EventArgs e)
        {
            foreach (Control c in pnlTempDependantRadios.Controls)
                if (c is RadioButton && ((RadioButton)c).Checked)
                {
                    string col = (string)c.Tag;
                    m_SpecieDataTable.Rows[this.BindingContext[m_SpecieDataTable].Position][col] = CreateDBString();
                    LoadEquationsIntoGraph(col);
                }
        }

        private void menuSortSpDBAlph_CheckedChanged(object sender, EventArgs e)
        {
            UpdateFilter();
        }

        private void menuSpDBAddToProject_Click(object sender, EventArgs e)
        {
            TransferSpecies();
        }
    }

    public class PolyEquationSeries : GraphSeries
    {
        protected float m_fMax = 100, m_fMin = 0;
        protected double m_dMinX = double.NaN, m_dMaxX = double.NaN;
        protected List<double> m_Coefficients = new List<double>();
        protected int m_nScanResolution = 100;

        #region Graph Functions
        public override double Granularity
        {
            get { return 0; }
        }

        public override float Max
        {
            get { return m_fMax; }
        }

        public override float Min
        {
            get { return m_fMin; }
        }

        public override double MinX
        {
            get { return m_dMinX; }
            set 
            {
                m_dMinX = value;
                RecalculateMinMax();
            }
        }

        public override double MaxX
        {
            get { return m_dMaxX; }
            set 
            {
                m_dMaxX = value;
                RecalculateMinMax();
            }
        }

        public override float ValueAt(double x)
        {
            double val = 1;
            double ret = 0;
            foreach (double coeff in m_Coefficients)
            {
                ret += val * coeff;
                val *= x;
            }
            return (float) ret;
        }

        public override bool ValidAt(double x)
        {
            return m_Coefficients.Count > 0;
        }
        #endregion Graph Functions

        public PolyEquationSeries(string s)
        {
            SetString(s);
        }

        public PolyEquationSeries(string s, double minX, double maxX)
        {
            SetString(s);
            m_dMinX = minX;
            m_dMaxX = maxX;
            RecalculateMinMax();
        }

        public int ScanResolution
        {
            get { return m_nScanResolution; }
            set
            {
                if (value < 2) value = 2;
                m_nScanResolution = value;
                RecalculateMinMax();
            }
        }

        static Regex numberGetter = new Regex(@"(\+|-)?(\d+(\.\d+)?|\.\d+)(e(\+|-)?\d+)?", RegexOptions.Compiled | RegexOptions.IgnoreCase);
        public void SetString(string s)
        {
            m_Coefficients.Clear();
            for (Match m = numberGetter.Match(s); m.Success; m = m.NextMatch())
                try
                {
                    m_Coefficients.Add(double.Parse(m.Value));
                }
                catch { } //for out of range problems.
            RecalculateMinMax();
        }

        protected void RecalculateMinMax()
        {
            if (double.IsNaN(m_dMinX) || double.IsNaN(m_dMaxX))
                return;
            float min = float.PositiveInfinity; float max = float.NegativeInfinity;
            double step = (m_dMaxX - m_dMinX) / (m_nScanResolution - 1);
            for (int i = 0; i < m_nScanResolution; i++)
            {
                float v = ValueAt(m_dMinX + step * i);
                if (min > v)
                    min = v;
                if (max < v)
                    max = v;
            }
            m_fMin = min;
            m_fMax = max;
        }
    }

    public class MultiEquationDataset : GraphSeries
    {
        protected List<GraphSeries> m_DataSets  = new List<GraphSeries>();
        protected GraphSeries m_BackupSeries;
        protected double m_dMinX, m_dMaxX;

        public GraphSeries BackupSeries
        {
            get { return m_BackupSeries; }
            set
            {
                m_BackupSeries = value;
                m_BackupSeries.MinX = MinX;
                m_BackupSeries.MaxX = MaxX;
                m_BackupSeries.RedrawRequired += new EventHandler(series_RedrawRequired);
                FireRedrawRequired(this, new EventArgs());
            }
        }

        public void AddSeries(GraphSeries series)
        {
            m_DataSets.Add(series);
            series.RedrawRequired += new EventHandler(series_RedrawRequired);
        }

        public void RemoveSeries(GraphSeries series)
        {
            m_DataSets.Remove(series);
            series.RedrawRequired -= new EventHandler(series_RedrawRequired);
        }

        public void ClearSeries()
        {
            m_DataSets.Clear();
            m_BackupSeries = null;
            FireRedrawRequired(this, new EventArgs());
        }

        public GraphSeries GetSeries(int i) { return m_DataSets[i]; }

        void series_RedrawRequired(object sender, EventArgs e)
        {
            FireRedrawRequired(this, e);
        }

        #region IDataSet Members
        public override double Granularity
        {
            get 
            {
                double minVal = m_BackupSeries != null ? m_BackupSeries.Granularity : double.PositiveInfinity;
                foreach (GraphSeries d in m_DataSets)
                    if (d.Granularity < minVal)
                        minVal = d.Granularity;
                return minVal;
            }
        }

        public override float Max
        {
            get 
            {
                float maxVal = m_BackupSeries != null ? m_BackupSeries.Max : float.NegativeInfinity;
                foreach (GraphSeries d in m_DataSets)
                    if (d.Max > maxVal)
                        maxVal = d.Max;
                return maxVal;
            }
        }

        public override float Min
        {
            get 
            {
                float minVal = m_BackupSeries != null ? m_BackupSeries.Min : float.PositiveInfinity;
                foreach (GraphSeries d in m_DataSets)
                    if (d.Min < minVal)
                        minVal = d.Min;
                return minVal;
            }
        }

        public override double MaxX
        {
            set 
            {
                if (m_BackupSeries != null)
                    m_BackupSeries.MaxX = value;
                m_dMaxX = value;
            }
            get
            {
                double ret = m_BackupSeries != null ? m_dMaxX : double.NegativeInfinity;
                foreach (GraphSeries gs in m_DataSets)
                    if (gs.MaxX > ret)
                        ret = gs.MaxX;
                return ret;
            }
        }

        public override double MinX
        {
            set 
            {
                if (m_BackupSeries != null)
                    m_BackupSeries.MinX = value;
                m_dMinX = value;
            }
            get
            {
                double ret = m_BackupSeries != null ? m_dMinX : double.PositiveInfinity;
                foreach (GraphSeries gs in m_DataSets)
                    if (gs.MinX < ret)
                        ret = gs.MinX;
                return ret;
            }
        }

        public override float ValueAt(double x)
        {
            foreach (GraphSeries gs in m_DataSets)
                if (gs.MinX < x && gs.MaxX > x && gs.ValidAt(x))
                    return gs.ValueAt(x);
            return m_BackupSeries != null ? m_BackupSeries.ValueAt(x) : float.NaN;
        }

        public override bool ValidAt(double x)
        {
            foreach (GraphSeries gs in m_DataSets)
                if (gs.MinX < x && gs.MaxX > x && gs.ValidAt(x))
                    return true;
            return m_BackupSeries != null && m_BackupSeries.ValidAt(x);
        }
        #endregion
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

    public class ProjectVectorItemDrag
    {
        public ProjectVectorItem[] items;
    }

    public class SpecieDrag
    {
        public DataRow[] Value;

        public SpecieDrag(DataRow[] _Value)
        {
            Value = _Value;
        }
    }

    public class Pair<T> 
    { 
        public Pair(T _s1, T _s2)
        {
            s1 = _s1; s2 = _s2;
        }
        public T s1, s2;
        public override bool  Equals(object obj)
        {
            if (!(obj is Pair<T>))
                return false;
            Pair<T> other = obj as Pair<T>;
            return other.s1.Equals(s1) && other.s2.Equals(s2);
        }

        public override int GetHashCode()
        {
            return unchecked( s1.GetHashCode() * s2.GetHashCode() );
        }
    }
}