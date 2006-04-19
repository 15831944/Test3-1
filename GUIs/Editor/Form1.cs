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

    private void Edit_Paste()
    {
      foreach (GraphicItem graphicItem in cbGraphicItems.Values)
      {
        graphicItem.Tag += ".";
        graphicItem.X += 10.0F;
        graphicItem.Y += 10.0F;
        frmFlowChart.state.newItem(graphicItem, true, frmFlowChart.fcFlowChart);
        tvNavigation.Nodes.Add(graphicItem.Tag, graphicItem.Tag);
        tvNavigation.AddSelectedNode(tvNavigation.GetNodeByKey(graphicItem.Tag));
      }

      foreach (GraphicLink graphicLink in cbGraphicLinks.Values)
      {
        graphicLink.Tag += ".";
        graphicLink.Source += ".";
        graphicLink.Destination += ".";
        Arrow arrow = frmFlowChart.fcFlowChart.CreateArrow(new PointF(0.0F, 0.0F), new PointF(10.0F, 10.0F));
        frmFlowChart.state.newGraphicLink(graphicLink, arrow, true);
      }
    }

    private void Edit_Cut()
    {
      throw new Exception("The method or operation is not implemented.");
    }

    private void Edit_Copy()
    {
      cbGraphicItems = new Dictionary<string, GraphicItem>();
      cbGraphicLinks = new Dictionary<string, GraphicLink>();

      //frmFlowChart.fcFlowChart.CopyToClipboard(true);

      foreach (ChartObject chartObject in frmFlowChart.fcFlowChart.Selection.Objects)
      {
        if (chartObject is Box)
        {
          Box box = chartObject as Box;
          GraphicItem graphicItem = frmFlowChart.state.GraphicItem(box.Text);
          if (graphicItem != null)
          {
            GraphicItem cbGraphicItem = new GraphicItem(box.Text);
            cbGraphicItem.X = graphicItem.X + 10.0F;
            cbGraphicItem.Y = graphicItem.Y + 10.0F;
            cbGraphicItem.Width = graphicItem.Width;
            cbGraphicItem.Height = graphicItem.Height;
            cbGraphicItem.Angle = graphicItem.Angle;
            cbGraphicItem.Model = graphicItem.Model;
            cbGraphicItem.Shape = graphicItem.Shape;
            cbGraphicItem.MirrorX = graphicItem.MirrorX;
            cbGraphicItem.MirrorY = graphicItem.MirrorY;
            cbGraphicItem.fillColor = graphicItem.fillColor;

            cbGraphicItems.Add(box.Text, cbGraphicItem);
          }
        }

        if (chartObject is Arrow)
        {
          Arrow arrow = chartObject as Arrow;
          GraphicLink graphicLink = frmFlowChart.state.GraphicLink(arrow.Text);
          if (graphicLink != null)
          {
            GraphicLink cbLink = new GraphicLink(arrow.Text);
            cbLink.Tag = graphicLink.Tag;
            cbLink.ClassID = graphicLink.ClassID;
            cbLink.Source = graphicLink.Source;
            cbLink.Destination = graphicLink.Destination;

            cbGraphicLinks.Add(arrow.Text, cbLink);
          }
        }

      }
    }

    #region clipboard support

    public void CopyToClipboard()
    {
      // create clones of selected items
      Graphic data = copySelection(frmFlowChart.fcFlowChart);

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
          Graphic pasteData = dataObj.GetData("Kenwalt.GraphicData") as Graphic;

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
              if (tagConversion.ContainsKey(newGraphicLink.Source))
                newGraphicLink.Source = tagConversion[newGraphicLink.Source];
              else
                newGraphicLink.Source = "";
              if (tagConversion.ContainsKey(newGraphicLink.Destination))
                newGraphicLink.Destination = tagConversion[newGraphicLink.Destination];
              else
                newGraphicLink.Destination = "";

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

    private Graphic copySelection(FlowChart doc)
    {
      if (doc.Selection.Objects.Count == 0)
        return null;

      Graphic copyGraphic = new Graphic();

      //Dictionary<string, int> areaCount = new Dictionary<string,int>();

      foreach (Box box in doc.Selection.Boxes)
      {
        GraphicItem graphicItem = frmFlowChart.state.GraphicItem(box.Text);
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

          copyGraphic.graphicItems.Add(box.Text, copyGraphicItem);

          // calculate the most contained area for pasting later-on -- going to use the currently selected area instead.
          //string areaText = tvNavigation.GetNodeByKey(box.Text).Parent.Text;
          //if (areaCount.ContainsKey(areaText))
          //  areaCount[areaText] = areaCount[areaText] + 1;
          //else
          //  areaCount.Add(areaText, 0);
        }
      }

      //foreach (string key in areaCount.Keys)
      //areaCount.OnDeserialization

      foreach (Arrow arrow in doc.Selection.Arrows)
      {
        GraphicLink graphicLink = frmFlowChart.state.GraphicLink(arrow.Text);
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

          copyGraphic.graphicLinks.Add(arrow.Text, copyGraphicLink);
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
      frmFlowChart.fcFlowChart.Behavior = BehaviorType.CreateBox;
    }

    private void Mode_Modify()
    {
      barManager1.Commands["NewItem.GraphicType"].Enabled = false;
      barManager1.Commands["NewItem.ModelType"].Enabled = false;
      frmFlowChart.fcFlowChart.Behavior = BehaviorType.Modify;
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

      foreach (Item itemBox in frmFlowChart.state.Items)
      {
        if (itemBox.Visible)
          itemBox.Model.Visible = frmFlowChart.state.ShowModels;
      }
    }

    private void View_ShowGraphics()
    {
      frmFlowChart.state.ShowGraphics = ((IBarCheckableCommand)barManager1.Commands["View.ShowGraphics"]).Checked;

      foreach (Item itemBox in frmFlowChart.state.Items)
      {
        if (itemBox.Visible)
          itemBox.Graphic.Visible = frmFlowChart.state.ShowGraphics;
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

        if (visible)
          arrow.Visible = frmFlowChart.state.ShowLinks;
      }
    }

    private void View_ShowTags()
    {
      frmFlowChart.state.ShowTags = ((IBarCheckableCommand)barManager1.Commands["View.ShowTags"]).Checked;

      foreach (Arrow arrow in frmFlowChart.fcFlowChart.Arrows)
      {
        if (frmFlowChart.state.ShowTags)
          arrow.Font = new System.Drawing.Font("Microsoft Sans Serif", 5.25F);
        else
          arrow.Font = new System.Drawing.Font("Microsoft Sans Serif", 0.25F);
      }

      foreach (Item itemBox in frmFlowChart.state.Items)
      {
        if (frmFlowChart.state.ShowTags)
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

        frmFlowChart = new FrmFlowChart();

        frmFlowChart.SuspendLayout();
        SuspendLayout();

        frmFlowChart.WindowState = System.Windows.Forms.FormWindowState.Maximized;

        frmFlowChart.MdiParent = this;
        frmFlowChart.Text = openProjectForm.graphic.Name;
        frmFlowChart.fcFlowChart.SelectionChanged += new SelectionEvent(this.frmFlowChart_fcFlowChart_SelectionChanged);

        frmFlowChart.SetProject(openProjectForm.graphic, openProjectForm.config, tvNavigation);
        tvNavigation_SetProject();
        ovOverview.Document = frmFlowChart.fcFlowChart;

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
      foreach (GraphicArea graphicArea in frmFlowChart.state.GraphicAreas)
      {
        PureComponents.TreeView.Node areaNode = tvNavigation.Nodes.Add(graphicArea.tag, graphicArea.tag);
        areaNode.Tag = graphicArea.tag;
        areaNode.AllowDrag = true;
        areaNode.AllowDrop = true;
        SetArea(areaNode, graphicArea);
      }

      foreach (PureComponents.TreeView.Node node in tvNavigation.Nodes)
      {
        node.Select();
        node.Expand();
      }
    }

    private void SetArea(PureComponents.TreeView.Node node, GraphicArea graphicArea)
    {
      foreach (string areaKey in graphicArea.graphicAreas.Keys)
      {
        GraphicArea subGraphicArea = graphicArea.graphicAreas[areaKey] as GraphicArea;
        PureComponents.TreeView.Node areaNode = node.Nodes.Add(subGraphicArea.tag, subGraphicArea.tag);
        areaNode.Tag = subGraphicArea.tag;
        areaNode.AllowDrag = true;
        areaNode.AllowDrop = true;
        SetArea(areaNode, subGraphicArea);
      }
      foreach (string key in graphicArea.graphicItems.Keys)
      {
        GraphicItem graphicItem = frmFlowChart.state.GraphicItem(key);
        PureComponents.TreeView.Node itemNode = node.Nodes.Add(graphicItem.Tag, graphicItem.Tag);
        itemNode.Tag = graphicItem.Tag;
        itemNode.AllowDrag = true;
        itemNode.AllowDrop = false;
      }
    }

    private void Form1_FormClosing(object sender, FormClosingEventArgs e)
    {
      if (frmFlowChart != null)
        File_CloseProject();

      //dockManager1.SaveToolWindowLayoutToFile("Recent.layout");
    }

    static System.Collections.Hashtable wasSelectedNodes = new System.Collections.Hashtable();

    private void tvNavigation_NodeSelectionChange(object sender, EventArgs e)
    {
      tvNavigation.NodeSelectionChange -= new System.EventHandler(this.tvNavigation_NodeSelectionChange);

      foreach (GraphicArea graphicArea in frmFlowChart.state.GraphicAreas)
      {
        SelectSubNodes(graphicArea.tag);
      }

      wasSelectedNodes.Clear();
      foreach (PureComponents.TreeView.Node node in tvNavigation.SelectedNodes)
      {
        wasSelectedNodes.Add(node.Text, node);
      }

      foreach (PureComponents.TreeView.Node node in tvNavigation.SelectedNodes)
      {
        frmFlowChart.state.ItemVisible(node.Key, true);
      }

      frmFlowChart.ZoomToVisible();

      tvNavigation.NodeSelectionChange += new System.EventHandler(this.tvNavigation_NodeSelectionChange);
    }

    private void SelectSubNodes(string key)
    {
      PureComponents.TreeView.Node wasNode = (wasSelectedNodes[key] as PureComponents.TreeView.Node);

      bool wasSelected = false;
      if (wasNode != null)
        wasSelected = true;

      PureComponents.TreeView.Node isNode = tvNavigation.GetNodeByKey(key);

      bool isSelected = isNode.IsSelected;

      if (wasSelected == isSelected)
      {
        // Nothings changed at area level, update flowchart with innerNodes selection status.
        foreach (PureComponents.TreeView.Node innerNode in isNode.Nodes)
        {
          if (innerNode.Key != null)
          {
            frmFlowChart.state.ItemVisible(innerNode.Key, innerNode.IsSelected);
            SelectSubNodes(innerNode.Key);
          }
        }
      }
      else if (wasSelected && !isSelected)
      {
        // Been deselected, deselect all underlings and update flowchart.
        foreach (PureComponents.TreeView.Node innerNode in isNode.Nodes)
        {
          if (innerNode.Key != null)
          {
            frmFlowChart.state.ItemVisible(innerNode.Key, false);
            tvNavigation.RemoveSelectedNode(innerNode);
            SelectSubNodes(innerNode.Key);
          }
        }
      }
      else if (!wasSelected && isSelected)
      {
        // Been selected, select all underlings and update flowchart.
        foreach (PureComponents.TreeView.Node innerNode in isNode.Nodes)
        {
          if (innerNode.Key != null)
          {
            frmFlowChart.state.ItemVisible(innerNode.Key, true);
            tvNavigation.AddSelectedNode(innerNode);
            SelectSubNodes(innerNode.Key);
          }
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
        }

        if (item.Model.Selected)
        {
          item.Model.ZTop();
          item.Model.Visible = true;
        }
        else
        {
          item.Model.Visible = frmFlowChart.state.ShowModels;
        }
      }

      //if (frmFlowChart.fcFlowChart.ActiveObject is Arrow)
      //{
      //  Arrow activeArrow = frmFlowChart.fcFlowChart.ActiveObject as Arrow;
      //  GraphicLink graphicLink = frmFlowChart.state.GraphicLink(activeArrow.Text);
      //  if (graphicLink != null)
      //  {
      //    propertyGrid1.SelectedObject = graphicLink;

      //  }
      //}
      
      //if (frmFlowChart.fcFlowChart.ActiveObject is Box)
      //{
      //  Box activeBox = frmFlowChart.fcFlowChart.ActiveObject as Box;
      //  GraphicItem graphicItem = frmFlowChart.state.GraphicItem(activeBox.Text);
      //  if (graphicItem != null)
      //  {
      //    propertyGrid1.SelectedObject = graphicItem;
      //    propertyGrid1.PropertyValueChanged += new PropertyValueChangedEventHandler(propertyGrid1_PropertyValueChanged);

      //    int i;
          
      //    i = 0;
      //    foreach (string model in (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).Items)
      //    {
      //      if (model == graphicItem.Model)
      //      {
      //        (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).SelectedIndex = i;
      //        (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).Text = model;
      //      }
      //      i++;
      //    }
      //    if ((barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).SelectedIndex == -1)
      //      (barManager1.Commands["NewItem.ModelType"] as BarComboBoxCommand).SelectedIndex = 0;

      //    ModelStencil modelStencil = frmFlowChart.state.ModelStencil(graphicItem.Model);
      //    if (modelStencil != null)
      //    {
      //      GraphicType_Populate(modelStencil.groupName);
      //    }

      //    i = 0;
      //    foreach (string shape in (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).Items)
      //    {
      //      if (shape == graphicItem.Shape)
      //      {
      //        (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).SelectedIndex = i;
      //        (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).Text = shape;
      //      }
      //      i++;
      //    }
      //    if ((barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).SelectedIndex == -1)
      //      (barManager1.Commands["NewItem.GraphicType"] as BarComboBoxCommand).SelectedIndex = 0;
      //  }
      //}

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
          frmFlowChart.state.SetStencil(graphicItem.Tag, graphicShape.ShapeTemplate(graphicItem.MirrorX, graphicItem.MirrorY));
        }
      }

      if (label == "Angle")
      {
        float angle = (float)e.ChangedItem.Value;
        GraphicItem graphicItem = (e.ChangedItem.Parent.Parent.Value as GraphicItem);

        graphicItem.Angle = angle;
        frmFlowChart.state.SetAngle(graphicItem.Tag, angle);
      }

      if (label == "Height")
      {
        float height = (float)e.ChangedItem.Value;
        GraphicItem graphicItem = (e.ChangedItem.Parent.Parent.Value as GraphicItem);

        graphicItem.Height = height;
        frmFlowChart.state.SetHeight(graphicItem.Tag, height);
      }

      if (label == "Width")
      {
        float width = (float)e.ChangedItem.Value;
        GraphicItem graphicItem = (e.ChangedItem.Parent.Parent.Value as GraphicItem);

        graphicItem.Width = width;
        frmFlowChart.state.SetWidth(graphicItem.Tag, width);
      }

      if (label == "Left")
      {
        float x = (float)e.ChangedItem.Value;
        GraphicItem graphicItem = (e.ChangedItem.Parent.Parent.Value as GraphicItem);

        graphicItem.X = x;
        frmFlowChart.state.SetX(graphicItem.Tag, x);
      }

      if (label == "Right")
      {
        float y = (float)e.ChangedItem.Value;
        GraphicItem graphicItem = (e.ChangedItem.Parent.Parent.Value as GraphicItem);

        graphicItem.Y = y;
        frmFlowChart.state.SetY(graphicItem.Tag, y);
      }

      if (label == "Mirror X")
      {
        bool mirrorX = (bool)e.ChangedItem.Value;
        GraphicItem graphicItem = (e.ChangedItem.Parent.Parent.Value as GraphicItem);

        graphicItem.MirrorX = mirrorX;
        frmFlowChart.state.SetMirrorX(graphicItem.Tag, mirrorX);
      }

      if (label == "Mirror Y")
      {
        bool mirrorY = (bool)e.ChangedItem.Value;
        GraphicItem graphicItem = (e.ChangedItem.Parent.Parent.Value as GraphicItem);

        graphicItem.MirrorY = mirrorY;
        frmFlowChart.state.SetMirrorY(graphicItem.Tag, mirrorY);
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