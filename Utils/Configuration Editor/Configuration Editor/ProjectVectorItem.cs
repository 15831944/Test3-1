using System;
using System.Windows.Forms;
using System.Text.RegularExpressions;
using System.Collections.Generic;
using System.Data;
using System.Drawing;

namespace Configuration_Editor
{
  public abstract class ProjectVectorItem : IComparable
  {
    #region Variables
    protected ListViewItem m_LVI;
    protected string m_sName;
    protected bool m_bValid = true;
    protected string m_sStatusDetails;
    #endregion Variables

    public bool IsValidTag(string tag)
    {
      if (tag.Length == 0) return false;

      if (char.IsDigit(tag[0])) return false;

      if ((tag[0] == '\\') || (tag[0] == '=')) return false;

      if ((tag.Contains("."))
        || (tag.Contains(","))
        || (tag.Contains("%"))
        || (tag.Contains("\t"))
        || (tag.Contains("\n"))
        || (tag.Contains("\f"))
        || (tag.Contains("\r"))
        || (tag.Contains("{"))) return false;

      foreach (char c in tag)
      {
        if (!char.IsDigit(c)) return true;
      }

      return false;
    }

    public ProjectVectorItem()
    {
      m_LVI = new ListViewItem(new string[] { "", "", "" });
      m_LVI.Tag = this;
    }

    #region Properties
    public virtual string StatusDetails
    {
      get { return m_sStatusDetails; }
      set
      {
        m_sStatusDetails = value;
        FireChanged();
      }
    }

    public virtual bool Valid
    {
      get { return m_bValid; }
      set
      {
        m_bValid = value;
        if (value == true)
          m_sStatusDetails = "";
        UpdateLVI();
        FireChanged();
      }
    }

    public ListViewItem LVI
    {
      get { return m_LVI; }
      set
      {
        m_LVI = value;
        m_LVI.Tag = this;
        UpdateLVI();
        FireChanged();
      }
    }

    public virtual String Name
    {
      get { return m_sName; }
      set
      {
        if (m_sName == value)
          return;
        m_sName = value;
        UpdateLVI();
        FireChanged();
      }
    }

    public abstract string Value
    {
      get;
      set;
    }
    #endregion Properties

    #region Protected Functions

    #endregion Protected Functions

    #region Public Functions
    public override string ToString()
    {
      return Name;
    }

    public virtual void UpdateLVI()
    {
      if (m_LVI != null)
      {
        m_LVI.Text = this.ToString();

        m_LVI.ForeColor = m_bValid ? SystemColors.WindowText : Color.Red;
      }
    }

    public abstract string ToSaveString();

    public static ProjectVectorItem Parse(string s, DataTable t, List<string> ErrorList)
    {
      Match m = ProjectSpecie.ParseRegex.Match(s);
      if (m.Success)
        return ProjectSpecie.Parse(m, t, ErrorList);
      m = ProjectText.ParseRegex.Match(s);
      if (m.Success)
        return ProjectText.Parse(m);
      m = ProjectCalculation.ParseRegex.Match(s);
      if (m.Success)
        return ProjectCalculation.Parse(m);
      m = ProjectAttribute.ParseRegex.Match(s);
      if (m.Success)
        return ProjectAttribute.Parse(m);
      m = ProjectPage.ParseRegex.Match(s);
      if (m.Success)
        return ProjectPage.Parse(m);
      m = ProjectSum.ParseRegex.Match(s);
      if (m.Success)
        return ProjectSum.Parse(m);
      return null;
    }
    #endregion Public Functions

    #region IComparable Members

    public virtual int CompareTo(object obj)
    {
      if (!(obj is ProjectVectorItem))
        throw new ArgumentException();
      return m_sName.CompareTo(((ProjectVectorItem)obj).m_sName);
    }

    #endregion

    #region Events
    public event EventHandler Changed;
    public void FireChanged() { if (Changed != null) Changed(this, new EventArgs()); }
    #endregion Events
  }

  public enum Phase { Unkown = -1, Solid = 1, Liquid, Gas, Total }
  public class ProjectSpecie : ProjectVectorItem
  {
    public static Regex ParseRegex = new Regex(
            @"(Specie(,[^,]*){2},(?<Symbol>[^,]+)((,[^,]*){4},(?<ToleranceBelow>\d+(\.\d+)?|\.\d+)?,(?<ToleranceAbove>\d+(\.\d+)?|\.\d+)?,(?<Ideal>\d+)?)?)|" + //New Method
            @"(0                ,(?<Symbol>[^,]+)(           ,(?<ToleranceBelow>\d+(\.\d+)?|\.\d+)?,(?<ToleranceAbove>\d+(\.\d+)?|\.\d+)?,(?<Ideal>\d+)?)?)", //Old Method
        RegexOptions.Compiled | RegexOptions.IgnoreCase | RegexOptions.IgnorePatternWhitespace | RegexOptions.ExplicitCapture);
    //For ease of sorting: Phase will be cached:

    protected Phase m_eCachedPhase = (Phase)(-1);
    public static bool CachePhase = false;
    public static bool SortByPhase;
    public static List<Phase> PhaseOrder = new List<Phase>();

    protected string m_sComp, m_sPhase;
    protected string m_sSymbol;
    protected DataRow m_DataRow;
    protected double m_dExtrapBelowT, m_dExtrapAboveT;
    protected bool m_bIdeal;

    public ConvertEventHandler TempFormatter;
    public ConvertEventHandler TempParser;

    protected double FormatT(double d, bool IncludeOffset)
    {
      ConvertEventArgsEx e = new ConvertEventArgsEx(d, typeof(double), IncludeOffset);
      if (TempFormatter != null)
        TempFormatter(this, e);
      return (double)e.Value;
    }

    protected double ParseT(double d, bool IncludeOffset)
    {
      ConvertEventArgsEx e = new ConvertEventArgsEx(d, typeof(double), IncludeOffset);
      if (TempParser != null)
        TempParser(this, e);
      return (double)e.Value;
    }

    public static Regex CompoundSeperator = new Regex(@"^(?<Compound>.*)\((?<Phase>[^()]+)\)$", RegexOptions.Compiled);

    //public static DataTable sSpecieDataTable;

    public DataRow SpDataRow
    {
      get { return m_DataRow; }
    }

    public ProjectSpecie(DataTable t, string symbol, List<string> ErrorList)
    {
      m_sSymbol = symbol;

      Match m = CompoundSeperator.Match(m_sSymbol);
      m_sComp = m.Groups["Compound"].Value;
      m_sPhase = m.Groups["Phase"].Value;
      foreach (DataRow r in t.Rows)
        if ((string)r["Compound"] == m_sComp && (string)r["Phase"] == m_sPhase)
        {
          m_DataRow = r;
          break;
        }
      if (m_DataRow == null)
        ErrorList.Add("Compound '" + symbol + "'Not Found in Database");
      UpdateLVI();
    }

    public ProjectSpecie(DataRow row)
    {
      m_DataRow = row;
      m_sComp = (string)row["Compound"];
      m_sPhase = (string)row["Phase"];
      m_sSymbol = m_sComp + "(" + m_sPhase + ")";
      UpdateLVI();
    }

    public Phase Phase
    {
      get
      {
        if (m_DataRow == null) return Phase.Unkown;
        if (m_eCachedPhase == (Phase)(-1) || !CachePhase)
        {
          string occ = (string)m_DataRow["Occurence"];
          if (occ == "g")
            return m_eCachedPhase = Phase.Gas;
          else if (occ == "l")
            return m_eCachedPhase = Phase.Liquid;
          else
            return m_eCachedPhase = Phase.Solid;
        }
        else
          return m_eCachedPhase;
      }
    }

    public override string Value
    {
      get
      {
        return Symbol;
      }
      set
      {
      }
    }

    public override void UpdateLVI()
    {
      if (m_DataRow != null)
      {
        m_LVI.SubItems[0].Text = Symbol;
        m_LVI.SubItems[1].Text = MinAllowedTemp.ToString("0.##");//((float)m_DataRow["Ts"] - m_dExtrapBelowT).ToString();
        m_LVI.SubItems[2].Text = MaxAllowedTemp.ToString("0.##");//((float)m_DataRow["Te"] + m_dExtrapAboveT).ToString();
        m_LVI.ForeColor = SystemColors.WindowText;
      }
      else
      {
        m_LVI.SubItems[0].Text = m_sSymbol;
        m_LVI.SubItems[1].Text = m_LVI.SubItems[2].Text = "*";
        m_LVI.ForeColor = Color.Red;
      }
    }

    public override string Name
    {
      get
      {
        if (m_DataRow != null)
          return (string)m_DataRow["Name"];
        else
          return "Unkown";
      }
      set
      {
        //Do Nothing
      }
    }

    public double ExtraBelowT
    {
      get { return FormatT(m_dExtrapBelowT, false); }
      set
      {
        value = ParseT(value, false);
        if (m_dExtrapBelowT == value) return;
        m_dExtrapBelowT = value;
        UpdateLVI();
        FireChanged();
      }
    }

    public double ExtraAboveT
    {
      get { return FormatT(m_dExtrapAboveT, false); }
      set
      {
        value = ParseT(value, false);
        if (m_dExtrapAboveT == value) return;
        m_dExtrapAboveT = value;
        UpdateLVI();
        FireChanged();
      }
    }

    public double MinAllowedTemp
    {
      get
      {
        if (m_DataRow != null)
        {
          float minDefined = (float)m_DataRow["Ts"];
          return FormatT((minDefined - m_dExtrapBelowT < Program.ZeroK) ? Program.ZeroK : minDefined - m_dExtrapBelowT, true);
        }
        else
          return -1;
      }
    }

    public double MinDefinedTemp
    {
      get
      {
        if (m_DataRow != null)
          return FormatT((float)m_DataRow["Ts"], true);
        else
          return -1;
      }
    }

    public double MaxAllowedTemp
    {
      get
      {
        if (m_DataRow != null)
        {
          float maxDefined = (float)m_DataRow["Te"];
          return FormatT(maxDefined + m_dExtrapAboveT, true);
        }
        else
          return -1;
      }
    }

    public double MaxDefinedTemp
    {
      get
      {
        if (m_DataRow != null)
          return FormatT((float)m_DataRow["Te"], true);
        else
          return -1;
      }
    }


    public bool Ideal
    {
      get { return m_bIdeal; }
      set
      {
        m_bIdeal = value;
        FireChanged();
      }
    }

    public string Symbol
    {
      get
      {
        if (m_DataRow != null)
          return (string)m_DataRow["Compound"] + "(" + m_DataRow["Phase"] + ")";
        else
          return m_sSymbol;
      }
    }

    public override int CompareTo(object obj)
    {
      if (!(obj is ProjectSpecie))
        return base.CompareTo(obj);

      ProjectSpecie other = (ProjectSpecie)obj;
      if (SortByPhase && other.Phase != Phase)
        try
        {
          return (PhaseOrder.IndexOf(Phase)).CompareTo(PhaseOrder.IndexOf(other.Phase));
        }
        catch
        {
          return ((int)Phase).CompareTo((int)other.Phase);
        }
      return Symbol.CompareTo(other.Symbol);
    }

    public override string ToSaveString()
    {
      //      Type,  Parent,Tag,Symbol,SDB Tag,Cnv,OccBasis,PhBasis,Spare,Info
      return "Specie,,," + Symbol + "," + Symbol + ",*,,,," + m_dExtrapBelowT + "," + m_dExtrapAboveT + "," + (Ideal ? "1" : "0"); //No name? Oh well...
    }

    public static ProjectSpecie Parse(Match m, DataTable t, List<string> ErrorList)
    {
      ProjectSpecie ret = new ProjectSpecie(t, m.Groups["Symbol"].Value, ErrorList);
      try
      {
        if (m.Groups["ToleranceBelow"].Success)
          ret.ExtraBelowT = double.Parse(m.Groups["ToleranceBelow"].Value);
        if (m.Groups["ToleranceAbove"].Success)
          ret.ExtraAboveT = double.Parse(m.Groups["ToleranceAbove"].Value);
        if (m.Groups["Ideal"].Success)
          ret.Ideal = m.Groups["Ideal"].Value != "0";
      }
      catch
      {
      }
      return ret;
    }
  }

  public class ProjectText : ProjectVectorItem
  {
    public static Regex ParseRegex = new Regex(@"(Text,[^,]*,(?<Value>[^,]*)) | (5,(?<Value>[^,]*))", RegexOptions.Compiled |
        RegexOptions.IgnoreCase | RegexOptions.IgnorePatternWhitespace | RegexOptions.ExplicitCapture);

    public ProjectText()
    {
      LVI.ImageKey = "Text";
    }

    public override string Value
    {
      get
      {
        return m_sName;
      }
      set
      {
        m_sName = value;
        UpdateLVI();
        FireChanged();
      }
    }

    public override string ToSaveString()
    {
      return "Text,," + Value.Replace(',', '.') + ",,,,,,,";
    }

    public static ProjectText Parse(Match m)
    {
      ProjectText ret = new ProjectText();
      ret.Value = m.Groups["Value"].Value;
      return ret;
    }
  }

  public class ProjectCalculation : ProjectVectorItem
  {
    public static Regex ParseRegex = new Regex(@"Calculation(,[^,]*),(?<Desc>[^,]*),(?<Symbol>[^,]+)(,[^,]*){5},(?<Value>[^,]*)",
        RegexOptions.Compiled | RegexOptions.IgnorePatternWhitespace | RegexOptions.IgnoreCase | RegexOptions.ExplicitCapture);
    protected string m_sValue = "";
    protected string m_sDesc = "";

    public ProjectCalculation()
    {
      LVI.ImageKey = "Calculation";
    }

    public override bool Valid
    {
      get
      {
        return ((base.Valid) 
          && (!string.IsNullOrEmpty(Name)) 
          && (!string.IsNullOrEmpty(m_sValue)) 
          && (IsValidTag(Name)) 
          && (IsValidTag(Desc)));
      }
      set
      {
        base.Valid = value;
      }
    }

    public override string Value
    {
      get { return m_sValue; }
      set
      {
        m_sValue = value;
        UpdateLVI();
        FireChanged();
      }
    }

    public string Desc
    {
      get { return m_sDesc; }
      set
      {
        m_sDesc = value;
        UpdateLVI();
        FireChanged();
      }
    }

    public override string ToString()
    {
      return Name + ": " + Regex.Replace(Value, "\\s+", " ");
    }

    public override string ToSaveString()
    {
      return "Calculation,," + Desc + "," + Name + ",,*,,,," + Regex.Replace(Value, "\\s+", " ");
    }

    public static ProjectCalculation Parse(Match m)
    {
      ProjectCalculation ret = new ProjectCalculation();
      ret.Desc = m.Groups["Desc"].Value;
      ret.Name = m.Groups["Symbol"].Value;
      ret.Value = m.Groups["Value"].Value;
      return ret;
    }
  }

  public enum AttributeType { Extensive, Intensive }
  public class ProjectAttribute : ProjectVectorItem
  {
    public static Regex ParseRegex = new Regex("Attribute,(?<Parent>[^,]*),[^,]*,(?<Name>[^,]*),[^,]*,(?<Cnv>[^,]*)(,[^,]*){3},(?<Type>[^,]*)", RegexOptions.IgnoreCase | RegexOptions.Compiled);
    protected AttributeType m_eType;
    protected string m_sParent;
    protected string m_sCnv;

    public ProjectAttribute()
    {
      LVI.ImageKey = "Attribute";
    }

    #region Properties
    public override string Value
    {
      get
      {
        return m_sName;
      }
      set
      {
        m_sName = value;
        UpdateLVI();
        FireChanged();
      }
    }

    public override bool Valid
    {
      get
      {
        return ((base.Valid) 
          && (!string.IsNullOrEmpty(Value.Trim()))
          && (IsValidTag(Name)));
      }
      set
      {
        base.Valid = value;
      }
    }

    public AttributeType AttributeType
    {
      get { return m_eType; }
      set
      {
        m_eType = value;
        UpdateLVI();
        FireChanged();
      }
    }

    public string Parent
    {
      get { return m_sParent; }
      set
      {
        m_sParent = value;
        UpdateLVI();
        FireChanged();
      }
    }

    public string Cnv
    {
      get { return m_sCnv; }
      set
      {
        m_sCnv = value;
        UpdateLVI();
        FireChanged();
      }
    }
    #endregion Properties

    #region Functions
    public override string ToString()
    {
      return Name + ": {Parent - " + Parent + ", " + "Type - " + AttributeType + "}";
    }

    public override string ToSaveString()
    {
      return "Attribute," + Parent + ",," + Name + ",," + Cnv + ",,,," + AttributeType;
    }

    public static ProjectAttribute Parse(Match m)
    {
      ProjectAttribute ret = new ProjectAttribute();
      ret.Name = m.Groups["Name"].Value;
      ret.AttributeType = m.Groups["Type"].Value.ToLower() == "quantity" || m.Groups["Type"].Value.ToLower() == "extensive" ? AttributeType.Extensive : AttributeType.Intensive;
      ret.Cnv = m.Groups["Cnv"].Value;
      ret.Parent = m.Groups["Parent"].Value;
      return ret;
    }
    #endregion Functions
  }

  public class ProjectPage : ProjectVectorItem
  {
    public static Regex ParseRegex = new Regex(@"RqdPage(,[^,]*),(?<Name>[^,]*)", RegexOptions.Compiled | RegexOptions.IgnoreCase | RegexOptions.ExplicitCapture);

    public ProjectPage()
    {
      LVI.ImageKey = "PageBreak";
    }

    public override string Value
    {
      get
      {
        return m_sName;
      }
      set
      {
        m_sName = value;
        UpdateLVI();
        FireChanged();
      }
    }

    public override string ToSaveString()
    {
      return "RqdPage,," + Name + "," + Name + ",,,,,,,";
    }

    public static ProjectPage Parse(Match m)
    {
      ProjectPage ret = new ProjectPage();
      ret.Name = m.Groups["Name"].Value;
      return ret;
    }
  }

  public class ProjectSum : ProjectVectorItem
  {
    public static Regex ParseRegex = new Regex(@"(?<Val>1|2|3|4),", RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled);
    public Phase m_ePhase;

    public ProjectSum(Phase phase)
    {
      m_ePhase = phase;
      LVI.ImageKey = "Sum";
      UpdateLVI();
    }
    public override string Value
    {
      get
      {
        return m_ePhase.ToString();
      }
      set
      {
        //Can't touch this!
      }
    }

    public override string Name
    {
      get
      {
        return Value;
      }
      set
      {
        //Can't touch this!
      }
    }

    public override int CompareTo(object obj)
    {
      if (!(obj is ProjectSum))
        return base.CompareTo(obj);
      else
        return ProjectSpecie.PhaseOrder.IndexOf(m_ePhase).CompareTo(ProjectSpecie.PhaseOrder.IndexOf(((ProjectSum)obj).m_ePhase));
    }

    public override string ToSaveString()
    {
      return ((int)m_ePhase).ToString() + "," + m_ePhase.ToString();
      //'s right!
    }

    public Phase Phase
    {
      get { return m_ePhase; }
    }

    public static ProjectSum Parse(Match m)
    {
      return new ProjectSum((Phase)int.Parse(m.Groups["Val"].Value));
    }
  }
}