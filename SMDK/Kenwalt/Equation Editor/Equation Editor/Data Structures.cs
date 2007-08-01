using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Windows.Forms;
using System.Drawing;
using System.Text.RegularExpressions;
using System.Text;
using System.Runtime.Serialization;
using System.Collections.ObjectModel;
using System.Runtime.InteropServices;

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
    public enum HXTypes { None, FinalT, ApproachT, ApproachAmbient, Power, Electrolysis };
    public enum FracTypes { ByMass, ByMole };
    public enum TPConditions { Feed, Product, Standard, Custom };

    #region Reaction Extent Structures
    [Serializable]
    public class RxnExtent
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
                m_Specie = value; 
                if (m_Owner != null)
                    m_Owner.FireChanged();
            }
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
                if (s.Contains(" Rate "))
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
            if (m.Groups["FinalFraction"].Success)
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

    [Serializable]
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
            if (m.Groups["Rate"].Success)
                ret = new RateExtent(null);
            else
                ret = new FractionExtent();
            if (m.Groups["Specie"].Success)
                ret.Specie = Compound.FromString(m.Groups["Specie"].Value);
            ret.Value = double.Parse(m.Groups["Value"].Value);
            if (m.Groups["Percent"].Success)
                ret.Value *= 0.01;
            if (m.Groups["Rate"].Success && m.Groups["Stabilised"].Success)
                ((RateExtent)ret).Stabilised = true;
            return ret;
        }
    }

    [Serializable]
    public class RatioExtent : RxnExtent
    {
        protected Compound m_Specie2;
        protected double m_Ratio2 = double.NaN;
        protected double m_Time = double.NaN;
        public RatioExtent(RxnExtent original)
            : base(original) { }
        public RatioExtent() : base() { }
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
            return "Ratio - Not implemented.";
        }

        public override bool IsValid()
        {
            return base.IsValid() && Specie2 != null;
        }

        public static RatioExtent Parse(string s)
        {
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
        }
    }

    [Serializable]
    public class EquilibriumExtent : RxnExtent
    {
        public EquilibriumExtent(RxnExtent original)
            : base(original) { }

        public EquilibriumExtent() : base () {}

        public override string ToString()
        {
            return "Equilibrium " + Value;
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

    [Serializable]
    public class Final_ConcExtent : RxnExtent
    {
        protected static Regex s_FinalConcRegex = new Regex(
            @"FinalConc\s*(?<Aim>Target|Strict)\s*(?<Specie>[^\s=]*)?\s*=\s*(?<Value>\d+(\.\d+)?|\.\d+)\s*(At\s*(?<T>\d+(\.\d+)?|\.\d+))?",
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
                m_dT = value;
                if (m_Owner != null)
                    m_Owner.FireChanged();
            }
        }

        public override string ToString()
        {
            return "FinalConc " + Specie + " = " + Value + (double.IsNaN(T) ? "" : " At " + T);
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

    [Serializable]
    public class Final_FracExtent : RxnExtent
    {
        protected Regex m_FinalFracRegex = new Regex(
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

        public override double Value
        {
            get { return base.Value; }
            set { base.Value = value > 1 ? 1 : value; }
        }

        public static Final_FracExtent Parse(string s)
        {
            Final_FracExtent ret = new Final_FracExtent(null);
            Match m = s_ExtentRegex.Match(s);
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

    [Serializable]
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
                m_bStabilised = value;
                if (m_Owner != null)
                    m_Owner.FireChanged();
            }
        }

        public override string ToString()
        {
            return "Fraction " + Specie + " Rate = " + Value + (Stabilised ? "Stabilised" : "");
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
        private Dictionary<Element, double> m_Elements = new Dictionary<Element, double>();
        private int m_nIndex;
        #endregion Internal Variables
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
        public Dictionary<Element, double> Elements
        {
            get { return m_Elements; }
            set { m_Elements = value; }
        }

        public int Index
        {
            get { return m_nIndex; }
            set { m_nIndex = value; }
        }

        public string Annotation;
        #endregion


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

    public class SimpleReaction
    {
        public static DataFormats.Format sFormat = DataFormats.GetFormat(typeof(SimpleReaction).FullName);
        public static double sMinValue = 5E-3;

        #region regex's
        public static Regex s_SequenceRegex = new Regex(@"^Sequence\s*:\s*(?<Value>\d+)$",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled);
        public static Regex s_HORRegex = new Regex(
            @"^\s*HeatOfReaction\s*:\s*(?<Type>MsFixed|MlFixed)\s*=\s*(?<Value>-?\d+(\.\d+)?|\.\d+)\s*(/\s*(?<Specie>\S*))?\s*(At\s*((?<Condition>Feed|Prod|Std)|(?<T>\d*(\.\d+)|\.\d+)(\s*,\s*(?<P>\d*(\.\d+)|\.\d+))?))?\s*$",
            RegexOptions.IgnoreCase | RegexOptions.ExplicitCapture | RegexOptions.Compiled);
        public static Regex s_CompoundRegex = new Regex(
            @"^\s*(?<Unparseable>.*?)(?<Amount>\d+/\d+|\d*(\.\d+)?)(?<Space>\s*)(?<Compound>[^\s+]*)\s*(?>(?<Unparseable>.*?)\+\s*(?<Amount>\d+/\d+|\d*(\.\d+)?)(?<Space>\s*)(?<Compound>[^\s+]*)\s*)*(?<Unparseable>.*?)$",
            RegexOptions.ExplicitCapture | RegexOptions.Compiled);
        #endregion Regex's

        #region Internal Variables
        protected ListViewItem m_LVI;
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

        protected Dictionary<Compound, Double> m_Reactants = new Dictionary<Compound, Double>();
        protected Dictionary<Compound, Double> m_Products = new Dictionary<Compound, Double>();
        protected List<Compound> m_OrderedReactants = new List<Compound>();
        protected List<Compound> m_OrderedProducts = new List<Compound>();

        protected Dictionary<Element, double> m_Unbalanced = new Dictionary<Element, double>();

        protected string m_sOriginalString;
        protected string m_sReactantString;
        protected string m_sProductString;
        protected string m_sComment;

        protected ILog m_Log;
        #endregion Internal Variables

        #region Constructors
        public SimpleReaction(ListViewItem _LVI, ILog log)
        {
            m_Log = log;
            m_Extent.Owner = this;
            m_Extent.Value = 1;
            LVI = _LVI;
            Changed += new EventHandler(UpdateStatus);
            Changed(this, new EventArgs());
        }
        #endregion Constructors

        #region Events
        public event EventHandler Changed;
        public event EventHandler ProductsChanged;
        public event EventHandler ReactantsChanged;
        #endregion Events

        #region Properties
        public ListViewItem LVI
        {
            get { return m_LVI; }
            set 
            {
                if (m_LVI == value) return;
                m_LVI = value;
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
                        return "<->";
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

        public Dictionary<Compound, Double> Reactants
        {
            get { return m_Reactants; }
            //set { m_Reactants = value; }
        }

        public Dictionary<Compound, Double> Products
        {
            get { return m_Products; }
            //set { m_Products = value; }
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
                Dictionary<Element, double> reactantAmounts = new Dictionary<Element,double>();
                Dictionary<Element, double> productAmounts = new Dictionary<Element,double>();
                foreach (KeyValuePair<Compound, double> kvpComps in m_Reactants)
                    foreach (KeyValuePair<Element, double> kvpElems in kvpComps.Key.Elements)
                    {
                        if (!reactantAmounts.ContainsKey(kvpElems.Key))
                            reactantAmounts.Add(kvpElems.Key, 0);
                        reactantAmounts[kvpElems.Key] += kvpComps.Value * kvpElems.Value;
                    }

                foreach (KeyValuePair<Compound, double> kvpComps in m_Products)
                    foreach (KeyValuePair<Element, double> kvpElems in kvpComps.Key.Elements)
                    {
                        if (!productAmounts.ContainsKey(kvpElems.Key))
                            productAmounts.Add(kvpElems.Key, 0);
                        productAmounts[kvpElems.Key] += kvpComps.Value * kvpElems.Value;
                    }

                foreach (KeyValuePair<Element, double> kvpReactants in reactantAmounts)
                    if (!productAmounts.ContainsKey(kvpReactants.Key))
                    {
                        ret = false;
                        m_Unbalanced[kvpReactants.Key] = -reactantAmounts[kvpReactants.Key];
                    }
                    else if (Math.Abs(kvpReactants.Value - productAmounts[kvpReactants.Key]) > sMinValue)
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

        public Dictionary<Element, Double> UnbalancedDetails
        {
            get { return m_Unbalanced; }
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

                if (Changed != null)
                    Changed(this, new EventArgs());
            }
        }

        public ILog Log
        {
            get { return m_Log; }
            set { m_Log = value; }
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
                List<Element> ret = new List<Element>();
                foreach (Compound c in Compounds)
                    foreach (Element e in c.Elements.Keys)
                        if (!ret.Contains(e))
                            ret.Add(e);
                return ret;
            }
        }

        public List<Compound> OrderedProducts { get { return m_OrderedProducts; } }
        public List<Compound> OrderedReactants { get { return m_OrderedReactants; } }
        #endregion Properties

        #region Public Functions
        public void AddReactant(Compound compound, Double fraction, int location)
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
            int i = 0;
            string tempReactantString = "";
            while (i < m_OrderedReactants.Count)
            {
                tempReactantString += m_Reactants[m_OrderedReactants[i]] + " ";
                if (location <= tempReactantString.Length)
                    break;
                tempReactantString += m_OrderedReactants[i] + " + ";
                i++;
            }
            m_nLastAddedNumStart = tempReactantString.Length;
            if (i < m_OrderedReactants.Count)
                m_nLastAddedNumStart -= 2;
            m_OrderedReactants.Insert(i, compound);
            m_Reactants.Add(compound, 1.0);

            FireChanged();
            if (ReactantsChanged != null)
                ReactantsChanged(this, new EventArgs());
        }

        public void AddProduct(Compound compound, Double fraction, int location)
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
            int i = 0;
            string tempProductString = "";
            while (i < m_OrderedProducts.Count)
            {
                tempProductString += m_Products[m_OrderedProducts[i]] + " ";
                if (location <= tempProductString.Length)
                    break;
                tempProductString += m_OrderedProducts[i] + " + ";
                i++;
            }
            m_nLastAddedNumStart = tempProductString.Length;
            if (i < m_OrderedProducts.Count)
                m_nLastAddedNumStart -= 2;
            m_OrderedProducts.Insert(i, compound);
            m_Products.Add(compound, 1.0);

            FireChanged();
            if (ProductsChanged != null)
                ProductsChanged(this, new EventArgs());
        }

        public string ToString(string numberFormat)
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
            return GetReactantsString(numberFormat) + arrow + GetProductsString(numberFormat);
        }

        public override string ToString()
        {
            return ToString("0.##");
        }

        public string ToSaveString(bool includeSequence)
        {
            StringBuilder sb = new StringBuilder();
            if (!m_bEnabled) sb.Append("; ");
            sb.AppendLine(this.ToString("0.######")); //Formula
            if (!m_bEnabled) sb.Append("; ");
            sb.AppendLine("Extent : " + m_Extent.ToString());
            if (CustomHeatOfReaction)
            {
                if (!m_bEnabled) sb.Append("; ");
                sb.Append(" HeatOfReaction : ");
                if (m_eHeatOfReactionType == FracTypes.ByMass)
                    sb.Append("MsFixed = ");
                else
                    sb.Append("MlFixed = ");
                sb.Append(m_dHeatOfReaction + " / " + m_HeatOfReactionSpecie);
                switch (m_eHeatOfReactionConditions)
                {
                    case TPConditions.Feed:
                        sb.AppendLine(" At Feed"); break;
                    case TPConditions.Product:
                        sb.AppendLine(" At Prod"); break;
                    case TPConditions.Standard:
                        sb.AppendLine(" At Std"); break;
                    case TPConditions.Custom:
                        sb.AppendLine(" At " + m_dHeatOfReactionT + ", " + m_dHeatOfReactionP); break;
                }
            }
            if (includeSequence)
            {
                if (!m_bEnabled) sb.Append("; ");
                sb.Append(" Sequence : " + m_nSequence);
            }
            return sb.ToString();
        }

        public string GetProductsString() { return GetProductsString("0.###"); }
        public string GetProductsString(string numberFormat)
        {
            if (!m_bProductsOK)
                return m_sProductString;
            string ret = "";
            foreach (Compound c in m_OrderedProducts)
            {
                ret += m_Products[c].ToString(numberFormat) + " " + c + " + ";
            }
            if (ret.Length > 3)
                ret = ret.Substring(0, ret.Length - 3); //Trim the trailing " + "
            return ret;
        }

        public string GetReactantsString() { return GetReactantsString("0.###"); }
        public string GetReactantsString(string numberFormat)
        {
            if (!m_bReactantsOK)
                return m_sReactantString;
            string ret = "";
            foreach (Compound c in m_OrderedReactants)
            {
                ret += m_Reactants[c].ToString(numberFormat) + " " + c + " + ";
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
                    Log.Message("Reactant Divider not found", MessageType.Error);
                    return;
                }

                string[] splitString = NewString.Split(new char[] { '-', '=' }, StringSplitOptions.None);
                if (splitString.Length != 2)
                {
                    Log.Message("More than one reactant divider found", MessageType.Error);
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

        public void ParseReactants(string reactantString)
        {
            if (reactantString == GetReactantsString())
                return;
            try
            {
                m_sReactantString = reactantString;
                m_bReactantsOK = false;
                m_Reactants.Clear(); m_OrderedReactants.Clear();
                m_Reactants = ReadCompounds(reactantString, out m_OrderedReactants, out m_bReactantsOK);
            }
            finally
            {
                try
                {
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
            if (productString == GetProductsString())
                return;
            try
            {
                m_sProductString = productString;
                m_bProductsOK = false;
                m_OrderedProducts.Clear(); m_Products.Clear();
                m_Products = ReadCompounds(productString, out m_OrderedProducts, out m_bProductsOK);
                //m_bProductsOK = true;
            }
            finally
            {
                try
                {
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
            ListViewItem lvi = new ListViewItem();
            lvi.SubItems.AddRange(new string[] { "", "", "" });
            return Clone(lvi);
        }

        public SimpleReaction Clone(ListViewItem newLVI)
        {
            SimpleReaction ret = (SimpleReaction) this.MemberwiseClone();
            ret.LVI = newLVI;
            //Items which must make their own clones:
            ret.m_Extent = m_Extent.Clone(ret);
            ret.m_OrderedProducts = new List<Compound>(m_OrderedProducts);
            ret.m_OrderedReactants = new List<Compound>(m_OrderedReactants);
            ret.m_Products = new Dictionary<Compound, double>(m_Products);
            ret.m_Reactants = new Dictionary<Compound, double>(m_Reactants);
            ret.m_Unbalanced = new Dictionary<Element, double>(m_Unbalanced);
            ret.Changed = null;
            ret.Changed += new EventHandler(ret.UpdateStatus);
            return ret;
        }

        public void DoDatabaseChanged()
        {
            //Extent:
            if (m_Extent.Specie != null && !Compound.CompoundList.ContainsKey(m_Extent.Specie.Symbol))
            {
                Log.Message("Extent specie not found in new database", MessageType.Error);
                m_Extent.Specie = null;
            }
            //HOR:
            if (CustomHeatOfReaction && !Compound.CompoundList.ContainsKey(m_HeatOfReactionSpecie.Symbol))
            {
                Log.Message("Heat of reaction specie not found in new database", MessageType.Error);
                m_HeatOfReactionSpecie = null;
                FireChanged();
            }
            //Spaces to force a reparse:
            ParseProducts(GetProductsString() + " "); 
            ParseReactants(GetReactantsString() + " ");
        }

        public RemovalInfo BalanceOptions()
        {
            //List<Compound> compounds = Compounds;

            Matrix m = GetBalanceMatrix();
            if (!m.IsSolveable())
                throw new InvalidOperationException("System is over-constrained, and not solveable.");
            RemovalInfo info = RecursiveRemoveableChecks(m);
            return info;
        }

        public void BalanceWith(int[] CompsToRemove)
        {
            Array.Sort<int>(CompsToRemove);
            for (int j = CompsToRemove.Length - 1; j >= 0; j--)
            {
                int i = CompsToRemove[j];
                int originalReactantCount = m_OrderedReactants.Count;
                if (i < originalReactantCount)
                {
                    m_Reactants.Remove(m_OrderedReactants[i]);
                    m_OrderedReactants.RemoveAt(i);
                }
                else
                {
                    m_Products.Remove(m_OrderedProducts[i - originalReactantCount]);
                    m_OrderedProducts.RemoveAt(i - originalReactantCount);
                }
            }
            Matrix m = GetBalanceMatrix();
            double[] vals = m.getVariableValues();

            for (int i = 0; i < m_OrderedReactants.Count - 1; i++)
                m_Reactants[m_OrderedReactants[i + 1]] = vals[i];
            for (int i = 0; i < m_OrderedProducts.Count; i++)
                m_Products[m_OrderedProducts[i]] = vals[i + m_OrderedReactants.Count - 1];

            Dictionary<Compound, double> originalReactants = new Dictionary<Compound, double>(m_Reactants);
            Dictionary<Compound, double> originalProducts = new Dictionary<Compound, double>(m_Products);
            foreach (KeyValuePair<Compound, double> kvp in originalReactants)
                if (kvp.Value < sMinValue)
                {
                    m_Reactants.Remove(kvp.Key);
                    m_OrderedReactants.Remove(kvp.Key);
                }
            foreach (KeyValuePair<Compound, double> kvp in originalProducts)
                if (kvp.Value < sMinValue)
                {
                    m_Products.Remove(kvp.Key);
                    m_OrderedProducts.Remove(kvp.Key);
                }

            FireChanged();
            if (ReactantsChanged != null)
                ReactantsChanged(this, new EventArgs());
            if (ProductsChanged != null)
                ProductsChanged(this, new EventArgs());
        }
        #endregion Public Functions

        #region Protected Functions
        protected Matrix GetBalanceMatrix()
        {
            List<Element> elements = Elements;
            Matrix m = new Matrix(elements.Count, m_OrderedProducts.Count + m_OrderedReactants.Count);
            for (int i = 0; i < elements.Count; i++)
            {
                for (int j = 1; j < m_OrderedReactants.Count; j++)
                    m[i, j - 1] = m_OrderedReactants[j].Elements.ContainsKey(elements[i]) ? -m_OrderedReactants[j].Elements[elements[i]] : 0;
                int c = m_OrderedReactants.Count - 1;
                for (int j = 0; j < m_OrderedProducts.Count; j++)
                    m[i, j + c] = m_OrderedProducts[j].Elements.ContainsKey(elements[i]) ? m_OrderedProducts[j].Elements[elements[i]] : 0;
                m[i, m.Columns - 1] = FirstReactant.Elements.ContainsKey(elements[i]) ? m_Reactants[FirstReactant] * FirstReactant.Elements[elements[i]] : 0;
            }
            return m;
        }

        protected RemovalInfo RecursiveRemoveableChecks(Matrix m)
        {
            if (m.Rank == m.Columns - 1)
            {
                double[] vals = m.getVariableValues();
                bool AllPositive = true;
                for (int j = 0; j < vals.Length; j++)
                    if (vals[j] < 0)
                    {
                        AllPositive = false;
                        break;
                    }
                if (AllPositive)
                    return new RemovalInfo(true);
                else
                    return new RemovalInfo(false);
            }
            else
            {
                bool[] removeables = m.ColumnsRemoveable();
                RemovalInfo ret = new RemovalInfo(m.Columns - 1);
                for (int i = 0; i < removeables.Length; i++)
                    if (!removeables[i])
                        ret.m_IfRemoved[i] = new RemovalInfo(false);
                    else
                    {
                        Matrix OneColRemoved = m.RemoveColumn(i);
                        ret.SetIfRemoved(i, RecursiveRemoveableChecks(OneColRemoved));
                    }
                return ret;
            }
        }

        protected void UpdateStatus(object sender, EventArgs e)
        {
            SetStatus();

            if (m_LVI == null) return;
            m_LVI.Text = this.ToString();
            m_LVI.SubItems[1].Text = m_Extent.ToString();
            m_LVI.SubItems[2].Text = CustomHeatOfReaction ? HeatOfReactionValue + (HeatOfReactionType == FracTypes.ByMole ? "kJ/mol " : "kJ/kg") + HeatOfReactionSpecie : "";
            m_LVI.SubItems[3].Text = m_nSequence.ToString();
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

        protected Dictionary<Compound, Double> ReadCompounds(string halfReaction, out List<Compound> Order, out bool Success)
        {
            Success = true;
            string[] Compounds = halfReaction.Split('+');
            for (int i = 0; i < Compounds.Length; i++)
                Compounds[i] = Compounds[i].Trim();
            Dictionary<Compound, Double> ret = new Dictionary<Compound, Double>();
            Order = new List<Compound>();
            Match m = s_CompoundRegex.Match(halfReaction);
            if (!m.Success)
            {
                Log.Message("Unable to parse compound string '" + halfReaction + "'", MessageType.Error);
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
                    double val = 1.0;
                    if (!string.IsNullOrEmpty(grpAmount.Captures[i].Value) && !string.IsNullOrEmpty(grpSpace.Captures[i].Value))
                        val = ParseFrac(grpAmount.Captures[i].Value);
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
                            val = ParseFrac(grpAmount.Captures[i].Value);
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
                    Log.Message(ex.Message, MessageType.Error);
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
                    Log.Message("Unable to parse compound '" + UnparseableComps[0] + "'", MessageType.Warning);
                else
                {
                    startString += UnparseableComps[0];
                    for (int i = 1; i < UnparseableComps.Count; i++)
                        startString += "', '" + UnparseableComps[i];
                    startString += "'";
                    Log.Message(startString, MessageType.Warning);
                }
            }
            if (UnparseableTokens.Count > 0)
            {
                string startString = "Unkown tokens: '";
                if (UnparseableTokens.Count == 1)
                    Log.Message("Unknown token '" + UnparseableTokens[0] + "'", MessageType.Warning);
                else
                {
                    startString += UnparseableTokens[0];
                    for (int i = 1; i < UnparseableTokens.Count; i++)
                        startString += "', '" + UnparseableTokens[i];
                    startString += "'";
                    Log.Message(startString, MessageType.Warning);
                }
            }
            return ret;
        }

        protected double ParseFrac(string s)
        {
            if (s.Contains("/"))
            {
                string[] subs = s.Split('/');
                return double.Parse(subs[0]) / double.Parse(subs[1]);
            }
            else
                return double.Parse(s);
        }
        
        protected void SetStatus()
        {
            if (m_bUseOriginalString || !m_bProductsOK || !m_bReactantsOK || !m_Extent.IsValid()
                || m_Products.Count == 0 || m_Reactants.Count == 0)
                m_eStatus = RxnStatuses.Invalid;
            else
                m_eStatus = Balanced ? RxnStatuses.OK : RxnStatuses.Imbalanced;
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

        #endregion Subclasses
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
        double[][] m_Data;
        int m_nRows, m_nColumns;
        bool m_bReduced = true;
        #endregion Internal Variables

        #region Properties
        public double this[int row, int col]
        {
            get { return m_Data[row][col]; }
            set
            {
                m_Data[row][col] = value;
                m_bReduced = false;
            }
        }

        public int Rows { get { return m_nRows; } }//m_Data.Length; } }
        public int Columns { get { return m_nColumns; } }//m_Data[0].Length; } }

        /// <summary>
        /// Doesn't work if the matrix is not row-reduced.
        /// </summary>
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
        #endregion Properties

        #region Constructors
        public Matrix(int rows, int columns)
        {
            m_nRows = rows;
            m_nColumns = columns;
            m_Data = new double[rows][];
            for (int i = 0; i < rows; i++)
                m_Data[i] = new double[columns];
        }
        #endregion Constructors

        #region Public Functions
        public void RowReduce()
        {
            if (m_bReduced)
                return;
            int k;
            for (k = 0; k < m_nRows - 1; k++)
            {
                Array.Sort<double[]>(m_Data, new Comparison<double[]>(CompareRows));
                int firstNZElem = FirstNZElem(m_Data[k]);
                if (firstNZElem == -1)
                    break;
                double val = m_Data[k][firstNZElem];
                for (int i = k + 1; i < m_Data.Length; i++)
                {
                    double fac = m_Data[i][firstNZElem] / val;
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
                double val = m_Data[k][firstNZElem];
                for (int i = k - 1; i >= 0; i--)
                {
                    double fac = m_Data[i][firstNZElem] / val;
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
            //  v1 v2 v3 v4 v5
            //  #  0  #  0  0  #1
            //  0  #  #  0  0  #2
            //  0  0  0  #  0  #3
            //  0  0  0  0  #  #4

            /* Musings on linkages:
             * if #1 == 0,              setting v1 = 0 REQUIRES v3 = 0
             * if #1 == 0 && #2 == 0,   setting v1 = 0 REQUIRES v2 = v3 = 0
             *                          If we have an independant block, with all results zero, this should not be in our reaction.
             */
            m_bReduced = true;
        }

        public bool IsSolveable()
        {
            RowReduce();
            for (int i = 0; i < Rows; i++)
                if (FirstNZElem(m_Data[i]) == Columns - 1)
                    return false;
            return true;
        }

        public double[] getVariableValues()
        {
            RowReduce();
            double[] ret = new double[Columns - 1];
            for (int i = 0; i < Columns - 1; i++)
                ret[i] = 0;
            int rank = Rank;
            for (int i = rank - 1; i >= 0; i--)
            {
                double t = m_Data[i][Columns - 1];
                for (int j = Columns - 2; j > i; j--)
                    t -= ret[j] * m_Data[i][j];
                ret[i] = t / m_Data[i][i];
            }
            return ret;
        }

        /// <summary>
        /// Calculates which sets of variables are linked.
        /// </summary>
        public LinDependanceInfo[] GetVariableLinkages()
        {
            RowReduce();

            LinDependanceInfo[] ret = new LinDependanceInfo[Columns - 1];
            for (int i = 0; i < Columns - 1; i++)
                ret[i].Index = i;

            for (int i = 0; i < Rows; i++)
            {
                int fnze = FirstNZElem(m_Data[i]);
                if (fnze < 0 || fnze == Columns - 1)
                    break;
                ret[fnze].RowFirst = i;
                for (int j = fnze + 1; j < Columns - j; i++)
                    if (m_Data[i][j] != 0)
                        ret[fnze].LinkTo(ret[j]);
                if (m_Data[i][Columns - 1] != 0)
                    ret[fnze].SetLinkedToPrimary();
            }
            return ret;
        }

        public Matrix Clone()
        {
            Matrix ret = new Matrix(this.Rows, this.Columns);
            for (int i = 0; i < Rows; i++)
                Array.Copy(m_Data[i], ret.m_Data[i], Columns);
            return ret;
        }

        public bool[] ColumnsRemoveable()
        {
            bool[] removeable = new bool[Columns - 1];
            for (int i = 0; i < Columns - 1; i++)
                removeable[i] = true;
            for (int k = 0; k < Columns - 1; k++)
            {
                for (int i = 0; i < Rows; i++)
                {
                    if (m_Data[i][Columns - 1] == 0)    //Ignore rows where the sum is to zero.
                        continue;
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
            double[][] newData = new double[Rows][];
            for (int i = 0; i < Rows; i++)
            {
                newData[i] = new double[Columns - 1];
                Array.Copy(m_Data[i], 0, newData[i], 0, col);
                Array.Copy(m_Data[i], col + 1, newData[i], col, Columns - col - 1);
            }
            Matrix ret = (Matrix) this.MemberwiseClone();

            ret.m_Data = newData;
            ret.m_bReduced = false;
            ret.m_nColumns--;
            return ret;
        }
        #endregion Public Functions

        #region Protected Functions
        protected int CompareRows(double[] row1, double[] row2)
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

        protected int FirstNZElem(double[] row)
        {
            for (int i = 0; i < row.Length; i++)
                if (row[i] != 0)
                    return i;
            return -1;
        }
        #endregion Protected Functions

        #region Subclasses
        public class LinDependanceInfo
        {
            protected List<LinDependanceInfo> m_Linkages = new List<LinDependanceInfo>();
            protected bool m_bLinkedToPrimary = false;

            public bool LinkedToPrimary { get { return m_bLinkedToPrimary; } }
            public int Index = -1;
            public int RowFirst = -1;
            public ReadOnlyCollection<LinDependanceInfo> Linkages { get { return m_Linkages.AsReadOnly(); } }

            public int DegreesOfFreedom
            {
                get
                {
                    int ret = m_Linkages.Count;
                    foreach (LinDependanceInfo inf in m_Linkages)
                        if (inf.RowFirst != -1)
                            ret--;
                    return ret;
                }
            }

            public int[] LinkageIndices
            {
                get
                {
                    int[] ret = new int[m_Linkages.Count];
                    for (int i = 0; i < m_Linkages.Count; i++)
                        ret[i] = m_Linkages[i].Index;
                    return ret;
                }
            }

            public void LinkTo(LinDependanceInfo otherVar)
            {
                if (m_Linkages.Contains(otherVar) || otherVar == this) //This statement stops an infinite loop occurring
                    return;
                m_Linkages.Add(otherVar);
                foreach (LinDependanceInfo another in otherVar.m_Linkages)
                    LinkTo(another);
                if (otherVar.m_bLinkedToPrimary)
                    m_bLinkedToPrimary = true;
                otherVar.LinkTo(this);
            }

            public void SetLinkedToPrimary()
            {
                m_bLinkedToPrimary = true;
            }
        }

        public class LinDependanceInfo2
        {
            protected List<List<LinDependanceInfo2>> m_DirectLinks = new List<List<LinDependanceInfo2>>();
            protected bool m_bConstrained;

            public void AddLinks(List<LinDependanceInfo2> others)
            {
                m_DirectLinks.Add(others);
            }

            public bool Constrained
            {
                set { m_bConstrained = value; }
                get
                {
                    bool bNoFreedom = false;
                    return m_bConstrained || bNoFreedom;
                }
            }
        }
        #endregion Subclasses
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