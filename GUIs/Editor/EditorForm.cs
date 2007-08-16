using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

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

    public EditorForm()
    {
      //String testXaml = State.PreprocessXaml("test [[Test tag, 0, 5, #0, #ffffff, Linear, Hex]] test some more");

      InitializeComponent();
      SetButtonStates();

      BinaryServerFormatterSinkProvider serverProv = new BinaryServerFormatterSinkProvider();
      serverProv.TypeFilterLevel = System.Runtime.Serialization.Formatters.TypeFilterLevel.Full;

      BinaryClientFormatterSinkProvider clientProv = new BinaryClientFormatterSinkProvider();

      Hashtable tcpProps = new Hashtable();
      tcpProps["port"] = "0";
      TcpChannel tcpChannel = new TcpChannel(tcpProps, clientProv, serverProv);
      ChannelServices.RegisterChannel(tcpChannel, false);

      try
      {

        dockManager1.LoadToolWindowLayoutFromFile("Recent.layout");
      }

      catch (FileNotFoundException) { }

      (barManager1.Commands["CreateItem.ModelType"] as BarComboBoxCommand).SelectedIndexChanged += new EventHandler(NewItem_ModelType_Changed);
      (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).SelectedIndexChanged += new EventHandler(NewItem_GraphicType_Changed);
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

        clientProtocol.Connect();
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

        (barManager1.Commands["CreateItem.ModelType"] as BarComboBoxCommand).Items.Clear();

        foreach (ModelStencil modelStencil in frmFlowChart.State.ModelStencils)
        {
          (barManager1.Commands["CreateItem.ModelType"] as BarComboBoxCommand).Items.Add(modelStencil.Tag);
        }
        (barManager1.Commands["CreateItem.ModelType"] as BarComboBoxCommand).SelectedIndex = 3;


        frmFlowChart.State.ProjectOpen = true;
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
      barManager1.Commands["CreateItem.GraphicType"].Enabled = false;
      barManager1.Commands["CreateItem.ModelType"].Enabled = false;
      frmFlowChart.FlowChart.Behavior = BehaviorType.Modify;
      (barManager1.Commands["Mode.Modify"] as BarButtonCommand).Checked = true;
      (barManager1.Commands["Mode.CreateNode"] as BarButtonCommand).Checked = false;
      (barManager1.Commands["Mode.CreateLink"] as BarButtonCommand).Checked = false;
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

        case "Selection.SelectItems":
          this.ViewSelectItems();
          break;

        case "Selection.SelectLinks":
          this.ViewSelectArrows();
          break;

        case "Mode.Modify":
          this.ModeModify();
          break;

        case "Mode.CreateNode":
          this.ModeCreateNode();
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
        frmFlowChart.State.ProjectOpen = false;
        SetButtonStates();

        frmFlowChart.UnSetProject();

        frmFlowChart.Close();
        tvNavigation.Clear();
        ovOverview.Document = null;
      }
    }

    private void FileOpenProject()
    {
      OpenProjectForm openProjectForm = new OpenProjectForm();

      if (openProjectForm.ShowDialog(this) == DialogResult.OK)
      {
        openProjectForm.Close();

        LoadProject(openProjectForm.ClientProtocol, openProjectForm.Config);
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

      //dockManager1.SaveToolWindowLayoutToFile("Recent.layout");
    }

    private void frmFlowChart_fcFlowChart_SelectionChanged()
    {
      if (!frmFlowChart.State.creatingItem)
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

        foreach (Item item in frmFlowChart.State.Items)
        {

          if (item.Graphic.Selected)
          {
            item.Model.Selected = true;
            item.Graphic.Selected = false;
            item.Text.Selected = false;
          }

          if (item.Model.Selected)
          {
            item.Model.Visible = item.Visible;
            item.Model.ZIndex = item.Graphic.ZIndex + 100000;
            item.Text.ZIndex = item.Graphic.ZIndex - 100000;
            item.Text.Visible = item.Visible && frmFlowChart.State.ShowTags;
          }

          else
          {
            item.Model.Visible = item.Visible && frmFlowChart.State.ShowModels;
          }
        }

        if (frmFlowChart.FlowChart.ActiveObject is Arrow)
        {
          Arrow activeArrow = frmFlowChart.FlowChart.ActiveObject as Arrow;
          GraphicLink graphicLink = frmFlowChart.State.GraphicLink(activeArrow);

          if (graphicLink != null)
          {
            graphicPropertyGrid.SetSelectedObject(graphicLink, frmFlowChart.State);
          }
        }

        if (frmFlowChart.FlowChart.ActiveObject is Box)
        {
          Box activeBox = frmFlowChart.FlowChart.ActiveObject as Box;
          GraphicItem graphicItem = frmFlowChart.State.GraphicItem(activeBox);

          if (graphicItem != null)
          {
            graphicPropertyGrid.SetSelectedObject(graphicItem, frmFlowChart.State);

            ModelItem modelItem = new ModelItem(graphicItem.Guid);
            modelPropertiesGrid.SetSelectedObject(modelItem, frmFlowChart.State);

            Int64 requestId;
            frmFlowChart.State.PropertyList(out requestId, graphicItem.Guid, graphicItem.Tag, graphicItem.Path);

            int i;

            i = 0;

            foreach (String model in (barManager1.Commands["CreateItem.ModelType"] as BarComboBoxCommand).Items)
            {

              if (model == graphicItem.Model)
              {
                (barManager1.Commands["CreateItem.ModelType"] as BarComboBoxCommand).SelectedIndex = i;
                (barManager1.Commands["CreateItem.ModelType"] as BarComboBoxCommand).Text = model;
              }
              i++;
            }

            if ((barManager1.Commands["CreateItem.ModelType"] as BarComboBoxCommand).SelectedIndex == -1)
              (barManager1.Commands["CreateItem.ModelType"] as BarComboBoxCommand).SelectedIndex = 0;

            ModelStencil modelStencil = frmFlowChart.State.ModelShape(graphicItem.Model);

            if (modelStencil != null)
            {
              GraphicType_Populate(modelStencil.GroupName);
            }

            i = 0;

            foreach (String shape in (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).Items)
            {

              if (shape == graphicItem.Shape)
              {
                (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).SelectedIndex = i;
                (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).Text = shape;
              }
              i++;
            }

            if ((barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).SelectedIndex == -1)
              (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).SelectedIndex = 0;
          }
        }

        frmFlowChart.FlowChart.SelectionChanged += new SelectionEvent(this.frmFlowChart_fcFlowChart_SelectionChanged);
      }
    }

    private void frmFlowChart_fcFlowChart_SelectionChanged(object sender, EventArgs e)
    {
      frmFlowChart_fcFlowChart_SelectionChanged();
    }

    private void GraphicType_Populate(string groupName)
    {
      (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).Items.Clear();
      ShapeConverter.ClearStencilList();

      foreach (GraphicStencil graphicStencil in frmFlowChart.State.GraphicStencils)
      {

        if (groupName == graphicStencil.groupName)
        {
          ShapeConverter.AddStencil(graphicStencil.Tag);
        }
      }

      (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).Items.Add("-------");
      ShapeConverter.AddStencil("-------");

      foreach (GraphicStencil graphicStencil in frmFlowChart.State.GraphicStencils)
      {

        if (groupName != graphicStencil.groupName)
        {
          (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).Items.Add(graphicStencil.Tag);
          ShapeConverter.AddStencil(graphicStencil.Tag);
        }
      }
      (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).SelectedIndex = 0;
      (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).Text = groupName;
    }

    private void ModeCreateLink()
    {
      barManager1.Commands["CreateItem.GraphicType"].Enabled = false;
      barManager1.Commands["CreateItem.ModelType"].Enabled = false;
      frmFlowChart.FlowChart.Behavior = BehaviorType.CreateArrow;
    }

    private void ModeCreateNode()
    {
      barManager1.Commands["CreateItem.GraphicType"].Enabled = true;
      barManager1.Commands["CreateItem.ModelType"].Enabled = true;
      //frmFlowChart.fcFlowChart.Behavior = BehaviorType.CreateBox;
    }

    void NewItem_GraphicType_Changed(object sender, EventArgs e)
    {
      int stencilIndex = (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).SelectedIndex;

      if (stencilIndex != -1)
      {
        String stencilName = (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).Items[stencilIndex] as string;

        if (stencilName != "-------")
        {
          frmFlowChart.CurrentStencil = stencilName;
        }
      }
    }

    void NewItem_ModelType_Changed(object sender, EventArgs e)
    {
      string groupName = "";

      bool rememberEnabled = barManager1.Commands["CreateItem.GraphicType"].Enabled;
      barManager1.Commands["CreateItem.GraphicType"].Enabled = false;

      int stencilIndex = (barManager1.Commands["CreateItem.ModelType"] as BarComboBoxCommand).SelectedIndex;
      string stencilName = (barManager1.Commands["CreateItem.ModelType"] as BarComboBoxCommand).Items[stencilIndex] as string;
      frmFlowChart.CurrentModel = stencilName;

      ModelStencil modelStencil = frmFlowChart.State.ModelShape(stencilName);

      if (modelStencil != null)
      {
        groupName = modelStencil.GroupName;
        barManager1.Commands["CreateItem.GraphicType"].Enabled = rememberEnabled;
      }

      else
      {
        return;
      }

      GraphicType_Populate(groupName);
    }

    private void RePathNodes(PureComponents.TreeView.Node node)
    {
      if (IsLeaf(node))
      {
        Int64 requestId;

        if (IsItem(node))
          frmFlowChart.State.ModifyGraphicItemPath(out requestId, new Guid(node.Key), node.FullPath);

        if (IsThing(node))
          frmFlowChart.State.ModifyGraphicThingPath(out requestId, new Guid(node.Key), node.FullPath);
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
          projectOpen = frmFlowChart.State.ProjectOpen;
          permissions = frmFlowChart.State.Permissions;
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
        barManager1.Commands["Selection.SelectItems"].Enabled = projectOpen;
        barManager1.Commands["Selection.SelectLinks"].Enabled = projectOpen;
        barManager1.Commands["CreateItem.ModelType"].Enabled = projectOpen && permissions.Create;
        barManager1.Commands["Mode.Modify"].Enabled = projectOpen && permissions.Modify;
        barManager1.Commands["Mode.CreateNode"].Enabled = projectOpen && permissions.Create;
        barManager1.Commands["Mode.CreateLink"].Enabled = projectOpen && permissions.Create;
        barManager1.Commands["Edit.Cut"].Enabled = projectOpen && permissions.Delete;
        barManager1.Commands["Edit.Copy"].Enabled = projectOpen;
        barManager1.Commands["Edit.Paste"].Enabled = projectOpen && permissions.Create;

        barManager1.Commands["CreateItem.GraphicType"].Enabled = false;
        barManager1.Commands["CreateItem.ModelType"].Enabled = false;

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
      return ((node.Tag != null) && ((node.Tag is Item) || (node.Tag is Thing)));
    }

    /// <summary>Determine if this node represents a item.</summary>
    /// <param name="node">Node to be tested</param>
    /// <returns>True if node represents a item.</returns>
    private bool IsItem(PureComponents.TreeView.Node node)
    {
      return ((node.Tag != null) && (node.Tag is Item));
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
      return ((node.Tag == null) || (node.Tag is Group));
    }

    /// <summary>Determine if this node represents a group.</summary>
    /// <param name="node">Node to be tested</param>
    /// <returns>True if node represents a group.</returns>
    private bool IsGroup(PureComponents.TreeView.Node node)
    {
      return ((node.Tag != null) && (node.Tag is Group));
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

      foreach (Item item in frmFlowChart.State.Items)
      {
        item.Graphic.Visible = item.Visible && frmFlowChart.State.ShowGraphics;
        item.Model.ZIndex = item.Graphic.ZIndex + 100000;
        item.Text.ZIndex = item.Graphic.ZIndex - 100000;
      }
    }

    private void ViewShowLinks()
    {
      frmFlowChart.State.ShowLinks = ((IBarCheckableCommand)barManager1.Commands["View.ShowLinks"]).Checked;

      foreach (Arrow arrow in frmFlowChart.FlowChart.Arrows)
      {
        bool visible = true;
        Item origin = arrow.Origin.Tag as Item;
        Item destination = arrow.Destination.Tag as Item;

        // only set to false if the endpoint exists and is invisible.
        // disconnected arrows must be visible, because otherwise they'll never appear.
        if (origin != null) visible = visible && origin.Visible;

        if (destination != null) visible = visible && destination.Visible;

        arrow.Visible = visible && frmFlowChart.State.ShowLinks;
      }
    }

    private void ViewShowGroups()
    {
      frmFlowChart.State.ShowGroups = ((IBarCheckableCommand)barManager1.Commands["View.ShowGroups"]).Checked;

      foreach (Group group in frmFlowChart.State.Groups)
      {
        group.Box.Visible = group.Visible && frmFlowChart.State.ShowGroups;
        group.Box.ZBottom();
      }
    }

    private void ViewShowModels()
    {
      frmFlowChart.State.ShowModels = ((IBarCheckableCommand)barManager1.Commands["View.ShowModels"]).Checked;

      foreach (Item item in frmFlowChart.State.Items)
      {
        item.Model.Visible = item.Visible && frmFlowChart.State.ShowModels;
        item.Model.ZIndex = item.Graphic.ZIndex + 100000;
        item.Text.ZIndex = item.Graphic.ZIndex - 100000;
      }
    }

    private void ViewShowTags()
    {
      frmFlowChart.State.ShowTags = ((IBarCheckableCommand)barManager1.Commands["View.ShowTags"]).Checked;

      foreach (Link link in frmFlowChart.State.Links)
      {

        if (frmFlowChart.State.ShowTags)
          link.Arrow.Text = link.Tag;

        else
          link.Arrow.Text = "";
      }

      foreach (Item item in frmFlowChart.State.Items)
      {
        item.Text.Visible = frmFlowChart.State.ShowTags;
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

    //private Dictionary<string, GraphicLink> clipBoardGraphicLinks;
    //private Dictionary<string, GraphicItem> clipBoardGraphicItems;
    //private Dictionary<String, GraphicThing> clipBoardGraphicThings;

    #region clipboard support

    public void CopyToClipboard()
    {
      // create clones of selected items
      ClientProtocol data = copySelection(frmFlowChart.FlowChart);

      DataFormats.Format format =
           DataFormats.GetFormat("Kenwalt.GraphicData");

      //now copy to clipboard
      IDataObject dataObj = new DataObject();
      dataObj.SetData(format.Name, false, data);
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
      Guid guid;
      try
      {
        // try getting clipboard data; might throw exceptions
        IDataObject dataObj = Clipboard.GetDataObject();

        // is there anything of interest in the clipboard ?
        if (dataObj != null && dataObj.GetDataPresent("Kenwalt.GraphicData"))
        {
          BaseProtocol pasteData = dataObj.GetData("Kenwalt.GraphicData") as BaseProtocol;

          frmFlowChart.State.newElementSelectionList.Clear();

          if (pasteData != null)
          {

            foreach (GraphicItem graphicItem in pasteData.graphicItems.Values)
            {
              graphicItem.X += dx;
              graphicItem.Y += dy;
              frmFlowChart.NewGraphicItem(out guid, graphicItem, tvNavigation.SelectedNode.FullPath + tvNavigation.PathSeparator);
              frmFlowChart.State.newElementSelectionList.Add(guid);
            }

            foreach (GraphicLink graphicLink in pasteData.graphicLinks.Values)
            {
              List<PointF> newControlPoints = new List<PointF>();
              foreach (PointF point in graphicLink.ControlPoints)
              {
                newControlPoints.Add(new PointF(point.X + dx, point.Y + dy));
              }
              graphicLink.ControlPoints = newControlPoints;

              frmFlowChart.NewGraphicLink(out guid, graphicLink);
              frmFlowChart.State.newElementSelectionList.Add(guid);

              //GraphicLink newGraphicLink = new GraphicLink(graphicLink.Tag);

              //newGraphicLink.Destination = graphicLink.Destination;
              //newGraphicLink.Origin = graphicLink.Origin;

              //foreach (PointF point in graphicLink.ControlPoints)
              //{
              //  newGraphicLink.ControlPoints.Add(new PointF(point.X, point.Y));
              //}

              //// use new tags for connected items.
              ////if (tagConversion.ContainsKey(newGraphicLink.Source))
              ////  newGraphicLink.Source = tagConversion[newGraphicLink.Source];
              ////else
              ////  newGraphicLink.Source = "";
              ////if (tagConversion.ContainsKey(newGraphicLink.Destination))
              ////  newGraphicLink.Destination = tagConversion[newGraphicLink.Destination];
              ////else
              ////  newGraphicLink.Destination = "";

              //frmFlowChart.NewGraphicLink(guid, newGraphicLink, dx, dy);
              //frmFlowChart.newItemSelectionList.Add(guid);
            }

            foreach (GraphicThing graphicThing in pasteData.graphicThings.Values)
            {
              graphicThing.X += dx;
              graphicThing.Y += dy;
              frmFlowChart.NewGraphicThing(out guid, graphicThing, tvNavigation.SelectedNode.FullPath + tvNavigation.PathSeparator);
              frmFlowChart.State.newElementSelectionList.Add(guid);
            }

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

    private ClientProtocol copySelection(FlowChart doc)
    {

      if (doc.Selection.Objects.Count == 0)
        return null;

      ClientProtocol copyClientProtocol = new ClientProtocol();

      foreach (Box box in doc.Selection.Boxes)
      {
        GraphicItem graphicItem = frmFlowChart.State.GraphicItem(box);

        if (graphicItem != null)
        {
          GraphicItem copyGraphicItem = new GraphicItem(box.Text);
          copyGraphicItem.BoundingRect = graphicItem.BoundingRect;
          copyGraphicItem.Angle = graphicItem.Angle;
          copyGraphicItem.Model = graphicItem.Model;
          copyGraphicItem.Shape = graphicItem.Shape;
          copyGraphicItem.MirrorX = graphicItem.MirrorX;
          copyGraphicItem.MirrorY = graphicItem.MirrorY;
          copyGraphicItem.FillColor = graphicItem.FillColor;

          copyClientProtocol.graphicItems.Add(copyGraphicItem.Guid, copyGraphicItem);
        }

        GraphicThing graphicThing = frmFlowChart.State.GraphicThing(box);

        if (graphicThing != null)
        {
          GraphicThing copyGraphicThing = new GraphicThing(graphicThing.Tag);
          copyGraphicThing.BoundingRect = graphicThing.BoundingRect;
          copyGraphicThing.Xaml = graphicThing.Xaml;
          copyGraphicThing.Angle = graphicThing.Angle;
          copyGraphicThing.MirrorX = graphicThing.MirrorX;
          copyGraphicThing.MirrorY = graphicThing.MirrorY;

          copyClientProtocol.graphicThings.Add(copyGraphicThing.Guid, copyGraphicThing);
        }

      }

      foreach (Arrow arrow in doc.Selection.Arrows)
      {
        GraphicLink graphicLink = frmFlowChart.State.GraphicLink((arrow.Tag as Link).Guid);

        if (graphicLink != null)
        {
          GraphicLink copyGraphicLink = new GraphicLink(arrow.Text);
          copyGraphicLink.Tag = graphicLink.Tag;
          copyGraphicLink.ClassID = graphicLink.ClassID;
          copyGraphicLink.Origin = graphicLink.Origin;
          copyGraphicLink.Destination = graphicLink.Destination;

          foreach (PointF point in graphicLink.ControlPoints)
          {
            copyGraphicLink.ControlPoints.Add(point);
          }

          copyClientProtocol.graphicLinks.Add(copyGraphicLink.Guid, copyGraphicLink);
        }
      }

      return copyClientProtocol;
    }

    #endregion

  }
}
