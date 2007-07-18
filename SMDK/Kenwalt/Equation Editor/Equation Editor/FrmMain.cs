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
        //Rectangle m_DragRect = Rectangle.Empty;
        //Compound m_DraggedCompound = null;
        int m_nUntitledNo = 1;
        ILog Log = new FrmLog();
        RegistryKey regKey = Registry.CurrentUser.CreateSubKey("Software").CreateSubKey("Kenwalt").CreateSubKey("SysCAD Reaction Editor");
        protected Queue<String> m_RecentFiles = new Queue<string>();
        protected int m_nRecentFileCount = 6;
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
                    else if (m.Groups["Phase"].Value == "Vapour")
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
                TreeNode newNode = new TreeNode(filename);
                newNode.ContextMenuStrip = menuDatabaseFile;
                treeFiles.Nodes["SpecieDB"].Nodes.Add(newNode);
                treeFiles.Nodes["SpecieDB"].Expand();
                regKey.SetValue("Last Database", filename);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Open database", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Log.Message(ex.Message, MessageType.Error);
            }
            DoDatabaseChanged();
        }
        protected static Regex s_ElementRegex = new Regex(
            @"^E\d{3}=(?<Sym>[^,\s]{1,3})\s*,\s*(?<AtmNo>\d{1,3})\s*,\s*(?<AtmWt>(\d+(\.\d+)?|\.\d+))\s*$",
            RegexOptions.Compiled | RegexOptions.Multiline | RegexOptions.ExplicitCapture);
        protected static Regex s_CompoundRegex = new Regex(
            @"^S\d{3}=Specie\s*,\s*(?<Name>[^\r\n,\s]*)\s*,\s*(?<Sym>[^\r\n\s,]*)\s*,\s*(?<Phase>Solid|Liquid|Vapour)\s*,\s*(?<MolWt>(\d+(.\d+)?|.\d+))(\s*,\s*(?<ElementSym>[^,\r\n\s]*)\s*,\s*(?<ElemCount>\d*(\.\d+)?)\s*)+$",
            RegexOptions.Compiled | RegexOptions.Multiline | RegexOptions.ExplicitCapture);

        protected void DoDatabaseChanged()
        {
            foreach (Form f in MdiChildren)
                if (f is FrmReaction)
                    ((FrmReaction)f).DoDatabaseChanged();
        }

        protected void Open(string filename)
        {
            try
            {
                FrmReaction frm = new FrmReaction(filename, Log);
                RegisterForm(frm);
                m_RecentFiles.Enqueue(filename);
                while (m_RecentFiles.Count > m_nRecentFileCount)
                    m_RecentFiles.Dequeue();
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
            frm.CompoundsChanged += new EventHandler(frm_CompoundsChanged);
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

        protected void UpdateUsedSpecies(List<Compound> compsInUse)
        {
            lstSpecies.BeginUpdate();
            foreach (ListViewItem lvi in lstSpecies.Items)
                if (compsInUse.Contains((Compound)lvi.Tag))
                    lvi.Font = new Font(lvi.Font, FontStyle.Bold);
                else
                    lvi.Font = new Font(lvi.Font, FontStyle.Regular);
            lstSpecies.EndUpdate();
        }

        protected void UnloadDatabase()
        {
            Compound.CompoundList.Clear();
            Element.ElementList.Clear();
            lstSpecies.Items.Clear();
            treeFiles.Nodes["SpecieDB"].Nodes.Clear();
        }

        protected override void OnClosed(EventArgs e)
        {
            foreach (string s in regKey.GetValueNames())
                if (recentFileRegex.Match(s).Success)
                    regKey.DeleteValue(s);

            int i = 0;
            while (m_RecentFiles.Count > 0)
                regKey.SetValue("RecentFile" + i++, m_RecentFiles.Dequeue());
        }
        #endregion

        public FrmMain()
        {
            InitializeComponent();
            treeFiles.Nodes["SpecieDB"].ContextMenuStrip = menuDatabaseFile;
        }

        void frm_CompoundsChanged(object sender, EventArgs e)
        {
            if (sender == ActiveMdiChild)
                UpdateUsedSpecies(((FrmReaction)sender).GetCompounds());
        }

        Regex recentFileRegex = new Regex("RecentFile\\d+");
        private void FrmMain_Load(object sender, EventArgs e)
        {
            FrmLog frm = (FrmLog)Log;
            frm.MdiParent = this;

            frm.Show();
            frm.TopMost = true;
            RepositionLog();
            string fn = (string)regKey.GetValue("Last Database", "");
            if (!string.IsNullOrEmpty(fn) && File.Exists(fn))
                OpenSpecieDB(fn);
            string[] valueNames = regKey.GetValueNames();
            Array.Sort<String>(valueNames);
            bool firstName = true;
            int recentFileCount = 0;
            int insertionLoc = menuFile.DropDown.Items.IndexOf(menuExit);
            foreach (string s in valueNames)
                if (recentFileRegex.Match(s).Success)
                {
                    if (recentFileCount++ >= m_nRecentFileCount)
                        break; //Don't load more than [m_nRecentFileCount] files.
                    string filename = regKey.GetValue(s).ToString();
                    ToolStripMenuItem recentFile = new ToolStripMenuItem(Path.GetFileName(filename));
                    recentFile.Tag = filename;
                    recentFile.Click += new EventHandler(recentFile_Click);
                    if (firstName)
                    {
                        menuFile.DropDown.Items.Insert(insertionLoc, new ToolStripSeparator());
                        firstName = false;
                    }
                    menuFile.DropDown.Items.Insert(insertionLoc, recentFile);
                    m_RecentFiles.Enqueue(filename);
                }
        }

        void recentFile_Click(object sender, EventArgs e)
        {
            Open((string)((ToolStripMenuItem)sender).Tag);
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
            {
                UpdateUsedSpecies(new List<Compound>());
                this.Text = "SysCAD Reaction Editor";
            }
            else if (ActiveMdiChild.GetType() == typeof(FrmReaction))
            {
                this.Text = "SysCAD Reaction Editor - " + ((FrmReaction)ActiveMdiChild).Title;
                UpdateUsedSpecies(((FrmReaction)ActiveMdiChild).GetCompounds());
            }
            else
            {
                UpdateUsedSpecies(new List<Compound>());
                this.Text = "SysCAD Reaction Editor";
            }
        }

        private void menuOpenDB_Click(object sender, EventArgs e)
        {
            DialogResult res = DialogResult.No;
            if (treeFiles.Nodes["SpecieDB"].Nodes.Count != 0 )
                res = MessageBox.Show("Unload existing specie databases before loading new database?", "Load Database", MessageBoxButtons.YesNoCancel) ;
            if (res == DialogResult.Cancel)
                return;

            if (res == DialogResult.Yes)
                UnloadDatabase();

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
            MessageBox.Show("SysCAD Reaction editor version 0.1.1\r\nTest version.");
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

        private void lstSpecies_ItemDrag(object sender, ItemDragEventArgs e)
        {
            if (e.Item != null && ((ListViewItem) e.Item).Tag is Compound)
                DoDragDrop(((ListViewItem)e.Item).Tag, DragDropEffects.Link);
        }

        private void treeFiles_ItemDrag(object sender, ItemDragEventArgs e)
        {
            if ((e.Item != null) && ((TreeNode)e.Item).Tag is SimpleReaction)
                DoDragDrop(((TreeNode)e.Item).Tag, DragDropEffects.Copy);
        }

        private void treeFiles_DragOver(object sender, DragEventArgs e)
        {
            if (!e.Data.GetDataPresent(typeof(SimpleReaction)))
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            SimpleReaction rxn = (SimpleReaction)e.Data.GetData(typeof(SimpleReaction));
            TreeViewHitTestInfo hti = treeFiles.HitTest(treeFiles.PointToClient(new Point(e.X, e.Y)));
            if (hti.Node == null)
                e.Effect = DragDropEffects.None;
            else if (hti.Node.Tag is FrmReaction)
                e.Effect = ((FrmReaction)hti.Node.Tag).ContainsReaction(rxn) ? DragDropEffects.None : DragDropEffects.Copy;
            else if (hti.Node.Parent != null && hti.Node.Parent.Tag is FrmReaction)
                e.Effect = ((FrmReaction)hti.Node.Parent.Tag).ContainsReaction(rxn) ? DragDropEffects.None : DragDropEffects.Copy;
        }

        private void treeFiles_DragDrop(object sender, DragEventArgs e)
        {
            if ((e.AllowedEffect & DragDropEffects.Copy) == 0)
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            if (!e.Data.GetDataPresent(typeof(SimpleReaction)))
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            SimpleReaction rxn = (SimpleReaction)e.Data.GetData(typeof(SimpleReaction));
            TreeViewHitTestInfo hti = treeFiles.HitTest(treeFiles.PointToClient(new Point(e.X, e.Y)));
            if (hti.Node == null)
                e.Effect = DragDropEffects.None;
            else if (hti.Node.Tag is FrmReaction && !((FrmReaction)hti.Node.Tag).ContainsReaction(rxn))
            {
                e.Effect = DragDropEffects.Copy;
                ((FrmReaction)hti.Node.Tag).AddReaction(rxn, -1);
            }
            else if (hti.Node.Parent != null && hti.Node.Parent.Tag is FrmReaction && !((FrmReaction)hti.Node.Parent.Tag).ContainsReaction(rxn))
            {
                e.Effect = DragDropEffects.Copy;
                ((FrmReaction)hti.Node.Parent.Tag).AddReaction(rxn, -1);
            }
            else
                e.Effect = DragDropEffects.None;

        }

        private void editToolStripMenuItem_DropDownOpening(object sender, EventArgs e)
        {

            if (!(ActiveMdiChild is FrmReaction))
                menuCopy.Enabled = menuCut.Enabled = menuPaste.Enabled = false;
            else
            {
                menuCopy.Enabled = menuCut.Enabled = ((FrmReaction)ActiveMdiChild).CanCutCopy;
                menuPaste.Enabled = ((FrmReaction)ActiveMdiChild).CanPaste;
            }

        }

        private void menuPaste_Click(object sender, EventArgs e)
        {
            if (ActiveMdiChild is FrmReaction)
                ((FrmReaction)ActiveMdiChild).Paste();
        }

        private void menuCopy_Click(object sender, EventArgs e)
        {
            if (ActiveMdiChild is FrmReaction)
                ((FrmReaction)ActiveMdiChild).Copy();
        }

        private void menuCut_Click(object sender, EventArgs e)
        {
            if (ActiveMdiChild is FrmReaction)
                ((FrmReaction)ActiveMdiChild).Cut();
        }

        private void logToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ((FrmLog)Log).Activate();
        }

        private void unloadAllSpeciesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Are you sure you wish to unload all specie data from all databases?", "Unload All", MessageBoxButtons.YesNo) == DialogResult.Yes)
            {
                UnloadDatabase();
                Log.Active = false;
                DoDatabaseChanged();
                regKey.SetValue("Last Database", "");
                Log.Active = true;
            }
        }

        private void menuOpenDir_Click(object sender, EventArgs e)
        {
            if (folderBrowserDialog1.ShowDialog(this) != DialogResult.OK)
                return;
            if (File.Exists(folderBrowserDialog1.SelectedPath + "\\SpecieData.ini"))
            {
                int dbOpen = 0; bool reloadRequired = false;
                foreach (TreeNode tn in treeFiles.Nodes["SpecieDB"].Nodes)
                    if (tn.Text == folderBrowserDialog1.SelectedPath + "\\SpecieData.ini")
                        dbOpen = 1;
                if (treeFiles.Nodes["SpecieDB"].Nodes.Count > dbOpen && MessageBox.Show("Unload existing specie databases before loading new database?", "Open Directory", MessageBoxButtons.YesNo) == DialogResult.Yes)
                {
                    reloadRequired = true;
                    UnloadDatabase();
                }
                if (dbOpen == 0 || reloadRequired)
                {
                    OpenSpecieDB(folderBrowserDialog1.SelectedPath + "\\SpecieData.ini");
                    DoDatabaseChanged();
                }
            }
            foreach (string fn in Directory.GetFiles(folderBrowserDialog1.SelectedPath, "*.rct", SearchOption.TopDirectoryOnly))
                Open(fn);
        }
    }
}