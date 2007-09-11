using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Mehroz;

namespace Reaction_Editor
{
    public partial class FrmBalanceOptions2 : Form
    {
        protected Matrix m_OriginalMatrix;

        protected bool m_bDoOnValueChanged = false;
        
        protected List<CheckBox> m_Checkboxes = new List<CheckBox>();
        protected List<FractionalUpDown> m_NumericUDs = new List<FractionalUpDown>();

        protected Fraction[] m_MinValues, m_MaxValues;

        int chkSpacing = 23;
        int extraFormSpace = 170 - 23;

        int chkX = 6, chkY = 19;
        int numX = 151, numY = 14;

        public int[] ToBeRemoved
        {
            get
            {
                List<int> ret = new List<int>();
                for (int i = 0; i < m_Checkboxes.Count; i++)
                    if (m_Checkboxes[i].Checked)
                        ret.Add(i + 1); //Because we don't display compound 0 in this list.
                return ret.ToArray();
            }
        }

        public Fraction[] Coefficients
        {
            get
            {
                Fraction[] ret = new Fraction[m_NumericUDs.Count];
                for (int i = 0; i < ret.Length; i++)
                    ret[i] = (Fraction)m_NumericUDs[i].Value;
                return ret;
            }
        }

        //Based on the logic that the minimum and maximum value allowed of any variable will be when those variables that can be removed are set to zero.
        protected void CalculateAbsoluteMinMaxValues(Matrix m, List<int> alreadyRemoved)
        {
            if (m.Rank == m.Columns - 1)
            {
                if (m.GetRemovalInfo().m_bCanRemove)
                {
                    Fraction[] vals = m.getVariableValues();
                    int j = 0;
                    for (int i = 0; i < m_MaxValues.Length; i++)
                        if (alreadyRemoved.Contains(i))
                        {
                            if (m_MinValues[i] > 0)
                                m_MinValues[i] = 0;
                            if (m_MaxValues[i] < 0)
                                m_MaxValues[i] = 0;
                        }
                        else
                        {
                            if (vals[j] > m_MaxValues[i])
                                m_MaxValues[i] = vals[j];
                            if (vals[j] < m_MinValues[i])
                                m_MinValues[i] = vals[j];
                            j++;
                        }
                }
            }
            else
            {
                bool[] removeables = m.ColumnsRemoveable();
                int j = 0;
                for (int i = 0; i < m_MaxValues.Length; i++)
                    if (alreadyRemoved.Contains(i))
                        continue;
                    else
                    {
                        if (removeables[j])
                        {
                            List<int> subList = new List<int>(alreadyRemoved);
                            subList.Add(i);
                            CalculateAbsoluteMinMaxValues(m.RemoveColumn(j), subList);
                        }
                        j++;
                    }
            }
        }

        public FrmBalanceOptions2(SimpleReaction rxn, List<Compound> extraComps)
        {
            m_OriginalMatrix = rxn.GetBalanceMatrix(extraComps);
            m_OriginalMatrix.RowReduce();

            m_MinValues = new Fraction[m_OriginalMatrix.Columns - 1];
            m_MaxValues = new Fraction[m_OriginalMatrix.Columns - 1];
            for (int i = 0; i < m_MinValues.Length; i++)
            {
                m_MinValues[i] = Fraction.MaxValue;
                if (m_OriginalMatrix.CanGoNegative(i))
                    m_MaxValues[i] = Fraction.MinValue;
                else
                    m_MaxValues[i] = 0;
            }
            CalculateAbsoluteMinMaxValues(m_OriginalMatrix, new List<int>());

            InitializeComponent();

            int c = m_OriginalMatrix.Columns - 1 - m_OriginalMatrix.Rank;
            bool[] changeables = m_OriginalMatrix.ColumnsRemoveable();

            if (c > 1)
                lblInfo.Text = "The selected reaction is under-constrained, and as such requires that some compounds have specified coefficients to allow autbalancing. Please specify " + c + " values to balance.";
            else
                lblInfo.Text = "The selected reaction is under-constrained, and as such requires that a compound have a specified coefficient to allow autbalancing. Please specify a value.";

            List<Compound> products = rxn.OrderedProducts;
            List<Compound> reactants = rxn.OrderedReactants;
            int longestLength = products.Count > reactants.Count - 1 ? products.Count : reactants.Count - 1;
            if (extraComps.Count > longestLength)
                longestLength = extraComps.Count;

            this.Height = extraFormSpace + longestLength * chkSpacing;

            PopulateGroupBox(grpReactants, reactants, 0, changeables, 1);
            PopulateGroupBox(grpProducts, products, reactants.Count - 1, changeables, 0);

            if (extraComps.Count != 0)
            {
                lblNote.Text = "Note that a negative value in the third group indicates the compound will be a reactant, while a positive value indicates it will be a product.";
                int extraWidth = grpProducts.Right - grpReactants.Right;
                this.Width += extraWidth;
                GroupBox grpExtras = new GroupBox();
                grpExtras.Text = "Autobalance Added Compounds";
                this.Controls.Add(grpExtras);
                grpExtras.Top = grpProducts.Top;
                grpExtras.Left = grpProducts.Left + extraWidth;
                grpExtras.Width = grpProducts.Width;
                grpExtras.Height = grpProducts.Height;
                PopulateGroupBox(grpExtras, extraComps, reactants.Count - 1 + products.Count, changeables, 0);
            }

            SetInitialValues();

            m_bDoOnValueChanged = true;
        }

        private void PopulateGroupBox(GroupBox box, List<Compound> comps, int offset, bool[] changeables, int start)
        {
            for (int i = 0; i < comps.Count - start; i++)
            {
                CheckBox chk = new CheckBox();
                box.Controls.Add(chk);
                chk.AutoSize = true;
                chk.UseVisualStyleBackColor = true;
                chk.Name = "chkOption" + (i + offset);
                chk.Text = comps[i + start].Symbol;
                chk.Enabled = changeables[i + offset];
                chk.Location = new Point(chkX, chkY + i * chkSpacing);
                chk.TabIndex = 2 * i;
                chk.CheckedChanged += new EventHandler(chk_CheckedChanged);
                if (m_MinValues[i + offset] == 0)
                    chk.Font = new Font(chk.Font, FontStyle.Bold);
                m_Checkboxes.Add(chk);

                FractionalUpDown num = new FractionalUpDown();
                box.Controls.Add(num);
                num.Name = "numOption" + (i + offset);
                num.Enabled = false;
                num.Location = new Point(numX, numY + i * chkSpacing);
                num.Size = new Size(59, 20);
                num.Minimum = Fraction.MinValue;
                num.Maximum = Fraction.MaxValue;
                num.TabIndex = chk.TabIndex + 1;
                num.ValueChanged += new EventHandler(num_ValueChanged);
                num.GotFocus += new EventHandler(num_GotFocus);
                m_NumericUDs.Add(num);

                chk.Tag = chk.Text; num.Tag = chk;
            }
        }

        private void num_GotFocus(object sender, EventArgs e)
        {
            SetMinMaxValues();
        }

        private void num_ValueChanged(object s, EventArgs e)
        {
            if (!m_bDoOnValueChanged)
                return;
            //m_bDoOnValueChanged = false;

            SetMinMaxValues();

            Matrix.Vector finalCol = m_OriginalMatrix.GetColumn(m_OriginalMatrix.Columns - 1);

            for (int i = 0; i < m_Checkboxes.Count; i++)
            {
                if (m_Checkboxes[i].Checked)
                    finalCol -= m_OriginalMatrix.GetColumn(i) * (Fraction)m_NumericUDs[i].Value;
            }

            Matrix currentMatrix = RemoveColumns();

            currentMatrix.SetColumn(currentMatrix.Columns - 1, finalCol);

            Fraction[] vals = currentMatrix.getVariableValues();
            int j = 0;
            //Set values:
            for (int i = 0; i < m_Checkboxes.Count; i++)
                if (m_Checkboxes[i].Checked)
                    continue;
                else
                    m_NumericUDs[i].Value = vals[j++];
            //m_bDoOnValueChanged = true;
        }

        protected void SetMinMaxValues()
        {
            Matrix.Vector finalCol = m_OriginalMatrix.GetColumn(m_OriginalMatrix.Columns - 1);

            for (int i = 0; i < m_Checkboxes.Count; i++)
            {
                if (m_Checkboxes[i].Checked)
                    finalCol -= m_OriginalMatrix.GetColumn(i) * (Fraction)m_NumericUDs[i].Value;
            }

            Matrix currentMatrix = RemoveColumns(); //May be inefficient with particularly large matrices (Since this uses quite a bit of memory management)
            for (int i = 0; i < m_Checkboxes.Count; i++)
            {
                if (m_Checkboxes[i].Checked)
                {
                    Matrix.Vector currentCol = m_OriginalMatrix.GetColumn(i);
                    Matrix scratch = currentMatrix.InsertColumn(currentMatrix.Columns, currentCol);
                    scratch.SetColumn(currentMatrix.Columns - 1, finalCol + currentCol * (Fraction)m_NumericUDs[i].Value);
                    scratch.RowReduce();
                    Fraction min = 0;
                    Fraction max = Fraction.MaxValue;
                    if (!m_NumericUDs[i].Focused)
                        for (int k = 0; k < scratch.Rank; k++)
                        {
                            if (scratch[k, scratch.Columns - 1] == 0)
                                continue;
                            Fraction val = scratch[k, scratch.Columns - 2] / scratch[k, scratch.Columns - 1];
                            //Console.WriteLine(scratch[k, scratch.Columns - 1]);
                            if (scratch[k, k] * scratch[k, scratch.Columns - 1] < 0)
                            {
                                if (val > min)
                                    min = val;
                            }
                            else
                                if (val < max)
                                    max = val;
                        }
                    else
                    {
                        min = m_MinValues[i];
                        max = m_MaxValues[i];
                    }
                    m_NumericUDs[i].Minimum = min;
                    m_NumericUDs[i].Maximum = max;

                    m_Checkboxes[i].Text = m_Checkboxes[i].Tag + " [" + min + ", " + (max == Fraction.MaxValue ? "\u221E)" : max + "]");
                }
                else
                {
                    m_NumericUDs[i].Minimum = Fraction.MinValue;
                    m_NumericUDs[i].Maximum = Fraction.MaxValue;
                    m_Checkboxes[i].Text = m_Checkboxes[i].Tag.ToString();
                }
            }
        }

        private void chk_CheckedChanged(object sender, EventArgs e)
        {
            grpProducts.SuspendLayout();
            grpReactants.SuspendLayout();
            SetMinMaxValues();
            Matrix m = RemoveColumns();
            RemoveColumns();
            bool[] changeables = m.ColumnsRemoveable();
            int j = 0;
            for (int i = 0; i < m_Checkboxes.Count; i++)
            {
                if (m_Checkboxes[i].Checked)
                    m_Checkboxes[i].Enabled = true;
                else
                    m_Checkboxes[i].Enabled = changeables[j++];
            }
            if (m.Rank == m.Columns - 1) //We have specified enough checkboxes...
            {
                for (int i = 0; i < m_Checkboxes.Count; i++)
                {
                    m_NumericUDs[i].Enabled = m_Checkboxes[i].Checked;
                }
                btnOK.Enabled = true;
            }
            else
            {
                for (int i = 0; i < m_Checkboxes.Count; i++)
                    m_NumericUDs[i].Enabled = false;
                btnOK.Enabled = false;
            }
            grpProducts.ResumeLayout();
            grpReactants.ResumeLayout();
        }

        protected Matrix RemoveColumns()
        {
            Matrix m = m_OriginalMatrix;
            for (int i = m_Checkboxes.Count - 1; i >= 0; i--)
            {
                if (m_Checkboxes[i].Checked)
                {
                    m = m.RemoveColumn(i);
                }
            }
            return m;
        }

        protected void SetInitialValues()
        {
            Matrix.RemovalInfo info = m_OriginalMatrix.GetRemovalInfo();
            Matrix.RemovalInfo info2 = info;
            Matrix m = m_OriginalMatrix.Clone();
            int rem = 0;
            for (int i = 0; i < m_OriginalMatrix.Columns - 1; i++)
            {
                if (info2.DegreesOfFreedom > 0 && info2.m_IfRemoved[i - rem].m_bCanRemove)
                {
                    m = m.RemoveColumn(i);
                    info2 = info2.m_IfRemoved[i - rem];
                    rem++;
                }
            }
            info2 = info; rem = 0;
            Fraction[] vals = m.getVariableValues();
            for (int i = 0; i < m_OriginalMatrix.Columns - 1; i++)
            {
                if (info2.DegreesOfFreedom > 0 && info2.m_IfRemoved[i - rem].m_bCanRemove)
                {
                    m_NumericUDs[i].Value = 0;
                    info2 = info2.m_IfRemoved[i - rem];
                    rem++;
                }
                else
                    m_NumericUDs[i].Value = vals[i - rem];
            }
        }
    }
}