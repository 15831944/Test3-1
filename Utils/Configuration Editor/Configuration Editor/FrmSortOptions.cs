using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Configuration_Editor
{
    public partial class FrmSortOptions : Form
    {
        protected PVIOrderer m_Sorter;

        public FrmSortOptions()
        {
            InitializeComponent();

            lstPhaseOrder.Items.Add(Phase.Solid);
            lstPhaseOrder.Items.Add(Phase.Liquid);
            lstPhaseOrder.Items.Add(Phase.Gas);

            Sorter = new PVIOrderer();
        }

        public PVIOrderer Sorter
        {
            get { return m_Sorter; }
            set
            {
                if (m_Sorter == null)
                {
                    m_Sorter = value;
                    ChangeSorter();
                }
                else
                {
                    m_Sorter = value;
                    chkByPhase.Checked = m_Sorter.m_bByPhase;
                    chkPreservePageAndLabels.Checked = m_Sorter.m_bPreservePageAndLabelLocations;
                    chkPreserveCalculations.Checked = m_Sorter.m_bPreserveCalculationLocations;
                    if (m_Sorter.m_bAscending)
                        radioAscending.Checked = true;
                    else
                        radioDescending.Checked = true;
                    lstPhaseOrder.Items.Clear();
                    foreach (Phase ph in m_Sorter.m_PhaseOrder)
                        if (ph != Phase.Total)
                            lstPhaseOrder.Items.Add(ph);
                }
            }
        }

        private void ChangeSorter()
        {
            m_Sorter.m_bByPhase = chkByPhase.Checked;
            m_Sorter.m_bPreservePageAndLabelLocations = chkPreservePageAndLabels.Checked;
            m_Sorter.m_bPreserveCalculationLocations = chkPreserveCalculations.Checked;
            m_Sorter.m_bAscending = radioAscending.Checked;
            m_Sorter.m_PhaseOrder.Clear();
            m_Sorter.m_PhaseOrder.Add(Phase.Total);
            foreach (Phase ph in lstPhaseOrder.Items)
                m_Sorter.m_PhaseOrder.Add(ph);
        }

        private void chkByPhase_CheckedChanged(object sender, EventArgs e)
        {
            ChangeSorter();
        }

        private void chkPreservePageAndLabels_CheckedChanged(object sender, EventArgs e)
        {
            ChangeSorter();
        }

        private void chkPreserveCalculations_CheckedChanged(object sender, EventArgs e)
        {
            ChangeSorter();
        }

        private void radioAscending_CheckedChanged(object sender, EventArgs e)
        {
            ChangeSorter();
        }

        object m_PhaseSelectedItem = null;
        private void lstPhaseOrder_MouseDown(object sender, MouseEventArgs e)
        {
            for (int i = 0; i < lstPhaseOrder.Items.Count; i++)
                if (lstPhaseOrder.GetItemRectangle(i).Contains(e.X, e.Y))
                {
                    m_PhaseSelectedItem = lstPhaseOrder.Items[i];
                    return;
                }
            m_PhaseSelectedItem = null;
        }

        private void lstPhaseOrder_MouseMove(object sender, MouseEventArgs e)
        {
            if (m_PhaseSelectedItem != null && e.Button == MouseButtons.Left)
            {
                lstPhaseOrder.BeginUpdate();
                Rectangle oldRect = lstPhaseOrder.GetItemRectangle(lstPhaseOrder.Items.IndexOf(m_PhaseSelectedItem));
                if (oldRect.Top > e.Y || e.Y > oldRect.Bottom)
                {
                    int newIndex = 0;
                    //Find the index of the item that does contain the item:
                    if (lstPhaseOrder.GetItemRectangle(0).Top > e.Y)
                        newIndex = 0;
                    else if (lstPhaseOrder.GetItemRectangle(lstPhaseOrder.Items.Count - 1).Bottom < e.Y)
                        newIndex = lstPhaseOrder.Items.Count - 1;
                    else //Find which one does contain it:
                        for (int i = 0; i < lstPhaseOrder.Items.Count; i++)
                        {
                            Rectangle curRect = lstPhaseOrder.GetItemRectangle(i);
                            if (curRect.Bottom > e.Y && e.Y > curRect.Top)
                                newIndex = i;
                        }

                    //if (newIndex > lstPhaseOrder.Items.IndexOf(m_PhaseSelectedItem))
                      //  newIndex--;

                    lstPhaseOrder.Items.Remove(m_PhaseSelectedItem);
                    lstPhaseOrder.Items.Insert(newIndex, m_PhaseSelectedItem);
                    lstPhaseOrder.SelectedIndex = newIndex;
                }
                lstPhaseOrder.EndUpdate();
            }
        }

        private void lstPhaseOrder_MouseUp(object sender, MouseEventArgs e)
        {
            m_PhaseSelectedItem = null;
            ChangeSorter();
        }
    }
}