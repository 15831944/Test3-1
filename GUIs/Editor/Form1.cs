using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

using SysCAD.Interface;
using MindFusion.FlowChartX;
using ActiproSoftware.UIStudio.Bar;
using MindFusion.FlowChartX.Commands;
using System.Collections;
using System.Runtime.Remoting.Channels;
using System.Runtime.Remoting.Channels.Tcp;

namespace SysCAD.Editor
{
  public partial class Form1 : Form
  {
    FrmFlowChart frmFlowChart;

    public Form1()
    {
      InitializeComponent();
      SetProjectBasedButtons(false);
      
      IDictionary tcpProps = new Hashtable();
      tcpProps["port"] = "0";
      TcpChannel tcpChannel = new TcpChannel(tcpProps, null, null);
      ChannelServices.RegisterChannel(tcpChannel, false);

      try
      {
        dockManager1.LoadToolWindowLayoutFromFile("Recent.layout");
      }
      catch (Exception) { }

      (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).SelectedIndexChanged += new EventHandler(NewItem_ModelType_Changed);
      (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).SelectedIndexChanged += new EventHandler(NewItem_GraphicType_Changed);
    }

    void NewItem_GraphicType_Changed(object sender, EventArgs e)
    {
      int stencilIndex = (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).SelectedIndex;

      if (stencilIndex != -1)
      {
        string stencilName = (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).Items[stencilIndex] as string;
        if (stencilName != "-------")
        {
          frmFlowChart.currentGraphicShape = stencilName;
        }
      }
    }

    void NewItem_ModelType_Changed(object sender, EventArgs e)
    {
      string groupName = "";

      bool rememberEnabled = barManager1.Commands["NewItem.GraphicType"].Enabled;
      barManager1.Commands["NewItem.GraphicType"].Enabled = false;

      int stencilIndex = (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).SelectedIndex;
      string stencilName = (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).Items[stencilIndex] as string;
      frmFlowChart.currentModelShape = stencilName;

      ModelStencil modelStencil = frmFlowChart.state.ModelStencil(stencilName);
      if (modelStencil != null)
      {
        groupName = modelStencil.groupName;
        barManager1.Commands["NewItem.GraphicType"].Enabled = rememberEnabled;
      }
      else
      {
        return;
      }

      GraphicType_Populate(groupName);
    }

    private void GraphicType_Populate(string groupName)
    {
      (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).Items.Clear();
      GlobalShapes.list.Clear();

      foreach (GraphicStencil graphicStencil in frmFlowChart.state.GraphicStencils)
      {
        if (groupName == graphicStencil.groupName)
        {
          int i = (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).Items.Add(graphicStencil.Tag);
          GlobalShapes.list.Add(graphicStencil.Tag);
        }
      }

      (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).Items.Add("-------");
      GlobalShapes.list.Add("-------");

      foreach (GraphicStencil graphicStencil in frmFlowChart.state.GraphicStencils)
      {
        if (groupName != graphicStencil.groupName)
        {
          (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).Items.Add(graphicStencil.Tag);
          GlobalShapes.list.Add(graphicStencil.Tag);
        }
      }
      (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).SelectedIndex = 0;
      (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).Text = groupName;
    }

    private void barManager1_CommandClick(object sender, BarCommandLinkEventArgs e)
    {
      switch (e.Command.FullName)
      {
        case "File.Exit":
          this.Close();
          break;

        case "File.Open":
          this.File_OpenProject();
          break;

        case "File.Close":
          this.File_CloseProject();
          break;

        case "View.ZoomIn":
          this.View_ZoomIn();
          break;

        case "View.ZoomOut":
          this.View_ZoomOut();
          break;

        case "View.ZoomToSelected":
          this.View_ZoomToSelected();
          break;

        case "View.ZoomToVisible":
          this.View_ZoomToVisible();
          break;

        case "View.ShowModels":
          this.View_ShowModels();
          break;

        case "View.ShowGraphics":
          this.View_ShowGraphics();
          break;

        case "View.ShowLinks":
          this.View_ShowLinks();
          break;

        case "View.ShowTags":
          this.View_ShowTags();
          break;

        case "Selection.SelectItems":
          this.View_SelectItems();
          break;

        case "Selection.SelectLinks":
          this.View_SelectArrows();
          break;

        case "Mode.Modify":
          this.Mode_Modify();
          break;

        case "Mode.CreateNode":
          this.Mode_CreateNode();
          break;

        case "Mode.CreateLink":
          this.Mode_CreateLink();
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

    public Dictionary<string, GraphicLink> cbGraphicLinks;
    public Dictionary<string, GraphicItem> cbGraphicItems;

    //private void Edit_Paste()
    //{
    //  foreach (GraphicItem graphicItem in cbGraphicItems.Values)
    //  {
    //    graphicItem.Tag += ".";
    //    graphicItem.X += 10.0F;
    //    graphicItem.Y += 10.0F;
    //    frmFlowChart.state.newItem(graphicItem, true, frmFlowChart.fcFlowChart);
    //    tvNavigation.Nodes.Add(graphicItem.Tag, graphicItem.Tag);
    //    tvNavigation.AddSelectedNode(tvNavigation.GetNodeByKey(graphicItem.Tag));
    //  }

    //  foreach (GraphicLink graphicLink in cbGraphicLinks.Values)
    //  {
    //    graphicLink.Tag += ".";
    //    graphicLink.Source += ".";
    //    graphicLink.Destination += ".";
    //    Arrow arrow = frmFlowChart.fcFlowChart.CreateArrow(new PointF(0.0F, 0.0F), new PointF(10.0F, 10.0F));
    //    frmFlowChart.state.newGraphicLink(graphicLink, arrow, true);
    //  }
    //}

    //private void Edit_Cut()
    //{
    //  throw new Exception("The method or operation is not implemented.");
    //}

    //private void Edit_Copy()
    //{
    //  cbGraphicItems = new Dictionary<string, GraphicItem>();
    //  cbGraphicLinks = new Dictionary<string, GraphicLink>();

    //  //frmFlowChart.fcFlowChart.CopyToClipboard(true);

    //  foreach (ChartObject chartObject in frmFlowChart.fcFlowChart.Selection.Objects)
    //  {
    //    if (chartObject is Box)
    //    {
    //      Box box = chartObject as Box;
    //      GraphicItem graphicItem = frmFlowChart.state.GraphicItem(box.Text);
    //      if (graphicItem != null)
    //      {
    //        GraphicItem cbGraphicItem = new GraphicItem(box.Text);
    //        cbGraphicItem.X = graphicItem.X + 10.0F;
    //        cbGraphicItem.Y = graphicItem.Y + 10.0F;
    //        cbGraphicItem.Width = graphicItem.Width;
    //        cbGraphicItem.Height = graphicItem.Height;
    //        cbGraphicItem.Angle = graphicItem.Angle;
    //        cbGraphicItem.Model = graphicItem.Model;
    //        cbGraphicItem.Shape = graphicItem.Shape;
    //        cbGraphicItem.MirrorX = graphicItem.MirrorX;
    //        cbGraphicItem.MirrorY = graphicItem.MirrorY;
    //        cbGraphicItem.fillColor = graphicItem.fillColor;

    //        cbGraphicItems.Add(box.Text, cbGraphicItem);
    //      }
    //    }

    //    if (chartObject is Arrow)
    //    {
    //      Arrow arrow = chartObject as Arrow;
    //      GraphicLink graphicLink = frmFlowChart.state.GraphicLink(arrow.Text);
    //      if (graphicLink != null)
    //      {
    //        GraphicLink cbLink = new GraphicLink(arrow.Text);
    //        cbLink.Tag = graphicLink.Tag;
    //        cbLink.ClassID = graphicLink.ClassID;
    //        cbLink.Source = graphicLink.Source;
    //        cbLink.Destination = graphicLink.Destination;

    //        cbGraphicLinks.Add(arrow.Text, cbLink);
    //      }
    //    }

    //  }
    //}

    #region clipboard support

    public void CopyToClipboard()
    {
      // create clones of selected items
      BaseGraphic data = copySelection(frmFlowChart.fcFlowChart);

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
      CompositeCmd composite = frmFlowChart.fcFlowChart.UndoManager.StartComposite("_Kenwalt.SysCAD_");

      // delete selected items
      ChartObjectCollection temp = new ChartObjectCollection();
      foreach (ChartObject item in frmFlowChart.fcFlowChart.Selection.Objects)
        temp.Add(item);

      frmFlowChart.fcFlowChart.Selection.Clear();

      foreach (ChartObject item in temp)
        frmFlowChart.fcFlowChart.DeleteObject(item);

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
        if (dataObj != null && dataObj.GetDataPresent("Kenwalt.GraphicData"))
        {
          BaseGraphic pasteData = dataObj.GetData("Kenwalt.GraphicData") as BaseGraphic;

          Dictionary<string, string> tagConversion = new Dictionary<string,string>();

          if (pasteData != null)
          {
            foreach (GraphicItem graphicItem in pasteData.graphicItems.Values)
            {
              graphicItem.X += 10.0F;
              graphicItem.Y += 10.0F;
              GraphicItem newGraphicItem = frmFlowChart.NewGraphicItem(graphicItem, tvNavigation.SelectedNode.Text);
              tagConversion.Add(graphicItem.Tag, newGraphicItem.Tag);
            }

            foreach (GraphicLink graphicLink in pasteData.graphicLinks.Values)
            {
              GraphicLink newGraphicLink = new GraphicLink(graphicLink.Tag);

              newGraphicLink.Destination = graphicLink.Destination;
              newGraphicLink.Source = graphicLink.Source;

              foreach (PointF point in graphicLink.controlPoints)
              {
                newGraphicLink.controlPoints.Add(new PointF(point.X, point.Y));
              }

              // use new tags for connected items.
              //if (tagConversion.ContainsKey(newGraphicLink.Source))
              //  newGraphicLink.Source = tagConversion[newGraphicLink.Source];
              //else
              //  newGraphicLink.Source = "";
              //if (tagConversion.ContainsKey(newGraphicLink.Destination))
              //  newGraphicLink.Destination = tagConversion[newGraphicLink.Destination];
              //else
              //  newGraphicLink.Destination = "";

              frmFlowChart.NewGraphicLink(newGraphicLink, null, dx, dy);
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
      catch
      {
      }
    }

    private BaseGraphic copySelection(FlowChart doc)
    {
      if (doc.Selection.Objects.Count == 0)
        return null;

      BaseGraphic copyGraphic = new BaseGraphic();

      foreach (Box box in doc.Selection.Boxes)
      {
        GraphicItem graphicItem = frmFlowChart.state.GraphicItem(box);
        if (graphicItem != null)
        {
          GraphicItem copyGraphicItem = new GraphicItem(box.Text);
          copyGraphicItem.X = graphicItem.X;
          copyGraphicItem.Y = graphicItem.Y;
          copyGraphicItem.Width = graphicItem.Width;
          copyGraphicItem.Height = graphicItem.Height;
          copyGraphicItem.Angle = graphicItem.Angle;
          copyGraphicItem.Model = graphicItem.Model;
          copyGraphicItem.Shape = graphicItem.Shape;
          copyGraphicItem.MirrorX = graphicItem.MirrorX;
          copyGraphicItem.MirrorY = graphicItem.MirrorY;
          copyGraphicItem.fillColor = graphicItem.fillColor;

          copyGraphic.graphicItems.Add(copyGraphicItem.Guid, copyGraphicItem);
        }
      }

      foreach (Arrow arrow in doc.Selection.Arrows)
      {
        GraphicLink graphicLink = frmFlowChart.state.GraphicLink((arrow.Tag as Link).Guid);
        if (graphicLink != null)
        {
          GraphicLink copyGraphicLink = new GraphicLink(arrow.Text);
          copyGraphicLink.Tag = graphicLink.Tag;
          copyGraphicLink.ClassID = graphicLink.ClassID;
          copyGraphicLink.Source = graphicLink.Source;
          copyGraphicLink.Destination = graphicLink.Destination;

          foreach (PointF point in graphicLink.controlPoints)
          {
            copyGraphicLink.controlPoints.Add(point);
          }

          copyGraphic.graphicLinks.Add(copyGraphicLink.Guid, copyGraphicLink);
        }
      }

      return copyGraphic;
    }

    #endregion

    private void Mode_CreateLink()
    {
      barManager1.Commands["NewItem.GraphicType"].Enabled = false;
      barManager1.Commands["NewItem.ModelType"].Enabled = false;
      frmFlowChart.fcFlowChart.Behavior = BehaviorType.CreateArrow;
    }

    private void Mode_CreateNode()
    {
      barManager1.Commands["NewItem.GraphicType"].Enabled = true;
      barManager1.Commands["NewItem.ModelType"].Enabled = true;
      //frmFlowChart.fcFlowChart.Behavior = BehaviorType.CreateBox;
    }

    public void Mode_Modify()
    {
      barManager1.Commands["NewItem.GraphicType"].Enabled = false;
      barManager1.Commands["NewItem.ModelType"].Enabled = false;
      frmFlowChart.fcFlowChart.Behavior = BehaviorType.Modify;
      (barManager1.Commands["Mode.Modify"] as BarButtonCommand).Checked = true;
      (barManager1.Commands["Mode.CreateNode"] as BarButtonCommand).Checked = false;
      (barManager1.Commands["Mode.CreateLink"] as BarButtonCommand).Checked = false;
    }

    private void View_SelectItems()
    {
      frmFlowChart.state.SelectItems = true;
      ((IBarCheckableCommand)barManager1.Commands["Selection.SelectItems"]).Checked = true;
      
      frmFlowChart.state.SelectLinks = false;
      ((IBarCheckableCommand)barManager1.Commands["Selection.SelectLinks"]).Checked = false;

      frmFlowChart_fcFlowChart_SelectionChanged();
    }

    private void View_SelectArrows()
    {
      frmFlowChart.state.SelectLinks = true;
      ((IBarCheckableCommand)barManager1.Commands["Selection.SelectLinks"]).Checked = true;

      frmFlowChart.state.SelectItems = false;
      ((IBarCheckableCommand)barManager1.Commands["Selection.SelectItems"]).Checked = false;

      frmFlowChart_fcFlowChart_SelectionChanged();
    }

    private void View_ShowModels()
    {
      frmFlowChart.state.ShowModels = ((IBarCheckableCommand)barManager1.Commands["View.ShowModels"]).Checked;

      foreach (Item item in frmFlowChart.state.Items)
      {
        item.Model.Visible = item.Visible && frmFlowChart.state.ShowModels;
      }
    }

    private void View_ShowGraphics()
    {
      frmFlowChart.state.ShowGraphics = ((IBarCheckableCommand)barManager1.Commands["View.ShowGraphics"]).Checked;

      foreach (Item item in frmFlowChart.state.Items)
      {
        item.Graphic.Visible = item.Visible && frmFlowChart.state.ShowGraphics;
      }
    }

    private void View_ShowLinks()
    {
      frmFlowChart.state.ShowLinks = ((IBarCheckableCommand)barManager1.Commands["View.ShowLinks"]).Checked;

      foreach (Arrow arrow in frmFlowChart.fcFlowChart.Arrows)
      {
        bool visible = true;
        Item origin = arrow.Origin.Tag as Item;
        Item destination = arrow.Destination.Tag as Item;

        // only set to false if the endpoint exists and is invisible.  disconnected arrows must be visible.
        if (origin != null) visible = visible && origin.Visible;
        if (destination != null) visible = visible && destination.Visible;

        arrow.Visible = visible && frmFlowChart.state.ShowLinks;
      }
    }

    private void View_ShowTags()
    {
      frmFlowChart.state.ShowTags = ((IBarCheckableCommand)barManager1.Commands["View.ShowTags"]).Checked;

      foreach (Link link in frmFlowChart.state.Links)
      {
        if (frmFlowChart.state.ShowTags)
          link.Arrow.Text = link.Tag;
        else
          link.Arrow.Text = "";
      }

      foreach (Item item in frmFlowChart.state.Items)
      {
          item.Text.Visible = frmFlowChart.state.ShowTags;
      }

      frmFlowChart.fcFlowChart.Invalidate();
    }

    private void View_ZoomToVisible()
    {
      frmFlowChart.ZoomToVisible();
    }

    private void View_ZoomToSelected()
    {
      frmFlowChart.ZoomToSelected();
    }

    private void View_ZoomOut()
    {
      frmFlowChart.FixDocExtents();

      frmFlowChart.fcFlowChart.ZoomOut();
      foreach (Link link in frmFlowChart.state.Links)
      {
        link.Arrow.ArrowHeadSize = 200.0F / frmFlowChart.fcFlowChart.ZoomFactor;
        link.Arrow.IntermHeadSize = 133.0F / frmFlowChart.fcFlowChart.ZoomFactor;
      }
    }

    private void View_ZoomIn()
    {
      frmFlowChart.FixDocExtents();

      frmFlowChart.fcFlowChart.ZoomIn();
      foreach (Link link in frmFlowChart.state.Links)
      {
        link.Arrow.ArrowHeadSize = 200.0F / frmFlowChart.fcFlowChart.ZoomFactor;
        link.Arrow.IntermHeadSize = 133.0F / frmFlowChart.fcFlowChart.ZoomFactor;
      }
    }

    private void SetProjectBasedButtons(bool projectExists)
    {
      barManager1.Commands["File.PrintPreview"].Enabled = projectExists;
      barManager1.Commands["File.Print"].Enabled = projectExists;
      barManager1.Commands["File.Close"].Enabled = projectExists;
      barManager1.Commands["View.ZoomIn"].Enabled = projectExists;
      barManager1.Commands["View.ZoomOut"].Enabled = projectExists;
      barManager1.Commands["View.ZoomToVisible"].Enabled = projectExists;
      barManager1.Commands["View.ZoomToSelected"].Enabled = projectExists;
      barManager1.Commands["View.ShowModels"].Enabled = projectExists;
      barManager1.Commands["View.ShowGraphics"].Enabled = projectExists;
      barManager1.Commands["View.ShowLinks"].Enabled = projectExists;
      barManager1.Commands["View.ShowTags"].Enabled = projectExists;
      barManager1.Commands["Selection.SelectItems"].Enabled = projectExists;
      barManager1.Commands["Selection.SelectLinks"].Enabled = projectExists;
      barManager1.Commands["NewItem.ModelType"].Enabled = projectExists;
      barManager1.Commands["Mode.Modify"].Enabled = projectExists;
      barManager1.Commands["Mode.CreateNode"].Enabled = projectExists;
      barManager1.Commands["Mode.CreateLink"].Enabled = projectExists;
      barManager1.Commands["Edit.Cut"].Enabled = projectExists;
      barManager1.Commands["Edit.Copy"].Enabled = projectExists;
      barManager1.Commands["Edit.Paste"].Enabled = projectExists;

      barManager1.Commands["NewItem.GraphicType"].Enabled = false;
      barManager1.Commands["NewItem.ModelType"].Enabled = false;

      if (frmFlowChart != null)
      {
        frmFlowChart.state.SelectItems = true;
        frmFlowChart.state.SelectLinks = false;
      }

      ((IBarCheckableCommand)barManager1.Commands["Selection.SelectItems"]).Checked = true;
      ((IBarCheckableCommand)barManager1.Commands["Selection.SelectLinks"]).Checked = false;
    }

    private void File_CloseProject()
    {
      SetProjectBasedButtons(false);

      frmFlowChart.Close();
      tvNavigation.Nodes.Clear();
      ovOverview.Document = null;
    }

    private void File_OpenProject()
    {
      // Close the one selected first.
      if (frmFlowChart != null)
        File_CloseProject();

      OpenProjectForm openProjectForm = new OpenProjectForm();

      if (openProjectForm.ShowDialog() == DialogResult.OK)
      {
        openProjectForm.Close();
        Refresh();

        frmFlowChart = new FrmFlowChart(this);

        frmFlowChart.SuspendLayout();
        SuspendLayout();

        frmFlowChart.WindowState = System.Windows.Forms.FormWindowState.Maximized;

        frmFlowChart.MdiParent = this;
        frmFlowChart.Text = openProjectForm.graphic.Name;

        frmFlowChart.SetProject(openProjectForm.graphic, openProjectForm.config, tvNavigation);
        tvNavigation_SetProject();
        ovOverview.Document = frmFlowChart.fcFlowChart;

        frmFlowChart.fcFlowChart.SelectionChanged += new SelectionEvent(this.frmFlowChart_fcFlowChart_SelectionChanged);

        (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).Items.Clear();
        foreach (ModelStencil modelStencil in frmFlowChart.state.ModelStencils)
        {
          (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).Items.Add(modelStencil.Tag);
        }
        (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).SelectedIndex = 3;

        SetProjectBasedButtons(true);

        frmFlowChart.Show();

        ResumeLayout(true);
        frmFlowChart.ResumeLayout(true);

        frmFlowChart.ZoomToVisible();
      }
    }

    private void tvNavigation_SetProject()
    {
      foreach (GraphicItem graphicItem in frmFlowChart.state.GraphicItems)
      {
        PureComponents.TreeView.Node node = 
          tvNavigation.AddNodeByPath(graphicItem.Path + graphicItem.Tag, graphicItem.Guid.ToString());
      }

      foreach (PureComponents.TreeView.Node node in tvNavigation.Nodes)
      {
        node.Select();
        node.Expand();
      }
    }

    private void Form1_FormClosing(object sender, FormClosingEventArgs e)
    {
      if (frmFlowChart != null)
        File_CloseProject();

      //dockManager1.SaveToolWindowLayoutToFile("Recent.layout");
    }

    static List<PureComponents.TreeView.Node> wasSelectedNodes = new List<PureComponents.TreeView.Node>();

    private void tvNavigation_NodeSelectionChange(object sender, EventArgs e)
    {
      tvNavigation.NodeSelectionChange -= new System.EventHandler(this.tvNavigation_NodeSelectionChange);

      if (tvNavigation.SelectedNodes.Length > 0)
      {
        foreach (PureComponents.TreeView.Node node in tvNavigation.SelectedNodes)
        {
          if (node.Key == null) // not an item.
            SelectSubNodes(node);
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
          if (node.Key != null)
            frmFlowChart.state.ItemVisible(new Guid(node.Key), false);
        }

        wasSelectedNodes.Clear();
      }

      tvNavigation.NodeSelectionChange += new System.EventHandler(this.tvNavigation_NodeSelectionChange);
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
          if (innerNode.Key != null)
            frmFlowChart.state.ItemVisible(new Guid(innerNode.Key), innerNode.IsSelected);

          SelectSubNodes(innerNode);
        }
      }
      else if (wasSelected && !isSelected)
      {
        // Been deselected, deselect all underlings and update flowchart.
        foreach (PureComponents.TreeView.Node innerNode in node.Nodes)
        {
          if (innerNode.Key != null)
            frmFlowChart.state.ItemVisible(new Guid(innerNode.Key), false);

          tvNavigation.RemoveSelectedNode(innerNode);
          SelectSubNodes(innerNode);
        }
      }
      else if (!wasSelected && isSelected)
      {
        // Been selected, select all underlings and update flowchart.
        foreach (PureComponents.TreeView.Node innerNode in node.Nodes)
        {
          if (innerNode.Key != null)
            frmFlowChart.state.ItemVisible(new Guid(innerNode.Key), true);

          tvNavigation.AddSelectedNode(innerNode);
          SelectSubNodes(innerNode);
        }
      }
    }

    private void frmFlowChart_fcFlowChart_SelectionChanged(object sender, EventArgs e)
    {
      frmFlowChart_fcFlowChart_SelectionChanged();
    }

    private void frmFlowChart_fcFlowChart_SelectionChanged()
    {
      frmFlowChart.fcFlowChart.SelectionChanged -= new SelectionEvent(this.frmFlowChart_fcFlowChart_SelectionChanged);

      if (!frmFlowChart.state.SelectItems)
      {
        if (frmFlowChart.fcFlowChart.Selection.Boxes.Count > 1)
        {
          BoxCollection boxCollection = frmFlowChart.fcFlowChart.Selection.Boxes.Clone();
          foreach (Box box in boxCollection)
          {
            box.Selected = false;
          }
        }
      }

      if (!frmFlowChart.state.SelectLinks)
      {
        if (frmFlowChart.fcFlowChart.Selection.Arrows.Count > 1)
        {
          ArrowCollection arrowCollection = frmFlowChart.fcFlowChart.Selection.Arrows.Clone();
          foreach (Arrow arrow in arrowCollection)
          {
            arrow.Selected = false;
          }
        }
      }

      foreach (Item item in frmFlowChart.state.Items)
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
          item.Model.ZIndex = item.Graphic.ZIndex + 1;
          item.Text.Visible = item.Visible && frmFlowChart.state.ShowTags;
          item.Text.ZIndex = item.Graphic.ZIndex + 2;
        }
        else
        {
          item.Model.Visible = item.Visible && frmFlowChart.state.ShowModels;
        }
      }

      if (frmFlowChart.fcFlowChart.ActiveObject is Arrow)
      {
        Arrow activeArrow = frmFlowChart.fcFlowChart.ActiveObject as Arrow;
        GraphicLink graphicLink = frmFlowChart.state.GraphicLink(activeArrow);
        if (graphicLink != null)
        {
          propertyGrid1.SelectedObject = graphicLink;
        }
      }

      if (frmFlowChart.fcFlowChart.ActiveObject is Box)
      {
        Box activeBox = frmFlowChart.fcFlowChart.ActiveObject as Box;
        GraphicItem graphicItem = frmFlowChart.state.GraphicItem(activeBox);
        if (graphicItem != null)
        {
          propertyGrid1.SelectedObject = graphicItem;
          propertyGrid1.PropertyValueChanged += new PropertyValueChangedEventHandler(propertyGrid1_PropertyValueChanged);

          int i;

          i = 0;
          foreach (string model in (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).Items)
          {
            if (model == graphicItem.Model)
            {
              (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).SelectedIndex = i;
              (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).Text = model;
            }
            i++;
          }
          if ((barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).SelectedIndex == -1)
            (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).SelectedIndex = 0;

          ModelStencil modelStencil = frmFlowChart.state.ModelStencil(graphicItem.Model);
          if (modelStencil != null)
          {
            GraphicType_Populate(modelStencil.groupName);
          }

          i = 0;
          foreach (string shape in (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).Items)
          {
            if (shape == graphicItem.Shape)
            {
              (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).SelectedIndex = i;
              (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).Text = shape;
            }
            i++;
          }
          if ((barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).SelectedIndex == -1)
            (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).SelectedIndex = 0;
        }
      }

      frmFlowChart.fcFlowChart.SelectionChanged += new SelectionEvent(this.frmFlowChart_fcFlowChart_SelectionChanged);
    }

    void propertyGrid1_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
    {
      string label = e.ChangedItem.Label;
      if (label == "Stencil")
      {
        string graphicString = (e.ChangedItem.Value as String);
        GraphicStencil graphicShape = frmFlowChart.state.GraphicStencil(graphicString);
        if (graphicShape != null)
        {
          GraphicItem graphicItem = (e.ChangedItem.Parent.Parent.Value as GraphicItem);

          graphicItem.Shape = graphicString;
          frmFlowChart.state.SetStencil(graphicItem.Guid, graphicShape.ShapeTemplate(graphicItem.MirrorX, graphicItem.MirrorY));
        }
      }

      if (label == "Angle")
      {
        float angle = (float)e.ChangedItem.Value;
        GraphicItem graphicItem = (e.ChangedItem.Parent.Parent.Value as GraphicItem);

        graphicItem.Angle = angle;
        frmFlowChart.state.SetAngle(graphicItem.Guid, angle);
      }

      if (label == "Height")
      {
        float height = (float)e.ChangedItem.Value;
        GraphicItem graphicItem = (e.ChangedItem.Parent.Parent.Value as GraphicItem);

        graphicItem.Height = height;
        frmFlowChart.state.SetHeight(graphicItem.Guid, height);
      }

      if (label == "Width")
      {
        float width = (float)e.ChangedItem.Value;
        GraphicItem graphicItem = (e.ChangedItem.Parent.Parent.Value as GraphicItem);

        graphicItem.Width = width;
        frmFlowChart.state.SetWidth(graphicItem.Guid, width);
      }

      if (label == "X")
      {
        float x = (float)e.ChangedItem.Value;
        GraphicItem graphicItem = (e.ChangedItem.Parent.Parent.Value as GraphicItem);

        graphicItem.X = x;
        frmFlowChart.state.SetX(graphicItem.Guid, x);
      }

      if (label == "Y")
      {
        float y = (float)e.ChangedItem.Value;
        GraphicItem graphicItem = (e.ChangedItem.Parent.Parent.Value as GraphicItem);

        graphicItem.Y = y;
        frmFlowChart.state.SetY(graphicItem.Guid, y);
      }

      if (label == "Mirror X")
      {
        bool mirrorX = (bool)e.ChangedItem.Value;
        GraphicItem graphicItem = (e.ChangedItem.Parent.Parent.Value as GraphicItem);

        graphicItem.MirrorX = mirrorX;
        frmFlowChart.state.SetMirrorX(graphicItem.Guid, mirrorX);
      }

      if (label == "Mirror Y")
      {
        bool mirrorY = (bool)e.ChangedItem.Value;
        GraphicItem graphicItem = (e.ChangedItem.Parent.Parent.Value as GraphicItem);

        graphicItem.MirrorY = mirrorY;
        frmFlowChart.state.SetMirrorY(graphicItem.Guid, mirrorY);
      }
    }

    private void tvNavigation_NodeMouseClick(EventArgs e, PureComponents.TreeView.Node oNode)
    {
      MouseEventArgs me = e as MouseEventArgs;
      Keys keys = Control.ModifierKeys;
      
      // Select only this area/item if navigation node clicked.
      if (keys == Keys.None)
      {
        tvNavigation.ClearNodeSelection();
        oNode.Select();
      }
    }
  }
}