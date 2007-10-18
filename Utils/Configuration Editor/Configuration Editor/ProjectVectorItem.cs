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
        #endregion Variables

        public ProjectVectorItem()
        {
            m_LVI = new ListViewItem(new string[] { "", "", "" });
            m_LVI.Tag = this;
        }

        #region Properties
        public ListViewItem LVI
        {
            get { return m_LVI; }
            set
            {
                m_LVI = value;
                m_LVI.Tag = this;
                UpdateLVI();
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
            }
        }

        public abstract string Value
        {
            get;
            set;
        }
        #endregion Properties

        #region Protected Functions
        protected virtual void UpdateLVI()
        {
            if (m_LVI != null)
                m_LVI.Text = this.ToString();
        }
        #endregion Protected Functions

        #region Public Functions
        public override string ToString()
        {
            return Name;
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
            m_sComp = (string) row["Compound"];
            m_sPhase = (string) row["Phase"];
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
                return m_sSymbol;
            }
            set
            {
            }
        }

        protected override void UpdateLVI()
        {
            m_LVI.SubItems[0].Text = m_sSymbol;
            if (m_DataRow != null)
            {
                m_LVI.SubItems[1].Text = ((float)m_DataRow["Ts"] - m_dExtrapBelowT).ToString();
                m_LVI.SubItems[2].Text = ((float)m_DataRow["Te"] + m_dExtrapAboveT).ToString();
                m_LVI.ForeColor = SystemColors.WindowText;
            }
            else
            {
                m_LVI.SubItems[1].Text = m_LVI.SubItems[2].Text = "*";
                m_LVI.ForeColor = Color.Red;
            }
        }

        public override string  Name
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
            get { return m_dExtrapBelowT; }
            set
            {
                if (m_dExtrapBelowT == value) return;
                m_dExtrapBelowT = value;
                UpdateLVI();
            }
        }

        public double ExtraAboveT
        {
            get { return m_dExtrapAboveT; }
            set
            {
                if (m_dExtrapAboveT == value) return;
                m_dExtrapAboveT = value;
                UpdateLVI();
            }
        }

        public double MinAllowedTemp
        {
            get
            {
                if (m_DataRow != null)
                {
                    float minDefined = (float)m_DataRow["Ts"];
                    return (minDefined - m_dExtrapBelowT < Program.ZeroK) ? Program.ZeroK : minDefined - m_dExtrapBelowT;
                }
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
                    return maxDefined + m_dExtrapAboveT;
                }
                else
                    return -1;
            }
        }

        public bool Ideal
        {
            get { return m_bIdeal; }
            set { m_bIdeal = value; }
        }

        public string Symbol
        {
            get
            {
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
            return "Specie,,," + Symbol + "," + Symbol + ",*,,,," + ExtraBelowT + "," + ExtraAboveT + "," + (Ideal ? "1" : "0"); //No name? Oh well...
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
        public static Regex ParseRegex = new Regex(@"Calculation(,[^,]*),(?<Name>[^,]*),(?<Symbol>[^,]+)(,[^,]*){5},(?<Value>[^,]*)",
            RegexOptions.Compiled | RegexOptions.IgnorePatternWhitespace | RegexOptions.IgnoreCase | RegexOptions.ExplicitCapture);
        protected string m_sValue;
        protected string m_sSymbol;

        public ProjectCalculation()
        {
            LVI.ImageKey = "Calculation";
        }

        public override string Value
        {
            get { return m_sValue; }
            set
            {
                m_sValue = value;
                UpdateLVI();
            }
        }

        public string Symbol
        {
            get { return m_sSymbol; }
            set
            {
                m_sSymbol = value;
                UpdateLVI();
            }
        }

        public override string ToString()
        {
            return m_sSymbol + ": " + Regex.Replace(Value, "\\s+", " ");
        }

        public override string ToSaveString()
        {
            return "Calculation,," + Name + "," + Symbol + ",,*,,,," + Value;
        }

        public static ProjectCalculation Parse(Match m)
        {
            ProjectCalculation ret = new ProjectCalculation();
            ret.Name = m.Groups["Name"].Value;
            ret.Symbol = m.Groups["Symbol"].Value;
            ret.Value = m.Groups["Value"].Value;
            return ret;
        }
    }

    public enum AttributeType { Extensive, Intensive }
    public class ProjectAttribute : ProjectVectorItem
    {
        public static Regex ParseRegex = new Regex("Attribute,[^,]*,(?<Name>[^,]*)", RegexOptions.IgnoreCase | RegexOptions.Compiled);
        protected AttributeType m_eType;

        public ProjectAttribute()
        {
            LVI.ImageKey = "Attribute";
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
            }
        }

        public AttributeType AttributeType
        {
            get { return m_eType; }
            set 
            {
                m_eType = value;
                UpdateLVI();
            }
        }

        public override string ToSaveString()
        {
            return "Attribute,," + Name; //TODO: Find out what this is all about, 'eh?
        }

        public static ProjectAttribute Parse(Match m)
        {
            ProjectAttribute ret = new ProjectAttribute();
            ret.Name = m.Groups["Name"].Value;
            return ret;
        }
    }

    public class ProjectPage : ProjectVectorItem
    {
        public static Regex ParseRegex = new Regex(@"RqdPage(,[^,]*){2},(?<Name>)", RegexOptions.Compiled | RegexOptions.IgnoreCase | RegexOptions.ExplicitCapture);

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
            return  new ProjectSum((Phase)int.Parse(m.Groups["Val"].Value));
        }
    }
}