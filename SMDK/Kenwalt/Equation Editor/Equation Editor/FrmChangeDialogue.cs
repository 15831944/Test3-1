using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Text.RegularExpressions;

namespace Reaction_Editor
{
    public partial class FrmChangeDialogue : Form
    {
        public FrmChangeDialogue()
        {
            InitializeComponent();
        }

        #region Variables
        private Control LastControl = null;
        #endregion Variables

        #region constants
        const int sSeqLowTop = 97;
        const int sMaxGrpHeight = 128;
        const int sHeightPerBox = 26;
        const int sMaxFrmHeight = 378;
        const int sPadding = 6;
        const int sExtraFrmPadding = 32;
        const int sMaxWidth = 450;
        const int sMinWidth = 200;
        #endregion constants

        //Keep a static form which will be used...
        private static FrmChangeDialogue frm = new FrmChangeDialogue();

        public static DialogResult Show(SimpleReaction curReaction, SimpleReaction newReaction, string title, string message, string newReactionTitle, bool bSeq, bool bExtent)
        {
            FrmChangeDialogue frmCurrent = frm.Visible ? new FrmChangeDialogue() : frm;
            frmCurrent.SuspendLayout();
            frmCurrent.txtMessage.Text = message;
            frmCurrent.grpNewReaction.Text = newReactionTitle;
            frmCurrent.Text = title;

            frmCurrent.SetReactions(curReaction, newReaction, bSeq, bExtent);
            frmCurrent.ResumeLayout();
            return frmCurrent.ShowDialog();
        }

        public static DialogResult Show(Reaction curReaction, Reaction newReaction, string title, string message, string newReactionTitle)
        {
            FrmChangeDialogue frmCurrent = frm.Visible ? new FrmChangeDialogue() : frm;
            frmCurrent.SuspendLayout();
            frmCurrent.txtMessage.Text = message;
            frmCurrent.grpNewReaction.Text = newReactionTitle;
            frmCurrent.Text = title;

            if (curReaction is HXReaction)
                frmCurrent.SetReactions((HXReaction)curReaction, (HXReaction)newReaction);
            else if (curReaction is CompoundListReaction)
                frmCurrent.SetReactions((CompoundListReaction)curReaction, (CompoundListReaction)newReaction);
            frmCurrent.ResumeLayout();
            return frmCurrent.ShowDialog();
        }

        protected void SetReactions(HXReaction curReaction, HXReaction newReaction)
        {
            ChangeGeneric(true);
            Font BoldFont = new Font(txtCurReactants.Font, FontStyle.Bold);

            txtCurGeneric.Text = curReaction.ToString();
            txtNewGeneric.Text = newReaction.ToString();

            Match curMatch = HXReaction.s_HXRegex.Match(txtCurGeneric.Text);
            Match newMatch = HXReaction.s_HXRegex.Match(txtNewGeneric.Text);
            string[] groups = new string[] { "Type", "Value", "Value2" };
            foreach (string s in groups)
            {
                Group curGroup = curMatch.Groups[s];
                Group newGroup = newMatch.Groups[s];
                if (curGroup.Value != newGroup.Value)
                {
                    if (curGroup.Success)
                    {
                        txtCurGeneric.Select(curGroup.Index, curGroup.Length);
                        HighlightText(txtCurGeneric);
                    }
                    if (newGroup.Success)
                    {
                        txtNewGeneric.Select(newGroup.Index, newGroup.Length);
                        HighlightText(txtNewGeneric);
                    }
                }
            }
        }

        protected void SetReactions(CompoundListReaction curReaction, CompoundListReaction newReaction)
        {
            ChangeGeneric(true);
            Font BoldFont = new Font(txtCurReactants.Font, FontStyle.Bold);

            txtCurGeneric.Text = curReaction.ToString();
            txtNewGeneric.Text = newReaction.ToString();

            //Bold changes:
            //Only reasonable approach here is using the all powerful Regex...
            Match m = CompoundListReaction.s_SourceSinkRegex.Match(txtCurGeneric.Text);
            Match curMatch = Compound.s_CompoundSeperator.Match(
                m.Groups["Value"].Value);
            Match newMatch = Compound.s_CompoundSeperator.Match(
                CompoundListReaction.s_SourceSinkRegex.Match(txtNewGeneric.Text).Groups["Value"].Value);
            int offset = m.Groups["Value"].Index;
            foreach (Capture curComp in curMatch.Groups["Comp"].Captures)
            {
                bool contained = false;
                string s = curComp.Value.Trim();
                foreach (Capture newComp in newMatch.Groups["Comp"].Captures)
                    if (newComp.Value.Trim() == s)
                    {
                        contained = true;
                        break;
                    }
                if (!contained)
                {
                    txtCurGeneric.Select(curComp.Index + offset, curComp.Length);
                    HighlightText(txtCurGeneric);
                }
            }

            foreach (Capture newComp in newMatch.Groups["Comp"].Captures)
            {
                bool contained = false;
                string s = newComp.Value.Trim();
                foreach (Capture curComp in curMatch.Groups["Comp"].Captures)
                    if (curComp.Value.Trim() == s)
                    {
                        contained = true;
                        break;
                    }
                if (!contained)
                {
                    txtNewGeneric.Select(newComp.Index + offset, newComp.Length);
                    HighlightText(txtNewGeneric);
                }
            }
        }

        protected void SetReactions(SimpleReaction curReaction, SimpleReaction newReaction, bool bSeq, bool bExtent)
        {
            ChangeGeneric(false);
            Font BoldFont = new Font(txtCurReactants.Font, FontStyle.Bold);

            bool HOR = curReaction.CustomHeatOfReaction || newReaction.CustomHeatOfReaction;

            //Current Reaction:
            txtCurReactants.Text = curReaction.GetReactantsString();
            txtCurDirection.Text = curReaction.DirectionString;
            txtCurProducts.Text = curReaction.GetProductsString();
            txtCurExtent.Text = curReaction.ExtentInfo.ToString();
            if (curReaction.CustomHeatOfReaction)
                txtCurHOR.Text = curReaction.HeatOfReactionString;
            else
                txtCurHOR.Text = "HeatOfReaction: Default";

            if (bSeq)
                txtCurSeq.Text = "Sequence: " + curReaction.Sequence;
            
            //New Reaction:
            txtNewReactants.Text = newReaction.GetReactantsString();
            txtNewDirection.Text = newReaction.DirectionString;
            txtNewProducts.Text = newReaction.GetProductsString();
            txtNewExtent.Text = newReaction.ExtentInfo.ToString();
            if (newReaction.CustomHeatOfReaction)
                txtNewHOR.Text = newReaction.HeatOfReactionString;
            else
                txtNewHOR.Text = "HeatOfReaction: Default";
            if (bSeq)
                txtNewSeq.Text = "Sequence: " + newReaction.Sequence;

            //Customise the layout:
            txtCurHOR.Visible = txtNewHOR.Visible = HOR;
            txtCurSeq.Visible = txtNewSeq.Visible = bSeq;
            txtCurExtent.Visible = txtNewExtent.Visible = bExtent;

            txtCurSeq.Top = txtNewSeq.Top = HOR ? sSeqLowTop : txtCurHOR.Top;

            grpCurrentReaction.Height = grpNewReaction.Height = sMaxGrpHeight - ((HOR ? 0 : 1) + (bSeq ? 0 : 1) + (bExtent ? 0 : 1)) * sHeightPerBox;
            grpNewReaction.Top = grpCurrentReaction.Bottom + sPadding;
            this.MaximumSize = this.MinimumSize = new Size(this.Width, sMaxFrmHeight - 2 * ((HOR ? 0 : 1) + (bSeq ? 0 : 1) + (bExtent ? 0 : 1)) * sHeightPerBox);
            this.Height = sMaxFrmHeight - 2 * ((HOR ? 0 : 1) + (bSeq ? 0 : 1) + (bExtent ? 0 : 1)) * sHeightPerBox;
            
            //Width:
            Graphics gfx = txtCurReactants.CreateGraphics();
            float w1 = gfx.MeasureString(txtCurReactants.Text, BoldFont).Width;
            float w2 = gfx.MeasureString(txtCurProducts.Text, BoldFont).Width;
            float w3 = gfx.MeasureString(txtNewReactants.Text, BoldFont).Width;
            float w4 = gfx.MeasureString(txtNewProducts.Text, BoldFont).Width;

            int MaxW = (int)Math.Max(w1, Math.Max(w2, Math.Max(w3, w4)));
            if (MaxW > sMaxWidth) MaxW = sMaxWidth;
            if (MaxW < sMinWidth) MaxW = sMinWidth;
            txtCurProducts.Width = txtCurReactants.Width = txtNewProducts.Width = txtNewReactants.Width = MaxW;
            txtCurDirection.Left = txtNewDirection.Left = txtCurReactants.Right + sPadding;
            txtCurProducts.Left = txtNewProducts.Left = txtCurDirection.Right + sPadding;
            grpCurrentReaction.Width = grpNewReaction.Width = 2 * MaxW + txtCurDirection.Width + 4 * sPadding;
            this.MinimumSize = this.MaximumSize = new Size(grpCurrentReaction.Width + sExtraFrmPadding, this.Height);
            this.Width = grpCurrentReaction.Width + sExtraFrmPadding;

            //Format text boxes:
            //Alignment:
            txtCurReactants.SelectAll(); txtNewReactants.SelectAll();
            txtCurDirection.SelectAll(); txtNewDirection.SelectAll();
            txtCurProducts.SelectAll(); txtNewProducts.SelectAll();
            txtCurReactants.SelectionAlignment = txtNewReactants.SelectionAlignment = 
                txtCurProducts.SelectionAlignment = txtNewProducts.SelectionAlignment =
                txtCurDirection.SelectionAlignment = txtNewDirection.SelectionAlignment =
                HorizontalAlignment.Center;

            #region Bold changes in reactants / products / direction:
            if (!curReaction.ReactantsOk || !curReaction.ProductsOk)
            {
                //Do a straight string comparison:
                if (txtCurReactants.Text != txtNewReactants.Text)
                {
                    HighlightText(txtCurReactants);
                    HighlightText(txtNewReactants);
                }
            }
            else
            {
                //Compare each coefficient:
                foreach (Compound c in curReaction.OrderedReactants)
                    if (!newReaction.OrderedReactants.Contains(c))
                    {
                        txtCurReactants.Select(curReaction.ReactantIndex(c), curReaction.Reactants[c].ToString().Length + c.ToString().Length + 1);
                        HighlightText(txtCurReactants);
                    }
                    else if (newReaction.Reactants[c] != curReaction.Reactants[c])
                    {
                        txtCurReactants.Select(curReaction.ReactantIndex(c), curReaction.Reactants[c].ToString().Length);
                        txtNewReactants.Select(newReaction.ReactantIndex(c), newReaction.Reactants[c].ToString().Length);
                        HighlightText(txtCurReactants);
                        HighlightText(txtNewReactants);
                    }
                foreach (Compound c in newReaction.OrderedReactants)
                    if (!curReaction.OrderedReactants.Contains(c))
                    {
                        txtNewReactants.Select(newReaction.ReactantIndex(c), newReaction.Reactants[c].ToString().Length + c.ToString().Length + 1);
                        HighlightText(txtNewReactants);
                    }
            }

            //Bold changes in products:
            if (!curReaction.ProductsOk || !curReaction.ProductsOk)
            {
                //Do a straight string comparison:
                if (txtCurProducts.Text != txtNewProducts.Text)
                {
                    HighlightText(txtCurProducts);
                    HighlightText(txtNewProducts);
                }
            }
            else
            {
                //Compare each coefficient:
                foreach (Compound c in curReaction.OrderedProducts)
                    if (!newReaction.OrderedProducts.Contains(c))
                    {
                        txtCurProducts.Select(curReaction.ProductIndex(c), curReaction.Products[c].ToString().Length + c.ToString().Length + 1);
                        HighlightText(txtCurProducts);
                    }
                    else if (newReaction.Products[c] != curReaction.Products[c])
                    {
                        txtCurProducts.Select(curReaction.ProductIndex(c), curReaction.Products[c].ToString().Length);
                        txtNewProducts.Select(newReaction.ProductIndex(c), newReaction.Products[c].ToString().Length);
                        HighlightText(txtCurProducts);
                        HighlightText(txtNewProducts);
                    }
                foreach (Compound c in newReaction.OrderedProducts)
                    if (!curReaction.OrderedProducts.Contains(c))
                    {
                        txtNewProducts.Select(newReaction.ProductIndex(c), newReaction.Products[c].ToString().Length + c.ToString().Length + 1);
                        HighlightText(txtNewProducts);
                    }
            }

            //Direction
            if (txtCurDirection.Text != txtNewDirection.Text)
            {
                HighlightText(txtCurDirection); HighlightText(txtNewDirection);
            }
            #endregion Formula

            #region Extent
            if (curReaction.ExtentType != newReaction.ExtentType)
            {
                txtCurExtent.SelectAll(); txtNewExtent.SelectAll();
                HighlightText(txtCurExtent);
                HighlightText(txtNewExtent);
            }
            else if (txtCurExtent.Text != txtNewExtent.Text)
            {
                //We only want to bold the stuff after "Extent: [Type]"
                txtCurExtent.Select(curReaction.ExtentInfo.StartString.Length, txtCurExtent.Text.Length - curReaction.ExtentInfo.StartString.Length);
                txtNewExtent.Select(newReaction.ExtentInfo.StartString.Length, txtNewExtent.Text.Length - newReaction.ExtentInfo.StartString.Length);
                HighlightText(txtCurExtent);
                HighlightText(txtNewExtent);
            }
            #endregion Extent

            #region HOR
            if (curReaction.CustomHeatOfReaction != newReaction.CustomHeatOfReaction)
            {
                txtCurHOR.SelectAll(); txtNewHOR.SelectAll();
                HighlightText(txtCurHOR);
                HighlightText(txtNewHOR);
            }
            else if (HOR)
            {
                Match curMatch = SimpleReaction.s_HORRegex.Match(curReaction.HeatOfReactionString);
                Match newMatch = SimpleReaction.s_HORRegex.Match(newReaction.HeatOfReactionString);
                foreach (string s in SimpleReaction.s_HORRegex.GetGroupNames())
                {
                    if (curMatch.Groups[s].Value != newMatch.Groups[s].Value)
                    {
                        if (curMatch.Groups[s].Success)
                        {
                            txtCurHOR.Select(curMatch.Groups[s].Index, curMatch.Groups[s].Length);
                            HighlightText(txtCurHOR);
                        }
                        if (newMatch.Groups[s].Success)
                        {
                            txtNewHOR.Select(newMatch.Groups[s].Index, newMatch.Groups[s].Length);
                            HighlightText(txtNewHOR);
                        }
                    }
                }
                #region Old way of going about it
                /*if (curReaction.HeatOfReactionType != newReaction.HeatOfReactionType) //Type
                {
                    txtCurHOR.Select(curMatch.Groups["Type"].Index, curMatch.Groups["Type"].Length);
                    txtNewHOR.Select(newMatch.Groups["Type"].Index, newMatch.Groups["Type"].Length);
                    txtCurHOR.SelectionFont = txtNewHOR.SelectionFont = BoldFont;
                }
                if (curReaction.HeatOfReactionValue != newReaction.HeatOfReactionValue) //Value
                {
                    txtCurHOR.Select(curMatch.Groups["Value"].Index, curMatch.Groups["Value"].Length);
                    txtNewHOR.Select(newMatch.Groups["Value"].Index, newMatch.Groups["Value"].Length);
                    txtCurHOR.SelectionFont = txtNewHOR.SelectionFont = BoldFont;
                }
                if (curReaction.HeatOfReactionSpecie != newReaction.HeatOfReactionSpecie) //Specie
                {
                    txtCurHOR.Select(curMatch.Groups["Specie"].Index, curMatch.Groups["Specie"].Length);
                    txtNewHOR.Select(newMatch.Groups["Specie"].Index, newMatch.Groups["Specie"].Length);
                    txtCurHOR.SelectionFont = txtNewHOR.SelectionFont = BoldFont;
                }
                if (curReaction.HeatOfReactionConditions != newReaction.HeatOfReactionConditions) //Conditions
                {
                    txtCurHOR.Select(curMatch.Groups["Cond"].Index, curMatch.Groups["Cond"].Length);
                    txtNewHOR.Select(newMatch.Groups["Cond"].Index, newMatch.Groups["Cond"].Length);
                    txtCurHOR.SelectionFont = txtNewHOR.SelectionFont = BoldFont;
                }
                else if (curReaction.HeatOfReactionConditions == TPConditions.Custom)
                {
                    if (curReaction.HeatOfReactionT != newReaction.HeatOfReactionT)
                    {
                        txtCurHOR.Select(curMatch.Groups["T"].Index, curMatch.Groups["T"].Length);
                        txtNewHOR.Select(newMatch.Groups["T"].Index, newMatch.Groups["T"].Length);
                        txtCurHOR.SelectionFont = txtNewHOR.SelectionFont = BoldFont;
                    }
                    if (curReaction.HeatOfReactionP != newReaction.HeatOfReactionP)
                    {
                        txtCurHOR.Select(curMatch.Groups["P"].Index, curMatch.Groups["P"].Length);
                        txtNewHOR.Select(newMatch.Groups["P"].Index, newMatch.Groups["P"].Length);
                        txtCurHOR.SelectionFont = txtNewHOR.SelectionFont = BoldFont;
                    }
                }*/
                #endregion
            }
            #endregion HOR

            #region Seq
            if (bSeq && curReaction.Sequence != newReaction.Sequence)
            {
                txtCurSeq.SelectAll(); txtNewSeq.SelectAll();
                HighlightText(txtCurSeq);
                HighlightText(txtNewSeq);
            }
            #endregion Seq
        }

        protected void ChangeGeneric(bool showGeneric)
        {
            txtCurGeneric.Visible = txtNewGeneric.Visible = showGeneric;
            txtCurReactants.Visible = txtCurDirection.Visible = txtCurProducts.Visible =
                txtCurExtent.Visible = txtCurHOR.Visible = txtCurSeq.Visible =
                txtNewReactants.Visible = txtNewDirection.Visible = txtNewProducts.Visible =
                txtNewExtent.Visible = txtNewHOR.Visible = txtNewSeq.Visible =
                !showGeneric;

            //Form layout:
            if (showGeneric)
            {
                grpCurrentReaction.Height = grpNewReaction.Height = txtCurGeneric.Height + grpCurrentReaction.Padding.Bottom + grpCurrentReaction.Padding.Top;
                this.MinimumSize = this.MaximumSize = new Size(this.Width, sMaxFrmHeight - 2 * (sMaxGrpHeight - grpCurrentReaction.Height));
                this.Height = sMaxFrmHeight - 2 * (sMaxGrpHeight - grpCurrentReaction.Height);
                grpNewReaction.Top = grpCurrentReaction.Bottom + sPadding;
            }
        }

        protected void HighlightText(RichTextBox box)
        {
            box.SelectionColor = Color.Blue;
            box.SelectionFont = new Font(box.Font, FontStyle.Bold);
        }

        private void richTextBox4_Enter(object sender, EventArgs e)
        {
            if (LastControl != null)
                LastControl.Select();
        }

        private void btnOk_Enter(object sender, EventArgs e)
        {
            LastControl = (Control)sender;
        }
    }
}