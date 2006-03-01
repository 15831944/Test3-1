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

      try
      {
        //dockManager1.LoadToolWindowLayoutFromFile("Recent.layout");
      }
      catch (Exception)
      {
      }
    }

    private void barManager1_CommandClick(object sender, ActiproSoftware.UIStudio.Bar.BarCommandLinkEventArgs e)
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

    private void File_CloseProject()
    {
      frmFlowChart.Close();
      barManager1.Commands["File.PrintPreview"].Enabled = false;
      barManager1.Commands["File.Print"].Enabled = false;
      barManager1.Commands["File.Close"].Enabled = false;
      barManager1.Commands["View.ZoomIn"].Enabled = false;
      barManager1.Commands["View.ZoomOut"].Enabled = false;
      barManager1.Commands["View.ZoomToVisible"].Enabled = false;
      barManager1.Commands["View.ZoomToSelected"].Enabled = false;
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

        tbStencils.Shapes = shapesStringArray;
        tbStencils.RecreateImages();

        tvNavigation_SetProject();
        frmFlowChart.SetProject(graphic, config);
        frmFlowChart.MdiParent = this;
        frmFlowChart.Text = openProjectForm.graphic.Name;
        frmFlowChart.fcFlowChart.SelectionChanged += new SelectionEvent(this.frmFlowChart_fcFlowChart_SelectionChanged);
        frmFlowChart.Show();

        ovOverview.Document = frmFlowChart.fcFlowChart;
        tbStencils.Document = frmFlowChart.fcFlowChart;

        // Force update of the panels.
        twOverview.Size = new Size(twOverview.Size.Width, twOverview.Size.Height + 1);
        this.Size = new Size(this.Size.Width, this.Size.Height + 1);
        twOverview.Size = new Size(twOverview.Size.Width, twOverview.Size.Height - 1);
        this.Size = new Size(this.Size.Width, this.Size.Height - 1);
      }

      barManager1.Commands["File.PrintPreview"].Enabled = true;
      barManager1.Commands["File.Print"].Enabled = true;
      barManager1.Commands["File.Close"].Enabled = true;
      barManager1.Commands["View.ZoomIn"].Enabled = true;
      barManager1.Commands["View.ZoomOut"].Enabled = true;
      barManager1.Commands["View.ZoomToVisible"].Enabled = true;
      barManager1.Commands["View.ZoomToSelected"].Enabled = true;

      CustomClass myProperties = new CustomClass();
      propertyGrid1.SelectedObject = myProperties;
      CustomProperty myProp = new CustomProperty("Test 1", "Test 2", false, true);
      myProperties.Add(myProp);
      propertyGrid1.Refresh();
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
          PureComponents.TreeView.Node itemNode = areaNode.Nodes.Add(item.tag, item.tag);
          itemNode.Tag = item.tag;
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

      foreach (PureComponents.TreeView.Node node in oNode.Nodes)
      {
        node.Checked = oNode.Checked;

        if (graphic.items.ContainsKey(node.Tag as string)) // This is an item, not an area.
        {
          frmFlowChart.SetVisible(node.Text, node.Checked);
        }
      }

      if (graphic.items.ContainsKey(oNode.Tag as string)) // This is an item, not an area.
      {
        frmFlowChart.SetVisible(oNode.Text, oNode.Checked);
      }
      
      this.tvNavigation.AfterNodeCheck += new PureComponents.TreeView.TreeView.AfterNodeCheckEventHandler(this.tvNavigation_AfterNodeCheck);
      frmFlowChart.ZoomToVisible();
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
          wasSelectedNodes.Add(node.Key, node);
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
      foreach (Box box in frmFlowChart.fcFlowChart.Boxes)
      {
        if (box.Selected)
        {
          if (box.Tag == null)
          {
            Box modelBox = frmFlowChart.fcFlowChart.FindBox(box.Text);
            if (modelBox != null)
            {
              modelBox.Selected = true;
              modelBox.ZTop();
              tvNavigation.AddSelectedNode(tvNavigation.GetNodeByKey(modelBox.Text));
            }
          }
          else
          {
            box.ZTop();
            tvNavigation.AddSelectedNode(tvNavigation.GetNodeByKey(box.Text));
          }
        }
      }

      this.tvNavigation.NodeSelectionChange += new System.EventHandler(this.tvNavigation_NodeSelectionChange);
      frmFlowChart.fcFlowChart.SelectionChanged += new SelectionEvent(this.frmFlowChart_fcFlowChart_SelectionChanged);
    }
  }
}