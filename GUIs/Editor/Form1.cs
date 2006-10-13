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

      (barManager1.Commands["CreateItem.ModelType"] as BarComboBoxCommand).SelectedIndexChanged += new EventHandler(NewItem_ModelType_Changed);
      (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).SelectedIndexChanged += new EventHandler(NewItem_GraphicType_Changed);
    }

    void NewItem_GraphicType_Changed(object sender, EventArgs e)
    {
      int stencilIndex = (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).SelectedIndex;

      if (stencilIndex != -1)
      {
        string stencilName = (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).Items[stencilIndex] as string;
        if (stencilName != "-------")
        {
          frmFlowChart.currentStencil = stencilName;
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
      frmFlowChart.currentModel = stencilName;

      ModelStencil modelStencil = frmFlowChart.state.ModelShape(stencilName);
      if (modelStencil != null)
      {
        groupName = modelStencil.groupName;
        barManager1.Commands["CreateItem.GraphicType"].Enabled = rememberEnabled;
      }
      else
      {
        return;
      }

      GraphicType_Populate(groupName);
    }

    private void GraphicType_Populate(string groupName)
    {
      (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).Items.Clear();
      ShapeConverter.stencilList.Clear();

      foreach (GraphicStencil graphicStencil in frmFlowChart.state.GraphicStencils)
      {
        if (groupName == graphicStencil.groupName)
        {
          int i = (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).Items.Add(graphicStencil.Tag);
          ShapeConverter.stencilList.Add(graphicStencil.Tag);
        }
      }

      (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).Items.Add("-------");
      ShapeConverter.stencilList.Add("-------");

      foreach (GraphicStencil graphicStencil in frmFlowChart.state.GraphicStencils)
      {
        if (groupName != graphicStencil.groupName)
        {
          (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).Items.Add(graphicStencil.Tag);
          ShapeConverter.stencilList.Add(graphicStencil.Tag);
        }
      }
      (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).SelectedIndex = 0;
      (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).Text = groupName;
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
      frmFlowChart.state.Remove(frmFlowChart.fcFlowChart);

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


          if (pasteData != null)
          {
            foreach (GraphicItem graphicItem in pasteData.graphicItems.Values)
            {
              graphicItem.X += dx;
              graphicItem.Y += dy;
              frmFlowChart.NewGraphicItem(graphicItem, tvNavigation.SelectedNode.FullPath);
            }

            foreach (GraphicLink graphicLink in pasteData.graphicLinks.Values)
            {
              GraphicLink newGraphicLink = new GraphicLink(graphicLink.Tag);

              newGraphicLink.Destination = graphicLink.Destination;
              newGraphicLink.Origin = graphicLink.Origin;

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

              frmFlowChart.NewGraphicLink(newGraphicLink, dx, dy);
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
          copyGraphicItem.FillColor = graphicItem.FillColor;

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
          copyGraphicLink.Origin = graphicLink.Origin;
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
      barManager1.Commands["CreateItem.GraphicType"].Enabled = false;
      barManager1.Commands["CreateItem.ModelType"].Enabled = false;
      frmFlowChart.fcFlowChart.Behavior = BehaviorType.CreateArrow;
    }

    private void Mode_CreateNode()
    {
      barManager1.Commands["CreateItem.GraphicType"].Enabled = true;
      barManager1.Commands["CreateItem.ModelType"].Enabled = true;
      //frmFlowChart.fcFlowChart.Behavior = BehaviorType.CreateBox;
    }

    public void Mode_Modify()
    {
      barManager1.Commands["CreateItem.GraphicType"].Enabled = false;
      barManager1.Commands["CreateItem.ModelType"].Enabled = false;
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
        item.Model.ZIndex = item.Graphic.ZIndex + 1;
        item.Text.ZIndex = item.Model.ZIndex + 1;
      }
    }

    private void View_ShowGraphics()
    {
      frmFlowChart.state.ShowGraphics = ((IBarCheckableCommand)barManager1.Commands["View.ShowGraphics"]).Checked;

      foreach (Item item in frmFlowChart.state.Items)
      {
        item.Graphic.Visible = item.Visible && frmFlowChart.state.ShowGraphics;
        item.Model.ZIndex = item.Graphic.ZIndex + 1;
        item.Text.ZIndex = item.Model.ZIndex + 1;
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

        // only set to false if the endpoint exists and is invisible.
        // disconnected arrows must be visible, because otherwise they'll never appear.
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

      frmFlowChart.setSizes();
    }

    private void View_ZoomIn()
    {
      frmFlowChart.FixDocExtents();

      frmFlowChart.fcFlowChart.ZoomIn();

      frmFlowChart.setSizes();
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
      barManager1.Commands["CreateItem.ModelType"].Enabled = projectExists;
      barManager1.Commands["Mode.Modify"].Enabled = projectExists;
      barManager1.Commands["Mode.CreateNode"].Enabled = projectExists;
      barManager1.Commands["Mode.CreateLink"].Enabled = projectExists;
      barManager1.Commands["Edit.Cut"].Enabled = projectExists;
      barManager1.Commands["Edit.Copy"].Enabled = projectExists;
      barManager1.Commands["Edit.Paste"].Enabled = projectExists;

      barManager1.Commands["CreateItem.GraphicType"].Enabled = false;
      barManager1.Commands["CreateItem.ModelType"].Enabled = false;

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

        (barManager1.Commands["CreateItem.ModelType"] as BarComboBoxCommand).Items.Clear();
        foreach (ModelStencil modelStencil in frmFlowChart.state.ModelStencils)
        {
          (barManager1.Commands["CreateItem.ModelType"] as BarComboBoxCommand).Items.Add(modelStencil.Tag);
        }
        (barManager1.Commands["CreateItem.ModelType"] as BarComboBoxCommand).SelectedIndex = 3;

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
        string path = graphicItem.Path;
        string tag = graphicItem.Tag;
        string pathTag = graphicItem.Path + graphicItem.Tag;

        Guid guid = graphicItem.Guid;
        string guidString = graphicItem.Guid.ToString();

        PureComponents.TreeView.Node node =
          tvNavigation.AddNodeByPath(pathTag, guidString);
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

          ModelItem modelItem = new ModelItem(graphicItem.Guid);
          modelItem.Properties = new BaseAccessItem[6];
          modelItem.Properties[0] = new FloatAccessItem("zero", "description zero", 074.6F);
          modelItem.Properties[1] = new FloatAccessItem("one", "description one", 174.6F);
          modelItem.Properties[2] = new FloatAccessItem("two", "description two", 274.6F);
          modelItem.Properties[3] = new FloatAccessItem("three", "description three", 374.6F);
          modelItem.Properties[4] = new FloatAccessItem("four", "description four", 474.6F);

          BaseAccessItem[] properties = new BaseAccessItem[5];
          properties[0] = new FloatAccessItem("zero branch", "description zero branch", 074.6F);
          properties[1] = new FloatAccessItem("one branch", "description one branch", 174.6F);
          properties[2] = new FloatAccessItem("two branch", "description two branch", 274.6F);
          properties[3] = new FloatAccessItem("three branch", "description three branch", 374.6F);
          properties[4] = new FloatAccessItem("four branch", "description four branch", 474.6F);

          modelItem.Properties[5] = new BranchAccessItem("branch zero", "description branch zero", properties);

          propertyGrid2.SelectedObject = modelItem;

          int i;

          i = 0;
          foreach (string model in (barManager1.Commands["CreateItem.ModelType"] as BarComboBoxCommand).Items)
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

          ModelStencil modelStencil = frmFlowChart.state.ModelShape(graphicItem.Model);
          if (modelStencil != null)
          {
            GraphicType_Populate(modelStencil.groupName);
          }

          i = 0;
          foreach (string shape in (barManager1.Commands["CreateItem.GraphicType"] as BarComboBoxCommand).Items)
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

      frmFlowChart.fcFlowChart.SelectionChanged += new SelectionEvent(this.frmFlowChart_fcFlowChart_SelectionChanged);
    }

    void propertyGrid1_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
    {
      uint requestID;
      string label = e.ChangedItem.Label;
      if (label == "Shape")
      {
        GridItem gridItem = e.ChangedItem;
        Shape shape = (gridItem.Value as Shape);
        GraphicStencil graphicShape = frmFlowChart.state.GraphicShape(shape);
        if (graphicShape != null)
        {
          while (!((gridItem.Value is GraphicItem) || (gridItem.Parent == null)))
            gridItem = gridItem.Parent;
          GraphicItem graphicItem = (gridItem.Value as GraphicItem);

          frmFlowChart.state.ModifyGraphicItem(out requestID, graphicItem.Guid, graphicItem.Tag, graphicItem.Path, graphicItem.Model, shape, graphicItem.BoundingRect, graphicItem.Angle, graphicItem.FillColor, graphicItem.MirrorX, graphicItem.MirrorY);
        }
      }

      if (label == "Angle")
      {
        GridItem gridItem = e.ChangedItem;
        float angle = (float)gridItem.Value;
        while (!((gridItem.Value is GraphicItem) || (gridItem.Parent == null)))
          gridItem = gridItem.Parent;
        GraphicItem graphicItem = (gridItem.Value as GraphicItem);

        frmFlowChart.state.ModifyGraphicItem(out requestID, graphicItem.Guid, graphicItem.Tag, graphicItem.Path, graphicItem.Model, graphicItem.Shape, graphicItem.BoundingRect, angle, graphicItem.FillColor, graphicItem.MirrorX, graphicItem.MirrorY);
      }

      if (label == "Height")
      {
        GridItem gridItem = e.ChangedItem;
        float height = (float)gridItem.Value;
        while (!((gridItem.Value is GraphicItem) || (gridItem.Parent == null)))
          gridItem = gridItem.Parent;
        GraphicItem graphicItem = (gridItem.Value as GraphicItem);

        RectangleF rectangleF = graphicItem.BoundingRect;
        rectangleF.Height = height;
        frmFlowChart.state.ModifyGraphicItem(out requestID, graphicItem.Guid, graphicItem.Tag, graphicItem.Path, graphicItem.Model, graphicItem.Shape, rectangleF, graphicItem.Angle, graphicItem.FillColor, graphicItem.MirrorX, graphicItem.MirrorY);
      }

      if (label == "Width")
      {
        GridItem gridItem = e.ChangedItem;
        float width = (float)gridItem.Value;
        while (!((gridItem.Value is GraphicItem) || (gridItem.Parent == null)))
          gridItem = gridItem.Parent;
        GraphicItem graphicItem = (gridItem.Value as GraphicItem);

        RectangleF rectangleF = graphicItem.BoundingRect;
        rectangleF.Width = width;
        frmFlowChart.state.ModifyGraphicItem(out requestID, graphicItem.Guid, graphicItem.Tag, graphicItem.Path, graphicItem.Model, graphicItem.Shape, rectangleF, graphicItem.Angle, graphicItem.FillColor, graphicItem.MirrorX, graphicItem.MirrorY);
      }

      if (label == "X")
      {
        GridItem gridItem = e.ChangedItem;
        float x = (float)gridItem.Value;
        while (!((gridItem.Value is GraphicItem) || (gridItem.Parent == null)))
          gridItem = gridItem.Parent;
        GraphicItem graphicItem = (gridItem.Value as GraphicItem);

        RectangleF rectangleF = graphicItem.BoundingRect;
        rectangleF.X = x;
        frmFlowChart.state.ModifyGraphicItem(out requestID, graphicItem.Guid, graphicItem.Tag, graphicItem.Path, graphicItem.Model, graphicItem.Shape, rectangleF, graphicItem.Angle, graphicItem.FillColor, graphicItem.MirrorX, graphicItem.MirrorY);
      }

      if (label == "Y")
      {
        GridItem gridItem = e.ChangedItem;
        float y = (float)gridItem.Value;
        while (!((gridItem.Value is GraphicItem) || (gridItem.Parent == null)))
          gridItem = gridItem.Parent;
        GraphicItem graphicItem = (gridItem.Value as GraphicItem);

        RectangleF rectangleF = graphicItem.BoundingRect;
        rectangleF.Y = y;
        frmFlowChart.state.ModifyGraphicItem(out requestID, graphicItem.Guid, graphicItem.Tag, graphicItem.Path, graphicItem.Model, graphicItem.Shape, rectangleF, graphicItem.Angle, graphicItem.FillColor, graphicItem.MirrorX, graphicItem.MirrorY);
      }

      if (label == "Mirror X")
      {
        GridItem gridItem = e.ChangedItem;
        bool mirrorX = (bool)gridItem.Value;
        while (!((gridItem.Value is GraphicItem) || (gridItem.Parent == null)))
          gridItem = gridItem.Parent;
        GraphicItem graphicItem = (gridItem.Value as GraphicItem);

        frmFlowChart.state.ModifyGraphicItem(out requestID, graphicItem.Guid, graphicItem.Tag, graphicItem.Path, graphicItem.Model, graphicItem.Shape, graphicItem.BoundingRect, graphicItem.Angle, graphicItem.FillColor, mirrorX, graphicItem.MirrorY);
      }

      if (label == "Mirror Y")
      {
        GridItem gridItem = e.ChangedItem;
        bool mirrorY = (bool)gridItem.Value;
        while (!((gridItem.Value is GraphicItem) || (gridItem.Parent == null)))
          gridItem = gridItem.Parent;
        GraphicItem graphicItem = (gridItem.Value as GraphicItem);

        frmFlowChart.state.ModifyGraphicItem(out requestID, graphicItem.Guid, graphicItem.Tag, graphicItem.Path, graphicItem.Model, graphicItem.Shape, graphicItem.BoundingRect, graphicItem.Angle, graphicItem.FillColor, graphicItem.MirrorX, mirrorY);
      }

      if (label == "Tag")
      {
        GridItem gridItem = e.ChangedItem;
        String tag = (String)gridItem.Value;
        while (!((gridItem.Value is GraphicItem) || (gridItem.Parent == null)))
          gridItem = gridItem.Parent;
        GraphicItem graphicItem = (gridItem.Value as GraphicItem);

        graphicItem.Tag = tag;
        frmFlowChart.state.SetTag(graphicItem.Guid, tag);
        frmFlowChart.state.ModifyGraphicItem(out requestID, graphicItem.Guid, graphicItem.Tag, graphicItem.Path, graphicItem.Model, graphicItem.Shape, graphicItem.BoundingRect, graphicItem.Angle, graphicItem.FillColor, graphicItem.MirrorX, graphicItem.MirrorY);
      }

      if (label == "Fill Color")
      {
        GridItem gridItem = e.ChangedItem;
        Color fillColor = (Color)gridItem.Value;
        while (!((gridItem.Value is GraphicItem) || (gridItem.Parent == null)))
          gridItem = gridItem.Parent;
        GraphicItem graphicItem = (gridItem.Value as GraphicItem);

        graphicItem.FillColor = fillColor;
        frmFlowChart.state.SetFillColor(graphicItem.Guid, fillColor);
        frmFlowChart.state.ModifyGraphicItem(out requestID, graphicItem.Guid, graphicItem.Tag, graphicItem.Path, graphicItem.Model, graphicItem.Shape, graphicItem.BoundingRect, graphicItem.Angle, graphicItem.FillColor, graphicItem.MirrorX, graphicItem.MirrorY);
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

    private void tvNavigation_AfterNodePositionChange(PureComponents.TreeView.Node oNode)
    {
      tvNavigation.ClearSelectedNodes();
      wasSelectedNodes.Clear();
    }
  }
}
