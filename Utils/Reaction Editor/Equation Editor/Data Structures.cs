using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Windows.Forms;
using System.Drawing;
using System.Text.RegularExpressions;
using System.Text;
using Mehroz;

namespace Reaction_Editor
{
    public class RxnEdException : Exception
    {
        public RxnEdException(string message) : base(message) { }
    }

    public enum Phases { Solid, Liquid, Gas, All };
    public enum RxnDirections { Forward, Equilibrium, Backward };
    public enum RxnStatuses { OK, Imbalanced, Invalid };
    public enum ExtentTypes { Fraction, Ratio, Equilibrium, FinalConc, FinalFrac, Rate }
    public enum FracTypes { ByMass, ByMole };
    public enum TPConditions { Feed, Product, Standard, Custom };

    #region Reaction Extent Structures
    public abstract class RxnExtent
    {
        public static Regex s_ExtentRegex = new Regex(
            @"Extent\s*:\s*(?<Data>((?<Fraction>Fraction)|(?<Equilibrium>Equilibrium)|(?<FinalFrac>m(s|l)FinalFraction)|(?<FinalConc>FinalConc)|(?<Ratio>Ratio))(?<Aim>Target|Strict)?.*)",
            RegexOptions.Compiled | RegexOptions.IgnoreCase | RegexOptions.ExplicitCapture);
        protected double m_dValue = double.NaN;
        protected Compound m_Specie;
        protected SimpleReaction m_Owner;
        public RxnExtent(RxnExtent original)
        {
            if (original != null)
            {
                m_Owner = original.Owner;
                this.Value = original.Value;
                this.Specie = original.Specie;
            }
        }
        public RxnExtent() { }

        public virtual double Value
        {
            get { return m_dValue; }
            set 
            {
                if (m_dValue == value) return;
                m_dValue = value > 0 ? value : 0;
                if (m_Owner != null)
                    m_Owner.FireChanged();
            }
        }
        public virtual Compound Specie
        {
            get { return m_Specie; }
            set 
            {
                if (m_Specie == value)
                    return;
                m_Specie = value; 
                if (m_Owner != null)
                    m_Owner.FireChanged();
            }
        }
        public abstract string StartString
        {
            get;
        }

        public SimpleReaction Owner
        {
            get { return m_Owner; }
            set { m_Owner = value; }
        }

        public override string ToString()
        {
            return "Undefined";
        }

        public virtual bool IsValid()
        {
            return this.GetType() != typeof(RxnExtent) 
                && Specie != null 
                && !double.IsNaN(Value);
        }

        public static RxnExtent Parse(string s, out ExtentTypes retType)
        {
            Match m = s_ExtentRegex.Match(s);
            if (!m.Success)
                throw new RxnEdException("Unsupported extent token");
            if (m.Groups["Fraction"].Success)
            {
                if (s.Contains(" Rate ") && Program.Dynamic)
                    retType = ExtentTypes.Rate;
                else
                    retType = ExtentTypes.Fraction;
                return FractionExtent.Parse(m.Groups["Data"].Value);
            }
            if (m.Groups["Equilibrium"].Success)
            {
                retType = ExtentTypes.Equilibrium;
                return EquilibriumExtent.Parse(m.Groups["Data"].Value);
            }
            if (m.Groups["FinalFrac"].Success)
            {
                retType = ExtentTypes.FinalFrac;
                return Final_FracExtent.Parse(m.Groups["Data"].Value);
            }
            if (m.Groups["FinalConc"].Success)
            {
                retType = ExtentTypes.FinalConc;
                return Final_ConcExtent.Parse(m.Groups["Data"].Value);
            }
            if (m.Groups["Ratio"].Success)
            {
                retType = ExtentTypes.Ratio;
                return RatioExtent.Parse(m.Groups["Data"].Value);
            }
            throw new Exception(); //Just to make the compiler happy
        }

        public RxnExtent Clone(SimpleReaction newOwner)
        {
            RxnExtent ret = (RxnExtent) this.MemberwiseClone();
            ret.m_Owner = newOwner;
            return ret;
        }
    }

    public class FractionExtent : RxnExtent
    {
        protected static Regex s_FractionRegex = new Regex(@"(Extent\s*:\s*)?Fraction\s*(?<Aim>Target|Strict)?\s*(?<Specie>[^\s=]+)?\s*(?<Rate>Rate)?\s*=\s*(?<Value>\d+(\.\d+)?|\.\d+)(?<Percent>%)?\s*(?<Stabilised>Stabilised)?",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled);
        public FractionExtent(RxnExtent original) 
            : base(original) { }
        public FractionExtent() : base() { }
        public override string ToString()
        {
            return "Fraction " + Specie + " = " + Value;
        }

        public override string StartString
        {
            get { return "Fraction"; }
        }

        public override double Value
        {
            get { return base.Value; }
            set { base.Value = value > 1 ? 1 : value; }
        }

        public static RxnExtent Parse(string s)
        {
            Match m = s_FractionRegex.Match(s);
            if (!m.Success)
                throw new Exception("Unable to Parse Fraction Extent");
            FractionExtent ret;
            if (m.Groups["Rate"].Success && Program.Dynamic)
                ret = new RateExtent(null);
            else
                ret = new FractionExtent();
            if (m.Groups["Specie"].Success)
                ret.Specie = Compound.FromString(m.Groups["Specie"].Value);
            
            double tmp = double.Parse(m.Groups["Value"].Value);
            if (m.Groups["Percent"].Success)
                tmp *= 0.01;
            ret.Value = tmp;

            if (m.Groups["Rate"].Success && Program.Dynamic && m.Groups["Stabilised"].Success)
                ((RateExtent)ret).Stabilised = true;
            return ret;
        }
    }

    public class RatioExtent : RxnExtent
    {
        protected static Regex s_RatioRegex = new Regex(@"(Extent\s*:\s*)?Ratio\s*(?<Aim>Target|Strict)?\s*
            (?<Specie1>[^\s:]+)\s*:\s*(?<Specie2>[^\s=]+)\s*=\s*(?<Value1>\d+(\.\d+)?|\.\d+)\s*:\s*(?<Value2>\d+(\.\d+)?|\.\d+)
            (\s*@\s*(?<Time>\d+(\.\d+)?|\.\d+)\s*=\s*(?<Value3>\d+(\.\d+)?|\.\d+)\s*:\s*(?<Value4>\d+(\.\d+)?|\.\d+))?",
            RegexOptions.Compiled | RegexOptions.IgnoreCase | RegexOptions.IgnorePatternWhitespace | RegexOptions.ExplicitCapture);
        protected Compound m_Specie2;
        protected double m_Ratio2 = double.NaN;
        protected double m_Time = double.NaN;
        public RatioExtent(RxnExtent original)
            : base(original) { }
        public RatioExtent() : base() { }

        public override string StartString
        {
            get { return "Ratio"; }
        }

        public double Ratio2
        {
            get { return m_Ratio2; }
            set
            {
                m_Ratio2 = value;
                if (m_Owner != null)
                    m_Owner.FireChanged();
            }
        }
        public double Time
        {
            get { return m_Time; }
            set
            {
                m_Time = value;
                if (m_Owner != null)
                    m_Owner.FireChanged();
            }
        }
        public Compound Specie2
        {
            get { return m_Specie2; }
            set
            {
                m_Specie2 = value;
                if (m_Owner != null)
                    m_Owner.FireChanged();
            }
        }
        public override string ToString()
        {
            string ret = "Ratio " + m_Specie + ":" + m_Specie2 + " = 1 : " + m_dValue;
            if (!double.IsNaN(m_Time) && !double.IsNaN(m_Ratio2))
                ret += " @ " + m_Time + " = 1 : " + m_Ratio2;
            return ret;
        }

        public override bool IsValid()
        {
            return base.IsValid() && Specie2 != null && Specie2 != Specie;
        }

        public static RatioExtent Parse(string s)
        {
            Match m = s_RatioRegex.Match(s);
            if (!m.Success)
                throw new Exception("Unable to parse string");
            RatioExtent ret = new RatioExtent();
            try { ret.m_Specie = Compound.FromString(m.Groups["Specie1"].Value); }
            catch (Exception ex) { Program.Log.Message(ex.Message, MessageType.Error); }
            try { ret.m_Specie2 = Compound.FromString(m.Groups["Specie2"].Value); }
            catch (Exception ex) { Program.Log.Message(ex.Message, MessageType.Error); }
            try { ret.m_dValue = double.Parse(m.Groups["Value2"].Value) / double.Parse(m.Groups["Value1"].Value); }
            catch (Exception ex) { Program.Log.Message("Unable to parse ratio", MessageType.Error); }
            if ( m.Groups["Time"].Success )
            {
                try { ret.m_Time = double.Parse(m.Groups["Time"].Value); }
                catch (Exception ex) { Program.Log.Message("Unable to parse time", MessageType.Error); }
                try { ret.Ratio2 = double.Parse(m.Groups["Value3"].Value) / double.Parse(m.Groups["Value4"].Value); }
                catch (Exception ex) { Program.Log.Message("Unable to parse ratio", MessageType.Error); }
            }
            return ret;
#if false   //Old method of going about it.
            if (!s.ToLowerInvariant().StartsWith("ratio"))
                throw new RxnEdException("Invalid string passed to RatioExtent.Parse");
            RatioExtent ret = new RatioExtent();
            s = s.Remove(0, ("Ratio").Length).Trim();
            string[] subs = s.Split(new char[] { '@' }, StringSplitOptions.RemoveEmptyEntries);
            if (subs.Length > 1)
            {
                string[] subSec = subs[1].Split(new char[] { '=', ':' }, StringSplitOptions.RemoveEmptyEntries);
                ret.Time = double.Parse(subSec[0].Trim());
                double r1 = double.Parse(subSec[1].Trim());
                double r2 = double.Parse(subSec[2].Trim());
                ret.Ratio2 = r2 / r1;
            }
            string[] subPri = subs[0].Split(new char[] { '=', ':' }, StringSplitOptions.RemoveEmptyEntries);
            try { ret.Specie = Compound.FromString(subPri[0].Trim()); }
            catch { }
            ret.Specie2 = Compound.FromString(subPri[1].Trim());
            double r3 = double.Parse(subPri[2].Trim());
            double r4 = double.Parse(subPri[3].Trim());
            ret.Value = r4 / r3;
            return ret;
#endif
        }
    }

    public class EquilibriumExtent : RxnExtent
    {
        public EquilibriumExtent(RxnExtent original)
            : base(original) { }

        public EquilibriumExtent() : base () {}

        public override string ToString()
        {
            return "Equilibrium " + Value;
        }

        public override string StartString
        {
            get { return "Equilibrium"; }
        }

        public static EquilibriumExtent Parse(string s)
        {
            if (!s.ToLowerInvariant().StartsWith("equilibrium"))
                throw new RxnEdException("EquilibriumExtent.Parse called with invalid string");
            EquilibriumExtent ret = new EquilibriumExtent();
            s = s.Remove(0, ("equilibrium").Length).Trim();
            ret.Value = double.Parse(s);
            return ret;
        }

        public override bool IsValid()
        {
            return !double.IsNaN(Value);
        }
    }

    public class Final_ConcExtent : RxnExtent
    {
        protected static Regex s_FinalConcRegex = new Regex(
            @"FinalConc\s*(?<Aim>Target|Strict)?\s*(?<Specie>[^\s=]*)?\s*=\s*(?<Value>\d+(\.\d+)?|\.\d+)\s*(At\s*(?<T>\d+(\.\d+)?|\.\d+))?",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled);
        protected double m_dT = double.NaN;
        public Final_ConcExtent(RxnExtent original)
            : base(original) { }
        public Final_ConcExtent() : base() { }
        public double T
        {
            get { return m_dT; }
            set
            {
                if (m_dT == value || (double.IsNaN(m_dT) && double.IsNaN(value))) return;
                m_dT = value;
                if (m_Owner != null)
                    m_Owner.FireChanged();
            }
        }

        public override string ToString()
        {
            return "FinalConc " + Specie + " = " + Value + (double.IsNaN(T) ? "" : " At " + T);
        }

        public override string StartString
        {
            get { return "FinalConc"; }
        }

        public static Final_ConcExtent Parse(string s)
        {
            Match m = s_FinalConcRegex.Match(s);
            if (!m.Success)
                throw new Exception("Invalid string passed to Final_ConcExtent.Parse");
            Final_ConcExtent ret = new Final_ConcExtent();
            if (m.Groups["Specie"].Success)
                ret.Specie = Compound.FromString(m.Groups["Specie"].Value);
            ret.Value = double.Parse(m.Groups["Value"].Value);
            if (m.Groups["T"].Success)
                ret.T = double.Parse(m.Groups["T"].Value);
            return ret;
        }
    }

    public class Final_FracExtent : RxnExtent
    {
        protected static Regex s_FinalFracRegex = new Regex(
            @"(?<Type>m(s|l))finalfraction\s*(?<Phase>Total|Phase)?\s*(?<Aim>Target|Strict)?\s*(?<Specie>[^\s=]+)?\s*=\s*(?<Value>\d+(\.\d+)?|\.\d+)(?<Percent>%)?",
            RegexOptions.Compiled | RegexOptions.IgnoreCase | RegexOptions.ExplicitCapture);
        protected bool m_bByPhase = true;
        protected FracTypes m_eFracType;
        public Final_FracExtent(RxnExtent original)
            : base(original) { }
        public bool ByPhase 
        {
            get{ return m_bByPhase; }
            set 
            {
                if (m_bByPhase == value) return;
                m_bByPhase = value;
                if (m_Owner != null)
                    m_Owner.FireChanged();
            }
        }
        public FracTypes FracType
        {
            get { return m_eFracType; }
            set
            {
                if (m_eFracType == value) return;
                m_eFracType = value;
                if (m_Owner != null)
                    m_Owner.FireChanged();
            }
        }

        public override string ToString()
        {
            string phaseString = m_bByPhase ? "Phase " : "Total ";
            string startString = FracType == FracTypes.ByMass ? "msFinalFraction " : "mlFinalFraction ";
            return startString + phaseString + Specie + " = " + Value;
        }

        public override string StartString
        {
            get { return (FracType == FracTypes.ByMass ? "msFinalFraction " : "mlFinalFraction "); }
        }

        public override double Value
        {
            get { return base.Value; }
            set { base.Value = value > 1 ? 1 : value; }
        }

        public static Final_FracExtent Parse(string s)
        {
            Final_FracExtent ret = new Final_FracExtent(null);
            Match m = s_FinalFracRegex.Match(s);
            if (!m.Success)
                throw new Exception("Unable to parse string");
            if (m.Groups["Type"].Value.ToLowerInvariant() == "ms")
                ret.FracType = FracTypes.ByMass;
            else 
                ret.FracType = FracTypes.ByMole;

            if (m.Groups["Phase"].Success && m.Groups["Phase"].Value.ToLowerInvariant() == "total")
                ret.m_bByPhase = false;

            ret.Value = double.Parse(m.Groups["Value"].Value);
            if (m.Groups["Percent"].Success)
                ret.Value *= 0.01;

            if (m.Groups["Specie"].Success)
                ret.Specie = Compound.FromString(m.Groups["Specie"].Value);

            return ret;
        }
    }

    public class RateExtent : FractionExtent
    {
        protected bool m_bStabilised = false;
        public RateExtent(RxnExtent original)
            : base(original) { }
        public bool Stabilised
        {
            get { return m_bStabilised; }
            set
            {
                if (m_bStabilised == value) return;
                m_bStabilised = value;
                if (m_Owner != null)
                    m_Owner.FireChanged();
            }
        }

        public override string ToString()
        {
            return "Fraction " + Specie + " Rate = " + Value + (Stabilised ? " Stabilised" : "");
        }
    }
    #endregion Reaction Extent Structures

    public class Compound
    {
        #region Internal Variables
        /// <summary>
        /// A list of all compounds in the current database, with their symbol as their key.
        /// </summary>
        public static Dictionary<string, Compound> CompoundList = new Dictionary<string, Compound>();
        private Phases m_ePhase;
        private double m_dHeatOfFormation;
        private double m_dMolWeight;
        private string m_sName;
        private string m_sSymbol;
        private Dictionary<Element, Fraction> m_Elements = new Dictionary<Element, Fraction>();
        private int m_nIndex;
        private bool m_bHeatOK = true;
        #endregion Internal Variables

        public static Regex s_CompoundSeperator = new Regex(@"^((?<Comp>[^,\r\n]*)(?<Delim>,(\r*\n)?|\r*\n|$))*?$",
            RegexOptions.Compiled | RegexOptions.ExplicitCapture);

        public static bool SilentAddFail = true;
        public static bool AddCompound(Compound comp)
        {
            try
            {
                CompoundList.Add(comp.Symbol, comp);
                return true;
            }
            catch (Exception ex)
            {
                if (SilentAddFail)
                    return false;
                else
                    throw ex;
            }
        }

        #region Properties
        /// <summary>
        /// Chemical symbol for the compound - e.g. CH3CHOH
        /// </summary>
        public string Symbol
        {
            get { return m_sSymbol; }
            set { m_sSymbol = value; }
        } 

        /// <summary>
        /// Chemical name of the compound - e.g. Ethanol
        /// </summary>
        public string Name
        {
            get { return m_sName; }
            set { m_sName = value; }
        } 

        public double MolWeight
        {
            get { return m_dMolWeight; }
            set { m_dMolWeight = value; }
        } 

        public double HeatOfFormation
        {
            get { return m_dHeatOfFormation; }
            set { m_dHeatOfFormation = value; }
        } 

        public Phases Phase
        {
            get { return m_ePhase; }
            set { m_ePhase = value; }
        }

        /// <summary>
        /// A Dictionary with the elements present in the compound as the key, and their count as the value.
        /// e.g. CH3CHOH would contain {{C, 2.0}, {H, 5.0}, {O, 1.0}}
        /// </summary>
        public Dictionary<Element, Fraction> Elements
        {
            get { return m_Elements; }
            set { m_Elements = value; }
        }

        public int Index
        {
            get { return m_nIndex; }
            set { m_nIndex = value; }
        }

        public bool HeatOK
        {
            get { return m_bHeatOK; }
            set { m_bHeatOK = value; }
        }

        public string Annotation;
        #endregion

        public static List<Compound> FromCommaList(string list) { bool t; return FromCommaList(list, out t); }
        public static List<Compound> FromCommaList(string list, out bool ok)
        {
            ok = true;
            Match m2 = s_CompoundSeperator.Match(list);
            List<Compound> ret = new List<Compound>();
            foreach (Capture c in m2.Groups["Comp"].Captures)
                if (Compound.Contains(c.Value.Trim()))
                    ret.Add(Compound.FromString(c.Value.Trim()));
                else if (!string.IsNullOrEmpty(c.Value.Trim()))
                    ok = false;
            return ret;
        }

        public static Compound FromString(string compString)
        {
            if (CompoundList.ContainsKey(compString))
                return CompoundList[compString];
            else
                throw new RxnEdException("Compound " + compString + " not found in database");
        }

        public static bool Contains(string compString)
        {
            return CompoundList.ContainsKey(compString);
        }

        public override string ToString()
        {
            return Symbol;
        }
    }

    public class Element
    {
        public Element() { }
        public Element(string sym, int AtmNo, double AtmWt)
        {
            this.Name = this.m_sSymbol = sym;
            this.m_dAtomicMass = AtmWt;
            this.m_nAtomicNo = AtmNo;
        }
        /// <summary>
        /// The chemical symbol of the element - e.g. O
        /// </summary>
        public string Symbol
        {
            get { return m_sSymbol; }
            set { m_sSymbol = value; }
        } private string m_sSymbol;

        /// <summary>
        /// The name of the element - e.g. Oxygen
        /// </summary>
        public string Name
        {
            get { return m_sName; }
            set { m_sName = value; }
        } private string m_sName;

        public int AtmoicNo
        {
            get { return m_nAtomicNo; }
            set { m_nAtomicNo = value; }
        } private int m_nAtomicNo;

        public double AtomicMass
        {
            get { return m_dAtomicMass; }
            set { m_dAtomicMass = value; }
        } private double m_dAtomicMass;

        public static Dictionary<string, Element> ElementList = new Dictionary<string, Element>();

        public static bool SilentAddFail = true;
        public static bool AddElement(Element e)
        {
            try
            {
                ElementList.Add(e.Symbol, e);
                return true;
            }
            catch (Exception ex)
            {
                if (SilentAddFail)
                    return false;
                else
                    throw ex;
            }
        }
    }

    // It may be an idea to make SimpleReaction an extension of Reaction at some point, but for now is not worth
    // the effort. Use this for Sources, Sinks, and Heat Exchange.
    public abstract class Reaction
    {
        public static int DisabledReactionNumber = -3;

        #region Protected Variables
        protected bool m_bHasChanged = false;
        protected int m_nOriginalReactionNumber = -1;
        protected int m_nReactionNumber = -1;
        protected Match m_OriginalMatch;
        protected ListViewItem m_LVI;
        protected bool m_bEnabled = true;
        protected string m_Comment;
        #endregion Protected Variables

        #region Properties
        public string Comment
        {
            get { return m_Comment; }
            set
            {
                if (m_Comment == value)
                    return;
                m_Comment = value;
                m_bHasChanged = true;
                if (NowChanged != null)
                    NowChanged(this, new EventArgs());
            }
        }

        public virtual bool CanRevert
        {
            get { return m_OriginalMatch != null && m_bHasChanged; }
        }

        public Match OriginalMatch
        {
            get { return m_OriginalMatch; }
            set { m_OriginalMatch = value; }
        }

        public int ReactionNumber
        {
            get { return m_nReactionNumber; }
            set
            {
                m_nReactionNumber = value;
                UpdateStatus(this, new EventArgs());
            }
        }

        public int OriginalReactionNumber
        {
            get { return m_nOriginalReactionNumber; }
            set { m_nOriginalReactionNumber = value; }
        }

        public bool HasChanged
        {
            get { return m_bHasChanged; }
            set 
            {
                m_bHasChanged = value;
                UpdateStatus(this, new EventArgs());
            }
        }

        public ListViewItem LVI
        {
            get { return m_LVI; }
            set { m_LVI = value; }
        }

        public virtual bool Enabled
        {
            get { return m_bEnabled; }
            set
            {
                if (m_bEnabled == value) return;
                m_bEnabled = value;
                HasChanged = true;
                FireChanged();
            }
        }

        public virtual RxnStatuses Status
        {
            get { return RxnStatuses.OK; }
        }

        public virtual string StatusString
        {
            get { return "Status: " + Status + "."; }
        }
        #endregion Properties

        #region Events
        public event EventHandler NowChanged;
        #endregion Events

        #region Constructors
        public Reaction(string[] LVIParams)
        {
            m_LVI = new ListViewItem(LVIParams);
            m_LVI.Tag = this;
            NowChanged += new EventHandler(UpdateStatus);
        }
        #endregion Constructors

        #region Protected Properties
        protected string OriginalReactionNumberString
        {
            get 
            {
                if (!m_bEnabled)
                {
                    if (m_nOriginalReactionNumber == DisabledReactionNumber)
                        return "";
                    if (m_nOriginalReactionNumber > 0)
                        return "(" + m_nOriginalReactionNumber + ")";
                }
                else
                {
                    if (m_nOriginalReactionNumber == DisabledReactionNumber)
                        return "";
                    if (m_nOriginalReactionNumber > 0 && m_nOriginalReactionNumber != m_nReactionNumber)
                        return "(" + m_nOriginalReactionNumber + ")";
                }
                return "";
            }
        }

        public abstract Reaction RevertReaction
        {
            get;
        }
        #endregion Protected Properties

        #region Functions
        public virtual void UpdateStatus(object sender, EventArgs e)
        {
            if (m_bEnabled)
                LVI.Text = 
                    (Enabled ? ReactionNumber.ToString() : "-") +
                    OriginalReactionNumberString +
                    (HasChanged ? "*" : "");
            LVI.ForeColor = m_bEnabled ? SystemColors.WindowText : Color.Gray;
        }

        public virtual void FireChanged()
        {
            HasChanged = true;
            if (NowChanged != null)
                NowChanged(this, new EventArgs());
        }

        public abstract void Revert();

        public abstract void Backup();

        public abstract string ToSaveString();

        public abstract string RevertString();
        #endregion Functions
    }

    public class CompoundListReaction : Reaction
    {
        #region Regex's
        public static Regex s_SourceSinkRegex = new Regex(@"(;RC(\d+|-):(?<Comment>[^\r\n]*))?(^|\r*\n)[^\S\r\n]*(?<Disabled>;)?(?<Type>Source|Sink)[^\S\r\n]*:(?<Value>[^;\r\n]*)(?(Disabled)\s*;Enabled=false)",
            RegexOptions.IgnoreCase | RegexOptions.ExplicitCapture | RegexOptions.Compiled);
        #endregion Regex's

        #region Variables
        protected string m_RawString = "";
        protected List<Compound> m_Cache = new List<Compound>();
        protected ListType m_ListType;
        protected bool m_bListOk = true;
        #endregion Variables

        #region Properties
        public override RxnStatuses Status
        {
            get { return m_bListOk ? RxnStatuses.OK : RxnStatuses.Invalid; }
        }

        public List<Compound> CompoundList
        {
            get { return m_Cache; }
        }

        public string CompoundString
        {
            get
            {
                if (m_bListOk)
                {
                    StringBuilder sb = new StringBuilder(m_Cache.Count);
                    for (int i = 0; i < m_Cache.Count; i++)
                        if (i < m_Cache.Count - 1)
                            sb.Append(m_Cache[i] + ", ");
                        else
                            sb.Append(m_Cache[i]);
                    return sb.ToString();
                }
                else
                    return m_RawString;
            }
        }

        public ListType SourceOrSink
        {
            get { return m_ListType; }
        }

        public override Reaction RevertReaction
        {
            get { return new CompoundListReaction(m_OriginalMatch); }
        }
        #endregion Properties

        #region Public Functions
        public override void Revert()
        {
            SetString(m_OriginalMatch.Groups["Value"].Value);
            this.Enabled = !m_OriginalMatch.Groups["Disabled"].Success;
            m_bHasChanged = false;
            UpdateStatus(this, new EventArgs());
        }

        public override void Backup()
        {
            m_OriginalMatch = s_SourceSinkRegex.Match(this.ToSaveString());
        }

        public CompoundListReaction(Match m)
            : base(new string[] { "", "" })
        {
            OriginalMatch = m;
            Enabled = !m.Groups["Disabled"].Success;
            if (m.Groups["Comment"].Success)
                Comment = m.Groups["Comment"].Value;
            m_ListType = m.Groups["Type"].Value.ToLowerInvariant() == "source" ? ListType.Source : ListType.Sink;
            SetString(m.Groups["Value"].Value);
            m_bHasChanged = false;
            m_nOriginalReactionNumber = DisabledReactionNumber;
            UpdateStatus(this, new EventArgs());
        }

        public CompoundListReaction(ListType type)
            : base (new string[] { "", ""})
        {
            m_ListType = type;
            UpdateStatus(this, new EventArgs());
        }

        public override string ToString()
        {
            return m_ListType.ToString() + ": " + CompoundString;
        }

        public override string ToSaveString()
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine(";RC" + ReactionNumber + ": " + Comment);
            sb.AppendLine((Enabled ? "" : ";") + this.ToString());
            if (!Enabled)
                sb.AppendLine(";Enabled=False");
            return sb.ToString();
        }

        public override string RevertString()
        {
            return m_ListType + ": " + m_OriginalMatch.Groups["Value"].Value;
        }

        public void SetString(string s)
        {
            if (s == CompoundString)
                return;
            m_RawString = s;
            m_Cache = Compound.FromCommaList(s, out m_bListOk);
            FireChanged();
        }

        public override void UpdateStatus(object sender, EventArgs e)
        {
            base.UpdateStatus(sender, e);
            LVI.ForeColor = Enabled ? (Status == RxnStatuses.OK ? SystemColors.WindowText : Color.Red) : Color.Gray;
            LVI.SubItems[1].Text = this.ToString();
        }

        public void AddCompound(Compound c)
        {
            if (m_Cache.Contains(c))
                return;
            m_Cache.Add(c);
            if (!m_bListOk)
            {
                if (!string.IsNullOrEmpty(m_RawString.Trim()))
                    m_RawString += m_RawString.TrimEnd('\r', '\n', ',', ' ') + ", " + c.Symbol;
                else
                    m_RawString = c.Symbol;
            }
            FireChanged();
        }

        public void RemoveCompound(Compound c)
        {
            if (!m_Cache.Contains(c))
                return;
            while (m_Cache.Contains(c))
                m_Cache.Remove(c);
            if (!m_bListOk)
            {
                List<string> temp = new List<string>(m_RawString.Split(new char[] { ',', '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries));
                for (int i = temp.Count - 1; i >= 0; i--)
                    if (temp[i].Trim() == c.Symbol)
                        temp.RemoveAt(i);
                StringBuilder sb = new StringBuilder(temp.Count);
                for (int i = 0; i < temp.Count; i++)
                    if (i < temp.Count - 1)
                        sb.Append(temp[i] + ", ");
                    else
                        sb.Append(temp[i]);
                m_RawString = sb.ToString();
            }
            FireChanged();
        }
        #endregion Public Functions

        #region Subclasses
        public enum ListType { Source, Sink }
        #endregion Subclasses
    }

    public class HXReaction : Reaction
    {
        public static Regex s_HXRegex = new Regex(
            @"^\s*(;(RC(\d+|-): ?)?(?<Comment>[^\r\n]*)\r*\n)?\s*(?<Disabled>;)?HeatExchange:\s*(
                        (?<Type>FinalT|Power|Electrolysis)\s*=\s*(?<Value>\d+(\.\d+)?|\.\d+) |
                        (?<Type>TargetT|Ambient)\s*=\s*(?<Value>\d+(\.\d+)?|\.\d+)\s*,\s*(ApproachT|ApproachAmbient)\s*=\s*(?<Value2>d+(\.\d+)?|\.\d+)
                        )(?(Disabled)\s*;Enabled=false)",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Multiline | RegexOptions.Compiled | RegexOptions.IgnorePatternWhitespace);


        #region Variables
        protected HXTypes m_eType;
        protected double m_dValue, m_dValue2;
        #endregion Variables

        #region Properties
        public HXTypes Type
        {
            get { return m_eType; }
            set 
            {
                if (m_eType == value) return;
                m_eType = value;
                FireChanged();
            }
        }

        public double Value
        {
            get { return m_dValue; }
            set 
            {
                if (m_dValue == value) return;
                m_dValue = value;
                FireChanged();
            }
        }

        public double Value2
        {
            get { return m_dValue2; }
            set 
            {
                if (m_dValue2 == value) return;
                m_dValue2 = value;
                FireChanged();
            }
        }

        public override Reaction RevertReaction
        {
            get { return new HXReaction(m_OriginalMatch); }
        }
        #endregion Properties

        #region Public Functions
        public HXReaction(Match m)
            : base(new string[] {"", ""})
        {
            m_nOriginalReactionNumber = DisabledReactionNumber;
            m_OriginalMatch = m;
            Revert();
        }

        public HXReaction()
            : base(new string[] {"",""})
        {
            UpdateStatus(this, new EventArgs());
        }

        public override void Revert()
        {
            m_bEnabled = !m_OriginalMatch.Groups["Disabled"].Success;
            if (m_OriginalMatch.Groups["Comment"].Success)
                m_Comment = m_OriginalMatch.Groups["Comment"].Value;

            m_eType = (HXTypes)Enum.Parse(typeof(HXTypes), m_OriginalMatch.Groups["Type"].Value, true);

            double.TryParse(m_OriginalMatch.Groups["Value"].Value, out m_dValue);
            double.TryParse(m_OriginalMatch.Groups["Value2"].Value, out m_dValue2);

            m_bHasChanged = false;
            UpdateStatus(this, new EventArgs());
        }

        public override void Backup()
        {
            m_OriginalMatch = s_HXRegex.Match(this.ToSaveString());
        }

        public override void UpdateStatus(object sender, EventArgs e)
        {
            base.UpdateStatus(sender, e);
            LVI.SubItems[1].Text = this.ToString();
        }

        public override string ToString()
        {
            if (m_eType == HXReaction.HXTypes.Electrolysis ||
               m_eType == HXReaction.HXTypes.FinalT ||
               m_eType == HXReaction.HXTypes.Power)
            {
                return "HeatExchange: " + m_eType + " = " + Value.ToString(); ;
            }
            else if (m_eType == HXReaction.HXTypes.ApproachAmbient)
            {
                return "HeatExchange: Ambient = " + Value.ToString() + ", ApproachAmbient = " + Value2.ToString();
            }
            else if (m_eType == HXReaction.HXTypes.ApproachT)
            {
                return "Heat Exchange: TargetT = " + Value.ToString() + ", ApproachT = " + Value2.ToString();
            }
            return "";
        }

        public override string ToSaveString()
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine(";RC" + ReactionNumber + ": " + Comment);
            sb.AppendLine((Enabled ? "" : ";" ) + ToString());
            if (!Enabled)
                sb.AppendLine(";Enabled=False");
            return sb.ToString();
        }

        public override string RevertString()
        {
            return m_OriginalMatch.Value;
        }
        #endregion Public Functions

        #region sublcasses
        public enum HXTypes { FinalT, ApproachT, ApproachAmbient, Power, Electrolysis };
        #endregion subclasses
    }

    public class SimpleReaction
    {
        public static DataFormats.Format sFormat = DataFormats.GetFormat(typeof(SimpleReaction).FullName);
        public static double sMinValue = 5E-3;

        #region regex's
        /*public static Regex s_ReactionRegex = new Regex(
            @"(^|\r*\n)\s*((;(RC(\d+|-):\s?)?(?<Comment>[^\r\n]*))\r*\n)?
             (?<Reactants>[^;\r\n<>=\:]*)(?<Direction>->|=|<->|->)\s*(?<Products>[^<>=;:\r\n]*?(?=Extent|Sequence|HeatOfReaction|;|\r*\n|$))
            (?>(?>\s*(;.*\r*\n)?)*
            ((?<Extent>Extent\s*:[^\r\n;]*?)|(?<Sequence>Sequence\s*:[^\r\n;]*?)|(?<HOR>HeatOfReaction\s*:[^\r\n;]*?))(?=Extent|Sequence|HeatOfReaction|;|\r\n|$)){0,3}",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled | RegexOptions.IgnorePatternWhitespace);

        public static Regex s_DisabledReactionRegex = new Regex(
            @"(^|\r*\n)\s*((;(RC(\d+|-):\s?)?(?<Comment>[^\r\n]*))\r*\n[^\S\r\n]*)?
            ;(?<Reactants>[^;\r\n<>=\:]*)(?<Direction>->|=|<->|->)\s*(?<Products>[^<>=;:\r\n]*?(?=Extent|Sequence|HeatOfReaction|;|\r*\n|$))
            (?>[^\S\r\n]*(\r*\n\s*;[^\S\r\n]*)?
            ((?<Extent>Extent\s*:[^\r\n;]*?)|(?<Sequence>Sequence\s*:[^\r\n;]*?)|(?<HOR>HeatOfReaction\s*:[^\r\n;]*?))(?=Extent|Sequence|HeatOfReaction|;|\r\n|$)){0,3}
            (\s*;Enabled=false)",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled | RegexOptions.IgnorePatternWhitespace);*/

        public static Regex s_GeneralReactionRegex = new Regex(
            @"(^|\r*\n)\s*((;(RC(\d+|-):\s?)?(?<Comment>[^\r\n]*))\r*\n[^\S\r\n]*)?
            (?<Disabled>;)?(?<Reactants>[^;\r\n<>=\:]*)(?<Direction>->|=|<->|->)\s*(?<Products>[^<>=;:\r\n]*?(?=Extent|Sequence|HeatOfReaction|;|\r*\n|$))
            (?>(?(Disabled)[^\S\r\n]*(\r*\n\s*;[^\S\r\n]*)?|(?>\s*(;.*\r*\n)?)*)
            ((?<Extent>Extent\s*:[^\r\n;]*?)|(?<Sequence>Sequence\s*:[^\r\n;]*?)|(?<HOR>HeatOfReaction\s*:[^\r\n;]*?))(?=Extent|Sequence|HeatOfReaction|;|\r*\n|$)){0,3}
            (?(Disabled)\s*;Enabled=false)",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled | RegexOptions.IgnorePatternWhitespace);

        public static Regex s_SequenceRegex = new Regex(@"^Sequence\s*:\s*(?<Value>\d+)$",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled);
        public static Regex s_HORRegex = new Regex(
            @"^\s*HeatOfReaction\s*:\s*(?<Type>MsFixed|MlFixed)\s*=\s*(?<Value>(\+|-)?(\d+(\.\d+)?|\.\d+))\s*(/\s*(?<Specie>\S*))?\s*(At\s*(?<Cond>(?<Condition>Feed|Prod|Std)|(?<T>(\+|-)?(\d*(\.\d+)?|\.\d+))C?(\s*,\s*(?<P>\d*(\.\d+)?|\.\d+))?))?\s*$",
            RegexOptions.IgnoreCase | RegexOptions.ExplicitCapture | RegexOptions.Compiled);
        public static Regex s_CompoundRegex = new Regex(
            @"^\s*(?<Unparseable>.*?)(?<Amount>\d+/\d+|\d*(\.\d+)?)(?<Space>\s*)(?<Compound>[^\s]*)(?>(?<Unparseable>.*?)\s+\+\s+(?<Amount>\d+/\d+|\d*(\.\d+)?)(?<Space>\s*)(?<Compound>[^\s]*))*\s*(?<Unparseable>.*?)$",
            RegexOptions.ExplicitCapture | RegexOptions.Compiled);
        #endregion Regex's

        #region Internal Variables
        protected SimpleReaction m_Original;
        protected ListViewItem m_LVI;
        protected bool m_bChanged = false;
        protected bool m_bInitialised = false;

        protected int m_nReactionNumber;
        protected int m_nOriginalReactionNumber = -1;

        protected int m_nLastAddedNumStart;

        protected bool m_bEnabled = true;
        protected RxnStatuses m_eStatus;
        protected bool m_bProductsOK = true, m_bReactantsOK = true;
        protected bool m_bUseOriginalString;

        protected RxnDirections m_eDirection = RxnDirections.Equilibrium;

        protected ExtentTypes m_eExtentType;
        protected RxnExtent m_Extent = new FractionExtent(null);

        protected double m_dHeatOfReaction = double.NaN;
        protected Compound m_HeatOfReactionSpecie = null;
        protected FracTypes m_eHeatOfReactionType = FracTypes.ByMole;
        protected TPConditions m_eHeatOfReactionConditions = TPConditions.Feed;
        protected double m_dHeatOfReactionT = 25, m_dHeatOfReactionP = 101.3;
        
        protected int m_nSequence = 1;

        protected Dictionary<Compound, Fraction> m_Reactants = new Dictionary<Compound, Fraction>();
        protected Dictionary<Compound, Fraction> m_Products = new Dictionary<Compound, Fraction>();
        protected List<Compound> m_OrderedReactants = new List<Compound>();
        protected List<Compound> m_OrderedProducts = new List<Compound>();

        protected Dictionary<Element, Fraction> m_Unbalanced = new Dictionary<Element, Fraction>();

        protected string m_sOriginalString;
        protected string m_sReactantString;
        protected string m_sProductString;
        protected string m_sComment;

        #endregion Internal Variables

        #region Constructors
        public SimpleReaction(ListViewItem _LVI)
        {
            m_Extent.Owner = this;
            m_Extent.Value = 1;
            LVI = _LVI;
            Changed += new EventHandler(UpdateStatus);
            UpdateStatus(this, new EventArgs());
        }
        #endregion Constructors

        #region Events
        public event EventHandler Changed;
        public event EventHandler ProductsChanged;
        public event EventHandler ReactantsChanged;
        public event EventHandler SequenceChanged;
        #endregion Events

        #region Properties
        protected string OriginalReactionNumberString
        {
            get
            {
                if (!m_bEnabled)
                {
                    if (m_nOriginalReactionNumber == Reaction.DisabledReactionNumber)
                        return "";
                    if (m_nOriginalReactionNumber > 0)
                        return "(" + m_nOriginalReactionNumber + ")";
                }
                else
                {
                    if (m_nOriginalReactionNumber == Reaction.DisabledReactionNumber)
                        return "(-)";
                    if (m_nOriginalReactionNumber > 0 && m_nOriginalReactionNumber != m_nReactionNumber)
                        return "(" + m_nOriginalReactionNumber + ")";
                }
                return "";
            }
        }

        public bool Initialised
        {
            get { return m_bInitialised; }
            set { m_bInitialised = value; }
        }

        public int ReactionNumber
        {
            get { return m_nReactionNumber; }
            set
            {
                if (m_nReactionNumber == value) return;
                m_nReactionNumber = value;
                bool temp = m_bInitialised;
                m_bInitialised = false;
                try
                {
                    FireChanged();
                }
                finally
                {
                    m_bInitialised = temp;
                }
            }
        }

        /// <summary>
        /// The reaction number of the reaction when loaded from the file.
        /// </summary>
        public int OriginalReactionNumber
        {
            get { return m_nOriginalReactionNumber; }
            set
            {
                if (m_nOriginalReactionNumber == value) return;
                m_nOriginalReactionNumber = value;
                FireChanged();
            }
        }

        public bool HasChanged
        {
            get { return m_bChanged; }
            set
            {
                m_bChanged = value;
                UpdateStatus(this, new EventArgs());
            }
        }

        public bool CanRevert
        {
            get { return m_Original != null; }
        }

        public SimpleReaction RevertReaction
        {
            get { return m_Original; }
        }

        public ListViewItem LVI
        {
            get { return m_LVI; }
            set 
            {
                if (m_LVI == value) return;
                m_LVI = value;
                if (m_LVI != null)
                    m_LVI.Tag = this;
            }
        }

        public int LastAddedNumStart
        {
            get { return m_nLastAddedNumStart; }
        }

        public RxnStatuses Status
        {
            get { return m_eStatus; }
        }

        public bool Enabled
        {
            get { return m_bEnabled; }
            set
            {
                if (value == m_bEnabled) return;
                m_bEnabled = value;
                FireChanged();
            }
        }

        public bool ReactantsOk
        {
            get { return m_bReactantsOK; }
        }

        public bool ProductsOk
        {
            get { return m_bProductsOK; }
        }

        public bool UseOriginalString
        {
            get { return m_bUseOriginalString; }
        }

        public RxnDirections Direction
        {
            get { return m_eDirection; }
            set
            {
                if (m_eDirection == value) return;
                m_eDirection = value;
                if (Changed != null)
                    Changed(this, new EventArgs());
            }
        } 

        public string DirectionString
        {
            get
            {
                switch (m_eDirection)
                {
                    case RxnDirections.Backward:
                        return "<-";
                    case RxnDirections.Forward:
                        return "->";
                    case RxnDirections.Equilibrium:
                        return "=";
                }
                throw new Exception("Invalid Direction");
            }
            set
            {
                if (value == "<-") m_eDirection = RxnDirections.Backward;
                else if (value == "->") m_eDirection = RxnDirections.Forward;
                else if (value == "<->" || value == "=") m_eDirection = RxnDirections.Equilibrium;
                else throw new Exception("Unknown direction string passed.");

                FireChanged();
            }
        }

        public string Comment
        {
            get { return m_sComment; }
            set
            {
                if (m_sComment == value) return;
                m_sComment = value;
                FireChanged();
            }
        }

        public Dictionary<Compound, Fraction> Reactants
        {
            get { return m_Reactants; }
            //set { m_Reactants = value; }
        }

        public Dictionary<Compound, Fraction> Products
        {
            get { return m_Products; }
            //set { m_Products = value; }
        }

        public string HeatOfReactionString
        {
            get
            {
                StringBuilder sb = new StringBuilder();
                sb.Append(" HeatOfReaction : ");
                if (m_eHeatOfReactionType == FracTypes.ByMass)
                    sb.Append("MsFixed = ");
                else
                    sb.Append("MlFixed = ");
                sb.Append(m_dHeatOfReaction + " / " + m_HeatOfReactionSpecie);
                switch (m_eHeatOfReactionConditions)
                {
                    case TPConditions.Feed:
                        sb.Append(" At Feed"); break;
                    case TPConditions.Product:
                        sb.Append(" At Prod"); break;
                    case TPConditions.Standard:
                        sb.Append(" At Std"); break;
                    case TPConditions.Custom:
                        sb.Append(" At " + m_dHeatOfReactionT + ", " + m_dHeatOfReactionP); break;
                }
                return sb.ToString();
            }
        }

        public double HeatOfReactionValue
        {
            get
            {
                return m_dHeatOfReaction;
            }
            set
            {
                if (m_dHeatOfReaction == value)
                    return;
                m_dHeatOfReaction = value;
                if (Changed != null)
                    Changed(this, new EventArgs());
            }
        }

        public Compound HeatOfReactionSpecie
        {
            get { return m_HeatOfReactionSpecie; }
            set 
            {
                if (m_HeatOfReactionSpecie == value) return;
                m_HeatOfReactionSpecie = value;
                if (Changed != null)
                    Changed(this, new EventArgs());
            }
        }

        public FracTypes HeatOfReactionType
        {
            get { return m_eHeatOfReactionType; }
            set
            {
                if (m_eHeatOfReactionType == value) return;
                m_eHeatOfReactionType = value;
                FireChanged();
            }
        }

        public TPConditions HeatOfReactionConditions
        {
            get { return m_eHeatOfReactionConditions; }
            set
            {
                if (m_eHeatOfReactionConditions == value) return;
                m_eHeatOfReactionConditions = value;
                FireChanged();
            }
        }

        public double HeatOfReactionP
        {
            get { return m_dHeatOfReactionP; }
            set
            {
                if (m_dHeatOfReactionP == value) return;
                m_dHeatOfReactionP = value;
                FireChanged();
            }
        }

        public double HeatOfReactionT
        {
            get { return m_dHeatOfReactionT; }
            set
            {
                if (m_dHeatOfReactionT == value) return;
                m_dHeatOfReactionT = value;
                FireChanged();
            }
        }

        public bool Balanced
        {
            get 
            {
                bool ret = true;
                m_Unbalanced.Clear();
                Dictionary<Element, Fraction> reactantAmounts = new Dictionary<Element, Fraction>();
                Dictionary<Element, Fraction> productAmounts = new Dictionary<Element, Fraction>();
                foreach (KeyValuePair<Compound, Fraction> kvpComps in m_Reactants)
                    foreach (KeyValuePair<Element, Fraction> kvpElems in kvpComps.Key.Elements)
                    {
                        if (!reactantAmounts.ContainsKey(kvpElems.Key))
                            reactantAmounts.Add(kvpElems.Key, 0);
                        reactantAmounts[kvpElems.Key] += kvpComps.Value * kvpElems.Value;
                    }

                foreach (KeyValuePair<Compound, Fraction> kvpComps in m_Products)
                    foreach (KeyValuePair<Element, Fraction> kvpElems in kvpComps.Key.Elements)
                    {
                        if (!productAmounts.ContainsKey(kvpElems.Key))
                            productAmounts.Add(kvpElems.Key, 0);
                        productAmounts[kvpElems.Key] += kvpComps.Value * kvpElems.Value;
                    }

                foreach (KeyValuePair<Element, Fraction> kvpReactants in reactantAmounts)
                    if (!productAmounts.ContainsKey(kvpReactants.Key))
                    {
                        ret = false;
                        m_Unbalanced[kvpReactants.Key] = -reactantAmounts[kvpReactants.Key];
                    }
                    else if (kvpReactants.Value - productAmounts[kvpReactants.Key] != 0)
                    {
                        ret = false;
                        m_Unbalanced[kvpReactants.Key] = productAmounts[kvpReactants.Key] - kvpReactants.Value;
                    }

                foreach (Element e in productAmounts.Keys)
                    if (!reactantAmounts.ContainsKey(e))
                    {
                        ret = false;
                        m_Unbalanced[e] = productAmounts[e];
                    }

                return ret;
            }
        }

        public bool Reducible
        {
            get
            {
                if (this.Reactants.Count == 0)
                    return false;

                Matrix m = GetBalanceMatrix();
                if (!m.IsSolveable())
                    return false;
                Matrix.RemovalInfo info = m.GetRemovalInfo();
                return info.DegreesOfFreedom > 0 && info.m_bCanRemove;
            }
        }

        public bool CanBalance
        {
            get { return m_Reactants.Count > 0; }
        }

        public Dictionary<Element, Fraction> UnbalancedDetails
        {
            get { return m_Unbalanced; }
        }

        public bool HeatOfReactionOK
        {
            get
            {
                if (!CustomHeatOfReaction)
                    foreach (Compound c in Compounds)
                        if (!c.HeatOK)
                            return false;
                return true;
            }
        }

        public bool CustomHeatOfReaction
        {
            get 
            {
                return !double.IsNaN(m_dHeatOfReaction) &&
                    m_HeatOfReactionSpecie != null;
                //return m_bCustomHeatOfReaction; 
            }
        }

        public ExtentTypes ExtentType
        {
            get 
            {
                if (m_Extent.GetType() == typeof(RateExtent))
                    return ExtentTypes.Rate;
                if (m_Extent.GetType() == typeof(RatioExtent))
                    return ExtentTypes.Ratio;
                if (m_Extent.GetType() == typeof(FractionExtent))
                    return ExtentTypes.Fraction;
                if (m_Extent.GetType() == typeof(Final_ConcExtent))
                    return ExtentTypes.FinalConc;
                if (m_Extent.GetType() == typeof(Final_FracExtent))
                    return ExtentTypes.FinalFrac;
                if (m_Extent.GetType() == typeof(EquilibriumExtent))
                    return ExtentTypes.Equilibrium;
                return (ExtentTypes)(-1);
            }///return m_eExtentType; }
            set
            {
                if (value == ExtentType) return; //Do not recreate the current extent if there is no request for a change.
                m_eExtentType = value;
                switch ((ExtentTypes)value)
                {
                    case ExtentTypes.Equilibrium:
                        m_Extent = new EquilibriumExtent(m_Extent);
                        break;
                    case ExtentTypes.FinalConc:
                        m_Extent = new Final_ConcExtent(m_Extent);
                        break;
                    case ExtentTypes.FinalFrac:
                        m_Extent = new Final_FracExtent(m_Extent);
                        break;
                    case ExtentTypes.Fraction:
                        m_Extent = new FractionExtent(m_Extent);
                        break;
                    case ExtentTypes.Rate:
                        m_Extent = new RateExtent(m_Extent);
                        break;
                    case ExtentTypes.Ratio:
                        m_Extent = new RatioExtent(m_Extent);
                        break;
                }

                if (Changed != null)
                    Changed(this, new EventArgs());
            }
        }

        public RxnExtent ExtentInfo
        {
            get { return m_Extent; }
        }

        public int Sequence
        {
            get { return m_nSequence; }
            set
            {
                if (m_nSequence == value) return;
                m_nSequence = value;
                if (m_nSequence > FrmReaction.sMaxSequences)
                    m_nSequence = FrmReaction.sMaxSequences;

                if (SequenceChanged != null)
                    SequenceChanged(this, new EventArgs());
                FireChanged();
            }
        }

        public List<Compound> Compounds
        {
            get
            {
                List<Compound> ret = new List<Compound>();
                foreach (Compound c in m_OrderedProducts)
                    if (!ret.Contains(c))
                        ret.Add(c);
                foreach (Compound c in m_OrderedReactants)
                    if (!ret.Contains(c))
                        ret.Add(c);
                return ret;
            }
        }

        public Compound FirstReactant
        {
            get
            {
                if (m_OrderedReactants.Count > 0)
                    return m_OrderedReactants[0];
                else
                    return null;
            }
        }

        public List<Element> Elements
        {
            get
            {
                return ElementsWith(new List<Compound>());
            }
        }

        public List<Compound> OrderedProducts { get { return m_OrderedProducts; } }
        public List<Compound> OrderedReactants { get { return m_OrderedReactants; } }
        #endregion Properties

        #region Public Functions
        public void SetRegex(Match rxnMatch, MessageSource source, string title)
        {
            Group grpComment = rxnMatch.Groups["Comment"];
            if (grpComment.Success)
                this.Comment = grpComment.Captures[0].Value.Trim();

            Group grpReactants = rxnMatch.Groups["Reactants"];
            this.ParseReactants(grpReactants.Captures[0].Value, true);


            Group grpDirection = rxnMatch.Groups["Direction"];
            this.DirectionString = grpDirection.Captures[0].Value;

            Group grpProducts = rxnMatch.Groups["Products"];
            this.ParseProducts(grpProducts.Captures[0].Value, true);

            source.Source = title + ": " + this;

            Group grpExtent = rxnMatch.Groups["Extent"];
            if (grpExtent.Success)
                try
                {
                    this.ParseExtent(grpExtent.Captures[0].Value);
                }
                catch (Exception ex)
                {
                    Program.Log.Message("Unable to parse extent (" + grpExtent.Value + "). Reason: " + ex.Message, MessageType.Warning);
                }
            else
                Program.Log.Message("Extent not found for reaction", MessageType.Warning);

            Group grpSequence = rxnMatch.Groups["Sequence"];
            if (grpSequence.Success)
            {
                Match sequenceMatch = SimpleReaction.s_SequenceRegex.Match(grpSequence.Captures[0].Value.Trim());
                if (sequenceMatch.Success)
                    m_nSequence = int.Parse(sequenceMatch.Groups["Value"].Captures[0].Value);
                else
                    Program.Log.Message("Unable to parse sequence '" + grpSequence.Value + "'", MessageType.Warning);
            }

            Group grpHOR = rxnMatch.Groups["HOR"];
            if (grpHOR.Success)
                try
                {
                    this.ParseHOR(grpHOR.Captures[0].Value.Trim());
                }
                catch (Exception ex)
                {
                    Program.Log.Message("Unable to parse HeatOfReaction '" + grpHOR.Value + "' Reason: " + ex.Message, MessageType.Warning);
                }

            this.Enabled = !rxnMatch.Groups["Disabled"].Success;
        }

        /// <summary>
        /// Creates a backup of this reaction that can be accessed using Revert()
        /// </summary>
        public void Backup()
        {
            m_Original = Clone();
        }

        /// <summary>
        /// Changes the tag of this reaction's LVI to point to the backup reaction. Also returns the backup reaction, with itself as a backup.
        /// </summary>
        public SimpleReaction Revert()
        {
            this.m_bInitialised = false;
            this.SetRegex(s_GeneralReactionRegex.Match(m_Original.ToSaveString(true)), new MessageSource(""), "");
            this.Enabled = m_Original.Enabled;
            if (!m_Original.CustomHeatOfReaction)
            {
                this.m_HeatOfReactionSpecie = null;
                this.m_dHeatOfReaction = double.NaN;
            }
            m_bChanged = false;
            UpdateStatus(this, new EventArgs());
            this.m_bInitialised = true;
            return this;
        }

        /// <summary>
        /// Sets coeffients for all compounds but the first reactant.
        /// </summary>
        public void SetCoefficients(Fraction[] newCoefficients, List<Compound> extraComps)
        {
            for (int i = 1; i < m_OrderedReactants.Count; i++)
                m_Reactants[m_OrderedReactants[i]] = newCoefficients[i - 1];
            for (int i = 0; i < m_OrderedProducts.Count; i++)
                m_Products[m_OrderedProducts[i]] = newCoefficients[i - 1 + m_Reactants.Count];
            int c = m_Reactants.Count - 1 + m_Products.Count;
            for (int i = 0; i < extraComps.Count; i++)
                if (newCoefficients[i + c] < 0)
                {
                    m_OrderedReactants.Add(extraComps[i]);
                    m_Reactants[extraComps[i]] = -newCoefficients[i + c];
                }
                else if (newCoefficients[i + c] > 0)
                {
                    m_OrderedProducts.Add(extraComps[i]);
                    m_Products[extraComps[i]] = newCoefficients[i + c];
                }

            //Move the negative coefficicent values:
            for (int i = m_OrderedReactants.Count - 1; i >= 0; i--)
            {
                Compound comp = m_OrderedReactants[i];
                if (m_Reactants[comp] < 0)
                {
                    if (m_Products.ContainsKey(comp))
                        m_Products[comp] -= m_Reactants[comp];
                    else
                    {
                        m_Products[comp] = -m_Reactants[comp]; m_OrderedProducts.Add(comp);
                    }
                    m_OrderedReactants.Remove(comp); m_Reactants.Remove(comp);
                }
            }
            for (int i = m_OrderedProducts.Count - 1; i >= 0; i--)
            {
                Compound comp = m_OrderedProducts[i];
                if (m_Products[comp] < 0)
                {
                    if (m_Reactants.ContainsKey(comp))
                        m_Reactants[comp] -= m_Products[comp];
                    else
                    {
                        m_Reactants[comp] = -m_Products[comp]; m_OrderedReactants.Add(comp);
                    }
                    m_OrderedProducts.Remove(comp); m_Products.Remove(comp);
                }
            }

            //Remove zero values:
            for (int i = m_OrderedReactants.Count - 1; i >= 0; i--)
            {
                Compound comp = m_OrderedReactants[i];
                if (m_Reactants[comp] == 0)
                {
                    m_OrderedReactants.Remove(comp); m_Reactants.Remove(comp);
                }
            }
            for (int i = m_OrderedProducts.Count - 1; i >= 0; i--)
            {
                Compound comp = m_OrderedProducts[i];
                if (m_Products[comp] == 0)
                {
                    m_OrderedProducts.Remove(comp); m_Products.Remove(comp);
                }
            }
            FireChanged();
            if (ReactantsChanged != null)
                ReactantsChanged(this, new EventArgs());
            if (ProductsChanged != null)
                ProductsChanged(this, new EventArgs());
        }

        public void AddReactant(Compound compound, Fraction fraction, int location)
        {
            if (!m_bReactantsOK)
            {
                throw new Exception("Attempt to add reactant when reactant string is unparseable");
            }
            if (m_Reactants.ContainsKey(compound))
            {
                throw new Exception("Attempt to add reactant that is already in the list of reactants");
            }
            //Determine where we want to add the new product:
            int i = GetInsertionIndex(m_OrderedReactants, m_Reactants, location);

            m_OrderedReactants.Insert(i, compound);
            m_Reactants.Add(compound, 1.0);

            FireChanged();
            if (ReactantsChanged != null)
                ReactantsChanged(this, new EventArgs());
        }

        public void AddProduct(Compound compound, Fraction fraction, int location)
        {
            if (!m_bProductsOK)
            {
                throw new Exception("Attempt to add reactant when product string is unparseable");
            }
            if (m_Products.ContainsKey(compound))
            {
                throw new Exception("Attempt to add product that is already in the list of products");
            }
            //Determine where we want to add the new product:
            int i = GetInsertionIndex(m_OrderedProducts, m_Products, location);
            m_OrderedProducts.Insert(i, compound);
            m_Products.Add(compound, 1.0);

            FireChanged();
            if (ProductsChanged != null)
                ProductsChanged(this, new EventArgs());
        }

        public void MoveReactant(int oldIndex, int newIndex)
        {
            int insert = GetMovingIndex(m_OrderedReactants, m_Reactants, newIndex);
            int remove = GetMovingIndex(m_OrderedReactants, m_Reactants, oldIndex);
            Compound comp = m_OrderedReactants[remove];
            if (insert < remove)
            {
                m_OrderedReactants.RemoveAt(remove);
                m_OrderedReactants.Insert(insert, comp);
            }
            else if (remove < insert)
            {
                if (insert < m_OrderedReactants.Count)
                    insert++;
                m_OrderedReactants.Insert(insert, comp);
                m_OrderedReactants.RemoveAt(remove);
            }
            else
                return;
            FireChanged();
            if (ReactantsChanged != null)
                ReactantsChanged(this, new EventArgs());
        }

        public void MoveProduct(int oldIndex, int newIndex)
        {
            int insert = GetMovingIndex(m_OrderedProducts, m_Products, newIndex);
            int remove = GetMovingIndex(m_OrderedProducts, m_Products, oldIndex);
            Compound comp = m_OrderedProducts[remove];
            if (insert < remove)
            {
                m_OrderedProducts.RemoveAt(remove);
                m_OrderedProducts.Insert(insert, comp);
            }
            else if (remove < insert)
            {
                if (insert < m_OrderedProducts.Count)
                    insert++;
                m_OrderedProducts.Insert(insert, comp);
                m_OrderedProducts.RemoveAt(remove);
            }
            else
                return;
            FireChanged();
            if (ProductsChanged != null)
                ProductsChanged(this, new EventArgs());
        }

        public void MoveReactantToProducts(int oldIndex, int newIndex)
        {
            int insert = GetMovingIndex(m_OrderedProducts, m_Products, newIndex);
            int remove = GetMovingIndex(m_OrderedReactants, m_Reactants, oldIndex);
            Compound comp = m_OrderedReactants[remove];
            m_OrderedReactants.RemoveAt(remove);
            if (insert < m_OrderedProducts.Count && newIndex != 0)
                insert++;
            if (!m_OrderedProducts.Contains(comp))
            {
                m_OrderedProducts.Insert(insert, comp);
                m_Products[comp] = m_Reactants[comp];
            }
            else
                m_Products[comp] += m_Reactants[comp];
            m_Reactants.Remove(comp);
            FireChanged();
            if (ProductsChanged != null)
                ProductsChanged(this, new EventArgs());
            if (ReactantsChanged != null)
                ReactantsChanged(this, new EventArgs());
        }

        public void MoveProductToReactants(int oldIndex, int newIndex)
        {
            int insert = GetMovingIndex(m_OrderedReactants, m_Reactants, newIndex);
            int remove = GetMovingIndex(m_OrderedProducts, m_Products, oldIndex);
            Compound comp = m_OrderedProducts[remove];
            m_OrderedProducts.RemoveAt(remove);
            if (!m_OrderedReactants.Contains(comp))
            {
                m_OrderedReactants.Insert(insert, comp);
                m_Reactants[comp] = m_Products[comp];
            }
            else
                m_Reactants[comp] += m_Products[comp];
            m_Products.Remove(comp);
            FireChanged();
            if (ReactantsChanged != null)
                ReactantsChanged(this, new EventArgs());
            if (ProductsChanged != null)
                ProductsChanged(this, new EventArgs());
        }

        public void RemoveReactant(int charIndex)
        {
            int i = GetMovingIndex(m_OrderedReactants, m_Reactants, charIndex);
            Compound comp = m_OrderedReactants[i];
            m_Reactants.Remove(comp);
            m_OrderedReactants.RemoveAt(i);

            FireChanged();
            if (ReactantsChanged != null)
                ReactantsChanged(this, new EventArgs());
        }

        public void RemoveProduct(int charIndex)
        {
            int i = GetMovingIndex(m_OrderedProducts, m_Products, charIndex);
            Compound comp = m_OrderedProducts[i];
            m_Products.Remove(comp);
            m_OrderedProducts.RemoveAt(i);

            FireChanged();
            if (ProductsChanged != null)
                ProductsChanged(this, new EventArgs());
        }

        public override string ToString()
        {
            if (m_bUseOriginalString)
            {
                return m_sOriginalString;
            }
            string arrow = "";
            switch (m_eDirection)
            {
                case RxnDirections.Forward:
                    arrow = " -> ";
                    break;
                case RxnDirections.Backward:
                    arrow = " <- ";
                    break;
                case RxnDirections.Equilibrium:
                    arrow = " = ";
                    break;
            }
            return GetReactantsString() + arrow + GetProductsString();
        }

        public string ToSaveString(bool includeSequence) { return ToSaveString(includeSequence, true); }

        public string ToSaveString(bool includeSequence, bool includeComment)
        {
            StringBuilder sb = new StringBuilder();
            if (includeComment)
                sb.AppendLine(";RC" + (m_bEnabled ? m_nReactionNumber.ToString() : "-") + ": " + m_sComment);
            if (!m_bEnabled) sb.Append("; ");
            sb.AppendLine(this.ToString()); //Formula
            if (!m_bEnabled) sb.Append("; ");
            sb.AppendLine(" Extent : " + m_Extent.ToString());
            if (CustomHeatOfReaction)
            {
                if (!m_bEnabled) sb.Append("; ");
                sb.AppendLine(HeatOfReactionString);
            }
            if (includeSequence)
            {
                if (!m_bEnabled) sb.Append("; ");
                sb.AppendLine(" Sequence : " + m_nSequence);
            }

            if (!m_bEnabled)
                sb.AppendLine(";Enabled=False");

            return sb.ToString();
        }

        public int ProductIndex(Compound c)
        {
            return GetCompoundIndex(m_OrderedProducts, m_Products, c);
        }

        public int ReactantIndex(Compound c)
        {
            return GetCompoundIndex(m_OrderedReactants, m_Reactants, c);
        }

        public string GetProductsString()
        {
            if (!m_bProductsOK)
                return m_sProductString;
            string ret = "";
            foreach (Compound c in m_OrderedProducts)
            {
                ret += m_Products[c].ToString() + " " + c + " + ";
            }
            if (ret.Length > 3)
                ret = ret.Substring(0, ret.Length - 3); //Trim the trailing " + "
            return ret;
        }

        public string GetReactantsString()
        {
            if (!m_bReactantsOK)
                return m_sReactantString;
            string ret = "";
            foreach (Compound c in m_OrderedReactants)
            {
                ret += m_Reactants[c].ToString() + " " + c + " + ";
            }
            if (ret.Length > 3)
                ret = ret.Substring(0, ret.Length - 3); //Trim the trailing " + "
            return ret;
        }

        public void SetString(string NewString)
        {
            try
            {
                m_bUseOriginalString = true;
                NewString.Trim();
                m_sOriginalString = NewString;
                if (NewString.Contains("->"))
                    m_eDirection = RxnDirections.Forward;
                else if (NewString.Contains("<->") || NewString.Contains("="))
                    m_eDirection = RxnDirections.Equilibrium;
                else if (NewString.Contains("<-"))
                    m_eDirection = RxnDirections.Backward;
                else
                {
                    Program.Log.Message("Reactant Divider not found", MessageType.Error);
                    return;
                }

                string[] splitString = NewString.Split(new char[] { '-', '=' }, StringSplitOptions.None);
                if (splitString.Length != 2)
                {
                    Program.Log.Message("More than one reactant divider found", MessageType.Error);
                    return;
                }

                string ReactantsString = splitString[0].TrimEnd('<');
                string ProductsString = splitString[1].TrimStart('>');
                m_bUseOriginalString = false;

                Exception MaybeEx = null;
                try
                {
                    ParseReactants(ReactantsString);
                }
                catch (Exception ex)
                {
                    MaybeEx = ex;
                }

                ParseProducts(ProductsString);

                if (MaybeEx != null)
                    throw MaybeEx; //Similar to a "finally", except for the order the code is executed...
            }
            finally
            {
                try
                {
                    FireChanged();
                }
                catch (Exception ex)
                {
#if DEBUG
                    throw ex; //Because this won't be caught by the usual try / catch handlers, and we don't want the program to crash after we ship it to the user.
#endif
                }
            }
        }

        public void ParseReactants(string reactantString) { ParseReactants(reactantString, false); }

        public void ParseReactants(string reactantString, bool bLogErrors)
        {
            if (reactantString == GetReactantsString())
                return;

            bool wasActive = Program.Log.Active;

            try
            {
                Program.Log.Active = bLogErrors;
                m_sReactantString = reactantString;
                m_bReactantsOK = false;
                //m_Reactants.Clear(); m_OrderedReactants.Clear();
                m_Reactants = ReadCompounds(reactantString, out m_OrderedReactants, out m_bReactantsOK);
            }
            finally
            {
                try
                {
                    Program.Log.Active = wasActive;
                    if (ReactantsChanged != null)
                        ReactantsChanged(this, new EventArgs());
                    FireChanged();
                }
                catch (Exception ex)
                {
#if DEBUG
                    Console.WriteLine(ex.ToString());
                    throw ex;
#endif
                }
            }

        }

        public void ParseProducts(string productString)
        {
            ParseProducts(productString, false);
        }

        public void ParseProducts(string productString, bool LogErrors)
        {
            if (productString == GetProductsString())
                return;
            bool wasActive = Program.Log.Active;
            try
            {
                Program.Log.Active = LogErrors;
                m_sProductString = productString;
                m_bProductsOK = false;
                //m_OrderedProducts.Clear(); m_Products.Clear();
                m_Products = ReadCompounds(productString, out m_OrderedProducts, out m_bProductsOK);
                //m_bProductsOK = true;
            }
            finally
            {
                try
                {
                    Program.Log.Active = wasActive;
                    if (ProductsChanged != null)
                        ProductsChanged(this, new EventArgs());
                    FireChanged();
                }
                catch (Exception ex)
                {
#if DEBUG
                    Console.WriteLine(ex.ToString());
                    throw ex; //Because this won't be caught by the usual try / catch handlers, and we don't want the program to crash after we ship it to the user.
#endif
                }
            }
        }

        public void ParseExtent(string extentString)
        {
            try
            {
                m_Extent = RxnExtent.Parse(extentString, out m_eExtentType);
                if (m_Extent.Specie == null)
                    m_Extent.Specie = FirstReactant;
            }
            finally
            {
                m_Extent.Owner = this;
                FireChanged(); //?This necessary?
            }
        }

        public void ParseHOR(string HORString)
        {
            Match m = s_HORRegex.Match(HORString);
            if (!m.Success)
                throw new Exception("Unable to parse string");
            HeatOfReactionValue = double.Parse(m.Groups["Value"].Value);
            Exception MaybeEx = null;
            if (m.Groups["Specie"].Success)
                try { HeatOfReactionSpecie = Compound.FromString(m.Groups["Specie"].Value); }
                catch (Exception ex) { MaybeEx = ex; }
            else
                HeatOfReactionSpecie = FirstReactant;

            if (m.Groups["Type"].Captures[0].Value.ToLowerInvariant() == "msfixed")
                HeatOfReactionType = FracTypes.ByMass;
            else
                HeatOfReactionType = FracTypes.ByMole;

            if (m.Groups["Condition"].Success)
            {
                if (m.Groups["Condition"].Value.ToLowerInvariant() == "feed")
                    m_eHeatOfReactionConditions = TPConditions.Feed;
                else if (m.Groups["Condition"].Value.ToLowerInvariant() == "prod")
                    m_eHeatOfReactionConditions = TPConditions.Product;
                else if (m.Groups["Condition"].Value.ToLowerInvariant() == "std")
                    m_eHeatOfReactionConditions = TPConditions.Standard;
            }
            else if (m.Groups["T"].Success)
            {
                m_eHeatOfReactionConditions = TPConditions.Custom;
                if (!double.TryParse(m.Groups["T"].Value, out m_dHeatOfReactionT))
                    m_dHeatOfReactionT = 25;
                if (!double.TryParse(m.Groups["P"].Value, out m_dHeatOfReactionP))
                    m_dHeatOfReactionP = 101.3;

            }

            if (MaybeEx != null)
                throw MaybeEx;
        }

        public void FireChanged()
        {
            if (Changed != null)
                Changed(this, new EventArgs());
        }

        public SimpleReaction Clone()
        {
            //Simply clone it with a dummy LVI:
            //ListViewItem lvi = new ListViewItem();
            //lvi.SubItems.AddRange(new string[] { "", "", "" });
            return Clone(null);
        }

        public SimpleReaction Clone(ListViewItem newLVI)
        {
            SimpleReaction ret = new SimpleReaction(newLVI);
            ret.SetRegex(s_GeneralReactionRegex.Match(this.ToSaveString(true)), 
                new MessageSource("SimpleReaction.Clone"), "Clone");
            /*SimpleReaction ret = (SimpleReaction) this.MemberwiseClone();
            ret.LVI = newLVI;
            //Items which must make their own clones:
            ret.m_Extent = m_Extent.Clone(ret);
            ret.m_OrderedProducts = new List<Compound>(m_OrderedProducts);
            ret.m_OrderedReactants = new List<Compound>(m_OrderedReactants);
            ret.m_Products = new Dictionary<Compound, Fraction>(m_Products);
            ret.m_Reactants = new Dictionary<Compound, Fraction>(m_Reactants);
            ret.m_Unbalanced = new Dictionary<Element, Fraction>(m_Unbalanced);
            ret.Changed = null;
            ret.Changed += new EventHandler(ret.UpdateStatus);*/
            return ret;
        }

        public void DoDatabaseChanged()
        {
            //Extent:
            if (m_Extent.Specie != null && !Compound.CompoundList.ContainsKey(m_Extent.Specie.Symbol))
            {
                Program.Log.Message("Extent specie not found in new database", MessageType.Error);
                m_Extent.Specie = null;
            }
            //HOR:
            if (CustomHeatOfReaction && !Compound.CompoundList.ContainsKey(m_HeatOfReactionSpecie.Symbol))
            {
                Program.Log.Message("Heat of reaction specie not found in new database", MessageType.Error);
                m_HeatOfReactionSpecie = null;
                FireChanged();
            }
            //Spaces to force a reparse:
            ParseProducts(GetProductsString() + " "); 
            ParseReactants(GetReactantsString() + " ");
        }

        public bool IsBalanceable() { return IsBalanceable(new List<Compound>()); }
        public bool IsBalanceable(List<Compound> extraComps) 
        {
            return GetBalanceMatrix(extraComps).IsSolveable(); 
        }

        public Matrix.RemovalInfo BalanceOptions() { return BalanceOptions(new List<Compound>()); }
        public Matrix.RemovalInfo BalanceOptions(List<Compound> extraComps)
        {
            Matrix m = GetBalanceMatrix(extraComps);
            if (!m.IsSolveable())
                throw new InvalidOperationException("System is over-constrained, and not solveable.");
            Matrix.RemovalInfo info = m.GetRemovalInfo();
            return info;
        }

        public void BalanceWith(List<Compound> extraComps)
        {
            Matrix m = GetBalanceMatrix(extraComps);
            Fraction[] vals = m.getVariableValues();
            SetCoefficients(vals, extraComps);
        }

        public Matrix GetBalanceMatrix()
        {
            return GetBalanceMatrix(new List<Compound>());
        }

        public Matrix GetBalanceMatrix(List<Compound> extraComps)
        {
            if (m_Reactants.Count == 0)
                return Matrix.InvalidMatrix;
            foreach (Compound c in Compounds)
                while (extraComps.Contains(c))
                    extraComps.Remove(c);
            List<Element> elements = ElementsWith(extraComps);
            Matrix m = new Matrix(elements.Count, m_OrderedProducts.Count + m_OrderedReactants.Count + extraComps.Count);
            for (int i = 0; i < elements.Count; i++)
            {
                for (int j = 1; j < m_OrderedReactants.Count; j++)
                    m[i, j - 1] = m_OrderedReactants[j].Elements.ContainsKey(elements[i]) ? -m_OrderedReactants[j].Elements[elements[i]] : 0;
                int c = m_OrderedReactants.Count - 1;
                for (int j = 0; j < m_OrderedProducts.Count; j++)
                    m[i, j + c] = m_OrderedProducts[j].Elements.ContainsKey(elements[i]) ? m_OrderedProducts[j].Elements[elements[i]] : 0;
                c += m_OrderedProducts.Count;
                for (int j = 0; j < extraComps.Count; j++)
                    m[i, j + c] = extraComps[j].Elements.ContainsKey(elements[i]) ? extraComps[j].Elements[elements[i]] : 0;
                m[i, m.Columns - 1] = FirstReactant.Elements.ContainsKey(elements[i]) ? m_Reactants[FirstReactant] * FirstReactant.Elements[elements[i]] : 0;
            }
            //extraComps (Now all comps) can go negative:
            for (int i = 0/*m_OrderedProducts.Count - 1 + m_OrderedReactants.Count*/; i < m.Columns - 1; i++)
                m.CanGoNegative(i, true);
            return m;
        }
        #endregion Public Functions

        #region Protected Functions
        protected List<Element> ElementsWith(List<Compound> extraCompounds)
        {
            List<Element> ret = new List<Element>();
            foreach (Compound c in Compounds)
                foreach (Element e in c.Elements.Keys)
                    if (!ret.Contains(e))
                        ret.Add(e);
            foreach (Compound c in extraCompounds)
                foreach (Element e in c.Elements.Keys)
                    if (!ret.Contains(e))
                        ret.Add(e);
            return ret;
        }

        protected int GetInsertionIndex(List<Compound> orderedComps, Dictionary<Compound, Fraction> Comps, int location)
        {
            int i = 0;
            string tempCompString = "";
            while (i < orderedComps.Count)
            {
                tempCompString += Comps[orderedComps[i]] + " ";
                if (location <= tempCompString.Length)
                    break;
                tempCompString += orderedComps[i] + " + ";
                i++;
            }
            m_nLastAddedNumStart = tempCompString.Length;
            if (i < orderedComps.Count)
                m_nLastAddedNumStart -= 2;
            return i;
        }

        protected int GetCompoundIndex(List<Compound> orderedComps, Dictionary<Compound, Fraction> Comps, Compound comp)
        {
            int i = 0;
            string tempCompString = "";
            while (i < orderedComps.Count)
            {
                if (orderedComps[i] == comp)
                    break;
                tempCompString += Comps[orderedComps[i]] + " ";
                tempCompString += orderedComps[i] + " + ";
                i++;
            }
            return tempCompString.Length;
        }

        protected int GetMovingIndex(List<Compound> orderedComps, Dictionary<Compound, Fraction> Comps, int location)
        {
            if (location == 0) return 0;
            int i = 0;
            string tempCompString = "";
            while (i < orderedComps.Count)
            {
                tempCompString += Comps[orderedComps[i]] + " ";
                tempCompString += orderedComps[i];
                if (i < orderedComps.Count - 1)
                    tempCompString += " + ";
                if (location < tempCompString.Length - 1)
                    break;
                i++;
            }
            return i;
        }

        protected void UpdateStatus(object sender, EventArgs e)
        {
            SetStatus();

            if (m_bInitialised)
                m_bChanged = true;

            if (m_LVI == null) return;
            m_LVI.Text = (m_bEnabled ? m_nReactionNumber.ToString() : "-")
                + OriginalReactionNumberString
                + (HasChanged ? "*" : "");
            m_LVI.SubItems[1].Text = this.ToString();
            m_LVI.SubItems[2].Text = m_Extent.ToString();
            m_LVI.SubItems[3].Text = CustomHeatOfReaction ? HeatOfReactionValue + (HeatOfReactionType == FracTypes.ByMole ? " kJ/mol " : " kJ/kg ") + HeatOfReactionSpecie : "";
            m_LVI.SubItems[4].Text = m_nSequence.ToString();
            if (m_bEnabled)
                switch (m_eStatus)
                {
                    case RxnStatuses.Imbalanced:
                        m_LVI.ForeColor = Color.Orange;
                        break;
                    case RxnStatuses.Invalid:
                        m_LVI.ForeColor = Color.Red;
                        break;
                    case RxnStatuses.OK:
                        m_LVI.ForeColor = System.Drawing.SystemColors.WindowText;
                        break;
                }
            else
                m_LVI.ForeColor = System.Drawing.SystemColors.GrayText;
        }

        protected Dictionary<Compound, Fraction> ReadCompounds(string halfReaction, out List<Compound> Order, out bool Success)
        {
            Success = true;
            string[] Compounds = halfReaction.Split('+');
            for (int i = 0; i < Compounds.Length; i++)
                Compounds[i] = Compounds[i].Trim();
            Dictionary<Compound, Fraction> ret = new Dictionary<Compound, Fraction>();
            Order = new List<Compound>();
            Match m = s_CompoundRegex.Match(halfReaction);
            if (!m.Success)
            {
                Program.Log.Message("Unable to parse compound string '" + halfReaction + "'", MessageType.Error);
                Success = false;
            }
            Group grpAmount = m.Groups["Amount"];
            Group grpCompound = m.Groups["Compound"];
            Group grpUnparseable = m.Groups["Unparseable"];
            Group grpSpace = m.Groups["Space"];
            List<string> UnparseableComps = new List<string>();
            List<string> UnparseableTokens = new List<string>();
            for (int i = 0; i < grpAmount.Captures.Count; i++)
            {
                try
                {
                    Fraction val = 1;
                    if (!string.IsNullOrEmpty(grpAmount.Captures[i].Value) && !string.IsNullOrEmpty(grpSpace.Captures[i].Value))
                        val = Fraction.ToFraction(grpAmount.Captures[i].Value);
                    Compound comp = null;
                    string compString;

                    if (grpSpace.Captures[i].Length == 0)
                        compString = grpAmount.Captures[i].Value + grpCompound.Captures[i].Value;
                    else
                        compString = grpCompound.Captures[i].Value;
                    if (Compound.Contains(compString))
                        comp = Compound.FromString(compString);
                    else if (grpSpace.Captures[i].Length == 0 && Compound.Contains(grpCompound.Captures[i].Value))
                    {
                        comp = Compound.FromString(grpCompound.Captures[i].Value);
                        if (grpAmount.Captures[i].Length != 0)
                            val = Fraction.ToFraction(grpAmount.Captures[i].Value);
                    }
                    else
                    {
                        UnparseableComps.Add(compString);
                        Success = false;
                    }

                    if (comp != null)
                        if (ret.ContainsKey(comp))
                            ret[comp] += val;
                        else
                        {
                            ret.Add(comp, val);
                            Order.Add(comp);
                        }
                }
                catch (Exception ex)
                {
                    Program.Log.Message(ex.Message, MessageType.Error);
                }
            }
            foreach (Capture c in grpUnparseable.Captures)
            {
                if (!string.IsNullOrEmpty(c.Value.Trim()))
                {
                    UnparseableTokens.Add(c.Value);
                    Success = false;
                }
            }
            if (UnparseableComps.Count > 0)
            {
                string startString = "Unable to parse compounds: '";
                if (UnparseableComps.Count == 1)
                    Program.Log.Message("Unable to parse compound '" + UnparseableComps[0] + "'", MessageType.Warning);
                else
                {
                    startString += UnparseableComps[0];
                    for (int i = 1; i < UnparseableComps.Count; i++)
                        startString += "', '" + UnparseableComps[i];
                    startString += "'";
                    Program.Log.Message(startString, MessageType.Warning);
                }
            }
            if (UnparseableTokens.Count > 0)
            {
                string startString = "Unkown tokens: '";
                if (UnparseableTokens.Count == 1)
                    Program.Log.Message("Unknown token '" + UnparseableTokens[0] + "'", MessageType.Warning);
                else
                {
                    startString += UnparseableTokens[0];
                    for (int i = 1; i < UnparseableTokens.Count; i++)
                        startString += "', '" + UnparseableTokens[i];
                    startString += "'";
                    Program.Log.Message(startString, MessageType.Warning);
                }
            }
            return ret;
        }
        
        protected void SetStatus()
        {
            if (m_bUseOriginalString || !m_bProductsOK || !m_bReactantsOK || !m_Extent.IsValid()
                || m_Products.Count == 0 || m_Reactants.Count == 0 || !HeatOfReactionOK)
                m_eStatus = RxnStatuses.Invalid;
            else
                m_eStatus = Balanced ? RxnStatuses.OK : RxnStatuses.Imbalanced;
        }
        #endregion Protected Functions

    }

    #region Log
    public enum MessageType {Error, Warning, Note};

    public class MessageSource
    {
        public MessageSource(string _Source)
        { Source = _Source; }
        public string Source;
        public virtual void DoCallback()
        { }
    }

    /// <summary>
    /// A single threaded log. If used on multiple threads, each thread should have its own ILog object.
    /// </summary>
    public interface ILog
    {
        /// <summary>
        /// Notes a message in the log
        /// </summary>
        /// <param name="msg">The main information of the message</param>
        /// <param name="msgType">The message type</param>
        /// <param name="src">The source of the message. If <value>null</value>, message uses last source from SetSource</param>
        void Message(string msg, MessageType msgType, MessageSource src);

        /// <summary>
        /// Notes a message in the log
        /// </summary>
        /// <param name="msg">The main information of the message</param>
        /// <param name="msgType">The message type</param>
        void Message(string msg, MessageType msgType);

        /// <summary>
        /// Sets the default message source. This should be implemented as a stack.
        /// </summary>
        /// <param name="src"></param>
        void SetSource(MessageSource src);

        /// <summary>
        /// Sets the default message source to be the new source on the stack.
        /// </summary>
        void RemoveSource();

        bool Active
        {
            get;
            set;
        }
    }

    public class ConsoleLog : ILog
    {
        #region ILog Members
        public Stack<MessageSource> sourceStack = new Stack<MessageSource>();
        public void Message(string msg, MessageType msgType, MessageSource src)
        {
            Console.WriteLine(msg + ", " + msgType + ", " + src.Source);
        }

        public void Message(string msg, MessageType msgType)
        {
            string srcString = "<null>";
            if (sourceStack.Count != 0)
                srcString = sourceStack.Peek().Source;
            Console.WriteLine(msg + ", " + msgType + ", " + srcString);
        }

        public void SetSource(MessageSource src)
        {
            sourceStack.Push(src);
        }

        public void RemoveSource()
        {
            sourceStack.Pop();
        }

        public bool Active
        {
            get { return true; }
            set { }
        }

        #endregion
    }
    #endregion Log

    #region Matrix
    /// <summary>
    /// A Matrix class. Has quite a bit of room for optimisation, 
    /// but we should never be using this on excessively large matrices.
    /// </summary>
    public class Matrix
    {
        #region Internal Variables
        Fraction[][] m_Data;
        bool[] m_CanGoNegative;
        int m_nRows, m_nColumns;
        bool m_bReduced = true;
        #endregion Internal Variables

        #region Properties
        public Fraction this[int row, int col]
        {
            get { return m_Data[row][col]; }
            set
            {
                m_Data[row][col] = value;
                m_bReduced = false;
            }
        }

        public int Rows { get { return m_nRows; } }
        public int Columns { get { return m_nColumns; } }

        public int Rank
        {
            get
            {
                RowReduce();
                for (int i = 0; i < Rows; i++)
                {
                    int fnze = FirstNZElem(m_Data[i]);
                    if (fnze < 0 || fnze == Columns - 1)
                        return i;
                }
                return Rows;
            }
        }

        public int DegreesOfFreedom
        {
            get
            {
                return m_nColumns - Rank - 1;
            }
        }
        #endregion Properties

        #region Constructors
        public Matrix(int rows, int columns)
        {
            m_nRows = rows;
            m_nColumns = columns;
            m_Data = new Fraction[rows][];
            for (int i = 0; i < rows; i++)
                m_Data[i] = new Fraction[columns];
            m_CanGoNegative = new bool[columns];
        }

        private Matrix()
        {}
        #endregion Constructors

        #region Public Functions
        public void CanGoNegative(int Col, bool Val)
        {
            if (this == InvalidMatrix)
                return;
            m_CanGoNegative[Col] = Val;
        }

        public bool CanGoNegative(int Col)
        {
            if (this == InvalidMatrix)
                return false;
            return m_CanGoNegative[Col];
        }

        public void RowReduce()
        {
            if (this == InvalidMatrix)
                return;
            if (m_bReduced)
                return;
            int k;
            for (k = 0; k < m_nRows - 1; k++)
            {
                Array.Sort<Fraction[]>(m_Data, new Comparison<Fraction[]>(CompareRows));
                int firstNZElem = FirstNZElem(m_Data[k]);
                if (firstNZElem == -1)
                    break;
                Fraction val = m_Data[k][firstNZElem];
                for (int i = k + 1; i < m_Data.Length; i++)
                {
                    Fraction fac = m_Data[i][firstNZElem] / val;
                    if (fac != 0)
                    {
                        m_Data[i][firstNZElem] = 0;
                        for (int j = firstNZElem + 1; j < m_nColumns; j++)
                        {
                            m_Data[i][j] = m_Data[i][j] - m_Data[k][j] * fac;
                        }
                    }
                }
            }
            //We now have:
            //  #  #  #  #  #  #
            //  0  #  #  #  #  #
            //  0  0  0  #  #  #
            //  0  0  0  0  #  #
            //For a full row reduce:
            for (; k > 0; k--)
            {
                int firstNZElem = FirstNZElem(m_Data[k]);
                if (firstNZElem < 0 || firstNZElem == Columns - 1)
                    continue;
                Fraction val = m_Data[k][firstNZElem];
                for (int i = k - 1; i >= 0; i--)
                {
                    Fraction fac = m_Data[i][firstNZElem] / val;
                    if (fac != 0)
                    {
                        m_Data[i][firstNZElem] = 0;
                        for (int j = firstNZElem + 1; j < m_nColumns; j++)
                        {
                            m_Data[i][j] = m_Data[i][j] - m_Data[k][j] * fac;
                        }
                    }
                }
            }
            m_bReduced = true;
        }

        public bool IsSolveable()
        {
            if (this == InvalidMatrix)
                return false;

            RowReduce();
            for (int i = 0; i < Rows; i++)
                if (FirstNZElem(m_Data[i]) == Columns - 1)
                    return false;
            return true;
        }

        public Fraction[] getVariableValues()
        {
            if (this == InvalidMatrix)
                return null;

            RowReduce();
            Fraction[] ret = new Fraction[Columns - 1];
            for (int i = 0; i < Columns - 1; i++)
                ret[i] = 0;
            int rank = Rank;
            for (int i = rank - 1; i >= 0; i--)
            {
                Fraction t = m_Data[i][Columns - 1];
                for (int j = Columns - 2; j > i; j--)
                    t -= ret[j] * m_Data[i][j];
                ret[i] = t / m_Data[i][i];
            }
            return ret;
        }

        public Matrix Clone()
        {
            if (this == InvalidMatrix)
                return this;

            Matrix ret = new Matrix(this.Rows, this.Columns);
            for (int i = 0; i < Rows; i++)
                Array.Copy(m_Data[i], ret.m_Data[i], Columns);
            return ret;
        }

        /// <summary>
        /// Calculates which columns can be removed and still leave a solveable matrix.
        /// Alternatively, specifies which variables can be adjusted.
        /// </summary>
        /// <returns>A list describing whether each column can be changed or removed.</returns>
        public bool[] ColumnsRemoveable()
        {
            if (this == InvalidMatrix)
                return null;

            RowReduce();
            bool[] removeable = new bool[Columns - 1];
            for (int i = 0; i < Columns - 1; i++)
                removeable[i] = true;
            for (int k = 0; k < Columns - 1; k++)
            {
                for (int i = 0; i < Rows; i++)
                {
                    //if (m_Data[i][Columns - 1] == 0)    //Ignore rows where the sum is to zero.
                      //  continue;
                    if (m_Data[i][k] == 0)              //Ignore rows where this variable is not used
                        continue;
                    int c = 0;
                    for (int j = 0; j < Columns - 1; j++)
                        if (m_Data[i][j] != 0)
                            c++;
                    if (c == 1)
                    {
                        removeable[k] = false;              //If the only variable is this one, it's not removeable
                        break;
                    }
                }
            }
            return removeable;
        }

        public Matrix RemoveColumn(int col)
        {
            if (this == InvalidMatrix)
                return null;

            Fraction[][] newData = new Fraction[Rows][];
            for (int i = 0; i < Rows; i++)
            {
                newData[i] = new Fraction[Columns - 1];
                Array.Copy(m_Data[i], 0, newData[i], 0, col);
                Array.Copy(m_Data[i], col + 1, newData[i], col, Columns - col - 1);
            }
            bool[] newCanGoNegative = new bool[Columns - 1];
            Array.Copy(m_CanGoNegative, 0, newCanGoNegative, 0, col);
            Array.Copy(m_CanGoNegative, col + 1, newCanGoNegative, col, Columns - col - 1);

            Matrix ret = (Matrix) this.MemberwiseClone();

            ret.m_CanGoNegative = newCanGoNegative;
            ret.m_Data = newData;
            ret.m_bReduced = false;
            ret.m_nColumns--;
            return ret;
        }

        public Vector GetColumn(int col)
        {
            if (this == InvalidMatrix)
                return null;

            Vector ret = new Vector(Rows);
            for (int i = 0; i < Rows; i++)
                ret[i] = this[i, col];
            return ret;
        }

        public Matrix InsertColumn(int loc, Vector newCol)
        {
            if (this == InvalidMatrix)
                return null;

            Fraction[][] newData = new Fraction[Rows][];
            for (int i = 0; i < Rows; i++)
            {
                newData[i] = new Fraction[Columns + 1];
                Array.Copy(m_Data[i], 0, newData[i], 0, loc);
                Array.Copy(m_Data[i], loc, newData[i], loc + 1, Columns - loc);
            }
            Matrix ret = (Matrix)this.MemberwiseClone();

            ret.m_Data = newData;
            ret.SetColumn(loc, newCol);
            ret.m_bReduced = false;
            ret.m_nColumns++;
            return ret;
        }

        public void SetColumn(int col, Vector newVal)
        {
            if (this == InvalidMatrix)
                return;

            for (int i = 0; i < Rows; i++)
                this[i, col] = newVal[i];
        }

        
        public RemovalInfo GetRemovalInfo()
        {
            return GetRemovalInfo(true);
        }
        public RemovalInfo GetRemovalInfo(bool ThoroughComparison)
        {
            if (this == InvalidMatrix)
                return null;

            return RecursiveRemoveableChecks(this, ThoroughComparison);
        }
        #endregion Public Functions

        #region Protected Functions
        protected int CompareRows(Fraction[] row1, Fraction[] row2)
        {

            for (int i = 0; i < row1.Length; i++)
                if (row1[i] != 0 && row2[i] != 0)
                    return 0;
                else if (row1[i] == 0 && row2[i] == 0)
                    continue;
                else if (row1[i] == 0)
                    return 1;
                else if (row2[i] == 0)
                    return -1;
            return 0;
        }

        protected int FirstNZElem(Fraction[] row)
        {
            for (int i = 0; i < row.Length; i++)
                if (row[i] != 0)
                    return i;
            return -1;
        }

        protected RemovalInfo RecursiveRemoveableChecks(Matrix m, bool bThoroughComparison)
        {
            if (m.Rank == m.Columns - 1)
            {
                Fraction[] vals = m.getVariableValues();
                bool AllPositive = true;
                for (int j = 0; j < vals.Length; j++)
                    if (vals[j] < 0 && !m.m_CanGoNegative[j])
                    {
                        AllPositive = false;
                        break;
                    }
                if (AllPositive)
                    return new RemovalInfo(true);
                else
                    return new RemovalInfo(false);
            }
            else if (m.Rank < m.Columns - 1)
            {
                bool[] removeables = m.ColumnsRemoveable();
                RemovalInfo ret = new RemovalInfo(m.Columns - 1);
                for (int i = 0; i < removeables.Length; i++)
                {
                    bool found = false;
                    if (!removeables[i] || (!bThoroughComparison && found))
                        ret.m_IfRemoved[i] = new RemovalInfo(false);
                    else
                    {
                        Matrix OneColRemoved = m.RemoveColumn(i);
                        ret.SetIfRemoved(i, RecursiveRemoveableChecks(OneColRemoved, bThoroughComparison));
                    }
                }
                return ret;
            }
            else
                return new RemovalInfo(false);  //If we call this on a matrix with too high Rank, it's not solveable (And so nothing is removeable)
        }
        #endregion Protected Functions

        #region Subclasses
        public class RemovalInfo
        {
            public RemovalInfo() { }
            public RemovalInfo(bool CanRemove) { m_bCanRemove = CanRemove; }
            public RemovalInfo(int subRemoveCounts) { m_IfRemoved = new RemovalInfo[subRemoveCounts]; }
            public bool m_bCanRemove = false;
            public RemovalInfo[] m_IfRemoved;
            public RemovalInfo m_Parent;

            public void SetIfRemoved(int i, RemovalInfo info)
            {
                m_IfRemoved[i] = info;
                if (info.m_bCanRemove)
                    m_bCanRemove = true;
                info.m_Parent = this;
            }

            public int DegreesOfFreedom
            {
                get
                {
                    if (m_IfRemoved == null)
                        return 0;
                    else
                        for (int i = 0; i < m_IfRemoved.Length; i++)
                            if (m_IfRemoved[i] != null && m_IfRemoved[i].m_bCanRemove)
                                return m_IfRemoved[i].DegreesOfFreedom + 1;
                    return 0;
                }
            }
        }

        public class Vector
        {
            public Vector(int dimension)
            {
                vals = new Fraction[dimension];
            }
            protected Fraction[] vals;
            public Fraction this[int index]
            {
                get { return vals[index]; }
                set { vals[index] = value; }
            }
            public int Dimension
            {
                get { return vals.Length; }
            }
            public static Vector operator +(Vector v1, Vector v2)
            {
                if (v1.Dimension != v2.Dimension)
                    throw new ArgumentException("Dimensions of vectors are not equal");
                Vector ret = new Vector(v1.Dimension);
                for (int i = 0; i < ret.Dimension; i++)
                    ret[i] = v1[i] + v2[i];
                return ret;
            }
            public static Vector operator -(Vector v)
            {
                Vector ret = new Vector(v.Dimension);
                for (int i = 0; i < ret.Dimension; i++)
                    ret[i] = -v[i];
                return ret;
            }
            public static Vector operator -(Vector v1, Vector v2)
            {
                return v1 + (-v2);
            }
            public static Vector operator *(Vector v, Fraction coeff)
            {
                Vector ret = new Vector(v.Dimension);
                for (int i = 0; i < ret.Dimension; i++)
                    ret[i] = v[i] * coeff;
                return ret;
            }
        }
        #endregion Subclasses

        #region Overrides
        public override string ToString()
        {
            StringBuilder sb = new StringBuilder(Columns * Rows);
            for (int i = 0; i < Rows; i++)
                for (int j = 0; j < Columns; j++)
                    if (j < Columns - 1)
                        sb.Append(this[i, j] + ", ");
                    else
                        sb.AppendLine(this[i, j]);
            return sb.ToString();
        }
        #endregion Overrides

        public static Matrix InvalidMatrix = new Matrix();
    }

    public class CompoundDrag
    {
        public CompoundDrag(Control _ctrl, int _startIndex, FrmReaction _frm)
        {
            ctrl = _ctrl;
            startIndex = _startIndex;
            frm = _frm;
        }
        public FrmReaction frm;
        public Control ctrl;
        public int startIndex;
    }

    #endregion Matrix

    #region Fraction
    /*public struct Fraction : IComparable
    {
        /// <summary>
        /// Used in conversion from a double to a fraction.
        /// </summary>
        public static double AccuracyRequired;
        public Fraction(int Num, int Den)
        {
            if (Den == 0)
                throw new Exception("Zero denominator passed to Fraction");
            m_nNumerator = Num;
            m_nDenominator = Den;
        }
        public Fraction(double value)
        {
            throw new Exception("Fraction(double) Constructor Not Implemented");
        }
        public int Denominator
        {
            get { return m_nDenominator; }
            set
            {
                if (value == 0) throw new Exception("Attempt to create zero denominator fraction");
                m_nDenominator = value;
                Factorise();
            }
        } private int m_nDenominator;
        public int Numerator
        {
            get { return m_nNumerator; }
            set 
            { 
                m_nNumerator = value;
                Factorise();
            }
        } private int m_nNumerator;
        public static implicit operator double(Fraction frac)
        {
            return (double)frac.Numerator / frac.Denominator;
        }

        public static Fraction operator +(Fraction frac1, Fraction frac2)
        {
            int newDen = frac1.Denominator * frac2.Denominator;
            int newNum = frac1.Numerator * frac2.Denominator + frac2.Numerator * frac1.Denominator;
            return new Fraction(newNum, newDen);
        }

        public override string ToString()
        {
            if (Denominator != 1)
                return Numerator + "/" + Denominator;
            else
                return Numerator.ToString();
        }

        public override bool Equals(object obj)
        {
            if (!(obj is double) || !(obj is Fraction))
                throw new InvalidOperationException();
            return (double)this == (double)obj;
        }

        private void Factorise()
        {
            throw new Exception("Fraction.Factorise() not yet implemented");
        }

        #region IComparable Members
        public int CompareTo(object obj)
        {
            if (obj is double || obj is Fraction)
                return ((double)this).CompareTo((double)obj);
            throw new InvalidOperationException();
        }
        #endregion IComparable Members
    }*/
    #endregion Fraction
}