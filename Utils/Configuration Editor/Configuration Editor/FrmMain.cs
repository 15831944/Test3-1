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
using System.Reflection;
using Microsoft.Win32;
using System.Globalization;

namespace Configuration_Editor
{
    public partial class FrmMain : Form
    {
        public static string[] ReqColumnList = new string[] {
            "Name",
            "Compound",
            "Phase",
            "Definition",
            "Occurence",
            "Checked",
            "Ts",
            "Te",
            "Density",
            "Solvent",
            "DensityCorrection",
            "VPCorrection",
            "PhaseChange",
            "Hf25",
            "S25",
            "Cp",
            "VP",
            "Pc",
            "Tc",
            "Vc",
            "Ac",
            "Dissociation",
            "Reference"};

        #region Regex's
        //Allowed characters: Alphanumerics, _.*+-[]/
        protected static Regex s_AcceptableNameRegex = new Regex(@"^[A-Za-z][\w\[\]\.*+\-_/() ]*$", RegexOptions.Compiled);
        protected static Regex s_AcceptableSymbRegex = new Regex(        @"^[\w\[\]\.*+\-_/]*$", RegexOptions.Compiled);

        protected static Regex s_DefinitionRegex = new Regex(@"^
             (((?<Symb>[A-Za-z]+)(?<Count>[0-9]+(/[0-9]+|\.[0-9]+)?)) |
             ((?<CustomSymb>[A-Za-z]+)\((?<Wt>[0-9]+(\.[0-9]+)?)\)))+$", RegexOptions.Compiled | RegexOptions.IgnorePatternWhitespace);
        #endregion Regexs

        #region Variables
        ProjectVectorItem m_CurrentItem = null;
        FrmAddSelector m_AddSelector = new FrmAddSelector();
        FrmSortOptions m_SortOptions = new FrmSortOptions();
        PVIOrderer m_Sorter = new PVIOrderer();

        DataTable m_SpecieDataTable = new DataTable("Species");
        OleDbDataAdapter m_SpecieDataAdapter;
        string m_CurrentGraphColumn = null;

        Dictionary<string, MultiEquationDataset> m_GraphSeries = new Dictionary<string, MultiEquationDataset>();

        FileStream m_ConfigFile;
        Dictionary<string, Dictionary<string, string>> m_UnparsedInfo = new Dictionary<string, Dictionary<string, string>>(); //Because it's easier to work with an empty dictionary than null.
        Dictionary<string, Dictionary<string, string>> m_DefaultIniDictionary;

        RegistryKey regKey;

        double m_dCurSpeciesMolWt = double.NaN;
        Dictionary<string, double> AtomicWts = new Dictionary<string, double>();

        #endregion Variables
        
        #region Constructors
        public FrmMain()
        {
            InitializeComponent();
            try
            {
                regKey = Registry.CurrentUser.CreateSubKey("Software").CreateSubKey("Kenwalt").CreateSubKey("SysCAD Configuration Editor");
                SetDirectory((string)regKey.GetValue("Initial Directory", "My Documents"));
            }
            catch { }

            this.Load += new EventHandler(FrmMain_Load);
        }

        void FrmMain_Load(object sender, EventArgs e)
        {
            try
            {
                m_DefaultIniDictionary = ReadIni(Configuration_Editor.Properties.Resources.DefaultInI, new Dictionary<string, Dictionary<string, string>>());

                SetupMisc();
                SetupSpDbEditor();
                SetupSpecies();

                if (dlgOpenDB.ShowDialog() == DialogResult.Cancel)
                    this.Close();
                else
                    OpenDatabase(dlgOpenDB.FileName);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Initialisation Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                this.Close();
            }

        }

        private bool SetupSpDbEditor()
        {
            if (File.Exists("DatabaseColumns.ini"))
                LoadDBIni("DatabaseColumns.ini");
            else if (File.Exists("..\\..\\DatabaseColumns.ini"))
                LoadDBIni("..\\..\\DatabaseColumns.ini"); //Allow the database.ini to sit in the same folder as the source files
            else
                throw new Exception("DatabaseColumns.ini not found.");

            LoadAtomicWts();

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
            m_SpecieDataTable.Columns.Add("PhaseChange", typeof(String));
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

            m_SpecieDataTable.PrimaryKey = new DataColumn[] {
                m_SpecieDataTable.Columns["Compound"],
                m_SpecieDataTable.Columns["Phase"],
                m_SpecieDataTable.Columns["Ts"],
                m_SpecieDataTable.Columns["Te"]};

            errorProvider1.DataSource = m_SpecieDataTable;
            #endregion Database stuff

            
            
            txtName.DataBindings.Add("Text", m_SpecieDataTable, "Name");
            txtSymbol.DataBindings.Add("Text", m_SpecieDataTable, "Compound");
            comboPhase.DataBindings.Add("Text", m_SpecieDataTable, "Phase");

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
            txtPhaseChange.DataBindings.Add("Text", m_SpecieDataTable, "PhaseChange");
            txtDensityCorrection.DataBindings.Add("Text", m_SpecieDataTable, "DensityCorrection");
            return true;
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
            comboMaxCompFlow.Items.AddRange(new object[] { "Transer", "Simple", "Linear", "Full" });
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

            projectVectorControl1.Sorter = m_Sorter;
            projectVectorControl1.ListDBSpecies = lstDBSpecies;
            projectVectorControl1.SpecieDataTable = m_SpecieDataTable;

            projectVectorControl1.SetupSpecies();

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
                if (m_ConfigFile != null) try { m_ConfigFile.Close(); }
                    catch { }
                m_ConfigFile = new FileStream(dlgSaveConfig.FileName, FileMode.Create, FileAccess.ReadWrite, FileShare.Read);
                if (m_ConfigFile != null)
                    Save();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        protected void Save()
        {
            if (m_ConfigFile == null)
            {
                SaveAs();
                return;
            }

            StreamWriter sw = null;
            try
            {
                m_ConfigFile.SetLength(0);
                sw = new StreamWriter(m_ConfigFile);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                return;
            }

            try
            {
                Dictionary<string, Dictionary<string, string>> temp = new Dictionary<string, Dictionary<string, string>>(m_UnparsedInfo);
                sw.WriteLine("[General]");
                sw.WriteLine("Std_Temp=" + txtStT.Text);
                sw.WriteLine("Std_Press=" + txtStP.Text);
                sw.WriteLine("Norm_Temp=" + txtNT.Text);
                sw.WriteLine("Norm_Press=" + txtNP.Text);
                sw.WriteLine("Minimum_Temp=" + txtMinT.Text);
                sw.WriteLine("Minimum_Press=" + txtMinP.Text);
                sw.WriteLine("Maximum_Temp=" + txtMaxT.Text);
                sw.WriteLine("Maximum_Press=" + txtMaxP.Text);
                sw.WriteLine("Default_SpModel=" + comboSpecieModel.Text);
                sw.WriteLine("H2O_As_Aqueous=" + (chkH2OAqueous.Checked ? "1" : "0"));
                if (temp.ContainsKey("general"))
                {
                    foreach (KeyValuePair<string, string> kvp in temp["general"])
                        sw.WriteLine(kvp.Key + "=" + kvp.Value);
                    temp.Remove("general");
                }

                sw.WriteLine("[ModelDLLs]");
                //TODO: Model DLLs
                if (temp.ContainsKey("modeldlls"))
                {
                    foreach (KeyValuePair<string, string> kvp in temp["modeldlls"])
                        sw.WriteLine(kvp.Key + "=" + kvp.Value);
                    temp.Remove("modeldlls");
                }

                sw.WriteLine("[HelpDLLs]");
                //TODO: Find out what these are, then do 'em.
                if (temp.ContainsKey("helpdlls"))
                {
                    foreach (KeyValuePair<string, string> kvp in temp["helpdlls"])
                        sw.WriteLine(kvp.Key + "=" + kvp.Value);
                    temp.Remove("helpdlls");
                }

                sw.WriteLine("[Species]");
                int i = 0;
                foreach (ListViewItem lvi in lstProjectVector.Items)
                {
                    try
                    {
                        sw.WriteLine("S" + i++.ToString("0000") + "=" + ((ProjectVectorItem)lvi.Tag).ToSaveString());
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show(ex.ToString());
                    }
                }
                if (temp.ContainsKey("species"))
                {
                    foreach (KeyValuePair<string, string> kvp in temp["species"])
                        sw.WriteLine(kvp.Key + "=" + kvp.Value);
                    temp.Remove("species");
                }

                sw.WriteLine("[PhaseNames]");
                //TODO: Phase Names. Actually, what the hell are these things?
                if (temp.ContainsKey("phasenames"))
                {
                    foreach (KeyValuePair<string, string> kvp in temp["phasenames"])
                        sw.WriteLine(kvp.Key + "=" + kvp.Value);
                    temp.Remove("phasenames");
                }

                sw.WriteLine("[Selectable]");
                //TODO: Models.
                if (temp.ContainsKey("selectable"))
                {
                    foreach (KeyValuePair<string, string> kvp in temp["selectable"])
                        sw.WriteLine(kvp.Key + "=" + kvp.Value);
                    temp.Remove("selectable");
                }

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
                if (temp.ContainsKey("modes"))
                {
                    foreach (KeyValuePair<string, string> kvp in temp["modes"])
                        sw.WriteLine(kvp.Key + "=" + kvp.Value);
                    temp.Remove("modes");
                }

                foreach (KeyValuePair<string, Dictionary<string, string>> Sets in temp)
                {
                    sw.WriteLine("[" + Sets.Key + "]");
                    foreach (KeyValuePair<string, string> val in Sets.Value)
                        sw.WriteLine(val.Key + "=" + val.Value);
                }

                SaveDatabase();
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

        protected string Pop(Dictionary<string, string> source, string key)
        {
            string ret = source[key];
            source.Remove(key);
            return ret;
        }

        protected void LoadFile(string filename)
        {
            if (m_ConfigFile != null)
                try { m_ConfigFile.Close(); }
                catch { }

            m_ConfigFile = new FileStream(filename, FileMode.Open, FileAccess.ReadWrite, FileShare.Read, 8192, FileOptions.SequentialScan);

            string contents = new StreamReader(m_ConfigFile).ReadToEnd();

            List<string> errors = LoadFromDictionary(ReadIni(contents, m_DefaultIniDictionary));
            CheckCalculations();
            if (errors.Count != 0)
            {
                StringBuilder sb = new StringBuilder("The following errors occured trying to load the file:\n");
                foreach (string error in errors)
                    sb.AppendLine(error);
                MessageBox.Show(sb.ToString(), "Load Configuration", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        protected List<string> LoadFromDictionary(Dictionary<string, Dictionary<string, string>> data)
        {
            List<string> ret = new List<string>();
            m_UnparsedInfo = new Dictionary<string, Dictionary<string, string>>();

            if (data.ContainsKey("general"))
            {
                Dictionary<string, string> General = data["general"];
                //FileVersion = Pop(General, "prjfileversion");
                txtStT.Text = Pop(General,"std_temp");
                txtStP.Text = Pop(General,"std_press");
                txtNT.Text = Pop(General, "norm_temp");
                txtNP.Text = Pop(General, "norm_press");
                txtMinT.Text = Pop(General,"minimum_temp");
                txtMaxT.Text = Pop(General, "maximum_temp");
                txtMinP.Text = Pop(General, "minimum_press");
                txtMaxP.Text = Pop(General, "maximum_press");
                comboSpecieModel.Text = Pop(General, "default_spmodel");
                chkH2OAqueous.Checked = Pop(General, "h2o_as_aqueous") != "0";
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
                LoadSpecies(data["species"], ret);
                projectVectorControl1.LoadSpecies(data["species"], ret);
                data.Remove("species");
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
                comboDefaultSolution.Text = Pop(Modes,"default_netmode");
                chkProbalAllowed.Checked = Pop(Modes,"probal_allowed") != "0";
                comboProBalSurge.Text = Pop(Modes,"probal_nodemode");
                comboProbalFlow.Text = Pop(Modes,"probal_flowmode");
                comboProBalHeat.Text = Pop(Modes,"probal_heatmode");

                chkDynamicAllowed.Checked = Pop(Modes,"dynamic_allowed") != "0";
                comboDynamicSurge.Text = Pop(Modes,"dynamic_nodemode");
                comboDynamicFlow.Text = Pop(Modes,"dynamic_flowmode");
                comboDynamicHeat.Text = Pop(Modes,"dynamic_heatmode");

                comboMaxCompSurge.Text = Pop(Modes,"maximum_nodemode");
                comboMaxCompFlow.Text = Pop(Modes,"maximum_flowmode");
                comboMaxCompHeat.Text = Pop(Modes,"maximum_heatmode");
            }

            m_UnparsedInfo = data;
            return ret;
        }

        private static Regex SpecieRegex = new Regex(@"^\s*S\d+\s*$", RegexOptions.Compiled | RegexOptions.IgnoreCase);
        public void LoadSpecies(Dictionary<string, string> RelevantContents, List<string> Errors)
        {
            lstProjectVector.BeginUpdate();
            lstProjectVector.Items.Clear();
            foreach (KeyValuePair<string, string> s in RelevantContents)
            {
                if (!SpecieRegex.Match(s.Key).Success)
                    continue;
                try
                {
                    ProjectVectorItem item = ProjectVectorItem.Parse(s.Value, m_SpecieDataTable, Errors);
                    item.Changed += new EventHandler(item_Changed);
                    lstProjectVector.Items.Add(item.LVI);
                }
                catch
                {
                    Errors.Add("Unalbe to Parse: '" + s + "'");
                }
            }
            AddSumItems();
            lstProjectVector.EndUpdate();
        }

        protected static Regex CommentStripper = new Regex(@"^[^;\r\n]*", RegexOptions.Compiled | RegexOptions.Multiline);
        protected static Regex iniRegex1 = new Regex(@"(^\[(?<Title>\w*)\](?<Values>.*?))*\Z", 
            RegexOptions.Singleline | RegexOptions.Compiled | RegexOptions.Multiline); //This'll be kinda slow with that .*?
        protected static Regex iniRegex2 = new Regex(@"^(?<Variable>[^\s=]*)=(?<Value>.*)",
            RegexOptions.Multiline | RegexOptions.Compiled);

        //Work in pure lowercase.
        protected Dictionary<string, Dictionary<string, string>> ReadIni(string contents, Dictionary<string, Dictionary<string, string>> defaults)
        {
            StringBuilder sb = new StringBuilder();
            for (Match m = CommentStripper.Match(contents); m.Success; m = m.NextMatch())
                sb.AppendLine(m.Value);
            contents = sb.ToString();
            Match tier1Match = iniRegex1.Match(contents);
            Dictionary<string, Dictionary<string, string>> ret = new Dictionary<string,Dictionary<string,string>>();
            foreach (KeyValuePair<string, Dictionary<string, string>> kvp in defaults) //Make a deep copy
                ret[kvp.Key] = new Dictionary<string, string>(kvp.Value);
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

        protected void SetDirectory(string directory)
        {
            dlgOpenConfig.InitialDirectory = dlgOpenDB.InitialDirectory = dlgSaveConfig.InitialDirectory = directory;
            try
            {
                regKey.SetValue("Initial Directory", directory);
            }
            catch { }
        }
        #endregion File Operations

        #region Specie database stuff
        protected void SaveDatabase()
        {
            //Determine max lengths for each column:
            Dictionary<string, int> MaxLengths = new Dictionary<string, int>();
            Dictionary<string, string> MaxLengthNames = new Dictionary<string,string>();
            foreach (DataColumn c in m_SpecieDataTable.Columns)
                MaxLengths[c.ColumnName] = 0;

            foreach (DataRow r in m_SpecieDataTable.Rows)
                if (r.RowState != DataRowState.Deleted)
                foreach (DataColumn c in m_SpecieDataTable.Columns)
                    if (r[c] is string && MaxLengths[c.ColumnName] < ((string)r[c]).Length)
                    {
                        MaxLengths[c.ColumnName] = ((string)r[c]).Length;
                        MaxLengthNames[c.ColumnName] = (string)r["Name"];
                    }
            if (m_SpecieDataAdapter != null)
                m_SpecieDataAdapter.Update(m_SpecieDataTable);
        }

        protected void OpenDatabase(string filename)
        {
            string tableName = "Species";

            OleDbConnection conn = new OleDbConnection(@"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + filename);
            conn.Open();
            //Putting things in Try {} Catch{} is the worst form of flow control, or so they say.
            //But there seems no other option
            DataTable t = new DataTable();
            OleDbDataAdapter da = new OleDbDataAdapter("SELECT * FROM " + tableName, conn);
            da.FillSchema(t, SchemaType.Source);

            #region Change column layout to what is expected:
            Dictionary<string, string> Remapping = new Dictionary<string, string>(); //<To, From>
            List<string> NewColumns = new List<string>();

            //Check for any columns that must be renamed:
            foreach (DataColumn c in t.Columns)
                if (c.ColumnName == "Rho")
                    Remapping.Add("Density", "Rho");
                else if (c.ColumnName == "dHf")
                    Remapping.Add("Hf25", "dHf");
                else if (c.ColumnName == "S°298")
                    Remapping.Add("S25", "S°298");
            //Check for any columns that must be added
            foreach (string s in ReqColumnList)
                if (!t.Columns.Contains(s) && !Remapping.ContainsKey(s))
                    NewColumns.Add(s);


            OleDbCommand cmd = new OleDbCommand();
            cmd.Connection = conn;
            if (NewColumns.Count > 0)
            {
                for (int i = 0; i < NewColumns.Count; i++)
                {
                    if (NewColumns[i] == "Ts" || NewColumns[i] == "Te")
                        cmd.CommandText = "ALTER TABLE " + tableName + " ADD " + NewColumns[i] + " single;";
                    else
                        cmd.CommandText = "ALTER TABLE " + tableName + " ADD " + NewColumns[i] + " varchar;";
                    cmd.ExecuteNonQuery();
                }
            }
            if (Remapping.Count > 0)
            {
                foreach (KeyValuePair<string, string> kvp in Remapping)
                {
                    cmd.CommandText = "ALTER TABLE " + tableName + " ADD COLUMN " + kvp.Key + " varchar;";
                    cmd.ExecuteNonQuery();
                }
                StringBuilder sb = new StringBuilder("UPDATE " + tableName + " SET ");
                int j = 0;
                foreach (KeyValuePair<string, string> kvp in Remapping)
                {
                    sb.Append(kvp.Key + " = " + kvp.Value);
                    if (j++ != Remapping.Count - 1)
                        sb.Append(", ");
                }
                sb.AppendLine(";");
                cmd.CommandText = sb.ToString();
                cmd.ExecuteNonQuery();
                /*foreach (KeyValuePair<string, string> kvp in Remapping)
                {
                    cmd.CommandText = "UPDATE " + tableName + " SET " + kvp.Key + " = " + kvp.Value;
                    cmd.ExecuteNonQuery();
                }*/

                foreach (KeyValuePair<string, string> kvp in Remapping)
                {
                    cmd.CommandText = "ALTER TABLE " + tableName + " DROP COLUMN " + kvp.Value + ";";
                    cmd.ExecuteNonQuery();
                }
            }
            #endregion Column Layout

            #region Check Constraints:
            List<DataColumn> PK = new List<DataColumn>(t.PrimaryKey);
            if (t.PrimaryKey.Length == 0) //If we need to remove or change the primary key...
            {
                cmd.CommandText = "ALTER TABLE " + tableName + " ADD CONSTRAINT pk PRIMARY KEY(Compound, Phase, Ts, Te)";
                cmd.ExecuteNonQuery();
            }

            foreach (DataColumn c in t.Columns)
                if (c.DataType == typeof(string) && !Remapping.ContainsValue(c.ColumnName))
                {
                    cmd.CommandText = "ALTER TABLE " + tableName + " ALTER COLUMN " + c.ColumnName + " varchar(255)";
                    cmd.ExecuteNonQuery();
                }

            /*Dictionary<string, string> RequiredConstraints = new Dictionary<string, string>();
            //RequiredConstraints.Add("Occ_gls", "CHECK (Occurence IN ('g', 'l', 's', 'G', 'L', 'S'))");
            RequiredConstraints.Add("Tgt0", "CHECK (Ts > 0 and Te > 0)"); //We're not going to force Te > Ts, but rather allow the user to make the change...
            foreach (string s in RequiredConstraints.Keys)
                if (!t.Constraints.Contains(s))
                {
                    cmd.CommandText = "ALTER TABLE " + tableName + " ADD CONSTRAINT " + s + " " + RequiredConstraints[s];
                    cmd.ExecuteNonQuery();
                }*/ //It seems that it really doesn't like these constraints for an unkown reason.


            #endregion Check Constraints

            m_SpecieDataAdapter = new OleDbDataAdapter("SELECT * FROM " + tableName, conn);

            m_SpecieDataTable.Clear();
            m_SpecieDataAdapter.FillSchema(m_SpecieDataTable, SchemaType.Source);
            m_SpecieDataAdapter.Fill(m_SpecieDataTable);

            new OleDbCommandBuilder(m_SpecieDataAdapter);

            #region Commented Out Code
            //Alternative (2-table) method:
            /*
            try
            {
                m_SpecieDataAdapter = new OleDbDataAdapter("SELECT * FROM Species2", conn);

                m_SpecieDataAdapter.Fill(m_SpecieDataTable);

                new OleDbCommandBuilder(m_SpecieDataAdapter);
            }
            catch
            {
                m_SpecieDataAdapter = new OleDbDataAdapter("SELECT * FROM " + tableName, conn);

                DataTableMapping mapping = m_SpecieDataAdapter.TableMappings.Add("Table", "" + tableName);
                mapping.ColumnMappings.Add("Rho", "Density");
                mapping.ColumnMappings.Add("dHf", "Hf25");
                mapping.ColumnMappings.Add("S°298", "S25");

                OleDbCommandBuilder cb = new OleDbCommandBuilder(m_SpecieDataAdapter);

                DataTable intermediate = m_SpecieDataTable.Clone();
                m_SpecieDataAdapter.Fill(intermediate);
                foreach (DataRow r in intermediate.Rows)
                    m_SpecieDataTable.ImportRow(r);

                MergeRows();

                CreateNewTable(conn);

                m_SpecieDataAdapter.SelectCommand = new OleDbCommand("SELECT * FROM Species2", conn);

                SaveDatabase();
            }*/
            #endregion Commented Out Code

            MergeRows();

            SaveDatabase();

            //After we have merged the variables, we no longer need to have a 4 column primary key (Ts and Te can be removed).
            m_SpecieDataTable.PrimaryKey = new DataColumn[] { m_SpecieDataTable.Columns["Compound"], m_SpecieDataTable.Columns["Phase"] };
            cmd.CommandText = "ALTER TABLE " + tableName + " DROP CONSTRAINT pk"; cmd.ExecuteNonQuery();
            cmd.CommandText = "ALTER TABLE " + tableName + " ADD CONSTRAINT pk PRIMARY KEY(Compound, Phase)"; cmd.ExecuteNonQuery();

            UpdateFilter();
            conn.Close();
        }

        protected void CreateNewTable(OleDbConnection conn)
        {
            OleDbCommand cmd = new OleDbCommand(@"CREATE TABLE Species2 (
            Name varchar,
            Compound varchar,
            Phase varchar,
            Definition varchar,
            Occurence varchar,
            Checked varchar,
            Ts Single,
            Te Single,
            Density varchar,
            Solvent varchar,
            DensityCorrection varchar,
            VPCorrection varchar,
            PhaseChange varchar,
            Hf25 varchar,
            S25 varchar,
            Cp varchar,
            VP varchar,
            Pc varchar,
            Tc varchar,
            Vc varchar,
            Ac varchar,
            Dissociation varchar,
            Reference varchar,
            PRIMARY KEY (Compound, Phase))", conn);
            cmd.ExecuteNonQuery();

            //OleDbCommand cmd2 = new OleDbCommand(@"ALTER TABLE Species2 ADD ", conn);
            //cmd2.ExecuteNonQuery();

            m_SpecieDataTable.PrimaryKey = new DataColumn[] {
                m_SpecieDataTable.Columns["Compound"],
                m_SpecieDataTable.Columns["Phase"],
                m_SpecieDataTable.Columns["Ts"],
                m_SpecieDataTable.Columns["Te"]};

        }

        protected string m_sSelectedEquationColumn = "Density";
        protected static Regex EquationSplitterRegex = new Regex(
            @"^((""?(?<Equation>[^{}""]+)""?{(?<TMin>\d+(\.\d+)?|\.\d+),(?<TMax>\d+(\.\d+)?|\.\d+)})*|""?(?<GeneralEquation>[^{}""]+))""?$",
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
            if ((o is string))
                s = (string) o;
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
            m_SpecieDataTable.BeginLoadData(); //Stop constraint notifications caused by faulty input data...
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
                        r.Delete();
            }
            m_SpecieDataTable.EndLoadData();
        }
        
        protected string TableToCSV(DataTable dt)
        {
            StringBuilder sb = new StringBuilder();
            foreach (DataColumn c in dt.Columns)
                sb.Append(c.ColumnName + ",");
            sb.AppendLine();
            foreach (DataRow r in dt.Rows)
            {
                foreach (DataColumn c in dt.Columns)
                    sb.Append(r[c].ToString() + "\t");
                sb.AppendLine();
            }
            return sb.ToString();
        }

        //Load column details from an ini:
        Dictionary<string, Dictionary<string, FunctionValue>> availableDBFuncs = new Dictionary<string, Dictionary<string, FunctionValue>>();
        static Regex CSVRegex = new Regex(@"\s*(?<Quotes>"")?(?<Val>(?(Quotes)[^""]*|[^,]*))(?(Quotes)(?<-Quotes>""))
            (\s*,\s*(?<Quotes>"")?(?<Val>(?(Quotes)[^""]*|[^,]*))(?(Quotes)(?<-Quotes>"")))*", RegexOptions.Compiled | RegexOptions.IgnorePatternWhitespace);
        protected void LoadDBIni(string filename)
        {
            tlpTempDependantRadios.RowCount = 0;
            string s = new StreamReader(new FileStream(filename, FileMode.Open)).ReadToEnd();
            Dictionary<string, Dictionary<string, string>> data = ReadIni(s, new Dictionary<string, Dictionary<string, string>>());
            foreach (KeyValuePair<string, Dictionary<string, string>> kvp1 in data)
            {
                if (!availableDBFuncs.ContainsKey(kvp1.Key))
                    availableDBFuncs[kvp1.Key] = new Dictionary<string, FunctionValue>();

                MultiEquationDataset equations = new MultiEquationDataset();
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
                    catch { throw new Exception("Unable to parse colour associated with " + kvp1.Key); }
                }

                //Read in meta data
                equations.DefaultColour = c;
                equations.Logarithmic = kvp1.Value.ContainsKey("logarithmic") && kvp1.Value["logarithmic"] != "0";

                m_GraphSeries.Add(kvp1.Key, equations);
                graph1.AddDataset(equations);

                RadioButton rb = new RadioButton();
                rb.Text = kvp1.Value.ContainsKey("friendlyname") ? kvp1.Value["friendlyname"].Trim('"') : kvp1.Key;
                rb.ForeColor = c;
                rb.CheckedChanged += new EventHandler(radioEntropy_CheckedChanged);
                rb.Tag = kvp1.Key;
                //tlpTempDependantRadios.RowCount++;
                //tlpTempDependantRadios.RowStyles[tlpTempDependantRadios.RowCount - 1].SizeType = SizeType.AutoSize;
                tlpTempDependantRadios.Controls.Add(rb);
                rb.Dock = DockStyle.Top;

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

        static Regex AtomicWtsRegex = new Regex(@"(?<Symb>[^,\r\n]*),(?<Val>[^,\r\n]*)");
        protected void LoadAtomicWts()
        {
            for (Match m = AtomicWtsRegex.Match(Configuration_Editor.Properties.Resources.AtmoicWts); m.Success; m = m.NextMatch())
            {
                AtomicWts[m.Groups["Symb"].Value] = double.Parse(m.Groups["Val"].Value);
            }
        }

        protected void UpdateDBLVIs()
        {
            try
            {
                DataRow curRow = ((DataRowView)this.BindingContext[m_SpecieDataTable].Current).Row;
                foreach (ListViewItem lvi in lstDBSpecies.Items)
                    if (lvi.Tag == curRow)
                    {
                        lvi.SubItems[0].Text = (string)curRow["Compound"] + "(" + curRow["Phase"] + ")";
                        lvi.SubItems[1].Text = (string)curRow["Name"];
                        break; //Should only occur once.
                    }
            }
            catch { };
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
                    specie.Changed += new EventHandler(item_Changed);

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

        protected void UpdateStatusBar()
        {
            if (tcSpecies.SelectedTab == this.tabProjectSpecies)
                if (m_CurrentItem != null)
                {
                    statusLabel.Text = m_CurrentItem.Valid ? "Status: Ok" :
                        "Status: Invalid. " + m_CurrentItem.StatusDetails;
                    statusLabel.ForeColor = m_CurrentItem.Valid ? SystemColors.WindowText : Color.Red;
                }
                else
                    statusLabel.Text = "";
            else
                statusLabel.Text = "";
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
                    if (r.RowState == DataRowState.Deleted)
                        continue;

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

        protected void PreventCommas(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == ',')
                e.Handled = true;
        }

        protected void PreventCurlyBraces(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == '{' || e.KeyChar == '}')
                e.Handled = true;
        }

        protected void NumbersOnly(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar < '0' || e.KeyChar > '9')
                e.Handled = true;
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
            try
            {
                if (m_SpecieDataAdapter != null)
                    SaveDatabase();
            }
            catch { }

            try
            {
                if (dlgOpenDB.ShowDialog() == DialogResult.Cancel)
                    return;

                OpenDatabase(dlgOpenDB.FileName);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void lstDBSpecies_ItemDrag(object sender, ItemDragEventArgs e)
        {
            SpecieDrag drag = new SpecieDrag(new DataRow[lstDBSpecies.SelectedItems.Count]);
            for (int i = 0; i < lstDBSpecies.SelectedItems.Count; i++)
                drag.Value[i] = (DataRow)lstDBSpecies.SelectedItems[i].Tag;
            DoDragDrop(drag, DragDropEffects.Link);
        }

        #region Project Species Tab
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

            UpdateStatusBar();

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
            if (m_AddSelector.ShowDialog() == DialogResult.Cancel) return;
            lstProjectVector.SelectedItems.Clear();
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

        void item_Changed(object sender, EventArgs e)
        {
            UpdateStatusBar();
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
                    EquationFragment frag = EquationFragment.Parse(txtCalculation.Text, new Dictionary<string,FunctionValue>());
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

        protected void HighlightText(RichTextBox box, int start, int length)
        {
            box.Select(start, length);
            box.SelectionBackColor = Color.DarkRed;
            box.SelectionColor = Color.White;
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

        //Called after a symbol is changed in the database. If the selected compound is in the project specie list, updates that compound & rechecks calculations (Does not refactor calculations).
        protected void UpdateProjectLVIs()
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
         #endregion Project Species Tab

        #region Database Tab
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
            m_CurrentGraphColumn = (string)rSender.Tag;
            m_GraphSeries[m_CurrentGraphColumn].Selected = rSender.Checked;
            if (rSender.Checked)
                LoadEquationsIntoTextboxes(m_CurrentGraphColumn);
            tlpEquations.Enabled = true;
        }

        private void txtFormula0_Leave(object sender, EventArgs e)
        {
            try
            {
                if (!string.IsNullOrEmpty(m_CurrentGraphColumn))
                {
                    m_SpecieDataTable.Rows[this.BindingContext[m_SpecieDataTable].Position][m_CurrentGraphColumn] = CreateDBString();
                    LoadEquationsIntoGraph(m_CurrentGraphColumn);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Formulae", MessageBoxButtons.OK, MessageBoxIcon.Error);
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
            comboPhase.Items.Clear();
            foreach (DataRow r in m_SpecieDataTable.Rows)
                if (!comboPhase.Items.Contains(r["Phase"]))
                    comboPhase.Items.Add(r["Phase"]);
        }

        private void txtSymbol_Validated(object sender, EventArgs e)
        {
            UpdateProjectLVIs();
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

        #endregion Database Tab

        private void menuSpDBContext_Opening(object sender, CancelEventArgs e)
        {
            menuDelete.Enabled = lstDBSpecies.SelectedItems.Count > 0;
            menuSpDBAddToProject.Enabled = lstDBSpecies.SelectedItems.Count > 0;
        }

        private void lstDBSpecies_ItemActivate(object sender, EventArgs e)
        {
            if (tcSpecies.SelectedTab == tabProjectSpecies && m_CurrentItem is ProjectCalculation)
            {
                DataRow r = (DataRow)(lstDBSpecies.SelectedItems[0].Tag);
                txtCalculation.Text += (string)r["Compound"] + "(" + r["Phase"] + ")";
            }
        }

        private void txtFilter_TextChanged(object sender, EventArgs e)
        {
            UpdateFilter();
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

        private void lstDBSpecies_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lstDBSpecies.SelectedItems.Count > 0)
            {
                try
                {
                    this.BindingContext[m_SpecieDataTable].Position = m_SpecieDataTable.Rows.IndexOf((DataRow)lstDBSpecies.SelectedItems[0].Tag);
                    LoadEquationsIntoGraph();
                    LoadEquationsIntoTextboxes();
                    RecalculateMolWt();
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, "Invalid Data", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            else
            {
                //We need to disable the speciedatabase panel.
            }
        }

        private void FireCheckCalculations(object sender, EventArgs e)
        {
            CheckCalculations();
        }

        private void menuOpen_Click(object sender, EventArgs e)
        {
            try
            {
                if (m_ConfigFile != null)
                    switch (MessageBox.Show("Do you wish to save changes to " + Path.GetFileName(m_ConfigFile.Name) + "?", "SysCAD Configuration Editor", MessageBoxButtons.YesNoCancel))
                    {
                        case DialogResult.Yes: Save(); break;
                        case DialogResult.Cancel:return;
                        case DialogResult.No: break;
                    }
                if (m_SpecieDataTable.Rows.Count == 0)
                {
                    string t = dlgOpenDB.Title;
                    dlgOpenDB.Title = "Please select a specie database to open before opening a configuration file";
                    DialogResult d = dlgOpenDB.ShowDialog();
                    dlgOpenDB.Title = t;

                    if (d != DialogResult.OK)
                        return;
                    OpenDatabase(dlgOpenDB.FileName);
                }
                if (dlgOpenConfig.ShowDialog() == DialogResult.OK)
                    LoadFile(dlgOpenConfig.FileName);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Save();
        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveAs();
        }

        private void menuSortSpDBAlph_CheckedChanged(object sender, EventArgs e)
        {
            UpdateFilter();
        }

        private void menuSpDBAddToProject_Click(object sender, EventArgs e)
        {
            TransferSpecies();
        }

        private void menuNew_Click(object sender, EventArgs e)
        {
            if (m_ConfigFile != null)
                switch (MessageBox.Show("Do you wish to save changes to " + Path.GetFileName(m_ConfigFile.Name) + "?", "SysCAD Configuration Editor", MessageBoxButtons.YesNoCancel))
                {
                    case DialogResult.Yes: Save(); break;
                    case DialogResult.Cancel: return;
                    case DialogResult.No: break;
                }
            if (m_SpecieDataTable.Rows.Count == 0)
            {
                string t = dlgOpenDB.Title;
                dlgOpenDB.Title = "Please select a specie database to open before creating a configuration file";
                DialogResult d = dlgOpenDB.ShowDialog();
                dlgOpenDB.Title = t;

                if (d != DialogResult.OK)
                    return;
                OpenDatabase(dlgOpenDB.FileName);
            }
            Dictionary<string, Dictionary<string, string>> tmp = new Dictionary<string, Dictionary<string, string>>();
            foreach (KeyValuePair<string, Dictionary<string, string>> kvp in m_DefaultIniDictionary)
                tmp[kvp.Key] = new Dictionary<string,string>(kvp.Value);
            LoadFromDictionary(tmp);
            AddSumItems();
        }

        private void aboutSysCADConfigurationEditorToolStripMenuItem_Click(object sender, EventArgs e)
        {
            new About.FrmAbout().ShowDialog(this);
        }

        private void dlgOpenDB_FileOk(object sender, CancelEventArgs e)
        {
            SetDirectory(((FileDialog)sender).InitialDirectory);
        }

        private void fileToolStripMenuItem_DropDownOpening(object sender, EventArgs e)
        {
            menuSave.Enabled = menuSaveAs.Enabled = m_ConfigFile != null;
        }

        private void menuExit_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            if (m_ConfigFile != null)
                switch (MessageBox.Show("Do you wish to save changes to " + Path.GetFileName(m_ConfigFile.Name) + "?", "SysCAD Configuration Editor", MessageBoxButtons.YesNoCancel))
                {
                    case DialogResult.Yes: Save(); break;
                    case DialogResult.Cancel: e.Cancel = true; break;
                    case DialogResult.No: break;
                }
            base.OnClosing(e);
        }

        protected override void OnClosed(EventArgs e)
        {
            try
            {
                SaveDatabase();
            }
            catch { }
            base.OnClosed(e);
        }

        private void menuDelete_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Are you sure you wish to delete the selected species from the Specie Database and Project Vector?", "SysCAD Configuration Editor", MessageBoxButtons.YesNo, MessageBoxIcon.Warning) == DialogResult.Yes)
                DeleteSelectedSpecies();
        }

        protected void DeleteSelectedSpecies()
        {
            ArrayList selectedItems = new ArrayList(lstDBSpecies.SelectedItems);
            lstProjectVector.BeginUpdate();
            foreach (ListViewItem lvi in selectedItems)
            {
                lstDBSpecies.Items.Remove(lvi);
                ((DataRow)lvi.Tag).Delete();
                ArrayList projectItems = new ArrayList(lstProjectVector.Items);
                foreach (ListViewItem projLVI in projectItems)
                    if (projLVI.Tag is ProjectSpecie && ((ProjectSpecie)projLVI.Tag).SpDataRow == lvi.Tag)
                        lstProjectVector.Items.Remove(projLVI);
            }
            lstProjectVector.EndUpdate();
            UpdateFilter();
            if (lstDBSpecies.Items.Count > 0)
                lstDBSpecies.Items[0].Selected = true;
            else
            {
                tcSpecies.SelectedTab = tabProjectSpecies;
                tcSpecies.Enabled = false;
            }
        }

        protected void CreateSpecies()
        {
            DataRow r = m_SpecieDataTable.NewRow();
            int i = 1;
            while (m_SpecieDataTable.Select("Compound = 'NewSpecie" + i + "'").Length > 0)
                i++;
            r["Name"] = r["Compound"] = "NewSpecie" + i;
            r["Phase"] = "s";
            r["Occurence"] = "s";
            r["Ts"] = 298; r["Te"] = 500;
            r["Definition"] = "Temp(1)";
            m_SpecieDataTable.Rows.Add(r);
            UpdateFilter();
            foreach (ListViewItem lvi in lstDBSpecies.Items)
                if (lvi.Tag == r)
                {
                    lvi.EnsureVisible();
                    lvi.Selected = true;
                }
        }

        private void menuNewSpecies_Click(object sender, EventArgs e)
        {
            CreateSpecies();
        }
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