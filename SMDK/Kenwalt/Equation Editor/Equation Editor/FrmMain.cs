using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Text.RegularExpressions;
using Microsoft.Win32;

namespace Reaction_Editor
{
    public partial class FrmMain : Form
    {
        #region Internal Variables
        Rectangle m_DragRect = Rectangle.Empty;
        Compound m_DraggedCompound = null;
        int m_nUntitledNo = 1;
        ILog Log = new FrmLog();
        RegistryKey regKey = Registry.CurrentUser.CreateSubKey("Software").CreateSubKey("Kenwalt").CreateSubKey("SysCAD Reaction Editor");
        #endregion Internal Variables

        #region Protected Functions
        /// <summary>
        /// If an element is found in the specie list that is not present in the element databse, attempt to guess its properties.
        /// </summary>
        public static bool CreateElems = true;
        protected void OpenSpecieDB(string filename)
        {
            try
            {
                string db = File.ReadAllText(filename);
                if (!db.Contains("[Species]"))
                {
                    Log.Message(
                        "[Species] token not found in database file",
                        MessageType.Error,
                        new MessageSource(filename));
                    return;
                }
                int elementCount = 0, specieCount = 0;
                for (Match m = s_ElementRegex.Match(db); m.Success; m = m.NextMatch())
                {
                    Element elem = new Element();
                    elem.AtmoicNo = int.Parse(m.Groups["AtmNo"].Value);
                    elem.AtomicMass = double.Parse(m.Groups["AtmWt"].Value);
                    elem.Name = elem.Symbol = m.Groups["Sym"].Value;
                    if (Element.AddElement(elem))
                        elementCount++;
                }

                for (Match m = s_CompoundRegex.Match(db); m.Success; m = m.NextMatch())
                {
                    Compound comp = new Compound();
                    comp.Name = m.Groups["Name"].Value;
                    comp.Symbol = m.Groups["Sym"].Value;
                    if (m.Groups["Phase"].Value == "Solid")
                        comp.Phase = Phases.Solid;
                    else if (m.Groups["Phase"].Value == "Liquid")
                        comp.Phase = Phases.Liquid;
                    else if (m.Groups["Phase"].Value == "Gas")
                        comp.Phase = Phases.Gas;
                    for (int i = 0; i < m.Groups["ElementSym"].Captures.Count; i++)
                    {
                        string sym = m.Groups["ElementSym"].Captures[i].Value;
                        if (CreateElems && !Element.ElementList.ContainsKey(sym))
                        {
                            Element.AddElement(new Element(sym
                                , -1
                                , double.NaN));
                            Log.Message("Element '" + sym + "' not found in element database (Found in compound '" + comp.Name + "')",
                                MessageType.Warning,
                                new MessageSource(Path.GetFileName(filename)));
                        }
                        comp.Elements.Add(Element.ElementList[sym],
                                          double.Parse(m.Groups["ElemCount"].Captures[i].Value));
                    }

                    if (Compound.AddCompound(comp))
                    {
                        ListViewItem lvi = new ListViewItem(new string[] { comp.Name, comp.Symbol });
                        lstSpecies.Items.Add(lvi);
                        lvi.Group = lstSpecies.Groups[comp.Phase.ToString()];
                        lvi.Tag = comp;
                        specieCount++;
                    }
                }
                Log.Message(elementCount.ToString() + " Elements and " + specieCount.ToString() + " Species loaded.",
                    MessageType.Note,
                    new MessageSource(filename));
                treeFiles.Nodes["SpecieDB"].Nodes.Add(Path.GetFileName(filename));
                treeFiles.Nodes["SpecieDB"].Expand();
                regKey.SetValue("Last Database", filename);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Open database", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Log.Message(ex.Message, MessageType.Error);
            }
        }
        protected static Regex s_ElementRegex = new Regex(
            @"^E\d{3}=(?<Sym>[^,\s]{1,3})\s*,\s*(?<AtmNo>\d{1,3})\s*,\s*(?<AtmWt>(\d+(\.\d+)?|\.\d+))\s*$",
            RegexOptions.Compiled | RegexOptions.Multiline | RegexOptions.ExplicitCapture);
        protected static Regex s_CompoundRegex = new Regex(
            @"^S\d{3}=Specie\s*,\s*(?<Name>[^\r\n,\s]*)\s*,\s*(?<Sym>[^\r\n\s,]*)\s*,\s*(?<Phase>Solid|Liquid|Vapour)\s*,\s*(?<MolWt>(\d+(.\d+)?|.\d+))(\s*,\s*(?<ElementSym>[^,\r\n\s]*)\s*,\s*(?<ElemCount>\d*(\.\d+)?)\s*)+$",
            RegexOptions.Compiled | RegexOptions.Multiline | RegexOptions.ExplicitCapture);

        protected void Open(string filename)
        {
            try
            {
                FrmReaction frm = new FrmReaction(filename, Log);
                RegisterForm(frm);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Open file", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Log.Message(ex.Message, MessageType.Error);
            }
        }

        protected void RegisterForm(FrmReaction frm)
        {
            frm.FormClosing += new FormClosingEventHandler(frm_FormClosing);
            frm.FormClosed += new FormClosedEventHandler(frm_FormClosed);
            frm.MdiParent = this;
            frm.Show();
            TreeNode newNode = new TreeNode(frm.Text);
            newNode.Tag = frm;
            frm.Tag = newNode;
            newNode.Name = frm.Text;
            treeFiles.Nodes["RBs"].Nodes.Add(newNode);
            treeFiles.Nodes["RBs"].Expand();
            foreach (SimpleReaction r in frm.Reactions)
            {
                TreeNode subNode = new TreeNode(r.ToString());
                subNode.Tag = r;
                subNode.Name = r.ToString();
                newNode.Nodes.Add(subNode);
            }
        }

        protected void Save(FrmReaction target)
        {
            try
            {
                if (!target.FileOpen)
                    SaveAs(target);
                else
                    target.Save();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Save file", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Log.Message(ex.Message, MessageType.Error);
            }
        }

        protected void SaveAs(FrmReaction target)
        {
            try
            {
                if (target.FileOpen)
                {
                    dlgSaveRxn.FileName = target.Filename;
                }
                dlgSaveRxn.Title = "Save Reaction " + target.Title;
                if (dlgSaveRxn.ShowDialog(this) == DialogResult.OK)
                    target.SaveAs(dlgSaveRxn.FileName);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Save file", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Log.Message(ex.Message, MessageType.Error);
            }
        }

        protected void RepositionLog()
        {
            FrmLog frm = (FrmLog)Log;
            frm.Size = new Size(this.ClientSize.Width - grpFiles.Width - grpSpecies.Width - 20,
                150);
            frm.Location = new Point(5, this.ClientRectangle.Bottom - frm.Height - statusStrip1.Height - 30);
        }
        #endregion

        public FrmMain()
        {
            InitializeComponent();
        }

        private void editToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void FrmMain_Load(object sender, EventArgs e)
        {
            FrmLog frm = (FrmLog)Log;
            frm.MdiParent = this;

            frm.Show();
            RepositionLog();
            string fn = (string) regKey.GetValue("Last Database", "");
            if (!string.IsNullOrEmpty(fn) && File.Exists(fn))
                OpenSpecieDB(fn);

            /*//Debug/Test stuff:
            FrmReaction test = new FrmReaction(1, this.toolStripStatusLabel1);
            FrmReaction test2 = new FrmReaction(2, this.toolStripStatusLabel1);
            test2.MdiParent = this;
            test.MdiParent = this;
            test.Show();
            test2.Show();

            Element Hydrogen = new Element();
            Hydrogen.Name = "Hydrogen";
            Hydrogen.Symbol = "H";
            Element Oxygen = new Element();
            Oxygen.Name = "Oxygen";
            Oxygen.Symbol = "O";
            Element Nitrogen = new Element();
            Nitrogen.Name = "Nitrogen";
            Nitrogen.Symbol = "N";
            Element Carbon = new Element();
            Carbon.Name = "Carbon";
            Carbon.Symbol = "C";
            Element Nickel = new Element();
            Nickel.Name = "Nickel";
            Nickel.Symbol = "Ni";
            Element Copper = new Element();
            Copper.Name = "Copper";
            Copper.Symbol = "Cu";
            Element Sulfur = new Element();
            Element Iron = new Element();
            Element Arsenic = new Element();

            Compound water = new Compound();
            water.Elements.Add(Hydrogen, 2);
            water.Elements.Add(Oxygen, 1);
            water.HeatOfFormation = -1000;
            water.MolWeight = 18;
            water.Name = "Water";
            water.Phase = Phases.Liquid;
            water.Symbol = "H2O(l)";

            Compound steam = new Compound();
            steam.Elements = water.Elements;
            steam.Name = "Steam";
            steam.Phase = Phases.Gas;
            steam.Symbol = "H2O(g)";
            Compound.AddCompound(steam);

            Compound co2 = new Compound();
            co2.Elements.Add(Carbon, 1);
            co2.Elements.Add(Oxygen, 2);
            co2.HeatOfFormation = -425;
            co2.Name = "Carbon Dioxide";
            co2.Phase = Phases.Gas;
            co2.Symbol = "CO2(g)";

            Compound ammonia = new Compound();
            ammonia.Elements.Add(Nitrogen, 1);
            ammonia.Elements.Add(Hydrogen, 3);
            ammonia.HeatOfFormation = 100;
            ammonia.Name = "Ammonia";
            ammonia.Phase = Phases.Gas;
            ammonia.Symbol = "NH3(g)";

            Compound NickelSulfide = new Compound();
            NickelSulfide.Elements.Add(Nickel, 1);
            NickelSulfide.Elements.Add(Sulfur, 1);
            NickelSulfide.Name = "Nickel Sulfate";
            NickelSulfide.Phase = Phases.Solid;
            NickelSulfide.Symbol = "NiS(s)";
            Compound.AddCompound(NickelSulfide);

            Compound CopperSulfide = new Compound();
            CopperSulfide.Elements.Add(Copper, 1.0);
            CopperSulfide.Elements.Add(Sulfur, 1.0);
            CopperSulfide.Name = "Copper Sulfide";
            CopperSulfide.Symbol = "CuS(s)";
            CopperSulfide.Phase = Phases.Solid;
            Compound.AddCompound(CopperSulfide);

            Compound CopperSulfate = new Compound();
            CopperSulfate.Elements.Add(Copper, 1);
            CopperSulfate.Elements.Add(Sulfur, 1);
            CopperSulfate.Elements.Add(Oxygen, 4);
            CopperSulfate.Name = "Copper Sulfate";
            CopperSulfate.Symbol = "CuSO4(aq)";
            CopperSulfate.Phase = Phases.Liquid;
            Compound.AddCompound(CopperSulfate);

            Compound NickelSulfate = new Compound();
            NickelSulfate.Elements.Add(Nickel, 1);
            NickelSulfate.Elements.Add(Sulfur, 1);
            NickelSulfate.Elements.Add(Oxygen, 4);
            NickelSulfate.Name = "Nickel Sulfate";
            NickelSulfate.Symbol = "NiSO4(aq)";
            NickelSulfate.Phase = Phases.Liquid;
            Compound.AddCompound(NickelSulfate);

            Compound Ni3S4 = new Compound();
            Ni3S4.Elements.Add(Nickel, 3);
            Ni3S4.Elements.Add(Sulfur, 4);
            Ni3S4.Name = Ni3S4.Symbol = "Ni3S4(s)";
            Ni3S4.Phase = Phases.Solid;
            Compound.AddCompound(Ni3S4);

            Compound Cu9S5 = new Compound();
            Cu9S5.Elements.Add(Copper, 9);
            Cu9S5.Elements.Add(Sulfur, 5);
            Cu9S5.Name = Cu9S5.Symbol = "Cu9S5(s)";
            Cu9S5.Phase = Phases.Solid;
            Compound.AddCompound(Cu9S5);

            Compound SulfuricAcid = new Compound();
            SulfuricAcid.Elements.Add(Hydrogen, 2);
            SulfuricAcid.Elements.Add(Sulfur, 1);
            SulfuricAcid.Elements.Add(Oxygen, 4);
            SulfuricAcid.Name = "Sulfuric Acid";
            SulfuricAcid.Symbol = "H2SO4(aq)";
            SulfuricAcid.Phase = Phases.Liquid;
            Compound.AddCompound(SulfuricAcid);

            Compound ConcSulfuricAcid = new Compound();
            ConcSulfuricAcid.Elements.Add(Hydrogen, 2);
            ConcSulfuricAcid.Elements.Add(Sulfur, 1);
            ConcSulfuricAcid.Elements.Add(Oxygen, 4);
            ConcSulfuricAcid.Name = "Conc Sulfuric Acid";
            ConcSulfuricAcid.Symbol = "H2SO4(l)";
            ConcSulfuricAcid.Phase = Phases.Liquid;
            Compound.AddCompound(ConcSulfuricAcid);

            Compound NickelOxide = new Compound();
            NickelOxide.Elements.Add(Nickel, 1);
            NickelOxide.Elements.Add(Oxygen, 1);
            NickelOxide.Name = "Nickel Oxide";
            NickelOxide.Symbol = "NiO(s)";
            NickelOxide.Phase = Phases.Solid;
            Compound.AddCompound(NickelOxide);

            Compound CopperOxide = new Compound();
            CopperOxide.Elements.Add(Copper, 1);
            CopperOxide.Elements.Add(Oxygen, 1);
            CopperOxide.Name = "Copper Oxide";
            CopperOxide.Symbol = "CuO(s)";
            CopperOxide.Phase = Phases.Solid;
            Compound.AddCompound(CopperOxide);

            Compound FeAsO4 = new Compound();
            FeAsO4.Elements.Add(Iron, 1);
            FeAsO4.Elements.Add(Arsenic, 1);
            FeAsO4.Elements.Add(Oxygen, 4);
            FeAsO4.Name = FeAsO4.Symbol = "FeAsO4(s)";
            FeAsO4.Phase = Phases.Solid;
            Compound.AddCompound(FeAsO4);

            Compound Fe2SO43 = new Compound();
            Fe2SO43.Elements.Add(Iron, 2);
            Fe2SO43.Elements.Add(Sulfur, 3);
            Fe2SO43.Elements.Add(Oxygen, 12);
            Fe2SO43.Symbol = Fe2SO43.Name = "Fe2[SO4]3(aq)";
            Fe2SO43.Phase = Phases.Liquid;
            Compound.AddCompound(Fe2SO43);

            Compound H3AsO4 = new Compound();
            H3AsO4.Elements.Add(Hydrogen, 3);
            H3AsO4.Elements.Add(Arsenic, 1);
            H3AsO4.Elements.Add(Oxygen, 4);
            H3AsO4.Name = H3AsO4.Symbol = "H3AsO4(aq)";
            H3AsO4.Phase = Phases.Liquid;
            Compound.AddCompound(H3AsO4);

            Compound IronSulfate = new Compound();
            IronSulfate.Elements.Add(Iron, 1);
            IronSulfate.Elements.Add(Sulfur, 1);
            IronSulfate.Elements.Add(Oxygen, 4);
            IronSulfate.Name = "Iron Sulfate";
            IronSulfate.Symbol = "FeSO4(aq)";
            IronSulfate.Phase = Phases.Liquid;
            Compound.AddCompound(IronSulfate);

            Compound.AddCompound(water);
            Compound.AddCompound(co2);
            Compound.AddCompound(ammonia);

            foreach (Compound c in Compound.CompoundList.Values)
            {
                ListViewItem lvi = new ListViewItem(new string[] { c.Name, c.Symbol });
                lstSpecies.Items.Add(lvi);
                lvi.Group = lstSpecies.Groups[c.Phase.ToString()];
                lvi.Tag = c;
            }

            test.RepopulateSpecies();
            test2.RepopulateSpecies();*/
        }

        private void lstSpecies_MouseDown(object sender, MouseEventArgs e)
        {
            ListViewHitTestInfo hti = lstSpecies.HitTest(e.Location);
            if (hti.Item == null)
            {
                m_DragRect = Rectangle.Empty;
                return;
            }
            Size drgSz = SystemInformation.DragSize;
            m_DragRect = new Rectangle(e.X - drgSz.Width / 2, e.Y - drgSz.Height, drgSz.Width, drgSz.Height);
            m_DraggedCompound = (Compound)hti.Item.Tag;
        }

        private void lstSpecies_MouseUp(object sender, MouseEventArgs e)
        {
            m_DragRect = Rectangle.Empty;
        }

        private void lstSpecies_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                if (m_DragRect != Rectangle.Empty && !m_DragRect.Contains(e.Location) && m_DraggedCompound != null)
                    DoDragDrop(m_DraggedCompound, DragDropEffects.Link);
            }
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (dlgOpenRxn.ShowDialog(this) != DialogResult.OK)
                return;
            foreach (string file in dlgOpenRxn.FileNames)
                    Open(file);
        }


        void frm_FormClosed(object sender, FormClosedEventArgs e)
        {
            TreeNode node = (TreeNode)((Form)sender).Tag;
            if (treeFiles.Nodes["RBs"].Nodes.Contains(node))
                treeFiles.Nodes["RBs"].Nodes.Remove(node);
        }

        void frm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!((FrmReaction)sender).Changed) return;

            switch (MessageBox.Show(this,
                "Do you want to save the changes you made to '" + ((FrmReaction)sender).Title + "'",
                "SysCAD Reaction Editor",
                MessageBoxButtons.YesNoCancel, MessageBoxIcon.Warning))
            {
                case DialogResult.Yes:
                    Save((FrmReaction)sender);
                    break;
                case DialogResult.No:
                    break;
                case DialogResult.Cancel:
                    e.Cancel = true;
                    break;
            }
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (this.ActiveMdiChild.GetType() != typeof(FrmReaction))
                return;
            Save((FrmReaction) this.ActiveMdiChild);
        }


        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (this.ActiveMdiChild.GetType() != typeof(FrmReaction))
                return;
            SaveAs((FrmReaction)this.ActiveMdiChild);
        }

        private void FrmMain_MdiChildActivate(object sender, EventArgs e)
        {
            if (ActiveMdiChild == null)
                this.Text = "SysCAD Reaction Editor";
            else if (ActiveMdiChild.GetType() == typeof(FrmReaction))
                this.Text = "SysCAD Reaction Editor - " + ((FrmReaction)ActiveMdiChild).Title;
            else
                this.Text = "SysCAD Reaction Editor - " + ActiveMdiChild.Text;
        }

        private void menuOpenDB_Click(object sender, EventArgs e)
        {
            if (dlgOpenDB.ShowDialog(this) == DialogResult.OK)
            {
                OpenSpecieDB(dlgOpenDB.FileName);                    
            }
        }

        private void menuNew_Click(object sender, EventArgs e)
        {
            FrmReaction frm = new FrmReaction(m_nUntitledNo++, Log);
            RegisterForm(frm);
        }

        private void treeFiles_DoubleClick(object sender, EventArgs e)
        {
            if (treeFiles.SelectedNode != null)
            {
                if (treeFiles.SelectedNode.Parent == null)
                    return;
                else if (treeFiles.SelectedNode.Parent == treeFiles.Nodes["RBs"])
                    ((Form)treeFiles.SelectedNode.Tag).Activate();
                else if (treeFiles.SelectedNode.Tag != null
               && treeFiles.SelectedNode.Tag is SimpleReaction)
                {
                    ((Form)treeFiles.SelectedNode.Parent.Tag).Activate();
                    ((FrmReaction)treeFiles.SelectedNode.Parent.Tag).SelectReaction((SimpleReaction)treeFiles.SelectedNode.Tag);
                }
            }
        }

        private void menuClose_Click(object sender, EventArgs e)
        {
            if (ActiveMdiChild is FrmReaction)
                ActiveMdiChild.Close();
        }

        private void menuCascade_Click(object sender, EventArgs e)
        {
            LayoutMdi(MdiLayout.Cascade);
            RepositionLog();
        }

        private void undockToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form f = ActiveMdiChild;
            ActiveMdiChild.MdiParent = null;
            f.Owner = this;
        }

        private void aboutSysCADReactionEditorToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show("SysCAD Reaction editor version 0.1\r\nTest version.");
        }

        private void menuExit_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void fileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            menuClose.Enabled =
                menuSave.Enabled =
                menuSaveAs.Enabled =
                (ActiveMdiChild != null && ActiveMdiChild is FrmReaction);
        }

        private void menuSaveAll_Click(object sender, EventArgs e)
        {
            foreach (Form f in OwnedForms)
                if (f is FrmReaction)
                    Save((FrmReaction)f);
            foreach (Form f in MdiChildren)
                if (f is FrmReaction)
                    Save((FrmReaction)f);
        }
    }
}