using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace Reaction_Editor
{
    public class ReactionList : TableLayoutPanel, IEnumerable<SimpleReaction>
    {
        protected List<SimpleReaction> m_Reactions = new List<SimpleReaction>();

        public ReactionList()
        {
            this.ColumnCount = 4;
            this.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 50));
            this.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 80));
            this.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 50));
            this.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 20));
        }

        public SimpleReaction this[int i]
        {
            get { return m_Reactions[i]; }
            set 
            {
                m_Reactions[i] = value;
                //TODO: Manage updates.
            }
        }

        public void Add(SimpleReaction rxn)
        {
            m_Reactions.Add(rxn);
            //TODO: Manage Adds:

        }

        public void Remove(SimpleReaction rxn)
        {
            m_Reactions.Remove(rxn);
            //TODO: Manage removals
        }

        #region IEnumerable<SimpleReaction> Members
        public IEnumerator<SimpleReaction> GetEnumerator()
        {
            return m_Reactions.GetEnumerator();
        }
        #endregion

        protected struct ReactionControls
        {
            public ReactionControls(ReactionList owner, SimpleReaction rxn)
            {
                if (rxn.Status == RxnStatuses.Invalid)
                {
                    //TODO: Handle an invalid reaction status.
                }
                txt
                txtReactants = new TextBox();
                txtProducts = new TextBox();
                comboDirection = new ComboBox();
                owner.Controls.AddRange(new Control[] { txtReactants, txtProducts, comboDirection });

                txtReactants.Text = rxn.GetReactantsString(); txtReactants.TextAlign = HorizontalAlignment.Right;
                txtProducts.Text = rxn.GetProductsString(); txtProducts.TextAlign = HorizontalAlignment.Left;

                comboDirection.DropDownStyle = ComboBoxStyle.DropDownList;
                comboDirection.Items.AddRange(new string[] { "<-", "<->", "->" });
                comboDirection.SelectedIndex = (int)rxn.Direction;
            }
            TextBox txtReactants, txtProducts;
            ComboBox comboDirection;
        }
    }
}
