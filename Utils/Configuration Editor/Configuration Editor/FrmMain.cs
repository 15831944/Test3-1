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
    protected static Regex s_AcceptableSymbRegex = new Regex(@"^[\w\[\]\.*+\-_/]*$", RegexOptions.Compiled);

    protected static Regex s_DefinitionRegex = new Regex(@"^
             (((?<Symb>[A-Za-z]+)(?<Count>[0-9]+(/[0-9]+|\.[0-9]+)?)) |
             ((?<CustomSymb>[A-Za-z]+)\((?<Wt>[0-9]+(\.[0-9]+)?)\)))+$", RegexOptions.Compiled | RegexOptions.IgnorePatternWhitespace);

    protected static Regex s_ConversionRegex = new Regex(
        @"(?<Name>[^,\r\n]+)\s*,\s*(?<Offset>[^,]*)\s*,\s*(?<Factor>[^,]*)", RegexOptions.Compiled);
    #endregion Regexs

    #region Variables
    FrmSortOptions m_SortOptions = new FrmSortOptions();

    DataTable m_SpecieDataTable = new DataTable("Species");
    OleDbDataAdapter m_SpecieDataAdapter;

    Dictionary<string, MultiEquationDataset> m_GraphSeries = new Dictionary<string, MultiEquationDataset>();

    FileStream m_ConfigFile;
    Dictionary<string, Dictionary<string, string>> m_UnparsedInfo = new Dictionary<string, Dictionary<string, string>>(); //Because it's easier to work with an empty dictionary than null.
    Dictionary<string, Dictionary<string, string>> m_DefaultIniDictionary;

    RegistryKey regKey;

    Dictionary<string, BindingSource> m_UnitBindings = new Dictionary<string, BindingSource>();
    Dictionary<string, FormatterParserProvider> m_Formatters = new Dictionary<string, FormatterParserProvider>();
    FrmUnitSelection m_FrmUnitSelection = new FrmUnitSelection();
    int m_UpdateCalls = 0;
    #endregion Variables

    #region Constructors
    public FrmMain()
    {
      InitializeComponent();

      this.Load += new EventHandler(FrmMain_Load);
    }

    void FrmMain_Load(object sender, EventArgs e)
    {
      try
      {
        try
        {
          tcMain.TabPages.Remove(tabModels);
          regKey = Registry.CurrentUser.CreateSubKey("Software").CreateSubKey("Kenwalt").CreateSubKey("SysCAD Configuration Editor");
          SetDirectory((string)regKey.GetValue("Initial Directory", "My Documents"));
        }
        catch { }

        m_DefaultIniDictionary = ReadIni(Configuration_Editor.Properties.Resources.DefaultInI, new Dictionary<string, Dictionary<string, string>>());

        SetupConversions();
        SetupMisc();
        SetupSpDbEditor();
        SetupSpecies();

        HandleArgs(Program.Args);


      }
      catch (Exception ex)
      {
        MessageBox.Show(ex.Message, "Initialisation Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        this.Close();
      }
    }

    private void HandleArgs(string[] args)
    {
      //int DatabaseLoc = -1;
      //for (int i = 0; i < args.Length - 1; i++)
      //  if (args[i].ToLower() == "-d")
      //  {
      //    DatabaseLoc = i + 1;
      //    break;
      //  }

      //if (DatabaseLoc != -1)
      //{
      //  if (File.Exists(args[DatabaseLoc]))
      //    PKHOpenDatabase(args[DatabaseLoc]);
      //  else
      //  {
      //    MessageBox.Show("Specified database does not exist.", "Command Line Parameters", MessageBoxButtons.OK, MessageBoxIcon.Error);
      //    DatabaseLoc = -1;
      //  }
      //}

      int ConfigLoc = -1;
      for (int i = 0; i < args.Length - 1; i++)
        if (args[i].ToLower() == "-c")
        {
          ConfigLoc = i + 1;
          break;
        }

      if (ConfigLoc != -1)
      {
        if (File.Exists(args[ConfigLoc]))
          PKHLoadFile(args[ConfigLoc]);
        else
          MessageBox.Show("Specified configuration file does not exist.", "Command Line Parameters", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }

      if (ConfigLoc == -1)
      {
        if (dlgOpenConfig.ShowDialog() == DialogResult.Cancel)
        {
          this.Close();
          return;
        }
        else
          PKHLoadFile(dlgOpenConfig.FileName);
      }
    }

    void PKHLoadFile(string filename)
    {
      //if (DatabaseLoc == -1)
      //{
      string directoryName = Path.GetDirectoryName(filename);
      string mdbFilename = directoryName + @"\SysCAD.MDB";
      if (File.Exists(mdbFilename))
      {
        PKHOpenDatabase(mdbFilename);
      }
      else
      {
        MessageBox.Show("SysCAD.MDB not found in " + directoryName + "\n" + "Closing application.", "SysCAD.MDB not found.", MessageBoxButtons.OK, MessageBoxIcon.Error);
        Application.Exit();
      }

      //if (dlgOpenDB.ShowDialog() == DialogResult.Cancel)
      //{
      //  this.Close();
      //  return;
      //}
      //else
      //  PKHOpenDatabase(dlgOpenDB.FileName);
      //}

      LoadFile(filename);
    }

    private void SetupConversions()
    {
      ReadConversions();

      //specieDatabaseControl1.Formatters = m_Formatters;
      //specieDatabaseControl1.UnitBindingSource = m_UnitBindings;

      projectVectorControl1.TempFormatter = m_Formatters["temperature"].Formatter;
      projectVectorControl1.TempParser = m_Formatters["temperature"].Parser;
      projectVectorControl1.UnitBindingSource = m_UnitBindings;
    }

    private bool SetupSpDbEditor()
    {
      if (File.Exists("DatabaseColumns.ini"))
        LoadDBIni("DatabaseColumns.ini");
      else if (File.Exists("..\\..\\DatabaseColumns.ini"))
        LoadDBIni("..\\..\\DatabaseColumns.ini"); //Allow the database.ini to sit in the same folder as the source files
      else
        throw new Exception("DatabaseColumns.ini not found.");

      #region Database stuff
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

      //specieDatabaseControl1.DataSource = m_SpecieDataTable;

      //specieDatabaseControl1.UpdateLVIsRequired += new EventHandler(specieDatabaseControl1_UpdateLVIsRequired);

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
      projectVectorControl1.UsedSpeciesChanged += new EventHandler(projectVectorControl1_UsedSpeciesChanged);
      projectVectorControl1.ListDBSpecies = lstDBSpecies;
      projectVectorControl1.SpecieDataTable = m_SpecieDataTable;
      projectVectorControl1.Sorter = m_SortOptions.Sorter;

      projectVectorControl1.SetupSpecies();

      projectVectorControl1.StatusMessageChanged += new EventHandler(FireUpdateStatusBar);
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

        this.Text = "SysCAD Configuration Editor - " + dlgSaveConfig.FileName;

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


      string faults = string.Empty;
      bool allValid = true;

      foreach (ListViewItem lvi in projectVectorControl1.lstProjectVector.Items)
      {
        ProjectVectorItem lviProjectVectorItem = lvi.Tag as ProjectVectorItem;
        if (!lviProjectVectorItem.Valid)
              {
                faults += lviProjectVectorItem.ToString() + "\n";
                allValid = false;
              }
        //switch (lvi.Tag.GetType().FullName)
        //{
        //  case "Configuration_Editor.ProjectText":
        //    //{ // Do no checking here.
        //    //  if (false)
        //    //    allValid = false;
        //    //}
        //    break;
        //  case "Configuration_Editor.ProjectSpecie":
        //    {
        //      ProjectSpecie lviProjectSpecie = lvi.Tag as ProjectSpecie;
        //      if (!lviProjectSpecie.Valid)
        //      {
        //        faults += lviProjectSpecie.ToString() + "\n";
        //        allValid = false;
        //      }
        //      //if (!lstDBSpecies.Items.ContainsKey(lviProjectSpecie.Symbol))
        //      //  allValid = false;
        //    }
        //    break;
        //  case "Configuration_Editor.ProjectCalculation":
        //    {
        //      ProjectCalculation lviProjectCalculation = lvi.Tag as ProjectCalculation;
        //      if (!lviProjectCalculation.Valid)
        //      {
        //        faults += lviProjectCalculation.ToString() + "\n";
        //        allValid = false;
        //      }
        //    }
        //    break;
        //  case "Configuration_Editor.ProjectAttribute":
        //    {
        //      ProjectAttribute lviProjectAttribute = lvi.Tag as ProjectAttribute;
        //      if (!lviProjectAttribute.Valid)
        //      {
        //        faults += lviProjectAttribute.ToString() + "\n";
        //        allValid = false;
        //      }
        //    }
        //    break;
        //  case "Configuration_Editor.ProjectPage":
        //    {
        //      ProjectPage lviProjectPage = lvi.Tag as ProjectPage;
        //      if (!lviProjectPage.Valid)
        //      {
        //        faults += lviProjectPage.ToString() + "\n";
        //        allValid = false;
        //      }
        //    }
        //    break;
        //  case "Configuration_Editor.ProjectSum":
        //    {
        //      ProjectSum lviProjectSum = lvi.Tag as ProjectSum;
        //      if (!lviProjectSum.Valid)
        //      {
        //        faults += lviProjectSum.ToString() + "\n";
        //        allValid = false;
        //      }
        //    }
        //    break;
        //  default:
        //    throw new Exception();
        //    //break;
        //}
      }

      if (!allValid)
      {
        if (MessageBox.Show("The following entries are invalid:\n\n" + faults + "\n\nSave anyway?", "Entries invalid.", MessageBoxButtons.YesNo, MessageBoxIcon.Error) == DialogResult.No)
          return;
      }


      StreamWriter sw = null;
      try
      {
        //if (false) // Create backup...
        //{
        //  string filename = m_ConfigFile.Name;
        //  m_ConfigFile.Close();
        //  if (File.Exists(filename + ".bak"))
        //    File.Delete(filename + ".bak");
        //  File.Move(filename, filename + ".bak");
        //  File.Create(filename).Close();
        //  m_ConfigFile = new FileStream(filename, FileMode.Open, FileAccess.Write, FileShare.Read, 8192, FileOptions.SequentialScan);
        //}
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
        sw.WriteLine(projectVectorControl1.SaveString());
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

    bool oldFormat = false;
    Regex oldformatCheck = new Regex(@"S\d\d\d\d=\d,[^,]*,\d*,\d*,\d*");

    protected void LoadFile(string filename)
    {
      oldFormat = false;
      if (m_ConfigFile != null)
        try { m_ConfigFile.Close(); }
        catch { }

      m_ConfigFile = new FileStream(filename, FileMode.Open, FileAccess.ReadWrite, FileShare.Read, 8192, FileOptions.SequentialScan);

      string contents = new StreamReader(m_ConfigFile).ReadToEnd();

      if (oldformatCheck.IsMatch(contents))
      {
        if (MessageBox.Show("Old configuration file found.\nUpgrade to new configuration version format?", "Old configuration found.", MessageBoxButtons.OKCancel) == DialogResult.Cancel)
          Application.Exit();
        else
          oldFormat = true;
      }

      List<string> errors = LoadFromDictionary(ReadIni(contents, m_DefaultIniDictionary));
      if (errors.Count != 0)
      {
        StringBuilder sb = new StringBuilder("The following errors occured trying to load the file:\n");
        foreach (string error in errors)
          sb.AppendLine(error);
        MessageBox.Show(sb.ToString(), "Load Configuration", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
      else
      {
        this.Text = "SysCAD Configuration Editor - " + filename;
      }
    }

    Regex oldRegexSpecie = new Regex(@"(?<param1>\d),(?<param2>[^,]*),(?<param3>\d*),(?<param4>\d*),(?<param5>\d*)");
    Regex oldRegexText = new Regex(@"^(?<param1>[12345]),(?<param2>[^\b\n\r]*)$");

    protected List<string> LoadFromDictionary(Dictionary<string, Dictionary<string, string>> data)
    {
      List<string> ret = new List<string>();
      m_UnparsedInfo = new Dictionary<string, Dictionary<string, string>>();

      if (data.ContainsKey("general"))
      {
        Dictionary<string, string> General = data["general"];
        //FileVersion = Pop(General, "prjfileversion");
        txtStT.Text = Pop(General, "std_temp");
        txtStP.Text = Pop(General, "std_press");
        txtNT.Text = Pop(General, "norm_temp");
        txtNP.Text = Pop(General, "norm_press");
        txtMinT.Text = Pop(General, "minimum_temp");
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
        if (oldFormat)
          data.Add("oldspecies", new Dictionary<string, string>());

        foreach (string specieKey in new List<string>(data["species"].Keys))
        {
          if (oldFormat)
          {
            string specieValue = data["species"][specieKey];

            data["oldspecies"].Add(specieKey, "**Old Species***: " + specieValue);

            Match oldRegexSpecieMatch = oldRegexSpecie.Match(specieValue);
            if (oldRegexSpecieMatch.Success)
            {
              data["species"][specieKey] = "Specie,,,"
                + oldRegexSpecieMatch.Groups["param2"].ToString() + ","
                + oldRegexSpecieMatch.Groups["param2"].ToString() + ",*,,,,"
                + oldRegexSpecieMatch.Groups["param3"].ToString() + ","
                + oldRegexSpecieMatch.Groups["param4"].ToString() + ","
                + oldRegexSpecieMatch.Groups["param5"].ToString();
            }

            Match oldRegexTextMatch = oldRegexText.Match(specieValue);
            if (oldRegexTextMatch.Success)
            {
              if (oldRegexTextMatch.Groups["param1"].ToString() == "5")
              {
                data["species"][specieKey] = oldRegexTextMatch.Groups["param0"] + "="
                  + "Text,,"
                  + oldRegexTextMatch.Groups["param2"].ToString() + ",,,,,,,";
              }
              else
              {
                data["species"].Remove(specieKey);
              }
            }
          }
        }
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
        comboDefaultSolution.Text = Pop(Modes, "default_netmode");
        chkProbalAllowed.Checked = Pop(Modes, "probal_allowed") != "0";
        comboProBalSurge.Text = Pop(Modes, "probal_nodemode");
        comboProbalFlow.Text = Pop(Modes, "probal_flowmode");
        comboProBalHeat.Text = Pop(Modes, "probal_heatmode");

        chkDynamicAllowed.Checked = Pop(Modes, "dynamic_allowed") != "0";
        comboDynamicSurge.Text = Pop(Modes, "dynamic_nodemode");
        comboDynamicFlow.Text = Pop(Modes, "dynamic_flowmode");
        comboDynamicHeat.Text = Pop(Modes, "dynamic_heatmode");

        comboMaxCompSurge.Text = Pop(Modes, "maximum_nodemode");
        comboMaxCompFlow.Text = Pop(Modes, "maximum_flowmode");
        comboMaxCompHeat.Text = Pop(Modes, "maximum_heatmode");
      }

      m_UnparsedInfo = data;
      return ret;
    }

    protected static Regex CommentStripper = new Regex(@"^[^;\r\n]*", RegexOptions.Compiled | RegexOptions.Multiline);
    protected static Regex iniRegex1 = new Regex(@"(^\[(?<Title>[\w ]+)\](?<Values>.*?))*\Z",
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
      Dictionary<string, Dictionary<string, string>> ret = new Dictionary<string, Dictionary<string, string>>();
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
      ////Determine max lengths for each column:
      //Dictionary<string, int> MaxLengths = new Dictionary<string, int>();
      //Dictionary<string, string> MaxLengthNames = new Dictionary<string, string>();
      //foreach (DataColumn c in m_SpecieDataTable.Columns)
      //  MaxLengths[c.ColumnName] = 0;

      //foreach (DataRow r in m_SpecieDataTable.Rows)
      //  if (r.RowState != DataRowState.Deleted)
      //    foreach (DataColumn c in m_SpecieDataTable.Columns)
      //      if (r[c] is string && MaxLengths[c.ColumnName] < ((string)r[c]).Length)
      //      {
      //        MaxLengths[c.ColumnName] = ((string)r[c]).Length;
      //        MaxLengthNames[c.ColumnName] = (string)r["Name"];
      //      }
      //if (m_SpecieDataAdapter != null)
      //  m_SpecieDataAdapter.Update(m_SpecieDataTable);
    }

    protected void PKHOpenDatabase(string filename)
    {
      string tableName = "Species";

      OleDbConnection conn = new OleDbConnection(@"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + filename);
      conn.Open();
      //Putting things in Try {} Catch{} is the worst form of flow control, or so they say.
      //But there seems no other option
      DataTable t = new DataTable();
      OleDbDataAdapter da = new OleDbDataAdapter("SELECT * FROM " + tableName, conn);
      da.FillSchema(t, SchemaType.Source);

      m_SpecieDataTable.PrimaryKey = new DataColumn[0];

      m_SpecieDataAdapter = new OleDbDataAdapter("SELECT * FROM " + tableName, conn);
      m_SpecieDataTable.Clear();
      m_SpecieDataAdapter.FillSchema(m_SpecieDataTable, SchemaType.Source);
      m_SpecieDataAdapter.Fill(m_SpecieDataTable);

      new OleDbCommandBuilder(m_SpecieDataAdapter);

      UpdateFilter();

      conn.Close();

      //Create a dummy row to prevent any sort of errors...
      if (m_SpecieDataTable.Rows.Count == 0)
      {
        CreateSpecies();
        //tcSpecies.SelectedTab = tabDatabase;
      }

      //specieDatabaseControl1.DatabaseChanged();
    }

    protected void OldOpenDatabase(string filename)
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
          }*/
      //It seems that it really doesn't like these constraints for an unkown reason.


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

      //Create a dummy row to prevent any sort of errors...
      if (m_SpecieDataTable.Rows.Count == 0)
      {
        CreateSpecies();
        //tcSpecies.SelectedTab = tabDatabase;
      }

      //specieDatabaseControl1.DatabaseChanged();
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

    protected Dictionary<Pair<float>, string> SeperateVariousTemps(string s, out bool single)
    {
      Match m = SpecieDatabaseControl.EquationSplitterRegex.Match(s);
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
                if (lst[i].s1 <= lst[j].s2 && lst[i].s2 >= lst[j].s1)
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
    protected void LoadDBIni(string filename)
    {
      string s = new StreamReader(new FileStream(filename, FileMode.Open)).ReadToEnd();
      Dictionary<string, Dictionary<string, string>> data = ReadIni(s, new Dictionary<string, Dictionary<string, string>>());
      //specieDatabaseControl1.LoadDBIni(data);
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





    #endregion Specie DB stuff

    protected void ReadConversions()
    {
      string file;
      if (File.Exists("UnitConversions.ini"))
        file = File.ReadAllText("UnitConversions.ini", Encoding.UTF7);
      else if (File.Exists("..\\..\\UnitConversions.ini"))
        file = File.ReadAllText("..\\..\\UnitConversions.ini", Encoding.UTF7); //Allow the database.ini to sit in the same folder as the source files
      else
        throw new Exception("UnitConversions.ini not found.");
      Dictionary<string, Dictionary<string, string>> Units = ReadIni(file, new Dictionary<string, Dictionary<string, string>>());

      foreach (KeyValuePair<string, Dictionary<string, string>> kvp in Units)
      {
        BindingSource current = new BindingSource();
        current.DataSource = typeof(Conversion);

        List<string> OrderedKeys = new List<string>(kvp.Value.Keys);
        OrderedKeys.Sort();
        foreach (string s in OrderedKeys)
          try
          {
            if (s.StartsWith("cnv"))
            {
              Match m = s_ConversionRegex.Match(kvp.Value[s]);
              if (m.Success)
                current.Add(new Conversion(
                    double.Parse(m.Groups["Offset"].Value),
                    double.Parse(m.Groups["Factor"].Value),
                    m.Groups["Name"].Value));
            }
          }
          catch { }

        current.CurrentChanged += new EventHandler(ForceFormatting);
        m_UnitBindings[kvp.Key] = current;
        m_Formatters[kvp.Key] = new FormatterParserProvider(current);
      }
      m_FrmUnitSelection.UnitBindings = m_UnitBindings;
    }

    protected void UpdateStatusBar()
    {
      if (tcMain.SelectedTab == this.tabSpecies)
      {
        if (tcSpecies.SelectedTab == this.tabProjectSpecies)
        {
          statusLabel.Text = projectVectorControl1.StatusMessage;
          statusLabel.ForeColor = projectVectorControl1.StatusColour;
        }
        else
          statusLabel.Text = "";
      }
      else
        statusLabel.Text = "";
    }

    protected void UpdateFilter()
    {
      lstDBSpecies.BeginUpdate(); m_UpdateCalls++;
      lstDBSpecies.Items.Clear();
      List<DataRow> usedRows = projectVectorControl1.UsedSpecies;
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

          if (!lstDBSpecies.Items.ContainsKey(symb))
          {
            ListViewItem lvi = lstDBSpecies.Items.Add(symb, symb, 0);
            lvi.SubItems.Add((string)r["Name"]);
            lvi.Tag = r;
            if (usedRows.Contains(r))
              lvi.Font = new Font(lvi.Font, FontStyle.Bold);
          }
        }
      }
      lstDBSpecies.EndUpdate(); m_UpdateCalls--; ;
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

    void projectVectorControl1_UsedSpeciesChanged(object sender, EventArgs e)
    {
      List<DataRow> usedRows = projectVectorControl1.UsedSpecies;
      foreach (ListViewItem lvi in lstDBSpecies.Items)
        if (usedRows.Contains((DataRow)lvi.Tag))
          lvi.Font = new Font(lvi.Font, FontStyle.Bold);
        else
          lvi.Font = new Font(lvi.Font, FontStyle.Regular);
    }

    void specieDatabaseControl1_UpdateLVIsRequired(object sender, EventArgs e)
    {
      UpdateDBLVIs();
      projectVectorControl1.UpdateProjectLVIs();
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

        PKHOpenDatabase(dlgOpenDB.FileName);
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

    private void FireUpdateStatusBar(object sender, EventArgs e)
    {
      UpdateStatusBar();
    }

    private void menuSpDBContext_Opening(object sender, CancelEventArgs e)
    {
      //menuDelete.Enabled = lstDBSpecies.SelectedItems.Count > 0;

      DataRow[] values = new DataRow[lstDBSpecies.SelectedItems.Count];
      int i = 0;
      foreach (ListViewItem lvi in lstDBSpecies.SelectedItems)
        values[i++] = (DataRow)lvi.Tag;
      menuSpDBAddToProject.Enabled = lstDBSpecies.SelectedItems.Count > 0
          && projectVectorControl1.CanInsert(values);
    }

    private void lstDBSpecies_ItemActivate(object sender, EventArgs e)
    {
      if (tcSpecies.SelectedTab == tabProjectSpecies)
      {
        DataRow r = (DataRow)(lstDBSpecies.SelectedItems[0].Tag);
        projectVectorControl1.AddToCalculation("[" + r["Compound"] + "(" + r["Phase"] + ")]");
      }
    }

    private void txtFilter_TextChanged(object sender, EventArgs e)
    {
      UpdateFilter();
    }

    private void MenuSort_Click(object sender, EventArgs e)
    {
      projectVectorControl1.Sort();
    }

    private void menuAdvancedSort_Click(object sender, EventArgs e)
    {
      PVIOrderer oldSorter = projectVectorControl1.Sorter.Clone();
      if (m_SortOptions.ShowDialog(this) == DialogResult.OK)
        projectVectorControl1.Sort();
      else
      {
        projectVectorControl1.Sorter = oldSorter;
        m_SortOptions.Sorter = oldSorter;
      }
    }

    private void lstDBSpecies_SelectedIndexChanged(object sender, EventArgs e)
    {
      if (lstDBSpecies.SelectedItems.Count > 0 && m_UpdateCalls < 1)
      {
        try
        {
          this.BindingContext[m_SpecieDataTable].Position = m_SpecieDataTable.Rows.IndexOf((DataRow)lstDBSpecies.SelectedItems[0].Tag);
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

    private void menuOpen_Click(object sender, EventArgs e)
    {
      try
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
          dlgOpenDB.Title = "Please select a specie database to open before opening a configuration file";
          DialogResult d = dlgOpenDB.ShowDialog();
          dlgOpenDB.Title = t;

          if (d != DialogResult.OK)
            return;
          PKHOpenDatabase(dlgOpenDB.FileName);
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
      projectVectorControl1.TransferSpecies();
      projectVectorControl1.Select();
    }

    private void menuNew_Click(object sender, EventArgs e)
    {
      if (m_ConfigFile != null)
      {
        switch (MessageBox.Show("Do you wish to save changes to " + Path.GetFileName(m_ConfigFile.Name) + "?", "SysCAD Configuration Editor", MessageBoxButtons.YesNoCancel))
        {
          case DialogResult.Yes: Save(); break;
          case DialogResult.Cancel: return;
          case DialogResult.No: break;
        }
        m_ConfigFile.Close();
        m_ConfigFile = null;
      }
      if (m_SpecieDataTable.Rows.Count == 0)
      {
        string t = dlgOpenDB.Title;
        dlgOpenDB.Title = "Please select a specie database to open before creating a configuration file";
        DialogResult d = dlgOpenDB.ShowDialog();
        dlgOpenDB.Title = t;

        if (d != DialogResult.OK)
          return;
        PKHOpenDatabase(dlgOpenDB.FileName);
      }
      Dictionary<string, Dictionary<string, string>> tmp = new Dictionary<string, Dictionary<string, string>>();
      foreach (KeyValuePair<string, Dictionary<string, string>> kvp in m_DefaultIniDictionary)
        tmp[kvp.Key] = new Dictionary<string, string>(kvp.Value);

      this.Text = "SysCAD Configuration Editor - " + "New File";

      LoadFromDictionary(tmp);
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
      menuSave.Enabled = m_ConfigFile != null;
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
      if (MessageBox.Show("Are you sure you wish to delete the selected species from the Specie Database and Project Vector?", "SysCAD Configuration Editor", MessageBoxButtons.YesNo, MessageBoxIcon.Warning, MessageBoxDefaultButton.Button2) == DialogResult.Yes)
        DeleteSelectedSpecies();
    }

    protected void DeleteSelectedSpecies()
    {
      ArrayList selectedItems = new ArrayList(lstDBSpecies.SelectedItems);

      lstDBSpecies.BeginUpdate(); m_UpdateCalls++;
      foreach (ListViewItem lvi in selectedItems)
      {
        lstDBSpecies.Items.Remove(lvi);
        ((DataRow)lvi.Tag).Delete();

        projectVectorControl1.RemoveSpecies((DataRow)lvi.Tag);
      }
      UpdateFilter();
      if (lstDBSpecies.Items.Count > 0)
        lstDBSpecies.Items[0].Selected = true;
      else
        CreateSpecies();
      lstDBSpecies.EndUpdate(); m_UpdateCalls--;
      SaveDatabase();
      lstDBSpecies_SelectedIndexChanged(this, new EventArgs());
      ((CurrencyManager)this.BindingContext[m_SpecieDataTable]).Refresh();
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
      //tcSpecies.SelectedTab = tabDatabase;
    }

    private void menuNewSpecies_Click(object sender, EventArgs e)
    {
      CreateSpecies();
    }

    private void menuUnits_Click(object sender, EventArgs e)
    {
      Dictionary<string, int> oldPos = new Dictionary<string, int>();
      foreach (KeyValuePair<string, BindingSource> kvp in m_UnitBindings)
        oldPos[kvp.Key] = kvp.Value.Position;
      if (m_FrmUnitSelection.ShowDialog() == DialogResult.Cancel)
        foreach (KeyValuePair<string, int> kvp in oldPos)
          m_UnitBindings[kvp.Key].Position = kvp.Value;
    }

    private void ForceFormatting(object sender, EventArgs e)
    {
      ((CurrencyManager)this.BindingContext[m_SpecieDataTable]).Refresh();
    }

    private void menuEdit_DropDownOpening(object sender, EventArgs e)
    {
      menuAdvancedSort.Visible = MenuSort.Visible = tcMain.SelectedTab == tabSpecies &&
          tcSpecies.SelectedTab == tabProjectSpecies;

      MenuSort.Text = projectVectorControl1.SelectionSize > 1 ?
          "&Quick Sort Selection" : "&Quick Sort";
    }

    private void lstDBSpecies_KeyDown(object sender, KeyEventArgs e)
    {
      if (e.Control && e.KeyCode == Keys.A)
        foreach (ListViewItem lvi in lstDBSpecies.Items)
          lvi.Selected = true;
    }
  }
}