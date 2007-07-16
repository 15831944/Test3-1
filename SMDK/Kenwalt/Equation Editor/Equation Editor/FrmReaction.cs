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

namespace Reaction_Editor
{
    public partial class FrmReaction : Form
    {
        #region Regex's
        protected static Regex s_CommentRemovingRegex = new Regex(@"^[^:]*",
            RegexOptions.ExplicitCapture | RegexOptions.Multiline | RegexOptions.Compiled);
        protected static Regex s_OpeningCommentRegex = new Regex(@"^(\s*(?>;(?<Comment>[^\r\n]*))?)*",
            RegexOptions.ExplicitCapture | RegexOptions.Compiled); //If the first match is non-zero, discard.
        protected static Regex s_SourceRegex = new Regex(@"(?<=(^|\r\n)\s*?Source:)[^:]*(?=Reactions:)",
            RegexOptions.IgnoreCase | RegexOptions.ExplicitCapture | RegexOptions.Compiled);
        protected static Regex s_ReactionRegex = new Regex(
            @"(^|\r\n)\s*((?<Comment>;.*)\r\n)?(?<Reactants>[^;\r\n<>=\-:]*)(?<Direction>->|=|<->|->)\s*(?<Products>[^;:\r\n]*?(?=Extent|Sequence|HeatOfReaction|;|\r\n|$))(?>(?>\s*(;.*\r\n)?)*((?<Extent>Extent\s*:[^\r\n;]*?)|(?<Sequence>Sequence\s*:[^\r\n;]*?)|(?<HOR>HeatOfReaction\s*:[^\r\n;]*?))(?=Extent|Sequence|HeatOfReaction|;|\r\n|$)){0,3}",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled);
        protected static Regex s_DisabledReactionRegex = new Regex(
            @"(^|\r\n)\s*((?<Comment>;.*)\r\n[^\S\r\n]*)?;(?<Reactants>[^;\r\n<>=\-:]*)(?<Direction>->|=|<->|->)\s*(?<Products>[^;:\r\n]*?(?=Extent|Sequence|HeatOfReaction|;|\r\n|$))(?>[^\S\r\n]*(\r\n\s*;)?((?<Extent>Extent\s*:[^\r\n;]*?)|(?<Sequence>Sequence\s*:[^\r\n;]*?)|(?<HOR>HeatOfReaction\s*:[^\r\n;]*?))(?=Extent|Sequence|HeatOfReaction|;|\r\n|$)){0,3}",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled);
        protected static Regex s_SinkRegex = new Regex(@"(?<=\r\n\s*?Sink:)[^:]*?(?=End|HeatExchange|$)",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled);
        protected static Regex s_HXRegex = new Regex(@"^\s*HeatExchange:\s*(?<Type>FinalT|ApproachT|ApproachAmbient|Power|Electrolysis)\s*(?<Value>\d+(\.\d*)?|\d*\.\d+)",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Multiline | RegexOptions.Compiled);
        protected static Regex s_RxnBlockStartRegex = new Regex(@"(^|\r\n\s*)Reactions:",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled);
        protected static Regex s_RxnBlockEndRegex = new Regex(@"\r\n\s*(End|^|HeatExchange|Sink)",
            RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase | RegexOptions.Compiled);
        #endregion Regex's

        #region Variables
        //protected List<SimpleReaction> m_Reactions = new List<SimpleReaction>();
        //protected List<Compound> m_Sources = new List<Compound>();
        //protected List<Compound> m_Sinks = new List<Compound>();
        //Dictionary<int, ListViewGroup> m_ListViewGroups = new Dictionary<int, ListViewGroup>();
        protected bool m_bChanged = false;
        protected string m_sTitle = "Untitled Reaction Block";
        protected FileStream m_File;
        protected SimpleReaction m_CurrentReaction;

        //protected ToolStripStatusLabel m_StatusControl;
        protected ILog m_Log;

        protected bool m_bDoEvents = true;
        protected List<Control> m_OptionalExtentControls = new List<Control>();

        public const int sMaxSequences = 32;
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
        #endregion Properties

        #region Events
        public EventHandler NowChanged;
        #endregion Events

        #region Constructors
        public FrmReaction(ILog log)
        {
            SetUp(log);
        }

        public FrmReaction(string filename, ILog log)
        {
            SetUp(log);
            m_File = new FileStream(filename, FileMode.Open, FileAccess.ReadWrite, FileShare.Read);
            this.Title = Path.GetFileName(filename);
            ReadFile();
        }

        public FrmReaction(int titleNum, ILog log)
        {
            SetUp(log);
            this.Title = "Untitled-" + titleNum;
        }

        private void SetUp(ILog log)
        {
            InitializeComponent();
            m_Log = log;

            m_OptionalExtentControls.AddRange(new Control[] {
                this.numExtentVal2,
                this.numExtentVal3,
                this.comboExtentPhase,
                this.chkExtentStabilised,
                this.lblExtent2,
                this.lblExtent3 });

            ArrayList phases = new ArrayList(Enum.GetValues(typeof(Phases)));
            comboExtentPhase.Items.AddRange(phases.ToArray());

            for (int i = 0; i <= sMaxSequences; i++)
            {
                ListViewGroup grp = new ListViewGroup("grpSequence" + i, "Sequence " + i);
                lstReactions.Groups.Add(grp);
            }

            comboHXType.SelectedItem = "None";

            tabSources_Sinks_Resize(null, new EventArgs());
            pnlReaction_Resize(null, new EventArgs());
            Changed = false;
        }
        #endregion Constructors

        #region Public Functions
        public void Save()
        {
            m_File.SetLength(0);
            StreamWriter sr = new StreamWriter(m_File);
            //First, the comments:
            if (!string.IsNullOrEmpty(txtDescription.Text))
            {
                sr.WriteLine(";" + txtDescription.Text.TrimEnd().Replace("\r\n", "\r\n;"));
                sr.WriteLine();
            }

            //Sources, if they exist:
            if (!string.IsNullOrEmpty(txtSources.Text))
            {
                sr.WriteLine("Source: " + txtSources.Text.Replace("\r\n", ", "));
                sr.WriteLine();
            }

            //Reactions:
            sr.WriteLine("Reactions:");
            foreach (ListViewItem lvi in lstReactions.Items)
            {
                SimpleReaction rxn = (SimpleReaction)lvi.Tag;
                sr.WriteLine();
                if (!string.IsNullOrEmpty(rxn.Comment))
                    sr.WriteLine(";" + rxn.Comment);
                sr.WriteLine(rxn.ToSaveString(chkSequence.Checked));
            }
            sr.WriteLine();
            
            if ((HXTypes) comboHXType.SelectedIndex != HXTypes.None)
            {
                sr.WriteLine("HeatExchange: " + (HXTypes)comboHXType.SelectedIndex + " = " + numHX.Text);
                sr.WriteLine();
            }

            if (!string.IsNullOrEmpty(txtSinks.Text))
            {
                sr.WriteLine("Sink: " + txtSinks.Text.Replace("\r\n", ", "));
                sr.WriteLine();
            }

            sr.WriteLine("End");
            sr.Flush();
            Changed = false;
        }

        public void SaveAs(string newName)
        {
            if (m_File != null)
                m_File.Close();
            m_File = new FileStream(newName, FileMode.Create, FileAccess.ReadWrite, FileShare.Read);
            this.Title = Path.GetFileName(newName);
            Save();
        }

        public void RepopulateSpecies()
        {
            comboHORSpecie.Items.Clear();
            foreach (Compound c in Compound.CompoundList.Values)
            {
                comboHORSpecie.Items.Add(c);
                comboExtentSpecie.Items.Add(c);
            }
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
        #endregion Public Functions

        #region Protected Functions
        protected void ReadFile()
        {
            Log.SetSource(new MessageFrmReaction(this.Title, this, null));

            StreamReader sr = new StreamReader(m_File);
            string contents = sr.ReadToEnd().Trim();
            
            //Sources and sinks use a comment stripped version of the file:
            StringBuilder sb = new StringBuilder();
            for (Match m = s_CommentRemovingRegex.Match(contents); m.Success; m = m.NextMatch())
                sb.AppendLine(m.Value);
            string commentsRemoved = sb.ToString();

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
            }

            //Reactions:
            Match start = s_RxnBlockStartRegex.Match(contents);
            Match end = s_RxnBlockEndRegex.Match(contents);
            if (!start.Success || !end.Success)
            {
                Log.Message("Reaction Block Not Found", MessageType.Error);
                throw new Exception("Reaction block not found");
            }
            int rxnBlockStart = start.Index;
            int rxnBlockEnd = end.Index;
            string rxnBlock = contents.Substring(rxnBlockStart, rxnBlockEnd - rxnBlockStart);
            
            FindReactions(rxnBlock, s_ReactionRegex, true);
            FindReactions(rxnBlock, s_DisabledReactionRegex, false);

            Match HXMatch = s_HXRegex.Match(contents);
            if (HXMatch.Success)
            {
                string type = HXMatch.Groups["Type"].Value;
                if (type == "FinalT")
                    comboHXType.SelectedItem = "Final Temp.";
                else if (type == "ApproachT")
                    comboHXType.SelectedItem = "Approach Temp.";
                else if (type == "ApproachAmbient")
                    comboHXType.SelectedItem = "Approach Ambient";
                else if (type == "Power")
                    comboHXType.SelectedItem = "Power";
                else if (type == "Electrolyisis")
                    comboHXType.SelectedItem = "Electrolysis";
                else
                    Log.Message("Unknown Heat Exchange type '" + HXMatch.Groups["Type"].Value + "'", MessageType.Warning);
                numHX.Text = HXMatch.Groups["Value"].Captures[0].Value;
            }

            Match SinkMatch = s_SinkRegex.Match(commentsRemoved);
            if (SinkMatch.Success)
            {
                txtSources.Text = SinkMatch.Value.Trim().Replace("\r\n", ", ");
                //TODO: process input.
            }
            Changed = false;
            if (lstReactions.Items.Count != 1)
                Log.Message("File opened. " + lstReactions.Items.Count + " Reactions found.", MessageType.Note);
            else
                Log.Message("File opened. 1 Reaction found.", MessageType.Note);
            Log.RemoveSource();
        }

        protected void FindReactions(string rxnBlock, Regex reactionRegex, bool enabled)
        {
            int lastSequence = 1;
            bool sequenceFound = false;
            int reactionNo = 0;
            for (Match rxnMatch = reactionRegex.Match(rxnBlock); rxnMatch.Success; rxnMatch = rxnMatch.NextMatch())
            {
                reactionNo++;
                SimpleReaction currentReaction = CreateReaction(null);
                MessageSource source = new MessageFrmReaction(
                    this.Title + ", Reaction " + reactionNo,
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

        protected SimpleReaction CreateReaction(string initialFormula)
        {
            ListViewItem lvi = new ListViewItem();
            lvi.SubItems.AddRange(new string[] { "", "" });
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
            if (m_CurrentReaction != null)
            {
                m_CurrentReaction.Changed -= new EventHandler(currentReactionChanged);
                m_CurrentReaction.ReactantsChanged -= new EventHandler(rxn_ReactantsChanged);
            }
            m_CurrentReaction = rxn;
            txtReactants.Visible = txtProducts.Visible = comboDirection.Visible = true;
            txtFormula.Visible = false;
            if (rxn == null)
            {
                btnCopy.Enabled = btnMoveDown.Enabled = btnMoveUp.Enabled = btnRemove.Enabled = false;
                pnlReaction.Enabled = false;
                txtReactants.Text = txtProducts.Text = "";
                comboDirection.SelectedIndex = -1;
                comboExtentType.SelectedIndex = -1;
                comboHORSpecie.SelectedIndex = -1;
                comboHXType.SelectedIndex = -1;
                numSequence.Text = "";
                chkEnabled.Checked = false;
                return;
            }
            rxn.Changed += new EventHandler(currentReactionChanged);
            rxn.ReactantsChanged += new EventHandler(rxn_ReactantsChanged);
            btnCopy.Enabled = btnMoveDown.Enabled = btnMoveUp.Enabled = btnRemove.Enabled = true;
            if (rxn.UseOriginalString)
            {
                txtReactants.Visible = txtProducts.Visible = comboDirection.Visible = false;
                txtFormula.Visible = true;
                txtFormula.Text = rxn.ToString();
            }
            if (string.IsNullOrEmpty(rxn.GetProductsString()))
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
            }
            rxn_ReactantsChanged(null, new EventArgs());

            comboDirection.SelectedIndex = (int)rxn.Direction;

            comboExtentType.SelectedIndex = (int) rxn.ExtentType;
            comboHORType.SelectedIndex = rxn.CustomHeatOfReaction ? 1 : 0;

            numExtentValue.Text = rxn.ExtentInfo.Value.ToString();
            chkEnabled.Checked = rxn.Enabled;
            txtComment.Text = rxn.Comment;

            if (chkSequence.Checked)
                numSequence.Value = Math.Min(Math.Max(rxn.Sequence, 1), sMaxSequences);
            pnlReaction.Enabled = true;
        }

        void rxn_ReactantsChanged(object sender, EventArgs e)
        {
            object cHOR = comboHORSpecie.SelectedItem;
            object cExt = comboExtentSpecie.SelectedItem;
            comboExtentSpecie.Items.Clear();
            comboHORSpecie.Items.Clear();
            foreach (Compound c in m_CurrentReaction.Reactants.Keys)
            {
                comboExtentSpecie.Items.Add(c);
                comboHORSpecie.Items.Add(c);
            }
            if (cExt != null && comboExtentSpecie.Items.Contains(cExt))
                comboExtentSpecie.SelectedItem = cExt;
            if (cHOR != null && comboHORSpecie.Items.Contains(cHOR))
                comboHORSpecie.SelectedItem = cHOR;
        }

        protected void SetWaitingText(TextBox box, string text)
        {
            box.Text = text;
            box.TextAlign = HorizontalAlignment.Center;
            box.ForeColor = System.Drawing.SystemColors.GrayText;
            box.Tag = false;
        }

        protected void SetStatusMessage(string message)
        {
            //TODO: Make the message only appear for a finite amount of time.
            Log.Message("SetStatusMessageCalled - " + message, MessageType.Warning);
            //m_StatusControl.Text = message;
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
            lstReactions.Items.Insert(newIndex, item);
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
            Changed = true;
            if (NowChanged != null)
                NowChanged(this, new EventArgs());
        }
        #endregion Protected Functions

        #region Subclasses
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
            CreateReaction(null);
        }

        private void chkSequence_CheckedChanged(object sender, EventArgs e)
        {
            m_bDoEvents = false;
            numSequence.Enabled = chkSequence.Checked;

            List<ListViewItem> originalList = new List<ListViewItem>();

            if (!chkSequence.Checked)
            {
                //To get the damned order correct:
                foreach (ListViewGroup grp in lstReactions.Groups)
                    foreach (ListViewItem lvi in grp.Items)
                        originalList.Add(lvi);
                foreach (ListViewGroup grp in lstReactions.Groups)
                    grp.Items.Clear();
                lstReactions.Items.Clear();
                lstReactions.Items.AddRange(originalList.ToArray());

                lstReactions.ShowGroups = false;

                foreach (ListViewItem lvi in originalList)
                {
                    SimpleReaction rxn = (SimpleReaction)lvi.Tag;
                    rxn.Sequence = 1;
                }
            }
            else
            {
                foreach (ListViewItem lvi in lstReactions.Items)
                    originalList.Add(lvi);
                lstReactions.ShowGroups = true;
                foreach (ListViewItem lvi in originalList)
                    lvi.Group = lstReactions.Groups["grpSequence" + ((SimpleReaction)lvi.Tag).Sequence];
                
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
            comboExtentSpecie.SelectedItem = m_CurrentReaction.ExtentInfo.Specie;
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
                    comboExtentPhase.Visible = true;
                    comboExtentPhase.SelectedItem = ((Final_FracExtent) m_CurrentReaction.ExtentInfo).Phase;
                    break;
                case ExtentTypes.Rate:
                    lblExtentUnits.Text = "Frac / s";
                    chkExtentStabilised.Visible = true;
                    chkExtentStabilised.Checked = ((RateExtent) m_CurrentReaction.ExtentInfo).Stabilised;
                    break;
            }

        }

        private void FormulaBox_Enter(object sender, EventArgs e)
        {
            TextBox box = (TextBox)sender;
            if (box.Tag == null || (bool)box.Tag == true) return;
            box.Text = "";
            box.ForeColor = System.Drawing.SystemColors.WindowText;
            if (box == txtReactants) box.TextAlign = HorizontalAlignment.Right;
            if (box == txtProducts) box.TextAlign = HorizontalAlignment.Left;
        }

        private void FormulaBox_Leave(object sender, EventArgs e)
        {
            try
            {
                TextBox box = (TextBox)sender;
                if (m_CurrentReaction != null)
                {
                    if (box == txtReactants)
                        m_CurrentReaction.ParseReactants(box.Text);
                    if (box == txtProducts)
                        m_CurrentReaction.ParseProducts(box.Text);
                }
                if (box.Text == "")
                {
                    if (box == txtReactants) SetWaitingText(box, "Reactants");
                    if (box == txtProducts) SetWaitingText(box, "Products");
                }
                else
                    box.Tag = true;
            }
            catch (RxnEdException ex)
            {
                SetStatusMessage(ex.Message);
            }
        }

        private void comboDirection_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (m_CurrentReaction != null)
            {
                m_CurrentReaction.Direction = (RxnDirections)comboDirection.SelectedIndex;
            }
        }

        private void comboHORType_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboHORType.SelectedIndex == 0) //Default:
            {
                comboHORSpecie.Enabled = false;
                comboHORSpecie.SelectedIndex = -1;
                comboHORUnits.SelectedItem = "kJ/mol";
                comboHORUnits.Enabled = false;
                /*if (m_CurrentReaction != null)
                {
                    m_CurrentReaction.HeatOfReactionValue = double.NaN;
                    numHORValue.Text = m_CurrentReaction.HeatOfReactionValue.ToString();
                }
                else*/
                numHORValue.Text = "";
                numHORValue.Enabled = false;
            }
            else //Override:
            {
                comboHORUnits.Enabled = true;
                comboHORSpecie.Enabled = true;
                numHORValue.Enabled = true;
                if (m_CurrentReaction != null && m_CurrentReaction.CustomHeatOfReaction)
                {
                    comboHORSpecie.SelectedItem = m_CurrentReaction.HeatOfReactionSpecie;
                    numHORValue.Text = m_CurrentReaction.HeatOfReactionValue.ToString();
                    if (m_CurrentReaction.HeatOfReactionType == FracTypes.ByMass)
                        comboHORUnits.SelectedItem = "kJ/kg";
                    else
                        comboHORUnits.SelectedItem = "kJ/mol";
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
                int temp;
                int.TryParse(numSequence.Text, out temp);
                if (temp > sMaxSequences)
                {
                    numSequence.Text = sMaxSequences.ToString(); //This will refire this event...
                    return;
                }
                m_CurrentReaction.Sequence = temp;
            }
        }

        private void FrmReaction_Load(object sender, EventArgs e)
        {
            RepopulateSpecies();
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
            if ((HXTypes)comboHXType.SelectedIndex == HXTypes.None || m_CurrentReaction == null)
            {
                numHX.Enabled = false;
                //numHX.Text = "";
                lblHXUnits.Text = "";
                ChangeOccured();
                return;
            }
            else
            {
                numHX.Enabled = true;
            }
            if ((HXTypes)comboHXType.SelectedIndex == HXTypes.FinalT ||
                (HXTypes)comboHXType.SelectedIndex == HXTypes.ApproachT ||
                (HXTypes)comboHXType.SelectedIndex == HXTypes.ApproachAmbient)
                    lblHXUnits.Text = "degC";
            if ((HXTypes)comboHXType.SelectedIndex == HXTypes.Electrolysis)
                lblHXUnits.Text = "%";
            if ((HXTypes)comboHXType.SelectedIndex == HXTypes.Power)
                lblHXUnits.Text = "kJ/s";

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
            try
            {
                m_CurrentReaction.SetString(txtFormula.Text);
            }
            catch (Reaction_Editor.RxnEdException ex)
            {
                SetStatusMessage(ex.Message);
            }
        }

        private void comboExtentSpecie_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (m_CurrentReaction == null) return;
            m_CurrentReaction.ExtentInfo.Specie = (Compound) comboExtentSpecie.SelectedItem;
        }

        private void chkExtentStabilised_CheckedChanged(object sender, EventArgs e)
        {
            if (m_CurrentReaction == null || m_CurrentReaction.ExtentType != ExtentTypes.Rate) return;
            ((RateExtent)m_CurrentReaction.ExtentInfo).Stabilised = chkExtentStabilised.Checked;
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

        private void comboExtentPhase_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (m_CurrentReaction == null || m_CurrentReaction.ExtentType != ExtentTypes.FinalFrac)
                return;
            ((Final_FracExtent)m_CurrentReaction.ExtentInfo).Phase = (Phases)comboExtentPhase.SelectedItem;
        }

        private void txtProducts_DragEnter(object sender, DragEventArgs e)
        {
            if (m_CurrentReaction == null || !m_CurrentReaction.ProductsOk)
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            if (e.Data.GetDataPresent(typeof(Compound)))
                e.Effect = DragDropEffects.Link;
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
            m_CurrentReaction.AddProduct(compound, 1.0, chIndex);
            e.Effect = DragDropEffects.Link;
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

            Point clientPoint = txtProducts.PointToClient(new Point(e.X, e.Y));
            int chIndex = txtProducts.GetCharIndexFromPosition(clientPoint);
            if (!e.Data.GetDataPresent(typeof(Compound)))
            {
                e.Effect = DragDropEffects.None;
                return;
            }
            Compound compound = (Compound)e.Data.GetData(typeof(Compound));
            m_CurrentReaction.AddReactant(compound, 1.0, chIndex);
            e.Effect = DragDropEffects.Link;
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
            txtProducts.Width = txtReactants.Width = (pnlReaction.ClientSize.Width - comboWidth) / 2 - (outsidePadding + spacing);
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
            ListViewItem lvi = new ListViewItem();
            lvi.SubItems.AddRange(new string[] { "", "" });
            lstReactions.Items.Add(lvi);
            SimpleReaction rxn = m_CurrentReaction.Clone(lvi);
            rxn.FireChanged();
        }

        private void btnRemove_Click(object sender, EventArgs e)
        {
            if (lstReactions.SelectedItems.Count == 0)
                return;
            else
                lstReactions.SelectedItems[0].Remove();
        }

        private void txtSources_TextChanged(object sender, EventArgs e)
        {
            ChangeOccured();
        }

        private void txtSinks_TextChanged(object sender, EventArgs e)
        {
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
    }
}