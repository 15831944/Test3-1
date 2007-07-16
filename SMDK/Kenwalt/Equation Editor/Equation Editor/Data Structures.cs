using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Windows.Forms;
using System.Drawing;
using System.Text.RegularExpressions;
using System.Text;

namespace Reaction_Editor
{
    public class RxnEdException : Exception
    {
        public RxnEdException(string message) : base(message) { }
    }

    public enum Phases { Solid, Liquid, Gas, All };
    public enum RxnDirections { Forward, Equilibrium, Backward };
    public enum RxnStatuses { Ok, Imbalanced, Invalid };
    public enum ExtentTypes { Fraction, Ratio, Equilibrium, FinalConc, FinalFrac, Rate }
    public enum HXTypes { None, FinalT, ApproachT, ApproachAmbient, Power, Electrolysis };
    public enum FracTypes { ByMass, ByMole };

    #region Reaction Extent Structures
    public class RxnExtent
    {
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
        public double Value
        {
            get { return m_dValue; }
            set 
            {
                m_dValue = value;
                if (m_Owner != null)
                    m_Owner.FireChanged();
            }
        }
        public Compound Specie
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
            if (s.StartsWith("Extent :"))
                s = s.Remove(0, "Extent :".Length).Trim();
            if (s.ToLowerInvariant().StartsWith("fraction"))
            {
                if (s.Contains(" Rate "))
                    retType = ExtentTypes.Rate;
                else
                    retType = ExtentTypes.Fraction;
                return FractionExtent.Parse(s);
            }
            if (s.ToLowerInvariant().StartsWith("equilibrium"))
            {
                retType = ExtentTypes.Equilibrium;
                return EquilibriumExtent.Parse(s);
            }
            if (s.ToLowerInvariant().StartsWith("mlfinalfraction") ||
                s.ToLowerInvariant().StartsWith("msfinalfraction"))
            {
                retType = ExtentTypes.FinalFrac;
                return Final_FracExtent.Parse(s);
            }
            if (s.ToLowerInvariant().StartsWith("finalconc"))
            {
                retType = ExtentTypes.FinalConc;
                return Final_ConcExtent.Parse(s);
            }
            if (s.ToLowerInvariant().StartsWith("ratio"))
            {
                retType = ExtentTypes.Ratio;
                return RatioExtent.Parse(s);
            }
            throw new RxnEdException("Unsupported extent token");
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
        public FractionExtent(RxnExtent original) 
            : base(original) { }
        public FractionExtent() : base() { }
        public override string ToString()
        {
            return "Fraction " + Specie + " = " + Value;
        }

        public static RxnExtent Parse(string s)
        {
            ExtentTypes bleh;
            if (!s.ToLowerInvariant().StartsWith("fraction"))
                return RxnExtent.Parse(s, out bleh);
            if (s.Contains(" Rate "))
                return RateExtent.Parse(s);
            FractionExtent ret = new FractionExtent();
            s = s.Remove(0, "fraction".Length).Trim();
            string[] subs = s.Split(new string[] { "=" }, StringSplitOptions.RemoveEmptyEntries);
            ret.Specie = Compound.FromString(subs[0].Trim());
            ret.Value = double.Parse(subs[1].Trim());
            return ret;
        }
    }

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
            ret.Specie = Compound.FromString(subPri[0].Trim());
            ret.Specie2 = Compound.FromString(subPri[1].Trim());
            double r3 = double.Parse(subPri[2].Trim());
            double r4 = double.Parse(subPri[3].Trim());
            ret.Value = r4 / r3;
            return ret;
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
        protected double m_T = double.NaN;
        public Final_ConcExtent(RxnExtent original)
            : base(original) { }
        public Final_ConcExtent() : base() { }
        public double T
        {
            get { return m_T; }
            set
            {
                m_T = value;
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
            if (!s.ToLowerInvariant().StartsWith("finalconc"))
                throw new Exception("Invalid string passed to Final_ConcExtent.Parse");
            Final_ConcExtent ret = new Final_ConcExtent();
            s = s.Remove(0, "FinalConc".Length);
            string[] subs;
            if (s.ToLowerInvariant().Contains(" at "))
            {
                if (s.Contains(" at"))
                    subs = s.Split(new string[] { " at " }, StringSplitOptions.RemoveEmptyEntries);
                else if (s.Contains(" At "))
                    subs = s.Split(new string[] { " At " }, StringSplitOptions.RemoveEmptyEntries);
                else //(s.Contains(" AT "))
                    subs = s.Split(new string[] { " AT " }, StringSplitOptions.RemoveEmptyEntries);
                ret.T = double.Parse(subs[1].Trim());
            }
            else
                subs = new string[] { s };
            string[] subs2 = subs[0].Split(new char[] { '=' }, StringSplitOptions.RemoveEmptyEntries);
            ret.Specie = Compound.FromString(subs2[0].Trim());
            ret.Value = double.Parse(subs2[1].Trim());
            return ret;
        }
    }

    public class Final_FracExtent : RxnExtent
    {
        protected Phases m_Phase = (Phases)(-1);
        protected FracTypes m_eFracType;
        public Final_FracExtent(RxnExtent original)
            : base(original) { }
        public Phases Phase 
        {
            get{ return m_Phase; }
            set 
            {
                m_Phase = value;
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
            string phaseString = "";
            if ((int)Phase != -1)
                phaseString = Phase == Phases.All ? "Total " : Phase.ToString() + " ";
            return "msFinalFraction " + phaseString + Specie + " = " + Value;
        }

        public static Final_FracExtent Parse(string s)
        {
            Final_FracExtent ret = new Final_FracExtent(null);
            if (s.ToLowerInvariant().StartsWith("msfinalfrac"))
                ret.FracType = FracTypes.ByMass;
            else if (s.ToLowerInvariant().StartsWith("mlfinalfrac"))
                ret.FracType = FracTypes.ByMole;
            else
                throw new Exception("Final_FracExtent.Parse called with invalid string");
            s = s.Remove(0, "mXfinalfrac".Length).Trim();
            if (s.ToLowerInvariant().StartsWith("total"))
            {
                ret.Phase = Phases.All;
                s = s.Remove(0, "total".Length).Trim();
            }
            else if (s.ToLowerInvariant().StartsWith("liq ") || s.ToLowerInvariant().StartsWith("liq\t"))
            {
                ret.Phase = Phases.Liquid;
                s = s.Remove(0, "Liq ".Length).Trim();
            }
            else if (s.ToLowerInvariant().StartsWith("sol ") || s.ToLowerInvariant().StartsWith("sol\t"))
            {
                ret.Phase = Phases.Solid;
                s = s.Remove(0, "Sol ".Length).Trim();
            }
            else if (s.ToLowerInvariant().StartsWith("gas ") || s.ToLowerInvariant().StartsWith("gas\t"))
            {
                ret.Phase = Phases.Gas;
                s = s.Remove(0, "Gas ".Length).Trim();
            }
            string[] subs = s.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
            ret.Specie = Compound.FromString(subs[0].Trim());
            ret.Value = double.Parse(subs[1].Trim());
            return ret;
        }
    }

    public class RateExtent : RxnExtent
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

        public static RateExtent Parse(string s)
        {
            if (!s.ToLowerInvariant().StartsWith("fraction") ||
                !s.ToLowerInvariant().Contains(" Rate "))
                throw new Exception("Invalid string passed to RateExtent.Parse");
            s = s.Remove(0, "fraction".Length).Trim();
            RateExtent ret = new RateExtent(null);
            if (s.ToLowerInvariant().EndsWith("stabilised"))
            {
                ret.Stabilised = true;
                s = s.Remove(s.Length - "stabilised".Length).Trim();
            }
            string[] subs = s.Split(new string[] { " Rate = " }, StringSplitOptions.RemoveEmptyEntries);
            ret.Specie = Compound.FromString(subs[0].Trim());
            ret.Value = double.Parse(subs[1].Trim());
            return ret;
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
        #endregion


        public static Compound FromString(string compString)
        {
            if (CompoundList.ContainsKey(compString))
                return CompoundList[compString];
            else
                throw new RxnEdException("Compound " + compString + " not found in database");
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
        public static Regex s_SequenceRegex = new Regex(@"^Sequence\s*:\s*(?<Value>\d+)$",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled);
        public static Regex s_HORRegex = new Regex(
            @"^HeatOfReaction\s*:\s*(?<Type>MsFixed|MlFixed)\s*=\s*(?<Value>\d+(\.\d+)?|\.\d+)\s*/\s*(?<Specie>\S*)\s*$",
            RegexOptions.IgnoreCase | RegexOptions.ExplicitCapture | RegexOptions.Compiled);
        public static Regex s_CompoundRegex = new Regex(
            @"^\s*(?<Unparseable>.*?)(?<Amount>\d*(\.\d+)?)\s*(?<Compound>[A-Z][A-Za-z0-9\[\]]*\([a-zA-z]+\))\s*(?>(?<Unparseable>.*?)\+\s*(?<Amount>\d*(\.\d+)?)\s*(?<Compound>[A-Z][A-Za-z0-9\[\]]*\([a-zA-z]+\))\s*)*(?<Unparseable>.*?)$",
            RegexOptions.ExplicitCapture | RegexOptions.Compiled);
        #region Internal Variables
        protected ListViewItem m_LVI;

        protected bool m_bEnabled = true;
        protected RxnStatuses m_eStatus;
        protected bool m_bProductsOK = true, m_bReactantsOK = true;
        protected bool m_bUseOriginalString;

        protected RxnDirections m_eDirection;
        protected ExtentTypes m_eExtentType;
        protected bool m_bCustomHeatOfReaction;

        protected RxnExtent m_Extent = new RxnExtent(null);
        protected double m_dHeatOfReaction = 0;
        protected Compound m_HeatOfReactionSpecie = null;
        protected FracTypes m_eHeatOfReactionType = FracTypes.ByMole;
        
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

                if (double.IsNaN(value))
                    m_bCustomHeatOfReaction = false;
                else
                    m_bCustomHeatOfReaction = true;
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
                m_eHeatOfReactionType = value;
                FireChanged();
            }
        }

        public static double BalanceAccuracy = 1E-2;
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
                    else if (Math.Abs(kvpReactants.Value - productAmounts[kvpReactants.Key]) > BalanceAccuracy)
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
                if (i < tempReactantString.Length)
                    break;
                tempReactantString += m_OrderedReactants[i] + " + ";
                i++;
            }
            m_OrderedReactants.Insert(i, compound);
            m_Reactants.Add(compound, 1.0);

            if (Changed != null)
                Changed(this, new EventArgs());
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
            m_OrderedProducts.Insert(i, compound);
            m_Products.Add(compound, 1.0);

            if (Changed != null)
                Changed(this, new EventArgs());
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

        public string ToSaveString(bool includeSequence)
        {
            StringBuilder sb = new StringBuilder();
            if (!m_bEnabled)
                sb.Append("; ");
            sb.Append(this.ToString()); //Formula
            sb.Append(" Extent : " + m_Extent.ToString());
            if (CustomHeatOfReaction)
            {
                sb.Append(" HeatOfReaction : ");
                if (m_eHeatOfReactionType == FracTypes.ByMass)
                    sb.Append("MsFixed = ");
                else
                    sb.Append("MlFixed = ");
                sb.Append(m_dHeatOfReaction + " / " + m_HeatOfReactionSpecie);
            }
            if (includeSequence)
                sb.Append(" Sequence : " + m_nSequence);
            return sb.ToString();
        }

        public string GetProductsString()
        {
            if (!m_bProductsOK)
                return m_sProductString;
            string ret = "";
            foreach (Compound c in m_OrderedProducts)
            {
                ret += m_Products[c] + " " + c + " + ";
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
                ret += m_Reactants[c] + " " + c + " + ";
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
            m_Extent = RxnExtent.Parse(extentString, out m_eExtentType);
            m_Extent.Owner = this;
            FireChanged();
        }

        public void ParseHOR(string HORString)
        {
            Match m = s_HORRegex.Match(HORString);
            //TODO: Throw an exception in the event of no match
            HeatOfReactionValue = double.Parse(m.Groups["Value"].Captures[0].Value);
            HeatOfReactionSpecie = Compound.FromString(m.Groups["Specie"].Captures[0].Value);
            if (m.Groups["Type"].Captures[0].Value == "MsFixed")
                HeatOfReactionType = FracTypes.ByMass;
            else
                HeatOfReactionType = FracTypes.ByMole;
        }

        public void FireChanged()
        {
            if (Changed != null)
                Changed(this, new EventArgs());
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
            ret.Changed += new EventHandler(ret.UpdateStatus);
            return ret;
        }
        #endregion Public Functions

        #region Protected Functions

        protected void UpdateStatus(object sender, EventArgs e)
        {
            SetStatus();

            if (m_LVI == null) return;
            m_LVI.Text = this.ToString();
            m_LVI.SubItems[1].Text = m_Extent.ToString();
            m_LVI.SubItems[2].Text = CustomHeatOfReaction ? HeatOfReactionValue + "kJ/mol " + HeatOfReactionSpecie : "Default";
            if (m_bEnabled)
                switch (m_eStatus)
                {
                    case RxnStatuses.Imbalanced:
                        m_LVI.ForeColor = Color.Orange;
                        break;
                    case RxnStatuses.Invalid:
                        m_LVI.ForeColor = Color.Red;
                        break;
                    case RxnStatuses.Ok:
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
            List<string> UnparseableComps = new List<string>();
            List<string> UnparseableTokens = new List<string>();
            for (int i = 0; i < grpAmount.Captures.Count; i++)
            {
                try
                {
                    double val = 1.0;
                    if (!string.IsNullOrEmpty(grpAmount.Captures[i].Value))
                        val = double.Parse(grpAmount.Captures[i].Value);
                    Compound comp = Compound.FromString(grpCompound.Captures[i].Value);
                    if (ret.ContainsKey(comp))
                        ret[comp] += val;
                    else
                    {
                        ret.Add(comp, val);
                        Order.Add(comp);
                    }
                }
                catch
                {
                    UnparseableComps.Add(grpCompound.Captures[i].Value);
                    Success = false;
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
                    foreach (string s in UnparseableComps)
                        startString += "', '" + s;
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
                    foreach (string s in UnparseableTokens)
                        startString += "', '" + s;
                    startString += "'";
                    Log.Message(startString, MessageType.Warning);
                }
            }
            return ret;
            /*foreach (string s in Compounds)
            {
                s.Trim();
                string numString = "";
                //For now: Suport 1.0 and 1,0
                int i;
                for (i = 0; i < s.Length && (char.IsDigit(s[i]) || s[i] == '.' || s[i] == ','); i++) ;
                if (i == s.Length)
                {
                    Log.Message("Expected Compound not found", MessageType.Error);
                    Success = false;
                }
                numString = s.Substring(0, i);
                double amount;
                if (numString.Length == 0)
                    amount = 1.0;
                else if (!double.TryParse(numString, out amount))
                {
                    Success = false;
                    Log.Message("Unable to parse compound amount", MessageType.Error);
                }
                Double frac = amount;// new Double(amount);
                string compString = s.Substring(i, s.Length - i).Trim();
                Compound comp = Compound.FromString(compString);
                if (ret.ContainsKey(comp))
                    ret[comp] += frac;
                else
                {
                    ret[comp] = frac;
                    Order.Add(comp);
                }
            }*/


        }
        
        protected void SetStatus()
        {
            if (m_bUseOriginalString || !m_bProductsOK || !m_bReactantsOK || !m_Extent.IsValid()
                || m_Products.Count == 0 || m_Reactants.Count == 0)
                m_eStatus = RxnStatuses.Invalid;
            else
                m_eStatus = Balanced ? RxnStatuses.Ok : RxnStatuses.Imbalanced;
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

        #endregion
    }
    #endregion Log

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
}