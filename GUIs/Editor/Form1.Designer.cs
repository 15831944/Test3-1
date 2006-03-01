using MindFusion.FlowChartX;
namespace SysCAD.Editor
{
  partial class Form1
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      PureComponents.TreeView.ContextMenuStrings contextMenuStrings1 = new PureComponents.TreeView.ContextMenuStrings();
      PureComponents.TreeView.TreeViewStyle treeViewStyle1 = new PureComponents.TreeView.TreeViewStyle();
      PureComponents.TreeView.NodeStyle nodeStyle1 = new PureComponents.TreeView.NodeStyle();
      PureComponents.TreeView.CheckBoxStyle checkBoxStyle1 = new PureComponents.TreeView.CheckBoxStyle();
      PureComponents.TreeView.ExpandBoxStyle expandBoxStyle1 = new PureComponents.TreeView.ExpandBoxStyle();
      PureComponents.TreeView.NodeTooltipStyle nodeTooltipStyle1 = new PureComponents.TreeView.NodeTooltipStyle();
      ActiproSoftware.UIStudio.Bar.BarButtonCommand editUndoBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("Edit", "Undo", "&Undo", -1);
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
      ActiproSoftware.UIStudio.Bar.BarButtonCommand editRedoBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("Edit", "Redo", "&Redo", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand editCutBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("Edit", "Cut", "Cu&t", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand editCopyBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("Edit", "Copy", "&Copy", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand editPasteBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("Edit", "Paste", "&Paste", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand editDeleteBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("Edit", "Delete", "&Delete", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand editSelectAllBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("Edit", "SelectAll", "Select &All", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand editFindBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("Edit", "Find", "&Find", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand toolsCustomizeBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("Tools", "Customize", "&Customize...", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand toolsOptionsBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("Tools", "Options", "&Options...", -1);
      ActiproSoftware.UIStudio.Bar.BarExpanderButtonCommand windowWindowListBarExpanderButtonCommand = new ActiproSoftware.UIStudio.Bar.BarExpanderButtonCommand("Window", "WindowList", "Window List", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand helpContentsBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("Help", "Contents", "&Contents...", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand helpIndexBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("Help", "Index", "&Index...", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand helpSearchBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("Help", "Search", "&Search...", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand helpAboutBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("Help", "About", "&About...", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand viewZoomInBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("View", "ZoomIn", "Zoom In", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand viewZoomOutBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("View", "ZoomOut", "Zoom Out", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand viewZoomToVisibleBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("View", "ZoomToVisible", "Zoom To Visible", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand viewZoomToSelectedBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("View", "ZoomToSelected", "Zoom To Selected", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand fileNewBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("File", "New", "&New...", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand fileOpenBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("File", "Open", "&Open...", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand fileCloseBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("File", "Close", "&Close", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand fileSaveBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("File", "Save", "&Save", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand fileSaveAsBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("File", "SaveAs", "Save &As...", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand fileSaveAllBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("File", "SaveAll", "Save A&ll", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand filePrintBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("File", "Print", "&Print", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand filePrintPreviewBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("File", "PrintPreview", "Print Pre&view", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand fileExitBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("File", "Exit", "E&xit", -1);
      ActiproSoftware.UIStudio.Bar.BarPopupButtonCommand newMenuNewMenuBarPopupButtonCommand = new ActiproSoftware.UIStudio.Bar.BarPopupButtonCommand("New Menu", "New Menu", "New Menu", -1);
      ActiproSoftware.UIStudio.Bar.BarComboBoxCommand newItemModelTypeBarComboBoxCommand = new ActiproSoftware.UIStudio.Bar.BarComboBoxCommand("NewItem", "ModelType", "ModelType", -1);
      ActiproSoftware.UIStudio.Bar.BarComboBoxCommand newItemGraphicTypeBarComboBoxCommand = new ActiproSoftware.UIStudio.Bar.BarComboBoxCommand("NewItem", "GraphicType", "GraphicType", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand newItemNetItemDragTargetBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("NewItem", "NetItemDragTarget", "Net Item Drag Target", -1);
      ActiproSoftware.UIStudio.Bar.MenuBar mainMenuMenuBar = new ActiproSoftware.UIStudio.Bar.MenuBar("Main Menu");
      ActiproSoftware.UIStudio.Bar.BarPopupButtonLink newMenuNewMenuBarPopupButtonLink = new ActiproSoftware.UIStudio.Bar.BarPopupButtonLink(newMenuNewMenuBarPopupButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink fileNewBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(fileNewBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink fileOpenBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(fileOpenBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink fileCloseBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(fileCloseBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink fileSaveBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(fileSaveBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink fileSaveAsBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(fileSaveAsBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink fileSaveAllBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(fileSaveAllBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink filePrintBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(filePrintBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink filePrintPreviewBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(filePrintPreviewBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink fileExitBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(fileExitBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarPopupButtonLink newMenuNewMenuBarPopupButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarPopupButtonLink(newMenuNewMenuBarPopupButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editUndoBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editUndoBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editRedoBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editRedoBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editCutBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editCutBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editCopyBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editCopyBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editPasteBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editPasteBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editDeleteBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editDeleteBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editSelectAllBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editSelectAllBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editFindBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editFindBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarPopupButtonLink newMenuNewMenuBarPopupButtonLink3 = new ActiproSoftware.UIStudio.Bar.BarPopupButtonLink(newMenuNewMenuBarPopupButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewZoomInBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewZoomInBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewZoomOutBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewZoomOutBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewZoomToVisibleBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewZoomToVisibleBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewZoomToSelectedBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewZoomToSelectedBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarPopupButtonLink newMenuNewMenuBarPopupButtonLink4 = new ActiproSoftware.UIStudio.Bar.BarPopupButtonLink(newMenuNewMenuBarPopupButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink toolsCustomizeBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(toolsCustomizeBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink toolsOptionsBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(toolsOptionsBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarPopupButtonLink newMenuNewMenuBarPopupButtonLink5 = new ActiproSoftware.UIStudio.Bar.BarPopupButtonLink(newMenuNewMenuBarPopupButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarExpanderButtonLink windowWindowListBarExpanderButtonLink = new ActiproSoftware.UIStudio.Bar.BarExpanderButtonLink(windowWindowListBarExpanderButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarPopupButtonLink newMenuNewMenuBarPopupButtonLink6 = new ActiproSoftware.UIStudio.Bar.BarPopupButtonLink(newMenuNewMenuBarPopupButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink helpContentsBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(helpContentsBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink helpIndexBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(helpIndexBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink helpSearchBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(helpSearchBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink helpAboutBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(helpAboutBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarPopupButtonLink newMenuNewMenuBarPopupButtonLink7 = new ActiproSoftware.UIStudio.Bar.BarPopupButtonLink(newMenuNewMenuBarPopupButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink fileNewBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(fileNewBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink fileOpenBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(fileOpenBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink fileCloseBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(fileCloseBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink fileSaveBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(fileSaveBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink fileSaveAsBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(fileSaveAsBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink fileSaveAllBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(fileSaveAllBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink filePrintBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(filePrintBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink filePrintPreviewBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(filePrintPreviewBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink fileExitBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(fileExitBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarPopupButtonLink newMenuNewMenuBarPopupButtonLink8 = new ActiproSoftware.UIStudio.Bar.BarPopupButtonLink(newMenuNewMenuBarPopupButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editUndoBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editUndoBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editRedoBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editRedoBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editCutBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editCutBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editCopyBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editCopyBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editPasteBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editPasteBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editDeleteBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editDeleteBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editSelectAllBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editSelectAllBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editFindBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editFindBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarPopupButtonLink newMenuNewMenuBarPopupButtonLink9 = new ActiproSoftware.UIStudio.Bar.BarPopupButtonLink(newMenuNewMenuBarPopupButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewZoomInBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewZoomInBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewZoomOutBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewZoomOutBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewZoomToVisibleBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewZoomToVisibleBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewZoomToSelectedBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewZoomToSelectedBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarPopupButtonLink newMenuNewMenuBarPopupButtonLink10 = new ActiproSoftware.UIStudio.Bar.BarPopupButtonLink(newMenuNewMenuBarPopupButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink toolsCustomizeBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(toolsCustomizeBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink toolsOptionsBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(toolsOptionsBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarPopupButtonLink newMenuNewMenuBarPopupButtonLink11 = new ActiproSoftware.UIStudio.Bar.BarPopupButtonLink(newMenuNewMenuBarPopupButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarExpanderButtonLink windowWindowListBarExpanderButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarExpanderButtonLink(windowWindowListBarExpanderButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarPopupButtonLink newMenuNewMenuBarPopupButtonLink12 = new ActiproSoftware.UIStudio.Bar.BarPopupButtonLink(newMenuNewMenuBarPopupButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink helpContentsBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(helpContentsBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink helpIndexBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(helpIndexBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink helpSearchBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(helpSearchBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink helpAboutBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(helpAboutBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.DockableToolBar newItemDockableToolBar = new ActiproSoftware.UIStudio.Bar.DockableToolBar("newItem");
      ActiproSoftware.UIStudio.Bar.BarComboBoxLink newItemGraphicTypeBarComboBoxLink = new ActiproSoftware.UIStudio.Bar.BarComboBoxLink(newItemGraphicTypeBarComboBoxCommand);
      ActiproSoftware.UIStudio.Bar.BarComboBoxLink newItemModelTypeBarComboBoxLink = new ActiproSoftware.UIStudio.Bar.BarComboBoxLink(newItemModelTypeBarComboBoxCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink newItemNetItemDragTargetBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(newItemNetItemDragTargetBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarComboBoxLink newItemGraphicTypeBarComboBoxLink2 = new ActiproSoftware.UIStudio.Bar.BarComboBoxLink(newItemGraphicTypeBarComboBoxCommand);
      ActiproSoftware.UIStudio.Bar.BarComboBoxLink newItemModelTypeBarComboBoxLink2 = new ActiproSoftware.UIStudio.Bar.BarComboBoxLink(newItemModelTypeBarComboBoxCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink newItemNetItemDragTargetBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(newItemNetItemDragTargetBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.DockableToolBar standardDockableToolBar = new ActiproSoftware.UIStudio.Bar.DockableToolBar("Standard");
      ActiproSoftware.UIStudio.Bar.BarButtonLink editCutBarButtonLink3 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editCutBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editCopyBarButtonLink3 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editCopyBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editPasteBarButtonLink3 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editPasteBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editDeleteBarButtonLink3 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editDeleteBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editUndoBarButtonLink3 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editUndoBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editRedoBarButtonLink3 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editRedoBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editFindBarButtonLink3 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editFindBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editCutBarButtonLink4 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editCutBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editCopyBarButtonLink4 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editCopyBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editPasteBarButtonLink4 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editPasteBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editDeleteBarButtonLink4 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editDeleteBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editUndoBarButtonLink4 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editUndoBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editRedoBarButtonLink4 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editRedoBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink editFindBarButtonLink4 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(editFindBarButtonCommand);
      this.dockManager1 = new ActiproSoftware.UIStudio.Dock.DockManager();
      this.ovOverview = new Overview();
      this.twOverview = new ActiproSoftware.UIStudio.Dock.ToolWindow();
      this.tvNavigation = new PureComponents.TreeView.TreeView();
      this.twNavigation = new ActiproSoftware.UIStudio.Dock.ToolWindow();
      this.barManager1 = new ActiproSoftware.UIStudio.Bar.BarManager();
      this.barDockArea1 = new ActiproSoftware.UIStudio.Bar.BarDockArea();
      this.barDockArea2 = new ActiproSoftware.UIStudio.Bar.BarDockArea();
      this.barDockArea3 = new ActiproSoftware.UIStudio.Bar.BarDockArea();
      this.barDockArea4 = new ActiproSoftware.UIStudio.Bar.BarDockArea();
      this.toolWindowContainer1 = new ActiproSoftware.UIStudio.Dock.ToolWindowContainer();
      this.toolWindowContainer2 = new ActiproSoftware.UIStudio.Dock.ToolWindowContainer();
      this.dockContainerContainer1 = new ActiproSoftware.UIStudio.Dock.DockContainerContainer();
      this.twStencils = new ActiproSoftware.UIStudio.Dock.ToolWindow();
      this.tbStencils = new ShapeToolBar();
      this.toolWindowContainer3 = new ActiproSoftware.UIStudio.Dock.ToolWindowContainer();
      this.twGraphicProperties = new ActiproSoftware.UIStudio.Dock.ToolWindow();
      this.propertyGrid1 = new System.Windows.Forms.PropertyGrid();
      this.toolWindowContainer4 = new ActiproSoftware.UIStudio.Dock.ToolWindowContainer();
      ((System.ComponentModel.ISupportInitialize)(this.dockManager1)).BeginInit();
      this.twOverview.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.tvNavigation)).BeginInit();
      this.twNavigation.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.barManager1)).BeginInit();
      this.toolWindowContainer1.SuspendLayout();
      this.toolWindowContainer2.SuspendLayout();
      this.dockContainerContainer1.SuspendLayout();
      this.twStencils.SuspendLayout();
      this.toolWindowContainer3.SuspendLayout();
      this.twGraphicProperties.SuspendLayout();
      this.toolWindowContainer4.SuspendLayout();
      this.SuspendLayout();
      // 
      // dockManager1
      // 
      this.dockManager1.DocumentMdiStyle = ActiproSoftware.UIStudio.Dock.DocumentMdiStyle.Standard;
      this.dockManager1.HostContainerControl = this;
      this.dockManager1.NextWindowNavigationEnabled = true;
      this.dockManager1.TabbedMdiMaximumTabExtent = 150;
      // 
      // ovOverview
      // 
      this.ovOverview.DisplayShadows = false;
      this.ovOverview.Dock = System.Windows.Forms.DockStyle.Fill;
      this.ovOverview.Document = null;
      this.ovOverview.FitAll = true;
      this.ovOverview.Location = new System.Drawing.Point(0, 0);
      this.ovOverview.Name = "ovOverview";
      this.ovOverview.Size = new System.Drawing.Size(172, 139);
      this.ovOverview.TabIndex = 0;
      // 
      // twOverview
      // 
      this.twOverview.Controls.Add(this.ovOverview);
      this.twOverview.Dock = System.Windows.Forms.DockStyle.Fill;
      this.twOverview.DockManager = this.dockManager1;
      this.twOverview.Key = "twOverview";
      this.twOverview.Location = new System.Drawing.Point(0, 29);
      this.twOverview.Name = "twOverview";
      this.twOverview.Size = new System.Drawing.Size(172, 139);
      this.twOverview.TabIndex = 0;
      this.twOverview.Text = "Overview";
      // 
      // tvNavigation
      // 
      this.tvNavigation.AllowAdding = true;
      this.tvNavigation.AllowArranging = true;
      this.tvNavigation.AllowDeleting = true;
      this.tvNavigation.AllowEditing = true;
      this.tvNavigation.AutoDragDrop = true;
      this.tvNavigation.BackColor = System.Drawing.Color.White;
      this.tvNavigation.CheckBoxes = true;
      this.tvNavigation.ContextMenuArranging = false;
      this.tvNavigation.ContextMenuEditing = false;
      contextMenuStrings1.AddNode = "Add Node";
      contextMenuStrings1.Collapse = "Collapse";
      contextMenuStrings1.Copy = "Copy";
      contextMenuStrings1.DeleteNode = "Delete Node";
      contextMenuStrings1.EditNode = "Edit Node";
      contextMenuStrings1.Expand = "Expand";
      contextMenuStrings1.LoadXml = "Load Xml...";
      contextMenuStrings1.MoveBottom = "Move Bottom";
      contextMenuStrings1.MoveDown = "Move Down";
      contextMenuStrings1.MoveLeft = "Move Left";
      contextMenuStrings1.MoveRight = "Move Right";
      contextMenuStrings1.MoveTop = "Move Top";
      contextMenuStrings1.MoveUp = "Move Up";
      contextMenuStrings1.Paste = "Paste";
      contextMenuStrings1.SaveXml = "Save Xml...";
      this.tvNavigation.ContextMenuStrings = contextMenuStrings1;
      this.tvNavigation.ContextMenuXmlOperations = false;
      this.tvNavigation.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tvNavigation.ExpandOnDblClick = true;
      this.tvNavigation.Flags = false;
      this.tvNavigation.HideSelection = false;
      this.tvNavigation.ImageList = null;
      this.tvNavigation.LabelEdit = true;
      this.tvNavigation.Location = new System.Drawing.Point(0, 0);
      this.tvNavigation.Multiline = true;
      this.tvNavigation.Name = "tvNavigation";
      this.tvNavigation.NodeAutoNumbering = false;
      this.tvNavigation.PathSeparator = "\\";
      this.tvNavigation.SelectionMode = PureComponents.TreeView.SelectionMode.MultipleExtended;
      this.tvNavigation.Size = new System.Drawing.Size(172, 434);
      this.tvNavigation.Sorted = false;
      treeViewStyle1.AutoCollapse = false;
      treeViewStyle1.BackColor = System.Drawing.Color.White;
      treeViewStyle1.BorderColor = System.Drawing.Color.DimGray;
      treeViewStyle1.BorderStyle = PureComponents.TreeView.BorderStyle.Solid;
      treeViewStyle1.FadeColor = System.Drawing.Color.Khaki;
      treeViewStyle1.FillStyle = PureComponents.TreeView.FillStyle.Flat;
      treeViewStyle1.FlashColor = System.Drawing.Color.Red;
      treeViewStyle1.FullRowSelect = true;
      treeViewStyle1.HighlightSelectedPath = false;
      treeViewStyle1.LineColor = System.Drawing.Color.Gray;
      treeViewStyle1.LineStyle = PureComponents.TreeView.LineStyle.Dot;
      treeViewStyle1.NodeSpaceHorizontal = 0;
      treeViewStyle1.NodeSpaceVertical = 0;
      checkBoxStyle1.BackColor = System.Drawing.Color.White;
      checkBoxStyle1.BorderColor = System.Drawing.Color.Black;
      checkBoxStyle1.BorderStyle = PureComponents.TreeView.CheckBoxBorderStyle.Solid;
      checkBoxStyle1.CheckColor = System.Drawing.Color.Black;
      checkBoxStyle1.HoverBackColor = System.Drawing.Color.White;
      checkBoxStyle1.HoverBorderColor = System.Drawing.Color.Black;
      checkBoxStyle1.HoverCheckColor = System.Drawing.Color.Black;
      nodeStyle1.CheckBoxStyle = checkBoxStyle1;
      expandBoxStyle1.BackColor = System.Drawing.Color.SkyBlue;
      expandBoxStyle1.BorderColor = System.Drawing.Color.Black;
      expandBoxStyle1.ForeColor = System.Drawing.Color.Black;
      expandBoxStyle1.Shape = PureComponents.TreeView.ExpandBoxShape.XP;
      nodeStyle1.ExpandBoxStyle = expandBoxStyle1;
      nodeStyle1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
      nodeStyle1.ForeColor = System.Drawing.Color.Black;
      nodeStyle1.HoverBackColor = System.Drawing.Color.Moccasin;
      nodeStyle1.HoverForeColor = System.Drawing.Color.Black;
      nodeStyle1.Parent = null;
      nodeStyle1.SelectedBackColor = System.Drawing.Color.CornflowerBlue;
      nodeStyle1.SelectedBorderColor = System.Drawing.Color.RoyalBlue;
      nodeStyle1.SelectedFillStyle = PureComponents.TreeView.FillStyle.VerticalFading;
      nodeStyle1.SelectedForeColor = System.Drawing.Color.Black;
      nodeTooltipStyle1.BackColor = System.Drawing.Color.WhiteSmoke;
      nodeTooltipStyle1.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(142)))), ((int)(((byte)(125)))));
      nodeTooltipStyle1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
      nodeTooltipStyle1.ForeColor = System.Drawing.Color.Black;
      nodeStyle1.TooltipStyle = nodeTooltipStyle1;
      nodeStyle1.UnderlineColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))));
      nodeStyle1.UnderlineStyle = PureComponents.TreeView.UnderlineStyle.Tilde;
      treeViewStyle1.NodeStyle = nodeStyle1;
      treeViewStyle1.ShowLines = true;
      treeViewStyle1.ShowPlusMinus = true;
      treeViewStyle1.ShowSubitemsIndicator = false;
      treeViewStyle1.TrackNodeHover = true;
      this.tvNavigation.Style = treeViewStyle1;
      this.tvNavigation.TabIndex = 1;
      this.tvNavigation.TooltipPopDelay = 1500;
      this.tvNavigation.Tooltips = true;
      this.tvNavigation.NodeSelectionChange += new System.EventHandler(this.tvNavigation_NodeSelectionChange);
      this.tvNavigation.AfterNodeCheck += new PureComponents.TreeView.TreeView.AfterNodeCheckEventHandler(this.tvNavigation_AfterNodeCheck);
      // 
      // twNavigation
      // 
      this.twNavigation.Controls.Add(this.tvNavigation);
      this.twNavigation.Dock = System.Windows.Forms.DockStyle.Fill;
      this.twNavigation.DockManager = this.dockManager1;
      this.twNavigation.Key = "treeView1";
      this.twNavigation.Location = new System.Drawing.Point(0, 29);
      this.twNavigation.Name = "twNavigation";
      this.twNavigation.Size = new System.Drawing.Size(172, 434);
      this.twNavigation.TabIndex = 0;
      this.twNavigation.Text = "Navigation";
      // 
      // barManager1
      // 
      editUndoBarButtonCommand.Enabled = false;
      editUndoBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("editUndoBarButtonCommand.Image")));
      editUndoBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131162, false)});
      editRedoBarButtonCommand.Enabled = false;
      editRedoBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("editRedoBarButtonCommand.Image")));
      editRedoBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131161, false)});
      editCutBarButtonCommand.Enabled = false;
      editCutBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("editCutBarButtonCommand.Image")));
      editCutBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131160, false)});
      editCopyBarButtonCommand.Enabled = false;
      editCopyBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("editCopyBarButtonCommand.Image")));
      editCopyBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131139, false)});
      editPasteBarButtonCommand.Enabled = false;
      editPasteBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("editPasteBarButtonCommand.Image")));
      editPasteBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131158, false)});
      editDeleteBarButtonCommand.Enabled = false;
      editDeleteBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("editDeleteBarButtonCommand.Image")));
      editDeleteBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 46, false)});
      editSelectAllBarButtonCommand.Enabled = false;
      editSelectAllBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131137, false)});
      editFindBarButtonCommand.Enabled = false;
      editFindBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("editFindBarButtonCommand.Image")));
      editFindBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131142)});
      toolsCustomizeBarButtonCommand.Enabled = false;
      toolsOptionsBarButtonCommand.Enabled = false;
      windowWindowListBarExpanderButtonCommand.PopulationStyle = ActiproSoftware.UIStudio.Bar.BarExpanderButtonCommandPopulationStyle.StandardMdiWindowList;
      helpContentsBarButtonCommand.Enabled = false;
      helpContentsBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 393328)});
      helpIndexBarButtonCommand.Enabled = false;
      helpIndexBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 393329)});
      helpSearchBarButtonCommand.Enabled = false;
      helpSearchBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 393330)});
      helpAboutBarButtonCommand.Enabled = false;
      viewZoomInBarButtonCommand.Enabled = false;
      viewZoomInBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131259)});
      viewZoomOutBarButtonCommand.Enabled = false;
      viewZoomOutBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131261)});
      viewZoomToVisibleBarButtonCommand.Enabled = false;
      viewZoomToSelectedBarButtonCommand.Enabled = false;
      fileNewBarButtonCommand.Enabled = false;
      fileNewBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("fileNewBarButtonCommand.Image")));
      fileNewBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131150)});
      fileOpenBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("fileOpenBarButtonCommand.Image")));
      fileOpenBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131151)});
      fileCloseBarButtonCommand.Enabled = false;
      fileCloseBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131159)});
      fileSaveBarButtonCommand.Enabled = false;
      fileSaveBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("fileSaveBarButtonCommand.Image")));
      fileSaveBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131155)});
      fileSaveAsBarButtonCommand.Enabled = false;
      fileSaveAllBarButtonCommand.Enabled = false;
      filePrintBarButtonCommand.Enabled = false;
      filePrintBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("filePrintBarButtonCommand.Image")));
      filePrintBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131152)});
      filePrintPreviewBarButtonCommand.Enabled = false;
      filePrintPreviewBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("filePrintPreviewBarButtonCommand.Image")));
      newItemModelTypeBarComboBoxCommand.Items.Add("Tank1");
      newItemModelTypeBarComboBoxCommand.Items.Add("Tank2");
      newItemModelTypeBarComboBoxCommand.Items.Add("Tank3");
      newItemModelTypeBarComboBoxCommand.Style = ActiproSoftware.UIStudio.Bar.BarComboBoxCommandStyle.DropDownList;
      newItemGraphicTypeBarComboBoxCommand.Items.Add("Tie");
      newItemGraphicTypeBarComboBoxCommand.Items.Add("Tank");
      newItemGraphicTypeBarComboBoxCommand.Style = ActiproSoftware.UIStudio.Bar.BarComboBoxCommandStyle.DropDownList;
      this.barManager1.Commands.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommand[] {
            editUndoBarButtonCommand,
            editRedoBarButtonCommand,
            editCutBarButtonCommand,
            editCopyBarButtonCommand,
            editPasteBarButtonCommand,
            editDeleteBarButtonCommand,
            editSelectAllBarButtonCommand,
            editFindBarButtonCommand,
            toolsCustomizeBarButtonCommand,
            toolsOptionsBarButtonCommand,
            windowWindowListBarExpanderButtonCommand,
            helpContentsBarButtonCommand,
            helpIndexBarButtonCommand,
            helpSearchBarButtonCommand,
            helpAboutBarButtonCommand,
            viewZoomInBarButtonCommand,
            viewZoomOutBarButtonCommand,
            viewZoomToVisibleBarButtonCommand,
            viewZoomToSelectedBarButtonCommand,
            fileNewBarButtonCommand,
            fileOpenBarButtonCommand,
            fileCloseBarButtonCommand,
            fileSaveBarButtonCommand,
            fileSaveAsBarButtonCommand,
            fileSaveAllBarButtonCommand,
            filePrintBarButtonCommand,
            filePrintPreviewBarButtonCommand,
            fileExitBarButtonCommand,
            newMenuNewMenuBarPopupButtonCommand,
            newItemModelTypeBarComboBoxCommand,
            newItemGraphicTypeBarComboBoxCommand,
            newItemNetItemDragTargetBarButtonCommand});
      mainMenuMenuBar.Active = true;
      fileSaveBarButtonLink.BeginAGroup = true;
      filePrintBarButtonLink.BeginAGroup = true;
      fileExitBarButtonLink.BeginAGroup = true;
      newMenuNewMenuBarPopupButtonLink.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            fileNewBarButtonLink,
            fileOpenBarButtonLink,
            fileCloseBarButtonLink,
            fileSaveBarButtonLink,
            fileSaveAsBarButtonLink,
            fileSaveAllBarButtonLink,
            filePrintBarButtonLink,
            filePrintPreviewBarButtonLink,
            fileExitBarButtonLink});
      newMenuNewMenuBarPopupButtonLink.DefaultText = "&Project";
      newMenuNewMenuBarPopupButtonLink.Text = "&Project";
      editCutBarButtonLink.BeginAGroup = true;
      editSelectAllBarButtonLink.BeginAGroup = true;
      editFindBarButtonLink.BeginAGroup = true;
      newMenuNewMenuBarPopupButtonLink2.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            editUndoBarButtonLink,
            editRedoBarButtonLink,
            editCutBarButtonLink,
            editCopyBarButtonLink,
            editPasteBarButtonLink,
            editDeleteBarButtonLink,
            editSelectAllBarButtonLink,
            editFindBarButtonLink});
      newMenuNewMenuBarPopupButtonLink2.DefaultText = "&Edit";
      newMenuNewMenuBarPopupButtonLink2.Text = "&Edit";
      newMenuNewMenuBarPopupButtonLink3.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            viewZoomInBarButtonLink,
            viewZoomOutBarButtonLink,
            viewZoomToVisibleBarButtonLink,
            viewZoomToSelectedBarButtonLink});
      newMenuNewMenuBarPopupButtonLink3.DefaultText = "&View";
      newMenuNewMenuBarPopupButtonLink3.Text = "&View";
      newMenuNewMenuBarPopupButtonLink4.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            toolsCustomizeBarButtonLink,
            toolsOptionsBarButtonLink});
      newMenuNewMenuBarPopupButtonLink4.DefaultText = "&Tools";
      newMenuNewMenuBarPopupButtonLink4.Text = "&Tools";
      newMenuNewMenuBarPopupButtonLink5.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            windowWindowListBarExpanderButtonLink});
      newMenuNewMenuBarPopupButtonLink5.DefaultText = "&Window";
      newMenuNewMenuBarPopupButtonLink5.Text = "&Window";
      helpAboutBarButtonLink.BeginAGroup = true;
      newMenuNewMenuBarPopupButtonLink6.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            helpContentsBarButtonLink,
            helpIndexBarButtonLink,
            helpSearchBarButtonLink,
            helpAboutBarButtonLink});
      newMenuNewMenuBarPopupButtonLink6.DefaultText = "&Help";
      newMenuNewMenuBarPopupButtonLink6.Text = "&Help";
      mainMenuMenuBar.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            newMenuNewMenuBarPopupButtonLink,
            newMenuNewMenuBarPopupButtonLink2,
            newMenuNewMenuBarPopupButtonLink3,
            newMenuNewMenuBarPopupButtonLink4,
            newMenuNewMenuBarPopupButtonLink5,
            newMenuNewMenuBarPopupButtonLink6});
      fileSaveBarButtonLink2.BeginAGroup = true;
      filePrintBarButtonLink2.BeginAGroup = true;
      fileExitBarButtonLink2.BeginAGroup = true;
      newMenuNewMenuBarPopupButtonLink7.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            fileNewBarButtonLink2,
            fileOpenBarButtonLink2,
            fileCloseBarButtonLink2,
            fileSaveBarButtonLink2,
            fileSaveAsBarButtonLink2,
            fileSaveAllBarButtonLink2,
            filePrintBarButtonLink2,
            filePrintPreviewBarButtonLink2,
            fileExitBarButtonLink2});
      newMenuNewMenuBarPopupButtonLink7.DefaultText = "&Project";
      newMenuNewMenuBarPopupButtonLink7.Text = "&Project";
      editCutBarButtonLink2.BeginAGroup = true;
      editSelectAllBarButtonLink2.BeginAGroup = true;
      editFindBarButtonLink2.BeginAGroup = true;
      newMenuNewMenuBarPopupButtonLink8.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            editUndoBarButtonLink2,
            editRedoBarButtonLink2,
            editCutBarButtonLink2,
            editCopyBarButtonLink2,
            editPasteBarButtonLink2,
            editDeleteBarButtonLink2,
            editSelectAllBarButtonLink2,
            editFindBarButtonLink2});
      newMenuNewMenuBarPopupButtonLink8.DefaultText = "&Edit";
      newMenuNewMenuBarPopupButtonLink8.Text = "&Edit";
      newMenuNewMenuBarPopupButtonLink9.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            viewZoomInBarButtonLink2,
            viewZoomOutBarButtonLink2,
            viewZoomToVisibleBarButtonLink2,
            viewZoomToSelectedBarButtonLink2});
      newMenuNewMenuBarPopupButtonLink9.DefaultText = "&View";
      newMenuNewMenuBarPopupButtonLink9.Text = "&View";
      newMenuNewMenuBarPopupButtonLink10.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            toolsCustomizeBarButtonLink2,
            toolsOptionsBarButtonLink2});
      newMenuNewMenuBarPopupButtonLink10.DefaultText = "&Tools";
      newMenuNewMenuBarPopupButtonLink10.Text = "&Tools";
      newMenuNewMenuBarPopupButtonLink11.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            windowWindowListBarExpanderButtonLink2});
      newMenuNewMenuBarPopupButtonLink11.DefaultText = "&Window";
      newMenuNewMenuBarPopupButtonLink11.Text = "&Window";
      helpAboutBarButtonLink2.BeginAGroup = true;
      newMenuNewMenuBarPopupButtonLink12.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            helpContentsBarButtonLink2,
            helpIndexBarButtonLink2,
            helpSearchBarButtonLink2,
            helpAboutBarButtonLink2});
      newMenuNewMenuBarPopupButtonLink12.DefaultText = "&Help";
      newMenuNewMenuBarPopupButtonLink12.Text = "&Help";
      mainMenuMenuBar.DefaultCommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            newMenuNewMenuBarPopupButtonLink7,
            newMenuNewMenuBarPopupButtonLink8,
            newMenuNewMenuBarPopupButtonLink9,
            newMenuNewMenuBarPopupButtonLink10,
            newMenuNewMenuBarPopupButtonLink11,
            newMenuNewMenuBarPopupButtonLink12});
      mainMenuMenuBar.DockedRow = 0;
      newItemDockableToolBar.Active = true;
      newItemNetItemDragTargetBarButtonLink.DefaultText = " v ";
      newItemNetItemDragTargetBarButtonLink.Text = " v ";
      newItemDockableToolBar.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            newItemGraphicTypeBarComboBoxLink,
            newItemModelTypeBarComboBoxLink,
            newItemNetItemDragTargetBarButtonLink});
      newItemNetItemDragTargetBarButtonLink2.Text = " v ";
      newItemDockableToolBar.DefaultCommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            newItemGraphicTypeBarComboBoxLink2,
            newItemModelTypeBarComboBoxLink2,
            newItemNetItemDragTargetBarButtonLink2});
      newItemDockableToolBar.DockedColumn = 282;
      newItemDockableToolBar.DockedRow = 1;
      standardDockableToolBar.Active = true;
      editCutBarButtonLink3.BeginAGroup = true;
      editUndoBarButtonLink3.BeginAGroup = true;
      editFindBarButtonLink3.BeginAGroup = true;
      standardDockableToolBar.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            editCutBarButtonLink3,
            editCopyBarButtonLink3,
            editPasteBarButtonLink3,
            editDeleteBarButtonLink3,
            editUndoBarButtonLink3,
            editRedoBarButtonLink3,
            editFindBarButtonLink3});
      editCutBarButtonLink4.BeginAGroup = true;
      editUndoBarButtonLink4.BeginAGroup = true;
      editFindBarButtonLink4.BeginAGroup = true;
      standardDockableToolBar.DefaultCommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            editCutBarButtonLink4,
            editCopyBarButtonLink4,
            editPasteBarButtonLink4,
            editDeleteBarButtonLink4,
            editUndoBarButtonLink4,
            editRedoBarButtonLink4,
            editFindBarButtonLink4});
      standardDockableToolBar.DockedRow = 1;
      this.barManager1.DockableToolBars.Add(mainMenuMenuBar);
      this.barManager1.DockableToolBars.Add(newItemDockableToolBar);
      this.barManager1.DockableToolBars.Add(standardDockableToolBar);
      this.barManager1.HostContainerControl = this;
      this.barManager1.CommandClick += new ActiproSoftware.UIStudio.Bar.BarCommandLinkEventHandler(this.barManager1_CommandClick);
      // 
      // barDockArea1
      // 
      this.barDockArea1.BarManager = this.barManager1;
      this.barDockArea1.Dock = System.Windows.Forms.DockStyle.Left;
      this.barDockArea1.Location = new System.Drawing.Point(0, 56);
      this.barDockArea1.Name = "barDockArea1";
      this.barDockArea1.Size = new System.Drawing.Size(0, 635);
      this.barDockArea1.TabIndex = 2;
      // 
      // barDockArea2
      // 
      this.barDockArea2.BarManager = this.barManager1;
      this.barDockArea2.Dock = System.Windows.Forms.DockStyle.Top;
      this.barDockArea2.Location = new System.Drawing.Point(0, 0);
      this.barDockArea2.Name = "barDockArea2";
      this.barDockArea2.Size = new System.Drawing.Size(1111, 56);
      this.barDockArea2.TabIndex = 3;
      // 
      // barDockArea3
      // 
      this.barDockArea3.BarManager = this.barManager1;
      this.barDockArea3.Dock = System.Windows.Forms.DockStyle.Right;
      this.barDockArea3.Location = new System.Drawing.Point(1111, 56);
      this.barDockArea3.Name = "barDockArea3";
      this.barDockArea3.Size = new System.Drawing.Size(0, 635);
      this.barDockArea3.TabIndex = 4;
      // 
      // barDockArea4
      // 
      this.barDockArea4.BarManager = this.barManager1;
      this.barDockArea4.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.barDockArea4.Location = new System.Drawing.Point(0, 691);
      this.barDockArea4.Name = "barDockArea4";
      this.barDockArea4.Size = new System.Drawing.Size(1111, 0);
      this.barDockArea4.TabIndex = 5;
      // 
      // toolWindowContainer1
      // 
      this.toolWindowContainer1.Controls.Add(this.twOverview);
      this.toolWindowContainer1.DockManager = this.dockManager1;
      this.toolWindowContainer1.Location = new System.Drawing.Point(0, 0);
      this.toolWindowContainer1.Name = "toolWindowContainer1";
      this.toolWindowContainer1.Size = new System.Drawing.Size(172, 168);
      this.toolWindowContainer1.TabIndex = 8;
      // 
      // toolWindowContainer2
      // 
      this.toolWindowContainer2.Controls.Add(this.twNavigation);
      this.toolWindowContainer2.DockManager = this.dockManager1;
      this.toolWindowContainer2.Location = new System.Drawing.Point(0, 172);
      this.toolWindowContainer2.Name = "toolWindowContainer2";
      this.toolWindowContainer2.Size = new System.Drawing.Size(172, 463);
      this.toolWindowContainer2.TabIndex = 9;
      // 
      // dockContainerContainer1
      // 
      this.dockContainerContainer1.Controls.Add(this.toolWindowContainer1);
      this.dockContainerContainer1.Controls.Add(this.toolWindowContainer2);
      this.dockContainerContainer1.Dock = System.Windows.Forms.DockStyle.Left;
      this.dockContainerContainer1.DockManager = this.dockManager1;
      this.dockContainerContainer1.Location = new System.Drawing.Point(0, 56);
      this.dockContainerContainer1.Name = "dockContainerContainer1";
      this.dockContainerContainer1.Orientation = System.Windows.Forms.Orientation.Vertical;
      this.dockContainerContainer1.SelectedToolWindow = null;
      this.dockContainerContainer1.Size = new System.Drawing.Size(176, 635);
      this.dockContainerContainer1.TabIndex = 8;
      // 
      // twStencils
      // 
      this.twStencils.Controls.Add(this.tbStencils);
      this.twStencils.Dock = System.Windows.Forms.DockStyle.Fill;
      this.twStencils.DockManager = this.dockManager1;
      this.twStencils.Location = new System.Drawing.Point(0, 29);
      this.twStencils.Name = "twStencils";
      this.twStencils.Size = new System.Drawing.Size(935, 44);
      this.twStencils.TabIndex = 0;
      this.twStencils.Text = "Stencils";
      this.twStencils.TitleBarText = "Stencils";
      // 
      // tbStencils
      // 
      this.tbStencils.AllowDrag = true;
      this.tbStencils.Document = null;
      this.tbStencils.DropDownArrows = true;
      this.tbStencils.Location = new System.Drawing.Point(0, 0);
      this.tbStencils.Name = "tbStencils";
      this.tbStencils.Shapes = null;
      this.tbStencils.ShowToolTips = true;
      this.tbStencils.Size = new System.Drawing.Size(935, 42);
      this.tbStencils.TabIndex = 0;
      // 
      // toolWindowContainer3
      // 
      this.toolWindowContainer3.Controls.Add(this.twStencils);
      this.toolWindowContainer3.Dock = System.Windows.Forms.DockStyle.Top;
      this.toolWindowContainer3.DockManager = this.dockManager1;
      this.toolWindowContainer3.Location = new System.Drawing.Point(176, 56);
      this.toolWindowContainer3.Name = "toolWindowContainer3";
      this.toolWindowContainer3.Size = new System.Drawing.Size(935, 77);
      this.toolWindowContainer3.TabIndex = 10;
      // 
      // twGraphicProperties
      // 
      this.twGraphicProperties.AccessibleName = "";
      this.twGraphicProperties.Controls.Add(this.propertyGrid1);
      this.twGraphicProperties.Dock = System.Windows.Forms.DockStyle.Fill;
      this.twGraphicProperties.DockManager = this.dockManager1;
      this.twGraphicProperties.Location = new System.Drawing.Point(4, 29);
      this.twGraphicProperties.Name = "twGraphicProperties";
      this.twGraphicProperties.Size = new System.Drawing.Size(200, 529);
      this.twGraphicProperties.TabIndex = 0;
      this.twGraphicProperties.Text = "Graphic Properties";
      // 
      // propertyGrid1
      // 
      this.propertyGrid1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.propertyGrid1.Location = new System.Drawing.Point(0, 0);
      this.propertyGrid1.Name = "propertyGrid1";
      this.propertyGrid1.Size = new System.Drawing.Size(200, 529);
      this.propertyGrid1.TabIndex = 0;
      // 
      // toolWindowContainer4
      // 
      this.toolWindowContainer4.Controls.Add(this.twGraphicProperties);
      this.toolWindowContainer4.Dock = System.Windows.Forms.DockStyle.Right;
      this.toolWindowContainer4.DockManager = this.dockManager1;
      this.toolWindowContainer4.Location = new System.Drawing.Point(907, 133);
      this.toolWindowContainer4.Name = "toolWindowContainer4";
      this.toolWindowContainer4.Size = new System.Drawing.Size(204, 558);
      this.toolWindowContainer4.TabIndex = 12;
      // 
      // Form1
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(1111, 691);
      this.Controls.Add(this.toolWindowContainer4);
      this.Controls.Add(this.toolWindowContainer3);
      this.Controls.Add(this.dockContainerContainer1);
      this.Controls.Add(this.barDockArea3);
      this.Controls.Add(this.barDockArea1);
      this.Controls.Add(this.barDockArea4);
      this.Controls.Add(this.barDockArea2);
      this.IsMdiContainer = true;
      this.Name = "Form1";
      this.Text = "SysCAD.Editor";
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
      ((System.ComponentModel.ISupportInitialize)(this.dockManager1)).EndInit();
      this.twOverview.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.tvNavigation)).EndInit();
      this.twNavigation.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.barManager1)).EndInit();
      this.toolWindowContainer1.ResumeLayout(false);
      this.toolWindowContainer2.ResumeLayout(false);
      this.dockContainerContainer1.ResumeLayout(false);
      this.twStencils.ResumeLayout(false);
      this.twStencils.PerformLayout();
      this.toolWindowContainer3.ResumeLayout(false);
      this.twGraphicProperties.ResumeLayout(false);
      this.toolWindowContainer4.ResumeLayout(false);
      this.ResumeLayout(false);

    }

    #endregion

    private ActiproSoftware.UIStudio.Dock.DockManager dockManager1;
    private ActiproSoftware.UIStudio.Dock.ToolWindow twNavigation;
    private ActiproSoftware.UIStudio.Dock.ToolWindow twOverview;
    private Overview ovOverview;
    private ActiproSoftware.UIStudio.Bar.BarDockArea barDockArea3;
    private ActiproSoftware.UIStudio.Bar.BarManager barManager1;
    private ActiproSoftware.UIStudio.Bar.BarDockArea barDockArea1;
    private ActiproSoftware.UIStudio.Bar.BarDockArea barDockArea4;
    private ActiproSoftware.UIStudio.Bar.BarDockArea barDockArea2;
    private ActiproSoftware.UIStudio.Dock.DockContainerContainer dockContainerContainer1;
    private ActiproSoftware.UIStudio.Dock.ToolWindowContainer toolWindowContainer1;
    private ActiproSoftware.UIStudio.Dock.ToolWindowContainer toolWindowContainer2;
    private ActiproSoftware.UIStudio.Dock.ToolWindowContainer toolWindowContainer3;
    private ActiproSoftware.UIStudio.Dock.ToolWindow twStencils;
    private ShapeToolBar tbStencils;
    private PureComponents.TreeView.TreeView tvNavigation;
    private ActiproSoftware.UIStudio.Dock.ToolWindowContainer toolWindowContainer4;
    private ActiproSoftware.UIStudio.Dock.ToolWindow twGraphicProperties;
    private System.Windows.Forms.PropertyGrid propertyGrid1;
  }
}

