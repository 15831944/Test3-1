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
using System.Collections;
using Mehroz;
using System.Reflection;

namespace Reaction_Editor
{
    public partial class FrmMain : Form
    {
        #region Internal Variables
        protected int m_nUntitledNo = 1;
        protected RegistryKey regKey = Registry.CurrentUser.CreateSubKey("Software").CreateSubKey("Kenwalt").CreateSubKey("SysCAD Reaction Editor");
        protected List<ToolStripMenuItem> m_RecentFiles = new List<ToolStripMenuItem>();
        protected int m_nRecentFileCount = 6;

        protected static Regex s_RctRegex = new Regex(@"$(?<Filename>.*\.rct)^", RegexOptions.IgnoreCase);
        protected static Regex s_SpecieRegex = new Regex(@"$(?<Filename>.*\.ini)^", RegexOptions.IgnoreCase);
        protected static Regex s_DirRegex = new Regex(@"$(?<Filename>.*\\)^", RegexOptions.IgnoreCase);

        protected static Regex s_ElementRegex = new Regex(
            @"^E\d{3}=(?<Sym>[^,\s]{1,3})\s*,\s*(?<AtmNo>\d{1,3})\s*,\s*(?<AtmWt>(\d+(\.\d+)?|\.\d+))\s*$",
            RegexOptions.Compiled | RegexOptions.Multiline | RegexOptions.ExplicitCapture);
        protected static Regex s_CompoundRegex = new Regex(
            @"^S(?<Index>\d{3})=Specie\s*,\s*(?<Name>[^\r\n,]*)\s*,\s*(?<Sym>[^\s,]*)\s*,\s*(?<Phase>Solid|Liquid|Vapour)\s*,\s*(?<MolWt>(\d+(.\d+)?|.\d+))\s*,\s*(?<HfOK>[^,\s]*)\s*,\s*(?<CpOK>[^,\s])([^,\r\n]*,){5}(\s*,\s*(?<ElementSym>[^,\r\n\s]*)\s*,\s*(?<ElemCount>\d*(\.\d+)?)\s*)+$",
            RegexOptions.Compiled | RegexOptions.Multiline | RegexOptions.ExplicitCapture);
        protected static Regex s_GroupRegex = new Regex(
            @"^S(?<Index>\d{3})=Annotation\s*,\s*(?<Name>[^\r\n,]*)",
            RegexOptions.Compiled | RegexOptions.Multiline | RegexOptions.ExplicitCapture);
        #endregion Internal Variables

        protected ILog Log { get { return Program.Log; } }

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

                List<int> groupIndices = new List<int>();
                Dictionary<int, string> groupNames = new Dictionary<int, string>();
                for (Match m = s_GroupRegex.Match(db); m.Success; m = m.NextMatch())
                {
                    int i = int.Parse(m.Groups["Index"].Value);
                    groupIndices.Add(i);
                    groupNames.Add(i, m.Groups["Name"].Value);
                    if (lstSpecies.Groups[m.Groups["Name"].Value] == null)
                        lstSpecies.Groups.Add(m.Groups["Name"].Value, m.Groups["Name"].Value);
                }
                groupIndices.Sort();


                for (Match m = s_CompoundRegex.Match(db); m.Success; m = m.NextMatch())
                {
                    Compound comp = new Compound();
                    comp.Name = m.Groups["Name"].Value;
                    comp.Symbol = m.Groups["Sym"].Value;
                    comp.Index = int.Parse(m.Groups["Index"].Value);
                    int HfOK, CpOK;
                    int.TryParse(m.Groups["HfOK"].Value, out HfOK);
                    int.TryParse(m.Groups["CpOK"].Value, out CpOK);
                    comp.HeatOK = HfOK != 0 && CpOK != 0;
                    int j = 0;
                    while (j < groupIndices.Count && groupIndices[j] < comp.Index)
                        j++;
                    if (j != 0)
                        j--;
                    if (groupNames.Count > 0)//To stop the program from simply crashing.
                        comp.Annotation = groupNames[groupIndices[j]]; 

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
                                new MessageSource(filename));
                        }
                        comp.Elements.Add(Element.ElementList[sym],
                                          Fraction.ToFraction(m.Groups["ElemCount"].Captures[i].Value));
                    }

                    if (Compound.AddCompound(comp))
                    {
                        specieCount++;
                    }
                }
                txtFilter_TextChanged(this, new EventArgs());
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

        protected void DoDatabaseChanged()
        {
            foreach (Form f in MdiChildren)
                if (f is FrmReaction)
                    ((FrmReaction)f).DoDatabaseChanged();
            Program.FrmAutobalanceExtraComps.UpdateAutocomplete();
        }

        protected bool CheckSpecieDB(string filename)
        {
            bool bDBLoaded = false;
            foreach (TreeNode n in treeFiles.Nodes["SpecieDB"].Nodes)
                if (Path.GetDirectoryName(n.Text) == Path.GetDirectoryName(filename))
                {
                    bDBLoaded = true;
                    break;
                }
            if (!bDBLoaded)
            {
                switch (MessageBox.Show(this, "No specie database loaded from same folder as reaction. Open database from reaction folder?", "Open", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Warning, MessageBoxDefaultButton.Button1))
                {
                    case DialogResult.Yes:
                        if (!File.Exists(Path.GetDirectoryName(filename) + "\\SpecieData.ini") && !File.Exists(Path.GetDirectoryName(filename) + "\\Reactions\\SpecieData.ini"))
                            MessageBox.Show(this, "No specie database found in reaction folder", "Open", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        else
                        {
                            if (treeFiles.Nodes["SpecieDB"].Nodes.Count > 0 && MessageBox.Show(this, "Unload existing specie databases before loading new database?", "Open", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
                                UnloadDatabase();
                            if (File.Exists(Path.GetDirectoryName(filename) + "\\SpecieData.ini"))
                                OpenSpecieDB(Path.GetDirectoryName(filename) + "\\SpecieData.ini");
                            else
                                OpenSpecieDB(Path.GetDirectoryName(filename) + "\\Reactions\\SpecieData.ini");
                        }
                        break;
                    case DialogResult.No:
                        break;
                    case DialogResult.Cancel:
                        return false;
                }
            }
            return true;
        }

        protected void Open(string filename)
        {
            try
            {
                Form f = AlreadyOpen(filename);
                if (f != null)
                {
                    f.Activate();
                    return;
                }
                FrmReaction frm = new FrmReaction(filename, Log);
                RegisterForm(frm);
                RegisterRecentFile(filename);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Open file", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Log.Message(ex.Message, MessageType.Error);
            }
        }


        protected void RegisterRecentFile(string filename)
        {
            bool bAlreadyContained = false;
            int insertionLoc = menuFile.DropDown.Items.IndexOf(menuRecentFileSeperator) + 1;

            foreach (ToolStripMenuItem item in m_RecentFiles)
                if ((string)item.Tag == filename)
                {
                    bAlreadyContained = true;
                    break;
                }
            if (m_RecentFiles.Count == 0)
            {
                menuFile.DropDown.Items.Insert(insertionLoc, new ToolStripSeparator());
            }
            if (!bAlreadyContained)
            {
                ToolStripMenuItem recentFile = new ToolStripMenuItem(Path.GetFileName(filename));
                recentFile.Tag = filename;
                recentFile.Click += new EventHandler(recentFile_Click);
                m_RecentFiles.Add(recentFile);
                menuFile.DropDown.Items.Insert(insertionLoc, recentFile);
                if (m_RecentFiles.Count > m_nRecentFileCount)
                {
                    menuFile.DropDown.Items.Remove(m_RecentFiles[0]);
                    m_RecentFiles.RemoveAt(0);
                }
            }
            else
            {
                ToolStripMenuItem recentFile = null;
                foreach (ToolStripMenuItem item in m_RecentFiles)
                    if ((string)item.Tag == filename)
                    {
                        recentFile = item;
                        break;
                    }
                m_RecentFiles.Remove(recentFile);
                m_RecentFiles.Add(recentFile);
                menuFile.DropDown.Items.Remove(recentFile);
                menuFile.DropDown.Items.Insert(insertionLoc, recentFile);
            }
            for (int i = 0; i < m_RecentFiles.Count; i++)
                m_RecentFiles[i].Text = "&" + (m_RecentFiles.Count - i) + " " + m_RecentFiles[i].Tag;
        }

        protected void RegisterForm(FrmReaction frm)
        {
            if (Program.FrmAutobalanceExtraComps.Visible)
                frm.Enabled = false;
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
            frm.NowChanged += new EventHandler(frm_NowChanged);
            frm.CompoundsChanged += new EventHandler(frm_CompoundsChanged);
            frm.SelectedReactionChanged += new EventHandler(frm_SelectedReactionChanged);
            frm.SourcesSinksChanged += new EventHandler(frm_SourcesSinksChanged);
            frm.ReactionAdded += new FrmReaction.ReactionHandler(frm_ReactionAdded);
            frm.ReactionRemoved += new FrmReaction.ReactionHandler(frm_ReactionRemoved);

            frm.FormClosing += new FormClosingEventHandler(frm_FormClosing);
            frm.FormClosed += new FormClosedEventHandler(frm_FormClosed);
            frm.RecheckCutCopyPaste += new EventHandler(frm_RecheckCutCopyPaste);
            frm.ReactionChanged += new FrmReaction.ReactionHandler(frm_ReactionChanged);
        }

        protected void Save(FrmReaction target)
        {
            try
            {
                if (!target.FileOpen)
                    SaveAs(target);
                else
                {
                    target.Save();
                    RegisterRecentFile(target.Filename);
                }
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
                RegisterRecentFile(target.Filename);
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

        protected void UpdateUsedSpecies()
        {
            List<Compound> compsInUse;
            List<Compound> compsInReaction;
            List<Compound> compsInSource;
            List<Compound> compsInSink;
            if (ActiveMdiChild is FrmReaction)
            {
                compsInUse = ((FrmReaction)ActiveMdiChild).GetCompounds();
                compsInReaction = ((FrmReaction)ActiveMdiChild).CurrentReactionCompounds;
                compsInSource = ((FrmReaction)ActiveMdiChild).SourceCompounds;
                compsInSink = ((FrmReaction)ActiveMdiChild).SinkCompounds;
            }
            else
            {
                compsInSource = compsInSink = compsInReaction = compsInUse = new List<Compound>();
            }
            lstSpecies.BeginUpdate();
            foreach (ListViewItem lvi in lstSpecies.Items)
            {
                if (compsInUse.Contains((Compound)lvi.Tag))
                    lvi.Font = new Font(lvi.Font, FontStyle.Bold);
                else
                    lvi.Font = new Font(lvi.Font, FontStyle.Regular);

                if (compsInReaction.Contains((Compound)lvi.Tag))
                    lvi.ForeColor = Color.Blue;
                else
                    lvi.ForeColor = System.Drawing.SystemColors.WindowText;

                if (compsInSource.Contains((Compound)lvi.Tag) && compsInSink.Contains((Compound)lvi.Tag))
                    lvi.ImageKey = "PlusMinus";
                else if (compsInSource.Contains((Compound)lvi.Tag))
                    lvi.ImageKey = "Plus";
                else if (compsInSink.Contains((Compound)lvi.Tag))
                    lvi.ImageKey = "Minus";
                else
                    lvi.ImageKey = null;
            }
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
            foreach(ToolStripMenuItem item in m_RecentFiles)
                regKey.SetValue("RecentFile" + i++, item.Tag);
        }

        protected void UpdateToolbar()
        {
            if (this.MdiChildren.Length == 0)
                btnSaveAll.Enabled = false;
            else
                btnSaveAll.Enabled = true;
            if (!(ActiveMdiChild is FrmReaction))
            {
                btnSave.Enabled = btnCut.Enabled = btnCopy.Enabled = btnPaste.Enabled = false;
                return;
            }
            FrmReaction frm = (FrmReaction)ActiveMdiChild;
            btnSave.Enabled = frm.Changed;
            btnCopy.Enabled = btnCut.Enabled = frm.CanCutCopy;
            btnPaste.Enabled = frm.CanPaste;
            btnRevert.Enabled = frm.CanRevert;
        }
        #endregion

        public FrmMain()
        {
            InitializeComponent();
            treeFiles.Nodes["SpecieDB"].ContextMenuStrip = menuDatabaseFile;
            //ResourceReader rr = new ResourceReader(Assembly.GetExecutingAssembly().GetManifestResourceStream(typeof(FrmMain), "Icons.resources"));
            lstSpecies.SmallImageList = Program.Images;
            Program.Log = new ListLog(lstLog);

            string[] valueNames = regKey.GetValueNames();
            Array.Sort<String>(valueNames);
            List<ToolStripMenuItem> recentList = new List<ToolStripMenuItem>();
            foreach (string s in valueNames)
                if (recentFileRegex.Match(s).Success)
                {
                    if (m_RecentFiles.Count >= m_nRecentFileCount)
                        break; //Don't load more than [m_nRecentFileCount] files.
                    RegisterRecentFile(regKey.GetValue(s).ToString());
                }

            Program.FrmAutobalanceExtraComps = new FrmAutobalanceExtraComps(regKey.CreateSubKey("Autobalance Sets"));

            Program.FrmAutobalanceExtraComps.VisibleChanged += new EventHandler(FrmAutobalanceExtraComps_VisibleChanged);

            UpdateToolbar();

            HandleArgs(Program.Args);
            Program.interopMessenger.StringSent += new InteropMessenger.StringSentDelegate(HandleArgs);
        }

        void HandleArgs(string[] args)
        {
            if (this.InvokeRequired)
            {
                this.BeginInvoke(new InteropMessenger.StringSentDelegate(HandleArgs), new object[] { args });
                return;
            }
            string lastPath = "My Documents";
            try
            {
                lastPath = (string)regKey.GetValue("Last Folder", "My Documents");
            }
            catch { }

            bool bDBOpen = treeFiles.Nodes["SpecieDB"].Nodes.Count > 0;
            bool bAll = false;
            foreach (string s in args)
                if (s.Trim().ToLowerInvariant() == "/all")
                    bAll = true;

            //Check for a specified specieDB:
            foreach (string s in args)
            {
                try
                {
                    if (Path.GetExtension(s).ToLowerInvariant() == ".ini")
                    {
                        lastPath = Path.GetDirectoryName(Path.GetFullPath(s));
                        if (lastPath == "") lastPath = Path.GetPathRoot(Path.GetFullPath(s));
                        OpenSpecieDB(s);
                        bDBOpen = true;
                        break;
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.ToString());
                }
            }

            //Check for a specified folder with a specieDB:
            if (!bDBOpen)
            {
                foreach (string s in args)
                    try
                    {
                        if (File.Exists(Path.Combine(Path.GetDirectoryName(s), "speciedata.ini")))
                        {
                            lastPath = Path.GetDirectoryName(Path.GetFullPath(s));
                            if (lastPath == "") lastPath = Path.GetPathRoot(Path.GetFullPath(s));
                            OpenSpecieDB(Path.Combine(Path.GetDirectoryName(s), "speciedata.ini"));
                            bDBOpen = true;
                            break;
                        }
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show(ex.ToString());
                    }
            }

            //Load any folders specified:
            foreach (string s in args)
            {
                try
                {
                    string s2 = bAll && Path.GetExtension(s).ToLowerInvariant() == ".rct" ?
                        Path.GetDirectoryName(s) : s;
                    if (Directory.Exists(s2))
                    {
                        lastPath = Path.GetDirectoryName(Path.GetFullPath(s2));
                        if (lastPath == "") lastPath = Path.GetPathRoot(Path.GetFullPath(s2));
                        foreach (string f in Directory.GetFiles(s2, "*.rct"))
                            Open(f);
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.ToString());
                }
            }

            //Load any reactions:
            foreach (string s in args)
            {
                try
                {
                    if (Path.GetExtension(s).ToLowerInvariant() == ".rct")
                    {
                        lastPath = Path.GetDirectoryName(Path.GetFullPath(s));
                        if (lastPath == "") lastPath = Path.GetPathRoot(Path.GetFullPath(s));
                        Open(s);
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.ToString());
                }
            }

            UpdateLastPath(lastPath);
            this.Activate();
        }

        protected void UpdateLastPath(string lastPath)
        {
            regKey.SetValue("Last Folder", lastPath);
            dlgOpenDB.InitialDirectory = lastPath;
            dlgOpenRxn.InitialDirectory = lastPath;
            dlgSaveRxn.InitialDirectory = lastPath;
            dlgOpenFolder.SelectedPath = lastPath;
        }

        void FrmAutobalanceExtraComps_VisibleChanged(object sender, EventArgs e)
        {
            if (!Program.FrmAutobalanceExtraComps.Modal && Program.FrmAutobalanceExtraComps.Visible)
                foreach (Form f in this.MdiChildren)
                    if (f != Program.FrmAutobalanceExtraComps)
                        f.Enabled = false;
            if (!Program.FrmAutobalanceExtraComps.Visible)
                foreach (Form f in this.MdiChildren)
                    if (f != Program.FrmAutobalanceExtraComps)
                        f.Enabled = true;
        }

        void frm_RecheckCutCopyPaste(object sender, EventArgs e)
        {
            if (sender == ActiveMdiChild)
                UpdateToolbar();
        }

        void frm_NowChanged(object sender, EventArgs e)
        {
            //if (sender == ActiveMdiChild)
            //    m_StatusLabel.Text = ((FrmReaction)sender).StatusMessage;
            FrmReaction frm = sender as FrmReaction;
            if (frm.Changed)
            {
                TreeNode frmNode = (TreeNode)frm.Tag;
                frmNode.ForeColor = Color.Blue;
                foreach (TreeNode t in frmNode.Nodes)
                    if (((SimpleReaction)t.Tag).HasChanged)
                        t.ForeColor = Color.Blue;
                    else
                        t.ForeColor = SystemColors.WindowText;
            }
            else
                ((TreeNode)frm.Tag).ForeColor = SystemColors.WindowText;
        }

        void frm_CompoundsChanged(object sender, EventArgs e)
        {
            if (sender == ActiveMdiChild)
                UpdateUsedSpecies();
        }

        void frm_ReactionRemoved(FrmReaction form, SimpleReaction reaction)
        {
            if (form == ActiveMdiChild)
                UpdateUsedSpecies();
            TreeNode t = FindTreeNode(form, reaction);
            if (t != null) t.Remove();
        }

        protected TreeNode FindTreeNode(FrmReaction form, SimpleReaction reaction)
        {
            TreeNode f = (TreeNode)form.Tag;
            foreach (TreeNode r in f.Nodes)
                if (r.Tag == reaction)
                    return r;
            return null;
        }

        void frm_ReactionAdded(FrmReaction form, SimpleReaction reaction)
        {
            if (form == ActiveMdiChild)
                UpdateUsedSpecies();
            TreeNode newNode = new TreeNode(reaction.ToString());
            newNode.Tag = reaction;
            ((TreeNode)form.Tag).Nodes.Add(newNode);
        }

        void frm_ReactionChanged(FrmReaction form, SimpleReaction reaction)
        {
            TreeNode frmNode = (TreeNode)form.Tag;
            TreeNode n = FindTreeNode(form, reaction);
            if (n != null)
                n.Text = reaction.ToString();
        }

        void frm_SelectedReactionChanged(object sender, EventArgs e)
        {
            if (sender == ActiveMdiChild)
            {
                UpdateUsedSpecies();
                m_StatusLabel.Text = ((FrmReaction)sender).StatusMessage;
            }
        }

        void frm_SourcesSinksChanged(object sender, EventArgs e)
        {
            if (sender == ActiveMdiChild)
                UpdateUsedSpecies();
        }

        Regex recentFileRegex = new Regex("RecentFile\\d+");

        void recentFile_Click(object sender, EventArgs e)
        {
            if (CheckSpecieDB((string)((ToolStripMenuItem)sender).Tag))
                Open((string)((ToolStripMenuItem)sender).Tag);
        }

        private void menuOpen_Click(object sender, EventArgs e)
        {
            if (dlgOpenRxn.ShowDialog(this) != DialogResult.OK)
                return;
            
            if (CheckSpecieDB(dlgOpenRxn.FileName))
                foreach (string file in dlgOpenRxn.FileNames)
                    Open(file);
        }

        Form AlreadyOpen(string filename)
        {
            foreach (Form f in this.MdiChildren)
                if (f is FrmReaction && ((FrmReaction)f).Filename == filename)
                    return f;
            return null;
        }

        void frm_FormClosed(object sender, FormClosedEventArgs e)
        {
            TreeNode node = (TreeNode)((Form)sender).Tag;
            if (node.Parent != null)
                node.Parent.Nodes.Remove(node);
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

        private void menuSave_Click(object sender, EventArgs e)
        {
            if (!(this.ActiveMdiChild is FrmReaction))
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
            //Essentially makes the form a modal form, but allows drag-drop from the specie list on the RHS:
            /*if (Program.FrmAutobalanceExtraComps.Visible)
                Program.FrmAutobalanceExtraComps.Activate();*/

            if (ActiveMdiChild is FrmReaction)
                m_StatusLabel.Text = ((FrmReaction)ActiveMdiChild).StatusMessage;
            else
                m_StatusLabel.Text = "";
            UpdateUsedSpecies();
            UpdateToolbar();
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
            //RepositionLog();
        }

        private void undockToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form f = ActiveMdiChild;
            ActiveMdiChild.MdiParent = null;
            f.Owner = this;
        }

        private void aboutSysCADReactionEditorToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string ver = Assembly.GetExecutingAssembly().GetName().Version.ToString(3);
            MessageBox.Show(this, "SysCAD Reaction Editor version " + ver + "\r\nTest version.", "About");
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
                menuRevert.Enabled = 
                ActiveMdiChild is FrmReaction;
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
            dlgOpenFolder.Description = "Select a directory containing the required reaction files and specie database.";
            if (dlgOpenFolder.ShowDialog(this) != DialogResult.OK)
                return;
            try
            {
                UpdateLastPath(dlgOpenFolder.SelectedPath);

                lstLog.BeginUpdate();
                treeFiles.BeginUpdate();
                if (File.Exists(dlgOpenFolder.SelectedPath + "\\SpecieData.ini"))
                {
                    bool dbOpen = false;
                    foreach (TreeNode tn in treeFiles.Nodes["SpecieDB"].Nodes)
                        if (tn.Text == dlgOpenFolder.SelectedPath + "\\SpecieData.ini")
                        {
                            dbOpen = true;
                            break;
                        }
                    if (!dbOpen)
                    {
                        if (treeFiles.Nodes["SpecieDB"].Nodes.Count > 0 && MessageBox.Show("Unload existing specie databases before loading new database?", "Open Directory", MessageBoxButtons.YesNo) == DialogResult.Yes)
                            UnloadDatabase();
                        OpenSpecieDB(dlgOpenFolder.SelectedPath + "\\SpecieData.ini");
                    }
                }
                else
                    MessageBox.Show(this, "Specie database not found in selected directory", "Open Directory", MessageBoxButtons.OK, MessageBoxIcon.Warning);

                foreach (string fn in Directory.GetFiles(dlgOpenFolder.SelectedPath, "*.rct", SearchOption.TopDirectoryOnly))
                    Open(fn);
            }
            finally
            {
                lstLog.EndUpdate();
                treeFiles.EndUpdate();
            }
        }

        private void txtFilter_TextChanged(object sender, EventArgs e)
        {
            lstSpecies.BeginUpdate();
            lstSpecies.Items.Clear();
            lstSpecies.ShowGroups = true;
            string[] tokens = string.IsNullOrEmpty(txtFilter.Text.Trim()) ? new string[] {""} :
                txtFilter.Text.Split(new char[]{' '}, StringSplitOptions.RemoveEmptyEntries);
            foreach (Compound c in Compound.CompoundList.Values)
                foreach (string s in tokens)
                    if (c.Symbol.Contains(s))
                    {
                        ListViewItem lvi = new ListViewItem(new string[] { c.Symbol, c.Name });
                        lstSpecies.Items.Add(lvi);
                        lvi.ToolTipText = c.Name;
                        lvi.Group = lstSpecies.Groups[c.Annotation];
                        lvi.Tag = c;
                        continue;
                    }
            lstSpecies.ShowGroups = menuSortByPhase.Checked;
            if (!string.IsNullOrEmpty(txtFilter.Text.Trim()))
                txtFilter.BackColor = Color.Pink;
            else
                txtFilter.BackColor = System.Drawing.SystemColors.Window;

            UpdateUsedSpecies();
            lstSpecies.EndUpdate();
        }

        private void menuFile_DropDownOpening(object sender, EventArgs e)
        {
            menuSave.Enabled = menuSaveAs.Enabled = menuClose.Enabled = ActiveMdiChild is FrmReaction;
            bool anyFilesOpen = false;
            foreach (Form f in MdiChildren)
                if (f is FrmReaction)
                {
                    anyFilesOpen = true;
                    break;
                }
            menuSaveAll.Enabled = anyFilesOpen;
        }

        private void grpSpecies_Resize(object sender, EventArgs e)
        {
            btnCollapseRight.Left = grpSpecies.Left - btnCollapseRight.Width;
            btnCollapseRight.Top = grpSpecies.Top + (grpSpecies.Height - btnCollapseRight.Height) / 2;
        }

        int m_nOldGrpSpeciesWidth;
        private void btnCollapseRight_Click(object sender, EventArgs e)
        {
            if (!splitterRight.Enabled)
            {
                grpSpecies.Width = m_nOldGrpSpeciesWidth;
                splitterRight.Enabled = true;
            }
            else
            {
                m_nOldGrpSpeciesWidth = grpSpecies.Width;
                grpSpecies.Width = 0;
                splitterRight.Enabled = false;
            }
            grpSpecies_Resize(sender, e);
        }

        private void grpFiles_Resize(object sender, EventArgs e)
        {
            btnCollapseLeft.Left = grpFiles.Right;
            btnCollapseLeft.Top = grpFiles.Top + (grpFiles.Height - btnCollapseRight.Height) / 2;

        }

        protected int m_nOldGrpFilesWidth;
        private void btnCollapseLeft_Click(object sender, EventArgs e)
        {
            if (!splitterLeft.Enabled)
            {
                grpFiles.Width = m_nOldGrpFilesWidth;
                splitterLeft.Enabled = true;
            }
            else
            {
                m_nOldGrpFilesWidth = grpFiles.Width;
                grpFiles.Width = 0;
                splitterLeft.Enabled = false;
            }
            grpFiles_Resize(sender, e);
        }

        static System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FrmMain));
        protected int m_nOldPnlLogHeight;
        private void btnLogCollapse_Click(object sender, EventArgs e)
        {
            if (splitterLog.Enabled)
            {
                m_nOldPnlLogHeight = pnlLog.Height;
                pnlLog.Height = pnlLogHeader.Height + 4;
                lstLog.Enabled = splitterLog.Enabled = false;
                btnLogCollapse.BackgroundImage = Reaction_Editor.Properties.Resources.UpArrow;
                if (ActiveMdiChild != null && ActiveMdiChild.ActiveControl != null)
                    ActiveMdiChild.ActiveControl.Focus();
            }
            else
            {
                pnlLog.Height = m_nOldPnlLogHeight;
                lstLog.Enabled = splitterLog.Enabled = true;
                btnLogCollapse.BackgroundImage = Reaction_Editor.Properties.Resources.DownArrow;
                pnlLog.Focus();
            }
        }

        private void splitterRight_DoubleClick(object sender, EventArgs e)
        {
            chName.AutoResize(ColumnHeaderAutoResizeStyle.ColumnContent);
            chSymbol.AutoResize(ColumnHeaderAutoResizeStyle.ColumnContent);
            //chSymbol.Width += 16; //To account for images
            grpSpecies.Width = (chName.Width + chSymbol.Width + (lstSpecies.Width - lstSpecies.ClientSize.Width) )+ (grpSpecies.Width - grpSpecies.ClientSize.Width);
            grpSpecies_Resize(sender, e);
        }

        protected FrmReaction LastChild = null;
        private void pnlLog_Enter(object sender, EventArgs e)
        {
            btnLogCollapse.FlatAppearance.BorderColor = btnLogCollapse.BackColor = pnlLogHeader.BackColor = SystemColors.ActiveCaption;
            lblLogHeader.ForeColor = SystemColors.ActiveCaptionText;
            if (ActiveMdiChild != null)
                Messaging.SendMessage(ActiveMdiChild.Handle, Messaging.WM_NCACTIVATE, (IntPtr) 0, (IntPtr)0);
            if (ActiveMdiChild is FrmReaction)
                (LastChild = ((FrmReaction)ActiveMdiChild)).ActualActive = false;
        }

        private void pnlLog_Leave(object sender, EventArgs e)
        {
            btnLogCollapse.FlatAppearance.BorderColor = btnLogCollapse.BackColor = pnlLogHeader.BackColor = SystemColors.InactiveCaption;
            lblLogHeader.ForeColor = SystemColors.InactiveCaptionText;
            if (ActiveMdiChild != null)
                Messaging.SendMessage(ActiveMdiChild.Handle, Messaging.WM_NCACTIVATE, (IntPtr)1, (IntPtr)0);
            if (LastChild is FrmReaction)
            {
                ((FrmReaction)LastChild).ActualActive = true;
                LastChild = null;
            }
        }

        private void pnlLogHeader_Click(object sender, EventArgs e)
        {
            if (splitterLog.Enabled)
                ((Control)sender).Select();
        }

        private void splitterRight_Move(object sender, EventArgs e)
        {
            grpSpecies_Resize(sender, e);
        }

        private void menuSpecieList_Opening(object sender, CancelEventArgs e)
        {
            if (lstSpecies.SelectedItems.Count == 0 || !(ActiveMdiChild is FrmReaction))
            {
                menuSetSink.Enabled =
                    menuSetSource.Enabled =
                    menuRemoveSink.Enabled =
                    menuRemoveSource.Enabled = false;
            }
            else
            {
                List<Compound> sources = ((FrmReaction)ActiveMdiChild).SourceCompounds;
                List<Compound> sinks = ((FrmReaction)ActiveMdiChild).SinkCompounds;
                bool anySinks = false;
                bool anySources = false;
                bool anyNonSinks = false;
                bool anyNonSources = false;
                foreach (ListViewItem lvi in lstSpecies.SelectedItems)
                {
                    if (sources.Contains((Compound)lvi.Tag))
                        anySources = true;
                    else
                        anyNonSources = true;
                    if (sinks.Contains((Compound)lvi.Tag))
                        anySinks = true;
                    else
                        anyNonSinks = true;
                }
                menuSetSink.Enabled = anyNonSinks;
                menuSetSource.Enabled = anyNonSources;
                menuRemoveSink.Enabled = anySinks;
                menuRemoveSource.Enabled = anySources;
            }
        }

        private void menuSetSource_Click(object sender, EventArgs e)
        {
            List<Compound> comps = new List<Compound>();
            foreach (ListViewItem lvi in lstSpecies.SelectedItems)
                comps.Add((Compound)lvi.Tag);
            ((FrmReaction)ActiveMdiChild).AddSources(comps.ToArray());
        }

        private void menuSetSink_Click(object sender, EventArgs e)
        {
            List<Compound> comps = new List<Compound>();
            foreach (ListViewItem lvi in lstSpecies.SelectedItems)
                comps.Add((Compound)lvi.Tag);
            ((FrmReaction)ActiveMdiChild).AddSinks(comps.ToArray());
        }

        private void menuRemoveSource_Click(object sender, EventArgs e)
        {
            List<Compound> comps = new List<Compound>();
            foreach (ListViewItem lvi in lstSpecies.SelectedItems)
                comps.Add((Compound)lvi.Tag);
            ((FrmReaction)ActiveMdiChild).RemoveSources(comps.ToArray());
        }

        private void menuRemoveSink_Click(object sender, EventArgs e)
        {
            List<Compound> comps = new List<Compound>();
            foreach (ListViewItem lvi in lstSpecies.SelectedItems)
                comps.Add((Compound)lvi.Tag);
            ((FrmReaction)ActiveMdiChild).RemoveSinks(comps.ToArray());
        }

        private void menuSortAlphabetically_CheckedChanged(object sender, EventArgs e)
        {
            if (menuSortByPhase.Checked && menuSortAlphabetically.Checked)
                menuSortByPhase.Checked = false;
            if (menuSortAlphabetically.Checked)
                lstSpecies.Sorting = SortOrder.Ascending;
            else
            {
                lstSpecies.Sorting = SortOrder.None;
                txtFilter_TextChanged(sender, e);
            }
        }

        private void menuSortByPhase_CheckedChanged(object sender, EventArgs e)
        {
            if (menuSortAlphabetically.Checked && menuSortByPhase.Checked)
                menuSortAlphabetically.Checked = false;
            lstSpecies.ShowGroups = menuSortByPhase.Checked;
        }

        private void btnRevert_Click(object sender, EventArgs e)
        {
            if (ActiveMdiChild is FrmReaction)
                ((FrmReaction)ActiveMdiChild).RevertCurrent();
        }

        private void menuAutoCompleteSets_Click(object sender, EventArgs e)
        {
            Program.FrmAutobalanceExtraComps.Show();
            Program.FrmAutobalanceExtraComps.MdiParent = this;
            /*FrmAutobalanceExtraComps temp = new FrmAutobalanceExtraComps(Program.FrmAutobalanceExtraComps);
            if (temp.ShowDialog(this) == DialogResult.OK)
            {
                Program.FrmAutobalanceExtraComps.Dispose();
                Program.FrmAutobalanceExtraComps = temp;
            }
            else
                temp.Dispose();*/
        }

        protected override void OnDragDrop(DragEventArgs drgevent)
        {
            if (drgevent.Data.GetDataPresent(typeof(CompoundDrag)) &&
                ((CompoundDrag)drgevent.Data.GetData(typeof(CompoundDrag))).frm.MdiParent == this)
            {
                CompoundDrag data = (CompoundDrag) drgevent.Data.GetData(typeof(CompoundDrag));
                data.frm.DropCompound(data);
                drgevent.Effect = DragDropEffects.Move;
            }

            base.OnDragDrop(drgevent);
        }

        protected override void OnDragEnter(DragEventArgs drgevent)
        {
            if (drgevent.Data.GetDataPresent(typeof(CompoundDrag)) &&
                ((CompoundDrag)drgevent.Data.GetData(typeof(CompoundDrag))).frm.MdiParent == this)
                drgevent.Effect = DragDropEffects.Move;
            else
                drgevent.Effect = DragDropEffects.None;

            base.OnDragEnter(drgevent);
        }

        private void menuRevert_Click(object sender, EventArgs e)
        {
            string fn = ((FrmReaction)ActiveMdiChild).Filename;
            if (MessageBox.Show("Are you sure you wish to revert to the saved version of \"" + fn + "\"?", "Revert", MessageBoxButtons.YesNo, MessageBoxIcon.Warning) == DialogResult.No)
                return;
            ActiveMdiChild.Close();
            Open(fn);
        }

        List<ToolStripMenuItem> m_WindowList = new List<ToolStripMenuItem>();
        private void windowToolStripMenuItem_Open(object sender, EventArgs e)
        {
            for (int i = m_WindowList.Count - 1; i >= 0; i--)
            {
                if (((Form)m_WindowList[i].Tag).IsDisposed || ((Form)m_WindowList[i].Tag).MdiParent != this)
                {
                    windowToolStripMenuItem.DropDown.Items.Remove(m_WindowList[m_WindowList.Count - 1]);
                    windowToolStripMenuItem.DropDown.Items.Remove(m_WindowList[i]);
                    m_WindowList.RemoveAt(i);
                }
            }
            foreach (Form f in MdiChildren)
            {
                bool contains = false;
                foreach (ToolStripMenuItem item in m_WindowList)
                    if (item.Tag == f)
                        contains = true;
                if (!contains)
                {
                    ToolStripMenuItem windowMenuItem = new ToolStripMenuItem(f.Text);
                    windowMenuItem.Tag = f;
                    windowMenuItem.Click += new EventHandler(windowMenuItem_Click);
                    m_WindowList.Add(windowMenuItem);
                    windowToolStripMenuItem.DropDown.Items.Add(windowMenuItem);
                }
            }
        }

        void windowMenuItem_Click(object sender, EventArgs e)
        {
            ((Form)((ToolStripMenuItem)sender).Tag).Activate();
        }
    }

    public class ListLog : ILog
    {
        protected ListView listView1;
        public ListLog(ListView lv)
        {
            listView1 = lv;
            listView1.DoubleClick += new EventHandler(listView1_DoubleClick);
            listView1.SmallImageList = Program.Images;
        }

        #region ILog Members
        protected bool m_bActive = true;
        protected Stack<MessageSource> sourceStack = new Stack<MessageSource>();
        protected void AddMessage(string msg, MessageType msgType, MessageSource src)
        {
            if (!m_bActive) return;
            string imageKey = "";
            switch (msgType)
            {
                case MessageType.Error:
                    imageKey = "Error_ico";
                    break;
                case MessageType.Note:
                    imageKey = "Info_ico";
                    break;
                case MessageType.Warning:
                    imageKey = "Warning_ico";
                    break;
            }
            string source = "Global";
            if (src != null)
                source = src.Source;
            ListViewItem lvi = new ListViewItem(new string[] { source, msg }, imageKey);
            lvi.Tag = src;
            listView1.Items.Add(lvi);
            listView1.EnsureVisible(lvi.Index);
            //listView1.AutoScrollOffset = new Point(0);//(lvi.Position);
        }

        public void Message(string msg, MessageType msgType, MessageSource src)
        {
            AddMessage(msg, msgType, src);
        }

        public void Message(string msg, MessageType msgType)
        {
            MessageSource src = null;
            if (sourceStack.Count != 0)
                src = sourceStack.Peek();
            AddMessage(msg, msgType, src);
        }

        public void SetSource(MessageSource src)
        {
            sourceStack.Push(src);
        }

        public void RemoveSource()
        {
            sourceStack.Pop();
        }

        public bool Active
        {
            get { return m_bActive; }
            set { m_bActive = value; }
        }
        #endregion

        private void listView1_DoubleClick(object sender, EventArgs e)
        {
            if (listView1.SelectedItems.Count != 0 &&
                listView1.SelectedItems[0].Tag != null)
            {
                ((MessageSource)listView1.SelectedItems[0].Tag).DoCallback();
            }
        }
    }
}