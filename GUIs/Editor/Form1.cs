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

namespace SysCAD.Editor
{
  public partial class Form1 : Form
  {
    FrmFlowChart frmFlowChart;

    Graphic graphic;
    Config config;

    public Form1()
    {
      PureComponents.TreeView.Licensing.RunTimeLicenseKey = "064F-7C2E-DE50-EC09-2385-CF16-C27A-E74F";

      InitializeComponent();
      SetProjectBasedButtons(false);

      try
      {
        //dockManager1.LoadToolWindowLayoutFromFile("Recent.layout");
      }
      catch (Exception)
      {
      }

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
          frmFlowChart.fcFlowChart.DefaultShape = config.graphicStencils[stencilName].ShapeTemplate();
        }
      }
    }

    void NewItem_ModelType_Changed(object sender, EventArgs e)
    {
      string groupName = "";

      barManager1.Commands["NewItem.GraphicType"].Enabled = false;

      int stencilIndex = (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).SelectedIndex;
      string stencilName = (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).Items[stencilIndex] as string;
      frmFlowChart.currentModelShape = stencilName;

      ModelStencil modelStencil;
      config.modelStencils.TryGetValue(stencilName, out modelStencil);
      if (modelStencil != null)
      {
        groupName = modelStencil.groupName;
        barManager1.Commands["NewItem.GraphicType"].Enabled = true;
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

      foreach (GraphicStencil graphicStencil in config.graphicStencils.Values)
      {
        if (groupName == graphicStencil.groupName)
        {
          int i = (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).Items.Add(graphicStencil.id);
          GlobalShapes.list.Add(graphicStencil.id);
        }
      }

      (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).Items.Add("-------");
      GlobalShapes.list.Add("-------");

      foreach (GraphicStencil graphicStencil in config.graphicStencils.Values)
      {
        if (groupName != graphicStencil.groupName)
        {
          (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).Items.Add(graphicStencil.id);
          GlobalShapes.list.Add(graphicStencil.id);
        }
      }
      (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).SelectedIndex = 0;
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
      }
    }

    private void Mode_CreateLink()
    {
      frmFlowChart.fcFlowChart.Behavior = BehaviorType.CreateArrow;
    }

    private void Mode_CreateNode()
    {
      frmFlowChart.fcFlowChart.Behavior = BehaviorType.CreateBox;
    }

    private void Mode_Modify()
    {
      frmFlowChart.fcFlowChart.Behavior = BehaviorType.Modify;
    }

    private void View_SelectItems()
    {
      frmFlowChart.bod.SelectItems = ((IBarCheckableCommand)barManager1.Commands["Selection.SelectItems"]).Checked;

      if (!frmFlowChart.bod.SelectItems)
      {
        foreach (Box box in frmFlowChart.fcFlowChart.Boxes)
        {
          box.Selected = false;
        }
      }
    }

    private void View_SelectArrows()
    {
      frmFlowChart.bod.SelectLinks = ((IBarCheckableCommand)barManager1.Commands["Selection.SelectLinks"]).Checked;

      if (!frmFlowChart.bod.SelectLinks)
      {
        foreach (Arrow arrow in frmFlowChart.fcFlowChart.Arrows)
        {
          arrow.Selected = false;
        }
      }
    }

    private void View_ShowModels()
    {
      frmFlowChart.bod.ShowModels = ((IBarCheckableCommand)barManager1.Commands["View.ShowModels"]).Checked;

      foreach (BODThing itemBox in frmFlowChart.bod.things.Values)
      {
        if (itemBox.Visible)
          itemBox.Model.Visible = frmFlowChart.bod.ShowModels;
      }
    }

    private void View_ShowGraphics()
    {
      frmFlowChart.bod.ShowGraphics = ((IBarCheckableCommand)barManager1.Commands["View.ShowGraphics"]).Checked;

      foreach (BODThing itemBox in frmFlowChart.bod.things.Values)
      {
        if (itemBox.Visible)
          itemBox.Graphic.Visible = frmFlowChart.bod.ShowGraphics;
      }
    }

    private void View_ShowLinks()
    {
      frmFlowChart.bod.ShowLinks = ((IBarCheckableCommand)barManager1.Commands["View.ShowLinks"]).Checked;

      foreach (Arrow arrow in frmFlowChart.fcFlowChart.Arrows)
      {
        bool visible = true;
        BODThing origin = arrow.Origin.Tag as BODThing;
        BODThing destination = arrow.Destination.Tag as BODThing;

        // only set to false if the endpoint exists and is invisible.  disconnected arrows must be visible.
        if (origin != null) visible = visible && origin.Visible;
        if (destination != null) visible = visible && destination.Visible;

        if (visible)
          arrow.Visible = frmFlowChart.bod.ShowLinks;
      }
    }

    private void View_ShowTags()
    {
      frmFlowChart.bod.ShowTags = ((IBarCheckableCommand)barManager1.Commands["View.ShowTags"]).Checked;

      foreach (Arrow arrow in frmFlowChart.fcFlowChart.Arrows)
      {
        if (frmFlowChart.bod.ShowTags)
          arrow.Font = new System.Drawing.Font("Microsoft Sans Serif", 5.25F);
        else
          arrow.Font = new System.Drawing.Font("Microsoft Sans Serif", 0.25F);
      }

      foreach (BODThing itemBox in frmFlowChart.bod.things.Values)
      {
        if (frmFlowChart.bod.ShowTags)
        {
          itemBox.Graphic.Font = new System.Drawing.Font("Microsoft Sans Serif", 5.25F);
          itemBox.Model.Font = new System.Drawing.Font("Microsoft Sans Serif", 5.25F);
        }
        else
        {
          itemBox.Graphic.Font = new System.Drawing.Font("Microsoft Sans Serif", 0.25F);
          itemBox.Model.Font = new System.Drawing.Font("Microsoft Sans Serif", 0.25F);
        }
      }  
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
      frmFlowChart.fcFlowChart.ZoomOut();
    }

    private void View_ZoomIn()
    {
      frmFlowChart.fcFlowChart.ZoomIn();
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
    }

    private void File_CloseProject()
    {
      SetProjectBasedButtons(false);
      frmFlowChart.Close();
    }

    private void File_OpenProject()
    {
      // Close the one selected first.
      if (frmFlowChart != null)
        frmFlowChart.Close();
      tvNavigation.Nodes.Clear();

      OpenProjectForm openProjectForm = new OpenProjectForm();

      if (openProjectForm.ShowDialog() == DialogResult.OK)
      {
        openProjectForm.Close();
        Refresh();

        config = openProjectForm.config;
        graphic = openProjectForm.graphic;

        frmFlowChart = new FrmFlowChart();

        string[] shapesStringArray = new string[config.graphicStencils.Count];
        int i=0;
        
        foreach (string key in config.graphicStencils.Keys)
        {
          shapesStringArray[i] = key;
          i++;
        }

        tvNavigation_SetProject();
        frmFlowChart.SetProject(graphic, config, tvNavigation);

        (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).Items.Clear();
        foreach (string key in config.modelStencils.Keys)
        {
          (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).Items.Add(key);
        }
        (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).SelectedIndex = 3;

        frmFlowChart.MdiParent = this;
        frmFlowChart.Text = openProjectForm.graphic.Name;
        frmFlowChart.fcFlowChart.SelectionChanged += new SelectionEvent(this.frmFlowChart_fcFlowChart_SelectionChanged);
        frmFlowChart.Show();

        ovOverview.Document = frmFlowChart.fcFlowChart;
        
        // Force update of the panels.
        twOverview.Size = new Size(twOverview.Size.Width, twOverview.Size.Height + 1);
        this.Size = new Size(this.Size.Width, this.Size.Height + 1);
        twOverview.Size = new Size(twOverview.Size.Width, twOverview.Size.Height - 1);
        this.Size = new Size(this.Size.Width, this.Size.Height - 1);
      }

      SetProjectBasedButtons(true);
    }

    private void tvNavigation_SetProject()
    {
      foreach (Area area in graphic.___areas.Values)
      {
        PureComponents.TreeView.Node areaNode = tvNavigation.Nodes.Add(area.tag, area.tag);
        areaNode.Tag = area.tag;
        foreach (string itemKey in area.items.Keys)
        {
          Item item = graphic.items[itemKey] as Item;
          PureComponents.TreeView.Node itemNode = areaNode.Nodes.Add(item.Tag, item.Tag);
          itemNode.Tag = item.Tag;
        }
      }
    }

    private void Form1_FormClosing(object sender, FormClosingEventArgs e)
    {
      //dockManager1.SaveToolWindowLayoutToFile("Recent.layout");
    }

    private void tvNavigation_AfterNodeCheck(PureComponents.TreeView.Node oNode)
    {
      this.tvNavigation.AfterNodeCheck -= new PureComponents.TreeView.TreeView.AfterNodeCheckEventHandler(this.tvNavigation_AfterNodeCheck);

      CheckSubNodes(oNode);

      if (graphic.items.ContainsKey(oNode.Text as string)) // This is an item, not an area.
      {
        frmFlowChart.bod.ThingVisible(oNode.Text, oNode.Checked);
      }

       this.tvNavigation.AfterNodeCheck += new PureComponents.TreeView.TreeView.AfterNodeCheckEventHandler(this.tvNavigation_AfterNodeCheck);
      frmFlowChart.ZoomToVisible();
    }

    private void CheckSubNodes(PureComponents.TreeView.Node oNode)
    {
      foreach (PureComponents.TreeView.Node node in oNode.Nodes)
      {
        node.Checked = oNode.Checked;

        if (graphic.items.ContainsKey(node.Tag as string)) // This is an item, not an area.
        {
          frmFlowChart.bod.ThingVisible(node.Text, node.Checked);
        }
        else
        {
          CheckSubNodes(node);
        }
      }
    }

    static System.Collections.Hashtable wasSelectedNodes = new System.Collections.Hashtable();

    private void tvNavigation_NodeSelectionChange(object sender, EventArgs e)
    {
      frmFlowChart.fcFlowChart.SelectionChanged -= new SelectionEvent(this.frmFlowChart_fcFlowChart_SelectionChanged);
      this.tvNavigation.NodeSelectionChange -= new System.EventHandler(this.tvNavigation_NodeSelectionChange);

      foreach (string key in graphic.___areas.Keys)
      {
        PureComponents.TreeView.Node wasNode = (wasSelectedNodes[key] as PureComponents.TreeView.Node);
        
        bool wasSelected = false;
        if (wasNode!=null)
          wasSelected = true;

        PureComponents.TreeView.Node isNode = tvNavigation.GetNodeByKey(key);

        bool isSelected = isNode.IsSelected;

        if (wasSelected == isSelected)
        {
          // Nothings changed at area level, update flowchart with innerNodes selection status.
          foreach (PureComponents.TreeView.Node innerNode in isNode.Nodes)
          {
            frmFlowChart.SetSelected(innerNode.Key, innerNode.IsSelected);
          }
        }
        else if (wasSelected && !isSelected)
        {
          // Been deselected, deselect all underlings and update flowchart.
          foreach (PureComponents.TreeView.Node innerNode in isNode.Nodes)
          {
            frmFlowChart.SetSelected(innerNode.Key, false);
            tvNavigation.RemoveSelectedNode(innerNode);
          }
        }
        else if (!wasSelected && isSelected)
        {
          // Been selected, select all underlings and update flowchart.
          foreach (PureComponents.TreeView.Node innerNode in isNode.Nodes)
          {
            frmFlowChart.SetSelected(innerNode.Key, true);
            tvNavigation.AddSelectedNode(innerNode);
          }
        }

        foreach (Arrow arrow in frmFlowChart.fcFlowChart.Arrows)
        {
          if ((arrow.Destination.Selected) && (arrow.Origin.Selected))
          {
            arrow.Selected = true;
          }
          else
          {
            arrow.Selected = false;
          }
        }

        wasSelectedNodes.Clear();
        foreach (PureComponents.TreeView.Node node in tvNavigation.SelectedNodes)
        {
          wasSelectedNodes.Add(node.Text, node);
        }
      }

      foreach (PureComponents.TreeView.Node node in tvNavigation.SelectedNodes)
      {
        frmFlowChart.fcFlowChart.Selection.AddObject(frmFlowChart.fcFlowChart.FindBox(node.Tag));
      }

      this.tvNavigation.NodeSelectionChange += new System.EventHandler(this.tvNavigation_NodeSelectionChange);
      frmFlowChart.fcFlowChart.SelectionChanged += new SelectionEvent(this.frmFlowChart_fcFlowChart_SelectionChanged);
    }

    private void frmFlowChart_fcFlowChart_SelectionChanged(object sender, EventArgs e)
    {
      frmFlowChart.fcFlowChart.SelectionChanged -= new SelectionEvent(this.frmFlowChart_fcFlowChart_SelectionChanged);
      this.tvNavigation.NodeSelectionChange -= new System.EventHandler(this.tvNavigation_NodeSelectionChange);

      tvNavigation.ClearSelectedNodes();

      foreach (BODThing itemBox in frmFlowChart.bod.things.Values)
      {
        if (frmFlowChart.bod.SelectItems)
        {
          if (itemBox.Graphic.Selected)
          {
            itemBox.Model.Selected = true;
            itemBox.Graphic.Selected = false;
          }

          if (itemBox.Model.Selected)
          {
            tvNavigation.AddSelectedNode(tvNavigation.GetNodeByKey(itemBox.Model.Text));
            itemBox.Graphic.FillColor = Color.FromArgb(50, itemBox.Graphic.FillColor);
            itemBox.Graphic.Pen.Color = Color.FromArgb(50, itemBox.Graphic.Pen.Color);
            itemBox.Graphic.ShadowColor = Color.FromArgb(50, itemBox.Graphic.ShadowColor);
            itemBox.Model.Visible = true;
            itemBox.Model.ZTop();
          }
          else
          {
            itemBox.Graphic.FillColor = Color.FromArgb(255, itemBox.Graphic.FillColor);
            itemBox.Graphic.Pen.Color = Color.FromArgb(255, itemBox.Graphic.Pen.Color);
            itemBox.Graphic.ShadowColor = Color.FromArgb(255, itemBox.Graphic.ShadowColor);
            itemBox.Model.Visible = frmFlowChart.bod.ShowModels;
            itemBox.Model.ZTop();
          }
        }
        else
        {
          itemBox.Model.Selected = false;
          itemBox.Graphic.Selected = false;
        }
      }

      if (!frmFlowChart.bod.SelectLinks)
      {
        foreach (Arrow arrow in frmFlowChart.fcFlowChart.Arrows)
        {
          arrow.Selected = false;
        }
      }

      if (frmFlowChart.fcFlowChart.ActiveObject is Arrow)
      {
        Arrow activeArrow = frmFlowChart.fcFlowChart.ActiveObject as Arrow;
        Link link;
        if (frmFlowChart.bod.graphic.links.TryGetValue(activeArrow.Text, out link))
        {
          propertyGrid1.SelectedObject = link;

        }
      }
      
      if (frmFlowChart.fcFlowChart.ActiveObject is Box)
      {
        Box activeBox = frmFlowChart.fcFlowChart.ActiveObject as Box;
        Item item;
        if (frmFlowChart.bod.graphic.items.TryGetValue(activeBox.Text, out item))
        {
          propertyGrid1.SelectedObject = item;
          propertyGrid1.PropertyValueChanged += new PropertyValueChangedEventHandler(propertyGrid1_PropertyValueChanged);

          int i;
          
          i = 0;
          foreach (string model in (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).Items)
          {
            if (model == item.Model)
            {
              (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).SelectedIndex = i;
              (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).Text = model;
            }
            i++;
          }
          if ((barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).SelectedIndex == -1)
            (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).SelectedIndex = 0;

          ModelStencil modelStencil;
          if (config.modelStencils.TryGetValue(item.Model, out modelStencil))
          {
            GraphicType_Populate(modelStencil.groupName);
          }

          i = 0;
          foreach (string shape in (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).Items)
          {
            if (shape == item.Shape)
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

      this.tvNavigation.NodeSelectionChange += new System.EventHandler(this.tvNavigation_NodeSelectionChange);
      frmFlowChart.fcFlowChart.SelectionChanged += new SelectionEvent(this.frmFlowChart_fcFlowChart_SelectionChanged);
    }

    void propertyGrid1_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
    {
      string label = e.ChangedItem.Label;
      if (label == "Stencil")
      {
        string graphicString = (e.ChangedItem.Value as String);
        GraphicStencil graphicShape;
        if (config.graphicStencils.TryGetValue(graphicString, out graphicShape))
        {
          Item item = (e.ChangedItem.Parent.Parent.Value as Item);

          item.Shape = graphicString;
          frmFlowChart.bod.SetStencil(item.Tag, graphicShape.ShapeTemplate());
        }
      }
    }

    private void tvNavigation_NodeDragDrop(DragEventArgs e, PureComponents.TreeView.Node oNode)
    {

    }

    private void tvNavigation_NodeDragEnter(DragEventArgs e, PureComponents.TreeView.Node oNode)
    {

    }

    private void tvNavigation_NodeDragLeave(DragEventArgs e, PureComponents.TreeView.Node oNode)
    {

    }

    private void tvNavigation_NodeDragOver(DragEventArgs e, PureComponents.TreeView.Node oNode)
    {

    }

    private void tvNavigation_DragDrop(object sender, DragEventArgs e)
    {

    }

    private void tvNavigation_DragEnter(object sender, DragEventArgs e)
    {

    }

    private void tvNavigation_DragLeave(object sender, EventArgs e)
    {

    }

    private void tvNavigation_DragOver(object sender, DragEventArgs e)
    {

    }

    private void tvNavigation_GiveFeedback(object sender, GiveFeedbackEventArgs e)
    {

    }

    private void tvNavigation_QueryContinueDrag(object sender, QueryContinueDragEventArgs e)
    {

    }
  }
}