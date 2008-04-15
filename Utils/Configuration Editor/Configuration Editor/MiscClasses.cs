
using System.Data;
using System.Windows.Forms;
using System;
namespace Configuration_Editor
{
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
        public override bool Equals(object obj)
        {
            if (!(obj is Pair<T>))
                return false;
            Pair<T> other = obj as Pair<T>;
            return other.s1.Equals(s1) && other.s2.Equals(s2);
        }

        public override int GetHashCode()
        {
            return unchecked(s1.GetHashCode() * s2.GetHashCode());
        }
    }

    public class ConvertEventArgsEx : ConvertEventArgs
    {
        public ConvertEventArgsEx(object value, Type desiredType, bool includeOffset)
            : base(value, desiredType)
        {
            m_IncludeOffset = includeOffset;
        }
        protected bool m_IncludeOffset;
        public bool IncludeOffset
        {
            get { return m_IncludeOffset; }
            set { m_IncludeOffset = value; }
        }
    }

    /// <summary>
    /// I guess you could call it "Unit", since it's effectively an engineering unit.
    /// </summary>
    public class Conversion
    {
        public Conversion(double offset, double factor, string name)
        {
            this.offset = offset;
            this.factor = factor;
            this.name = name;
        }

        public double offset;
        public double factor;
        public string name;

        public string Name { get { return name; } set { name = value; } }

        public double Parse(double formattedVal, bool includeOfset)
        {
            return formattedVal / factor + (includeOfset ? offset : 0);
        }

        public double Format(double SIVal, bool includeOfset)
        {
            return (SIVal - (includeOfset ? offset : 0)) * factor;
        }
    }

    /// <summary>
    /// Provides a formatter and a parser for whatever the currently selected unit on the BindingSource is
    /// </summary>
    public class FormatterParserProvider
    {
        protected BindingSource m_ConversionBindings;

        protected void Parse(object sender, ConvertEventArgs e)
        {
            try
            {
                bool IncludeOffset = !(e is ConvertEventArgsEx) || ((ConvertEventArgsEx)e).IncludeOffset;
                double d = ((Conversion)m_ConversionBindings.Current).Parse(System.Convert.ToDouble(e.Value), IncludeOffset);
                if (e.DesiredType == typeof(double))
                    e.Value = d;
                else if (e.DesiredType == typeof(float))
                    e.Value = (float)d;
                else if (e.DesiredType == typeof(string))
                    e.Value = d.ToString();
            }
            catch { };
        }

        protected void Format(object sender, ConvertEventArgs e)
        {
            try
            {
                bool IncludeOffset = !(e is ConvertEventArgsEx) || ((ConvertEventArgsEx)e).IncludeOffset;
                double d = ((Conversion)m_ConversionBindings.Current).Format(System.Convert.ToDouble(e.Value), IncludeOffset);
                if (e.DesiredType == typeof(double))
                    e.Value = d;
                else if (e.DesiredType == typeof(float))
                    e.Value = (float)d;
                else if (e.DesiredType == typeof(string))
                    e.Value = d.ToString("F2");
            }
            catch { };
        }

        public FormatterParserProvider(BindingSource ConversionBindings)
        {
            if (ConversionBindings.DataSource != typeof(Conversion))
                throw new Exception("ConversionBindings must contain a list of Conversions");
            m_ConversionBindings = ConversionBindings;
            Parser = new ConvertEventHandler(Parse);
            Formatter = new ConvertEventHandler(Format);
        }

        public ConvertEventHandler Parser;
        public ConvertEventHandler Formatter;

        public string CurrentCnv { get { return ((Conversion)m_ConversionBindings.Current).Name; } }
    }
}