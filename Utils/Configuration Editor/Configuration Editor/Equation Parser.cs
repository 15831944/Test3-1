using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using System;

namespace Configuration_Editor
{
    public abstract class FunctionValue
    {
        protected string m_sName;

        public abstract int ParamCount { get; }
        public virtual string Name { get { return m_sName; } set { m_sName = value; } }
        public abstract bool CanEvaluate(Dictionary<String, Double> extraVals);
        public abstract double Evaluate(double[] paramValue, Dictionary<string, double> extraVals);
    }

    public class EquationFunctionValue : FunctionValue
    {
        string[] m_paramOrders;
        double[] m_defaults;
        EquationFragment m_Frag;

        protected static Regex prototypeRegex = new Regex(@"(?<Name>[^(]*)\((?<VarName>[^,)]+)?(,(?<VarName>[^,)]+))*\)", RegexOptions.Compiled);

        public override int ParamCount { get { return m_paramOrders.Length - m_defaults.Length; } }

        public override bool CanEvaluate(Dictionary<string, double> extraVals)
        {
            if (extraVals != null)
                foreach (KeyValuePair<string, double> kvp in extraVals)
                    m_Frag.VariableValues[kvp.Key] = kvp.Value;
            foreach (string s in m_paramOrders)
                m_Frag.VariableValues[s] = 0;
            return m_Frag.CanEvaluate();
        }

        public EquationFunctionValue(string prototype, string definition, double[] defaults)
        {
            Match m = prototypeRegex.Match(prototype);
            m_paramOrders = new string[m.Groups["VarName"].Captures.Count];
            for (int i = 0; i < m.Groups["VarName"].Captures.Count; i++)
            {
                m_paramOrders[i] = m.Groups["VarName"].Captures[i].Value;
            }
            m_Frag = EquationFragment.Parse(definition, null);
            m_defaults = defaults;
            m_sName = m.Groups["Name"].Value;
        }

        public double Evaluate(double[] paramValues) { return Evaluate(paramValues, null); }
        public override double Evaluate(double[] paramValues, Dictionary<string, double> extraVals)
        {
            if (paramValues.Length > m_paramOrders.Length)
                throw new Exception("More parameters passed to function than it can handle");

            if (extraVals != null)
                foreach (KeyValuePair<string, double> kvp in extraVals)
                    m_Frag.VariableValues[kvp.Key] = kvp.Value;

            for (int i = 0; i < paramValues.Length; i++)
                m_Frag.VariableValues[m_paramOrders[i]] = paramValues[i];

            for (int i = m_paramOrders.Length - 1; i >= paramValues.Length; i--)
                m_Frag.VariableValues[m_paramOrders[i]] = m_defaults[m_paramOrders.Length - 1 - i];

            return m_Frag.Value();
        }
    }

    public class EquationFragment
    {
        public static Dictionary<string, FunctionValue> GlobalFunctions = new Dictionary<string, FunctionValue>();

        const string OpeningBracket = "(";
        const string ClosingBracket = ")";
        const string VariableOpeningBracket = "[";
        const string VariableClosingBracket = "]";

        //Variable Constraints: A variable must be enclosed in square brackets, and cannot contain whitespace.
        //Any opening square brackets inside the variable name must be closed (i.e. [...]...[...] would
        //not produce '...]...[...' as a valid variable name.

        //Function Constraints: Func(a,b,c,d,...).
        //But, we strip brackets out of the equation first... So it is instead going to be
        //Func\25\
        protected static Regex FunctionRegex = new Regex(@"(?<Name>[A-Za-z]\w*)\\(?<Number>\d+)\\", RegexOptions.Compiled);
        //Values      numbers (We need to exclude sub-EquationFragments).
        protected static Regex LiteralRegex = new Regex(@"[A-Za-z]\w*|(?<=[^\\\d]|^)(\d+(\.\d+)?|\.\d+)(E(\+|-)?\d{1,2})?", RegexOptions.Compiled | RegexOptions.IgnoreCase);

        protected static Regex RebuildRegex = new Regex(@"\\(?<Fragment>\d+)\\", RegexOptions.Compiled);

        //protected static Regex BinaryOpRegex = new Regex(@"\\(?<f1>\d+)\\\s*(?<Op>\+|-|/|\*|\^)\s*\\(?<f2>\d+)\\", RegexOptions.Compiled);
        protected static Regex PowRegex = new Regex(@"\\(?<f1>\d+)\\\s*(?<Op>\^)\s*\\(?<f2>\d+)\\", RegexOptions.Compiled);
        protected static Regex DivRegex = new Regex(@"\\(?<f1>\d+)\\\s*(?<Op>/)\s*\\(?<f2>\d+)\\", RegexOptions.Compiled);
        protected static Regex MultRegex = new Regex(@"\\(?<f1>\d+)\\\s*(?<Op>\*)\s*\\(?<f2>\d+)\\", RegexOptions.Compiled);
        protected static Regex SubRegex = new Regex(@"\\(?<f1>\d+)\\\s*(?<Op>-)\s*\\(?<f2>\d+)\\", RegexOptions.Compiled);
        protected static Regex SumRegex = new Regex(@"\\(?<f1>\d+)\\\s*(?<Op>\+)\s*\\(?<f2>\d+)\\", RegexOptions.Compiled);

        protected static Regex UnaryOpRegex = new Regex(@"^\s*(?<Op>\+|-)\s*\\(?<f>\d+)\\", RegexOptions.Compiled);

        protected static Regex AcceptableRegex = new Regex(@"^\s*\\\d+\\\s*$", RegexOptions.Compiled);

        protected static Regex[] OrderedOperations;

        #region variables
        protected string ParsedString;
        public List<EquationFragment> m_Fragments = new List<EquationFragment>();
        protected EquationFragment m_Parent;
        protected List<EquationFragment> m_subFragments = new List<EquationFragment>();
        protected Dictionary<string, double> m_VariableValues = new Dictionary<string, double>();
        protected Dictionary<string, FunctionValue> m_FunctionValues = new Dictionary<string, FunctionValue>();
        #endregion variables

        #region Properties
        public Dictionary<string, double> VariableValues
        {
            get
            {
                if (this.m_Parent == null)
                    return m_VariableValues;
                else
                    return m_Parent.VariableValues;
            }
            set
            {
                if (this.m_Parent == null)
                    m_VariableValues = value;
                else
                    m_Parent.VariableValues = value;
            }
        }

        public string[] ReqVariableNames
        {
            get
            {
                List<string> ret = new List<string>();
                foreach (EquationFragment frag in Fragments)
                    if (frag is ValueFragment && !((ValueFragment)frag).IsConst && !ret.Contains(frag.ToString()))
                        ret.Add(frag.ToString());
                return ret.ToArray();
            }
        }
        #endregion Properties

        #region Helper Properties
        protected List<EquationFragment> Fragments
        {
            get
            {
                if (m_Parent == null)
                    return m_Fragments;
                else
                    return m_Parent.Fragments;
            }
        }

        protected Dictionary<string, FunctionValue> FunctionValues
        {
            get
            {
                if (m_Parent == null)
                    return m_FunctionValues;
                else
                    return m_Parent.FunctionValues;
            }
        }
        #endregion Helper Properties

        #region Constructors
        public static EquationFragment Parse(string s, Dictionary<string, FunctionValue> functions)
        {
            if (s.Contains("\\")) throw new Exception("The \\ character is reserved.");
            return new EquationFragment(s, functions);
        }

        protected EquationFragment(string s, Dictionary<string, FunctionValue> functions)
        {
            m_FunctionValues = functions;
            InternalParse(s);
        }

        protected EquationFragment(EquationFragment parent) { this.m_Parent = parent; }

        protected EquationFragment(string s, EquationFragment parent)
            : this(parent)
        {
            InternalParse(s);
        }

        private void InternalParse(string s)
        {
            if (OrderedOperations == null)
                OrderedOperations = new Regex[] { PowRegex, DivRegex, MultRegex, SubRegex, SumRegex };

            //Since we can have Values with names like H2SO4(aq), H[+](aq),
            //and various other things that use mathematical symbols, we are
            //going to first strip out variable names:
            Dictionary<int, int> varLocVarLen = SegmentLocations(s, VariableOpeningBracket, VariableClosingBracket);
            List<int> variableOpeningBrackets = FindLocations(s, VariableOpeningBracket);

            //int variableNumber = 0;
            for (int i = variableOpeningBrackets.Count - 1; i >= 0; i--)
            {
                if (varLocVarLen.ContainsKey(variableOpeningBrackets[i]))
                {
                    Fragments.Add(new ValueFragment(s.Substring(variableOpeningBrackets[i], varLocVarLen[variableOpeningBrackets[i]]), this)); //+1 and -2 because we don't want to include the brackets.
                    s = s.Remove(variableOpeningBrackets[i], varLocVarLen[variableOpeningBrackets[i]]);
                    s = s.Insert(variableOpeningBrackets[i], "\\" + (Fragments.Count - 1) + "\\ ");
                }
            }

            if (s.Contains(VariableOpeningBracket) || s.Contains(VariableClosingBracket))
                throw new Exception("A nonmatching variable bracket was detected");

            //Ok, now we can search for brackets:
            Dictionary<int, int> bracLocLen = SegmentLocations(s, OpeningBracket, ClosingBracket);
            List<int> openingBrackets = FindLocations(s, OpeningBracket);

            //int fragmentNumber = 0;
            for (int i = openingBrackets.Count - 1; i >= 0; i--)
            {
                if (bracLocLen.ContainsKey(openingBrackets[i]))
                {
                    string subs = s.Substring(openingBrackets[i] + 1, bracLocLen[openingBrackets[i]] - 2).Trim(); //+1 and -2 because we don't want to include the brackets.
                    if (!string.IsNullOrEmpty(subs))
                        Fragments.Add(new EquationFragment(subs, this));
                    else
                        Fragments.Add(new EmptyEquationFragment());
                    s = s.Remove(openingBrackets[i], bracLocLen[openingBrackets[i]]);
                    s = s.Insert(openingBrackets[i], "\\" + (Fragments.Count - 1) + "\\ ");
                }
            }

            if (s.Contains(OpeningBracket) || s.Contains(ClosingBracket))
                throw new Exception("A nonmatching bracket was found");

            //Strip functions. Although functions are very similar to equation fragments...
            s = FunctionRegex.Replace(s, (MatchEvaluator)delegate(Match m)
            {
                Fragments.Add(new Function(m.Groups["Name"].Value, Fragments[int.Parse(m.Groups["Number"].Value)], this));
                return "\\" + (Fragments.Count - 1) + "\\ ";
            }); //Replace logic...

            //Now, all we have left is a series of values, equation fragments, values, and literals.
            //Replace all the literals:
            s = LiteralRegex.Replace(s, (MatchEvaluator)delegate(Match m)
            {
                Fragments.Add(new ValueFragment(m.Value, this));
                return "\\" + (Fragments.Count - 1) + "\\ ";
            });


            //To consider: An equation fragment could consist of a parameter list:
            string[] subStrs = s.Split(',');
            if (subStrs.Length > 1)
            {
                foreach (string st in subStrs)
                    m_subFragments.Add(new EquationFragment(st, this));
            }
            else //If it's not a list of fragments, we want to parse operands...
            {
                Match m2 = UnaryOpRegex.Match(s);
                while (m2.Success)
                {
                    Fragments.Add(new UnaryOperand(
                        m2.Groups["Op"].Value,
                        Fragments[int.Parse(m2.Groups["f"].Value)],
                        this));
                    s = s.Remove(m2.Index, m2.Length);
                    s = s.Insert(m2.Index, "\\" + (Fragments.Count - 1) + "\\");
                    m2 = UnaryOpRegex.Match(s);
                }

                //Order of operations:
                foreach (Regex r in OrderedOperations)
                {
                    Match m = r.Match(s);
                    while (m.Success)
                    {
                        Fragments.Add(new BinaryOperand(
                            m.Groups["Op"].Value,
                            Fragments[int.Parse(m.Groups["f1"].Value)],
                            Fragments[int.Parse(m.Groups["f2"].Value)], this));
                        s = s.Remove(m.Index, m.Length);
                        s = s.Insert(m.Index, "\\" + (Fragments.Count - 1) + "\\");
                        m = r.Match(s);
                    }
                }

                ParsedString = s.Trim();
                if (!AcceptableRegex.Match(ParsedString).Success)
                    throw new Exception("Input was not reduced to a single equation fragment.");
                if (Fragments[int.Parse(RebuildRegex.Match(ParsedString).Groups["Fragment"].Value)] is EmptyEquationFragment)
                    throw new EmptyEquationFragmentException();

                //m_subFragments.Add(this);
            }

        }
        #endregion Constructors

        //const string bracketString = @"(([^\[\]](?<Opening>\[))*([^\[\]]*((?<Closing-Opening>\])|(?<ErronousClosing>\])))*)*";
        public static Regex VariableBracketsRegex = new Regex(@"(([^\[\]]*(?<Opening>\[))*([^\[\]]*((?<Closing-Opening>\])|(?<ErronousClosing>\])))*)*", RegexOptions.Compiled);
        public static Regex BracketsRegex = new Regex(@"(([^()]*(?<Opening>\())*([^()]*((?<Closing-Opening>\))|(?<ErronousClosing>\))))*)*", RegexOptions.Compiled);
        //public static Regex FunctionRegex = new Regex(@"(?<Name>[A-Za-z]\w*)\((?<Param>[^,]*)(,(?<Param>[^,]*))*", RegexOptions.Compiled);

        #region public functions
        public override string ToString()
        {
            return ToString(true);
        }

        public virtual string ToString(bool IncludeBrackets)
        {
            string ret;
            if (ParsedString == null && m_subFragments == null)
                return base.ToString();
            else if (m_subFragments == null || m_subFragments.Count == 0)
                ret = RebuildRegex.Replace(ParsedString, (MatchEvaluator)delegate(Match m)
                {
                    return Fragments[int.Parse(m.Groups["Fragment"].Value)].ToString();
                });
            else
            {
                StringBuilder sb = new StringBuilder();
                for (int i = 0; i < m_subFragments.Count; i++)
                    sb.Append(m_subFragments[i].ToString(false) + (i == m_subFragments.Count - 1 ? "" : ","));
                ret = sb.ToString();
            }
            if (IncludeBrackets)
                return "(" + ret + ")";
            else
                return ret;
        }

        public virtual double Value()
        {
            return Fragments[int.Parse(RebuildRegex.Match(ParsedString).Groups["Fragment"].Value)].Value();
        }

        public double Value(int index) //Used for functions.
        {
            return m_subFragments[index].Value();
        }

        public double[] Values()
        {
            if (m_subFragments.Count == 0 && ParsedString != null)
                return new double[] { Value() };
            double[] ret = new double[m_subFragments.Count];
            for (int i = 0; i < ret.Length; i++)
                ret[i] = m_subFragments[i].Value();
            return ret;
        }

        public virtual bool CanEvaluate()
        {
            if (m_Fragments == null)
                return true;

            foreach (EquationFragment frag in m_Fragments)
                if (!frag.CanEvaluate())
                    return false;
            return true;
        }
        #endregion public functions

        #region Protected Functions
        //Bugger this, we're gonna identify 'em by their start and length, not end and length.
        protected Dictionary<int, int> SegmentLocations(string s, string openingC, string closingC)
        {
            List<int> BracketLocations = new List<int>();
            BracketLocations.AddRange(FindLocations(s, openingC));
            BracketLocations.AddRange(FindLocations(s, closingC));

            BracketLocations.Sort();

            Dictionary<int, int> varLocVarLen = new Dictionary<int, int>();

            Stack<int> openingLocations = new Stack<int>();
            for (int i = 0; i < BracketLocations.Count; i++)
            {
                if (s[BracketLocations[i]] == openingC[0])
                    openingLocations.Push(BracketLocations[i]);
                else
                {
                    if (openingLocations.Count == 1) //If we have only one opening location on the stack, we want to add this as a variable name.
                    {
                        int openingLoc = openingLocations.Pop();
                        varLocVarLen.Add(openingLoc, BracketLocations[i] - openingLoc + 1);
                    }
                    else if (openingLocations.Count > 0) //It's just a sub bracket within the name of another bracket
                        openingLocations.Pop();
                }
            }
            return varLocVarLen;
        }

        protected List<int> FindLocations(string s, string c)
        {
            List<int> ret = new List<int>();
            int start = 0;
            while (start > -1)
            {
                start = s.IndexOf(c, start);
                if (start > -1)
                    ret.Add(start++);
            }
            return ret;
        }
        #endregion protected functions.

        #region Subclasses
        public abstract class SingleParamFunctionValue : FunctionValue
        {
            public override int ParamCount
            {
                get { return 1; }
            }

            public override bool CanEvaluate(Dictionary<string, double> extraVals)
            {
                return true;
            }
        }

        public class Exp : SingleParamFunctionValue
        {
            public override string Name
            {
                get { return "Exp"; }
                set { }
            }

            public override double Evaluate(double[] paramValue, Dictionary<string, double> extraVals)
            {
                if (paramValue.Length != 1)
                    throw new ArgumentException("Exp Accepts only one parameter", "paramValue");
                return Math.Exp(paramValue[0]);
            }
        }

        public class Log10 : SingleParamFunctionValue
        {
            public override string Name
            {
                get { return "Log10"; }
                set { }
            }

            public override double Evaluate(double[] paramValue, Dictionary<string, double> extraVals)
            {
                if (paramValue.Length != 1)
                    throw new ArgumentException("Log10 Accepts only one parameter", "paramValue");
                return Math.Log10(paramValue[0]);
            }
        }

        public static void Setup()
        {
            GlobalFunctions.Add("Exp", new Exp());
            GlobalFunctions.Add("Log10", new Log10());
        }
        #endregion Subclasses
    }

    public class EmptyEquationFragment : EquationFragment
    {
        public EmptyEquationFragment() : base(null) { }
    }

    public class EmptyEquationFragmentException : Exception
    { }

    public class Function : EquationFragment
    {
        #region variables
        protected string m_sFunctionName;
        protected EquationFragment m_Parameters;
        protected FunctionValue m_FunctionValue;
        #endregion variables

        public Function(string name, EquationFragment parameters, EquationFragment parent)
            : base(parent)
        {
            m_sFunctionName = name;
            m_Parameters = parameters;
            if (FunctionValues != null)
            {
                if (FunctionValues.ContainsKey(m_sFunctionName))
                    m_FunctionValue = FunctionValues[m_sFunctionName];
                else if (EquationFragment.GlobalFunctions.ContainsKey(m_sFunctionName))
                    m_FunctionValue = EquationFragment.GlobalFunctions[m_sFunctionName];
                else
                    throw new FunctionNotFoundException("Function '" + name + "' was not found", name);
            }
            else if (EquationFragment.GlobalFunctions.ContainsKey(m_sFunctionName))
                m_FunctionValue = EquationFragment.GlobalFunctions[m_sFunctionName];

            if (m_FunctionValue != null && m_FunctionValue.ParamCount != 0 && parameters is EmptyEquationFragment)
                throw new EmptyEquationFragmentException();
        }

        public override string ToString()
        {
            return m_sFunctionName + m_Parameters.ToString(true);
        }

        public override string ToString(bool IncludeBrackets)
        {
            if (IncludeBrackets)
                return "(" + ToString() + ")";
            else
                return ToString();
        }

        public override double Value()
        {
            if (m_FunctionValue.ParamCount > 0)
                return m_FunctionValue.Evaluate(m_Parameters.Values(), VariableValues);
            else
                return m_FunctionValue.Evaluate(new double[] { }, VariableValues);
        }

        public override bool CanEvaluate()
        {
            return m_FunctionValue != null && m_FunctionValue.CanEvaluate(VariableValues);
        }

        #region Subclasses
        public class FunctionNotFoundException : Exception
        {
            protected string m_sFunctionName;
            public string FunctionName { get { return m_sFunctionName; } }
            public FunctionNotFoundException(string Message, string FunctionName)
                : base(Message)
            {
                m_sFunctionName = FunctionName;
            }
        }
        #endregion Subclasses
    }

    public class BinaryOperand : EquationFragment
    {
        protected string m_sOp;
        protected EquationFragment m_Frag1, m_Frag2;
        public BinaryOperand(string op, EquationFragment frag1, EquationFragment frag2, EquationFragment parent)
            : base(parent)
        {
            m_sOp = op;
            if (m_Frag1 is EmptyEquationFragment || m_Frag2 is EmptyEquationFragment)
                throw new EmptyEquationFragmentException();
            m_Frag1 = frag1;
            m_Frag2 = frag2;
        }

        public override string ToString()
        {
            return ToString(false);
        }

        public override string ToString(bool IncludeBrackets)
        {
            string ret = m_Frag1 + m_sOp + m_Frag2;
            if (IncludeBrackets)
                return "(" + ret + ")";
            else
                return ret;
        }

        public override bool CanEvaluate()
        {
            return true;
        }

        //We could add an extra level of indirection with a dictionary and things, but we want it to still be somewhat efficient.
        public override double Value()
        {
            if (m_sOp == "+")
                return m_Frag1.Value() + m_Frag2.Value();
            else if (m_sOp == "-")
                return m_Frag1.Value() - m_Frag2.Value();
            else if (m_sOp == "*")
                return m_Frag1.Value() * m_Frag2.Value();
            else if (m_sOp == "/")
                return m_Frag1.Value() / m_Frag2.Value();
            else if (m_sOp == "^")
                return Math.Pow(m_Frag1.Value(), m_Frag2.Value());
            return double.NaN;
        }
    }

    public class UnaryOperand : EquationFragment
    {
        protected string m_sOp;
        protected EquationFragment m_Frag1;
        public UnaryOperand(string op, EquationFragment frag1, EquationFragment parent)
            : base(parent)
        {
            m_sOp = op;
            if (m_Frag1 is EmptyEquationFragment)
                throw new EmptyEquationFragmentException();
            m_Frag1 = frag1;
        }

        public override string ToString()
        {
            return ToString(false);
        }

        public override string ToString(bool IncludeBrackets)
        {
            string ret = m_sOp + m_Frag1;
            if (IncludeBrackets)
                return "(" + ret + ")";
            else
                return ret;
        }

        public override bool CanEvaluate()
        {
            return true;
        }

        //We could add an extra level of indirection with a dictionary and things, but we want it to still be somewhat efficient.
        public override double Value()
        {
            if (m_sOp == "-")
                return -m_Frag1.Value();

            return double.NaN;
        }
    }

    public class ValueFragment : EquationFragment
    {
        #region variables
        protected string m_sValueName;
        protected double m_dConstValue;
        protected bool m_bIsConst;
        #endregion variables

        public ValueFragment(string val, EquationFragment parent)
            : base(parent)
        {
            m_sValueName = val;
            m_bIsConst = double.TryParse(val, out m_dConstValue);
        }

        public override bool CanEvaluate()
        {
            return m_bIsConst || VariableValues.ContainsKey(m_sValueName);
        }

        public override string ToString()
        {
            return ToString(false);
        }

        public bool IsConst { get { return m_bIsConst; } }

        public override string ToString(bool IncludeBrackets)
        {
            if (IncludeBrackets)
                return "(" + m_sValueName + ")";
            else
                return m_sValueName;
        }

        public override double Value()
        {
            if (m_bIsConst)
                return m_dConstValue;
            else
                return VariableValues[m_sValueName];
        }
    }
}
