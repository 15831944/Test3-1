using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using System.IO;
using System.Collections;
using Auto_Complete;
using System.Threading;
using Mehroz;

namespace Reaction_Editor
{
    public partial class FrmReaction : Form
    {
        #region Regex's
        protected static Regex s_LastSelectedRegex = new Regex(@"<LastSelected\s*=\s*(?<Value>\d+|None)>",
            RegexOptions.Compiled | RegexOptions.IgnoreCase);

        protected static Regex s_EndRegex = new Regex(@"(^|\r*\n)\s*End",
            RegexOptions.Compiled | RegexOptions.IgnoreCase | RegexOptions.ExplicitCapture);

        protected static Regex s_CommentRemovingRegex = new Regex(@"^(?<Active>[^;]*)(;(?<Comment>.*))?",
            RegexOptions.ExplicitCapture | RegexOptions.Multiline | RegexOptions.Compiled);

        protected static Regex s_OpeningCommentRegex = new Regex(@"^\s*(?>;(?<Comment>[^\r\n]*)\s*)*",
            RegexOptions.ExplicitCapture | RegexOptions.Compiled); //If the first match is non-zero, discard.

        protected static Regex s_RxnBlockStartRegex = new Regex(@"(^|\r*\n\s*)Reactions\s*:",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled);

        protected static Regex s_RxnBlockEndRegex = new Regex(@"\r*\n\s*(End|^|HeatExchange|Sink)",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled);
                                                                                                  //Maj .Min.Rev.Build
        protected static Regex s_FileVersionRegex = new Regex(@"<ReactionEditorVer\s*=\s*(?<Version>\d+(\.\d+){0,3})>",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled);
        #endregion Regex's

        #region Variables
        protected FrmAddSelector m_AddSelector = new FrmAddSelector();

        protected ColumnHeader m_SequenceHeader = new ColumnHeader();
        protected bool m_bChanged = false;
        protected bool m_bLoading = false;
        protected string m_sTitle = "Untitled Reaction Block";
        protected FileStream m_File;
        protected SimpleReaction m_CurrentReaction;
        protected bool m_bSupressSaveMessage = false;

        protected ILog m_Log;

        protected CompoundListReaction m_Sinks, m_Sources;
        protected HXReaction m_HX;

        protected bool m_bUpdateTextboxes = true;
        protected bool m_bDoEvents = true;
        protected List<Control> m_OptionalExtentControls = new List<Control>();
        protected Control[] m_HXControls;

        public const int sMaxSequences = 32;

        protected ListViewGroup m_selectedGroup = null;

        //It's only ever used in adding a new reaction. And non-software people don't like zero referenced arrays.
        //We should replace this with a better method to determine reaction numbers
        protected int ReactionNo
        {
            get 
            {
                if (m_Sources != null && !m_Sources.Enabled)
                    return lstReactions.Items.Count - 1; 
                else
                    return lstReactions.Items.Count; 
            }
        }

        protected bool m_bActualActive = true;
        List<Auto_Complete.BoxAutoComplete> AutoCompleteBoxes = new List<Auto_Complete.BoxAutoComplete>();
        #endregion Variables

        #region Properties
        public string Filename
        {
            get
            {
                if (m_File != null)
                    return m_File.Name;
                throw new IOException("Attempt to retrieve filename when no file has been opened");
            }
        }

        public bool FileOpen
        {
            get { return m_File != null; }
        }

        public bool Changed
        {
            get { return m_bChanged; }
            protected set 
            { 
                m_bChanged = value;
                Title = Title;
            }
        }

        public string Title
        {
            get { return m_sTitle; }
            set 
            {
                m_sTitle = value;
                Text = m_sTitle + (m_bChanged ? "*" : ""); 
            }
        }

        public List<SimpleReaction> Reactions
        {
            get
            {
                List<SimpleReaction> ret = new List<SimpleReaction>();
                foreach (ListViewItem lvi in lstReactions.Items)
                    if (lvi.Tag is SimpleReaction)
                        ret.Add((SimpleReaction)lvi.Tag);
                return ret;
            }
        }

        public ILog Log
        {
            get { return m_Log; }
            set { m_Log = value; }
        }

        public bool SupressSaveMessages
        {
            get { return m_bSupressSaveMessage; }
            set { m_bSupressSaveMessage = value; }
        }

        protected ListViewGroup SelectedGroup
        {
            set
            {
                if (value == m_selectedGroup) return;
                if (m_selectedGroup != null)
                    m_selectedGroup.HeaderAlignment = HorizontalAlignment.Left;
                m_selectedGroup = value;
                if (m_selectedGroup != null)
                    m_selectedGroup.HeaderAlignment = HorizontalAlignment.Center;
            }
        }

        public bool CanCutCopy
        {
            get
            {
                Control act = ActualActiveControl(this);
                return CanCutCopyReaction ||
                    (act is RichTextBox && !string.IsNullOrEmpty(((RichTextBox)act).SelectedText)) ||
                    (act is TextBoxEx && !string.IsNullOrEmpty(((TextBoxEx)act).SelectedText));
            }
        }

        public bool CanCutCopyReaction
        {
            get { return (lstReactions.Focused && lstReactions.SelectedItems.Count > 0); }
        }

        public bool CanRevert
        {
            get 
            {
                if (lstReactions.SelectedItems.Count == 0)
                    return false;
                if (lstReactions.SelectedItems[0].Tag is SimpleReaction)
                    return m_CurrentReaction != null && m_CurrentReaction.CanRevert && m_CurrentReaction.HasChanged;
                else
                    return ((Reaction)lstReactions.SelectedItems[0].Tag).CanRevert;
            }
        }

        public bool CanPaste
        {
            get
            {
                IDataObject obj = Clipboard.GetDataObject();
                Control act = ActualActiveControl(this);
                return CanPasteReaction ||
                    (Clipboard.ContainsText() && (act is RichTextBox || act is TextBoxEx));
            }
        }

        public bool CanPasteReaction
        {
            get 
            {
                IDataObject obj = Clipboard.GetDataObject();
                return obj.GetDataPresent(typeof(SimpleReaction)); 
            }
        }

        public List<Compound> CurrentReactionCompounds
        {
            get
            {
                if (m_CurrentReaction == null) return new List<Compound>();
                return m_CurrentReaction.Compounds;
            }
        }

        public List<Compound> SourceCompounds { get { return m_Sources == null ? new List<Compound>() : m_Sources.CompoundList; } }

        public List<Compound> SinkCompounds { get { return m_Sinks == null ? new List<Compound>() : m_Sinks.CompoundList; } }

        public string StatusMessage
        {
            get
            {
                if (m_CurrentReaction == null)
                    if (lstReactions.SelectedItems.Count == 0)
                        return "No Reaction Selected.";
                    else
                        return ((Reaction)lstReactions.SelectedItems[0].Tag).StatusString;
                string text = "Status: " + m_CurrentReaction.Status + ".";
                if (!m_CurrentReaction.ProductsOk)
                    text += " Invalid product string.";
                if (!m_CurrentReaction.ReactantsOk)
                    text += " Invalid reactant string.";
                if (!m_CurrentReaction.ExtentInfo.IsValid())
                    text += " Invalid extent specified.";
                if (!m_CurrentReaction.HeatOfReactionOK)
                {
                    text += " Thermodynamic properties undefined for: {";
                    foreach (Compound c in m_CurrentReaction.Compounds)
                        if (!c.HeatOK)
                            text += c.Symbol + ", ";
                    text = text.Substring(0, text.Length - 2) + "}.";
                }
                if (!m_CurrentReaction.Balanced)
                {
                    text += " Excess product elements: ";
                    foreach (KeyValuePair<Element, Fraction> kvp in m_CurrentReaction.UnbalancedDetails)
                        text += "{" + kvp.Key.Symbol + " = " + kvp.Value.ToString() + "}, ";
                    text = text.Substring(0, text.Length - 2);
                    text += ".";
                }
                if (m_CurrentReaction.Reducible)
                    text += " (Reduced Reaction Possible.)";
                return text;
            }
        }

        public bool ActualActive
        {
            get{ return m_bActualActive; }
            set 
            {
                m_bActualActive = value;
                if (!value)
                    Messaging.SendMessage(Handle, Messaging.WM_NCACTIVATE, (IntPtr)0, (IntPtr)0);
            }
        }

        protected int MaxReactionLocation
        {
            get
            {
                int ret = lstReactions.Items.Count;
                if (m_HX != null)
                    ret--;
                if (m_Sinks != null)
                    ret--;
                return ret;
            }
        }
        #endregion Properties

        #region Events
        public event EventHandler NowChanged;
        public event EventHandler CompoundsChanged;
        public event EventHandler SelectedReactionChanged;
        public event EventHandler SourcesSinksChanged;
        public event ReactionHandler ReactionAdded;
        public event ReactionHandler ReactionRemoved;
        public event ReactionHandler ReactionChanged;
        public event EventHandler RecheckCutCopyPaste;
        #endregion Events

        #region Constructors
        public FrmReaction(ILog log)
        {
            SetUp(log);
            SetPanelEnables(pnlReaction, false);
        }

        public FrmReaction(string filename, ILog log)
        {
            SetUp(log);
            m_File = new FileStream(filename, FileMode.Open, FileAccess.ReadWrite, FileShare.ReadWrite);
            this.Title = Path.GetFileName(filename);
            ReadFile();

            //And because of the beauty and elegance that is working with the ListView control, we must put in a timer to update reaction numbers! WEEE!
            //Let's see if we can do this with Application.DoEvents rather... Nope. Back to multithreading approach...
            Thread updateLVIThread = new Thread(new ThreadStart(InitialUpdateLstReactions));
            updateLVIThread.Start();
        }

        private void InitialUpdateLstReactions()
        {
            DelayedUpdateLstReactions(); //This has got a 200ms sleep in it...
            try
            {
                this.BeginInvoke(new ThreadStart(HideGroups));
            }
            catch { }
        }

        private void DelayedUpdateLstReactions()
        {
            Thread.Sleep(200);
            try
            {
                this.BeginInvoke(new ThreadStart(UpdateReactionNumbers));
            }
            catch { }
        }

        public FrmReaction(int titleNum, ILog log)
        {
            SetUp(log);
            this.Title = "Untitled-" + titleNum;
            SetPanelEnables(pnlReaction, false);
        }

        private void SetUp(ILog log)
        {
            InitializeComponent();

            //lstReactions.ListViewItemSorter = new IntStringComparer();

            m_SequenceHeader.Text = "Sequence";
            m_Log = log;
            txtReactants.FrmOwner = this;
            txtProducts.FrmOwner = this;

            m_OptionalExtentControls.AddRange(new Control[] {
                this.numExtentVal2,
                this.numExtentVal3,
                this.chkExtentOption,
                this.lblExtent2,
                this.lblExtent3 });

            m_HXControls = new Control[] {
                this.numHXApproach,
                this.numHX,
                this.lblHXApproach,
                this.lblHXPercent,
                this.lblHXUnits,
                this.lblHXValue, };

            comboHORConditions.Items.AddRange(new object[] {
                TPConditions.Feed,
                TPConditions.Product,
                TPConditions.Standard,
                TPConditions.Custom });

            this.txtProducts.DisallowedCharaceters = this.txtReactants.DisallowedCharaceters = new List<char>("<>=:;");

            txtProducts.SelectAll(); txtReactants.SelectAll();
            txtReactants.SelectionAlignment = txtProducts.SelectionAlignment = HorizontalAlignment.Center;
            txtProducts.Select(0, 0); txtReactants.Select(0, 0);

            ListViewGroup srcGrp = new ListViewGroup("grpSources", "Sources");
            lstReactions.Groups.Add(srcGrp);
            for (int i = 0; i <= sMaxSequences; i++)
            {
                ListViewGroup grp = new ListViewGroup("grpSequence" + i, "Sequence " + i);
                lstReactions.Groups.Add(grp);
            }
            ListViewGroup sinkGrp = new ListViewGroup("grpSinks", "Sinks");
            lstReactions.Groups.Add(sinkGrp);
            ListViewGroup HXGrp = new ListViewGroup("grpHX", "Heat Exchange");
            lstReactions.Groups.Add(HXGrp);

            comboHXType.SelectedItem = null;
            comboDirection.SelectedItem = "=";

            pnlReaction_Resize(null, new EventArgs());

            txtSources.AllowDrop = true;
            txtSinks.AllowDrop = true;

            txtSources.DragDrop += new DragEventHandler(txtCompounds_DragDrop);
            txtSinks.DragDrop += new DragEventHandler(txtCompounds_DragDrop);

            txtSources.DragEnter += new DragEventHandler(txtCompounds_DragEnter);
            txtSinks.DragEnter += new DragEventHandler(txtCompounds_DragEnter);

            numSequence.Text = "";

            Changed = false;

            AutoCompleteBoxes.AddRange(new BoxAutoComplete[] { txtSources, txtSinks, txtReactants, txtProducts });

            UpdateAutoCompleteForms();

            foreach (TabPage tp in tcIDE.TabPages)
            {
                foreach (Control c in tp.Controls)
                {
                    c.Dock = DockStyle.Bottom;
                    tp.Controls.Remove(c);
                    tabReactions.Controls.Add(c);
                    c.Visible = false;
                }
                tp.Dispose();
            }
            pnlBlank.Visible = true;
            tcIDE.Dispose();

            SetFocusChanged(this);
            lstReactions.SelectedIndexChanged += new EventHandler(FireRecheckCutCopyPaste);

            if (Program.Dynamic)
                comboExtentType.Items.Add("Rate");
        }

        private void SetFocusChanged(Control ctrl)
        {
            ctrl.Enter += new EventHandler(FireRecheckCutCopyPaste);
            if (ctrl is RichTextBox)
                ((RichTextBox)ctrl).SelectionChanged += new EventHandler(FireRecheckCutCopyPaste);
            else if (ctrl is TextBoxEx)
                ((TextBoxEx)ctrl).SelectionChanged += new EventHandler(FireRecheckCutCopyPaste);
            foreach (Control c in ctrl.Controls)
                SetFocusChanged(c);
        }        
        #endregion Constructors

        #region Public Functions
        public void Save()
        {
            bool ok = true;
            bool balanced = true;
            foreach (ListViewItem lvi in lstReactions.Items)
                if (lvi.Tag is SimpleReaction)
                {
                    if (((SimpleReaction)lvi.Tag).Status == RxnStatuses.Invalid)
                        ok = false;
                    else if (((SimpleReaction)lvi.Tag).Status == RxnStatuses.Imbalanced)
                        balanced = false;
                }
            if (!ok)
                MessageBox.Show(this, "Some reactions are not valid. This reaction block may not load into SysCAD.", Title, MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            else if (!balanced)
                MessageBox.Show(this, "Some reactions are not balanced.", Title, MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            m_File.SetLength(0);
            StreamWriter sw = new StreamWriter(m_File);
            //First, the comments:
            if (!string.IsNullOrEmpty(txtDescription.Text))
            {
                sw.WriteLine(";" + txtDescription.Text.TrimEnd().Replace("\n", "\n;"));
            }

            sw.WriteLine();

            try
            {
                //Sources, if they exist:
                if (m_Sources != null)
                {
                    /*sw.WriteLine(";RC1: " + txtSourceComments.Text + "");
                    if (!chkSourcesEnabled.Checked)
                        sw.Write(';');
                    sw.WriteLine("Source: " + txtSources.Text);*/
                    sw.WriteLine(m_Sources.ToSaveString());
                    sw.WriteLine();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
            
            if (chkFirstReactant.Checked)
                sw.WriteLine(";<UseFirstReactant=True>");

            //Reactions:
            try
            {
                sw.WriteLine("Reactions:");
                foreach (ListViewItem lvi in OrderedLVIs(lstReactions.Items))
                    if (lvi.Tag is SimpleReaction)
                    {
                        SimpleReaction rxn = (SimpleReaction)lvi.Tag;
                        sw.WriteLine();
                        sw.WriteLine(rxn.ToSaveString(chkSequence.Checked));
                    }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
            sw.WriteLine();

            try
            {
                if (m_Sinks != null)
                {
                    /*sw.WriteLine(";RC" + m_Sinks.LVI.Text + ": " + txtSinkComments.Text);
                    if (!chkSinksEnabled.Checked)
                        sw.Write(';');
                    sw.WriteLine("Sink: " + txtSinks.Text);*/
                    sw.WriteLine(m_Sinks.ToSaveString());
                    sw.WriteLine();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }

            try
            {
                if (m_HX != null)
                {
                    /*sw.WriteLine(";RC" + m_HX.LVI.Text + ": " + txtHXComment.Text);
                    if (!chkHXEnabled.Checked)
                        sw.Write(';');
                    sw.WriteLine(m_HX.ToString());*/
                    sw.WriteLine(m_HX.ToSaveString());
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }

            sw.WriteLine("End");

            string LastSelected = "None";
            if (lstReactions.SelectedIndices.Count > 0)
                LastSelected = lstReactions.SelectedIndices[0].ToString();
            sw.WriteLine(";<LastSelected=" + LastSelected + ">");
            sw.WriteLine(";<ReactionEditorVer=" + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString(4) + ">");

            sw.Flush();

            Changed = false;
            foreach (ListViewItem lvi in lstReactions.Items)
                if (lvi.Tag is SimpleReaction)
                {
                    ((SimpleReaction)lvi.Tag).Initialised = false;
                    ((SimpleReaction)lvi.Tag).HasChanged = false;
                    ((SimpleReaction)lvi.Tag).Backup();
                    ((SimpleReaction)lvi.Tag).Initialised = true;
                }
                else if (lvi.Tag is Reaction)
                {
                    ((Reaction)lvi.Tag).Backup();
                    ((Reaction)lvi.Tag).HasChanged = false;
                }

            //To update the colour in the file view on the right to indicate that the reaction block is unchanged:
            if (NowChanged != null)
                NowChanged(this, new EventArgs());
        }

        public void SaveAs(string newName)
        {
            if (m_File != null)
                m_File.Close();
            m_File = new FileStream(newName, FileMode.Create, FileAccess.ReadWrite, FileShare.Read);
            this.Title = Path.GetFileName(newName);
            Save();
        }

        public void SelectReaction(SimpleReaction rxn)
        {
            if (rxn == null)
            {
                lstReactions.SelectedItems.Clear();
                return;
            }
            if (rxn.LVI == null 
                || !lstReactions.Items.Contains(rxn.LVI)
                || lstReactions.SelectedItems.Contains(rxn.LVI))
                return;
            lstReactions.SelectedItems.Clear();
            rxn.LVI.Selected = true;
        }

        public bool ContainsReaction(SimpleReaction rxn)
        {
            foreach (ListViewItem lvi in lstReactions.Items)
                if (lvi.Tag == rxn)
                    return true;
            return false;
        }

        public SimpleReaction AddReaction(SimpleReaction rxn, int location)
        {
            ListViewItem lvi = new ListViewItem();
            lvi.SubItems.AddRange(new string[] {"", "", "", "" });
            if (location == 0 && m_Sources != null)
                lstReactions.Items.Insert(1, lvi);
            else if (location > MaxReactionLocation)
                lstReactions.Items.Insert(MaxReactionLocation, lvi);
            else if (location >= 0)
                lstReactions.Items.Insert(location, lvi);
            else
                lstReactions.Items.Insert(MaxReactionLocation, lvi);
            SimpleReaction newRxn = rxn.Clone(lvi);
            newRxn.SequenceChanged += new EventHandler(rxn_SequenceChanged);
            newRxn.Changed += new EventHandler(rxn_Changed);
            newRxn.FireChanged();
            UpdateReactionNumbers();
            if (ReactionAdded != null)
                ReactionAdded(this, rxn);
            return newRxn;
        }

        public List<Compound> GetCompounds()
        {
            List<Compound> ret = new List<Compound>();
            foreach (ListViewItem lvi in lstReactions.Items)
                if (lvi.Tag is SimpleReaction)
                {
                SimpleReaction rxn = (SimpleReaction)lvi.Tag;
                foreach (Compound c in rxn.Compounds)
                    if (!ret.Contains(c))
                        ret.Add(c);
                }
            return ret;
        }

        public void DoDatabaseChanged()
        {
            foreach (ListViewItem lvi in lstReactions.Items)
                if (lvi.Tag is SimpleReaction)
                {
                    Log.SetSource(new FrmReaction.MessageFrmReaction(Title, this, (SimpleReaction)lvi.Tag));
                    ((SimpleReaction)lvi.Tag).DoDatabaseChanged();
                }
        }

        public void RevertCurrent()
        {
            if (m_CurrentReaction != null && m_CurrentReaction.CanRevert)
            {
                if (FrmChangeDialogue.Show(m_CurrentReaction, m_CurrentReaction.RevertReaction, "Revert",
                    "Are you sure you wish to revert the following reaction", "Original Reaction", chkSequence.Checked, true) == DialogResult.OK)
                {
                    LoadReaction(m_CurrentReaction.Revert());
                    UpdateReactionNumbers();
                    if (ReactionChanged != null)
                        ReactionChanged(this, m_CurrentReaction);
                    if (NowChanged != null)
                        NowChanged(this, new EventArgs());
                }
            }
            else if (lstReactions.SelectedItems.Count > 0)
            {
                Reaction rxn = (Reaction)lstReactions.SelectedItems[0].Tag;
                if (rxn.CanRevert &&
                    FrmChangeDialogue.Show(rxn, rxn.RevertReaction, "Revert",
                    "Are you sure you wish to revert the following reaction", "Original Reaction") == DialogResult.OK)
                    rxn.Revert();
            }
        }

        public void Copy()
        {
            if (lstReactions.Focused && lstReactions.SelectedItems.Count > 0)
                Clipboard.SetData(SimpleReaction.sFormat.Name, ((SimpleReaction)lstReactions.SelectedItems[0].Tag).ToSaveString(true));
            Control act = ActualActiveControl(this);
            if (act is RichTextBox)
                ((RichTextBox)act).Copy();
            else if (act is TextBoxEx)
                ((TextBoxEx)act).Copy();
        }

        public void Cut()
        {
            if (lstReactions.Focused && lstReactions.SelectedItems.Count > 0)
            {
                /*DataObject obj = new DataObject(SimpleReaction.sFormat.Name, lstReactions.SelectedItems[0].Tag);
                Clipboard.SetDataObject(obj);*/
                DataObject obj = new DataObject();
                string s = ((SimpleReaction)lstReactions.SelectedItems[0].Tag).ToSaveString(true);
                obj.SetData(
                    SimpleReaction.sFormat.Name,
                    s);
                Clipboard.SetData(SimpleReaction.sFormat.Name, s);

                lstReactions.SelectedItems[0].Remove();
            }
            else if (txtComment.Focused)
                txtComment.Cut();
            else if (txtDescription.Focused)
                txtDescription.Cut();
            else if (txtSources.Focused)
                txtSources.Cut();
            else if (txtSinks.Focused)
                txtSinks.Cut();
            else if (txtProducts.Focused)
                txtProducts.Cut();
            else if (txtReactants.Focused)
                txtReactants.Cut();
        }

        public void Paste()
        {
            IDataObject obj = Clipboard.GetDataObject();
            object bleh = Clipboard.GetData(SimpleReaction.sFormat.Name);
            if (obj.GetDataPresent(SimpleReaction.sFormat.Name))
            {
                string data = (string)obj.GetData(SimpleReaction.sFormat.Name);
                CheckBox properChkSequence = chkSequence; //Because the new reaction will always carry sequence information.
                chkSequence = new CheckBox();
                //A reaction shouldn't be both enabled and disabled, so simply call both:
                Match m = SimpleReaction.s_GeneralReactionRegex.Match(data);/*SimpleReaction.s_ReactionRegex.Match(data);
                if (!m.Success)
                    m = SimpleReaction.s_DisabledReactionRegex.Match(data);*/
                SimpleReaction rxn = CreateReaction();
                rxn.SetRegex(m, new MessageSource(""), "");
                chkSequence = properChkSequence;
                ChangeOccured();
                if (ReactionAdded != null)
                    ReactionAdded(this, rxn);
                if (CompoundsChanged != null)
                    CompoundsChanged(this, new EventArgs());
            }
            if (Clipboard.ContainsText())
            {
                PasteText(this);
                ChangeOccured();
            }
        }

        public void AddSources(params Compound[] newSources)
        {
            if (m_Sources == null)
            {
                m_Sources = new CompoundListReaction(CompoundListReaction.ListType.Source);
                SetupSources();
            }
            foreach (Compound c in newSources)
                m_Sources.AddCompound(c);

            ChangeOccured();
            if (SourcesSinksChanged != null)
                SourcesSinksChanged(this, new EventArgs());
        }

        public void AddSinks(params Compound[] newSinks)
        {
            if (m_Sinks == null)
            {
                m_Sinks = new CompoundListReaction(CompoundListReaction.ListType.Sink);
                SetupSinks();
            }
            foreach (Compound c in newSinks)
                m_Sinks.AddCompound(c);
            ChangeOccured();
            if (SourcesSinksChanged != null)
                SourcesSinksChanged(this, new EventArgs());
        }

        public void RemoveSinks(params Compound[] toRemove)
        {
            foreach (Compound c in toRemove)
                m_Sinks.RemoveCompound(c);
            ChangeOccured();
            if (SourcesSinksChanged != null) //TODO: Make sources and sinks nowchanged events fire these events.
                SourcesSinksChanged(this, new EventArgs());
        }

        public void RemoveSources(params Compound[] toRemove)
        {
            foreach (Compound c in toRemove)
                m_Sources.RemoveCompound(c);
            ChangeOccured();
            if (SourcesSinksChanged != null)
                SourcesSinksChanged(this, new EventArgs());
        }

        public void DropCompound(CompoundDrag data)
        {
            if (data.ctrl == txtProducts)
                m_CurrentReaction.RemoveProduct(data.startIndex);
            else if (data.ctrl == txtReactants)
                m_CurrentReaction.RemoveReactant(data.startIndex);
        }

        public void RenameFile(string newFileName)
        {
            string oldPath = m_File.Name;
            string newPath = Path.Combine(Path.GetDirectoryName(m_File.Name), newFileName);
            if (oldPath.ToLower() == newPath.ToLower())
                return;
            if (File.Exists(newPath))
                throw new IOException("File with desired name already exists");
            try
            {
                m_File.Close();
                File.Move(m_File.Name, newPath);
                m_File = new FileStream(newPath, FileMode.Open, FileAccess.ReadWrite, FileShare.ReadWrite);
                this.Title = Path.GetFileName(newPath);
            }
            catch (Exception ex)
            {
                m_File = new FileStream(oldPath, FileMode.Open, FileAccess.ReadWrite, FileShare.ReadWrite); //If it failes, we re-open the old file...
                throw ex;
            }
        }
        #endregion Public Functions

        #region Protected Functions
        protected void HideGroups()
        {
            foreach (ListViewGroup lvg in lstReactions.Groups)
                if (lvg.Items.Count > 1)
                    return;
            menuShowSequence.Checked = false;
        }

        protected Control ActualActiveControl(ContainerControl c)
        {
            if (c.ActiveControl is ContainerControl)
                return ActualActiveControl((ContainerControl)c.ActiveControl);
            else
                return c.ActiveControl;
        }

        protected void UpdateReactionNumbers()
        {
            if (m_bLoading) return;
            ListViewItem[] orderedLVIs = OrderedLVIs(lstReactions.Items);
            int index = 1;
            for (int i = 0; i < orderedLVIs.Length; i++)
                if ((orderedLVIs[i].Tag is SimpleReaction && ((SimpleReaction)orderedLVIs[i].Tag).Enabled))
                    ((SimpleReaction)orderedLVIs[i].Tag).ReactionNumber = index++;
                else if ((orderedLVIs[i].Tag == m_Sources && chkSourcesEnabled.Checked)
                    || (orderedLVIs[i].Tag == m_Sinks && chkSinksEnabled.Checked)
                    || (orderedLVIs[i].Tag == m_HX && chkHXEnabled.Checked))
                    ((Reaction)orderedLVIs[i].Tag).ReactionNumber = index++;
                else
                    orderedLVIs[i].Text = "-";
        }

        protected ListViewItem[] OrderedLVIs(ListView.ListViewItemCollection collection)
        {
            ListViewItem[] ret = new ListViewItem[collection.Count];
            int i = 0;
            foreach (ListViewItem LVI in collection)
                ret[i++] = LVI;
            Array.Sort<ListViewItem>(ret, new Comparison<ListViewItem>(LVIHeightComp));
            return ret;
        }

        protected int LVIHeightComp(ListViewItem lvi1, ListViewItem lvi2)
        {
            return lvi1.Position.Y.CompareTo(lvi2.Position.Y);
        }

        protected void UpdateAutoCompleteForms()
        {
            //txtReactants.HitCounts.Clear();
            foreach (BoxAutoComplete box in AutoCompleteBoxes)
            {
                box.HitCounts = Program.AutocompleteHitCounts;
                box.Items.Clear();
                foreach (Compound c in Compound.CompoundList.Values)
                    box.Items.Add(c);
            }
        }

        protected bool PasteText(Control c)
        {
            if ((c is TextBox || c is RichTextBox)
                && c.Focused)
            {
                ((TextBoxBase)c).Paste();
                return true;
            }
            foreach (Control subc in c.Controls)
                if (PasteText(subc))
                    return true;
            return false;
        }

        protected void ReadFile()
        {
            try
            {
                m_bLoading = true;
                
                Log.SetSource(new MessageFrmReaction(this.Title, this, null));

                byte[] buffer = new byte[m_File.Length];
                m_File.Read(buffer, 0, (int)m_File.Length);
                string contents = Encoding.Default.GetString(buffer);

                //Version:
                Match verMatch = s_FileVersionRegex.Match(contents);
                Version savedVersion = new Version(0,0,0,0);
                if (verMatch.Success)
                    try{ savedVersion = new Version(verMatch.Groups["Version"].Value); }
                    catch { }
                Version MinVersion = new Version("1.0.17");

                //General comments:
                StringBuilder openingCommentsSB = new StringBuilder();
                Match commentsMatch = s_OpeningCommentRegex.Match(contents);
                if (commentsMatch.Success)
                {
                    Group g = commentsMatch.Groups["Comment"];
                    if (g.Success)
                        foreach (Capture c in g.Captures)
                            openingCommentsSB.AppendLine(c.Value);
                    txtDescription.Text = openingCommentsSB.ToString();
                }

                //Sources and sinks use a comment stripped version of the file:
                StringBuilder activeSB = new StringBuilder();
                StringBuilder commentSB = new StringBuilder();
                StringBuilder relevantSB = new StringBuilder();
                for (Match m = s_CommentRemovingRegex.Match(contents, commentsMatch.Index + commentsMatch.Length);
                     m.Success; m = m.NextMatch())
                {
                    relevantSB.AppendLine(m.Value);
                    activeSB.AppendLine(m.Groups["Active"].Value);
                    if (m.Groups["Comment"].Success)
                        commentSB.AppendLine(m.Groups["Comment"].Value);
                    if (m.Groups["Active"].Value.ToLowerInvariant().Trim() == "end")
                        break;
                    //Now, stopping at the End token:
                    if (s_EndRegex.Match(m.Value).Success)
                        break;
                }
                string relevant = relevantSB.ToString();
                string commentsRemoved = activeSB.ToString();
                string commentsOnly = commentSB.ToString();
                if (!s_EndRegex.Match(commentsRemoved).Success)
                    Log.Message("'End' token not found. Reading to end of file (May cause unpredictable results)", MessageType.Warning);

                if (savedVersion < MinVersion)
                {
                    txtDescription.Text += "\r\n---- Upgraded To Reaction Editor Format " + DateTime.Now.Date.ToString("dd/MM/yyyy") + " ----";
                    if (!string.IsNullOrEmpty(commentsOnly.Trim()))
                        txtDescription.Text += "\r\n\r\n------------ Original Comments ------------\r\n"
                                                + commentsOnly;
                }

                List<int> indexlist = new List<int>();
                int sourceIndex = -1, sinkindex = -1, HXindex = -1;

                //Sources:
                Match sourcesSinksMatch = CompoundListReaction.s_SourceSinkRegex.Match(relevant);
                if (sourcesSinksMatch.Success && sourcesSinksMatch.Groups["Type"].Value.ToLowerInvariant() == "source")
                {
                    m_Sources = new CompoundListReaction(sourcesSinksMatch);
                    if (!m_Sources.Enabled && savedVersion < MinVersion)
                        m_Sources = null;
                    else
                    {
                        sourceIndex = sourcesSinksMatch.Index;
                        SetupSources();
                        indexlist.Add(sourceIndex);
                        sourcesSinksMatch = sourcesSinksMatch.NextMatch();
                    }
                }

                //Reactions:
                Match start = s_RxnBlockStartRegex.Match(relevant);
                Match end = s_RxnBlockEndRegex.Match(relevant);
                if (!start.Success || !end.Success)
                    throw new Exception("Reaction Block Not Found");

                int rxnBlockStart = start.Index + start.Length;
                int rxnBlockEnd = end.Index;
                string rxnBlock = relevant.Substring(rxnBlockStart, rxnBlockEnd - rxnBlockStart);

                Dictionary<int, SimpleReaction> indices = new Dictionary<int, SimpleReaction>();
                FindReactions(rxnBlock, SimpleReaction.s_GeneralReactionRegex, savedVersion >= MinVersion, ref indices, start.Index);

                //Sinks:
                if (sourcesSinksMatch.Success && sourcesSinksMatch.Groups["Type"].Value.ToLowerInvariant() == "sink")
                {
                    m_Sinks = new CompoundListReaction(sourcesSinksMatch);
                    if (!m_Sinks.Enabled && savedVersion < MinVersion)
                        m_Sinks = null;
                    else
                    {
                        sinkindex = sourcesSinksMatch.Index;
                        indexlist.Add(sinkindex);
                        SetupSinks();
                    }
                }

                //Heat Exchange:
                Match HXMatch = HXReaction.s_HXRegex.Match(relevant);
                if (HXMatch.Success)
                {
                    m_HX = new HXReaction(HXMatch);
                    if (!m_HX.Enabled && savedVersion < MinVersion)
                        m_HX = null;
                    else
                    {
                        HXindex = HXMatch.Index;
                        indexlist.Add(HXindex);
                        SetupHX();
                    }
                }

                //Reaction Editor Saved Comments:
                //First Reactant:
                chkFirstReactant.Checked = contents.ToLowerInvariant().Contains("<usefirstreactant=true>");
                //LastSelected:
                Match LSMatch = s_LastSelectedRegex.Match(contents);
                if (LSMatch.Success)
                {
                    int index;
                    if (int.TryParse(LSMatch.Groups["Value"].Value, out index) && index < lstReactions.Items.Count) //"None" will return false here, therefore selecting nothing.
                        lstReactions.Items[index].Selected = true;
                }
                else if (lstReactions.Items.Count > 0)
                    lstReactions.Items[0].Selected = true;

                //To handle reaction numbers:
                indexlist.AddRange(indices.Keys);
                indexlist.Sort();
                int num = 1;
                foreach (int i in indexlist)
                {
                    if (indices.ContainsKey(i))
                    {
                        SimpleReaction currentReaction = indices[i];
                        if (currentReaction.Enabled)
                            currentReaction.ReactionNumber = currentReaction.OriginalReactionNumber = num++;
                        currentReaction.Backup();
                        currentReaction.Initialised = true;
                    }
                    else if (i == sourceIndex && m_Sources.Enabled)
                        m_Sources.ReactionNumber = m_Sources.OriginalReactionNumber = num++;
                    else if (i == sinkindex && m_Sinks.Enabled)
                        m_Sinks.ReactionNumber = m_Sinks.OriginalReactionNumber = num++;
                    else if (i == HXindex && m_HX.Enabled)
                        m_HX.ReactionNumber = m_HX.OriginalReactionNumber = num++;
                }

                Changed = false;
                if (lstReactions.Items.Count != 1)
                    Log.Message("File opened. " + lstReactions.Items.Count + " Reactions found.", MessageType.Note);
                else
                    Log.Message("File opened. 1 Reaction found.", MessageType.Note);
                Log.RemoveSource();
            }
            finally
            {
                UpdateReactionNumbers();
                m_bLoading = false;
            }
        }

        //Now returning a dictionary containing the reaction indexes in the file.
        protected void FindReactions(string rxnBlock, Regex reactionRegex, bool includeDisabled, ref Dictionary<int, SimpleReaction> ret, int offset)
        {
            int lastSequence = 1;
            bool sequenceFound = false;
            for (Match rxnMatch = reactionRegex.Match(rxnBlock); rxnMatch.Success; rxnMatch = rxnMatch.NextMatch())
            {
                SimpleReaction currentReaction = CreateReaction();
                currentReaction.Initialised = false;
                MessageSource source = new MessageFrmReaction(
                    this.Title + ", Reaction " + ReactionNo,
                    this,
                    currentReaction);
                Log.SetSource(source);
                try
                {
                    currentReaction.SetRegex(rxnMatch, source, this.Title);
                    currentReaction.OriginalReactionNumber = Reaction.DisabledReactionNumber;

                    if (currentReaction.Enabled || includeDisabled)
                    {
                        ret.Add(rxnMatch.Index + offset, currentReaction);

                        Group grpSequence = rxnMatch.Groups["Sequence"];
                        if (grpSequence.Success)
                        {
                            Match sequenceMatch = SimpleReaction.s_SequenceRegex.Match(grpSequence.Captures[0].Value.Trim());
                            if (sequenceMatch.Success)
                            {
                                sequenceFound = true;
                                lastSequence = int.Parse(sequenceMatch.Groups["Value"].Captures[0].Value);
                            }
                            else
                                Log.Message("Unable to parse sequence '" + grpSequence.Value + "'", MessageType.Warning);
                        }
                        currentReaction.Sequence = lastSequence;
                    }
                    else
                        lstReactions.Items.Remove(currentReaction.LVI);
                }
                catch (Exception ex)
                {
                    Log.Message(ex.Message, MessageType.Error);
                    lstReactions.Items.Remove(currentReaction.LVI);
                }
                Log.RemoveSource();
            }
            if (sequenceFound)
                chkSequence.Checked = true;
        }

        protected SimpleReaction CreateReaction()
        {
            ListViewItem lvi = new ListViewItem();
            lvi.SubItems.AddRange(new string[] { "", "", "", "" });
            lstReactions.Items.Insert(MaxReactionLocation, lvi);
            SimpleReaction rxn = new SimpleReaction(lvi);
            rxn.Changed += new EventHandler(rxn_Changed);
            rxn.SequenceChanged += new EventHandler(rxn_SequenceChanged);
            
            rxn_Changed(rxn, new EventArgs());
            rxn_SequenceChanged(rxn, new EventArgs());

            UpdateReactionNumbers();
            rxn.Initialised = true;
            return rxn;
        }

        protected void LoadReaction(SimpleReaction rxn)
        {
            m_bLoading = true;
            if (m_CurrentReaction != null)
            {
                m_CurrentReaction.Changed -= new EventHandler(currentReactionChanged);
                m_CurrentReaction.ReactantsChanged -= new EventHandler(rxn_ReactantsChanged);
                m_CurrentReaction.ProductsChanged -= new EventHandler(rxn_ReactantsChanged);
            }
            m_CurrentReaction = rxn;
            txtReactants.Visible = txtProducts.Visible = comboDirection.Visible = true;
            txtFormula.Visible = false;
            if (SelectedReactionChanged != null)
                SelectedReactionChanged(this, new EventArgs());
            if (rxn == null)
            {
                SetPanelEnables(pnlReaction, false);
                //pnlReaction.Enabled = false;
                txtReactants.Text = txtProducts.Text = "";
                comboDirection.SelectedIndex = -1;
                comboExtentType.SelectedIndex = -1;
                comboExtentSpecie.SelectedIndex = -1;
                comboHORSpecie.SelectedIndex = -1;
                chkHOROverride.Checked = false;
                numSequence.Text = "";
                chkEnabled.Checked = false;
                m_bLoading = false;
                return;
            }
            rxn.Changed += new EventHandler(currentReactionChanged);
            rxn.ReactantsChanged += new EventHandler(rxn_ReactantsChanged);
            rxn.ProductsChanged += new EventHandler(rxn_ReactantsChanged);
            btnCopy.Enabled = btnMoveDown.Enabled = btnMoveUp.Enabled = btnRemove.Enabled = true;
            if (rxn.UseOriginalString)
            {
                txtReactants.Visible = txtProducts.Visible = comboDirection.Visible = false;
                txtFormula.Visible = true;
                txtFormula.Text = rxn.ToString();
            }
            /*if (string.IsNullOrEmpty(rxn.GetProductsString()))
                SetWaitingText(txtProducts, "Products");
            else
            {
                txtProducts.Text = rxn.GetProductsString();
                txtProducts.TextAlign = HorizontalAlignment.Left;
                txtProducts.ForeColor = System.Drawing.SystemColors.WindowText;
                txtProducts.Tag = true;
            }
            if (string.IsNullOrEmpty(rxn.GetReactantsString()))
                SetWaitingText(txtReactants, "Reactants");
            else
            {
                txtReactants.TextAlign = HorizontalAlignment.Right;
                txtReactants.Text = rxn.GetReactantsString();
                txtReactants.ForeColor = System.Drawing.SystemColors.WindowText;
                txtReactants.Tag = true;
            }*/

            SetPanelEnables(pnlReaction, true);

            comboExtentSpecie.Enabled = !chkFirstReactant.Checked;
            numSequence.Enabled = chkSequence.Checked;
            SetHOREnabled(rxn.CustomHeatOfReaction);

            txtReactants.Text = rxn.GetReactantsString();
            txtProducts.Text = rxn.GetProductsString();

            RepopulateSpecies();

            comboDirection.SelectedIndex = (int)rxn.Direction;

            comboExtentType.SelectedIndex = (int)rxn.ExtentType;
            chkHOROverride.Checked = rxn.CustomHeatOfReaction;

            numExtentValue.Text = rxn.ExtentInfo.Value.ToString();
            chkEnabled.Checked = rxn.Enabled;
            txtComment.Text = rxn.Comment;

            if (chkSequence.Checked)
            {
                numSequence.Value = Math.Min(Math.Max(rxn.Sequence, 1), sMaxSequences);
                numSequence.Text = numSequence.Value.ToString();
            }
            //pnlReaction.Enabled = true;
            
            m_bLoading = false;
            btnBalance.Enabled = rxn.CanBalance;
        }

        protected void UpdateStatusLabel()
        {
            statusLabel.ForeColor = StatusMessage.StartsWith("Status: Invalid") ? Color.DarkRed : SystemColors.WindowText;
            statusLabel.Text = StatusMessage;
        }

        protected void SetPanelEnables(Control p, bool val)
        {
            foreach (Control c in p.Controls)
                if (c == p)
                    continue;
                else if (c is Panel || c is GroupBox)
                    SetPanelEnables(c, val);
                else
                    c.Enabled = val;
        }

        void rxn_ReactantsChanged(object sender, EventArgs e)
        {
            if (m_bUpdateTextboxes && m_CurrentReaction != null)
                txtReactants.Text = m_CurrentReaction.GetReactantsString();
            if (m_bUpdateTextboxes && m_CurrentReaction != null)
                txtProducts.Text = m_CurrentReaction.GetProductsString();

            object cHOR = comboHORSpecie.SelectedItem;
            object cExt = chkFirstReactant.Checked ? m_CurrentReaction.FirstReactant : comboExtentSpecie.SelectedItem;
            RepopulateSpecies();
            if (cExt != null && comboExtentSpecie.Items.Contains(cExt))
                comboExtentSpecie.SelectedItem = cExt;
            comboExtentSpecie_SelectedIndexChanged(this, new EventArgs());
            if (cHOR != null && comboHORSpecie.Items.Contains(cHOR))
                comboHORSpecie.SelectedItem = cHOR;
            comboHORSpecie_SelectedIndexChanged(this, new EventArgs());
            if (CompoundsChanged != null)
                CompoundsChanged(this, new EventArgs());
        }

        protected void RepopulateSpecies()
        {
            comboExtentSpecie.Items.Clear();
            comboHORSpecie.Items.Clear();
            foreach (Compound c in m_CurrentReaction.Compounds)
            {
                comboExtentSpecie.Items.Add(c);
                comboHORSpecie.Items.Add(c);
            }
        }

        protected void ChangePosition(ListViewItem item, int newIndex)
        {
            m_bDoEvents = false;
            bool wasSelected = item.Selected;
            int oldIndex = item.Index;
            ListViewGroup grp = item.Group;
            lstReactions.Items.Remove(item);
            try
            {
                lstReactions.Items.Insert(newIndex, item);
            }
            catch
            {
                lstReactions.Items.Add(item);
            }
            item.Group = grp;

            lstReactions.SelectedIndices.Clear();
            if (wasSelected)
            {
                lstReactions.SelectedIndices.Add(item.Index);
                btnMoveUp.Enabled = lstReactions.SelectedIndices[0] >
                    (m_Sources != null ? m_Sources.LVI.Index + 1 : 0);
                btnMoveDown.Enabled = 
                    lstReactions.SelectedIndices[0] < MaxReactionLocation - 1;
            }
            UpdateReactionNumbers();
            m_bDoEvents = true;
        }
            
        protected void rxn_Changed(object sender, EventArgs e)
        {
            if (ReactionChanged != null)
                ReactionChanged(this, (SimpleReaction)sender);
            ChangeOccured();
        }

        protected void rxn_SequenceChanged(object sender, EventArgs e)
        {
            SimpleReaction rxn = (SimpleReaction)sender;
            if (lstReactions.ShowGroups)
            {
                m_bDoEvents = false;
                rxn.LVI.Group = lstReactions.Groups["grpSequence" + rxn.Sequence];
                m_bDoEvents = true;
            }
            //Can we get around this with Application.DoEvents()?
            Thread delayedUpdate = new Thread(new ThreadStart(DelayedUpdateLstReactions));
            delayedUpdate.Start();
        }

        protected void currentReactionChanged(object sender, EventArgs e)
        {
            txtFormula.Text = m_CurrentReaction.ToString();
            /*if (txtProducts.Text != m_CurrentReaction.GetProductsString())
                txtProducts.Text = m_CurrentReaction.GetProductsString();
            if (txtReactants.Text != m_CurrentReaction.GetReactantsString())
                txtReactants.Text = m_CurrentReaction.GetReactantsString();*/
        }

        protected void ChangeOccured()
        {
            if (m_CurrentReaction != null)
                btnBalance.Enabled = m_CurrentReaction.CanBalance;
            if (!m_bLoading)
                Changed = true;
            if (NowChanged != null)
                NowChanged(this, new EventArgs());
            UpdateStatusLabel();
        }

        /// <summary>
        /// Checks a comma seperated list of compounds.
        /// </summary>
        /// <param name="s">The list of compounds</param>
        /// <returns>A list of all locations where the compound is unparseable, and the length of this compound</returns>
        protected Dictionary<int, int> CheckCompounds(string s)
        {
            Dictionary<int, int> ret = new Dictionary<int, int>();
            if (string.IsNullOrEmpty(s.Trim()))
                return ret;
            Match m = Compound.s_CompoundSeperator.Match(s);
            foreach (Capture c in m.Groups["Comp"].Captures)
            {
                if (!Compound.CompoundList.ContainsKey(c.Value.Trim()))
                {
                    if (!string.IsNullOrEmpty(c.Value.Trim()))
                    {
                        Log.Message("Unable to parse compound '" + c.Value.Trim() + "'", MessageType.Warning);
                        int beforeLen = c.Length - c.Value.TrimStart().Length;
                        ret.Add(c.Index + beforeLen, c.Length);
                    }
                    else
                    {
                        Log.Message("Empty field found", MessageType.Warning);
                        ret.Add(c.Index, c.Length);
                    }
                }
            }
            return ret;
        }

        protected void ColourCompounds(RichTextBox box)
        {
            int oldSelectionStart = box.SelectionStart;
            int oldSelectionLength = box.SelectionLength;
            box.SelectAll();
            box.SelectionBackColor = box.BackColor;
            box.SelectionColor = box.ForeColor;
            Dictionary<int, int> unparseable = CheckCompounds(box.Text);
            foreach (KeyValuePair<int, int> kvp in unparseable)
            {
                box.Select(kvp.Key, kvp.Value);
                box.SelectionBackColor = Color.DarkRed;
                box.SelectionColor = Color.White;
            }
            box.Select(oldSelectionStart, oldSelectionLength);
        }

        protected ListViewItem FindLVIBelow(Point p)
        {
            int nClosestVal = int.MaxValue;
            ListViewItem closestLVI = null;
            foreach (ListViewItem lvi in lstReactions.Items)
                if (lvi.Tag is SimpleReaction &&
                    lvi.Bounds.Bottom > p.Y && 
                    lvi.Bounds.Bottom - p.Y < nClosestVal)
                {
                    nClosestVal = lvi.Bounds.Bottom - p.Y;
                    closestLVI = lvi;
                }
            return closestLVI;
        }
        #endregion Protected Functions

        #region Subclasses
        public delegate void ReactionHandler(FrmReaction form, SimpleReaction reaction);

        /// <summary>
        /// Only to be used on list view items.
        /// </summary>
        protected class IntStringComparer : IComparer
        {
            #region IComparer Members

            public int Compare(object x, object y)
            {
                if (!(x is ListViewItem && y is ListViewItem))
                    return 0;
                int nX, nY;
                int.TryParse(((ListViewItem)x).Text, out nX);
                int.TryParse(((ListViewItem)y).Text, out nY);
                return nX.CompareTo(nY);
            }

            #endregion
        }

        protected class MessageFrmReaction : MessageSource
        {
            protected FrmReaction m_owner;
            protected SimpleReaction m_rxn;
            public MessageFrmReaction(string title, FrmReaction owner, SimpleReaction rxn) :
                base(title)
            {
                m_owner = owner;
                m_rxn = rxn;
            }

            public override void DoCallback()
            {
                try
                {
                    if (m_owner != null && !m_owner.IsDisposed)
                    {
                        m_owner.Activate();
                        if (m_rxn != null)
                            m_owner.SelectReaction(m_rxn);
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.ToString());
                }
                base.DoCallback();
            }
        }
        #endregion Subclasses

        private void FireRecheckCutCopyPaste(object sender, EventArgs e)
        {
            if (RecheckCutCopyPaste != null)
                RecheckCutCopyPaste(this, new EventArgs());
        }

        private void lstReactions_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!m_bDoEvents) return;

            this.tabReactions.SuspendLayout();
            pnlBlank.Visible = pnlReaction.Visible = grpSinks.Visible = grpSources.Visible = grpHX.Visible = false;
            btnCopy.Enabled = btnMoveDown.Enabled = btnMoveUp.Enabled = btnRemove.Enabled = false;

            if (lstReactions.SelectedIndices.Count == 0)
            {
                LoadReaction(null);
                pnlBlank.Visible = true;
            }
            else
            {
                if (lstReactions.SelectedItems[0].Tag is SimpleReaction)
                {
                    LoadReaction((SimpleReaction)lstReactions.SelectedItems[0].Tag);
                    pnlReaction.Visible = true;
                    btnCopy.Enabled = btnRemove.Enabled = true;
                    btnMoveUp.Enabled = lstReactions.SelectedIndices[0] > (m_Sources != null ? 0 : 1);
                    btnMoveDown.Enabled = lstReactions.SelectedIndices[0] < MaxReactionLocation - 1;
                }
                else
                    LoadReaction(null);
                if (lstReactions.SelectedItems[0].Tag == m_Sources)
                {
                    grpSources.Visible = true;
                    btnRemove.Enabled = true;
                    txtSources.Text = m_Sources.CompoundString;
                    chkSourcesEnabled.Checked = m_Sources.Enabled;
                    txtSourceComments.Text = m_Sources.Comment;
                }
                else if (lstReactions.SelectedItems[0].Tag == m_Sinks)
                {
                    grpSinks.Visible = true;
                    btnRemove.Enabled = true;
                    txtSinks.Text = m_Sinks.CompoundString;
                    chkSinksEnabled.Checked = m_Sinks.Enabled;
                    txtSinkComments.Text = m_Sinks.Comment;
                }
                else if (lstReactions.SelectedItems[0].Tag == m_HX)
                {
                    grpHX.Visible = true;
                    comboHXType.SelectedIndex = (int) m_HX.Type;
                    numHX.Text = m_HX.Value.ToString();
                    numHXApproach.Value = (decimal)m_HX.Value2;
                    chkHXEnabled.Checked = m_HX.Enabled;
                    txtHXComment.Text = m_HX.Comment;
                    btnRemove.Enabled = true;
                }
            }

            UpdateStatusLabel();

            this.tabReactions.ResumeLayout();
        }

        private void btnAdd_Click(object sender, EventArgs e)
        {
            m_AddSelector.radioHX.Enabled = m_HX == null;
            m_AddSelector.radioSink.Enabled = m_Sinks == null;
            m_AddSelector.radioSource.Enabled = m_Sources == null;
            m_AddSelector.radioReaction.Checked = true;
            if (m_AddSelector.ShowDialog(this) == DialogResult.Cancel)
                return;
            if (m_AddSelector.radioReaction.Checked)
            {
                SimpleReaction rxn = CreateReaction();
                rxn.LVI.Selected = true;
                txtReactants.Select();
                UpdateReactionNumbers();
                if (ReactionAdded != null)
                    ReactionAdded(this, rxn);
            }
            else if (m_AddSelector.radioHX.Checked)
            {
                m_HX = new HXReaction();
                SetupHX();
                m_HX.LVI.Selected = true;
                comboHXType.Select();
            }
            else if (m_AddSelector.radioSink.Checked)
            {
                //TODO: Source and sink events.
                m_Sinks = new CompoundListReaction(CompoundListReaction.ListType.Sink);
                SetupSinks();
                m_Sinks.LVI.Selected = true;
                txtSinks.Select();
            }
            else if (m_AddSelector.radioSource.Checked)
            {
                m_Sources = new CompoundListReaction(CompoundListReaction.ListType.Source);
                SetupSources();
                m_Sources.LVI.Selected = true;
                txtSources.Select();
            }
        }

        private void SetupSinks()
        {
            lstReactions.Items.Add(m_Sinks.LVI);
            if (lstReactions.ShowGroups)
                m_Sinks.LVI.Group = lstReactions.Groups["grpSinks"];
            UpdateReactionNumbers();
            ChangePosition(m_Sinks.LVI,
                m_HX == null ? lstReactions.Items.Count - 1 :
                (m_HX.LVI.Index > 0 ? m_HX.LVI.Index : m_HX.LVI.Index - 1));
            m_Sinks.NowChanged += new EventHandler(m_Sinks_NowChanged);
        }

        void m_Sinks_NowChanged(object sender, EventArgs e)
        {
            if (m_bUpdateSourceSinksBlock)
                txtSinks.Text = m_Sinks.CompoundString;
            txtSinkComments.Text = m_Sinks.Comment;
            chkSinksEnabled.Checked = m_Sinks.Enabled;
            SourcesSinksChanged(this, new EventArgs());
            ChangeOccured();
        }

        private void SetupHX()
        {
            lstReactions.Items.Add(m_HX.LVI);
            if (lstReactions.ShowGroups)
                m_HX.LVI.Group = lstReactions.Groups["grpHX"];
            UpdateReactionNumbers();
            ChangePosition(m_HX.LVI, lstReactions.Items.Count);
            m_HX.NowChanged += new EventHandler(m_HX_NowChanged);
        }

        void m_HX_NowChanged(object sender, EventArgs e)
        {
            //comboHXType.SelectedIndex = (int)m_HX.Type;
            //numHXApproach.Value = (Decimal)m_HX.Value2;
            //numHX.Text = m_HX.Value.ToString();
            ChangeOccured();
        }

        private void SetupSources()
        {
            lstReactions.Items.Insert(0, m_Sources.LVI);
            if (lstReactions.ShowGroups)
                m_Sources.LVI.Group = lstReactions.Groups["grpSources"];
            UpdateReactionNumbers();
            ChangePosition(m_Sources.LVI, 0);
            m_Sources.NowChanged += new EventHandler(m_Sources_NowChanged);
        }

        void m_Sources_NowChanged(object sender, EventArgs e)
        {
            if (m_bUpdateSourceSinksBlock)
                txtSources.Text = m_Sources.CompoundString;
            txtSourceComments.Text = m_Sources.Comment;
            chkSourcesEnabled.Checked = m_Sources.Enabled;
            SourcesSinksChanged(this, new EventArgs());
            ChangeOccured();
        }

        private void chkSequence_CheckedChanged(object sender, EventArgs e)
        {
            m_bDoEvents = false;
            numSequence.Enabled = chkSequence.Checked;

            ListViewItem[] originalList = OrderedLVIs(lstReactions.Items);

            if (!chkSequence.Checked)
            {
                if (lstReactions.Columns.Contains(m_SequenceHeader))
                    lstReactions.Columns.Remove(m_SequenceHeader);
                /*foreach (ListViewGroup grp in lstReactions.Groups)
                    grp.Items.Clear();
                lstReactions.Items.Clear();
                lstReactions.Items.AddRange(originalList);*/

                menuShowSequence.Checked = false;

                /*foreach (ListViewItem lvi in originalList)
                    if (lvi.Tag is SimpleReaction)
                    {
                        SimpleReaction rxn = (SimpleReaction)lvi.Tag;
                        rxn.Sequence = 1;
                    }*/
            }
            else
            {
                if (!lstReactions.Columns.Contains(m_SequenceHeader))
                    lstReactions.Columns.Add(m_SequenceHeader);
                menuShowSequence.Checked = true;
                /*foreach (ListViewItem lvi in originalList)
                    if (lvi.Tag is SimpleReaction)
                        lvi.Group = lstReactions.Groups["grpSequence" + ((SimpleReaction)lvi.Tag).Sequence];
                    else if (lvi == m_SourcesLVI)
                        lvi.Group = lstReactions.Groups["grpSources"];
                    else if (lvi == m_SinksLVI)
                        lvi.Group = lstReactions.Groups["grpSinks"];
                    else if (lvi == m_HXLVI)
                        lvi.Group = lstReactions.Groups["grpHX"];*/
            }
            UpdateReactionNumbers();
            m_bDoEvents = true;

            ChangeOccured();
        }

        private void comboExtentType_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (m_CurrentReaction == null)
            {
                numExtentValue.Text = "";
                return;
            }
            m_CurrentReaction.ExtentType = (ExtentTypes) comboExtentType.SelectedIndex;
            foreach (Control ctrl in m_OptionalExtentControls)
                ctrl.Visible = false;
            this.comboExtentSpecie.Visible = true;
            numExtentValue.Text = m_CurrentReaction.ExtentInfo.Value.ToString();
            if (!chkFirstReactant.Checked)
            {
                if (m_CurrentReaction.ExtentInfo.Specie == null)
                    comboExtentSpecie.SelectedIndex = -1;
                else
                    comboExtentSpecie.SelectedItem = m_CurrentReaction.ExtentInfo.Specie;
            }
            else if (m_CurrentReaction.Reactants.Count > 0)
                comboExtentSpecie.SelectedItem = m_CurrentReaction.FirstReactant;
            else
                comboExtentSpecie.SelectedIndex = -1;


                lblExtentValue.Text = "Value";
            switch ((ExtentTypes)comboExtentType.SelectedIndex)
            {
                case ExtentTypes.Fraction:
                    lblExtentUnits.Text = "of";
                    break;
                case ExtentTypes.Ratio:
                    RatioExtent ratExtent = (RatioExtent) m_CurrentReaction.ExtentInfo;
                    lblExtentUnits.Text = "of";
                    lblExtentValue.Text = "Val 1:";
                    numExtentVal2.Visible =
                        numExtentVal3.Visible =
                        lblExtent2.Visible =
                        lblExtent3.Visible = true;
                    numExtentVal2.Text = ratExtent.Time.ToString();
                    numExtentVal3.Text = ratExtent.Ratio2.ToString();
                    lblExtent2.Text = "At Time";
                    lblExtent3.Text = "Val 1:";
                    break;
                case ExtentTypes.Equilibrium:
                    comboExtentSpecie.Visible = false;
                    lblExtentUnits.Text = "";
                    break;
                case ExtentTypes.FinalConc:
                    lblExtentUnits.Text = "g/L of";
                    lblExtent2.Visible = numExtentVal2.Visible = lblExtent3.Visible = true;
                    lblExtent2.Text = "At Temp";
                    numExtentVal2.Text = ((Final_ConcExtent) m_CurrentReaction.ExtentInfo).T.ToString();
                    lblExtent3.Text = "degC";
                    break;
                case ExtentTypes.FinalFrac:
                    lblExtentUnits.Text = "of";
                    chkExtentOption.Visible = true;
                    chkExtentOption.Checked = ((Final_FracExtent)m_CurrentReaction.ExtentInfo).ByPhase;
                    chkExtentOption.Text = "By Phase";
                    break;
                case ExtentTypes.Rate:
                    lblExtentUnits.Text = "Frac / s";
                    chkExtentOption.Visible = true;
                    chkExtentOption.Checked = ((RateExtent) m_CurrentReaction.ExtentInfo).Stabilised;
                    chkExtentOption.Text = "Stabilised";
                    break;
            }

        }

        private void FormulaBox_Leave(object sender, EventArgs e)
        {
            RichTextBox box = (RichTextBox)sender;
            if (m_CurrentReaction != null)
            {
                if (box == txtReactants)
                    box.Text = m_CurrentReaction.GetReactantsString();
                if (box == txtProducts)
                    box.Text = m_CurrentReaction.GetProductsString();
            }
        }

        private void comboDirection_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (m_CurrentReaction != null)
            {
                m_CurrentReaction.Direction = (RxnDirections)comboDirection.SelectedIndex;
            }
        }

        protected void SetHOREnabled(bool val)
        {
            comboHORSpecie.Enabled = val;
            comboHORConditions.Enabled = val;
            comboHORUnits.Enabled = val;
            numHORValue.Enabled = val;
        }

        private void chkHOROverride_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!chkHOROverride.Checked) //Default:
            {
                SetHOREnabled(false);
                comboHORSpecie.SelectedIndex = -1;
                comboHORConditions.SelectedItem = null;
                comboHORUnits.SelectedItem = "kJ/mol";
                /*if (m_CurrentReaction != null)
                {
                    m_CurrentReaction.HeatOfReactionValue = double.NaN;
                    numHORValue.Text = m_CurrentReaction.HeatOfReactionValue.ToString();
                }
                else*/
                numHORValue.Text = "";
            }
            else //Override:
            {
                SetHOREnabled(true);
                if (m_CurrentReaction != null)
                {
                    comboHORSpecie.SelectedItem = m_CurrentReaction.HeatOfReactionSpecie;
                    numHORValue.Text = m_CurrentReaction.HeatOfReactionValue.ToString();
                    if (m_CurrentReaction.HeatOfReactionType == FracTypes.ByMass)
                        comboHORUnits.SelectedItem = "kJ/kg";
                    else
                        comboHORUnits.SelectedItem = "kJ/mol";
                    comboHORConditions.SelectedItem = m_CurrentReaction.HeatOfReactionConditions;
                }
            }
        }

        private void comboHORSpecie_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (m_CurrentReaction != null)
                m_CurrentReaction.HeatOfReactionSpecie = (Compound)comboHORSpecie.SelectedItem;
        }

        private void numSequence_TextChanged(object sender, EventArgs e)
        {
            if (m_CurrentReaction != null)
            {
                m_CurrentReaction.Sequence = (int)numSequence.Value;
            }
        }

        private void btnMoveUp_Click(object sender, EventArgs e)
        {
            if (lstReactions.SelectedItems.Count == 0) return;

            ListViewItem curItem = lstReactions.SelectedItems[0];
            if (!lstReactions.ShowGroups)
            {
                int curIndex = lstReactions.SelectedIndices[0];
                if (curIndex > 0)
                    ChangePosition(curItem, curIndex - 1);
            }
            else
            {
                if (m_CurrentReaction.Sequence > 0)
                    m_CurrentReaction.Sequence--;
            }
            ChangeOccured();
        }

        private void comboHXType_SelectedIndexChanged(object sender, EventArgs e)
        {
            foreach (Control c in m_HXControls)
                c.Visible = false;
            if (m_HX != null)
                m_HX.Type = (HXReaction.HXTypes)comboHXType.SelectedIndex;
            switch ((HXReaction.HXTypes)comboHXType.SelectedIndex)
            {
                case HXReaction.HXTypes.FinalT:
                    lblHXUnits.Text = "degC"; lblHXUnits.Visible = true;
                    lblHXValue.Text = "Temperature"; lblHXValue.Visible = true;
                    numHX.Visible = true;
                    break;
                case HXReaction.HXTypes.ApproachT:
                    foreach (Control c in m_HXControls)
                        c.Visible = true;
                    lblHXUnits.Text = "degC,"; lblHXValue.Text = "Temperature";
                    break;
                case HXReaction.HXTypes.ApproachAmbient:
                    foreach (Control c in m_HXControls)
                        c.Visible = true;
                    lblHXUnits.Text = "degC,"; lblHXValue.Text = "Temperature";
                    break;
                case HXReaction.HXTypes.Power:
                    lblHXUnits.Text = "kJ/s"; lblHXUnits.Visible = true;
                    lblHXValue.Text = "Value"; lblHXValue.Visible = true;
                    numHX.Visible = true;
                    break;
                case HXReaction.HXTypes.Electrolysis:
                    lblHXUnits.Text = "%"; lblHXUnits.Visible = true;
                    lblHXValue.Text = "Value"; lblHXValue.Visible = true;
                    numHX.Visible = true;
                    break;
            }
            //ChangeOccured();
        }

        private void numHORValue_TextChanged(object sender, EventArgs e)
        {
            if (m_CurrentReaction == null || m_bLoading) return;
            double temp;
            double.TryParse(numHORValue.Text, out temp);
            m_CurrentReaction.HeatOfReactionValue = temp;
        }

        private void numExtentValue_TextChanged(object sender, EventArgs e)
        {
            if (m_CurrentReaction == null) return;
            double temp;
            double.TryParse(numExtentValue.Text, out temp);
            m_CurrentReaction.ExtentInfo.Value = temp;
        }

        private void txtFormula_Leave(object sender, EventArgs e)
        {
            if (m_CurrentReaction == null) return;
            m_CurrentReaction.SetString(txtFormula.Text);
        }

        private void comboExtentSpecie_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (m_CurrentReaction == null) return;
            m_CurrentReaction.ExtentInfo.Specie = (Compound) comboExtentSpecie.SelectedItem;
        }

        private void chkExtentOption_CheckedChanged(object sender, EventArgs e)
        {
            if (m_CurrentReaction == null) return;
            if (m_CurrentReaction.ExtentType == ExtentTypes.Rate)
                ((RateExtent)m_CurrentReaction.ExtentInfo).Stabilised = chkExtentOption.Checked;
            else if (m_CurrentReaction.ExtentType == ExtentTypes.FinalFrac)
                ((Final_FracExtent)m_CurrentReaction.ExtentInfo).ByPhase = chkExtentOption.Checked;
        }

        private void numExtentVal3_TextChanged(object sender, EventArgs e)
        {
            if (m_CurrentReaction == null || m_CurrentReaction.ExtentType != ExtentTypes.Ratio) return;
            double temp;
            if (!double.TryParse(numExtentVal3.Text, out temp) || numExtentVal3.Text.Length == 0)
                temp = double.NaN;
            ((RatioExtent)m_CurrentReaction.ExtentInfo).Ratio2 = temp;
        }

        private void numExtentVal2_TextChanged(object sender, EventArgs e)
        {
            if (m_CurrentReaction == null) return;
            double temp;
            if (!double.TryParse(numExtentVal2.Text, out temp) || numExtentVal2.Text.Length == 0)
                temp = double.NaN;

            if (m_CurrentReaction.ExtentType == ExtentTypes.Ratio)
                ((RatioExtent)m_CurrentReaction.ExtentInfo).Time = temp;
            if (m_CurrentReaction.ExtentType == ExtentTypes.FinalConc)
                ((Final_ConcExtent)m_CurrentReaction.ExtentInfo).T = temp;
        }

        private void txtProducts_DragEnter(object sender, DragEventArgs e)
        {
            if (m_CurrentReaction == null || !m_CurrentReaction.ProductsOk)
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            if (e.Data.GetDataPresent(typeof(Compound)))
            {
                if (m_CurrentReaction.Compounds.Contains((Compound)e.Data.GetData(typeof(Compound))))
                    e.Effect = DragDropEffects.None;
                else
                    e.Effect = DragDropEffects.Link;
            }
            else if (e.Data.GetDataPresent(typeof(CompoundDrag)) &&
                ((CompoundDrag)e.Data.GetData(typeof(CompoundDrag))).frm == this)
                e.Effect = DragDropEffects.Move;
            else
                e.Effect = DragDropEffects.None;
        }

        private void txtProducts_DragDrop(object sender, DragEventArgs e)
        {
            if (m_CurrentReaction == null || !m_CurrentReaction.ProductsOk)
            {
                e.Effect = DragDropEffects.None;
                return;
            }

            Point clientPoint = txtProducts.PointToClient(new Point(e.X, e.Y));
            int chIndex = txtProducts.GetCharIndexFromPosition(clientPoint);
            if (e.Data.GetDataPresent(typeof(Compound)))
            {
                Compound compound = (Compound)e.Data.GetData(typeof(Compound));
                if (m_CurrentReaction.Compounds.Contains(compound))
                {
                    e.Effect = DragDropEffects.None;
                    return;
                }
                m_CurrentReaction.AddProduct(compound, 1.0, chIndex);
                e.Effect = DragDropEffects.Link;
                txtProducts.Select();
                txtProducts.Select(m_CurrentReaction.LastAddedNumStart, 1);
            }
            else if (e.Data.GetDataPresent(typeof(CompoundDrag)) &&
                ((CompoundDrag)e.Data.GetData(typeof(CompoundDrag))).frm == this)
            {
                CompoundDrag data = (CompoundDrag)e.Data.GetData(typeof(CompoundDrag));
                if (data.ctrl == txtProducts)
                    m_CurrentReaction.MoveProduct(data.startIndex, chIndex);
                else
                    m_CurrentReaction.MoveReactantToProducts(data.startIndex, chIndex);
            }
            else
            {
                e.Effect = DragDropEffects.None;
                return;
            }
        }

        private void txtReactants_DragEnter(object sender, DragEventArgs e)
        {
            if (m_CurrentReaction == null || !m_CurrentReaction.ReactantsOk)
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            if (e.Data.GetDataPresent(typeof(Compound)))
            {
                if (m_CurrentReaction.Compounds.Contains((Compound)e.Data.GetData(typeof(Compound))))
                    e.Effect = DragDropEffects.None;
                else
                    e.Effect = DragDropEffects.Link;
            }
            else if (e.Data.GetDataPresent(typeof(CompoundDrag)) &&
                ((CompoundDrag)e.Data.GetData(typeof(CompoundDrag))).frm == this)
                e.Effect = DragDropEffects.Move;
            else
                e.Effect = DragDropEffects.None;
        }

        private void txtReactants_DragDrop(object sender, DragEventArgs e)
        {
            if (m_CurrentReaction == null || !m_CurrentReaction.ReactantsOk)
            {
                e.Effect = DragDropEffects.None;
                return;
            }

            Point clientPoint = txtReactants.PointToClient(new Point(e.X, e.Y));
            int chIndex = txtReactants.GetCharIndexFromPosition(clientPoint);
            if (e.Data.GetDataPresent(typeof(Compound)))
            {
                e.Effect = DragDropEffects.Link;
                Compound compound = (Compound)e.Data.GetData(typeof(Compound));
                m_CurrentReaction.AddReactant(compound, 1.0, chIndex);
                txtReactants.Select();
                txtReactants.Select(m_CurrentReaction.LastAddedNumStart, 1);
            }
            else if (e.Data.GetDataPresent(typeof(CompoundDrag)) &&
                ((CompoundDrag)e.Data.GetData(typeof(CompoundDrag))).frm == this)
            {
                CompoundDrag data = (CompoundDrag)e.Data.GetData(typeof(CompoundDrag));
                if (data.ctrl == txtReactants)
                    m_CurrentReaction.MoveReactant(data.startIndex, chIndex);
                else
                    m_CurrentReaction.MoveProductToReactants(data.startIndex, chIndex);
            }
            else
                e.Effect = DragDropEffects.None;
        }

        private void btnMoveDown_Click(object sender, EventArgs e)
        {
            if (lstReactions.SelectedItems.Count == 0) return;

            ListViewItem curItem = lstReactions.SelectedItems[0];
            if (!lstReactions.ShowGroups)
            {
                int curIndex = lstReactions.SelectedIndices[0];
                if (curIndex < lstReactions.Items.Count - 1)
                    ChangePosition(curItem, curIndex + 1);
            }
            else
            {
                if (m_CurrentReaction.Sequence < sMaxSequences)
                    m_CurrentReaction.Sequence++;
            }
            ChangeOccured();
        }

        private void pnlReaction_Resize(object sender, EventArgs e)
        {
            const int outsidePadding = 3;
            const int spacing = 6;
            int comboWidth = comboDirection.Width;
            int btnSpace = btnBalance.Width + spacing;
            int width = pnlReaction.ClientSize.Width - btnSpace;
            txtProducts.Width = txtReactants.Width = (width - comboWidth) / 2 - (outsidePadding + spacing);
            comboDirection.Left = txtReactants.Right + spacing;
            txtProducts.Left = comboDirection.Right + spacing;
            //txtReactants.Left = outsidePadding + 2 * spacing + comboWidth + txtReactants.Width;
        }

        private void chkEnabled_CheckedChanged(object sender, EventArgs e)
        {
            if (m_CurrentReaction != null)
            {
                m_CurrentReaction.Enabled = chkEnabled.Checked;
                UpdateReactionNumbers();
            }
        }

        private void txtComment_TextChanged(object sender, EventArgs e)
        {
            if (m_CurrentReaction != null)
                m_CurrentReaction.Comment = txtComment.Text;
        }

        private void comboHORUnits_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (m_CurrentReaction == null) return;
            if ((string)comboHORUnits.SelectedItem == "kJ/mol")
                m_CurrentReaction.HeatOfReactionType = FracTypes.ByMole;
            else if ((string)comboHORUnits.SelectedItem == "kJ/kg")
                m_CurrentReaction.HeatOfReactionType = FracTypes.ByMass;
        }

        private void btnClone_Click(object sender, EventArgs e)
        {
            if (m_CurrentReaction == null) return;
            SimpleReaction rxn = AddReaction(m_CurrentReaction, -1);
            rxn.LVI.Selected = true;
            if (ReactionAdded != null)
                ReactionAdded(this, rxn);
        }

        private void btnRemove_Click(object sender, EventArgs e)
        {
            if (lstReactions.SelectedItems.Count == 0)
                return;
            ListViewItem lvi = lstReactions.SelectedItems[0];
            lstReactions.SelectedItems[0].Remove();

            if (lvi.Tag is SimpleReaction)
            {
                SimpleReaction rxn = (SimpleReaction)lvi.Tag;
                if (ReactionRemoved != null)
                    ReactionRemoved(this, rxn);
            }
            else if (lvi.Tag == m_HX)
            {
                lstReactions.Items.Remove(m_HX.LVI);
                m_HX = null;
            }
            else if (lvi.Tag == m_Sinks)
            {
                lstReactions.Items.Remove(m_Sinks.LVI);
                m_Sinks = null;
                if (SourcesSinksChanged != null)
                    SourcesSinksChanged(this, new EventArgs());
            }
            else if (lvi.Tag == m_Sources)
            {
                lstReactions.Items.Remove(m_Sources.LVI);
                m_Sources = null;
                if (SourcesSinksChanged != null)
                    SourcesSinksChanged(this, new EventArgs());
            }
                
            ChangeOccured();
            UpdateReactionNumbers();
        }

        private void txtSources_TextChanged(object sender, EventArgs e)
        {
            m_bUpdateSourceSinksBlock = false;
            if (m_Sources != null)
                m_Sources.SetString(txtSources.Text);

            bool bWasActive = Log.Active;
            Log.Active = false;
            ColourCompounds((RichTextBox)sender);
            Log.Active = bWasActive;
            m_bUpdateSourceSinksBlock = true;
        }

        bool m_bUpdateSourceSinksBlock = true;
        private void txtSinks_TextChanged(object sender, EventArgs e)
        {
            m_bUpdateSourceSinksBlock = false;
            if (m_Sinks != null)
                m_Sinks.SetString(txtSinks.Text);
            bool bWasActive = Log.Active;
            Log.Active = false;
            ColourCompounds((RichTextBox)sender);
            Log.Active = bWasActive;
            m_bUpdateSourceSinksBlock = true;
        }

        private void numHX_TextChanged(object sender, EventArgs e)
        {
            if (m_HX != null)
                try { m_HX.Value = double.Parse(numHX.Text); }
                catch { }
            //ChangeOccured();
        }

        private void txtDescription_TextChanged(object sender, EventArgs e)
        {
            ChangeOccured();
        }

        private void FrmReaction_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (m_File != null)
                m_File.Close();
        }

        private void lstReactions_ItemDrag(object sender, ItemDragEventArgs e)
        {
            if (((ListViewItem)e.Item).Tag is SimpleReaction)
                DoDragDrop(((ListViewItem)e.Item).Tag, DragDropEffects.Move | DragDropEffects.Copy);
        }

        private void lstReactions_DragOver(object sender, DragEventArgs e)
        {
            if (!e.Data.GetDataPresent(typeof(SimpleReaction)))
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            SimpleReaction rxn = (SimpleReaction)e.Data.GetData(typeof(SimpleReaction));
            if (this.ContainsReaction(rxn))
                if ((e.AllowedEffect & DragDropEffects.Move) != 0)
                    e.Effect = DragDropEffects.Move;
                else
                {
                    e.Effect = DragDropEffects.None;
                    return;
                }
            else
                e.Effect = DragDropEffects.Copy;
            Point p = lstReactions.PointToClient(new Point(e.X, e.Y));
            ListViewHitTestInfo hti = lstReactions.HitTest(p);
            ListViewItem insertionItem = FindLVIBelow(p);
            if (!lstReactions.ShowGroups)
            { }
            else if (insertionItem == null) //It's in the lower portion of the list, so find the highest group with an item in it:
            {
                int i;
                for (i = sMaxSequences; i > 0 && lstReactions.Groups["grpSequence" + i].Items.Count == 0; i--) ;
                SelectedGroup = lstReactions.Groups["grpSequence" + (i)];
            }
            else                        //It's over an item, we'll move / add it to that group.
            {
                SelectedGroup = insertionItem.Group;
            }
        }

        private void lstReactions_DragLeave(object sender, EventArgs e)
        {
            SelectedGroup = null;
        }

        private void lstReactions_DragDrop(object sender, DragEventArgs e)
        {
            if (!e.Data.GetDataPresent(typeof(SimpleReaction)))
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            SimpleReaction rxn = (SimpleReaction)e.Data.GetData(typeof(SimpleReaction));
            if (this.ContainsReaction(rxn))
                if ((e.AllowedEffect & DragDropEffects.Move) != 0)
                    e.Effect = DragDropEffects.Move;
                else
                {
                    e.Effect = DragDropEffects.None;
                    return;
                }
            else
                e.Effect = DragDropEffects.Copy;
            Point p = lstReactions.PointToClient(new Point(e.X, e.Y));
            ListViewHitTestInfo hti = lstReactions.HitTest(p);
            ListViewItem insertionItem = FindLVIBelow(p);
            if (!lstReactions.ShowGroups)
            {
                int newLoc = insertionItem != null ? newLoc = insertionItem.Index : lstReactions.Items.Count;


                if (this.ContainsReaction(rxn))
                {
                    if (newLoc > MaxReactionLocation - 1) newLoc = MaxReactionLocation - 1;
                    ChangePosition(rxn.LVI, newLoc);
                }
                else
                {
                    if (newLoc > MaxReactionLocation) newLoc = MaxReactionLocation;
                    AddReaction(rxn, newLoc);
                }
            }
            else if (insertionItem == null) //It's in the lower portion of the list, so find the highest group with an item in it:
            {
                int i;
                for (i = sMaxSequences; i > 1 && lstReactions.Groups["grpSequence" + i].Items.Count == 0; i--) ;
                if (this.ContainsReaction(rxn))
                    rxn.Sequence = i;
                else
                    AddReaction(rxn, -1).Sequence = i;              
            }
            else                        //It's over an item, we'll move / add it to that group.
            {
                if (this.ContainsReaction(rxn))
                    rxn.Sequence = ((SimpleReaction)insertionItem.Tag).Sequence;
                else
                    AddReaction(rxn, -1).Sequence = ((SimpleReaction)insertionItem.Tag).Sequence;
            }
            SelectedGroup = null;
        }


        private void txtSources_Leave(object sender, EventArgs e)
        {
            if (m_Sources != null)
                txtSources.Text = m_Sources.CompoundString;
        }

        private void txtSinks_Leave(object sender, EventArgs e)
        {
            if (m_Sinks != null)
                txtSinks.Text = m_Sinks.CompoundString;
        }

        void txtCompounds_DragEnter(object sender, DragEventArgs e)
        {
            RichTextBox box = (RichTextBox)sender;
            if (!e.Data.GetDataPresent(typeof(Compound)))
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            Compound newComp = (Compound)e.Data.GetData(typeof(Compound));
            //Determine if we already have the compound in the textbox:
            if (Regex.Match(box.Text, @"(^|,|\n)\s*" + Regex.Escape(newComp.Symbol) + @"\s*(,|$)", RegexOptions.ExplicitCapture).Success)
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            e.Effect = DragDropEffects.Link;
        }

        void txtCompounds_DragDrop(object sender, DragEventArgs e)
        {
            RichTextBox box = (RichTextBox)sender;
            if (!e.Data.GetDataPresent(typeof(Compound)))
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            Compound newComp = (Compound)e.Data.GetData(typeof(Compound));
            //Determine if we already have the compound in the textbox:
            if (Regex.Match(box.Text, @"(^|,|\n)\s*" + Regex.Escape(newComp.Symbol) + @"\s*(,|$)", RegexOptions.ExplicitCapture).Success)
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            e.Effect = DragDropEffects.Link;
            if (box == txtSinks)
                AddSinks(newComp);
            else
                AddSources(newComp);
            ChangeOccured();
            if (SourcesSinksChanged != null)
                SourcesSinksChanged(this, new EventArgs());
        }

        private void numHXApproach_ValueChanged(object sender, EventArgs e)
        {
            if (m_HX != null)
                m_HX.Value2 = (double)numHXApproach.Value;
            //ChangeOccured();
        }

        private void chkFirstReactant_CheckedChanged(object sender, EventArgs e)
        {
            if (chkFirstReactant.Checked)
            {
                foreach (ListViewItem lvi in lstReactions.Items)
                    if (lvi.Tag is SimpleReaction)
                    {
                        SimpleReaction rxn = (SimpleReaction)lvi.Tag;
                        rxn.ExtentInfo.Specie = rxn.FirstReactant;
                    }
                comboExtentSpecie.SelectedItem = m_CurrentReaction != null ?
                    m_CurrentReaction.FirstReactant : null;
            }
            comboExtentSpecie.Enabled = !chkFirstReactant.Checked;
            ChangeOccured();
        }

        private void comboHORConditions_SelectedIndexChanged(object sender, EventArgs e)
        {
            numHORP.Visible = numHORT.Visible = lblHORP.Visible = lblHORT.Visible = lblHORPUnits.Visible = false;
            if (m_CurrentReaction == null)
                return;
            m_CurrentReaction.HeatOfReactionConditions = comboHORConditions.SelectedItem == null ? TPConditions.Feed : (TPConditions) comboHORConditions.SelectedItem;
            if (m_CurrentReaction.HeatOfReactionConditions == TPConditions.Custom)
            {
                numHORP.Visible = numHORT.Visible = lblHORP.Visible = lblHORT.Visible = lblHORPUnits.Visible = true;
                numHORP.Text = m_CurrentReaction.HeatOfReactionP.ToString();
                numHORT.Text = m_CurrentReaction.HeatOfReactionT.ToString();
            }
        }

        private void numHORT_TextChanged(object sender, EventArgs e)
        {
            if (m_CurrentReaction == null) return;
            double temp;
            double.TryParse(numHORT.Text, out temp);
            m_CurrentReaction.HeatOfReactionT = temp;
        }

        private void numHORP_TextChanged(object sender, EventArgs e)
        {
            if (m_CurrentReaction == null) return;
            double temp;
            double.TryParse(numHORP.Text, out temp);
            m_CurrentReaction.HeatOfReactionP = temp;
        }

        private void btnBalance_Click(object sender, EventArgs e)
        {
            if (m_CurrentReaction == null) return;
            try
            {
                List<Compound> extraComps = new List<Compound>();
                string note = "Reaction is over-constrained, and requires additional compounds to allow autobalancing. Please specify a compound set to allow autobalancing.";
                while (!m_CurrentReaction.IsBalanceable(extraComps))
                {
                    Program.FrmAutobalanceExtraComps.Note = note;
                    note = "Previously selected set did not contain suitable compounds to allow autobalancing. Please adjust the compound set, or choose a different set.";
                    if (Program.FrmAutobalanceExtraComps.ShowDialog(this) == DialogResult.Cancel)
                    {
                        Program.FrmAutobalanceExtraComps.Note = "";
                        return;
                    }
                    Program.FrmAutobalanceExtraComps.Note = "";
                    extraComps = Compound.FromCommaList(Program.FrmAutobalanceExtraComps.SelectedList);
                }
                //Matrix.RemovalInfo info = m_CurrentReaction.BalanceOptions(extraComps);
                //if (!info.m_bCanRemove)
                //    MessageBox.Show("Unable to balance reaction - no solutions available without forcing compounds to change sides of the reaction", "Autobalance", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                //else
                Matrix info = m_CurrentReaction.GetBalanceMatrix(extraComps);
                {
                    if (info.DegreesOfFreedom > 0)
                    {
                        FrmBalanceOptions2 frm = new FrmBalanceOptions2(m_CurrentReaction, extraComps);
                        if (frm.ShowDialog(this) == DialogResult.OK)
                        {
                            SimpleReaction demo = m_CurrentReaction.Clone();
                            demo.SetCoefficients(frm.Coefficients, extraComps);
                            if (FrmChangeDialogue.Show(m_CurrentReaction, demo, "Autobalancer", "Balance Result:", "New Reaction", false, false) == DialogResult.OK) 
                                m_CurrentReaction.SetCoefficients(frm.Coefficients, extraComps);
                        }
                        frm.Dispose();
                    }
                    else
                    {
                        SimpleReaction demo = m_CurrentReaction.Clone();
                        demo.BalanceWith(extraComps);
                        if (FrmChangeDialogue.Show(m_CurrentReaction, demo, "Autobalancer", "Balance Result:", "New Reaction", false, false) == DialogResult.OK)
                            m_CurrentReaction.BalanceWith(extraComps);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Autobalancer");
            }
        }

        private void txtFormula_TextChanged(object sender, EventArgs e)
        {
            m_bUpdateTextboxes = false;
            BoxAutoComplete box = (BoxAutoComplete)sender;
            box.SelectionAlignment = HorizontalAlignment.Center;
            int oldSelectStart = box.SelectionStart;
            int oldSelectLen = box.SelectionLength;
            box.SelectAll();
            box.SelectionColor = box.ForeColor;
            box.SelectionBackColor = box.BackColor;
            Match m = SimpleReaction.s_CompoundRegex.Match(box.Text);
            foreach (Capture c in m.Groups["Unparseable"].Captures)
            {
                box.Select(c.Index, c.Length);
                box.SelectionBackColor = Color.DarkRed;
                box.SelectionColor = Color.White;
            }
            foreach (Capture c in m.Groups["Compound"].Captures)
                if (!Compound.Contains(c.Value.Trim()))
                {
                    box.Select(c.Index, c.Length);
                    box.SelectionBackColor = Color.DarkRed;
                    box.SelectionColor = Color.White;
                }

            if (m_CurrentReaction != null)
            {
                if (box == txtReactants)
                    m_CurrentReaction.ParseReactants(box.Text);
                if (box == txtProducts)
                    m_CurrentReaction.ParseProducts(box.Text);
            }

            box.Select(oldSelectStart, oldSelectLen);
            m_bUpdateTextboxes = true;
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            if (!m_bActualActive) 
            {
                Messaging.SendMessage(Handle, Messaging.WM_NCACTIVATE, (IntPtr)0, (IntPtr)0);
            }
            base.OnPaint(e);
        }

        protected static Regex s_NumRegexLtoR = new Regex(@"(\s|\+|^)\d+(\.\d+)?|\.\d+", RegexOptions.Compiled);
        protected static Regex s_NumRegexRtoL = new Regex(@"(\s|\+|^)\d+(\.\d+)?|\.\d+", RegexOptions.Compiled | RegexOptions.RightToLeft);
        protected static TimeSpan s_TabWindow = new TimeSpan(0, 0, 0, 0, 50);
        protected DateTime lastTab = DateTime.MinValue;
        protected DateTime lastShiftTab = DateTime.MinValue;
        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            bool handled = false;
            BoxAutoComplete box = null;
            if (txtProducts.Focused)
                box = txtProducts;
            else if (txtReactants.Focused)
                box = txtReactants;

            if (box != null)
            {
                if (keyData == (Keys.Tab | Keys.Shift))
                {
                    Match m = s_NumRegexRtoL.Match(box.Text, box.SelectionStart);
                    if (m.Success)
                    {
                        handled = true;
                        box.Select(m.Index, m.Length);
                    }
                }
                else if (keyData == Keys.Tab)
                {
                    Match m = s_NumRegexLtoR.Match(box.Text, box.SelectionStart + box.SelectionLength);
                    if (m.Success)
                    {
                        handled = true;
                        box.Select(m.Index, m.Length);
                        box.HideSuggestions();
                    }
                }
            }

            if (keyData == (Keys.Tab | Keys.Shift))
                lastShiftTab = DateTime.Now;
            if (keyData == (Keys.Tab))
                lastTab = DateTime.Now;

            if (!handled)
                return base.ProcessCmdKey(ref msg, keyData);
            else
                return true;
        }

        private void FormulaBox_Enter(object sender, EventArgs e)
        {
            RichTextBox box = (RichTextBox)sender;
            if (DateTime.Now - lastTab < s_TabWindow)
            {
                Match m = s_NumRegexLtoR.Match(box.Text);
                if (m.Success)
                    box.Select(m.Index, m.Length);
            }
            else if (DateTime.Now - lastShiftTab < s_TabWindow)
            {
                Match m = s_NumRegexRtoL.Match(box.Text);
                if (m.Success)
                    box.Select(m.Index, m.Length);
            }
        }

        private void contextMenuStrip1_Opening(object sender, CancelEventArgs e)
        {
            menuClone.Enabled = menuRemove.Enabled = m_CurrentReaction != null;
            menuCopy.Enabled = menuCut.Enabled = CanCutCopyReaction;
            menuPaste.Enabled = CanPasteReaction;
            menuRevert.Enabled = CanRevert;
            menuShowSequence.Enabled = chkSequence.Checked;
        }

        private void menuShowSequence_CheckedChanged(object sender, EventArgs e)
        {
            if (menuShowSequence.Checked)
            {
                lstReactions.ShowGroups = true;
                foreach (ListViewItem lvi in lstReactions.Items)
                    if (lvi.Tag is SimpleReaction)
                        lvi.Group = lstReactions.Groups["grpSequence" + ((SimpleReaction)lvi.Tag).Sequence];
                    else if (lvi.Tag == m_Sources)
                        lvi.Group = lstReactions.Groups["grpSources"];
                    else if (lvi.Tag == m_Sinks)
                        lvi.Group = lstReactions.Groups["grpSinks"];
                    else if (lvi.Tag == m_HX)
                        lvi.Group = lstReactions.Groups["grpHX"];
            }
            else
            {
                ListViewItem[] originalList = OrderedLVIs(lstReactions.Items);
                lstReactions.ShowGroups = false;
                ListViewItem selectedItem = null;
                if (lstReactions.SelectedItems.Count > 0)
                    selectedItem = lstReactions.SelectedItems[0];
                lstReactions.Items.Clear();
                lstReactions.Items.AddRange(originalList);
                if (selectedItem != null)
                    selectedItem.Selected = true;
            }
            UpdateReactionNumbers();
        }

        private void lstReactions_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Delete)
                btnRemove_Click(sender, e);
        }

        private void chkSourcesEnabled_CheckedChanged(object sender, EventArgs e)
        {
            if (m_Sources != null)
                m_Sources.Enabled = chkSourcesEnabled.Checked;
            UpdateReactionNumbers();
        }

        private void chkSinksEnabled_CheckedChanged(object sender, EventArgs e)
        {
            if (m_Sinks != null)
                m_Sinks.Enabled = chkSinksEnabled.Checked;
            UpdateReactionNumbers();
        }

        private void chkHXEnabled_CheckedChanged(object sender, EventArgs e)
        {
            m_HX.Enabled = chkHXEnabled.Checked;
            UpdateReactionNumbers();
        }

        private void txtFormula_CompoundDragged(object sender, EventArgs e)
        {
            DoDragDrop(new CompoundDrag((Control)sender, ((BoxAutoComplete)sender).SelectionStart, this), DragDropEffects.Move);
        }

        private void txtReactants_PreCompSelect(object sender, CancelEventArgs e)
        {
            FormulaBox_Leave(sender, e);
            if (!m_CurrentReaction.ReactantsOk)
                e.Cancel = true;
        }

        private void txtProducts_PreCompSelect(object sender, CancelEventArgs e)
        {
            FormulaBox_Leave(sender, e);
            if (!m_CurrentReaction.ProductsOk)
                e.Cancel = true;
        }

        private void menuRevert_Click(object sender, EventArgs e)
        {
            RevertCurrent();
        }

        private void pnlReaction_DragEnter(object sender, DragEventArgs e)
        {
            e.Effect = DragDropEffects.None;
        }

        private void menuCut_Click(object sender, EventArgs e)
        {
            Cut();
        }

        private void menuCopy_Click(object sender, EventArgs e)
        {
            Copy();
        }

        private void menuPaste_Click(object sender, EventArgs e)
        {
            Paste();
        }

        private void txtSinkComments_TextChanged(object sender, EventArgs e)
        {
            m_Sinks.Comment = txtSinkComments.Text;
        }

        private void txtSourceComments_TextChanged(object sender, EventArgs e)
        {
            m_Sources.Comment = txtSourceComments.Text;
        }

        private void txtHXComment_TextChanged(object sender, EventArgs e)
        {
            m_HX.Comment = txtHXComment.Text;
        }
    }
}