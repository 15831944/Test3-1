using MindFusion.FlowChartX;
using System.Windows.Forms;
namespace SysCAD.Editor
{
  partial class EditorForm
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    //protected override void Dispose(bool disposing)
    //{
    //  if (disposing && (components != null))
    //  {
    //    components.Dispose();
    //  }
    //  base.Dispose(disposing);
    //}

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(EditorForm));
      PureComponents.TreeView.ContextMenuStrings contextMenuStrings1 = new PureComponents.TreeView.ContextMenuStrings();
      PureComponents.TreeView.TreeViewStyle treeViewStyle1 = new PureComponents.TreeView.TreeViewStyle();
      PureComponents.TreeView.NodeStyle nodeStyle1 = new PureComponents.TreeView.NodeStyle();
      PureComponents.TreeView.CheckBoxStyle checkBoxStyle1 = new PureComponents.TreeView.CheckBoxStyle();
      PureComponents.TreeView.ExpandBoxStyle expandBoxStyle1 = new PureComponents.TreeView.ExpandBoxStyle();
      PureComponents.TreeView.NodeTooltipStyle nodeTooltipStyle1 = new PureComponents.TreeView.NodeTooltipStyle();
      ActiproSoftware.UIStudio.Bar.BarButtonCommand editUndoBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("Edit", "Undo", "&Undo", -1);
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
      ActiproSoftware.UIStudio.Bar.BarButtonCommand fileOpenBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("File", "Open", "Attach...", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand fileCloseBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("File", "Close", "&Close", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand fileSaveBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("File", "Save", "&Save", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand fileSaveAsBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("File", "SaveAs", "Save &As...", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand fileSaveAllBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("File", "SaveAll", "Save A&ll", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand filePrintBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("File", "Print", "&Print", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand filePrintPreviewBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("File", "PrintPreview", "Print Pre&view", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand fileExitBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("File", "Exit", "E&xit", -1);
      ActiproSoftware.UIStudio.Bar.BarPopupButtonCommand newMenuNewMenuBarPopupButtonCommand = new ActiproSoftware.UIStudio.Bar.BarPopupButtonCommand("New Menu", "New Menu", "New Menu", -1);
      ActiproSoftware.UIStudio.Bar.BarComboBoxCommand createItemModelTypeBarComboBoxCommand = new ActiproSoftware.UIStudio.Bar.BarComboBoxCommand("CreateItem", "ModelType", "ModelType", -1);
      ActiproSoftware.UIStudio.Bar.BarComboBoxCommand createItemGraphicTypeBarComboBoxCommand = new ActiproSoftware.UIStudio.Bar.BarComboBoxCommand("CreateItem", "GraphicType", "GraphicType", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand viewShowGraphicsBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("View", "ShowGraphics", "Show Graphics", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand viewShowModelsBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("View", "ShowModels", "Show Models", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand viewShowLinksBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("View", "ShowLinks", "Show Links", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand selectionSelectItemsBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("Selection", "SelectItems", "Select Nodes", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand selectionSelectLinksBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("Selection", "SelectLinks", "Select Links", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand modeModifyBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("Mode", "Modify", "Modify", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand modeCreateLinkBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("Mode", "CreateLink", "Create Link", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand viewShowTagsBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("View", "ShowTags", "Show Tags", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand viewShowAreasBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("View", "ShowAreas", "Show Areas", -1);
      ActiproSoftware.UIStudio.Bar.BarButtonCommand toolsUnlockDemoBarButtonCommand = new ActiproSoftware.UIStudio.Bar.BarButtonCommand("Tools", "Unlock(Demo)", "Unlock (Demo)", -1);
      ActiproSoftware.UIStudio.Bar.DockableToolBar controlDockableToolBar = new ActiproSoftware.UIStudio.Bar.DockableToolBar("Control");
      ActiproSoftware.UIStudio.Bar.BarButtonLink modeModifyBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(modeModifyBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink modeCreateLinkBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(modeCreateLinkBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink modeModifyBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(modeModifyBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink modeCreateLinkBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(modeCreateLinkBarButtonCommand);
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
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewShowModelsBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewShowModelsBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewShowGraphicsBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewShowGraphicsBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewShowLinksBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewShowLinksBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarPopupButtonLink newMenuNewMenuBarPopupButtonLink4 = new ActiproSoftware.UIStudio.Bar.BarPopupButtonLink(newMenuNewMenuBarPopupButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink toolsUnlockDemoBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(toolsUnlockDemoBarButtonCommand);
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
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewShowModelsBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewShowModelsBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewShowGraphicsBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewShowGraphicsBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewShowLinksBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewShowLinksBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarPopupButtonLink newMenuNewMenuBarPopupButtonLink10 = new ActiproSoftware.UIStudio.Bar.BarPopupButtonLink(newMenuNewMenuBarPopupButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink toolsUnlockDemoBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(toolsUnlockDemoBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink toolsCustomizeBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(toolsCustomizeBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink toolsOptionsBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(toolsOptionsBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarPopupButtonLink newMenuNewMenuBarPopupButtonLink11 = new ActiproSoftware.UIStudio.Bar.BarPopupButtonLink(newMenuNewMenuBarPopupButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarExpanderButtonLink windowWindowListBarExpanderButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarExpanderButtonLink(windowWindowListBarExpanderButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarPopupButtonLink newMenuNewMenuBarPopupButtonLink12 = new ActiproSoftware.UIStudio.Bar.BarPopupButtonLink(newMenuNewMenuBarPopupButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink helpContentsBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(helpContentsBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink helpIndexBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(helpIndexBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink helpSearchBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(helpSearchBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink helpAboutBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(helpAboutBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.DockableToolBar selectionControlDockableToolBar = new ActiproSoftware.UIStudio.Bar.DockableToolBar("SelectionControl");
      ActiproSoftware.UIStudio.Bar.BarButtonLink selectionSelectLinksBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(selectionSelectLinksBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink selectionSelectItemsBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(selectionSelectItemsBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink selectionSelectLinksBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(selectionSelectLinksBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink selectionSelectItemsBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(selectionSelectItemsBarButtonCommand);
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
      ActiproSoftware.UIStudio.Bar.DockableToolBar viewDockableToolBar = new ActiproSoftware.UIStudio.Bar.DockableToolBar("View");
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewShowAreasBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewShowAreasBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewShowModelsBarButtonLink3 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewShowModelsBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewShowGraphicsBarButtonLink3 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewShowGraphicsBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewShowLinksBarButtonLink3 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewShowLinksBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewShowTagsBarButtonLink = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewShowTagsBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewShowAreasBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewShowAreasBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewShowModelsBarButtonLink4 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewShowModelsBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewShowGraphicsBarButtonLink4 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewShowGraphicsBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewShowLinksBarButtonLink4 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewShowLinksBarButtonCommand);
      ActiproSoftware.UIStudio.Bar.BarButtonLink viewShowTagsBarButtonLink2 = new ActiproSoftware.UIStudio.Bar.BarButtonLink(viewShowTagsBarButtonCommand);
      this.dockManager1 = new ActiproSoftware.UIStudio.Dock.DockManager();
      this.ovOverview = new MindFusion.FlowChartX.Overview();
      this.fcHoverview = new MindFusion.FlowChartX.FlowChart();
      this.twOverview = new ActiproSoftware.UIStudio.Dock.ToolWindow();
      this.twHoverview = new ActiproSoftware.UIStudio.Dock.ToolWindow();
      this.splitContainer1 = new System.Windows.Forms.SplitContainer();
      this.listView1 = new System.Windows.Forms.ListView();
      this.NavigationTreeView = new PureComponents.TreeView.TreeView();
      this.twNavigation = new ActiproSoftware.UIStudio.Dock.ToolWindow();
      this.barManager1 = new ActiproSoftware.UIStudio.Bar.BarManager();
      this.barDockArea1 = new ActiproSoftware.UIStudio.Bar.BarDockArea();
      this.barDockArea2 = new ActiproSoftware.UIStudio.Bar.BarDockArea();
      this.barDockArea3 = new ActiproSoftware.UIStudio.Bar.BarDockArea();
      this.barDockArea4 = new ActiproSoftware.UIStudio.Bar.BarDockArea();
      this.toolWindowContainer1 = new ActiproSoftware.UIStudio.Dock.ToolWindowContainer();
      this.GraphicPropertiesToolWindow = new ActiproSoftware.UIStudio.Dock.ToolWindow();
      this.graphicPropertyGrid = new SysCAD.Editor.GraphicPropertyGrid();
      this.toolWindowContainer4 = new ActiproSoftware.UIStudio.Dock.ToolWindowContainer();
      this.ModelPropertiesToolWindow = new ActiproSoftware.UIStudio.Dock.ToolWindow();
      this.modelPropertiesGrid = new SysCAD.Editor.ModelPropertyGrid();
      this.itemToolWindow = new ActiproSoftware.UIStudio.Dock.ToolWindow();
      this.stencilChooser1 = new SysCAD.StencilChooser();
      this.statusStrip = new System.Windows.Forms.StatusStrip();
      this.toolStripStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
      this.dockContainerContainer1 = new ActiproSoftware.UIStudio.Dock.DockContainerContainer();
      this.toolWindowContainer3 = new ActiproSoftware.UIStudio.Dock.ToolWindowContainer();
      this.toolWindowContainer2 = new ActiproSoftware.UIStudio.Dock.ToolWindowContainer();
      this.dockContainerContainer2 = new ActiproSoftware.UIStudio.Dock.DockContainerContainer();
      ((System.ComponentModel.ISupportInitialize)(this.dockManager1)).BeginInit();
      this.twOverview.SuspendLayout();
      this.twHoverview.SuspendLayout();
      this.splitContainer1.Panel1.SuspendLayout();
      this.splitContainer1.Panel2.SuspendLayout();
      this.splitContainer1.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.NavigationTreeView)).BeginInit();
      this.twNavigation.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.barManager1)).BeginInit();
      this.toolWindowContainer1.SuspendLayout();
      this.GraphicPropertiesToolWindow.SuspendLayout();
      this.ModelPropertiesToolWindow.SuspendLayout();
      this.itemToolWindow.SuspendLayout();
      this.statusStrip.SuspendLayout();
      this.dockContainerContainer1.SuspendLayout();
      this.toolWindowContainer3.SuspendLayout();
      this.toolWindowContainer2.SuspendLayout();
      this.dockContainerContainer2.SuspendLayout();
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
      this.ovOverview.DisplayText = false;
      this.ovOverview.Dock = System.Windows.Forms.DockStyle.Fill;
      this.ovOverview.Document = null;
      this.ovOverview.FitAll = true;
      this.ovOverview.Location = new System.Drawing.Point(0, 0);
      this.ovOverview.Margin = new System.Windows.Forms.Padding(2);
      this.ovOverview.Name = "ovOverview";
      this.ovOverview.Size = new System.Drawing.Size(131, 111);
      this.ovOverview.TabIndex = 0;
      // 
      // fcHoverview
      // 
      this.fcHoverview.ArrowHandlesStyle = MindFusion.FlowChartX.HandlesStyle.Invisible;
      this.fcHoverview.ArrowHead = MindFusion.FlowChartX.ArrowHead.None;
      this.fcHoverview.BoxHandlesStyle = MindFusion.FlowChartX.HandlesStyle.Invisible;
      this.fcHoverview.CurrentCursor = System.Windows.Forms.Cursors.Default;
      this.fcHoverview.Cursor = System.Windows.Forms.Cursors.Default;
      this.fcHoverview.DefaultControlType = typeof(System.Windows.Forms.Button);
      this.fcHoverview.DocExtents = ((System.Drawing.RectangleF)(resources.GetObject("fcHoverview.DocExtents")));
      this.fcHoverview.Dock = System.Windows.Forms.DockStyle.Fill;
      this.fcHoverview.Enabled = false;
      this.fcHoverview.InplaceEditFont = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.fcHoverview.Location = new System.Drawing.Point(0, 0);
      this.fcHoverview.Margin = new System.Windows.Forms.Padding(2);
      this.fcHoverview.Name = "fcHoverview";
      this.fcHoverview.ShadowsStyle = MindFusion.FlowChartX.ShadowsStyle.None;
      this.fcHoverview.ShowAnchors = MindFusion.FlowChartX.ShowAnchors.Always;
      this.fcHoverview.ShowDisabledHandles = false;
      this.fcHoverview.Size = new System.Drawing.Size(149, 92);
      this.fcHoverview.TabIndex = 0;
      // 
      // twOverview
      // 
      this.twOverview.Controls.Add(this.ovOverview);
      this.twOverview.Dock = System.Windows.Forms.DockStyle.Fill;
      this.twOverview.DockManager = this.dockManager1;
      this.twOverview.Key = "OverviewToolWindow";
      this.twOverview.Location = new System.Drawing.Point(0, 25);
      this.twOverview.Margin = new System.Windows.Forms.Padding(2);
      this.twOverview.Name = "twOverview";
      this.twOverview.Size = new System.Drawing.Size(131, 111);
      this.twOverview.TabIndex = 0;
      this.twOverview.Text = "Overview";
      // 
      // twHoverview
      // 
      this.twHoverview.Controls.Add(this.splitContainer1);
      this.twHoverview.Dock = System.Windows.Forms.DockStyle.Fill;
      this.twHoverview.DockManager = this.dockManager1;
      this.twHoverview.Key = "HoverviewToolWindow";
      this.twHoverview.Location = new System.Drawing.Point(0, 25);
      this.twHoverview.Margin = new System.Windows.Forms.Padding(2);
      this.twHoverview.Name = "twHoverview";
      this.twHoverview.Size = new System.Drawing.Size(149, 214);
      this.twHoverview.TabIndex = 0;
      this.twHoverview.Text = "Hoverview";
      // 
      // splitContainer1
      // 
      this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.splitContainer1.Location = new System.Drawing.Point(0, 0);
      this.splitContainer1.Name = "splitContainer1";
      this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
      // 
      // splitContainer1.Panel1
      // 
      this.splitContainer1.Panel1.Controls.Add(this.fcHoverview);
      this.splitContainer1.Panel1MinSize = 20;
      // 
      // splitContainer1.Panel2
      // 
      this.splitContainer1.Panel2.Controls.Add(this.listView1);
      this.splitContainer1.Panel2MinSize = 20;
      this.splitContainer1.Size = new System.Drawing.Size(149, 214);
      this.splitContainer1.SplitterDistance = 92;
      this.splitContainer1.TabIndex = 0;
      // 
      // listView1
      // 
      this.listView1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.listView1.Location = new System.Drawing.Point(0, 0);
      this.listView1.Name = "listView1";
      this.listView1.Size = new System.Drawing.Size(149, 118);
      this.listView1.TabIndex = 0;
      this.listView1.UseCompatibleStateImageBehavior = false;
      // 
      // NavigationTreeView
      // 
      this.NavigationTreeView.AllowAdding = false;
      this.NavigationTreeView.AllowArranging = false;
      this.NavigationTreeView.AllowDeleting = false;
      this.NavigationTreeView.AllowEditing = false;
      this.NavigationTreeView.ContextMenuStrings = contextMenuStrings1;
      this.NavigationTreeView.Dock = System.Windows.Forms.DockStyle.Fill;
      this.NavigationTreeView.Location = new System.Drawing.Point(0, 0);
      this.NavigationTreeView.Margin = new System.Windows.Forms.Padding(2);
      this.NavigationTreeView.Multiline = false;
      this.NavigationTreeView.Name = "NavigationTreeView";
      this.NavigationTreeView.PathSeparator = "/";
      this.NavigationTreeView.SelectionMode = PureComponents.TreeView.SelectionMode.MultipleExtended;
      this.NavigationTreeView.Size = new System.Drawing.Size(131, 349);
      nodeStyle1.CheckBoxStyle = checkBoxStyle1;
      nodeStyle1.ExpandBoxStyle = expandBoxStyle1;
      nodeStyle1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
      nodeTooltipStyle1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
      nodeStyle1.TooltipStyle = nodeTooltipStyle1;
      treeViewStyle1.NodeStyle = nodeStyle1;
      this.NavigationTreeView.Style = treeViewStyle1;
      this.NavigationTreeView.TabIndex = 1;
      this.NavigationTreeView.Tooltips = false;
      // 
      // twNavigation
      // 
      this.twNavigation.Controls.Add(this.NavigationTreeView);
      this.twNavigation.Dock = System.Windows.Forms.DockStyle.Fill;
      this.twNavigation.DockManager = this.dockManager1;
      this.twNavigation.Key = "NavigationToolWindow";
      this.twNavigation.Location = new System.Drawing.Point(0, 25);
      this.twNavigation.Margin = new System.Windows.Forms.Padding(2);
      this.twNavigation.Name = "twNavigation";
      this.twNavigation.Size = new System.Drawing.Size(131, 349);
      this.twNavigation.TabIndex = 0;
      this.twNavigation.Text = "Navigation";
      this.twNavigation.Click += new System.EventHandler(this.twNavigation_Click);
      // 
      // barManager1
      // 
      editUndoBarButtonCommand.Enabled = false;
      editUndoBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("editUndoBarButtonCommand.Image")));
      editUndoBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131162)});
      editRedoBarButtonCommand.Enabled = false;
      editRedoBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("editRedoBarButtonCommand.Image")));
      editRedoBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131161),
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 196698)});
      editCutBarButtonCommand.Enabled = false;
      editCutBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("editCutBarButtonCommand.Image")));
      editCutBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131160, false)});
      editCopyBarButtonCommand.Enabled = false;
      editCopyBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("editCopyBarButtonCommand.Image")));
      editCopyBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131139),
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 196694)});
      editPasteBarButtonCommand.Enabled = false;
      editPasteBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("editPasteBarButtonCommand.Image")));
      editPasteBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131158)});
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
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131179)});
      viewZoomOutBarButtonCommand.Enabled = false;
      viewZoomOutBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131181)});
      viewZoomToVisibleBarButtonCommand.Enabled = false;
      viewZoomToSelectedBarButtonCommand.Enabled = false;
      viewZoomToSelectedBarButtonCommand.KeyboardShortcuts.AddRange(new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut[] {
            new ActiproSoftware.UIStudio.Bar.BarKeyboardShortcut(null, 0, 131178)});
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
      createItemModelTypeBarComboBoxCommand.Style = ActiproSoftware.UIStudio.Bar.BarComboBoxCommandStyle.DropDownList;
      createItemGraphicTypeBarComboBoxCommand.Style = ActiproSoftware.UIStudio.Bar.BarComboBoxCommandStyle.DropDownList;
      viewShowGraphicsBarButtonCommand.Checkable = true;
      viewShowGraphicsBarButtonCommand.Checked = true;
      viewShowGraphicsBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("viewShowGraphicsBarButtonCommand.Image")));
      viewShowModelsBarButtonCommand.Checkable = true;
      viewShowModelsBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("viewShowModelsBarButtonCommand.Image")));
      viewShowLinksBarButtonCommand.Checkable = true;
      viewShowLinksBarButtonCommand.Checked = true;
      viewShowLinksBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("viewShowLinksBarButtonCommand.Image")));
      selectionSelectItemsBarButtonCommand.Checkable = true;
      selectionSelectItemsBarButtonCommand.Checked = true;
      selectionSelectLinksBarButtonCommand.Checkable = true;
      selectionSelectLinksBarButtonCommand.Checked = true;
      modeModifyBarButtonCommand.Checkable = true;
      modeModifyBarButtonCommand.Checked = true;
      modeModifyBarButtonCommand.CheckGroupName = "ModeGroup";
      modeModifyBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("modeModifyBarButtonCommand.Image")));
      modeCreateLinkBarButtonCommand.Checkable = true;
      modeCreateLinkBarButtonCommand.CheckGroupName = "ModeGroup";
      modeCreateLinkBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("modeCreateLinkBarButtonCommand.Image")));
      viewShowTagsBarButtonCommand.Checkable = true;
      viewShowTagsBarButtonCommand.Checked = true;
      viewShowTagsBarButtonCommand.Image = ((System.Drawing.Image)(resources.GetObject("viewShowTagsBarButtonCommand.Image")));
      viewShowAreasBarButtonCommand.Checkable = true;
      viewShowAreasBarButtonCommand.Checked = true;
      toolsUnlockDemoBarButtonCommand.Enabled = false;
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
            createItemModelTypeBarComboBoxCommand,
            createItemGraphicTypeBarComboBoxCommand,
            viewShowGraphicsBarButtonCommand,
            viewShowModelsBarButtonCommand,
            viewShowLinksBarButtonCommand,
            selectionSelectItemsBarButtonCommand,
            selectionSelectLinksBarButtonCommand,
            modeModifyBarButtonCommand,
            modeCreateLinkBarButtonCommand,
            viewShowTagsBarButtonCommand,
            viewShowAreasBarButtonCommand,
            toolsUnlockDemoBarButtonCommand});
      controlDockableToolBar.Active = true;
      controlDockableToolBar.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            modeModifyBarButtonLink,
            modeCreateLinkBarButtonLink});
      controlDockableToolBar.DefaultCommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            modeModifyBarButtonLink2,
            modeCreateLinkBarButtonLink2});
      controlDockableToolBar.DockedColumn = 253;
      controlDockableToolBar.DockedRow = 1;
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
      viewShowModelsBarButtonLink.BeginAGroup = true;
      newMenuNewMenuBarPopupButtonLink3.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            viewZoomInBarButtonLink,
            viewZoomOutBarButtonLink,
            viewZoomToVisibleBarButtonLink,
            viewZoomToSelectedBarButtonLink,
            viewShowModelsBarButtonLink,
            viewShowGraphicsBarButtonLink,
            viewShowLinksBarButtonLink});
      newMenuNewMenuBarPopupButtonLink3.DefaultText = "&View";
      newMenuNewMenuBarPopupButtonLink3.Text = "&View";
      toolsCustomizeBarButtonLink.BeginAGroup = true;
      newMenuNewMenuBarPopupButtonLink4.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            toolsUnlockDemoBarButtonLink,
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
      viewShowModelsBarButtonLink2.BeginAGroup = true;
      newMenuNewMenuBarPopupButtonLink9.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            viewZoomInBarButtonLink2,
            viewZoomOutBarButtonLink2,
            viewZoomToVisibleBarButtonLink2,
            viewZoomToSelectedBarButtonLink2,
            viewShowModelsBarButtonLink2,
            viewShowGraphicsBarButtonLink2,
            viewShowLinksBarButtonLink2});
      newMenuNewMenuBarPopupButtonLink9.DefaultText = "&View";
      newMenuNewMenuBarPopupButtonLink9.Text = "&View";
      toolsCustomizeBarButtonLink2.BeginAGroup = true;
      newMenuNewMenuBarPopupButtonLink10.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            toolsUnlockDemoBarButtonLink2,
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
      selectionControlDockableToolBar.Active = true;
      selectionControlDockableToolBar.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            selectionSelectLinksBarButtonLink,
            selectionSelectItemsBarButtonLink});
      selectionControlDockableToolBar.DefaultCommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            selectionSelectLinksBarButtonLink2,
            selectionSelectItemsBarButtonLink2});
      selectionControlDockableToolBar.DockedColumn = 311;
      selectionControlDockableToolBar.DockedRow = 2;
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
      viewDockableToolBar.Active = true;
      viewShowLinksBarButtonLink3.BeginAGroup = true;
      viewDockableToolBar.CommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            viewShowAreasBarButtonLink,
            viewShowModelsBarButtonLink3,
            viewShowGraphicsBarButtonLink3,
            viewShowLinksBarButtonLink3,
            viewShowTagsBarButtonLink});
      viewShowLinksBarButtonLink4.BeginAGroup = true;
      viewDockableToolBar.DefaultCommandLinks.AddRange(new ActiproSoftware.UIStudio.Bar.BarCommandLink[] {
            viewShowAreasBarButtonLink2,
            viewShowModelsBarButtonLink4,
            viewShowGraphicsBarButtonLink4,
            viewShowLinksBarButtonLink4,
            viewShowTagsBarButtonLink2});
      viewDockableToolBar.DockedRow = 2;
      this.barManager1.DockableToolBars.Add(controlDockableToolBar);
      this.barManager1.DockableToolBars.Add(mainMenuMenuBar);
      this.barManager1.DockableToolBars.Add(selectionControlDockableToolBar);
      this.barManager1.DockableToolBars.Add(standardDockableToolBar);
      this.barManager1.DockableToolBars.Add(viewDockableToolBar);
      this.barManager1.HostContainerControl = this;
      this.barManager1.CommandClick += new ActiproSoftware.UIStudio.Bar.BarCommandLinkEventHandler(this.barManager1_CommandClick);
      // 
      // barDockArea1
      // 
      this.barDockArea1.BarManager = this.barManager1;
      this.barDockArea1.Dock = System.Windows.Forms.DockStyle.Left;
      this.barDockArea1.Location = new System.Drawing.Point(0, 76);
      this.barDockArea1.Margin = new System.Windows.Forms.Padding(2);
      this.barDockArea1.Name = "barDockArea1";
      this.barDockArea1.Size = new System.Drawing.Size(0, 514);
      this.barDockArea1.TabIndex = 2;
      // 
      // barDockArea2
      // 
      this.barDockArea2.BarManager = this.barManager1;
      this.barDockArea2.Dock = System.Windows.Forms.DockStyle.Top;
      this.barDockArea2.Location = new System.Drawing.Point(0, 0);
      this.barDockArea2.Margin = new System.Windows.Forms.Padding(2);
      this.barDockArea2.Name = "barDockArea2";
      this.barDockArea2.Size = new System.Drawing.Size(963, 76);
      this.barDockArea2.TabIndex = 3;
      // 
      // barDockArea3
      // 
      this.barDockArea3.BarManager = this.barManager1;
      this.barDockArea3.Dock = System.Windows.Forms.DockStyle.Right;
      this.barDockArea3.Location = new System.Drawing.Point(963, 76);
      this.barDockArea3.Margin = new System.Windows.Forms.Padding(2);
      this.barDockArea3.Name = "barDockArea3";
      this.barDockArea3.Size = new System.Drawing.Size(0, 514);
      this.barDockArea3.TabIndex = 4;
      // 
      // barDockArea4
      // 
      this.barDockArea4.BarManager = this.barManager1;
      this.barDockArea4.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.barDockArea4.Location = new System.Drawing.Point(0, 590);
      this.barDockArea4.Margin = new System.Windows.Forms.Padding(2);
      this.barDockArea4.Name = "barDockArea4";
      this.barDockArea4.Size = new System.Drawing.Size(963, 0);
      this.barDockArea4.TabIndex = 5;
      // 
      // toolWindowContainer1
      // 
      this.toolWindowContainer1.Controls.Add(this.twOverview);
      this.toolWindowContainer1.DockManager = this.dockManager1;
      this.toolWindowContainer1.Location = new System.Drawing.Point(0, 0);
      this.toolWindowContainer1.Margin = new System.Windows.Forms.Padding(2);
      this.toolWindowContainer1.Name = "toolWindowContainer1";
      this.toolWindowContainer1.Size = new System.Drawing.Size(131, 136);
      this.toolWindowContainer1.TabIndex = 8;
      // 
      // GraphicPropertiesToolWindow
      // 
      this.GraphicPropertiesToolWindow.AccessibleName = "";
      this.GraphicPropertiesToolWindow.Controls.Add(this.graphicPropertyGrid);
      this.GraphicPropertiesToolWindow.Dock = System.Windows.Forms.DockStyle.Fill;
      this.GraphicPropertiesToolWindow.DockManager = this.dockManager1;
      this.GraphicPropertiesToolWindow.Location = new System.Drawing.Point(0, 0);
      this.GraphicPropertiesToolWindow.Margin = new System.Windows.Forms.Padding(2);
      this.GraphicPropertiesToolWindow.Name = "GraphicPropertiesToolWindow";
      this.GraphicPropertiesToolWindow.Size = new System.Drawing.Size(149, 222);
      this.GraphicPropertiesToolWindow.TabIndex = 0;
      this.GraphicPropertiesToolWindow.Text = "Graphic Properties";
      // 
      // graphicPropertyGrid
      // 
      this.graphicPropertyGrid.BorderStyle = System.Windows.Forms.BorderStyle.None;
      this.graphicPropertyGrid.CommentsHeight = 70;
      this.graphicPropertyGrid.CommentsVisibility = true;
      this.graphicPropertyGrid.Dock = System.Windows.Forms.DockStyle.Fill;
      this.graphicPropertyGrid.Font = new System.Drawing.Font("Tahoma", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.World);
      this.graphicPropertyGrid.Location = new System.Drawing.Point(0, 0);
      this.graphicPropertyGrid.Margin = new System.Windows.Forms.Padding(2);
      this.graphicPropertyGrid.Name = "graphicPropertyGrid";
      this.graphicPropertyGrid.Size = new System.Drawing.Size(149, 222);
      this.graphicPropertyGrid.TabIndex = 0;
      this.graphicPropertyGrid.NodeChanged += new SysCAD.Editor.GraphicPropertyGrid.NodeChangedHandler(this.graphicPropertyGrid_NodeChanged);
      // 
      // toolWindowContainer4
      // 
      this.toolWindowContainer4.DockManager = this.dockManager1;
      this.toolWindowContainer4.Location = new System.Drawing.Point(4, 0);
      this.toolWindowContainer4.Margin = new System.Windows.Forms.Padding(2);
      this.toolWindowContainer4.Name = "toolWindowContainer4";
      this.toolWindowContainer4.Size = new System.Drawing.Size(149, 271);
      this.toolWindowContainer4.TabIndex = 12;
      toolWindowContainer4.Controls.Add(GraphicPropertiesToolWindow);
      toolWindowContainer4.Controls.Add(ModelPropertiesToolWindow);
      toolWindowContainer4.Controls.Add(itemToolWindow);
      // 
      // ModelPropertiesToolWindow
      // 
      this.ModelPropertiesToolWindow.Controls.Add(this.modelPropertiesGrid);
      this.ModelPropertiesToolWindow.Dock = System.Windows.Forms.DockStyle.Fill;
      this.ModelPropertiesToolWindow.DockManager = this.dockManager1;
      this.ModelPropertiesToolWindow.Location = new System.Drawing.Point(0, 0);
      this.ModelPropertiesToolWindow.Name = "ModelPropertiesToolWindow";
      this.ModelPropertiesToolWindow.Size = new System.Drawing.Size(149, 222);
      this.ModelPropertiesToolWindow.TabIndex = 0;
      this.ModelPropertiesToolWindow.Text = "Model Properties";
      // 
      // modelPropertiesGrid
      // 
      this.modelPropertiesGrid.BorderStyle = System.Windows.Forms.BorderStyle.None;
      this.modelPropertiesGrid.CommentsHeight = 70;
      this.modelPropertiesGrid.CommentsVisibility = true;
      this.modelPropertiesGrid.Dock = System.Windows.Forms.DockStyle.Fill;
      this.modelPropertiesGrid.Font = new System.Drawing.Font("Tahoma", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.World);
      this.modelPropertiesGrid.Location = new System.Drawing.Point(0, 0);
      this.modelPropertiesGrid.Name = "modelPropertiesGrid";
      this.modelPropertiesGrid.Size = new System.Drawing.Size(149, 222);
      this.modelPropertiesGrid.TabIndex = 0;
      // 
      // itemToolWindow
      // 
      this.itemToolWindow.Controls.Add(this.stencilChooser1);
      this.itemToolWindow.Dock = System.Windows.Forms.DockStyle.Fill;
      this.itemToolWindow.DockManager = this.dockManager1;
      this.itemToolWindow.Location = new System.Drawing.Point(0, 0);
      this.itemToolWindow.Name = "itemToolWindow";
      this.itemToolWindow.Size = new System.Drawing.Size(149, 222);
      this.itemToolWindow.TabIndex = 0;
      this.itemToolWindow.Text = "Nodes";
      // 
      // stencilChooser1
      // 
      this.stencilChooser1.AllowDrop = true;
      this.stencilChooser1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.stencilChooser1.Location = new System.Drawing.Point(0, 0);
      this.stencilChooser1.Name = "stencilChooser1";
      this.stencilChooser1.Size = new System.Drawing.Size(149, 222);
      this.stencilChooser1.TabIndex = 0;
      // 
      // statusStrip
      // 
      this.statusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel});
      this.statusStrip.Location = new System.Drawing.Point(135, 568);
      this.statusStrip.Name = "statusStrip";
      this.statusStrip.Size = new System.Drawing.Size(675, 22);
      this.statusStrip.TabIndex = 14;
      this.statusStrip.Text = "statusStrip1";
      // 
      // toolStripStatusLabel
      // 
      this.toolStripStatusLabel.Name = "toolStripStatusLabel";
      this.toolStripStatusLabel.Size = new System.Drawing.Size(0, 17);
      // 
      // dockContainerContainer1
      // 
      this.dockContainerContainer1.Controls.Add(this.toolWindowContainer1);
      this.dockContainerContainer1.Controls.Add(this.toolWindowContainer3);
      this.dockContainerContainer1.Dock = System.Windows.Forms.DockStyle.Left;
      this.dockContainerContainer1.DockManager = this.dockManager1;
      this.dockContainerContainer1.Location = new System.Drawing.Point(0, 76);
      this.dockContainerContainer1.Name = "dockContainerContainer1";
      this.dockContainerContainer1.Orientation = System.Windows.Forms.Orientation.Vertical;
      this.dockContainerContainer1.SelectedToolWindow = null;
      this.dockContainerContainer1.Size = new System.Drawing.Size(135, 514);
      this.dockContainerContainer1.TabIndex = 18;
      // 
      // toolWindowContainer3
      // 
      this.toolWindowContainer3.Controls.Add(this.twNavigation);
      this.toolWindowContainer3.DockManager = this.dockManager1;
      this.toolWindowContainer3.Location = new System.Drawing.Point(0, 140);
      this.toolWindowContainer3.Name = "toolWindowContainer3";
      this.toolWindowContainer3.Size = new System.Drawing.Size(131, 374);
      this.toolWindowContainer3.TabIndex = 10;
      // 
      // toolWindowContainer2
      // 
      this.toolWindowContainer2.Controls.Add(this.twHoverview);
      this.toolWindowContainer2.DockManager = this.dockManager1;
      this.toolWindowContainer2.Location = new System.Drawing.Point(4, 275);
      this.toolWindowContainer2.Name = "toolWindowContainer2";
      this.toolWindowContainer2.Size = new System.Drawing.Size(149, 239);
      this.toolWindowContainer2.TabIndex = 13;
      // 
      // dockContainerContainer2
      // 
      this.dockContainerContainer2.Controls.Add(this.toolWindowContainer4);
      this.dockContainerContainer2.Controls.Add(this.toolWindowContainer2);
      this.dockContainerContainer2.Dock = System.Windows.Forms.DockStyle.Right;
      this.dockContainerContainer2.DockManager = this.dockManager1;
      this.dockContainerContainer2.Location = new System.Drawing.Point(810, 76);
      this.dockContainerContainer2.Name = "dockContainerContainer2";
      this.dockContainerContainer2.Orientation = System.Windows.Forms.Orientation.Vertical;
      this.dockContainerContainer2.SelectedToolWindow = null;
      this.dockContainerContainer2.Size = new System.Drawing.Size(153, 514);
      this.dockContainerContainer2.TabIndex = 19;
      // 
      // EditorForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(963, 590);
      this.Controls.Add(this.statusStrip);
      this.Controls.Add(this.dockContainerContainer2);
      this.Controls.Add(this.dockContainerContainer1);
      this.Controls.Add(this.barDockArea3);
      this.Controls.Add(this.barDockArea1);
      this.Controls.Add(this.barDockArea4);
      this.Controls.Add(this.barDockArea2);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.IsMdiContainer = true;
      this.Margin = new System.Windows.Forms.Padding(2);
      this.Name = "EditorForm";
      this.Text = "SysCAD.Editor";
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1FormClosing);
      ((System.ComponentModel.ISupportInitialize)(this.dockManager1)).EndInit();
      this.twOverview.ResumeLayout(false);
      this.twHoverview.ResumeLayout(false);
      this.splitContainer1.Panel1.ResumeLayout(false);
      this.splitContainer1.Panel2.ResumeLayout(false);
      this.splitContainer1.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.NavigationTreeView)).EndInit();
      this.twNavigation.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.barManager1)).EndInit();
      this.toolWindowContainer1.ResumeLayout(false);
      this.GraphicPropertiesToolWindow.ResumeLayout(false);
      this.ModelPropertiesToolWindow.ResumeLayout(false);
      this.itemToolWindow.ResumeLayout(false);
      this.statusStrip.ResumeLayout(false);
      this.statusStrip.PerformLayout();
      this.dockContainerContainer1.ResumeLayout(false);
      this.toolWindowContainer3.ResumeLayout(false);
      this.toolWindowContainer2.ResumeLayout(false);
      this.dockContainerContainer2.ResumeLayout(false);
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    //void tvNavigation_NodeMouseMove(MouseEventArgs e, PureComponents.TreeView.Node oNode)
    //{
    //  if (e.Button == MouseButtons.Left)
    //  {
    //    tvNavigation.DoDragDrop(oNode, DragDropEffects.All);
    //  }
    //}

    //void tvNavigation_DragLeave(object sender, System.EventArgs e)
    //{
    //  //throw new System.NotImplementedException("The method or operation is not implemented.");
    //}

    //void tvNavigation_DragEnter(object sender, System.EventArgs e)
    //{
    //  //throw new System.NotImplementedException("The method or operation is not implemented.");
    //}

    //void tvNavigation_DragOver(object sender, System.EventArgs e)
    //{
    //  //throw new System.NotImplementedException("The method or operation is not implemented.");
    //}

    #endregion

    private ActiproSoftware.UIStudio.Dock.DockManager dockManager1;
    private ActiproSoftware.UIStudio.Dock.ToolWindow twNavigation;
    private ActiproSoftware.UIStudio.Dock.ToolWindow twOverview;
    private ActiproSoftware.UIStudio.Dock.ToolWindow twHoverview;
    private Overview ovOverview;
    public FlowChart fcHoverview;
    private ActiproSoftware.UIStudio.Bar.BarDockArea barDockArea3;
    private ActiproSoftware.UIStudio.Bar.BarManager barManager1;
    private ActiproSoftware.UIStudio.Bar.BarDockArea barDockArea1;
    private ActiproSoftware.UIStudio.Bar.BarDockArea barDockArea4;
    private ActiproSoftware.UIStudio.Bar.BarDockArea barDockArea2;
    private ActiproSoftware.UIStudio.Dock.ToolWindowContainer toolWindowContainer1;
    private PureComponents.TreeView.TreeView NavigationTreeView;
    private ActiproSoftware.UIStudio.Dock.ToolWindowContainer toolWindowContainer4;
    private ActiproSoftware.UIStudio.Dock.ToolWindow GraphicPropertiesToolWindow;
    private GraphicPropertyGrid graphicPropertyGrid;
    private ActiproSoftware.UIStudio.Dock.ToolWindow ModelPropertiesToolWindow;
    private ModelPropertyGrid modelPropertiesGrid;
    private StatusStrip statusStrip;
    private ToolStripStatusLabel toolStripStatusLabel;
    private ActiproSoftware.UIStudio.Dock.ToolWindow itemToolWindow;
    private StencilChooser stencilChooser1;
    private ActiproSoftware.UIStudio.Dock.DockContainerContainer dockContainerContainer2;
    private ActiproSoftware.UIStudio.Dock.ToolWindowContainer toolWindowContainer2;
    private ActiproSoftware.UIStudio.Dock.DockContainerContainer dockContainerContainer1;
    private ActiproSoftware.UIStudio.Dock.ToolWindowContainer toolWindowContainer3;
    private SplitContainer splitContainer1;
    private ListView listView1;
  }
}


