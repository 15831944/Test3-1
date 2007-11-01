using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Reaction_Editor
{
    public partial class FrmBalanceOptions : Form
    {
        protected Matrix.RemovalInfo baseInfo;
        
        protected List<CheckBox> m_Checkboxes = new List<CheckBox>();

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

        public FrmBalanceOptions(Matrix.RemovalInfo info, SimpleReaction rxn)
        {
            baseInfo = info;
            int chkSpacing = 23;
            int extraFormSpace = 170 - 23;

            int chkX = 6, chkY = 19;

            InitializeComponent();
            if (info.DegreesOfFreedom > 1)
                lblInfo.Text = "The selected reaction is under-constrained, and as such requires that some compounds be removed from the reaction to allow autbalancing. Please select " + info.DegreesOfFreedom + " compounds to remove.";
            else
                lblInfo.Text = "The selected reaction is under-constrained, and as such requires that a compound be removed from the reaction to allow autbalancing. Please select which compound to remove.";

            List<Compound> products = rxn.OrderedProducts;
            List<Compound> reactants = rxn.OrderedReactants;
            int longestLength = products.Count > reactants.Count - 1 ? products.Count : reactants.Count - 1;

            this.Height = extraFormSpace + longestLength * chkSpacing;

            for (int i = 0; i < reactants.Count - 1; i++)
            {
                CheckBox chk = new CheckBox();
                grpReactants.Controls.Add(chk);
                chk.AutoSize = true;
                chk.UseVisualStyleBackColor = true;
                chk.Name = "chkOption" + i;
                chk.Text = reactants[i + 1].Symbol;
                chk.Enabled = info.m_IfRemoved[i].m_bCanRemove;
                chk.Location = new Point(chkX, chkY + i * chkSpacing);
                chk.TabIndex = i;
                chk.CheckedChanged += new EventHandler(chk_CheckedChanged);
                m_Checkboxes.Add(chk);
            }
            for (int i = 0; i < products.Count; i++)
            {
                CheckBox chk = new CheckBox();
                grpProducts.Controls.Add(chk);
                chk.AutoSize = true;
                chk.UseVisualStyleBackColor = true;
                chk.Name = "chkOption" + (i + reactants.Count - 1);
                chk.Text = products[i].Symbol;
                chk.Enabled = info.m_IfRemoved[i + reactants.Count - 1].m_bCanRemove;
                chk.Location = new Point(chkX, chkY + i * chkSpacing);
                chk.TabIndex = i + reactants.Count - 1;
                chk.CheckedChanged += new EventHandler(chk_CheckedChanged);
                m_Checkboxes.Add(chk);
            }
        }

        private void chk_CheckedChanged(object sender, EventArgs e)
        {
            grpProducts.SuspendLayout();
            grpReactants.SuspendLayout();
            foreach (CheckBox chk in m_Checkboxes)
                chk.Enabled = true;
            RecursivelyDisableCheckboxes(baseInfo, new List<int>());
            grpProducts.ResumeLayout();
            grpReactants.ResumeLayout();
        }

        protected void RecursivelyDisableCheckboxes(Matrix.RemovalInfo info, List<int> alreadyRemoved)
        {
            if (info.m_IfRemoved == null) //If IfRemoved is null, we're on the lowest level - everything not checked should be disabled
            {
                for (int i = 0; i < m_Checkboxes.Count; i++)
                    m_Checkboxes[i].Enabled = m_Checkboxes[i].Checked;
                btnOK.Enabled = true;
            }
            else
            {
                int removedCount = 0;
                for (int i = 0; i < m_Checkboxes.Count; i++)
                {
                    if (alreadyRemoved.Contains(i))
                        removedCount++;
                    else if (m_Checkboxes[i].Checked)
                    {
                        alreadyRemoved.Add(i);
                        RecursivelyDisableCheckboxes(info.m_IfRemoved[i - removedCount], alreadyRemoved);
                        return;
                    }
                }
                //If we exit the loop, this is the one we're using:
                removedCount = 0;
                for (int i = 0; i < m_Checkboxes.Count; i++)
                {
                    if (alreadyRemoved.Contains(i))
                        removedCount++;
                    else
                        m_Checkboxes[i].Enabled = info.m_IfRemoved[i - removedCount].m_bCanRemove;
                }
                btnOK.Enabled = false;
            }
            
        }
    }
}