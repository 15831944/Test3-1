using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using Mehroz;

namespace Reaction_Editor
{
    public partial class FractionalUpDown : TextBox
    {
        public FractionalUpDown()
        {
        }

        protected Fraction m_Max = 100, m_Min = 0;
        protected Fraction m_OldValue = Fraction.NaN;

        #region properties
        public Fraction Value
        {
            get 
            {
                try { return new Fraction(Text); }
                catch
                {
                    return 0;
                }
            }
            set
            {
                if (value > m_Max) value = m_Max;
                if (value < m_Min) value = m_Min;
                if (m_OldValue == value && Text == value.ToString())
                    return;
                m_OldValue = value;
                Text = value.ToString();
                FireValueChanged();
            }
        }

        public Fraction Maximum
        {
            get { return m_Max; }
            set
            {
                m_Max = value;
                if (m_Max < Value)
                    Value = m_Max;
                if (m_Max < m_Min)
                    m_Min = m_Max;
            }
        }

        public Fraction Minimum
        {
            get { return m_Min; }
            set
            {
                m_Min = value;
                if (m_Min > Value)
                    Value = m_Min;
                if (m_Min > m_Max)
                    m_Max = m_Min;
            }
        }
        #endregion Properties

        #region Events
        public event EventHandler ValueChanged;
        #endregion Events

        protected override void OnKeyPress(KeyPressEventArgs e)
        {
            //Accept digits:
            if ((e.KeyChar >= '0' && e.KeyChar <= '9') || char.IsControl(e.KeyChar));
            //Accept '/':
            else if (e.KeyChar == '/')
            {
                if (Text.Contains("/"))
                {
                    e.Handled = true;
                    int i = Text.IndexOf('/');
                    Select(i + 1, Text.Length - i - 1);
                }
            }
            else
                e.Handled = true;

            base.OnKeyPress(e);
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.Up:
                    Value += 1;
                    break;
                case Keys.Down:
                    Value -= 1;
                    break;
                default:
                    base.OnKeyDown(e);
                    break;
            }
        }

        protected override void WndProc(ref Message m)
        {
            base.WndProc(ref m);
        }

        protected override void OnLeave(EventArgs e)
        {
            Value = Value; //You've got to love c# that this code is actually logical.
        }

        protected void FireValueChanged()
        {
            if (ValueChanged != null)
                ValueChanged(this, new EventArgs());
        }
    }
}
