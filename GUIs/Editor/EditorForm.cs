using System;
using System.Reflection;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

using SysCAD;
using SysCAD.Protocol;
using MindFusion.FlowChartX;
using ActiproSoftware.UIStudio.Bar;
using MindFusion.FlowChartX.Commands;
using System.Collections;
using System.Runtime.Remoting.Channels;
using System.Runtime.Remoting.Channels.Tcp;

namespace SysCAD.Editor
{

  internal partial class EditorForm : Form
  {
    FrmFlowChart frmFlowChart;

    static List<PureComponents.TreeView.Node> wasSelectedNodes = new List<PureComponents.TreeView.Node>();

    private delegate void FileCloseProjectDelegate();

    //private void tvNavigation_BeforeNodePositionChange(PureComponents.TreeView.Node oNode)
    //{
    //  oldParent = oNode.Parent;
    //  oldIndex = oNode.Index;
    //}

    private delegate void LoadProjectDelegate(ClientProtocol clientProtocol, Config config);

    Int64 requestId;

    public EditorForm()
    {
      InitializeComponent();
      SetButtonStates();

      BinaryServerFormatterSinkProvider serverProv = new BinaryServerFormatterSinkProvider();
      serverProv.TypeFilterLevel = System.Runtime.Serialization.Formatters.TypeFilterLevel.Full;

      BinaryClientFormatterSinkProvider clientProv = new BinaryClientFormatterSinkProvider();

      Hashtable tcpProps = new Hashtable();
      tcpProps["port"] = "0";
      TcpChannel tcpChannel = new TcpChannel(tcpProps, clientProv, serverProv);
      ChannelServices.RegisterChannel(tcpChannel, false);

      //try { dockManager1.LoadToolWindowLayoutFromFile("Recent.layout"); }
      //catch (Exception) { }

      this.Text = "SysCAD.Editor (" + Assembly.Load("SysCAD.Editor").GetName().Version + ") -- SVN Revision: " + SysCAD.SVNVersion.SVNVersion.version + " -- ";
    }
    
    public void LoadProject(ClientProtocol clientProtocol, Config config)
    {

      if (InvokeRequired)
      {
        BeginInvoke(new LoadProjectDelegate(LoadProject), new object[] { clientProtocol, config });
      }
      else
      {
        // Close the one selected.
        if (frmFlowChart != null)
          FileCloseProject();

        clientProtocol.Connect("SysCAD10\nConnection: Client");
        config.Syncxxx();

        Refresh();

        frmFlowChart = new FrmFlowChart(this);

        SuspendLayout();
        frmFlowChart.SuspendLayout();
        frmFlowChart.FlowChart.SuspendLayout();
        tvNavigation.SuspendLayout();

        frmFlowChart.WindowState = System.Windows.Forms.FormWindowState.Maximized;

        frmFlowChart.MdiParent = this;
        frmFlowChart.Text = clientProtocol.Name;

        frmFlowChart.SetProject(clientProtocol, config, tvNavigation);

        ovOverview.Document = frmFlowChart.FlowChart;

        frmFlowChart.FlowChart.SelectionChanged += new SelectionEvent(this.frmFlowChart_fcFlowChart_SelectionChanged);

        {
          Dictionary<String, Bitmap> modelThumbnails = new Dictionary<String, Bitmap>();
          Dictionary<String, Bitmap> graphicThumbnails = new Dictionary<String, Bitmap>();

          FlowChart thumbnailFlowchart = new FlowChart();

          foreach (String key in frmFlowChart.State.Config.ModelStencils.Keys)
          {
            ModelStencil stencil = config.ModelStencils[key];
            thumbnailFlowchart.DocExtents = frmFlowChart.FlowChart.ClientToDoc(new System.Drawing.Rectangle(0, 0, 17, 17));
            thumbnailFlowchart.ShadowsStyle = ShadowsStyle.None;
            thumbnailFlowchart.BackColor = System.Drawing.SystemColors.Window;
            thumbnailFlowchart.AntiAlias = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
            RectangleF boxRect = thumbnailFlowchart.ClientToDoc(new System.Drawing.Rectangle(1, 1, 13, 13));
            Box box = thumbnailFlowchart.CreateBox(boxRect.X, boxRect.Y, boxRect.Width, boxRect.Height);
            box.Style = BoxStyle.Shape;
            box.Shape = State.GetShapeTemplate(stencil, false, false);
            box.FillColor = System.Drawing.Color.FromArgb(220, 222, 184, 136);
            box.FrameColor = System.Drawing.Color.FromArgb(255, 111, 92, 68);
            box.Locked = true;
            modelThumbnails.Add(key, thumbnailFlowchart.CreateImage());
            thumbnailFlowchart.DeleteObject(box);
          }

          stencilChooser1.PopulateTree(modelThumbnails, config.ModelStencils);
        }

        frmFlowChart.State.ProjectAttached = true;
        SetButtonStates();

        frmFlowChart.Show();

        this.tvNavigation.NodeSelectionChange += new System.EventHandler(this.tvNavigation_NodeSelectionChange);
        this.tvNavigation.AfterNodePositionChange += new PureComponents.TreeView.TreeView.AfterNodePositionChangeEventHandler(this.tvNavigation_AfterNodePositionChange);
        this.tvNavigation.NodeMouseClick += new PureComponents.TreeView.TreeView.NodeMouseClickEventHandler(this.tvNavigation_NodeMouseClick);

        tvNavigation.ClearNodeSelection();

        foreach (PureComponents.TreeView.Node node in tvNavigation.Nodes)
        {
          node.Select();
          node.Expand();
          SelectSubNodes(node);
        }

        tvNavigation.ResumeLayout(true);
        frmFlowChart.FlowChart.ResumeLayout(true);
        frmFlowChart.ResumeLayout(true);
        ResumeLayout(true);

        frmFlowChart.ZoomToVisible();
      }
    }

    public void ModeModify()
    {
      frmFlowChart.FlowChart.Behavior = BehaviorType.Modify;
    }

    public void PermissionsChanged(ClientBaseProtocol.Permissions permissions)
    {
      frmFlowChart.State.Permissions = permissions;
      SetButtonStates();
    }

    private void barManager1_CommandClick(object sender, BarCommandLinkEventArgs e)
    {

      switch (e.Command.FullName)
      {

        case "File.Exit":
          this.Close();
          break;

        case "File.Open":
          this.FileOpenProject();
          break;

        case "File.Save":
          this.FileSaveProject();
          break;

        case "File.Close":
          this.FileCloseProject();
          break;

        case "View.ZoomIn":
          this.ViewZoomIn();
          break;

        case "View.ZoomOut":
          this.ViewZoomOut();
          break;

        case "View.ZoomToSelected":
          this.ViewZoomToSelected();
          break;

        case "View.ZoomToVisible":
          this.ViewZoomToVisible();
          break;

        case "View.ShowGroups":
          this.ViewShowGroups();
          break;

        case "View.ShowModels":
          this.ViewShowModels();
          break;

        case "View.ShowGraphics":
          this.ViewShowGraphics();
          break;

        case "View.ShowLinks":
          this.ViewShowLinks();
          break;

        case "View.ShowTags":
          this.ViewShowTags();
          break;

        case "Tools.Unlock(Demo)":
          this.ToolsUnlock();
          break;

        case "Selection.SelectItems":
          this.ViewSelectItems();
          break;

        case "Selection.SelectLinks":
          this.ViewSelectArrows();
          break;

        case "Mode.Modify":
          this.ModeModify();
          break;

        case "Mode.CreateLink":
          this.ModeCreateLink();
          break;

        case "Edit.Copy":
          this.CopyToClipboard();
          break;

        case "Edit.Cut":
          this.CutToClipboard();
          break;

        case "Edit.Paste":
          this.PasteFromClipboard(10.0F, 10.0F);
          break;

        case "Edit.Delete":
          this.Delete();
          break;

      }
    }

    private void ToolsUnlock()
    {
      if (barManager1.Commands["Tools.Unlock(Demo)"].Text == "Unlock (Demo)")
      {
        frmFlowChart.State.Permissions = new ClientBaseProtocol.Permissions(true, true, true);
        barManager1.Commands["Tools.Unlock(Demo)"].Text= "Lock (Demo)";
        SetButtonStates();
      }
      else
      {
        frmFlowChart.State.Permissions = new ClientBaseProtocol.Permissions(false, false, false);
        barManager1.Commands["Tools.Unlock(Demo)"].Text = "Unlock (Demo)";
        SetButtonStates();
      }
    }

    private void FileCloseProject()
    {

      if (InvokeRequired)
      {
        BeginInvoke(new FileCloseProjectDelegate(FileCloseProject), new object[] { });
      }

      else
      {
        frmFlowChart.State.ProjectAttached = false;
        SetButtonStates();

        frmFlowChart.UnSetProject();

        frmFlowChart.Close();
        tvNavigation.Clear();
        ovOverview.Document = null;
      }
    }

    private void FileOpenProject()
    {
      AttachProjectForm attachProjectForm = new AttachProjectForm();

      if (attachProjectForm.ShowDialog(this) == DialogResult.OK)
      {
        attachProjectForm.Close();

        LoadProject(attachProjectForm.ClientProtocol, attachProjectForm.Config);
      }
    }

    private void FileSaveProject()
    {
      Int64 requestId;
      frmFlowChart.State.ClientProtocol.Save(out requestId);
    }

    private void Form1FormClosing(object sender, FormClosingEventArgs e)
    {
      if (frmFlowChart != null)
        FileCloseProject();

      dockManager1.SaveToolWindowLayoutToFile("Recent.layout");
    }

    private void frmFlowChart_fcFlowChart_SelectionChanged()
    {
      if (!frmFlowChart.State.creatingNode)
      {
        graphicPropertyGrid.Clear();

        frmFlowChart.FlowChart.SelectionChanged -= new SelectionEvent(this.frmFlowChart_fcFlowChart_SelectionChanged);

        if (!frmFlowChart.State.SelectItems)
        {

          if (frmFlowChart.FlowChart.Selection.Boxes.Count > 1)
          {
            BoxCollection boxCollection = frmFlowChart.FlowChart.Selection.Boxes.Clone();

            foreach (Box box in boxCollection)
            {
              box.Selected = false;
            }
          }
        }

        if (!frmFlowChart.State.SelectLinks)
        {

          if (frmFlowChart.FlowChart.Selection.Arrows.Count > 1)
          {
            ArrowCollection arrowCollection = frmFlowChart.FlowChart.Selection.Arrows.Clone();

            foreach (Arrow arrow in arrowCollection)
            {
              arrow.Selected = false;
            }
          }
        }

        foreach (EditorNode item in frmFlowChart.State.Items)
        {
          if (item.GraphicBox.Selected)
          {
            item.ModelBox.Selected = true;
            item.GraphicBox.Selected = false;
            item.TextBox.Selected = false;
          }

          if (item.ModelBox.Selected)
          {
            item.ModelBox.Visible = item.Visible;
            item.ModelBox.ZIndex = item.GraphicBox.ZIndex + 100000;
            item.TextBox.ZIndex = item.GraphicBox.ZIndex - 100000;
            item.TextBox.Visible = item.Visible && frmFlowChart.State.ShowTags;
          }
          else
          {
            item.ModelBox.Visible = item.Visible && frmFlowChart.State.ShowModels;
          }
        }

        foreach (EditorLink link in frmFlowChart.State.Links)
        {
          if (link.Arrow.Selected)
          {
            link.SetVisible(frmFlowChart.State);
          }
        }

        if (frmFlowChart.FlowChart.ActiveObject is Arrow)
        {
          Arrow activeArrow = frmFlowChart.FlowChart.ActiveObject as Arrow;
          GraphicLink graphicLink = frmFlowChart.State.GraphicLink(activeArrow);

          if (graphicLink != null)
          {
            ModelLink modelLink;
            if (frmFlowChart.State.ClientProtocol.model.Links.TryGetValue(graphicLink.ModelGuid, out modelLink))
            {
              graphicPropertyGrid.SetSelectedObject(graphicLink, modelLink, frmFlowChart.State);
            }
            else
            {
              frmFlowChart.State.ClientProtocol.LogMessage(out requestId, "ModelLink missing for GraphcLink (Tag: " + graphicLink.Tag + ", Guid: " + graphicLink.Guid + ")", SysCAD.Log.MessageType.Error);
            }
          }
        }

        if (frmFlowChart.FlowChart.ActiveObject is Box)
        {
          Box activeBox = frmFlowChart.FlowChart.ActiveObject as Box;
          GraphicNode graphicNode = frmFlowChart.State.GraphicNode(activeBox);
          ModelNode modelNode = frmFlowChart.State.ModelNode(activeBox);

          if ((graphicNode != null) && (modelNode != null))
          {
            graphicPropertyGrid.SetSelectedObject(graphicNode, modelNode, frmFlowChart.State);
            modelPropertiesGrid.SetSelectedObject(modelNode, frmFlowChart.State);

            Int64 requestId;
            frmFlowChart.State.PropertyList(out requestId, graphicNode.Guid, graphicNode.Tag, graphicNode.Path);
          }
        }

        frmFlowChart.FlowChart.SelectionChanged += new SelectionEvent(this.frmFlowChart_fcFlowChart_SelectionChanged);
      }
    }

    private void frmFlowChart_fcFlowChart_SelectionChanged(object sender, EventArgs e)
    {
      frmFlowChart_fcFlowChart_SelectionChanged();
    }

    private void ModeCreateLink()
    {
      frmFlowChart.FlowChart.Behavior = BehaviorType.CreateArrow;
    }

    private void RePathNodes(PureComponents.TreeView.Node node)
    {
      if (IsLeaf(node))
      {
        //if (IsItem(node))
          //frmFlowChart.State.ModifyGraphicNodePath(out requestId, new Guid(node.Key), node.FullPath);

        //if (IsThing(node))
          //frmFlowChart.State.ModifyGraphicThingPath(out requestId, new Guid(node.Key), node.FullPath);
      }

      foreach (PureComponents.TreeView.Node subNode in node.Nodes)
      {
        RePathNodes(subNode);
      }
    }

    private void SelectSubNodes(PureComponents.TreeView.Node node)
    {
      bool isSelected = node.IsSelected;
      bool wasSelected = wasSelectedNodes.Contains(node);

      if (wasSelected == isSelected)
      {
        // Nothings changed at area level, update flowchart with innerNodes selection status.
        foreach (PureComponents.TreeView.Node innerNode in node.Nodes)
        {
          frmFlowChart.State.SetVisible(innerNode.Key, innerNode.IsSelected);

          SelectSubNodes(innerNode);
        }
      }

      else if (wasSelected && !isSelected)
      {
        // Been deselected, deselect all underlings and update flowchart.
        foreach (PureComponents.TreeView.Node innerNode in node.Nodes)
        {
          frmFlowChart.State.SetVisible(innerNode.Key, false);

          tvNavigation.RemoveSelectedNode(innerNode);
          SelectSubNodes(innerNode);
        }
      }

      else if (!wasSelected && isSelected)
      {
        // Been selected, select all underlings and update flowchart.
        foreach (PureComponents.TreeView.Node innerNode in node.Nodes)
        {
          frmFlowChart.State.SetVisible(innerNode.Key, true);

          tvNavigation.AddSelectedNode(innerNode);
          SelectSubNodes(innerNode);
        }
      }
    }

    private delegate void SetButtonStatesDelegate();

    private void SetButtonStates()
    {
      if (InvokeRequired)
      {
        BeginInvoke(new SetButtonStatesDelegate(SetButtonStates));
      }
      else
      {
        bool projectOpen = false;
        ClientBaseProtocol.Permissions permissions = new ClientBaseProtocol.Permissions(false, false, false);

        if (frmFlowChart != null)
        {
          projectOpen = frmFlowChart.State.ProjectAttached;
          permissions = frmFlowChart.State.Permissions;

          // Uncomment to enable create/modify always.
          //permissions.Create = true;
          //permissions.Modify = true;
        }

        barManager1.Commands["File.PrintPreview"].Enabled = projectOpen;
        barManager1.Commands["File.Print"].Enabled = projectOpen;
        barManager1.Commands["File.Save"].Enabled = projectOpen;
        barManager1.Commands["File.Close"].Enabled = projectOpen;
        barManager1.Commands["View.ZoomIn"].Enabled = projectOpen;
        barManager1.Commands["View.ZoomOut"].Enabled = projectOpen;
        barManager1.Commands["View.ZoomToVisible"].Enabled = projectOpen;
        barManager1.Commands["View.ZoomToSelected"].Enabled = projectOpen;
        barManager1.Commands["View.ShowGroups"].Enabled = projectOpen;
        barManager1.Commands["View.ShowModels"].Enabled = projectOpen;
        barManager1.Commands["View.ShowGraphics"].Enabled = projectOpen;
        barManager1.Commands["View.ShowLinks"].Enabled = projectOpen;
        barManager1.Commands["View.ShowTags"].Enabled = projectOpen;
        barManager1.Commands["Tools.Unlock(Demo)"].Enabled = projectOpen;
        barManager1.Commands["Selection.SelectItems"].Enabled = projectOpen;
        barManager1.Commands["Selection.SelectLinks"].Enabled = projectOpen;
        barManager1.Commands["Mode.Modify"].Enabled = projectOpen && permissions.Modify;
        
        barManager1.Commands["Mode.CreateLink"].Enabled = projectOpen && permissions.Create;
        stencilChooser1.Enabled = projectOpen && permissions.Create;

        barManager1.Commands["Edit.Cut"].Enabled = projectOpen && permissions.Delete;
        barManager1.Commands["Edit.Copy"].Enabled = projectOpen;
        barManager1.Commands["Edit.Delete"].Enabled = projectOpen;
        barManager1.Commands["Edit.Paste"].Enabled = projectOpen && permissions.Create;

        if (frmFlowChart != null)
        {
          frmFlowChart.State.SelectItems = true;
          frmFlowChart.State.SelectLinks = false;
        }

        ((IBarCheckableCommand)barManager1.Commands["Selection.SelectItems"]).Checked = true;
        ((IBarCheckableCommand)barManager1.Commands["Selection.SelectLinks"]).Checked = false;
      }
    }

    private void tvNavigation_AfterNodePositionChange(PureComponents.TreeView.Node oNode)
    {
      tvNavigation.ClearNodeSelection();

      foreach (MindFusion.FlowChartX.ChartObject chartObject in frmFlowChart.FlowChart.Objects)
        chartObject.Visible = false;

      RePathNodes(oNode);

      if (oNode.Parent != null)
        oNode.Parent.Select();

      else
        oNode.Select();
    }

    private void tvNavigation_NodeMouseClick(EventArgs e, PureComponents.TreeView.Node oNode)
    {

      // Select only this area/item if navigation node clicked.
      if (Control.ModifierKeys == Keys.None)
      {
        tvNavigation.ClearNodeSelection();
        oNode.Select();
      }
    }

    private void tvNavigation_NodeSelectionChange(object sender, EventArgs e)
    {
      tvNavigation.NodeSelectionChange -= new System.EventHandler(this.tvNavigation_NodeSelectionChange);

      if (tvNavigation.SelectedNodes.Length > 0)
      {

        foreach (PureComponents.TreeView.Node node in tvNavigation.SelectedNodes)
        {
          if (IsBranch(node))
            SelectSubNodes(node);

          if (node.Key != null)
            frmFlowChart.State.SetVisible(node.Key, node.IsSelected);
        }

        wasSelectedNodes.Clear();

        foreach (PureComponents.TreeView.Node node in tvNavigation.SelectedNodes)
          wasSelectedNodes.Add(node);

        frmFlowChart.ZoomToVisible();
      }

      else
      {

        foreach (PureComponents.TreeView.Node node in wasSelectedNodes)
        {
          frmFlowChart.State.SetVisible(node.Key, false);
        }

        wasSelectedNodes.Clear();
      }

      tvNavigation.NodeSelectionChange += new System.EventHandler(this.tvNavigation_NodeSelectionChange);
    }

    /// <summary>Determine if this node represents a leaf (i.e. item or thing.)</summary>
    /// <param name="node">Node to be tested</param>
    /// <returns>True if node represents a leaf.</returns>
    private bool IsLeaf(PureComponents.TreeView.Node node)
    {
      return ((node.Tag != null) && ((node.Tag is EditorNode) || (node.Tag is Thing)));
    }

    /// <summary>Determine if this node represents a item.</summary>
    /// <param name="node">Node to be tested</param>
    /// <returns>True if node represents a item.</returns>
    private bool IsItem(PureComponents.TreeView.Node node)
    {
      return ((node.Tag != null) && (node.Tag is EditorNode));
    }

    /// <summary>Determine if this node represents a thing.</summary>
    /// <param name="node">Node to be tested</param>
    /// <returns>True if node represents a thing.</returns>
    private bool IsThing(PureComponents.TreeView.Node node)
    {
      return ((node.Tag != null) && (node.Tag is Thing));
    }

    /// <summary>Determine if this node represents a branch (i.e. root or group.)</summary>
    /// <param name="node">Node to be tested</param>
    /// <returns>True if node represents a branch.</returns>
    private bool IsBranch(PureComponents.TreeView.Node node)
    {
      return ((node.Tag == null) || (node.Tag is EditorGroup));
    }

    /// <summary>Determine if this node represents a group.</summary>
    /// <param name="node">Node to be tested</param>
    /// <returns>True if node represents a group.</returns>
    private bool IsGroup(PureComponents.TreeView.Node node)
    {
      return ((node.Tag != null) && (node.Tag is EditorGroup));
    }

    private void ViewSelectArrows()
    {
      frmFlowChart.State.SelectLinks = true;
      ((IBarCheckableCommand)barManager1.Commands["Selection.SelectLinks"]).Checked = true;

      frmFlowChart.State.SelectItems = false;
      ((IBarCheckableCommand)barManager1.Commands["Selection.SelectItems"]).Checked = false;

      frmFlowChart_fcFlowChart_SelectionChanged();
    }

    private void ViewSelectItems()
    {
      frmFlowChart.State.SelectItems = true;
      ((IBarCheckableCommand)barManager1.Commands["Selection.SelectItems"]).Checked = true;

      frmFlowChart.State.SelectLinks = false;
      ((IBarCheckableCommand)barManager1.Commands["Selection.SelectLinks"]).Checked = false;

      frmFlowChart_fcFlowChart_SelectionChanged();
    }

    private void ViewShowGraphics()
    {
      frmFlowChart.State.ShowGraphics = ((IBarCheckableCommand)barManager1.Commands["View.ShowGraphics"]).Checked;

      foreach (EditorNode item in frmFlowChart.State.Items)
      {
        item.GraphicBox.Visible = item.Visible && frmFlowChart.State.ShowGraphics;
        item.ModelBox.ZIndex = item.GraphicBox.ZIndex + 100000;
        item.TextBox.ZIndex = item.GraphicBox.ZIndex - 100000;
      }
    }

    private void ViewShowLinks()
    {
      frmFlowChart.State.ShowLinks = ((IBarCheckableCommand)barManager1.Commands["View.ShowLinks"]).Checked;

      foreach (Arrow arrow in frmFlowChart.FlowChart.Arrows)
      {
        EditorLink link = arrow.Tag as EditorLink;
        if (link != null)
        {
          link.SetVisible(frmFlowChart.State);
        }
        else
        {
          frmFlowChart.State.ClientProtocol.LogMessage(out requestId, "EditorLink missing for Arrow (Text: " + arrow.Text + ".)", SysCAD.Log.MessageType.Error);
        }
      }
    }

    private void ViewShowGroups()
    {
      frmFlowChart.State.ShowGroups = ((IBarCheckableCommand)barManager1.Commands["View.ShowGroups"]).Checked;

      foreach (EditorGroup group in frmFlowChart.State.Groups)
      {
        group.Box.Visible = group.Visible && frmFlowChart.State.ShowGroups;
        group.Box.ZBottom();
      }
    }

    private void ViewShowModels()
    {
      frmFlowChart.State.ShowModels = ((IBarCheckableCommand)barManager1.Commands["View.ShowModels"]).Checked;

      foreach (EditorNode item in frmFlowChart.State.Items)
      {
        item.ModelBox.Visible = item.Visible && frmFlowChart.State.ShowModels;
        item.ModelBox.ZIndex = item.GraphicBox.ZIndex + 100000;
        item.TextBox.ZIndex = item.GraphicBox.ZIndex - 100000;
      }
    }

    private void ViewShowTags()
    {
      frmFlowChart.State.ShowTags = ((IBarCheckableCommand)barManager1.Commands["View.ShowTags"]).Checked;

      foreach (EditorLink link in frmFlowChart.State.Links)
      {
        link.SetVisible(frmFlowChart.State);
      }

      foreach (EditorNode item in frmFlowChart.State.Items)
      {
        item.SetVisible(frmFlowChart.State);
      }

      frmFlowChart.FlowChart.Invalidate();
    }

    private void ViewZoomIn()
    {
      frmFlowChart.FixDocExtents();

      frmFlowChart.FlowChart.ZoomIn();

      frmFlowChart.SetSizes();
    }

    private void ViewZoomOut()
    {
      frmFlowChart.FixDocExtents();

      frmFlowChart.FlowChart.ZoomOut();

      frmFlowChart.SetSizes();
    }

    private void ViewZoomToSelected()
    {
      frmFlowChart.ZoomToSelected();
    }

    private void ViewZoomToVisible()
    {
      frmFlowChart.ZoomToVisible();
    }

    public ActiproSoftware.UIStudio.Bar.BarManager BarManager1
    {
      get { return barManager1; }
      set { barManager1 = value; }
    }

    public GraphicPropertyGrid GraphicPropertyGrid
    {
      get { return graphicPropertyGrid; }
      set { graphicPropertyGrid = value; }
    }

    public ToolStripStatusLabel ToolStripStatusLabel
    {
      get { return toolStripStatusLabel; }
      set { toolStripStatusLabel = value; }
    }

    #region clipboard support

    public void CopyToClipboard()
    {
      // create clones of selected items
      List<Guid> copyGuids = copySelection(frmFlowChart.FlowChart);

      DataFormats.Format format =
           DataFormats.GetFormat("Kenwalt.GraphicGuids");

      //now copy to clipboard
      IDataObject dataObj = new DataObject();
      dataObj.SetData(format.Name, false, copyGuids);
      Clipboard.SetDataObject(dataObj, false);
    }

    public void CutToClipboard()
    {
      CopyToClipboard();

      // that returns the active composite if somebody has already created one
      CompositeCmd composite = frmFlowChart.FlowChart.UndoManager.StartComposite("_Kenwalt.SysCAD_");

      // delete selected items
      frmFlowChart.State.Remove(frmFlowChart.FlowChart);

      if (composite != null && composite.Title == "_Kenwalt.SysCAD_")
      {
        // this is our own composite cmd
        composite.Title = "Cut";
        composite.Execute();
      }
    }

    public void PasteFromClipboard(float dx, float dy)
    {
      try
      {
        // try getting clipboard data; might throw exceptions
        IDataObject dataObj = Clipboard.GetDataObject();

        // is there anything of interest in the clipboard ?
        if (dataObj != null && dataObj.GetDataPresent("Kenwalt.GraphicGuids"))
        {
          List<Guid> pasteData = dataObj.GetData("Kenwalt.GraphicGuids") as List<Guid>;

          frmFlowChart.State.newElementSelectionList.Clear();

          if (pasteData != null)
          {

            foreach (Guid guid in pasteData)
            {
              //graphicNode.X += dx;
              //graphicNode.Y += dy;
              throw new NotImplementedException("The method or operation is not implemented.");
              //frmFlowChart.NewGraphicNode(out guid, graphicNode, tvNavigation.SelectedNode.FullPath + tvNavigation.PathSeparator);
              //frmFlowChart.State.newElementSelectionList.Add(guid);
            }

            //foreach (GraphicLink graphicLink in pasteData.graphicLinks.Values)
            //{
            //  List<SysCAD.Protocol.Point> newControlPoints = new List<SysCAD.Protocol.Point>();
            //  foreach (SysCAD.Protocol.Point point in graphicLink.ControlPoints)
            //  {
            //    newControlPoints.Add(new SysCAD.Protocol.Point(point.X + dx, point.Y + dy));
            //  }
            //  graphicLink.ControlPoints = newControlPoints;

            //  throw new NotImplementedException("The method or operation is not implemented.");
            //  //frmFlowChart.NewGraphicLink(out guid, graphicLink);
            //  frmFlowChart.State.newElementSelectionList.Add(guid);

            //  //GraphicLink newGraphicLink = new GraphicLink(graphicLink.Tag);

            //  //newGraphicLink.Destination = graphicLink.Destination;
            //  //newGraphicLink.Origin = graphicLink.Origin;

            //  //foreach (PointF point in graphicLink.ControlPoints)
            //  //{
            //  //  newGraphicLink.ControlPoints.Add(new PointF(point.X, point.Y));
            //  //}

            //  //// use new tags for connected items.
            //  ////if (tagConversion.ContainsKey(newGraphicLink.Source))
            //  ////  newGraphicLink.Source = tagConversion[newGraphicLink.Source];
            //  ////else
            //  ////  newGraphicLink.Source = "";
            //  ////if (tagConversion.ContainsKey(newGraphicLink.Destination))
            //  ////  newGraphicLink.Destination = tagConversion[newGraphicLink.Destination];
            //  ////else
            //  ////  newGraphicLink.Destination = "";

            //  //frmFlowChart.NewGraphicLink(guid, newGraphicLink, dx, dy);
            //  //frmFlowChart.newItemSelectionList.Add(guid);
            //}

            //foreach (GraphicThing graphicThing in pasteData.graphicThings.Values)
            //{
            //  graphicThing.X += dx;
            //  graphicThing.Y += dy;
            //  frmFlowChart.NewGraphicThing(out guid, graphicThing, tvNavigation.SelectedNode.FullPath + tvNavigation.PathSeparator);
            //  frmFlowChart.State.newElementSelectionList.Add(guid);
            //}

            //// that returns the active composite if somebody has already created one
            //CompositeCmd composite = frmFlowChart.fcFlowChart.UndoManager.StartComposite("_Kenwalt.SysCAD_");

            //// add the copied items to the document
            //result = pasteSelection(frmFlowChart.fcFlowChart, data, composite, dx, dy);

            //if (composite != null && composite.Title == "_Kenwalt.SysCAD_")
            //{
            //  // this is our own composite cmd
            //  composite.Title = "Paste";
            //  composite.Execute();
            //}

            //// fire *pasted events
            //foreach (Item item in data.items.Values)
            //  fireItemPasted(item);
          }
        }
      }

      catch (System.Runtime.InteropServices.ExternalException)
      {
        // just go on our merry way.
      }
    }

    private List<Guid> copySelection(FlowChart doc)
    {

      if (doc.Selection.Objects.Count == 0)
        return null;

      List<Guid> copyGuids = new List<Guid>();

      foreach (Box box in doc.Selection.Boxes)
      {
        GraphicNode graphicNode = frmFlowChart.State.GraphicNode(box);
        if (graphicNode != null)
          copyGuids.Add(graphicNode.Guid);

        GraphicThing graphicThing = frmFlowChart.State.GraphicThing(box);
        if (graphicThing != null)
          copyGuids.Add(graphicThing.Guid);
      }

      foreach (Arrow arrow in doc.Selection.Arrows)
      {
        GraphicLink graphicLink = frmFlowChart.State.GraphicLink(arrow);
        if (graphicLink != null)
          copyGuids.Add(graphicLink.Guid);
      }

      return copyGuids;
    }

    public void Delete()
    {
      frmFlowChart.DeleteSelection();
    }

    #endregion

  }
}
