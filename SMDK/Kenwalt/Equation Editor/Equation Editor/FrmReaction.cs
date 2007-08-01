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
using System.Collections.Specialized;
using Be.Windows.Forms;
using Auto_Complete;

namespace Reaction_Editor
{
    public partial class FrmReaction : Form
    {
        #region Regex's
        protected static Regex s_CompoundSeperator = new Regex(@"^((?<Comp>[^,\r\n]*)(?<Delim>,(\r?\n)?|\r?\n|$))*?$",
            RegexOptions.Compiled | RegexOptions.ExplicitCapture);

        protected static Regex s_LastSelectedRegex = new Regex(@"<LastSelected\s*=\s*(?<Value>\d+|None)>",
            RegexOptions.Compiled | RegexOptions.IgnoreCase);
        protected static Regex s_EndRegex = new Regex(@"(^|\r\n)\s*End",
            RegexOptions.Compiled | RegexOptions.IgnoreCase | RegexOptions.ExplicitCapture);
        protected static Regex s_CommentRemovingRegex = new Regex(@"^[^;]*",
            RegexOptions.ExplicitCapture | RegexOptions.Multiline | RegexOptions.Compiled);
        protected static Regex s_OpeningCommentRegex = new Regex(@"^(\s*(?>;(?<Comment>[^\r\n]*))?)*",
            RegexOptions.ExplicitCapture | RegexOptions.Compiled); //If the first match is non-zero, discard.
        protected static Regex s_SourceRegex = new Regex(@"(?<=(^|\r\n)\s*?Source:)[^:]*(?=Reactions:)",
            RegexOptions.IgnoreCase | RegexOptions.ExplicitCapture | RegexOptions.Compiled);
        public static Regex s_ReactionRegex = new Regex(
            @"(^|\r\n)\s*((?<Comment>;.*)\r\n)?(?<Reactants>[^;\r\n<>=\-:]*)(?<Direction>->|=|<->|->)\s*(?<Products>[^;:\r\n]*?(?=Extent|Sequence|HeatOfReaction|;|\r\n|$))(?>(?>\s*(;.*\r\n)?)*((?<Extent>Extent\s*:[^\r\n;]*?)|(?<Sequence>Sequence\s*:[^\r\n;]*?)|(?<HOR>HeatOfReaction\s*:[^\r\n;]*?))(?=Extent|Sequence|HeatOfReaction|;|\r\n|$)){0,3}",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled);
        protected static Regex s_DisabledReactionRegex = new Regex(
            @"(^|\r\n)\s*((?<Comment>;.*)\r\n[^\S\r\n]*)?;(?<Reactants>[^;\r\n<>=\-:]*)(?<Direction>->|=|<->|->)\s*(?<Products>[^<>=\-;:\r\n]*?(?=Extent|Sequence|HeatOfReaction|;|\r\n|$))(?>[^\S\r\n]*(\r\n\s*;[^\S\r\n]*)?((?<Extent>Extent\s*:[^\r\n;]*?)|(?<Sequence>Sequence\s*:[^\r\n;]*?)|(?<HOR>HeatOfReaction\s*:[^\r\n;]*?))(?=Extent|Sequence|HeatOfReaction|;|\r\n|$)){0,3}",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled);
        protected static Regex s_SinkRegex = new Regex(@"(?<=\r\n\s*?Sink:)[^:]*?(?=End|HeatExchange|$)",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled);
        protected static Regex s_HXRegex = new Regex(
            @"^\s*HeatExchange:\s*(
                (?<Type>FinalT|Power|Electrolysis)\s*=\s*(?<Value>\d+(\.\d+)?|\.\d+) |
                (?<Type>TargetT|Ambient)\s*=\s*(?<Value>\d+(\.\d+)?|\.\d+)\s*,\s*(ApproachT|ApproachAmbient)\s*=\s*(?<Value2>d+(\.\d+)?|\.\d+))",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Multiline | RegexOptions.Compiled | RegexOptions.IgnorePatternWhitespace);
        protected static Regex s_RxnBlockStartRegex = new Regex(@"(^|\r\n\s*)Reactions:",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled);
        protected static Regex s_RxnBlockEndRegex = new Regex(@"\r\n\s*(End|^|HeatExchange|Sink)",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled);
        #endregion Regex's

        #region Variables
        protected ColumnHeader m_SequenceHeader = new ColumnHeader();
        protected bool m_bChanged = false;
        protected bool m_bLoading = false;
        protected string m_sTitle = "Untitled Reaction Block";
        protected FileStream m_File;
        protected SimpleReaction m_CurrentReaction;

        protected ILog m_Log;

        protected bool m_bDoEvents = true;
        protected List<Control> m_OptionalExtentControls = new List<Control>();
        protected Control[] m_HXControls;

        public const int sMaxSequences = 32;

        protected ListViewGroup m_selectedGroup = null;

        protected List<Compound> m_SourcesCache = new List<Compound>();
        protected List<Compound> m_SinksCache = new List<Compound>();

        //It's only ever used in adding a new reaction. And non-software people don't like zero referenced arrays.
        protected int ReactionNo
        {
            get { return lstReactions.Items.Count + 1; }
        }

        protected bool m_bActualActive = true;
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
                    ret.Add((SimpleReaction)lvi.Tag);
                return ret;
            }
        }

        public ILog Log
        {
            get { return m_Log; }
            set { m_Log = value; }
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
                return 
                    (lstReactions.Focused && lstReactions.SelectedItems.Count > 0) ||
                    (txtComment.Focused && !string.IsNullOrEmpty(txtComment.SelectedText)) ||
                    (txtDescription.Focused && !string.IsNullOrEmpty(txtDescription.SelectedText)) ||
                    (txtSources.Focused && !string.IsNullOrEmpty(txtSources.SelectedText)) ||
                    (txtSinks.Focused && !string.IsNullOrEmpty(txtSinks.SelectedText)) ||
                    (txtProducts.Focused && !string.IsNullOrEmpty(txtProducts.SelectedText)) ||
                    (txtReactants.Focused && !string.IsNullOrEmpty(txtReactants.SelectedText));
            }
        }

        public bool CanPaste
        {
            get
            {
                IDataObject obj = Clipboard.GetDataObject();
                return obj.GetDataPresent(typeof(SimpleReaction)) ||
                    (Clipboard.ContainsText() && (
                    txtComment.Focused || txtDescription.Focused || txtSources.Focused || txtSinks.Focused ||
                    txtProducts.Focused || txtReactants.Focused));
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

        public List<Compound> SourceCompounds { get { return m_SourcesCache; } }

        public List<Compound> SinkCompounds { get { return m_SinksCache; } }

        public string StatusMessage
        {
            get
            {
                if (m_CurrentReaction == null)
                    return "No Reaction Selected.";
                string text = "Status: " + m_CurrentReaction.Status + ".";
                if (!m_CurrentReaction.ProductsOk)
                    text += " Invalid product string.";
                if (!m_CurrentReaction.ReactantsOk)
                    text += " Invalid reactant string.";
                if (!m_CurrentReaction.ExtentInfo.IsValid())
                    text += " Invalid extent specified.";
                if (!m_CurrentReaction.Balanced)
                {
                    text += " Excess product elements: ";
                    foreach (KeyValuePair<Element, double> kvp in m_CurrentReaction.UnbalancedDetails)
                        text += "{" + kvp.Key.Symbol + " = " + kvp.Value.ToString("+####0.##;-####0.##;\"Balanced\"") + "}, ";
                    text = text.Substring(0, text.Length - 2);
                }
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

        List<Auto_Complete.BoxAutoComplete> AutoCompleteBoxes = new List<Auto_Complete.BoxAutoComplete>();
        #endregion Properties

        #region Events
        public event EventHandler NowChanged;
        public event EventHandler CompoundsChanged;
        public event EventHandler ReactionChanged;
        public event EventHandler SourcesSinksChanged;
        public event ReactionHandler ReactionAdded;
        public event ReactionHandler ReactionRemoved;
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

            txtProducts.SelectAll(); txtReactants.SelectAll();
            txtReactants.SelectionAlignment = txtProducts.SelectionAlignment = HorizontalAlignment.Center;
            txtProducts.Select(0, 0); txtReactants.Select(0, 0);

            for (int i = 0; i <= sMaxSequences; i++)
            {
                ListViewGroup grp = new ListViewGroup("grpSequence" + i, "Sequence " + i);
                lstReactions.Groups.Add(grp);
            }

            comboHXType.SelectedItem = "None";
            comboDirection.SelectedItem = "=";

            tabSources_Sinks_Resize(null, new EventArgs());
            pnlReaction_Resize(null, new EventArgs());

            txtSources.AllowDrop = true;
            txtReactants.AllowDrop = true;

            txtSources.DragDrop += new DragEventHandler(txtCompounds_DragDrop);
            txtSinks.DragDrop += new DragEventHandler(txtCompounds_DragDrop);

            txtSources.DragEnter += new DragEventHandler(txtCompounds_DragEnter);
            txtSinks.DragEnter += new DragEventHandler(txtCompounds_DragEnter);

            numSequence.Text = "";

            Changed = false;

            AutoCompleteBoxes.AddRange(new BoxAutoComplete[] { txtSources, txtSinks, txtReactants, txtProducts });

            UpdateAutoCompleteForms();
        }
        #endregion Constructors

        #region Public Functions
        public void Save()
        {
            bool ok = true;
            bool balanced = true;
            foreach (ListViewItem lvi in lstReactions.Items)
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
                sw.WriteLine(";" + txtDescription.Text.TrimEnd().Replace("\r\n", "\r\n;"));
                sw.WriteLine();
            }

            //Sources, if they exist:
            if (!string.IsNullOrEmpty(txtSources.Text))
            {
                sw.WriteLine("Source: " + txtSources.Text.Replace("\r\n", ", "));
                sw.WriteLine();
            }
            
            if (chkFirstReactant.Checked)
                sw.WriteLine(";<UseFirstReactant=True>");

            //Reactions:
            sw.WriteLine("Reactions:");
            foreach (ListViewItem lvi in lstReactions.Items)
            {
                SimpleReaction rxn = (SimpleReaction)lvi.Tag;
                sw.WriteLine();
                if (!string.IsNullOrEmpty(rxn.Comment))
                    sw.WriteLine(";" + rxn.Comment);
                sw.WriteLine(rxn.ToSaveString(chkSequence.Checked));
            }
            sw.WriteLine();
            
            if ((HXTypes) comboHXType.SelectedIndex == HXTypes.Electrolysis ||
                (HXTypes) comboHXType.SelectedIndex == HXTypes.FinalT ||
                (HXTypes) comboHXType.SelectedIndex == HXTypes.Power)
            {
                sw.WriteLine("HeatExchange: " + (HXTypes)comboHXType.SelectedIndex + " = " + numHX.Text);
                sw.WriteLine();
            }
            else if ((HXTypes)comboHXType.SelectedIndex == HXTypes.ApproachAmbient)
            {
                sw.WriteLine("HeatExchange: Ambient = " + numHX.Text + ", ApproachAmbient = " + numHXApproach.Value.ToString());
                sw.WriteLine();
            }
            else if ((HXTypes)comboHXType.SelectedIndex == HXTypes.ApproachT)
            {
                sw.WriteLine("Heat Exchange: TargetT = " + numHX.Text + ", ApproachT = " + numHXApproach.Value.ToString());
                sw.WriteLine();
            }


            if (!string.IsNullOrEmpty(txtSinks.Text))
            {
                sw.WriteLine("Sink: " + txtSinks.Text.Replace("\r\n", ", "));
                sw.WriteLine();
            }

            sw.WriteLine("End");
            sw.Flush();

            string LastSelected = "None";
            if (lstReactions.SelectedIndices.Count > 0)
                LastSelected = lstReactions.SelectedIndices[0].ToString();
            sw.WriteLine(";<LastSelected=" + LastSelected + ">");

            Changed = false;

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
            lvi.SubItems.AddRange(new string[] { "", "", "" });
            if (location < 0 || location > lstReactions.Items.Count)
                lstReactions.Items.Add(lvi);
            else
                lstReactions.Items.Insert(location, lvi);
            SimpleReaction newRxn = rxn.Clone(lvi);
            newRxn.FireChanged();

            if (ReactionAdded != null)
                ReactionAdded(this, rxn);
            return newRxn;
        }

        public List<Compound> GetCompounds()
        {
            List<Compound> ret = new List<Compound>();
            foreach (ListViewItem lvi in lstReactions.Items)
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
                ((SimpleReaction)lvi.Tag).DoDatabaseChanged();
            txtSources_Leave(txtSources, new EventArgs());
            txtSinks_Leave(txtSinks, new EventArgs());
        }

        public void Copy()
        {
            if (lstReactions.Focused && lstReactions.SelectedItems.Count > 0)
                Clipboard.SetData(SimpleReaction.sFormat.Name, ((SimpleReaction)lstReactions.SelectedItems[0].Tag).ToSaveString(true));
            else if (txtComment.Focused)
                txtComment.Copy();
            else if (txtDescription.Focused)
                txtDescription.Copy();
            else if (txtSources.Focused)
                txtSources.Copy();
            else if (txtSinks.Focused)
                txtSinks.Copy();
            else if (txtProducts.Focused)
                txtProducts.Copy();
            else if (txtReactants.Focused)
                txtReactants.Copy();
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
                FindReactions(data, s_ReactionRegex, true);
                FindReactions(data, s_DisabledReactionRegex, false);
                chkSequence = properChkSequence;
                ChangeOccured();
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
            List<Compound> existingSources = SourceCompounds;
            foreach (Compound c in newSources)
            {
                if (!existingSources.Contains(c))
                {
                    if (s_EndsWithComma.Match(txtSources.Text).Success || string.IsNullOrEmpty(txtSources.Text.Trim()))
                        txtSources.AppendText(c.Symbol);
                    else
                        txtSources.AppendText(", " + c.Symbol);
                    m_SourcesCache.Add(c);
                }
            }
            ChangeOccured();
            if (SourcesSinksChanged != null)
                SourcesSinksChanged(this, new EventArgs());
        }

        public void AddSinks(params Compound[] newSinks)
        {
            List<Compound> existingSinks = SinkCompounds;
            foreach (Compound c in newSinks)
            {
                if (!existingSinks.Contains(c))
                {
                    if (s_EndsWithComma.Match(txtSinks.Text).Success || string.IsNullOrEmpty(txtSinks.Text.Trim()))
                        txtSinks.AppendText(c.Symbol);
                    else
                        txtSinks.AppendText(", " + c.Symbol);
                    m_SinksCache.Add(c);
                }
            }
            ChangeOccured();
            if (SourcesSinksChanged != null)
                SourcesSinksChanged(this, new EventArgs());
        }

        public void RemoveSinks(params Compound[] toRemove)
        {
            List<Compound> originalSinks = new List<Compound>(SinkCompounds);
            Match m = s_CompoundSeperator.Match(txtSinks.Text);
            CaptureCollection compCaptures = m.Groups["Comp"].Captures;
            CaptureCollection delimCaptures = m.Groups["Delim"].Captures;

            for (int i = compCaptures.Count - 1; i >= 0; i--)
                foreach (Compound c in toRemove)
                    if (originalSinks.Contains(c))
                    {
                        if (compCaptures[i].Value.Trim() == c.Symbol)
                        {
                            txtSinks.Select(compCaptures[i].Index, compCaptures[i].Length + delimCaptures[i].Length);
                            txtSinks.SelectedText = "";
                        }
                        m_SinksCache.Remove(c);
                    }
            ChangeOccured();
            if (SourcesSinksChanged != null)
                SourcesSinksChanged(this, new EventArgs());
        }

        public void RemoveSources(params Compound[] toRemove)
        {
            List<Compound> originalSources = new List<Compound>(SourceCompounds);
            Match m = s_CompoundSeperator.Match(txtSources.Text);
            CaptureCollection compCaptures = m.Groups["Comp"].Captures;
            CaptureCollection delimCaptures = m.Groups["Delim"].Captures;

            for (int i = compCaptures.Count - 1; i >= 0; i--)
                foreach (Compound c in toRemove)
                    if (originalSources.Contains(c))
                    {
                        if (compCaptures[i].Value.Trim() == c.Symbol)
                        {
                            txtSources.Select(compCaptures[i].Index, compCaptures[i].Length + delimCaptures[i].Length);
                            txtSources.SelectedText = "";
                        }
                        m_SourcesCache.Remove(c);
                    }
            ChangeOccured();
            if (SourcesSinksChanged != null)
                SourcesSinksChanged(this, new EventArgs());
        }
        #endregion Public Functions

        #region Protected Functions
        protected void UpdateAutoCompleteForms()
        {
            txtReactants.HitCounts.Clear();
            foreach (BoxAutoComplete box in AutoCompleteBoxes)
            {
                box.HitCounts = txtReactants.HitCounts;
                box.Items.Clear();
                foreach (Compound c in Compound.CompoundList.Values)
                    box.Items.Add(c);
            }
        }

        protected bool PasteText(Control c)
        {
            if ((c.GetType() == typeof(TextBox) || c.GetType() == typeof(RichTextBoxEx))
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
                //Now, stopping at the End token:
                Log.SetSource(new MessageFrmReaction(this.Title, this, null));

                StreamReader sr = new StreamReader(m_File);
                string contents = sr.ReadToEnd().Trim();

                //Sources and sinks use a comment stripped version of the file:
                StringBuilder sb = new StringBuilder();
                for (Match m = s_CommentRemovingRegex.Match(contents); m.Success; m = m.NextMatch())
                {
                    sb.AppendLine(m.Value);
                    if (m.Value.ToLowerInvariant().Trim() == "end")
                        break;
                }
                string commentsRemoved = sb.ToString();
                if (!s_EndRegex.Match(commentsRemoved).Success)
                    Log.Message("'End' token not found. Reading to end of file (May cause unpredictable results)", MessageType.Warning);

                //General comments:
                StringBuilder comments = new StringBuilder();
                Match commentsMatch = s_OpeningCommentRegex.Match(contents);
                if (commentsMatch.Success)
                {
                    Group g = commentsMatch.Groups["Comment"];
                    if (g.Success)
                        foreach (Capture c in g.Captures)
                            comments.AppendLine(c.Value);
                    txtDescription.Text = comments.ToString();
                }

                //Sources:
                Match sourcesMatch = s_SourceRegex.Match(commentsRemoved);
                if (sourcesMatch.Success)
                {
                    txtSources.Text = sourcesMatch.Value.Trim().Replace("\r\n", ", ");
                    txtSources_Leave(txtSources, new EventArgs());
                }

                //Reactions:
                Match start = s_RxnBlockStartRegex.Match(contents);
                Match end = s_RxnBlockEndRegex.Match(contents);
                if (!start.Success || !end.Success)
                {
                    Log.Message("Reaction Block Not Found", MessageType.Error);
                    throw new Exception("Reaction block not found");
                }
                int rxnBlockStart = start.Index + start.Length;
                int rxnBlockEnd = end.Index;
                string rxnBlock = contents.Substring(rxnBlockStart, rxnBlockEnd - rxnBlockStart);

                FindReactions(rxnBlock, s_ReactionRegex, true);
                FindReactions(rxnBlock, s_DisabledReactionRegex, false);

                chkFirstReactant.Checked = rxnBlock.ToLowerInvariant().Contains("<usefirstreactant=true>");

                Match LSMatch = s_LastSelectedRegex.Match(rxnBlock);
                if (LSMatch.Success)
                {
                    int index;
                    if (int.TryParse(LSMatch.Groups["Value"].Value, out index))
                        lstReactions.Items[index].Selected = true;
                }
                else if (lstReactions.Items.Count > 0)
                    lstReactions.Items[0].Selected = true;

                //Heat Exchange:
                Match HXMatch = s_HXRegex.Match(commentsRemoved);
                if (HXMatch.Success)
                {
                    string type = HXMatch.Groups["Type"].Value;
                    if (type == "FinalT")
                        comboHXType.SelectedItem = "Final Temp.";
                    else if (type == "TargetT")
                    {
                        comboHXType.SelectedItem = "Approach Temp.";
                        numHXApproach.Value = Decimal.Parse(HXMatch.Groups["Value2"].Value);
                    }
                    else if (type == "Ambient")
                    {
                        comboHXType.SelectedItem = "Approach Ambient";
                        numHXApproach.Value = Decimal.Parse(HXMatch.Groups["Value2"].Value);
                    }
                    else if (type == "Power")
                        comboHXType.SelectedItem = "Power";
                    else if (type == "Electrolyisis")
                        comboHXType.SelectedItem = "Electrolysis";
                    else
                        Log.Message("Unknown Heat Exchange type '" + HXMatch.Groups["Type"].Value + "'", MessageType.Warning);
                    numHX.Text = HXMatch.Groups["Value"].Captures[0].Value;
                }

                //Sinks:
                Match SinkMatch = s_SinkRegex.Match(commentsRemoved);
                if (SinkMatch.Success)
                {
                    txtSinks.Text = SinkMatch.Value.Trim();
                    txtSinks_Leave(txtSinks, new EventArgs());
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
                m_bLoading = false;
            }
        }

        protected void FindReactions(string rxnBlock, Regex reactionRegex, bool enabled)
        {
            int lastSequence = 1;
            bool sequenceFound = false;
            for (Match rxnMatch = reactionRegex.Match(rxnBlock); rxnMatch.Success; rxnMatch = rxnMatch.NextMatch())
            {
                SimpleReaction currentReaction = CreateReaction(null);
                MessageSource source = new MessageFrmReaction(
                    this.Title + ", Reaction " + ReactionNo,
                    this,
                    currentReaction);
                Log.SetSource(source);
                Group grpComment = rxnMatch.Groups["Comment"];
                if (grpComment.Success)
                    currentReaction.Comment = grpComment.Captures[0].Value;

                Group grpReactants = rxnMatch.Groups["Reactants"];
                currentReaction.ParseReactants(grpReactants.Captures[0].Value);


                Group grpDirection = rxnMatch.Groups["Direction"];
                currentReaction.DirectionString = grpDirection.Captures[0].Value;

                Group grpProducts = rxnMatch.Groups["Products"];
                currentReaction.ParseProducts(grpProducts.Captures[0].Value);

                source.Source = this.Title + ": " + currentReaction;

                Group grpExtent = rxnMatch.Groups["Extent"];
                if (grpExtent.Success)
                    try
                    {
                        currentReaction.ParseExtent(grpExtent.Captures[0].Value);
                    }
                    catch (Exception ex)
                    {
                        Log.Message("Unable to parse extent (" + grpExtent.Value + "). Reason: " +ex.Message, MessageType.Warning);
                    }
                else
                    Log.Message("Extent not found for reaction", MessageType.Warning);

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

                Group grpHOR = rxnMatch.Groups["HOR"];
                if (grpHOR.Success)
                    try
                    {
                        currentReaction.ParseHOR(grpHOR.Captures[0].Value.Trim());
                    }
                    catch (Exception ex)
                    {
                        Log.Message("Unable to parse HeatOfReaction '" + grpHOR.Value + "' Reason: " + ex.Message, MessageType.Warning);
                    }

                currentReaction.Enabled = enabled;
                Log.RemoveSource();
            }
            if (sequenceFound)
                chkSequence.Checked = true;
        }

        protected void OldReadFile()
        {
            StreamReader sr = new StreamReader(m_File);
            string str = sr.ReadLine();
            bool bReactionsFound = false, bEndFound = false;
            bool bSourceFound = false, bSinksFound = false;
            SimpleReaction curReaction = null;
            while (str != null)
            {
                str = str.Trim();
                if (string.IsNullOrEmpty(str) || str.StartsWith(";")) //Discard comment and empty lines
                {
                    str = sr.ReadLine();
                    continue;
                }
                if (str.Contains(";")) //Discard comments
                    str = str.Split(';')[0];

                if (str.ToLowerInvariant().StartsWith("end"))
                {
                    bEndFound = true;
                    break;
                }

                if (str.ToLowerInvariant().StartsWith("source:"))
                {
                    bSourceFound = true;
                    str = str.Remove(0, ("Source:").Length);
                    str.Trim();
                }
                if (bSourceFound && !bReactionsFound)
                {
                    string[] tokens = str.Split(',');
                    foreach (string s in tokens)
                    {
                        if (Compound.CompoundList.ContainsKey(s.Trim()))
                        {
                            if (!string.IsNullOrEmpty(txtSources.Text))
                                txtSources.Text += ", ";
                            txtSources.Text += s.Trim();
                        }
                        else
                        {
                            //TODO: Log errors
                        }
                    }
                }
                if (str.ToLowerInvariant().StartsWith("reactions:"))
                {
                    bReactionsFound = true;
                    if (!string.IsNullOrEmpty(str.Remove(0, ("reactions:").Length).Trim()))
                    {
                        //Log this error
                    }
                    str = sr.ReadLine();
                    continue;
                }
                if (bReactionsFound && !bSinksFound)
                {
                    string sFormula = null, sExtent = null, sHOR = null;
                    int nExtentLoc = str.ToLowerInvariant().IndexOf("extent :");
                    int nHORLoc = str.ToLowerInvariant().IndexOf("heat of reaction:");
                    if (nExtentLoc == -1 && nHORLoc == -1)
                        sFormula = str;
                    else if (nExtentLoc == -1)
                    {
                        sFormula = str.Substring(0, nHORLoc);
                        sHOR = str.Substring(nHORLoc);
                    }
                    else if (nHORLoc == -1)
                    {
                        sFormula = str.Substring(0, nExtentLoc);
                        sExtent = str.Substring(nExtentLoc, str.Length - nExtentLoc);
                    }
                    else if (nExtentLoc < nHORLoc)
                    {
                        sFormula = str.Substring(0, nExtentLoc);
                        sExtent = str.Substring(nExtentLoc, nHORLoc - nExtentLoc);
                        sHOR = str.Substring(nHORLoc);
                    }
                    else
                    {
                        sFormula = str.Substring(0, nHORLoc);
                        sHOR = str.Substring(nHORLoc, nExtentLoc - nHORLoc);
                        sExtent = str.Substring(nExtentLoc);
                    }
                    if (sFormula.Length > 0)
                        try
                        {
                            curReaction = CreateReaction(sFormula);
                        }
                        catch
                        {
                            //TODO: Log the error.
                        }
                    if (curReaction == null && (sHOR != null || sExtent != null))
                    {
                        //TODO: Log this error.
                        str = sr.ReadLine();
                        continue;
                    }
                    if (sHOR != null)
                    {
                        sHOR = sHOR.Remove(0, "heat of reaction :".Length).Trim();
                        string[] subStrings = sHOR.Split('/');
                        try
                        {
                            curReaction.HeatOfReactionValue = double.Parse(subStrings[0].Trim());
                            curReaction.HeatOfReactionSpecie = Compound.FromString(subStrings[1].Trim());
                        }
                        catch
                        {
                            //TODO: Log the error
                        }
                    }
                    if (sExtent != null)
                    {
                        try
                        {
                            sExtent = sExtent.Remove(0, "extent :".Length).Trim();
                            curReaction.ParseExtent(sExtent);
                        }
                        catch (Exception ex)
                        {
                            Console.WriteLine(ex.ToString());
                            //TODO: Log the error.
                        }
                    }
                }
                if (str.ToLowerInvariant().StartsWith("sink:"))
                {
                    bSinksFound = true;
                    str = str.Remove(0, ("sink:").Length);
                }
                if (bSinksFound)
                {
                    string[] sinks = str.Split(',');
                    foreach (string s in sinks)
                        if (Compound.CompoundList.ContainsKey(s.Trim()))
                        {
                            if (!string.IsNullOrEmpty(txtSinks.Text))
                                txtSinks.Text += ", ";
                            txtSinks.Text += s.Trim();
                        }
                        else
                        {
                            //TODO: Log this error.
                        }
                }
                str = sr.ReadLine();
            }
            //TODO: Log the error if "end" is not found.
        }

        public SimpleReaction CreateReaction(string initialFormula)
        {
            ListViewItem lvi = new ListViewItem();
            lvi.SubItems.AddRange(new string[] { "", "", "" });
            lstReactions.Items.Add(lvi);
            SimpleReaction rxn = new SimpleReaction(lvi, Log);
            rxn.Changed += new EventHandler(rxn_Changed);
            if (initialFormula != null)
                try
                {
                    rxn.SetString(initialFormula);
                }
                catch (Exception ex)
                {
                    //TODO: Log the error
                }
            else
                rxn_Changed(rxn, new EventArgs());
            return rxn;
        }

        protected void LoadReaction(SimpleReaction rxn)
        {
            m_bLoading = true;
            if (m_CurrentReaction != null)
            {
                m_CurrentReaction.Changed -= new EventHandler(currentReactionChanged);
                m_CurrentReaction.ReactantsChanged -= new EventHandler(rxn_ReactantsChanged);
                m_CurrentReaction.ProductsChanged -= new EventHandler(rxn_ProductsChanged);
            }
            m_CurrentReaction = rxn;
            txtReactants.Visible = txtProducts.Visible = comboDirection.Visible = true;
            txtFormula.Visible = false;
            if (ReactionChanged != null)
                ReactionChanged(this, new EventArgs());
            if (rxn == null)
            {
                SetPanelEnables(pnlReaction, false);
                btnCopy.Enabled = btnMoveDown.Enabled = btnMoveUp.Enabled = btnRemove.Enabled = false;
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
            rxn.ProductsChanged += new EventHandler(rxn_ProductsChanged);
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

            comboExtentType.SelectedIndex = (int) rxn.ExtentType;
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

        void rxn_ProductsChanged(object sender, EventArgs e)
        {
            if (CompoundsChanged != null)
                CompoundsChanged(this, new EventArgs());
        }

        void rxn_ReactantsChanged(object sender, EventArgs e)
        {
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
            //if (oldIndex < newIndex)
                //newIndex--;
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
                lstReactions.SelectedIndices.Add(item.Index);
            m_bDoEvents = true;
        }

        protected void rxn_Changed(object sender, EventArgs e)
        {
            SimpleReaction rxn = (SimpleReaction)sender;
            if (lstReactions.ShowGroups)
                if (lstReactions.Groups["grpSequence" + rxn.Sequence] != rxn.LVI.Group)
                {
                    m_bDoEvents = false;
                    rxn.LVI.Group = lstReactions.Groups["grpSequence" + rxn.Sequence];
                    m_bDoEvents = true;
                }
            ChangeOccured();
        }

        protected void currentReactionChanged(object sender, EventArgs e)
        {
            txtFormula.Text = m_CurrentReaction.ToString();
            txtProducts.Text = m_CurrentReaction.GetProductsString();
            txtReactants.Text = m_CurrentReaction.GetReactantsString();
        }

        protected void ChangeOccured()
        {
            if (!m_bLoading)
                Changed = true;
            if (NowChanged != null)
                NowChanged(this, new EventArgs());
            statusLabel.Text = StatusMessage;
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
            Match m = s_CompoundSeperator.Match(s);
            //string[] seperatedCompounds = s.Split(new string[] { ",", "\r\n" }, StringSplitOptions.None);
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
                if (lvi.Bounds.Bottom > p.Y && lvi.Bounds.Bottom - p.Y < nClosestVal)
                {
                    nClosestVal = lvi.Bounds.Bottom - p.Y;
                    closestLVI = lvi;
                }
            return closestLVI;
        }
        #endregion Protected Functions

        #region Subclasses
        public delegate void ReactionHandler(FrmReaction form, SimpleReaction reaction);

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

        private void lstReactions_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!m_bDoEvents) return;

            if (lstReactions.SelectedIndices.Count == 0)
                LoadReaction(null);
            else
                LoadReaction((SimpleReaction)lstReactions.SelectedItems[0].Tag);
        }

        private void btnAdd_Click(object sender, EventArgs e)
        {
            CreateReaction(null).LVI.Selected = true;
            txtReactants.Select();
        }

        private void chkSequence_CheckedChanged(object sender, EventArgs e)
        {
            m_bDoEvents = false;
            numSequence.Enabled = chkSequence.Checked;

            List<ListViewItem> originalList = new List<ListViewItem>();

            if (!chkSequence.Checked)
            {
                if (lstReactions.Columns.Contains(m_SequenceHeader))
                    lstReactions.Columns.Remove(m_SequenceHeader);
                //To get the damned order correct:
                foreach (ListViewGroup grp in lstReactions.Groups)
                    foreach (ListViewItem lvi in grp.Items)
                        originalList.Add(lvi);
                foreach (ListViewGroup grp in lstReactions.Groups)
                    grp.Items.Clear();
                lstReactions.Items.Clear();
                lstReactions.Items.AddRange(originalList.ToArray());

                menuShowSequence.Checked = false;

                foreach (ListViewItem lvi in originalList)
                {
                    SimpleReaction rxn = (SimpleReaction)lvi.Tag;
                    rxn.Sequence = 1;
                }
            }
            else
            {
                if (!lstReactions.Columns.Contains(m_SequenceHeader))
                    lstReactions.Columns.Add(m_SequenceHeader);
                foreach (ListViewItem lvi in lstReactions.Items)
                    originalList.Add(lvi);
                //lstReactions.ShowGroups = true;
                menuShowSequence.Checked = true;
                foreach (ListViewItem lvi in originalList)
                    lvi.Group = lstReactions.Groups["grpSequence" + ((SimpleReaction)lvi.Tag).Sequence];
                //lstReactions.ShowGroups = false;
            }
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
                    m_CurrentReaction.ParseReactants(box.Text);
                if (box == txtProducts)
                    m_CurrentReaction.ParseProducts(box.Text);
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
            switch ((HXTypes)comboHXType.SelectedIndex)
            {
                case HXTypes.None:
                    break;
                case HXTypes.FinalT:
                    lblHXUnits.Text = "degC"; lblHXUnits.Visible = true;
                    lblHXValue.Text = "Temperature"; lblHXValue.Visible = true;
                    numHX.Visible = true;
                    break;
                case HXTypes.ApproachT:
                    foreach (Control c in m_HXControls)
                        c.Visible = true;
                    lblHXUnits.Text = "degC,"; lblHXValue.Text = "Temperature";
                    break;
                case HXTypes.ApproachAmbient:
                    foreach (Control c in m_HXControls)
                        c.Visible = true;
                    lblHXUnits.Text = "degC,"; lblHXValue.Text = "Temperature";
                    break;
                case HXTypes.Power:
                    lblHXUnits.Text = "kJ/s"; lblHXUnits.Visible = true;
                    lblHXValue.Text = "Value"; lblHXValue.Visible = true;
                    numHX.Visible = true;
                    break;
                case HXTypes.Electrolysis:
                    lblHXUnits.Text = "%"; lblHXUnits.Visible = true;
                    lblHXValue.Text = "Value"; lblHXValue.Visible = true;
                    numHX.Visible = true;
                    break;
            }
            ChangeOccured();
        }

        private void numHORValue_TextChanged(object sender, EventArgs e)
        {
            if (m_CurrentReaction == null) return;
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
            if (!e.Data.GetDataPresent(typeof(Compound)))
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            Compound compound = (Compound) e.Data.GetData(typeof(Compound));
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

        private void txtReactants_DragEnter(object sender, DragEventArgs e)
        {
            if (m_CurrentReaction == null || !m_CurrentReaction.ReactantsOk)
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            if (e.Data.GetDataPresent(typeof(Compound)))
                e.Effect = DragDropEffects.Link;
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
            if (!e.Data.GetDataPresent(typeof(Compound)))
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            e.Effect = DragDropEffects.Link;
            Compound compound = (Compound)e.Data.GetData(typeof(Compound));
            m_CurrentReaction.AddReactant(compound, 1.0, chIndex);
            txtReactants.Select();
            txtReactants.Select(m_CurrentReaction.LastAddedNumStart, 1);
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

        private void tabSources_Sinks_Resize(object sender, EventArgs e)
        {
            grpSinks.Height = tabOptions.Height / 2;
        }

        private void chkEnabled_CheckedChanged(object sender, EventArgs e)
        {
            if (m_CurrentReaction != null)
                m_CurrentReaction.Enabled = chkEnabled.Checked;
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

        private void btnCopy_Click(object sender, EventArgs e)
        {
            if (m_CurrentReaction == null) return;
            ListViewItem lvi = AddReaction(m_CurrentReaction, -1).LVI;
            lvi.Selected = true;
        }

        private void btnRemove_Click(object sender, EventArgs e)
        {
            if (lstReactions.SelectedItems.Count == 0)
                return;
            else
            {
                SimpleReaction rxn = (SimpleReaction)lstReactions.SelectedItems[0].Tag;
                lstReactions.SelectedItems[0].Remove();
                if (ReactionRemoved != null)
                    ReactionRemoved(this, rxn);
            }
        }

        private void txtSources_TextChanged(object sender, EventArgs e)
        {
            bool bWasActive = Log.Active;
            Log.Active = false;
            ColourCompounds((RichTextBox)sender);
            Log.Active = bWasActive;
            ChangeOccured();
        }

        private void txtSinks_TextChanged(object sender, EventArgs e)
        {
            bool bWasActive = Log.Active;
            Log.Active = false;
            ColourCompounds((RichTextBox)sender);
            Log.Active = bWasActive;
            ChangeOccured();
        }

        private void numHX_TextChanged(object sender, EventArgs e)
        {

            ChangeOccured();
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
                    ChangePosition(rxn.LVI, newLoc);
                else
                    AddReaction(rxn, newLoc);
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

        protected List<Compound> GetCommaSeperatedCompounds(string s)
        {
            Match m2 = s_CompoundSeperator.Match(s);
            List<Compound> ret = new List<Compound>();
            foreach (Capture c in m2.Groups["Comp"].Captures)
                if (Compound.Contains(c.Value.Trim()))
                    ret.Add(Compound.FromString(c.Value.Trim()));
            return ret;
        }

        private void txtSources_Leave(object sender, EventArgs e)
        {
            Log.SetSource(new MessageFrmReaction(this.Title + " Sources", this, null));
            Match m = s_EndsWithComma.Match(txtSources.Text);
            if (m.Success)
                txtSources.Text = m.Groups["Data"].Value;
            ColourCompounds(txtSources);

            Log.RemoveSource();

            m_SourcesCache = GetCommaSeperatedCompounds(txtSources.Text);

            ChangeOccured();
            if (SourcesSinksChanged != null)
                SourcesSinksChanged(this, new EventArgs());
        }

        private void txtSinks_Leave(object sender, EventArgs e)
        {
            Log.SetSource(new MessageFrmReaction(this.Title + " Sinks", this, null));
            Match m = s_EndsWithComma.Match(txtSinks.Text);
            if (m.Success)
                txtSinks.Text = m.Groups["Data"].Value;
            ColourCompounds(txtSinks);

            m_SinksCache = GetCommaSeperatedCompounds(txtSinks.Text);

            Log.RemoveSource();

            ChangeOccured();
            if (SourcesSinksChanged != null)
                SourcesSinksChanged(this, new EventArgs());
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
            if (Regex.Match(box.Text, @"(^|,)\s*" + Regex.Escape(newComp.Symbol) + @"\s*(,|$)", RegexOptions.ExplicitCapture).Success)
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            e.Effect = DragDropEffects.Link;
        }

        protected static Regex s_EndsWithComma = new Regex(@"(?<Data>.*),\s*$", RegexOptions.Compiled | RegexOptions.ExplicitCapture | RegexOptions.Singleline);
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
            if (Regex.Match(box.Text, @"(^|,)\s*" + Regex.Escape(newComp.Symbol) + @"\s*(,|$)", RegexOptions.ExplicitCapture).Success)
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            e.Effect = DragDropEffects.Link;
            //bool bWasActive = Log.Active;
            //Log.Active = false;
            //ColourCompounds(box);
            //Log.Active = bWasActive;
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
            ChangeOccured();
        }

        private void chkFirstReactant_CheckedChanged(object sender, EventArgs e)
        {
            if (chkFirstReactant.Checked)
            {
                foreach (ListViewItem lvi in lstReactions.Items)
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
            double.TryParse(numHORT.Text, out temp);
            m_CurrentReaction.HeatOfReactionP = temp;
        }

        private void btnBalance_Click(object sender, EventArgs e)
        {
            if (m_CurrentReaction == null) return;
            try
            {
                SimpleReaction.RemovalInfo info = m_CurrentReaction.BalanceOptions();
                if (!info.m_bCanRemove)
                    MessageBox.Show("Unable to balance reaction - no solutions available without forcing compounds to change sides of the reaction", "Autobalance", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                else
                {
                    if (info.DegreesOfFreedom > 0)
                    {
                        FrmBalanceOptions frm = new FrmBalanceOptions(info, m_CurrentReaction);
                        if (frm.ShowDialog(this) == DialogResult.OK)
                        {
                            SimpleReaction demo = m_CurrentReaction.Clone();
                            demo.BalanceWith(frm.ToBeRemoved);
                            if (MessageBox.Show(this, "Resultant Reaction:\n" + demo.ToString(), "Confirm Change", MessageBoxButtons.OKCancel, MessageBoxIcon.Information) == DialogResult.OK)
                                m_CurrentReaction.BalanceWith(frm.ToBeRemoved);
                        }
                        frm.Dispose();
                    }
                    else
                    {
                        SimpleReaction demo = m_CurrentReaction.Clone();
                        demo.BalanceWith(new int[0]);
                        if (MessageBox.Show(this, "Resultant Reaction:\n" + demo.ToString(), "Confirm Change", MessageBoxButtons.OKCancel, MessageBoxIcon.Information) == DialogResult.OK)
                            m_CurrentReaction.BalanceWith(new int[0]);
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
            RichTextBox box = (RichTextBox)sender;
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
            box.Select(oldSelectStart, oldSelectLen);
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
            menuCopy.Enabled = menuRemove.Enabled = m_CurrentReaction != null;
            menuShowSequence.Enabled = chkSequence.Checked;
        }

        private void menuShowSequence_CheckedChanged(object sender, EventArgs e)
        {
            lstReactions.ShowGroups = menuShowSequence.Checked;
        }

        private void lstReactions_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Delete)
                btnRemove_Click(sender, e);
        }
    }
}