// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Collections;
using System.ComponentModel;
using System.ComponentModel.Design.Serialization;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Printing;
using System.Drawing.Text;
using System.IO;
using System.Reflection;
using System.Resources;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters;
using System.Runtime.Serialization.Formatters.Binary;
using System.Security;
//using System.Security.Permissions;
using System.Windows.Forms;

using MindFusion.FlowChartX.Behaviors;
using MindFusion.FlowChartX.Commands;
using MindFusion.FlowChartX.Design.Serialization;
using MindFusion.FlowChartX.LayoutSystem;
using MindFusion.FlowChartX.Visitors;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Implements the main diagram editor class. It acts as view, model and
	/// controller at the same time. In addition, it defines a set of properties
	/// that define the initial values of the diagram elements attributes.
	/// </summary>
	[DefaultEvent("BoxCreated")]
	[DefaultProperty("Behavior")]
#if !VERTIGO
	[LicenseProvider(typeof(RegistryLicenseProvider))]
#endif
	public class FlowChart : System.Windows.Forms.Control, IPersistObjFactory, IPersists
	{
		#region initialization

		/// <summary>
		/// Initializes a new instance of the FlowChart class.
		/// </summary>
		public FlowChart()
		{
			license = LicenseManager.Validate(typeof(FlowChart), this);

			measureUnit = GraphicsUnit.Millimeter;

			// set control styles for flicker-free redraw
			SetStyle(ControlStyles.UserPaint, true);
			SetStyle(ControlStyles.AllPaintingInWmPaint, true);
			SetStyle(ControlStyles.Opaque, true);
			SetStyle(ControlStyles.ResizeRedraw, true);

			// we want to process keyboard input
			SetStyle(ControlStyles.Selectable, true);

			// init static objects
			lock (syncRoot)
			{
				Arrow.initHeadTemplates();
				CustomCursors.Init(new ResourceManager(
					"MindFusion.FlowChartX.Cursors", typeof(ChartObject).Assembly));
			}

			mouseMoved = false;

			boxFillColor = Color.FromArgb(220, 220, 255);
			boxFrameColor = Color.Black;
			arrowColor = Color.Black;
			arrowTextStyle = ArrowTextStyle.Center;
			arrowFillColor = Color.FromArgb(120, 220, 255);
			tableFrameColor = Color.Black;
			tableFillColor = Color.FromArgb(180, 160, 160);
			penDashStyle = DashStyle.Solid;
			penWidth = 0;
			BackColor = Color.FromArgb(170, 170, 200);

			// grid properties
			alignToGrid = true;
			showGrid = false;
			gridColor = Color.FromArgb(140, 140, 150);
			gridSizeX = 4;
			gridSizeY = 4;
			gridStyle = GridStyle.Points;

			// shadows properties
			shadowOffsetX = 1;
			shadowOffsetY = 1;
			shadowColor = Color.FromArgb(110, 110, 140);
			shadowsStyle = ShadowsStyle.OneLevel;

			activeMnpColor = Color.White;
			selMnpColor = Color.FromArgb(170, 170, 170);
			disabledMnpColor = Color.FromArgb(200, 0, 0);

			textFormat = new StringFormat();
			textFormat.Alignment = StringAlignment.Center;
			textFormat.LineAlignment = StringAlignment.Center;

			// Set some flags, because otherwise the serializer
			// generates noncompilable code
			textFormat.FormatFlags = StringFormatFlags.NoFontFallback;

			imagePos = ImageAlign.Document;

			boxStyle = BoxStyle.RoundedRectangle;
			tableStyle = TableStyle.Rectangle;

			boxPen = new Pen(boxFrameColor, penWidth);
			boxBrush = new SolidBrush(boxFillColor);
			boxBrush.AddRef();
			arrowPen = new Pen(arrowColor, penWidth);
			arrowBrush = new SolidBrush(arrowFillColor);
			arrowBrush.AddRef();
			tablePen = new Pen(tableFrameColor, penWidth);
			tableBrush = new SolidBrush(tableFillColor);
			tableBrush.AddRef();

			exteriorBrush = null;
			brush = new SolidBrush(BackColor);
			brush.AddRef();

			boxes = new BoxCollection();
			controlHosts = new ControlHostCollection();
			tables = new TableCollection();
			arrows = new ArrowCollection();
			selection = new Selection(this);
			selectionOnTop = true;

			groups = new GroupCollection();

			zOrder = new ChartObjectCollection();

			textColor = Color.Black;
			arrowStyle = MindFusion.FlowChartX.ArrowStyle.Polyline;
			arrowSegments = 1;
			activeObject = null;

			modificationStart = ModificationStyle.SelectedOnly;
			autoHandlesObj = null;
			autoAnchorsObj = null;

			interaction = null;

			scrollX = scrollY = 0;
			zoomFactor = 100.0f;
			allowRefLinks = true;
			Behavior = BehaviorType.FlowChart;
			arrowEndsMovable = true;
			selectAfterCreate = true;

			// init default custom draw properties
			boxCustomDraw = CustomDraw.None;
			tableCustomDraw = CustomDraw.None;
			cellCustomDraw = CustomDraw.None;
			arrowCustomDraw = CustomDraw.None;

			restrObjsToDoc = RestrictToDoc.Intersection;
			dynamicArrows = false;
			arrowsSnapToBorders = false;
			arrowsRetainForm = false;
			arrowCascadeOrientation = Orientation.Auto;

			curPointer = Cursors.Arrow;
			curCannotCreate = Cursors.No;
			curModify = Cursors.SizeAll;
			curArrowStart = Cursors.Hand;
			curArrowEnd = Cursors.Hand;
			curArrowCannotCreate = Cursors.No;
			curHorzResize = Cursors.SizeWE;
			curVertResize = Cursors.SizeNS;
			curMainDgnlResize = Cursors.SizeNWSE;
			curSecDgnlResize = Cursors.SizeNESW;
			curRotateShape = CustomCursors.Rotate;
			panCursor = Cursors.NoMove2D;

			tableRowsCount = 4;
			tableColumnsCount = 2;
			tableRowHeight = 6;
			tableColWidth = 18;
			tableCaptionHeight = 5;
			tableCaption = "Table";
			tableCellBorders = CellFrameStyle.System3D;

			selHandleSize = 2;

			showDisabledHandles = true;
			arrowSelStyle = HandlesStyle.SquareHandles;
			boxSelStyle = HandlesStyle.SquareHandles;
			chostSelStyle = HandlesStyle.HatchHandles;
			tableSelStyle = HandlesStyle.DashFrame;

			recursiveExpand = false;
			expandOnIncoming = false;
			boxesExpandable = false;
			tablesScrollable = false;
			tablesExpandable = false;
			controlHostsExpandable = false;

			arrowHead = ArrowHead.Arrow;
			arrowBase = ArrowHead.None;
			arrowInterm = ArrowHead.None;

			arrowHeadSize = 5;
			arrowBaseSize = 5;
			arrowIntermSize = 5;

			ShowScrollbars = true;
			DocExtents = new RectangleF(0, 0, 210, 297);

			autoScroll = true;
			autoScrDX = autoScrDY = 0;

			dirty = false;
			antiAlias = SmoothingMode.None;
			textRendering = TextRenderingHint.SystemDefault;
			sortGroupsByZ = false;

			// tooltips
			showToolTips = true;
			toolTip = "";
			toolTipCtrl = new ToolTip();
			toolTipCtrl.Active = showToolTips;

			shapeRotation = 0;
			DefaultShape = ShapeTemplate.FromId("Cylinder");

			inplaceEditAllowed = false;
			inplaceObject = null;
			nowEditing = false;
			inplaceEditFont = (Font)Font.Clone();
			inplaceAcceptOnEnter = false;
			inplaceCancelOnEsc = true;

			allowSplitArrows = false;

			printOptions = new PrintOptions(this);
			printOptions.EnableBackground = false;
			printOptions.EnableBackgroundImage = false;
			printOptions.PaintControls = true;

			displayOptions = new PrintOptions(this);
			renderOptions = displayOptions;

			previewOptions = new PreviewOptions();

			allowLinksRepeat = true;
			showAnchors = ShowAnchors.Auto;
			snapToAnchor = SnapToAnchor.OnCreate;

			beginPrintHandler = new PrintEventHandler(this.BeginPrint);
			printPageHandler = new PrintPageEventHandler(this.PrintPage);
			printRect = new RectangleF(0, 0, 1, 1);

			usePolyTextLt = false;

			userAction = false;

			tableLinkStyle = TableLinkStyle.Rows;

			undoManager = new UndoManager(this);

			defaultControlType = typeof(System.Windows.Forms.Button);
			hostedCtrlMouseAction = HostMouseAction.SelectHost;

			dummy = new DummyNode(this);
			allowUnconnectedArrows = false;
			allowUnanchoredArrows = true;

			autoSizeDoc = MindFusion.FlowChartX.AutoSize.None;
			panMode = false;

			enableStyledText = false;

			expandBtnPos = ExpandButtonPosition.OuterRight;
			focusLost = false;

			hitTestPriority = HitTestPriority.NodesBeforeArrows;

			boxText = arrowText = "";

			// arrow crossings
			arrowCrossings = ArrowCrossings.Straight;
			crossRadius = 1.5f;
			redrawNonModified = false;

			roundRectFactor = 1;

			scriptHelper = new ScriptHelper(this);

			// link routing options
			routingOptions = new RoutingOptions(this);
			routingGrid = new RoutingGrid(this);
			routeArrows = false;
			dontRouteForAwhile = false;

			validityChecks = true;
			_modifierKeyActions = new ModifierKeyActions();
			middleButtonAction = MouseButtonAction.None;
			forceCacheRedraw = false;

			showHandlesOnDrag = true;
			mergeThreshold = 0;

			expandButtonAction = ExpandButtonAction.ExpandTreeBranch;
		}

		~FlowChart()
		{
			if (license != null)
				license.Dispose();
		}

		#endregion

		#region diagram contents and structure

		/// <summary>
		/// Gets a collection of all items in this flowchart.
		/// </summary>
		[Browsable(false)]
		public ChartObjectCollection Objects
		{
			get { return zOrder; }
		}

		/// <summary>
		/// Gets a collection of all boxes in this flowchart.
		/// </summary>
		[Browsable(false)]
		public BoxCollection Boxes
		{
			get { return boxes; }
		}

		/// <summary>
		/// Gets a collection of all tables in this flowchart.
		/// </summary>
		[Browsable(false)]
		public TableCollection Tables
		{
			get { return tables; }
		}

		/// <summary>
		/// Gets a collection of all control hosts in this flowchart.
		/// </summary>
		[Browsable(false)]
		public ControlHostCollection ControlHosts
		{
			get { return controlHosts; }
		}

		/// <summary>
		/// Gets a collection of all arrows in this flowchart.
		/// </summary>
		[Browsable(false)]
		public ArrowCollection Arrows
		{
			get { return arrows; }
		}

		/// <summary>
		/// Gets a collection of all groups in this flowchart.
		/// </summary>
		[Browsable(false)]
		public GroupCollection Groups
		{
			get { return groups; }
		}

		/// <summary>
		/// Gets a reference to a Selection object that represents
		/// the selection of items in this flowchart.
		/// </summary>
		[Browsable(false)]
		[Description("Selection settings.")]
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		[TypeConverter(typeof(ExpandableObjectConverter))]
		public Selection Selection
		{
			get { return selection; }
		}

		/// <summary>
		/// Gets or sets the active item - a selected item drawn with white handles
		/// around it. Usually that is the last item added to the selection.
		/// </summary>
		[Browsable(false)]
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public ChartObject ActiveObject
		{
			get
			{
				return activeObject;
			}
			set
			{
				if (activeObject == value)
					return;

				RectangleF rcUpdate = RectangleF.Empty;
				if (activeObject != null)
					rcUpdate = activeObject.getRepaintRect(false);

				activeObject = value;

				if (activeObject != null)
					rcUpdate = Utilities.unionNonEmptyRects(rcUpdate,
						activeObject.getRepaintRect(false));

				if (activeObject != null)
				{
					if (activeObject.getType() == ItemType.ControlHost)
					{
						// Bring the contained control to front
						ControlHost host = (ControlHost)activeObject;
						if (host.Control != null)
							bringContainedControlToFront(host.Control);
					}
				}

				invalidate(rcUpdate);
			}
		}

		/// <summary>
		/// Adds the specified item to the flowchart.
		/// </summary>
		/// <param name="item">A new item that should be added to the flowchart.</param>
		public void Add(ChartObject item)
		{
			Add(item, false);
		}

		/// <summary>
		/// Adds the specified item to the flowchart.
		/// </summary>
		/// <param name="item">A new item that should be added to the flowchart.</param>
		/// <param name="select">Specifies whether the item should be selected
		/// after adding it to the flowchart.</param>
		public void Add(ChartObject item, bool select)
		{
			// validity checks can be disabled to save processing time, but beware,
			// evil things can happen if the item collections are left in invalild state
			if (validityChecks)
			{
				// do not allow adding an item more than once
				if (zOrder.Contains(item))
					return;

				if (item.getType() == ItemType.Arrow)
				{
					Arrow arrow = item as Arrow;

					// links origin and destination nodes must be in the same diagram
					if (!zOrder.Contains(arrow.Origin) &&
						(arrow.Origin == null || !(arrow.Origin is DummyNode)))
						return;
					if (!zOrder.Contains(arrow.Destination) &&
						(arrow.Destination == null || !(arrow.Destination is DummyNode)))
						return;
				}
			}

			if (item.getType() == ItemType.ControlHost)
			{
				// add the hosted control to the flowchart
				ControlHost host = item as ControlHost;
				if (host.Control != null)
				{
					this.Controls.Add(host.Control);
					bringContainedControlToFront(host.Control);
				}
			}

			item.setParent(this);
			item.setConstructed();

			// add to the diagram
			AddItemCmd cmd = new AddItemCmd(item);
			cmd.Execute();

			RectangleF rc = item.getRepaintRect(false);
			if (select)
			{
				selection.Change(item);
				rc = Utilities.unionNonEmptyRects(rc,
					selection.getRepaintRect(true));
			}

			if (undoManager.UndoEnabled)
				cmd.saveSelState();

			// repaint the diagram area covered by the new item
			invalidate(rc);
			setDirty();
		}

		/// <summary>
		/// Adds the specified group to the flowchart.
		/// </summary>
		/// <param name="group">A new Group object that should be added to the flowchart.</param>
		public void Add(Group group)
		{
			group.flowChart = this;
			AddGroupCmd cmd = new AddGroupCmd(group.MainObject, group);
			undoManager.executeCommand(cmd);
			setDirty();
		}

		/// <summary>
		/// Creates a new table at the specified location.
		/// </summary>
		/// <param name="x">Specifies the horizontal position of the table.</param>
		/// <param name="y">Specifies the vertical position of the table.</param>
		/// <param name="width">Specifies the width of the table.</param>
		/// <param name="height">Specifies the height of the table.</param>
		/// <returns>A new Table instance.</returns>
		public Table CreateTable(float x, float y, float width, float height)
		{
			// create the box object and store it in the collection
			Table newTable = new Table(this);
			newTable.setPos(x, y, width, height);

			Add(newTable, SelectAfterCreate);

			return newTable;
		}

		/// <summary>
		/// Creates a relation between the specified rows of two tables.
		/// </summary>
		/// <param name="src">The source table.</param>
		/// <param name="srcRow">The source row.</param>
		/// <param name="dest">The destination table.</param>
		/// <param name="destRow">The destination row.</param>
		/// <returns>A new Arrow instance.</returns>
		public Arrow CreateRelation(Table src, int srcRow, Table dest, int destRow)
		{
			return CreateArrow(src, srcRow, dest, destRow);
		}

		private void addToObjColl(ChartObject obj)
		{
			switch (obj.getType())
			{
				case ItemType.Box:
					boxes.Add((Box)obj);
					break;
				case ItemType.ControlHost:
					controlHosts.Add((ControlHost)obj);
					break;
				case ItemType.Table:
					tables.Add((Table)obj);
					break;
				case ItemType.Arrow:
					arrows.Add((Arrow)obj);
					break;
			}
		}

		internal void addItem(ChartObject item)
		{
			Arrow arrow = item as Arrow;
			if (arrow != null)
				arrow.updateNodeCollections();

			addToObjColl(item);
			zOrder.Add(item);
			updateZOrder(zOrder.Count - 1);

			item.onAdd();

			// update document size if needed
			if (autoSizeDoc != MindFusion.FlowChartX.AutoSize.None)
				sizeDocForItem(item);
		}

		internal void removeItem(ChartObject item)
		{
			selection.RemoveObject(item);
			if (item == autoHandlesObj) autoHandlesObj = null;
			if (item == autoAnchorsObj) autoAnchorsObj = null;

			switch (item.getType())
			{
				case ItemType.Box:
					boxes.Remove((Box)item);
					break;
				case ItemType.ControlHost:
					controlHosts.Remove((ControlHost)item);
					break;
				case ItemType.Table:
					tables.Remove((Table)item);
					break;
				case ItemType.Arrow:
					arrows.Remove((Arrow)item);
					break;
			}

			removeFromZOrder(item);

			MethodCallVisitor visitor =
				new MethodCallVisitor(new VisitOperation(removeFromSelection));

			switch (item.getType())
			{
				case ItemType.ControlHost:
				{
					ControlHost host = (ControlHost)item;
					host.visitArrows(visitor);
					host.deleteArrows();
				}
					break;
				case ItemType.Box:
				{
					Box box = (Box)item;
					box.visitArrows(visitor);
					box.deleteArrows();
				}
					break;
				case ItemType.Table:
				{
					Table table = (Table)item;
					table.visitArrows(visitor);
					table.deleteArrows();
				}
					break;
				case ItemType.Arrow:
				{
					Arrow arrow = (Arrow)item;
					arrow.resetCrossings();
					arrow.getDestLink().removeArrowFromObj();
					arrow.getOrgnLink().removeArrowFromObj();
				}
					break;
			}

			item.onRemove();

			// update document size if needed
			if (autoSizeDoc != MindFusion.FlowChartX.AutoSize.None)
				sizeDocForItems();
		}

		/// <summary>
		/// Removes an item from the diagram.
		/// </summary>
		/// <param name="obj">Refers to the item that should be removed.</param>
		/// <returns>true if the item was deleted successfully, otherwise false.</returns>
		public bool DeleteObject(ChartObject obj)
		{
			if (obj == null) return false;

			bool deleted = deleteItem(obj);
			if (deleted)
				Invalidate();

			return deleted;
		}

		private void removeFromSelection(ChartObject obj)
		{
			selection.RemoveObject(obj);
		}

		internal bool deleteItem(ChartObject item)
		{
			if (!zOrder.Contains(item)) return false;

			if (interaction != null)
			{
				if (interaction.isCompleting())
				{
					if (interaction.CurrentObject == item)
						interaction.setItemDeleted();
				}
				else
				{
					buttonDown[0] = false;
					if (Capture)
						Capture = false;
					interaction.cancel(this);
					interaction = null;
				}
			}

			new RemoveItemCmd(item).Execute();
			
			setDirty();
			return true;
		}

		/// <summary>
		/// Removes all items and groups from the diagram.
		/// </summary>
		public void ClearAll()
		{
			endInplaceEdit(true);
#if !FCNET_STD
			undoManager.History.Clear();
			undoManager.resetContext();
#endif

			// clear selection
			selection.Clear();

			// destroy all groups
			foreach (Group group in groups)
				group.onDelete();
			groups.Clear();

			// clear type-specific lists
			arrows.Clear();
			controlHosts.Clear();
			boxes.Clear();
			tables.Clear();

			// free resources and clear all items
			foreach (ChartObject obj in zOrder)
			{
				obj.onRemove();
				obj.freeResources();
			}
			zOrder.Clear();

			autoHandlesObj = null;
			interaction = null;
			activeObject = null;
			autoAnchorsObj = null;

			GC.Collect();
			Invalidate();
			setDirty();
		}

		internal void zLevelUp(ChartObject obj)
		{
			int i = getZIndex(obj);
			if (i == -1) return;
			if (i == zOrder.Count - 1) return;

			zOrder[i] = zOrder[i+1];
			zOrder[i+1] = obj;

			setDirty();
			updateZOrder(i, i + 1);
		}

		internal void zLevelDown(ChartObject obj)
		{
			int i = getZIndex(obj);
			if (i == -1) return;
			if (i == 0) return;

			zOrder[i] = zOrder[i-1];
			zOrder[i-1] = obj;

			setDirty();
			updateZOrder(i - 1, i);
		}

		internal void zBottom(ChartObject obj)
		{
			int i = getZIndex(obj);
			if (i == -1) return;
			if (i == 0) return;

			while (i > 0)
			{
				zOrder[i] = zOrder[i-1];
				i--;
			}
			zOrder[0] = obj;

			setDirty();
			updateZOrder(0);
		}

		internal void zTop(ChartObject obj)
		{
			int i = getZIndex(obj);
			int start = i;
			if (i == -1) return;
			if (i == zOrder.Count - 1) return;

			while (i < zOrder.Count - 1)
			{
				zOrder[i] = zOrder[i+1];
				i++;
			}
			zOrder[i] = obj;

			setDirty();
			updateZOrder(start);
		}

		private int getZIndex(ChartObject obj)
		{
			for (int i = 0; i < zOrder.Count; ++i)
			{
				if (obj == zOrder[i])
					return i;
			}

			return -1;
		}

		internal bool setZIndex(ChartObject obj, int index)
		{
			if (index < 0) return false;
			if (index >= zOrder.Count) return false;

			int i = getZIndex(obj);
			if (i == -1) return false;
			if (i == index) return true;

			int start = Math.Min(i, index);
			if (i < index)
			{
				while (i < index)
				{
					zOrder[i] = zOrder[i+1];
					i++;
				}
				zOrder[i] = obj;
			}
			else
				if (i > index)
			{
				while (i > index)
				{
					zOrder[i] = zOrder[i-1];
					i--;
				}
				zOrder[i] = obj;
			}

			setDirty();
			updateZOrder(start);

			return true;
		}

		private void updateZOrder(int from)
		{
			for (int i = from; i < zOrder.Count; ++i)
				zOrder[i].updateZIndex(i);
		}

		private void updateZOrder(int from, int to)
		{
			for (int i = from; i <= to; ++i)
				zOrder[i].updateZIndex(i);
		}

		internal void setZOrder(ChartObjectCollection zOrder)
		{
			this.zOrder = zOrder;
			updateZOrder(0);
		}

		/// <summary>
		/// Creates a new hierarchical group.
		/// </summary>
		/// <param name="mainObj">The group master item; the subordinated items will follow the
		/// master one when it is moved around.</param>
		/// <returns>A new Group instance.</returns>
		public Group CreateGroup(ChartObject mainObj)
		{
			if (mainObj == null) return null;

			Group group = new Group(this);
			if (group.setMainObject(mainObj))
			{
				AddGroupCmd cmd= new AddGroupCmd(mainObj, group);
				undoManager.executeCommand(cmd);
				return group;
			}
			else
			{
				group.onDelete();
				group = null;
				return null;
			}
		}

		/// <summary>
		/// Destroys the specified group; the group items are not deleted.
		/// </summary>
		/// <param name="group">The group that should be destroyed.</param>
		public void DestroyGroup(Group group)
		{
			new RemoveGroupCmd(group.MainObject, group).Execute();
		}

		internal void deleteGroup(Group group)
		{
			groups.Remove(group);
			group.onDelete();
			group = null;
		}

		internal bool rectRestrict(ref RectangleF rc)
		{
			// no restrictions when rdNoRestriction is set
			if (restrObjsToDoc == RestrictToDoc.NoRestriction)
				return false;

			// no restrictions when auto-resizing is enabled for all directions
			if (autoSizeDoc == MindFusion.FlowChartX.AutoSize.AllDirections)
				return false;

			RectangleF rect = Utilities.normalizeRect(rc);

			// if resizing is enabled to the right and down
			if (autoSizeDoc == MindFusion.FlowChartX.AutoSize.RightAndDown)
			{
				// make sure the item is entirely inside at the top and left sides ...
				if (restrObjsToDoc == RestrictToDoc.InsideOnly)
				{
					if (docExtents.Left <= rect.Left &&
						docExtents.Top <= rect.Top)
						return false;
					return true;
				}

				// ... or at least partly inside at the top and left sides ...
				if (docExtents.Left > rect.Right ||
					docExtents.Top > rect.Bottom)
					return true;

				return false;
			}

			// no auto-resizing, ensure all sides of the item are inside the document
			if (restrObjsToDoc == RestrictToDoc.InsideOnly)
			{
				if (docExtents.Left <= rect.Left &&
					docExtents.Top <= rect.Top &&
					docExtents.Right >=  rect.Right &&
					docExtents.Bottom >= rect.Bottom)
					return false;
				return true;
			}

			// or at least that the item is partly inside the document
			if (docExtents.Left > rect.Right ||
				docExtents.Top > rect.Bottom ||
				docExtents.Right < rect.Left ||
				docExtents.Bottom < rect.Top)
				return true;

			return false;
		}

		internal void getIntersectingObjects(RectangleF rect,
			ChartObjectCollection objects, bool multiple, bool ifIntersect)
		{
			RectangleF rcSel = Utilities.normalizeRect(rect);

			if (ifIntersect)
			{
				foreach (ChartObject obj in zOrder)
				{
					RectangleF rcObjRect = obj.getRotatedBounds();
					if (!obj.notInteractive() && rcObjRect.IntersectsWith(rcSel))
					{
						objects.Add(obj);
						if (!multiple) return;
					}
				}
			}
			else
			{
				foreach (ChartObject obj in zOrder)
				{
					RectangleF rcObjRect = obj.getRotatedBounds();
					if (!obj.notInteractive() && rcSel.Contains(rcObjRect))
					{
						objects.Add(obj);
						if (!multiple) return;
					}
				}
			}
		}

		internal int getIntrsObjsCnt(RectangleF rect, bool ifIntersect)
		{
			int count = 0;
			RectangleF rcSel = Utilities.normalizeRect(rect);

			if (ifIntersect)
			{
				foreach (ChartObject obj in zOrder)
				{
					RectangleF rcObjRect = obj.getRotatedBounds();
					if (!obj.notInteractive() &&
						rcSel.IntersectsWith(rcObjRect))
						count++;
				}
			}
			else
			{
				foreach (ChartObject obj in zOrder)
				{
					RectangleF rcObjRect = obj.getRotatedBounds();
					if (!obj.notInteractive() &&
						rcSel.Contains(rcObjRect))
						count++;
				}
			}

			return count;
		}

		// Returns the box or table whose bounding rect
		// intersects with the specified rectangle.
		internal ChartObject objectFromRect(RectangleF rect,
			ChartObject obj1, ChartObject obj2)
		{
			RectangleF rc;
			float inflation = Constants.getInflation(measureUnit);

			for (int i = zOrder.Count - 1; i >= 0; i--)
			{
				ChartObject obj = zOrder[i];
				if (obj is Node)
				{
					rc = obj.getBoundingRect();

					if (obj == obj1 || obj == obj2)
						rc.Inflate(-inflation, -inflation);

					if(rc.IntersectsWith(rect))
						return obj;
				}
			}

			return null;
		}

		// Returns the box or table (if any) intersected
		// by the line segment, specified with its ending points
		internal ChartObject objectIntersectedBy(PointF pt1, PointF pt2,
			ChartObject obj1, ChartObject obj2)
		{
			RectangleF rc;
			float inflation = Constants.getInflation(measureUnit);

			for (int i = zOrder.Count - 1; i >= 0; i--)
			{
				ChartObject obj = zOrder[i];
				if (obj is Node)
				{
					rc = obj.getBoundingRect();
					if (obj == obj1 || obj == obj2)
						rc.Inflate(-inflation, -inflation);
					PointF p1 = new PointF(rc.Left, rc.Top);
					PointF p2 = new PointF(rc.Right, rc.Top);
					PointF p3 = new PointF(rc.Right, rc.Bottom);
					PointF p4 = new PointF(rc.Left, rc.Bottom);
					PointF pt = PointF.Empty;

					if (Utilities.segmentIntersect(p1, p2, pt1, pt2, ref pt) ||
						Utilities.segmentIntersect(p2, p3, pt1, pt2, ref pt) ||
						Utilities.segmentIntersect(p3, p4, pt1, pt2, ref pt) ||
						Utilities.segmentIntersect(p4, p1, pt1, pt2, ref pt))
						return obj;
				}
			}

			return null;
		}

		/// <summary>
		/// Returns the top-most node that lies at the specified position.
		/// </summary>
		/// <param name="point">A PointF specifying a diagram point in logical coordinates.</param>
		/// <returns>A Node instance if a node lies at the specified location, otherwise null.</returns>
		public Node GetNodeAt(PointF point)
		{
			Node node = null;
			
			// search for object containing the point
			for (int i = zOrder.Count - 1; i >= 0; i--)
			{
				ChartObject item = zOrder[i];
				if (item is Node
					&& item.containsPoint(point) && !item.notInteractive())
				{
					node = item as Node;
					break;
				}
			}
			
			return node;
		}

		/// <summary>
		/// Returns the top-most node lying at the specified location.
		/// </summary>
		/// <param name="point">Specifies a location in the diagram where to look for nodes.</param>
		/// <param name="excludeLocked">Specifies whether locked nodes should be excluded from the search.</param>
		/// <param name="excludeSelected">Specifies whether selected nodes should be excluded from the search.</param>
		/// <returns>A Node object found at the specified point.</returns>
		public Node GetNodeAt(PointF point,
			bool excludeLocked, bool excludeSelected)
		{
			Node node = null;

			// search for object containing the point
			for (int i = zOrder.Count - 1; i >= 0; i--)
			{
				ChartObject item = zOrder[i];

				if (!(item is Node))
					continue;
				if (item.Locked && excludeLocked)
					continue;
				if (item.Selected && excludeSelected)
					continue;

				if (item.containsPoint(point))
				{
					node = item as Node;
					break;
				}
			}
			
			return node;
		}

		/// <summary>
		/// Returns the top-most box that lies at the specified position.
		/// </summary>
		/// <param name="point">A PointF specifying a diagram point in logical coordinates.</param>
		/// <returns>A Box instance if a box lies at the specified location, otherwise null.</returns>
		public Box GetBoxAt(PointF point)
		{
			return GetBoxAt(point, 0);
		}

		/// <summary>
		/// Returns the top-most box that lies at the specified position.
		/// </summary>
		/// <param name="point">A PointF specifying a diagram point in logical coordinates.</param>
		/// <param name="threshold">A float threshold value specifying how far from the box the point is allowed to be.</param>
		/// <returns>A Box instance if a box lies at the specified location, otherwise null.</returns>
		public Box GetBoxAt(PointF point, float threshold)
		{
			Box foundBox = null;

			//search the bounding rectangle
			for (int i = zOrder.Count - 1; i >= 0; i--)
			{
				ChartObject item = zOrder[i];
				if (item.getType() == ItemType.Box && !item.notInteractive())
				{
					Box aBox = item as Box;
					bool contains = threshold == 0 ?
						item.containsPoint(point) : aBox.containsPoint(point, threshold);

					if (contains)
					{
						foundBox = aBox;
						break;
					}
				}
			}

			return foundBox;
		}

		/// <summary>
		/// Returns the top-most control host that lies at the specified position.
		/// </summary>
		/// <param name="pt">A PointF specifying a diagram point in logical coordinates.</param>
		/// <returns>A ControlHost instance if one lies at the specified location, otherwise null.</returns>
		public ControlHost GetControlHostAt(PointF pt)
		{
			ControlHost host = null;

			// Search the bounding rectangle
			for (int i = zOrder.Count - 1; i >= 0; i--)
			{
				ChartObject obj = zOrder[i];
				if (obj.getType() == ItemType.ControlHost &&
					obj.containsPoint(pt) && !obj.notInteractive())
				{
					// Found
					host = (ControlHost)obj;
					break;
				}
			}

			return host;
		}

		/// <summary>
		/// Returns the top-most table that lies at the specified position.
		/// </summary>
		/// <param name="pt">A PointF specifying a diagram point in logical coordinates.</param>
		/// <returns>A Table instance if a table lies at the specified location, otherwise null.</returns>
		public Table GetTableAt(PointF pt)
		{
			Table table = null;

			//search the bounding rectangle
			for (int i = zOrder.Count - 1; i >= 0; i--)
			{
				ChartObject obj = zOrder[i];
				if (obj.getType() == ItemType.Table &&
					obj.containsPoint(pt) && !obj.notInteractive())
				{
					//found
					table = (Table)obj;
					break;
				}
			}

			return table;
		}

		/// <summary>
		/// Returns the top-most arrow that lies at the specified position.
		/// </summary>
		/// <param name="pt">A PointF specifying a diagram point in logical coordinates.</param>
		/// <param name="maxDist">The maximum distance allowed between tested arrows and the specified point.</param>
		/// <returns>An Arrow object found near the specified point.</returns>
		public Arrow GetArrowAt(PointF pt, float maxDist)
		{
			int segmNum = 0;
			return GetArrowAt(pt, maxDist, true, ref segmNum);
		}

		/// <summary>
		/// Returns the top-most arrow that lies at the specified position;
		/// optionally exclude locked arrows from the search.
		/// </summary>
		/// <param name="pt">A PointF specifying a diagram point in logical coordinates.</param>
		/// <param name="maxDist">The maximum distance allowed between tested arrows and the specified point.</param>
		/// <param name="exclLocked">Specifies whether locked arrows should be excluded from the search.</param>
		/// <returns>An Arrow object found near the specified point.</returns>
		public Arrow GetArrowAt(PointF pt, float maxDist, bool exclLocked)
		{
			int segmNum = 0;
			return GetArrowAt(pt, maxDist, exclLocked, ref segmNum);
		}

		/// <summary>
		/// Returns the top-most arrow that lies at the specified position;
		/// optionally exclude locked arrows from the search.
		/// </summary>
		/// <param name="pt">A PointF specifying a diagram point in logical coordinates.</param>
		/// <param name="maxDist">The maximum distance allowed between tested arrows and the specified point.</param>
		/// <param name="exclLocked">Specifies whether locked arrows should be excluded from the search.</param>
		/// <param name="segmNum">Returns the index of the arrow segment that list at the specified point.</param>
		/// <returns>An Arrow object found near the specified point.</returns>
		public Arrow GetArrowAt(PointF pt, float maxDist, bool exclLocked, ref int segmNum)
		{
			Arrow nearest = null;

			// search for the nearest arrow containing the point
			float dist, minDist = float.MaxValue;
			for (int i = zOrder.Count - 1; i >= 0; i--)
			{
				if (zOrder[i].getType() == ItemType.Arrow)
				{
					Arrow arrow = (Arrow)zOrder[i];
					if ((exclLocked && arrow.notInteractive()) || arrow.Invisible)
						continue;
					RectangleF rc = arrow.getBoundingRect();
					rc.Inflate(maxDist, maxDist);

					if (Utilities.pointInRect(pt, rc))
					{
						int s = 0;
						dist = arrow.distToPt(pt, ref s);
						if (minDist > dist)
						{
							minDist = dist;
							nearest = arrow;
							segmNum = s;
						}
					}
				}
			}

			if (minDist > maxDist) return null;

			return nearest;
		}

		/// <summary>
		/// Returns the top-most diagram item lying at the specified location.
		/// </summary>
		/// <param name="pt">Specifies a location in the diagram where to look for items.</param>
		/// <param name="exclLocked">Specifies whether locked items should be excluded from the search.</param>
		/// <returns>A ChartObject instance found at the specified point.</returns>
		public ChartObject GetObjectAt(PointF pt, bool exclLocked)
		{
			if (hitTestPriority == HitTestPriority.NodesBeforeArrows)
			{
				ChartObject obj = null;
			
				// search for object containing the point
				for (int i = zOrder.Count-1; i >= 0; i--)
				{
					ChartObject objTest = zOrder[i];
					if (objTest is Node && objTest.containsPoint(pt) &&
						!(exclLocked && objTest.Locked) && !objTest.Invisible)
					{
						obj = objTest;
						break;
					}
				}
			
				if (obj != null)
					return obj;

				// search for the nearest arrow containing the point
				return GetArrowAt(pt, Constants.getLineHitTest(measureUnit), exclLocked);
			}
			else
			{
				// search for object containing the point
				for (int i = zOrder.Count - 1; i >= 0; i--)
				{
					ChartObject item = zOrder[i];
					if (item.containsPoint(pt) &&
						!(exclLocked && item.Locked) && !item.Invisible)
						return item;
				}

				return null;
			}
		}

		private void bringContainedControlToFront(Control control)
		{
			control.BringToFront();

			// Ensure scrollbars are at the top
			if (hScrollBar != null)
				hScrollBar.BringToFront();
			if (vScrollBar != null)
				vScrollBar.BringToFront();
		}

		internal void clearRuntimeData(int key)
		{
			foreach (ChartObject item in zOrder)
				item.freeData(key);
		}

		private ChartObject activeObject;
		private ChartObject autoHandlesObj;
		private Node autoAnchorsObj;
		private InteractionState interaction;

		internal ChartObject getAutoHObj() { return autoHandlesObj; }
		internal void setAutoHObj(ChartObject obj) { autoHandlesObj = obj; }

		void removeFromZOrder(ChartObject obj)
		{
			zOrder.Remove(obj);
			updateZOrder(0);
		}

		internal ChartObject getAtZ(int i)
		{
			return zOrder[i];
		}

		/// <summary>
		/// If enabled, validity checks are performed each time an item is added to the diagram.
		/// That involves enumerating the item collections and can slow up the process considerably
		/// for large diagrams. Disable this property to skip the checks, however be sure that you
		/// don't add an item twice to the diagram and that links are created only between items in
		/// the same diagram.
		/// </summary>
		[Category("Behavior")]
		[DefaultValue(true)]
		[Description("If enabled, validity checks are performed each time an item is added to the diagram. That involves enumerating the item collections and can slow up the process considerably for large diagrams. Disable this property to skip the checks, however be sure that you don't add an item twice to the diagram and that links are created only between items in the same diagram.")]
		public bool ValidityChecks
		{
			get { return validityChecks; }
			set { validityChecks = value; }
		}

		private bool validityChecks;

		/// <summary>
		/// Creates a new Box instance and adds it to the flowchart.
		/// </summary>
		/// <param name="x">Horizontal position of the box.</param>
		/// <param name="y">Vertical position of the box.</param>
		/// <param name="width">Width of the box.</param>
		/// <param name="height">Height of the box.</param>
		/// <returns>A reference to the new box.</returns>
		public Box CreateBox(float x, float y, float width, float height)
		{
			// create the box object and store it in the collection
			Box newBox = new Box(this);
			newBox.setPos(x, y, width, height);

			Add(newBox, SelectAfterCreate);

			return newBox;
		}

		/// <summary>
		/// Creates a new ControlHost instance and adds it to the flowchart.
		/// </summary>
		/// <param name="x">Horizontal position of the ControlHost.</param>
		/// <param name="y">Vertical position of the ControlHost.</param>
		/// <param name="width">Width of the ControlHost.</param>
		/// <param name="height">Height of the ControlHost.</param>
		/// <returns>A reference to the new ControlHost.</returns>
		public ControlHost CreateControlHost(float x, float y, float width, float height)
		{
			ControlHost newHost = new ControlHost(this);
			newHost.setPos(x, y, width, height);
			newHost.updateControlPosition();

			Add(newHost, SelectAfterCreate);

			return newHost;
		}

		/// <summary>
		/// Creates a new Arrow instance and adds it to the flowchart.
		/// </summary>
		/// <param name="srcNode">Specifies the arrow origin.</param>
		/// <param name="dstNode">Specifies the arrow destination.</param>
		/// <returns>A reference to the new arrow.</returns>
		public Arrow CreateArrow(Node srcNode, Node dstNode)
		{
			if (srcNode == null || dstNode == null) return null;

			// create the arrow object and store it in the collection
			Arrow newArrow = new Arrow(this);
			newArrow.setOrgAndDest(
				srcNode.createLink(newArrow, srcNode.getCenter(), false),
				dstNode.createLink(newArrow, dstNode.getCenter(), true));

			Add(newArrow, SelectAfterCreate);

			return newArrow;
		}

		/// <summary>
		/// Creates a new Arrow instance and adds it to the flowchart.
		/// </summary>
		/// <param name="srcBox">Specifies the origin box.</param>
		/// <param name="srcAnchor">Specifies the origin anchor point.</param>
		/// <param name="dstBox">Specifies the destination box.</param>
		/// <param name="dstAnchor">Specifies the destination anchor point.</param>
		/// <returns>A reference to the new arrow.</returns>
		public Arrow CreateArrow(Box srcBox, int srcAnchor, Box dstBox, int dstAnchor)
		{
			if (srcBox == null || dstBox == null) return null;

			// create the arrow object and store it in the collection
			Arrow newArrow = new Arrow(this);
			newArrow.setOrgAndDest(
				srcBox.createLink(newArrow, srcBox.getCenter(), false),
				dstBox.createLink(newArrow, dstBox.getCenter(), true));

			Add(newArrow, SelectAfterCreate);

			newArrow.OrgnAnchor = srcAnchor;
			newArrow.DestAnchor = dstAnchor;
			newArrow.updatePoints(newArrow.Points[newArrow.Points.Count-1]);

			return newArrow;
		}

		/// <summary>
		/// Creates a new Arrow instance and adds it to the flowchart.
		/// </summary>
		/// <param name="src">Specifies the origin table.</param>
		/// <param name="dest">Specifies the destination table.</param>
		/// <returns>A reference to the new arrow.</returns>
		public Arrow CreateArrow(Table src, Table dest)
		{
			return CreateArrow(src, -1, dest, -1);
		}

		/// <summary>
		/// Creates a new Arrow instance and adds it to the flowchart.
		/// </summary>
		/// <param name="src">Specifies the origin table.</param>
		/// <param name="srcRow">Specifies the origin row.</param>
		/// <param name="dest">Specifies the destination table.</param>
		/// <param name="destRow">Specifies the destination row.</param>
		/// <returns>A reference to the new arrow.</returns>
		public Arrow CreateArrow(Table src, int srcRow, Table dest, int destRow)
		{
			if (src == null || dest == null) return null;

			if (!src.canHaveArrows(true) && srcRow != -1) return null;
			if (!dest.canHaveArrows(false) && destRow != -1) return null;

			if (srcRow < -1 || srcRow >= src.RowCount) return null;
			if (destRow < -1 || destRow >= dest.RowCount) return null;

			// create the arrow object and store it in the item array
			Arrow newArrow = new Arrow(this);
			newArrow.setOrgAndDest(
				src.createLink(newArrow, false, srcRow),
				dest.createLink(newArrow, true, destRow));

			Add(newArrow, SelectAfterCreate);

			return newArrow;
		}

		/// <summary>
		/// Creates a new Arrow instance and adds it to the flowchart.
		/// </summary>
		/// <param name="srcNode">Specifies the origin node.</param>
		/// <param name="destTable">Specifies the destination table.</param>
		/// <param name="destRow">Specifies the destination row.</param>
		/// <returns>A reference to the new arrow.</returns>
		public Arrow CreateArrow(Node srcNode, Table destTable, int destRow)
		{
			if (srcNode == null || destTable == null) return null;

			if (!destTable.canHaveArrows(false) && destRow != -1) return null;

			if (destRow < -1 || destRow >= destTable.RowCount)	return null;

			// create the arrow object and store it in the collection
			Arrow newArrow = new Arrow(this);
			newArrow.setOrgAndDest(
				srcNode.createLink(newArrow, srcNode.getCenter(), false),
				destTable.createLink(newArrow, true, destRow));

			Add(newArrow, SelectAfterCreate);

			return newArrow;
		}

		/// <summary>
		/// Creates a new Arrow instance and adds it to the flowchart.
		/// </summary>
		/// <param name="srcTable">Specifies the origin table.</param>
		/// <param name="srcRow">Specifies the origin row.</param>
		/// <param name="dstNode">Specifies the destination node.</param>
		/// <returns>A reference to the new arrow.</returns>
		public Arrow CreateArrow(Table srcTable, int srcRow, Node dstNode)
		{
			if (srcTable == null || dstNode == null) return null;

			if (!srcTable.canHaveArrows(true) && srcRow != -1) return null;

			if (srcRow < -1 || srcRow >= srcTable.RowCount) return null;

			// create the arrow object and store it in the item array
			Arrow newArrow = new Arrow(this);
			newArrow.setOrgAndDest(
				srcTable.createLink(newArrow, false, srcRow),
				dstNode.createLink(newArrow, dstNode.getCenter(), true));

			Add(newArrow, SelectAfterCreate);

			return newArrow;
		}

		/// <summary>
		/// Creates a new Arrow instance and adds it to the flowchart.
		/// </summary>
		/// <param name="src">Specifies the origin node.</param>
		/// <param name="dest">Specifies the destination point.</param>
		/// <returns>A reference to the new arrow.</returns>
		public Arrow CreateArrow(Node src, PointF dest)
		{
			if (src == null) return null;

			// create the arrow instance and add it to the chart
			Arrow newArrow = new Arrow(this, src, dest);
			Add(newArrow, SelectAfterCreate);
			return newArrow;
		}

		/// <summary>
		/// Creates a new Arrow instance and adds it to the flowchart.
		/// </summary>
		/// <param name="src">Specifies the origin point.</param>
		/// <param name="dest">Specifies the destination node.</param>
		/// <returns>A reference to the new arrow.</returns>
		public Arrow CreateArrow(PointF src, Node dest)
		{
			if (dest == null) return null;

			// create the arrow instance and add it to the chart
			Arrow newArrow = new Arrow(this, src, dest);
			Add(newArrow, SelectAfterCreate);
			return newArrow;
		}

		/// <summary>
		/// Creates a new Arrow instance and adds it to the flowchart.
		/// </summary>
		/// <param name="src">Specifies the origin point.</param>
		/// <param name="dest">Specifies the destination point.</param>
		/// <returns>A reference to the new arrow.</returns>
		public Arrow CreateArrow(PointF src, PointF dest)
		{
			// create the arrow instance and add it to the chart
			Arrow newArrow = new Arrow(this, src, dest);
			Add(newArrow, SelectAfterCreate);
			return newArrow;
		}

		/// <summary>
		/// Finds the box that has the specified tag value.
		/// </summary>
		/// <param name="tag">The looked for tag value.</param>
		/// <returns>A box whose Tag equals to the specified value.</returns>
		public Box FindBox(object tag)
		{
			Box foundBox = null;

			// search for the box
			foreach (Box box in boxes)
			{
				if (Object.Equals(box.Tag, tag))
				{
					foundBox = box;
					break;
				}
			}

			return foundBox;
		}

		/// <summary>
		/// Finds the ControlHost that has the specified tag value.
		/// </summary>
		/// <param name="tag">The looked for tag value.</param>
		/// <returns>A ControlHost whose Tag equals to the specified value.</returns>
		public ControlHost FindControlHost(object tag)
		{
			ControlHost foundHost = null;

			// search the host list
			foreach (ControlHost host in controlHosts)
			{
				if (Object.Equals(host.Tag,	tag))
				{
					foundHost = host;
					break;
				}
			}

			return foundHost;
		}

		/// <summary>
		/// Finds the table that has the specified tag value.
		/// </summary>
		/// <param name="tag">The looked for tag value.</param>
		/// <returns>A table whose Tag equals to the specified value.</returns>
		public Table FindTable(object tag)
		{
			Table foundTable = null;

			// search for the Table
			foreach (Table table in tables)
			{
				if (Object.Equals(table.Tag, tag))
				{
					foundTable = table;
					break;
				}
			}

			return foundTable;
		}

		/// <summary>
		/// Finds the arrow that has the specified tag value.
		/// </summary>
		/// <param name="tag">The looked for tag value.</param>
		/// <returns>An arrow whose Tag equals to the specified value.</returns>
		public Arrow FindArrow(object tag)
		{
			Arrow foundArrow = null;

			// search for the arrow
			foreach (Arrow arrow in arrows)
			{
				if (Object.Equals(arrow.Tag, tag))
				{
					foundArrow = arrow;
					break;
				}
			}

			return foundArrow;
		}

		/// <summary>
		/// Finds the group that has the specified tag value.
		/// </summary>
		/// <param name="tag">The looked for tag value.</param>
		/// <returns>A group whose Tag equals to the specified value.</returns>
		public Group FindGroup(object tag)
		{
			Group foundGroup = null;

			// search for the group
			foreach (Group group in groups)
			{
				if (Object.Equals(group.Tag, tag))
				{
					foundGroup = group;
					break;
				}
			}

			return foundGroup;
		}

		#endregion

		#region drawing and printing

		protected override void OnPaint(PaintEventArgs pe)
		{
			if (!Visible) return;
			if (ClientRectangle.Width <= 0 || ClientRectangle.Height <= 0)
				return;

			// create off-screen bitmap 
			Bitmap offScr = new System.Drawing.Bitmap(
				ClientRectangle.Width, ClientRectangle.Height, pe.Graphics);

			// draw document contents
			Rectangle rc = pe.ClipRectangle;
			rc.Inflate(2, 2);
			rc.Intersect(ClientRectangle);
			
			drawFlowChart(offScr, rc, false);

			// blit the off-screen bitmap to the Graphics
			pe.Graphics.DrawImage(offScr, pe.ClipRectangle,
				pe.ClipRectangle.Left, pe.ClipRectangle.Top,
				pe.ClipRectangle.Width, pe.ClipRectangle.Height,
				System.Drawing.GraphicsUnit.Pixel);
			if (hScrollBar != null && vScrollBar != null && (
				hScrollBar.Visible || vScrollBar.Visible))
				pe.Graphics.FillRectangle(SystemBrushes.ScrollBar,
					this.ClientRectangle.Right - vScrollBar.Width,
					this.ClientRectangle.Bottom - hScrollBar.Height,
					vScrollBar.Width, hScrollBar.Height);

			// clean up
			offScr.Dispose();

			// show this so teasing message in trial version
#if DEMO_VERSION
			if (Objects.Count > 80)
			{
				System.Drawing.Brush blackBrush =
					new System.Drawing.SolidBrush(Color.Black);
				Font font = new Font("Arial", 3, GraphicsUnit.Millimeter);

				pe.Graphics.DrawString("FlowChart.NET trial version",
					font, blackBrush, 0, 0);

				font.Dispose();
				blackBrush.Dispose();
			}
#endif

			// Calling the base class OnPaint
			base.OnPaint(pe);
		}

		protected override void OnPaintBackground(System.Windows.Forms.PaintEventArgs pevent)
		{
			// All painting is carried by the control itself
		}

		[DefaultValue(GraphicsUnit.Millimeter)]
		public GraphicsUnit MeasureUnit
		{
			get { return measureUnit; }
			set
			{
				if (value != GraphicsUnit.World)
				{
					measureUnit = value;

					updateHostedControlsPos();
					if (ShowScrollbars)
						resetScrollbars();

					if (routingOptions != null)
						routingOptions.SetDefaultMeasures();

					Invalidate();
					setDirty();

					if (MeasureUnitChanged != null)
						MeasureUnitChanged(this, new EventArgs());
				}
			}
		}

		private GraphicsUnit measureUnit;

		[Category("Appearance")]
		[DefaultValue(SmoothingMode.None)]
		[Description("Anti-aliasing mode.")]
		public SmoothingMode AntiAlias
		{
			get { return antiAlias; }
			set
			{
				if (antiAlias != value && value != SmoothingMode.Invalid)
				{
					antiAlias = value;
					Invalidate();
				}
			}
		}

		private SmoothingMode antiAlias;

		[Category("Appearance")]
		[DefaultValue(typeof(TextRenderingHint), "SystemDefault")]
		[Description("Text rendering mode.")]
		public TextRenderingHint TextRendering
		{
			get { return textRendering; }
			set
			{
				if (textRendering != value)
				{
					textRendering = value;
					Invalidate();
				}
			}
		}

		private TextRenderingHint textRendering;

		[Category("Appearance")]
		[DefaultValue(false)]
		[Description("Specifies whether groups should be sorted by Z order when their items are drawn.")]
		public bool SortGroupsByZ
		{
			get { return sortGroupsByZ; }
			set
			{
				if (sortGroupsByZ != value)
				{
					sortGroupsByZ = value;
					Invalidate();
				}
			}
		}

		private bool sortGroupsByZ;

		[Category("Appearance")]
		[DefaultValue(false)]
		[Description("Enables or disables the DoubleBuffer ControlStyles bit. Enabling it reduces flicker in Remote Desktop Connection sessions.")]
		public bool DoubleBuffer
		{
			get { return GetStyle(ControlStyles.DoubleBuffer); }
			set { SetStyle(ControlStyles.DoubleBuffer, value); }
		}

		/// <summary>
		/// Creates a bitmap containing an image of the flowchart.
		/// </summary>
		/// <returns>A Bitmap instance containing the flowchart image.</returns>
		public Bitmap CreateImage()
		{
			Bitmap bmp = null;
			Graphics g = null;

			try
			{
				RectangleF size = new RectangleF(
					0, 0, docExtents.Width, docExtents.Height);

				g = this.CreateGraphics();
				g.ResetTransform();
				g.PageUnit = measureUnit;
				g.PageScale = zoomFactor / 100F;

				Rectangle rc = Utilities.docToDevice(g, size);
				g.ResetTransform();

				// do our own painting of ControlHost nodes
				displayOptions.PaintControls = true;

				bmp = createBackBuffer(rc, g);
				if (bmp != null)
					drawFlowChart(bmp, rc, false, false);
			}
			finally
			{
				if (g != null)
					g.Dispose();
				displayOptions.PaintControls = false;
			}

			return bmp;
		}

		/// <summary>
		/// Draws a formatted text string, the same way text is drawn in boxes
		/// whose EnableStyledText property is set to true. This method lets you display
		/// styled text in custom-drawn nodes or links.
		/// </summary>
		/// <param name="graphics">A .NET Graphics object used to draw the text.</param>
		/// <param name="text">The formatted text that should be displayed.</param>
		/// <param name="font">The font used to draw the text.</param>
		/// <param name="bounds">The bounds of the text layout rectangle.</param>
		/// <param name="format">Specifies the text alignment</param>
		public void DrawStyledText(Graphics graphics,
			string text, Font font, RectangleF bounds, StringFormat format)
		{
			System.Drawing.SolidBrush brush = new System.Drawing.SolidBrush(TextColor);

			// parse the text
			MindFusion.FlowChartX.Text.StyledText styledText =
				new MindFusion.FlowChartX.Text.StyledText();
			styledText.Setup(text, graphics, font);

			// setup the alignment options
			MindFusion.FlowChartX.Text.LayoutOptions options =
				new MindFusion.FlowChartX.Text.LayoutOptions();
			options.Alignment = format.Alignment;
			options.LineAlignment = format.LineAlignment;

			// lay out the text
			MindFusion.FlowChartX.Text.Layout layout =
				new MindFusion.FlowChartX.Text.Layout();
			layout.LayoutInRectangle(styledText, bounds, options);

			// render it
			MindFusion.FlowChartX.Text.DrawTextHint hint =
				new MindFusion.FlowChartX.Text.DrawTextHint(graphics, font, brush, format,
					(format.Trimming & StringTrimming.EllipsisWord) != 0);
			layout.Draw(0, 0,
				new MindFusion.FlowChartX.Text.RenderTextCallback(DrawText), hint);

			brush.Dispose();
		}

		private void DrawText(string text, RectangleF dest,
			MindFusion.FlowChartX.Text.DrawTextHint hint)
		{
			// Render formatted text
			hint.Graphics.DrawString(
				text, hint.Font, hint.Brush, dest, hint.Format);
		}

		private void drawBack(Graphics g, Rectangle clip, bool toScreen, PrintOptions renderOptions)
		{
			if (imagePos < ImageAlign.Document)
			{
				// the background image does not scroll or scale, the brush shouldn't too
				System.Drawing.Brush fillBrush = brush.CreateGDIBrush(ClientRectangle);
				g.FillRectangle(fillBrush, clip);
				fillBrush.Dispose();
			}
			else
			{
				if (toScreen)
				{
					// now drawing on the screen, include translation for
					// the scroll X and Y position
					setTransforms(g);
				}
				else
				{
					// exporting to bitmap; do not include translation, but just scaling
					g.PageUnit = measureUnit;
					g.PageScale = zoomFactor / 100F;
				}

				RectangleF rc = Utilities.deviceToDoc(g, clip);

				Point pt = Utilities.docToDevice(g, new PointF(0, 0));
				g.RenderingOrigin = pt;

				// paint with exterior brush if applicable
				bool paintExterior = exteriorBrush != null && toScreen &&
					!docExtents.Contains(rc) && !exteriorBrush.equals(BackBrush);
				if (paintExterior)
				{
					System.Drawing.Brush extBrush = exteriorBrush.CreateGDIBrush(
						docExtents, -docExtents.Left, -docExtents.Top);

					// paint the exterior to the left
					if (rc.Left < docExtents.Left)
					{
						RectangleF ext = rc;
						ext.Width = docExtents.Left - ext.Left;
						g.FillRectangle(extBrush, ext);
					}

					// paint the exterior to the top
					if (rc.Top < docExtents.Top)
					{
						RectangleF ext = rc;
						ext.Height = docExtents.Top - ext.Top;
						g.FillRectangle(extBrush, ext);
					}

					// paint the exterior to the right
					if (rc.Right > docExtents.Right)
					{
						RectangleF ext = rc;
						ext.X = Math.Max(rc.Left, docExtents.Right);
						ext.Width = rc.Right - ext.Left;
						g.FillRectangle(extBrush, ext);
					}

					// paint the exterior to the bottom
					if (rc.Bottom > docExtents.Bottom)
					{
						RectangleF ext = rc;
						ext.Y = Math.Max(rc.Top, docExtents.Bottom);
						ext.Height = rc.Bottom - ext.Top;
						g.FillRectangle(extBrush, ext);
					}
					
					extBrush.Dispose();
					rc.Intersect(docExtents);
				}

				// paint the document area with BackBrush
				System.Drawing.Brush fillBrush = brush.CreateGDIBrush(
					docExtents, -docExtents.Left, -docExtents.Top);
				g.FillRectangle(fillBrush, rc);
				fillBrush.Dispose();

				unsetTransforms(g);
			}
		}

		/// <summary>
		/// Recreates the cache bitmap, containing an image of the items that
		/// are not affected by the currently performed modification. This image
		/// is blitted to screen while dragging items, and the modified items
		/// are drawn over it.
		/// </summary>
		public void RecreateCacheImage()
		{
			if (docBuffer != null)
			{
				drawFlowChart(docBuffer, ClientRectangle, true);
				forceCacheRedraw = true;
			}
		}

		private void drawFlowChart(Bitmap bm, Rectangle clip, bool modfBackBuf)
		{
			drawFlowChart(bm, clip, modfBackBuf, true);
		}

		private void drawFlowChart(Bitmap bm, Rectangle clip,
			bool modfBackBuf, bool transforms)
		{
			// now drawing on screen
			renderOptions = displayOptions;
			nowPrinting = false;

			// get the rendering surface
			Graphics g = Graphics.FromImage(bm);

			// fill the background
			if (renderOptions.EnableBackground)
				drawBack(g, clip, transforms, renderOptions);
			g.SmoothingMode = antiAlias;
			g.TextRenderingHint = textRendering;

			// draw the background image
			if (renderOptions.EnableBackgroundImage)
			{
				if (BackgroundImage != null)
					drawImage(g, imagePos, transforms);
			}

			// set the coordinate transformations
			if (transforms)
				setTransforms(g);
			else
			{
				g.TranslateTransform(-docExtents.Left, -docExtents.Top);
				g.PageUnit = measureUnit;
				g.PageScale = zoomFactor / 100F;
			}

			RectangleF rcUpdate = Utilities.deviceToDoc(g, clip);

			// draw grid
			if (showGrid) drawGrid(g, bm, rcUpdate);

			// draw shadows
			if (shadowsStyle == ShadowsStyle.OneLevel)
				drawShadows(g, rcUpdate, modfBackBuf);

			// with SelectionOnTop disabled, do not paint any items if now drawing 
			// in the back buffer image (used for fast repainting when modifying items);
			// they will be all repainted later to preserve the Z order appearance
			if (!(modfBackBuf && !selectionOnTop))
			{
				// draw the items
				drawItems(g, rcUpdate, modfBackBuf);
			}
			
			// clean up
			g.Dispose();
		}

		internal void drawAutoHandles(Graphics g, PointF ptCurr)
		{
			// get the object the mouse is pointing to
			ChartObject obj = GetObjectAt(ptCurr, true);

			// allow for the last 'auto handles' object to stay selected
			// for some time even when it is not under the mouse cursor
			if (obj == null && autoHandlesObj != null && autoHandlesObj is Node)
			{
				Node node = autoHandlesObj as Node;
				RectangleF bounds = node.getRotatedBounds();
				float bufferZone = selHandleSize +
					7 * Constants.getMillimeter(measureUnit);
				bounds.Inflate(bufferZone, bufferZone);
				if (bounds.Contains(ptCurr))
					obj = autoHandlesObj;
			}

			// if it's different than the last pointed redraw the old one
			// and draw the handles of the new one
			if (obj != autoHandlesObj)
			{
				RectangleF rcInv = new RectangleF(0, 0, 0, 0);
				if (autoHandlesObj != null)
					rcInv = autoHandlesObj.getRepaintRect(false);
				if (obj != null)
				{
					rcInv = Utilities.unionNonEmptyRects(rcInv,
						obj.getRepaintRect(false));
				}

				rcInv.Inflate(selHandleSize, selHandleSize);
				autoHandlesObj = obj;
				Rectangle rcInvDev = Utilities.docToDevice(g, rcInv);
				Invalidate(rcInvDev);
			}
		}

		internal void setAutoAnchors(Node node)
		{
			if (node != autoAnchorsObj)
			{
				RectangleF rcInv = new RectangleF(0, 0, 0, 0);
				if (autoAnchorsObj != null)
					rcInv = autoAnchorsObj.getRepaintRect(false);
				if (node != null)
				{
					rcInv = Utilities.unionNonEmptyRects(rcInv,
						node.getRepaintRect(false));
				}

				rcInv.Inflate(Constants.getMarkSize(measureUnit), Constants.getMarkSize(measureUnit));
				autoAnchorsObj = node;

				Graphics g = CreateGraphics();
				setTransforms(g);
				Rectangle rcInvDev = Utilities.docToDevice(g, rcInv);
				g.Dispose();

				Invalidate(rcInvDev);
			}
		}

		private void drawAutoAnchors(Graphics g, PointF ptCurr)
		{
			// get the object the mouse is pointing to
			Node node = GetBoxAt(ptCurr);
			if (node == null)
				node = GetTableAt(ptCurr);

			// if it's different than the last pointed redraw the old one
			// and draw the anchors of the new one
			if (node != autoAnchorsObj)
			{
				RectangleF rcInv = new RectangleF(0, 0, 0, 0);
				if (autoAnchorsObj != null)
					rcInv = autoAnchorsObj.getRepaintRect(false);
				if (node != null)
				{
					rcInv = Utilities.unionNonEmptyRects(rcInv,
						node.getRepaintRect(false));
				}

				rcInv.Inflate(Constants.getMarkSize(measureUnit), Constants.getMarkSize(measureUnit));
				autoAnchorsObj = node;
				Rectangle rcInvDev = Utilities.docToDevice(g, rcInv);
				Invalidate(rcInvDev);
			}
		}

		private RectangleF calcSelectionRect(ChartObject obj, PointF pt)
		{
			RectangleF rc = new RectangleF(0, 0, 0, 0);

			if (obj != null)
			{
				rc = obj.getBoundingRect();
			}
			else
			{
				rc.X = pt.X;
				rc.Y = pt.Y;
			}

			return rc;
		}

		[Browsable(false)]
		public override string Text { get {return "";} set {} }

		[Browsable(false)]
		public override Cursor Cursor
		{ get {return base.Cursor;} set{base.Cursor=value;}}

		internal void setTransforms(Graphics g)
		{
			g.ResetTransform();
			g.TranslateTransform(-scrollX, -scrollY);
			g.PageUnit = measureUnit;
			g.PageScale = zoomFactor / 100F;
		}

		internal void unsetTransforms(Graphics g)
		{
			g.ResetTransform();
			g.PageScale = 1.0F;
			g.PageUnit = GraphicsUnit.Pixel;
		}

		private void drawGrid(Graphics g, Bitmap bmp, RectangleF rc)
		{
			float dx = -Math.Abs(rc.Left-(float)(gridSizeX*Math.Floor(rc.Left / gridSizeX)));
			float dy = -Math.Abs(rc.Top-(float)(gridSizeY*Math.Floor(rc.Top / gridSizeY)));
			if (dx < -gridSizeX/2) dx += gridSizeX;
			if (dy < -gridSizeY/2) dy += gridSizeY;

			if (gridStyle == GridStyle.Points)
			{
				SizeF step = new SizeF(gridSizeX, gridSizeY);
				step = Utilities.docToDeviceF(g, step);
				Rectangle rect = Utilities.docToDevice(g, rc);

				PointF orgDoc = new PointF(
					dx + rc.Left + gridSizeX / 2, dy + rc.Top + gridSizeY / 2);
				PointF orgDev = Utilities.docToDeviceF(g, orgDoc);

				// draw grid points
				for (float x = orgDev.X; x < rect.Right; x += step.Width)
				{
					for (float y = orgDev.Y; y < rect.Bottom; y += step.Height)
					{
						bmp.SetPixel((int)x, (int)y, gridColor);
					}
				}
			}
			else
			{
				// draw grid lines
				System.Drawing.Pen gridPen = new System.Drawing.Pen(gridColor, 0);

				for (float x = dx + rc.Left + gridSizeX / 2; x < rc.Right; x += gridSizeX)
					g.DrawLine(gridPen, x, rc.Top, x, rc.Bottom);

				for (float y = dy + rc.Top + gridSizeY / 2; y < rc.Bottom; y += gridSizeY)
					g.DrawLine(gridPen, rc.Left, y, rc.Right, y);

				gridPen.Dispose();
			}
		}

		private void drawObject(Graphics g, ChartObject obj, bool conn)
		{
			if (conn)
				drawHierarchy(g, obj, false);
			else
				obj.Draw(g, false);
		}


		private void drawObjectWithShadow(Graphics g, ChartObject obj, bool conn)
		{
			// draw item's shadow
			if (conn)
				drawHierarchy(g, obj, true);
			else
				obj.Draw(g, true);

			// draw the item
			drawObject(g, obj, conn);
		}

		private void drawShadows(Graphics g, RectangleF rc, bool modfBackBuf)
		{
			// iterate all types of items
			foreach (ChartObject obj in zOrder)
			{
				RectangleF rcObj = obj.getRepaintRect(false);
				if (!rcObj.IntersectsWith(rc))
					continue;

				if (!(modfBackBuf && obj.getModifying()))
					obj.Draw(g, true);
			}
		}

		private void drawItems(Graphics g, RectangleF clipRect, bool modfBackBuf)
		{
			if (modfBackBuf)
			{
				// draw in the back buffer all items that are not being modified
				// the back buffer will be blitted to the sceen later for faster
				// repainting instead of drawing the items again
				foreach (ChartObject item in zOrder)
				{
					RectangleF rc = item.getRepaintRect(false);
					if (!rc.IntersectsWith(clipRect))
						continue;

					if (!item.getModifying())
					{
						if (shadowsStyle == ShadowsStyle.ZOrder)
							drawObjectWithShadow(g, item, false);
						else
							drawObject(g, item, false);
					}
				}

				// draw the selection
				if (interaction == null ||
					(interaction.CurrentObject != selection && interaction.Action == Action.Create))
					selection.Draw(g, false);
			}
			else
			{
				// draw all items
				foreach (ChartObject item in zOrder)
				{
					if (item != activeObject || !selectionOnTop)
					{
						RectangleF rc = item.getRepaintRect(false);
						if (!rc.IntersectsWith(clipRect))
							continue;

						if (shadowsStyle == ShadowsStyle.ZOrder)
							drawObjectWithShadow(g, item, false);
						else
							drawObject(g, item, false);
					}
				}

				// draw the active object at the top
				if (activeObject != null)
				{
					if (selectionOnTop)
					{
						if (shadowsStyle == ShadowsStyle.ZOrder)
							drawObjectWithShadow(g, activeObject, true);
						else
							drawObject(g, activeObject, true);
					}

					// if anchors are drawn in saSelected mode ...
					if ((showAnchors & ShowAnchors.Selected) != 0 &&
						activeObject is Node)
						(activeObject as Node).drawAnchors(g);
				}

				// if auto-handles drawing is enabled draw the object handles
				if (autoHandlesObj != null && autoHandlesObj != activeObject)
					autoHandlesObj.drawSelHandles(g, selMnpColor);

				// if anchors are drawn in saAuto mode ...
				if (autoAnchorsObj != null && (showAnchors & ShowAnchors.Auto) != 0)
					autoAnchorsObj.drawAnchors(g);

				// object currently being created
				if (interaction != null && interaction.CurrentObject != null &&
					interaction.CurrentObject != activeObject)
					drawObject(g, interaction.CurrentObject, false);

				// draw the selection
				if (interaction == null || interaction.CurrentObject != selection)
					selection.Draw(g, false);
			}
		}

		private void drawAllItems(Graphics g, RectangleF clipRect)
		{
			foreach (ChartObject item in zOrder)
			{
				RectangleF rc = item.getRepaintRect(false);
				if (!rc.IntersectsWith(clipRect)) continue;

				if (shadowsStyle == ShadowsStyle.ZOrder)
					drawObjectWithShadow(g, item, false);
				else
					drawObject(g, item, false);
			}
		}

		private void drawImage(Graphics g, ImageAlign imgPos, bool transforms)
		{
			if (imagePos < ImageAlign.Document)
			{
				RectangleF rcs = new RectangleF(
					ClientRectangle.X, ClientRectangle.Y,
					ClientRectangle.Width, ClientRectangle.Height);
				Utilities.drawImage(g, BackgroundImage, rcs, imgPos);
			}
			else
			{
				if (transforms)
					setTransforms(g);
				else
				{
					g.PageUnit = measureUnit;
					g.PageScale = zoomFactor / 100F;
				}

				Utilities.drawImage(g, BackgroundImage, DocExtents,
					(ImageAlign)(imgPos - ImageAlign.Document));
				unsetTransforms(g);
			}
		}

		private void drawHierarchy(Graphics g, ChartObject obj, bool shadows)
		{
			if (!sortGroupsByZ)
			{
				PainterVisitor painter = new PainterVisitor(g, shadows);
				obj.visitHierarchy(painter);
			}
			else
			{
				MethodCallVisitor visitor =
					new MethodCallVisitor(new VisitOperation(addToSortedList));
				obj.visitHierarchy(visitor);
				foreach (ChartObject item in sortedByZ.Keys)
					item.Draw(g, shadows);
				sortedByZ.Clear();
			}
		}

		private void addToSortedList(ChartObject obj)
		{
			if (!sortedByZ.ContainsKey(obj))
				sortedByZ.Add(obj, null);
		}

		SortedList sortedByZ = new SortedList(new ZOrderComparer());

		[Browsable(true)]
		[Category("Printing")]
		[Description("Print settings.")]
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
		[TypeConverter(typeof(ExpandableObjectConverter))]
		public PrintOptions PrintOptions
		{
			get { return printOptions; }
		}

		[Browsable(true)]
		[Category("Printing")]
		[Description("Print preview settings.")]
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
		[TypeConverter(typeof(ExpandableObjectConverter))]
		public PreviewOptions PreviewOptions
		{
			get { return previewOptions; }
		}

		PrintOptions printOptions;		// for printing
		PrintOptions displayOptions;	// for screen drawing
		PreviewOptions previewOptions;	// for print preview

		// should be set to one of the above
		PrintOptions renderOptions;
		internal PrintOptions RenderOptions
		{
			get { return renderOptions; }
		}

		private PrintOptions prevRenderOptions;
		
		public void SetRenderOptions(PrintOptions options)
		{
			prevRenderOptions = renderOptions;
			renderOptions = options;
		}
		
		public void ResetRenderOptions()
		{
			renderOptions = prevRenderOptions;
		}
		
		private int currentPage = 0;
		private bool nowPrinting = false;
		private RectangleF printRect;
		private PrintDocument printDoc = null;

		internal bool NowPrinting { get { return nowPrinting; }}

		private void BeginPrint(object sender, PrintEventArgs args)
		{
			if (BackgroundImage == null || !printOptions.EnableBackgroundImage)
			{
				// if there is no background image set, print just pages
				// that enclose the bounding rect of the diagram
				printRect = getContentRect(true, true);
			}
			else
			{
				// otherwise print all pages because we need to draw the image
				// as it is on the screen covering the whole document area
				printRect = DocExtents;
			}

			if (printRect.Width == 0 || printRect.Height == 0)
				args.Cancel = true;

			// init page counter
			currentPage = 1;
			if (printDoc.PrinterSettings.PrintRange == PrintRange.SomePages)
				currentPage = Math.Max(1, printDoc.PrinterSettings.FromPage);
		}


		private void PrintPage(object sender, PrintPageEventArgs ev)
		{
			PrinterSettings pset = ev.PageSettings.PrinterSettings;
			GraphicsState state = ev.Graphics.Save();
			PrintHeader(ev);

			// get margins rectangle in device coordinates
			RectangleF mrgnRectDev = Utilities.transformRect(ev.Graphics,
				ev.MarginBounds, CoordinateSpace.Device, CoordinateSpace.World);

			// set Graphics coord. transformation and drawing modes
			ev.Graphics.ResetTransform();
			ev.Graphics.PageUnit = measureUnit;
			ev.Graphics.PageScale = (float)printOptions.Scale / 100f;

			// calculate page size and origin
			RectangleF mrgnRectDoc = Utilities.transformRect(ev.Graphics,
				mrgnRectDev, CoordinateSpace.World, CoordinateSpace.Device);
			int xPages = (int)Math.Ceiling(printRect.Width / mrgnRectDoc.Width);
			int yPages = (int)Math.Ceiling(printRect.Height / mrgnRectDoc.Height);
			float xPrint = printRect.Left +
				((currentPage - 1) % xPages) * mrgnRectDoc.Width;
			float yPrint = printRect.Top +
				((currentPage - 1) / xPages) * mrgnRectDoc.Height;

			// print the page
			ev.Graphics.TranslateTransform(
				mrgnRectDoc.X - xPrint, mrgnRectDoc.Y - yPrint);
			mrgnRectDoc.X = xPrint;
			mrgnRectDoc.Y = yPrint;
			printFlowChart(ev.Graphics, mrgnRectDoc);

			// determine if there are more pages
			currentPage++;
			ev.HasMorePages = currentPage <= xPages * yPages;
			if (pset.PrintRange == PrintRange.SomePages)
				ev.HasMorePages = ev.HasMorePages && currentPage <= pset.ToPage;

			// cleanup
			ev.Graphics.Restore(state);
		}

		internal void printFlowChart(Graphics g, RectangleF pageRect)
		{
			renderOptions = printOptions;
			nowPrinting = true;
			g.SetClip(pageRect);

			// fill the background
			if (renderOptions.EnableBackground)
			{
				if (imagePos < ImageAlign.Document)
				{
					System.Drawing.Brush fillBrush = brush.CreateGDIBrush(pageRect);
					g.FillRectangle(fillBrush, pageRect);
					fillBrush.Dispose();
				}
				else
				{
					System.Drawing.Brush fillBrush = brush.CreateGDIBrush(
						DocExtents, -DocExtents.Left, -DocExtents.Top);
					g.FillRectangle(fillBrush, pageRect);
					fillBrush.Dispose();
				}
			}
			g.SmoothingMode = this.antiAlias;
			g.TextRenderingHint = this.textRendering;

			// print the background image
			if (BackgroundImage != null && renderOptions.EnableBackgroundImage)
			{
				// Printing of back images occur only if ImageAlign.Document is set
				if (imagePos < ImageAlign.Document)
				{
					Utilities.drawImage(g, BackgroundImage, pageRect, imagePos);
				}
				else
				{
					Utilities.drawImage(g, BackgroundImage, DocExtents,
						(ImageAlign)(imagePos - ImageAlign.Document));
				}
			}

			// draw shadows
			if (renderOptions.EnableShadows &&
				shadowsStyle == ShadowsStyle.OneLevel)
				drawShadows(g, pageRect, false);

			// draw all objects
			foreach (ChartObject obj in zOrder)
			{
				if (obj != activeObject || !selectionOnTop)
				{
					RectangleF rc = obj.getRepaintRect(false);
					if (!rc.IntersectsWith(pageRect))
						continue;

					if (renderOptions.EnableShadows &&
						shadowsStyle == ShadowsStyle.ZOrder)
						drawObjectWithShadow(g, obj, false);
					else
						drawObject(g, obj, false);
				}
			}

			// draw the active object at the top
			if (activeObject != null && selectionOnTop)
			{
				if (renderOptions.EnableShadows &&
					shadowsStyle == ShadowsStyle.ZOrder)
					drawObjectWithShadow(g, activeObject, true);
				else
					drawObject(g, activeObject, true);
			}

			renderOptions = displayOptions;
			nowPrinting = false;
		}

		private void PrintHeader(PrintPageEventArgs ev)
		{
			if (printOptions.HeaderFormat != "")
			{
				PointF ptHeader = ev.MarginBounds.Location;
				ptHeader.X += ev.MarginBounds.Width / 2;

				// create the header string
				String header = printOptions.HeaderFormat.Replace(
					"%D", printOptions.DocumentName);
				header = header.Replace("%P", currentPage.ToString());

				// prepare drawing objects and parameters
				Font font = new Font("Arial", 10, GraphicsUnit.Point);
				System.Drawing.SolidBrush brush =
					new System.Drawing.SolidBrush(Color.Black);
				StringFormat format = new StringFormat();
				format.Alignment = StringAlignment.Center;
				format.LineAlignment = StringAlignment.Far;

				// draw the header
				ev.Graphics.DrawString(header, font, brush, ptHeader, format);

				// cleanup
				brush.Dispose();
				font.Dispose();
			}
		}

		/// <summary>
		/// Prints the diagram to the default printer.
		/// </summary>
		public void Print()
		{
			PrintDialog dlg = new PrintDialog();

			dlg.AllowPrintToFile = false;
			dlg.AllowSomePages = true;

			// setup the .NET print document
			dlg.Document = new PrintDocument();
			dlg.Document.DocumentName = printOptions.DocumentName;
			dlg.Document.DefaultPageSettings.Margins = printOptions.Margins;
			dlg.Document.PrinterSettings.MinimumPage = 1;
			dlg.Document.PrinterSettings.MaximumPage = 10000;
			dlg.Document.PrinterSettings.FromPage = 1;
			dlg.Document.PrinterSettings.ToPage = 100;

			if (dlg.ShowDialog(this) == DialogResult.OK)
				Print(dlg.Document);
		}

		private PrintEventHandler beginPrintHandler = null;
		private PrintPageEventHandler printPageHandler = null;

		public void Print(PrintDocument doc)
		{
			printDoc = doc;

			doc.BeginPrint += beginPrintHandler;
			doc.PrintPage += printPageHandler;
			doc.Print();
			doc.PrintPage -= printPageHandler;
			doc.BeginPrint -= beginPrintHandler;
		}

		/// <summary>
		/// Displays the standard .NET print preview form.
		/// </summary>
		public void PrintPreview()
		{
			// setup the .NET print document
			PrintDocument doc = new PrintDocument();
			doc.DocumentName = printOptions.DocumentName;
			doc.DefaultPageSettings.Margins = printOptions.Margins;

			PrintPreview(doc);
		}

		public void PrintPreview(PrintDocument doc)
		{
			printDoc = doc;

			doc.BeginPrint += beginPrintHandler;
			doc.PrintPage += printPageHandler;

			PrintPreviewDialog dlg = new PrintPreviewDialog();
			dlg.Document = doc;
			dlg.ShowDialog(this);

			doc.PrintPage -= printPageHandler;
			doc.BeginPrint -= beginPrintHandler;
		}

		public void PrintPreviewEx(Form previewForm)
		{
			// setup the .NET print document
			PrintDocument doc = new PrintDocument();
			doc.DocumentName = printOptions.DocumentName;
			doc.DefaultPageSettings.Margins = printOptions.Margins;

			printDoc = doc;

			doc.BeginPrint += beginPrintHandler;
			doc.PrintPage += printPageHandler;

			if (previewForm is IPrintPreview)
				(previewForm as IPrintPreview).Document = doc;
			previewForm.ShowDialog(this);

			doc.PrintPage -= printPageHandler;
			doc.BeginPrint -= beginPrintHandler;
		}

		/// <summary>
		/// Displays an extended and better looking print preview form.
		/// </summary>
		public void PrintPreviewEx()
		{
			// setup the .NET print document
			PrintDocument doc = new PrintDocument();
			doc.DocumentName = printOptions.DocumentName;
			doc.DefaultPageSettings.Margins = printOptions.Margins;

			PrintPreviewEx(doc);
		}

		public void PrintPreviewEx(PrintDocument doc)
		{
			printDoc = doc;

			doc.BeginPrint += beginPrintHandler;
			doc.PrintPage += printPageHandler;

			FrmPrintPreview frmPrintPreview =
				new FrmPrintPreview(this, doc.DocumentName);
			frmPrintPreview.Text = doc.DocumentName;
			frmPrintPreview.Document = doc;

			// set preview dialog options
			frmPrintPreview.init(previewOptions);
			frmPrintPreview.ShowDialog(this);

			doc.PrintPage -= printPageHandler;
			doc.BeginPrint -= beginPrintHandler;
		}

		public SizeF MeasureString(string text,
			Font font, int maxWidth, StringFormat textFormat)
		{
			Graphics graphics = CreateGraphics();
			setTransforms(graphics);

			SizeF size = graphics.MeasureString(
				text, font, maxWidth, textFormat);

			graphics.Dispose();
			return size;
		}

		internal void drawActiveSelHandles(Graphics g) 
		{
			if (activeObject != null)
				activeObject.drawSelHandles(g, activeMnpColor);
		}

		internal void invalidate(RectangleF invArea)
		{
			// get graphics for the transformation
			Graphics g = this.CreateGraphics();
			setTransforms(g);

			// get the invalid area in device (pixel) units, and
			// inflate it a bit to accomodate for rounding errors
			Rectangle dev = Utilities.docToDevice(g, invArea);
			dev.Inflate(2, 2);
			this.Invalidate(dev);

			// cleanup
			g.Dispose();
		}

		internal void redrawNonModifiedItems()
		{
			redrawNonModified = true;
		}

		private bool redrawNonModified;

		private bool forceCacheRedraw;

		#endregion

		#region appearance

		[Category("Appearance")]
		[DefaultValue(2.0f)]
		[Description("Size of selection handles.")]
		public float SelHandleSize
		{
			get { return selHandleSize; }
			set
			{
				if (value > 0 && selHandleSize != value)
				{
					selHandleSize = value;
					Invalidate();
				}
			}
		}

		private float selHandleSize;

		[Category("Appearance")]
		[DefaultValue(true)]
		[Description("Specifies if selected items should be painted on top of other items or the Z order should be always preserved.")]
		public bool SelectionOnTop
		{
			get
			{
				return selectionOnTop;
			}
			set
			{
				selectionOnTop = value;
			}
		}

		private bool selectionOnTop;

		[Category("Appearance")]
		[DefaultValue(typeof(ShowAnchors), "Auto")]
		[Description("Gets or sets when anchor point marks are displayed")]
		public ShowAnchors ShowAnchors
		{
			get { return showAnchors; }
			set
			{
				showAnchors = value;
				renderOptions.EnableAnchors =
					(showAnchors == ShowAnchors.Always);
				Invalidate();
			}
		}

		private ShowAnchors showAnchors;

		[Category("Appearance")]
		[DefaultValue(typeof(Color), "170, 170, 200")]
		[Description("The document area defined by DocExtents is painted using this color.")]
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public new Color BackColor
		{
			get { return base.BackColor; }
			set
			{
				if (!base.BackColor.Equals(value))
				{
					base.BackColor = value;

					this.BackBrush = new SolidBrush(value);

					setDirty();
					Invalidate();
				}
			}
		}

		[Category("Appearance")]
		[Description("The document area defined by DocExtents is painted using this brush.")]
		[Editor(typeof(MindFusion.FlowChartX.Design.BrushEditor),
			 typeof(System.Drawing.Design.UITypeEditor))]
		public MindFusion.FlowChartX.Brush BackBrush
		{
			get { return brush; }
			set
			{
				if(value == null)
					throw new Exception("null is not acceptable Brush value");

				if (brush != null)
					brush.Release();
				brush = value;
				brush.AddRef();

				MindFusion.FlowChartX.SolidBrush solidBrush = value as
					MindFusion.FlowChartX.SolidBrush;
				if (solidBrush != null)
					base.BackColor = solidBrush.Color;

				setDirty();
				Invalidate();
			}
		}

		private MindFusion.FlowChartX.Brush brush;

		private bool ShouldSerializeBackBrush()
		{
			return !brush.equals(new SolidBrush(Color.FromArgb(170, 170, 200)));
		}

		[Category("Appearance")]
		[Description("The area outside DocExtents is painted using this brush.")]
		[Editor(typeof(MindFusion.FlowChartX.Design.BrushEditor),
			 typeof(System.Drawing.Design.UITypeEditor))]
		public MindFusion.FlowChartX.Brush ExteriorBrush
		{
			get
			{
				return exteriorBrush;
			}
			set
			{
				if (exteriorBrush != null)
					exteriorBrush.Release();

				exteriorBrush = value;

				if (exteriorBrush != null)
					exteriorBrush.AddRef();

				Invalidate();
			}
		}

		private MindFusion.FlowChartX.Brush exteriorBrush;

		private bool ShouldSerializeExteriorBrush()
		{
			return exteriorBrush != null;
		}

		[Category("Appearance")]
		[DefaultValue(typeof(ExpandButtonPosition), "OuterRight")]
		public ExpandButtonPosition ExpandButtonPosition
		{
			get { return expandBtnPos; }
			set
			{
				if (expandBtnPos != value)
				{
					expandBtnPos = value;
					dirty = true;
					Invalidate();
				}
			}
		}

		private ExpandButtonPosition expandBtnPos;

		[Category("Appearance")]
		[DefaultValue(typeof(ArrowCrossings), "Straight")]
		[Description("Specifies how to display intersection points where arrows cross their paths.")]
		public ArrowCrossings ArrowCrossings
		{
			get { return arrowCrossings; }
			set
			{
				if (arrowCrossings != value)
				{
					arrowCrossings = value;
					clearRuntimeData(Constants.ARROW_CROSSINGS);
					Invalidate();
				}
			}
		}

		private ArrowCrossings arrowCrossings;

		[Category("Appearance")]
		[DefaultValue(1.5f)]
		[Description("Specifies the radius of arcs displayed over intersection points where arrows cross their paths.")]
		public float CrossingRadius
		{
			get { return crossRadius; }
			set
			{
				if (crossRadius != value)
				{
					crossRadius = value;
					clearRuntimeData(Constants.ARROW_CROSSINGS);
					Invalidate();
				}
			}
		}

		private float crossRadius;

		[Category("Appearance")]
		[DefaultValue(1f)]
		[Description("Specifies the relative size of arcs displayed at the corners of rounded rectangles.")]
		public float RoundRectFactor
		{
			get { return roundRectFactor; }
			set
			{
				if (roundRectFactor != value && value >= 0)
				{
					roundRectFactor = value;
					Invalidate();
				}
			}
		}

		private float roundRectFactor;

		internal ArrowCollection getArrowsFromZ(bool lessThan, int z)
		{
			ArrowCollection arrows = new ArrowCollection();

			if (lessThan)
			{
				for (int i = z - 1; i >= 0; i--)
				{
					ChartObject obj = this.zOrder[i];
					if (obj is Arrow)
						arrows.Add(obj as Arrow);
				}
			}
			else
			{
				for (int i = z + 1; i < zOrder.Count; i++)
				{
					ChartObject obj = zOrder[i];
					if (obj is Arrow)
						arrows.Add(obj as Arrow);
				}
			}

			return arrows;
		}

		[DefaultValue(true)]
		[Description("Specifies whether to display selection handles of objects under the mouse while another object is being modified.")]
		public bool ShowHandlesOnDrag
		{
			get { return showHandlesOnDrag; }
			set
			{
				if (showHandlesOnDrag == value)
					return;

				showHandlesOnDrag = value;
				Invalidate();
			}
		}

		private bool showHandlesOnDrag;

		/// <summary>
		/// Gets or sets whether arrow lines have round joins.
		/// Only polyline and cascading arrows regard this property.
		/// </summary>
		[Category("Appearance")]
		[DefaultValue(false)]
		[Description("Specifies whether arrow segment joins should be rounded.")]
		public bool RoundedArrows
		{
			get { return roundedArrows; }
			set
			{
				if (roundedArrows == value)
					return;

				roundedArrows = value;

				Invalidate();
			}
		}

		private bool roundedArrows = false;

		/// <summary>
		/// Gets or sets the curve radius of rounded arrows.
		/// </summary>
		[Category("Appearance")]
		[DefaultValue(2f)]
		[Description("The radius of the arc segment-joins of rounded arrows.")]
		public float RoundedArrowsRadius
		{
			get { return roundedArrowsRadius; }
			set
			{
				if (roundedArrowsRadius == value)
					return;

				roundedArrowsRadius = value;

				Invalidate();
			}
		}

		private float roundedArrowsRadius = 2f;

		/// <summary>
		/// Shadows drawing style
		/// </summary>
		[Category("Appearance")]
		[DefaultValue(typeof(ShadowsStyle), "OneLevel")]
		[Description("Visual style of shadows.")]
		public ShadowsStyle ShadowsStyle
		{
			get
			{
				return shadowsStyle;
			}
			set
			{
				if (shadowsStyle != value)
				{
					shadowsStyle = value;
					setDirty();
					Invalidate();
				}
			}
		}

		private ShadowsStyle shadowsStyle;

		/// <summary>
		/// Defines the placement of the background image
		/// </summary>
		[Category("Appearance")]
		[DefaultValue(typeof(ImageAlign), "Document")]
		[Description("Specifies alignment style and position for the background image.")]
		public ImageAlign BkgrImagePos
		{
			get
			{
				return imagePos;
			}
			set
			{
				if (imagePos != value)
				{
					imagePos = value;
					setDirty();
					Invalidate();
				}
			}
		}

		private ImageAlign imagePos;

		/// <summary>
		/// Color of manipulation handles of the active object
		/// </summary>
		[Category("Appearance")]
		[DefaultValue(typeof(Color), "White")]
		[Description("Manipulation handles of the active object are rendered in this color.")]
		public Color ActiveMnpColor
		{
			get
			{
				return activeMnpColor;
			}
			set
			{
				if (!activeMnpColor.Equals(value))
				{
					activeMnpColor = value;
					setDirty();
					Invalidate();
				}
			}
		}

		private Color activeMnpColor;

		/// <summary>
		/// Color of manipulation handles of the selected objects
		/// </summary>
		[Category("Appearance")]
		[DefaultValue(typeof(Color), "170, 170, 170")]
		[Description("Manipulation handles of selected objects are rendered in this color.")]
		public Color SelMnpColor
		{
			get
			{
				return selMnpColor;
			}
			set
			{
				if (!selMnpColor.Equals(value))
				{
					selMnpColor = value;
					setDirty();
					Invalidate();
				}
			}
		}

		private Color selMnpColor;

		/// <summary>
		/// Color of disabled manipulation handles
		/// </summary>
		[Category("Appearance")]
		[DefaultValue(typeof(Color), "200, 0, 0")]
		[Description("Disabled manipulation handles are rendered in this color.")]
		public Color DisabledMnpColor
		{
			get
			{
				return disabledMnpColor;
			}
			set
			{
				if (!disabledMnpColor.Equals(value))
				{
					disabledMnpColor = value;
					setDirty();
					Invalidate();
				}
			}
		}

		private Color disabledMnpColor;

		[Category("Appearance")]
		[DefaultValue(true)]
		public bool ShowDisabledHandles
		{
			get { return showDisabledHandles; }
			set { showDisabledHandles = value; }
		}

		private bool showDisabledHandles;

		#endregion

		#region user interaction

		protected override void OnKeyDown(KeyEventArgs e)
		{
			base.OnKeyDown(e);

			if (!Enabled) return;

			userAction = true;
			switch (e.KeyCode)
			{
				case Keys.Delete:
					// do not delete anything while creating or modifying
					// otherwise delete the active selected object
					if (interaction == null && activeObject != null &&
						!activeObject.Locked && confirmDelete(activeObject))
					{
						DeleteObject(activeObject);
					}
					break;
			}
			userAction = false;
		}

		bool[] buttonDown = new bool[] { false, false, false };
		Point[] downPos = new Point[] { new Point(0, 0), new Point(0, 0), new Point(0, 0) };

		private int btnNum(MouseButtons btn)
		{
			if (btn == MouseButtons.Left)
				return 0;

			if (btn == MouseButtons.Right)
				return 2;

			return 1;
		}

		internal void RaiseMouseDown(MouseEventArgs e)
		{
			OnMouseDown(e);
		}

		protected override void OnLostFocus(EventArgs e)
		{
			base.OnLostFocus(e);

			focusLost = true;
		}

		private bool focusLost = false;

		/// <summary>
		/// Creates a back-buffer bitmap used for double-buffered drawing of diagrams.
		/// </summary>
		private Bitmap createBackBuffer(Rectangle rect, Graphics compatible)
		{
			Bitmap backBmp = new Bitmap(rect.Width, rect.Height, compatible);
			/*
			if (backBmp != null)
			{
				Graphics backGrfx = Graphics.FromImage(backBmp);
				backGrfx.Clear(BackColor);
				backGrfx.Dispose();
			}
			*/

			return backBmp;
		}

		protected override void OnMouseDown(MouseEventArgs e)
		{
			if (Enabled)
				try { this.Focus(); } 
				catch (SecurityException) {};
			focusLost = false;

			endInplaceEdit(true);
			base.OnMouseDown(e);

			if (!Enabled) return;
			if (focusLost) return;
			if (panMode) return;

			// get mouse position in document coordinates
			Graphics g = this.CreateGraphics();
			setTransforms(g);
			PointF ptMousePos = Utilities.deviceToDoc(g, e.X, e.Y);
			g.Dispose();

			if (manipulatorEnacted(ptMousePos)) return;

			if (e.Clicks > 1)
			{
				ChartObject objUnderMouse = GetObjectAt(ptMousePos, false);
				if (objUnderMouse != null)
				{
					fireDblClickedEvent(objUnderMouse, ptMousePos, e.Button);
				}
				else
				{
					if (DocDblClicked != null)
					{
						DocDblClicked(this, new MousePosArgs(
							e.Button, ptMousePos.X, ptMousePos.Y));
					}
				}

				if (this.Capture)
				{
					this.Capture = false;
					if (docBuffer != null)
					{
						docBuffer.Dispose();
						docBuffer = null;
					}

					interaction = null;
				}

				return;
			}

			int btn = btnNum(e.Button);
			buttonDown[btn] = true;
			downPos[btn] = new Point(e.X, e.Y);
			ptStartDragDev = new Point(e.X, e.Y);

			if (interaction == null)
			{
				// enter pan mode if a modifier key is pressed or a mouse button is mapped to panning
				panMode = ((middleButtonAction == MouseButtonAction.Pan && e.Button == MouseButtons.Middle) ||
					((_modifierKeyActions.GetKeys(ModifierKeyAction.Pan) & ModifierKeys) != 0 && e.Button == MouseButtons.Left));
				if (panMode)
				{
					try { Capture = true; } 
					catch (SecurityException) {};
					panPoint = new PointF(ScrollX, ScrollY);
					Cursor = panCursor;
					return;
				}
			}
			
			// or start drawing - we care only for left-button clicks
			if (e.Button == MouseButtons.Left && Behavior != BehaviorType.DoNothing)
			{
				userAction = true;

				// get mouse position in document coordinates
				g = this.CreateGraphics();
				setTransforms(g);
				ptStartDrag = Utilities.deviceToDoc(g, e.X, e.Y);

				// start dragging
				try {this.Capture = true; } 
				catch (SecurityException) {};
				mouseMoved = false;

				// create or modify an item
				interaction = currentBehavior.startDraw(ptStartDrag, g);
				interaction.start(ptStartDrag, this);

				// save the control background
				docBuffer = createBackBuffer(ClientRectangle, g);
				drawFlowChart(docBuffer, ClientRectangle, true);

				// cleanup
				g.Dispose();
				userAction = false;
			}
		}

		protected override void OnMouseMove(MouseEventArgs e)
		{
			base.OnMouseMove(e);
			if (!Enabled) return;

			if (panMode)
			{
				Point ptDev = new Point(e.X, e.Y);
				int dx = ptDev.X - ptStartDragDev.X;
				int dy = ptDev.Y - ptStartDragDev.Y;

				Graphics gp = this.CreateGraphics();
				gp.ResetTransform();
				gp.PageUnit = measureUnit;
				gp.PageScale = zoomFactor / 100F;
				PointF diff = Utilities.deviceToDoc(gp, dx, dy);
				RectangleF rcPage = Utilities.deviceToDoc(gp, ClientRectangle);
				gp.Dispose();

				float sx = panPoint.X - diff.X;
				float sy = panPoint.Y - diff.Y;
				scrollStayInDoc(sx, sy, rcPage);
				return;
			}

			// get mouse position in document coordinates
			Graphics g = this.CreateGraphics();
			setTransforms(g);
			PointF ptCurr = Utilities.deviceToDoc(g, e.X, e.Y);

			if (interaction != null && interaction.CurrentObject != null &&
				Behavior != BehaviorType.DoNothing && this.Capture && buttonDown[0])
			{
				if (autoScroll) checkAutoScroll(e);

				// for easier selection check if distance the mouse moved is not too small
				if (Math.Abs(ptStartDragDev.X - e.X) > 2 || 
					Math.Abs(ptStartDragDev.Y - e.Y) > 2)
					mouseMoved = true;

				// set the mouse pointer
				if (mouseMoved)
					interaction.setCursor(ptCurr, this);

				// prepare for drawing
				Bitmap tempBuffer = createBackBuffer(ClientRectangle, g);
				Graphics tg = Graphics.FromImage(tempBuffer);
				tg.SmoothingMode = antiAlias;
				tg.TextRenderingHint = textRendering;

				// update the item position
				redrawNonModified = false;
				bool redrawBack = interaction.update(ptCurr, this);
				if (redrawBack || redrawNonModified)
				{
					drawFlowChart(docBuffer, ClientRectangle, true);
					redrawNonModified = false;
				}

				// update document size if needed
				if (autoSizeDoc != MindFusion.FlowChartX.AutoSize.None)
					sizeDocForItem(interaction.CurrentObject);

				// paint the invalid rect
				RectangleF rcInvDoc = interaction.InvalidRect;
				Rectangle rcInvDev = Utilities.docToDevice(g, rcInvDoc);
				rcInvDev.Inflate(2, 2);
				rcInvDev.Intersect(ClientRectangle);
				if (forceCacheRedraw)
				{
					rcInvDev = ClientRectangle;
					forceCacheRedraw = false;
				}
				tg.DrawImage(docBuffer, rcInvDev, rcInvDev, GraphicsUnit.Pixel);

				// redraw the items
				setTransforms(tg);
				if (!selectionOnTop)
				{
					// all items must be repainted in order to
					// preserve their Z order appearance
					drawAllItems(tg, rcInvDoc);

					// item being created
					if (interaction.Action == Action.Create && interaction.CurrentObject != null)
					{
						if (interaction.CurrentObject != selection)
						{
							if (shadowsStyle != ShadowsStyle.None)
								drawObjectWithShadow(tg, interaction.CurrentObject, false);
							else
								drawObject(tg, interaction.CurrentObject, false);
						}
						else
						{
							selection.Draw(tg, false);
						}
					}
				}
				else
				{
					// items that aren't being modified are already painted in the
					// back buffer; now repaint only the items being modified
					if (interaction.CurrentObject != selection)
					{
						if (shadowsStyle != ShadowsStyle.None)
							drawObjectWithShadow(tg, interaction.CurrentObject, interaction.Action == Action.Modify);
						else
							drawObject(tg, interaction.CurrentObject, interaction.Action == Action.Modify);
					}
					else
					{
						// draw selection
						PainterVisitor painter = new PainterVisitor(tg, false);
						selection.visitHierarchy(painter);
						selection.Draw(tg, false);
					}
				}

				if (interaction.Action == Action.Modify && showHandlesOnDrag)
					interaction.CurrentObject.drawSelHandles(tg, ActiveMnpColor);
				if ((showAnchors & ShowAnchors.Auto) != 0 && autoAnchorsObj != null)
					autoAnchorsObj.drawAnchors(tg);

				// blit offscreen to screen
				unsetTransforms(g);
				Rectangle clr = ClientRectangle;
				if (hScrollBar != null && hScrollBar.Visible)
					clr.Height -= hScrollBar.Height;
				if (vScrollBar != null && vScrollBar.Visible)
					clr.Width -= vScrollBar.Width;
				rcInvDev.Intersect(clr);
				g.DrawImage(tempBuffer, rcInvDev, rcInvDev, GraphicsUnit.Pixel);

				// clean up
				tg.Dispose();
				tempBuffer.Dispose();
			}
			else // not dragging
			{
				if ((showAnchors & ShowAnchors.Auto) != 0)
					drawAutoAnchors(g, ptCurr);
				if (modificationStart == ModificationStyle.AutoHandles)
					drawAutoHandles(g, ptCurr);
				currentBehavior.SetMouseCursor(ptCurr);
				setTooltipText(ptCurr);
			}

			// clean up
			g.Dispose();
		}

		protected override void OnMouseUp(MouseEventArgs e)
		{
#if VERTIGO
			if (!DesignMode && !loadedFromVertigoAssembly())
				throw new Exception("This FlowChart.NET version can be used only " +
					"with the CodePlex Vertigo project.");
#endif

			base.OnMouseUp(e);

			// stop autoscrolling
			if (scrollTimer != null)
			{
				scrollTimer.Enabled = false;
				scrollTimer = null;
			}

			// stop pan mode if it's active now
			if (panMode)
			{
				panMode = mouseMoved = false;
				buttonDown[btnNum(e.Button)] = false;
				if (Capture)
					Capture = false;
				Cursor = curPointer;
				return;
			}

			if (!Enabled) return;
			userAction = true;

			Graphics g = this.CreateGraphics();
			setTransforms(g);

			// get mouse position in document coordinates
			PointF ptCurr = Utilities.deviceToDoc(g, e.X, e.Y);
			PointF ptMousePos = ptCurr;

			if (interaction != null && Behavior != BehaviorType.DoNothing &&
				this.Capture && buttonDown[0])
			{
				ChartObject currObject = interaction.CurrentObject;
				RectangleF rcInv = interaction.InvalidRect;

				// should the operation be performed?
				if (e.Button != MouseButtons.Left || !mouseMoved || !interaction.isAllowed(ptCurr))
				{
					// cancel the operation
					interaction.cancel(this);

					// if the mouse wasn't moved, select the object under cursor
					if (!mouseMoved && (interaction.Action == Action.Create || interaction.Action == Action.Split))
					{
						ChartObject obj = GetObjectAt(ptCurr, true);
						if (obj == null || confirmSelect(obj))
						{
							RectangleF rcOld = calcSelectionRect(activeObject, ptCurr);
							rcInv = Utilities.unionRects(rcInv, rcOld);
							rcInv = Utilities.unionNonEmptyRects(
								rcInv, selection.getRepaintRect(false));

							int oldSelCount = selection.GetSize();

							// change selection
							if ((_modifierKeyActions.GetKeys(ModifierKeyAction.Select) & ModifierKeys) != 0)
								selection.Toggle(obj);
							else
								selection.Change(obj);

							int newSelCount = selection.GetSize();

							rcInv = Utilities.unionRects(rcInv,
								selection.getRepaintRect(false));
							rcInv = Utilities.unionRects(rcInv,
								calcSelectionRect(activeObject, ptCurr));
							rcInv.Inflate(selHandleSize, selHandleSize);

							if (!(oldSelCount == 0 && newSelCount == 0))
								dirty = true;

						}	// if confirmSelect()

					}	// if !mouseMoved

				}	// if (e.Button != MouseButtons.Left)

				else
				{
					// complete operation
					interaction.complete(ptCurr, this);
					rcInv = interaction.InvalidRect;

					// update document size if needed
					if (autoSizeDoc != MindFusion.FlowChartX.AutoSize.None)
						sizeDocForItem(currObject);
				}

				Rectangle rcInvDev = Utilities.docToDevice(g, rcInv);
				rcInvDev.Inflate(2, 2);
				this.Invalidate(rcInvDev, false);

				this.Capture = false;
				docBuffer.Dispose();
				docBuffer = null;

				interaction = null;
			}

			g.Dispose();

			int btn = btnNum(e.Button);
			if (buttonDown[btn] &&
				Math.Abs(downPos[btn].X - e.X) <= 2 &&
				Math.Abs(downPos[btn].Y - e.Y) <= 2)
			{
				ChartObject objUnderMouse = GetObjectAt(ptMousePos, false);
				if (objUnderMouse != null)
				{
					fireClickedEvent(objUnderMouse, ptMousePos, e.Button);
				}
				else
				{
					if (DocClicked != null)
					{
						DocClicked(this, new MousePosArgs(
							e.Button, ptMousePos.X, ptMousePos.Y));
					}
				}
			}
			buttonDown[btn] = false;
			userAction = false;
		}
		
		protected override void OnDragOver(System.Windows.Forms.DragEventArgs drgevent)
		{
			base.OnDragOver(drgevent);
			if (!Enabled) return;

			// get mouse position in document coordinates
			Graphics g = this.CreateGraphics();
			setTransforms(g);
			PointF ptCurr = Utilities.deviceToDoc(g, drgevent.X, drgevent.Y);

			if ((showAnchors & ShowAnchors.Auto) != 0)
				drawAutoAnchors(g, ptCurr);

			// clean up
			g.Dispose();
		}

		private PointF ptStartDrag;
		private Point ptStartDragDev;
		internal bool mouseMoved;
		private Bitmap docBuffer;

		internal PointF InteractionStartPoint { get {return ptStartDrag;} }

		public PointF AlignPointToGrid(PointF point)
		{
			if (alignToGrid)
			{
				float dx = (float)(point.X - gridSizeX*Math.Floor(point.X / gridSizeX));
				float dy = (float)(point.Y - gridSizeY*Math.Floor(point.Y / gridSizeY));
				return new PointF(
					point.X - dx + gridSizeX / 2,
					point.Y - dy + gridSizeY / 2);
			}

			return point;
		}

		bool manipulatorEnacted(PointF pt)
		{
			userAction = true;

			// try all manipulators
			foreach (ChartObject obj in zOrder)
			{
				if (!obj.notInteractive() && obj.manipulatorEnacted(pt))
				{
					userAction = false;
					return true;
				}
			}

			userAction = false;
			return false;
		}

		private bool userAction;

		private bool panMode;
		PointF panPoint;

		/// <summary>
		/// Gets a value specifying whether the diagram
		/// is currently being modified by the user.
		/// </summary>
		[Browsable(false)]
		public bool IsModifying
		{
			get { return interaction != null && interaction.Action != Action.None; }
		}

		[Browsable(false)]
		public ModifierKeyActions ModifierKeyActions
		{
			get { return _modifierKeyActions; }
		}

		private ModifierKeyActions _modifierKeyActions;

		[Category("Inplace text editing")]
		[DefaultValue(false)]
		[Description("Allows activating inplace editing mode by double clicking.")]
		public bool AllowInplaceEdit
		{
			get { return inplaceEditAllowed; }
			set
			{
				if (inplaceEditAllowed != value)
				{
					inplaceEditAllowed = value;
					setDirty();
				}
			}
		}

		private bool inplaceEditAllowed;

		TextBox inplaceTextBox = null;

		internal void startInplaceEdit(InplaceEditable obj, RectangleF rect)
		{
			if (nowEditing) return;

			inplaceObject = obj;
			string text = obj.getTextToEdit();

			// get the position of the object in device coordinates
			Graphics g = this.CreateGraphics();
			setTransforms(g);
			Rectangle rc = Utilities.docToDevice(g, rect);

			// setup the inplace text box there
			inplaceTextBox = new TextBox();
			inplaceTextBox.Multiline = true;
			inplaceTextBox.AcceptsReturn = true;
			inplaceTextBox.Visible = true;
			inplaceTextBox.Bounds = rc;
			inplaceTextBox.Font = inplaceEditFont;
			inplaceTextBox.Text = text;
			inplaceTextBox.KeyPress +=
				new KeyPressEventHandler(inplaceTextBoxKeyPress);
			Controls.Add(inplaceTextBox);
			inplaceTextBox.Focus();

			nowEditing = true;

			// raise the EnterInplaceEditMode event
			if (EnterInplaceEditMode != null)
			{
				EnterInplaceEditMode(this,
					new InplaceEditArgs(inplaceObject, inplaceTextBox));
			}
		}


		internal void endInplaceEdit(bool accept)
		{
			if (!nowEditing)
				return;

			// raise the LeaveInplaceEditMode event
			if (LeaveInplaceEditMode != null)
			{
				LeaveInplaceEditMode(this,
					new InplaceEditArgs(inplaceObject, inplaceTextBox));
			}

			// get texts while the text box still exists
			string oldText = inplaceObject.getTextToEdit();
			string newText = inplaceTextBox.Text;

			// remove text box
			inplaceTextBox.Visible = false;
			Controls.Remove(inplaceTextBox);
			inplaceTextBox = null;

			// end inplace editing
			InplaceEditable lastIO = inplaceObject;
			inplaceObject = null;
			nowEditing = false;

			if (accept)
			{
				lastIO.setEditedText(newText);

				// fire event
				if (lastIO is NodeInplaceEditable)
				{
					Node node = ((NodeInplaceEditable)lastIO).Node;
					if (node is Box && BoxTextEdited != null)
						BoxTextEdited(this, new BoxTextArgs((Box)node, oldText, newText));
					if (node is Table && TableCaptionEdited != null)
						TableCaptionEdited(this, new TableCaptionArgs(
							(Table)node, oldText, newText));
				}
				if (lastIO is Table.Cell)
				{
					Table.Cell cell = (Table.Cell)lastIO;
					if (CellTextEdited != null)
						CellTextEdited(this, new CellTextArgs(
							cell.table, oldText, cell.Text, lastClickedCol, lastClickedRow));
				}
			}
		}


		void inplaceTextBoxKeyPress(object o, KeyPressEventArgs args)
		{
			if (inplaceAcceptOnEnter && args.KeyChar == (char)13)
			{
				endInplaceEdit(true);
				args.Handled = true;
			}

			if (inplaceCancelOnEsc && args.KeyChar == (char)27)
			{
				endInplaceEdit(false);
				args.Handled = true;
			}
		}

		[Category("Inplace text editing")]
		[DefaultValue(false)]
		[Description("End inplace editing and accept changes when ENTER is pressed.")]
		public bool InplaceEditAcceptOnEnter
		{
			get { return inplaceAcceptOnEnter; }
			set { inplaceAcceptOnEnter = value; }
		}


		[Category("Inplace text editing")]
		[DefaultValue(true)]
		[Description("End inplace editing and cancel changes when ESC is pressed.")]
		public bool InplaceEditCancelOnEsc
		{
			get { return inplaceCancelOnEsc; }
			set { inplaceCancelOnEsc = value; }
		}


		/// <summary>
		/// Font of inplace text box 
		/// </summary>
		[Category("Inplace text editing")]
		[Description("Font of text being edited inplace.")]
		public Font InplaceEditFont
		{
			get
			{
				return inplaceEditFont;
			}
			set
			{
				if (inplaceEditFont != value)
				{
					inplaceEditFont = value;
					setDirty();
				}
			}
		}

		private Font inplaceEditFont;


		private bool confirmTableInplaceEdit(Table tbl, int row, int col)
		{
			if (TableInplaceEditing == null) return true;

			TableConfirmArgs args = new TableConfirmArgs(tbl, row, col);
			TableInplaceEditing(this, args);
			return args.Confirm;
		}


		private bool confirmBoxInplaceEdit(Box box)
		{
			if (BoxInplaceEditing == null) return true;

			BoxConfirmArgs args = new BoxConfirmArgs(box);
			BoxInplaceEditing(this, args);
			return args.Confirm;
		}


		private InplaceEditable inplaceObject;

		bool nowEditing;
		bool inplaceAcceptOnEnter;
		bool inplaceCancelOnEsc;

		int lastClickedRow;
		int lastClickedCol;

		/// <summary>
		/// Activates the grid
		/// </summary>
		[Category("Grid")]
		[DefaultValue(true)]
		[Description("Specifies whether to align items to grid points.")]
		public bool AlignToGrid
		{
			get
			{
				return alignToGrid;
			}
			set
			{
				if (alignToGrid != value)
				{
					alignToGrid = value;
					setDirty();
				}
			}
		}

		private bool alignToGrid;

		/// <summary>
		/// Defines the visibility of the grid
		/// </summary>
		[Category("Grid")]
		[DefaultValue(false)]
		[Description("Shows or hides the grid points.")]
		public bool ShowGrid
		{
			get
			{
				return showGrid;;
			}
			set
			{
				if (showGrid != value)
				{
					showGrid = value;
					setDirty();
					Invalidate();
				}
			}
		}

		private bool showGrid;

		/// <summary>
		/// The color of the grid points
		/// </summary>
		[Category("Grid")]
		[DefaultValue(typeof(Color), "140, 140, 150")]
		[Description("Gets or sets the color used to paint grid dots or lines.")]
		public Color GridColor
		{
			get
			{
				return gridColor;
			}
			set
			{
				if (gridColor != value)
				{
					gridColor = value;
					setDirty();
					Invalidate();
				}
			}
		}

		private Color gridColor;

		/// <summary>
		/// The horizontal distance between adjacent grid points.
		/// </summary>
		[Category("Grid")]
		[DefaultValue(4.0f)]
		[Description("The horizontal distance between grid points.")]
		public float GridSizeX
		{
			get
			{
				return gridSizeX;
			}
			set
			{
				if (gridSizeX != value)
				{
					gridSizeX = value;
					setDirty();
					Invalidate();
				}
			}
		}

		private float gridSizeX;

		/// <summary>
		/// The vertical distance between adjacent grid points.
		/// </summary>
		[Category("Grid")]
		[DefaultValue(4.0f)]
		[Description("The vertical distance between grid points.")]
		public float GridSizeY
		{
			get
			{
				return gridSizeY;
			}
			set
			{
				if (gridSizeY != value)
				{
					gridSizeY = value;
					setDirty();
					Invalidate();
				}
			}
		}

		private float gridSizeY;

		/// <summary>
		/// Indicates how to draw the grid
		/// </summary>
		[
		Category("Grid"),
		DefaultValue(typeof(GridStyle), "Points"),
		Description("Indicates how to draw the grid.")
		]
		public GridStyle GridStyle
		{
			get
			{
				return gridStyle;
			}
			set
			{
				if (gridStyle != value)
				{
					gridStyle = value;
					Invalidate();
				}
			}
		}

		private GridStyle gridStyle;

		/// <summary>
		/// Enables tool-tips
		/// </summary>
		[Category("Tooltips")]
		[DefaultValue(true)]
		[Description("Specifies if tooltips are displayed when mouse hovers over an object.")]
		public bool ShowToolTips
		{
			get
			{
				return showToolTips;
			}
			set
			{
				showToolTips = value;
				if (!this.DesignMode)
					toolTipCtrl.Active = showToolTips;
			}
		}

		bool showToolTips;

		private ToolTip toolTipCtrl;

		private void setTooltipText(PointF docPt)
		{
			ChartObject cobj = GetObjectAt(docPt, false);

			// display tooltip if text not empty
			string text;
			if (cobj != null)
				text = cobj.getToolTip(docPt);
			else
				text = this.ToolTip;

			if (toolTipCtrl.GetToolTip(this) != text)
			{
				toolTipCtrl.SetToolTip(this, "");
				toolTipCtrl.SetToolTip(this, text);
			}
		}

		[Category("Tooltips")]
		[DefaultValue("")]
		public string ToolTip
		{
			get
			{
				return toolTip;
			}
			set
			{
				if (toolTip != value)
				{
					if (value == null)
						toolTip = "";
					else
						toolTip = value;

					setDirty();
				}
			}
		}

		private string toolTip;

		[Category("Tooltips")]
		[DefaultValue(500)]
		[Description("Gets or sets the delay for tooltips in milliseconds.")]
		public int ToolTipDelay
		{
			get { return toolTipCtrl.AutomaticDelay; }
			set { toolTipCtrl.AutomaticDelay = value; }
		}

		[Category("Mouse cursors")]
		[Description("Cursor displayed when the mouse pointer is over empty document area.")]
		public Cursor CurPointer
		{
			get { return curPointer; }
			set { curPointer = value; }
		}
		private Cursor curPointer;

		private bool ShouldSerializeCurPointer()
		{
			return curPointer != Cursors.Arrow;
		}

		[Category("Mouse cursors")]
		[Description("Cursor displayed when an object cannot be created.")]
		public Cursor CurCannotCreate
		{
			get { return curCannotCreate; }
			set { curCannotCreate = value; }
		}
		private Cursor curCannotCreate;

		private bool ShouldSerializeCurCannotCreate()
		{
			return curCannotCreate != Cursors.No;
		}

		[Category("Mouse cursors")]
		[Description("Cursor displayed when an object can be modified.")]
		public Cursor CurModify
		{
			get { return curModify; }
			set { curModify = value; }
		}
		private Cursor curModify;

		private bool ShouldSerializeCurModify()
		{
			return curModify != Cursors.SizeAll;
		}

		[Category("Mouse cursors")]
		[Description("Cursor displayed when dragging the mouse will create an arrow.")]
		public Cursor CurArrowStart
		{
			get { return curArrowStart; }
			set { curArrowStart = value; }
		}
		private Cursor curArrowStart;

		private bool ShouldSerializeCurArrowStart()
		{
			return curArrowStart != Cursors.Hand;
		}

		[Category("Mouse cursors")]
		[Description("Cursor displayed when an arrow can be created.")]
		public Cursor CurArrowEnd
		{
			get { return curArrowEnd; }
			set { curArrowEnd = value; }
		}
		private Cursor curArrowEnd;

		private bool ShouldSerializeCurArrowEnd()
		{
			return curArrowEnd != Cursors.Hand;
		}

		[Category("Mouse cursors")]
		[Description("Cursor displayed if an arrow cannot be created.")]
		public Cursor CurArrowCannotCreate
		{
			get { return curArrowCannotCreate; }
			set { curArrowCannotCreate = value; }
		}
		private Cursor curArrowCannotCreate;

		private bool ShouldSerializeCurArrowCannotCreate()
		{
			return curArrowCannotCreate != Cursors.No;
		}

		[Category("Mouse cursors")]
		[Description("Indicates that an object would be resized horizontally.")]
		public Cursor CurHorzResize
		{
			get { return curHorzResize; }
			set { curHorzResize = value; }
		}
		private Cursor curHorzResize;

		private bool ShouldSerializeCurHorzResize()
		{
			return curHorzResize != Cursors.SizeWE;
		}

		[Category("Mouse cursors")]
		[Description("Indicates that dragging a selection handle would rotate the selected box.")]
		public Cursor CurRotateShape
		{
			get { return curRotateShape; }
			set { curRotateShape = value; }
		}
		private Cursor curRotateShape;

		private void ResetCurRotateShape()
		{
			curRotateShape = CustomCursors.Rotate;
		}

		private bool ShouldSerializeCurRotateShape()
		{
			// HACK: just comparing curRotateShape with CustomCursors.Rotate never
			// seems to work, so use the pretty piece of code below

			// these should be all Cursors members
			Cursor[] cursors = new Cursor[] {
												Cursors.AppStarting,
												Cursors.Arrow,
												Cursors.Cross,
												Cursors.Default,
												Cursors.Hand,
												Cursors.Help,
												Cursors.HSplit,
												Cursors.IBeam,
												Cursors.No,
												Cursors.NoMove2D,
												Cursors.NoMoveHoriz,
												Cursors.NoMoveVert,
												Cursors.PanEast,
												Cursors.PanNE,
												Cursors.PanNorth,
												Cursors.PanNW,
												Cursors.PanSE,
												Cursors.PanSouth,
												Cursors.PanSW,
												Cursors.PanWest,
												Cursors.SizeAll,
												Cursors.SizeNESW,
												Cursors.SizeNS,
												Cursors.SizeNWSE,
												Cursors.SizeWE,
												Cursors.UpArrow,
												Cursors.VSplit,
												Cursors.WaitCursor};

			foreach (Cursor cursor in cursors)
			{
				if (cursor == curRotateShape)
					return true;
			}

			return false;
		}

		[Category("Mouse cursors")]
		[Description("Indicates that an object would be resized vertically.")]
		public Cursor CurVertResize
		{
			get { return curVertResize; }
			set { curVertResize = value; }
		}
		private Cursor curVertResize;

		private bool ShouldSerializeCurVertResize()
		{
			return curVertResize != Cursors.SizeNS;
		}

		[Category("Mouse cursors")]
		[Description("Displayed while panning the view.")]
		public Cursor PanCursor
		{
			get { return panCursor; }
			set { panCursor = value; }
		}
		private Cursor panCursor;

		private bool ShouldSerializePanCursor()
		{
			return panCursor != Cursors.NoMove2D;
		}

		[Category("Mouse cursors")]
		[Description("Indicates that an object would be resized in both directions.")]
		public Cursor CurMainDgnlResize
		{
			get { return curMainDgnlResize; }
			set { curMainDgnlResize = value; }
		}
		private Cursor curMainDgnlResize;

		private bool ShouldSerializeCurMainDgnlResize()
		{
			return curMainDgnlResize != Cursors.SizeNWSE;
		}

		[Category("Mouse cursors")]
		[Description("Indicates that an object would be resized in both directions.")]
		public Cursor CurSecDgnlResize
		{
			get { return curSecDgnlResize; }
			set { curSecDgnlResize = value; }
		}
		private Cursor curSecDgnlResize;

		private bool ShouldSerializeCurSecDgnlResize()
		{
			return curSecDgnlResize != Cursors.SizeNESW;
		}

		[Browsable(false)]
		public Cursor CurrentCursor
		{
			get { return this.Cursor; }
			set
			{
				this.Cursor = value;
				this.getCurrBehavior().setCurrentCursor(value);
			}
		}

		[Category("Behavior")]
		[DefaultValue(0f)]
		[Description("The maximum distance between adjacent control points of an arrow at which the respective segments can be merged.")]
		public float MergeThreshold
		{
			get { return mergeThreshold; }
			set { mergeThreshold = value; }
		}

		internal bool mergePoints(PointF point1, PointF point2)
		{
			return
				Math.Abs(point1.X - point2.X) <= mergeThreshold &&
				Math.Abs(point1.Y - point2.Y) <= mergeThreshold;
		}

		private float mergeThreshold;

		[Category("Behavior")]
		[DefaultValue(typeof(MouseButtonAction), "None")]
		[Description("Specifies the function of the middle mouse button.")]
		public MouseButtonAction MiddleButtonAction
		{
			get { return middleButtonAction; }
			set { middleButtonAction = value; }
		}

		private MouseButtonAction middleButtonAction;

		#endregion

		#region behavior

		/// <summary>
		/// Automatically select object after it is created
		/// </summary>
		[Category("Behavior")]
		[DefaultValue(true)]
		[Description("Determines whether newly-created objects are automatically selected")]
		public bool SelectAfterCreate
		{
			get
			{
				return selectAfterCreate;
			}
			set
			{
				if (selectAfterCreate != value)
				{
					selectAfterCreate = value;
					setDirty();
				}
			}
		}

		bool selectAfterCreate;

		/// <summary>
		/// Restriction of the object positions in the document extents
		/// </summary>
		[Category("Behavior")]
		[DefaultValue(typeof(RestrictToDoc), "Intersection")]
		[Description("Specifies whether users can create or drag objects outside document boundaries.")]
		public RestrictToDoc RestrObjsToDoc
		{
			get
			{
				return restrObjsToDoc;
			}
			set
			{
				if (restrObjsToDoc != value)
				{
					restrObjsToDoc = value;
					setDirty();
				}
			}
		}

		private RestrictToDoc restrObjsToDoc;

		/// <summary>
		/// Defines how the user can start object modification
		/// </summary>
		[Category("Behavior")]
		[DefaultValue(typeof(ModificationStyle), "SelectedOnly")]
		[Description("Specifies if objects have to be selected in order to modify them.")]
		public ModificationStyle ModificationStart
		{
			get
			{
				return modificationStart;
			}
			set
			{
				if (modificationStart != value)
				{
					modificationStart = value;
					setDirty();
					autoHandlesObj = null;
				}
			}
		}

		private ModificationStyle modificationStart;

		[Category("Behavior")]
		[DefaultValue(typeof(HitTestPriority), "NodesBeforeArrows")]
		[Description("Specifies whether nodes should have higher priority than links when hit testing.")]
		public HitTestPriority HitTestPriority
		{
			get { return hitTestPriority; }
			set	{ hitTestPriority = value; }
		}

		private HitTestPriority hitTestPriority;

		[Category("Behavior")]
		[DefaultValue(false)]
		[Description("Specifies the direction in which node hierarchies are expanded and collapsed.")]
		public bool ExpandOnIncoming
		{
			get { return expandOnIncoming; }
			set
			{	
				if (expandOnIncoming != value)
				{
					expandOnIncoming = value;
					setDirty();
				}
			}
		}
		private bool expandOnIncoming;

		[Category("Behavior")]
		[DefaultValue(false)]
		[Description("Enables recursive multi-level expanding of node hierarchies.")]
		public bool RecursiveExpand
		{
			get { return recursiveExpand; }
			set
			{
				if (recursiveExpand != value)
				{
					recursiveExpand = value;
					setDirty();
				}
			}
		}

		private bool recursiveExpand;

		[Category("Behavior")]
		[DefaultValue(true)]
		[Description("Specifies whether two nodes can be connected one to another with more than one link.")]
		public bool AllowLinksRepeat
		{
			get { return allowLinksRepeat; }
			set
			{
				if (allowLinksRepeat != value)
				{
					allowLinksRepeat = value;
					setDirty();
				}
			}
		}

		private bool allowLinksRepeat;

		[Category("Behavior")]
		[DefaultValue(typeof(SnapToAnchor), "OnCreate")]
		[Description("Specifies when arrow ends snap to nearest anchor point")]
		public SnapToAnchor SnapToAnchor
		{
			get { return snapToAnchor; }
			set { snapToAnchor = value; }
		}

		private SnapToAnchor snapToAnchor;

		[Category("Behavior")]
		[DefaultValue(false)]
		[Description("Allow drawing arrows without connecting them to nodes.")]
		public bool AllowUnconnectedArrows
		{
			get { return allowUnconnectedArrows; }
			set { allowUnconnectedArrows = value; }
		}

		private bool allowUnconnectedArrows;

		[Category("Behavior")]
		[DefaultValue(true)]
		[Description("Allow attaching arrows to nodes that have not anchor points.")]
		public bool AllowUnanchoredArrows
		{
			get { return allowUnanchoredArrows; }
			set { allowUnanchoredArrows = value; }
		}

		private bool allowUnanchoredArrows;

		[Category("Behavior")]
		[DefaultValue(typeof(MindFusion.FlowChartX.AutoSize), "None")]
		[Description("Automatically resize document scrolling area when items are created, deleted or removed.")]
		public MindFusion.FlowChartX.AutoSize AutoSizeDoc
		{
			get { return autoSizeDoc; }
			set { autoSizeDoc = value; }
		}

		private MindFusion.FlowChartX.AutoSize autoSizeDoc;

		private void sizeDocForItem(ChartObject item)
		{
			if (item == null)
			{
				sizeDocForItems();
				return;
			}

			bool changed = false;
			float docLeft = docExtents.Left;
			float docTop = docExtents.Top;
			float docRight = docExtents.Right;
			float docBottom = docExtents.Bottom;

			RectangleF rcItem = item.getRepaintRect(true);
			float hinch = Constants.getHalfInch(measureUnit);

			// if auto-sizing to the left and top too...
			if (autoSizeDoc == MindFusion.FlowChartX.AutoSize.AllDirections)
			{
				if (rcItem.Left < docLeft + hinch)
				{
					docLeft = rcItem.Left - hinch;
					changed = true;
				}

				if (rcItem.Top < docTop + hinch)
				{
					docTop = rcItem.Top - hinch;
					changed = true;
				}
			}

			if (rcItem.Right > docRight - hinch)
			{
				docRight = rcItem.Right + hinch;
				changed = true;
			}

			if (rcItem.Bottom > docBottom - hinch)
			{
				docBottom = rcItem.Bottom + hinch;
				changed = true;
			}

			if (changed)
			{
				docExtents = RectangleF.FromLTRB(docLeft, docTop, docRight, docBottom);
				resetDocSize();
			}
		}

		private void sizeDocForItems()
		{
			RectangleF rcNewSize = docExtentsMin;
			RectangleF rcCurrent = docExtents;

			foreach (ChartObject item in zOrder)
			{
				RectangleF itemRect = item.getRepaintRect(false);
				rcNewSize = RectangleF.Union(rcNewSize, itemRect);
			}

			if (rcNewSize != rcCurrent)
			{
				docExtents = RectangleF.FromLTRB(
					docExtents.Left, docExtents.Top,
					rcNewSize.Right, rcNewSize.Bottom);
				resetDocSize();
			}
		}

		[Category("Behavior")]
		[DefaultValue(true)]
		[Description("Specifies whether arrow end points can be moved after the arrow is created.")]
		public bool ArrowEndsMovable
		{
			get { return arrowEndsMovable; }
			set { arrowEndsMovable = value;	}
		}

		private bool arrowEndsMovable;

		[Category("Behavior")]
		[DefaultValue(false)]
		[Description("Specifies whether arrow segments can be added and removed interactively.")]
		public bool AllowSplitArrows
		{
			get { return allowSplitArrows; }
			set { allowSplitArrows = value;	}
		}

		private bool allowSplitArrows;

		/// <summary>
		/// Sets the control behavior on user`s actions
		/// </summary>
		[Category("Behavior")]
		[DefaultValue(typeof(BehaviorType), "FlowChart")]
		[Description("Specifies how the control responds to users actions with the mouse.")]
		public BehaviorType Behavior
		{
			get
			{
				return behavior;
			}
			set
			{
				behavior = value;
				switch (behavior)
				{
					case BehaviorType.Modify:
					case BehaviorType.DoNothing:
						currentBehavior = new ModifyBehavior(this);
						break;
					case BehaviorType.FlowChart:
						currentBehavior = new FlowChartBehavior(this);
						break;
					case BehaviorType.TableRelations:
						currentBehavior = new TableRelationsBehavior(this);
						break;
					case BehaviorType.CreateArrow:
						selection.Clear();
						currentBehavior = new CreateArrowBehavior(this);
						break;
					case BehaviorType.CreateBox:
						selection.Clear();
						currentBehavior = new CreateBoxBehavior(this);
						break;
					case BehaviorType.CreateTable:
						selection.Clear();
						currentBehavior = new CreateTableBehavior(this);
						break;
					case BehaviorType.CreateControlHost:
						selection.Clear();
						currentBehavior = new CreateControlHostBehavior(this);
						break;
					case BehaviorType.LinkedControls:
						selection.Clear();
						currentBehavior = new ControlHostBehavior(this);
						break;
					case BehaviorType.Custom:
						selection.Clear();
						currentBehavior = customBehavior == null ?
							new FlowChartBehavior(this) : customBehavior;
						break;
				}
			}
		}

		internal Behavior getCurrBehavior()
		{
			return currentBehavior;
		}

		[Browsable(false)]
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public Behavior CustomBehavior
		{
			get { return customBehavior; }

			set
			{
				customBehavior = value;
				if (customBehavior != null)
				{
					currentBehavior = customBehavior;
					behavior = BehaviorType.Custom;
				}
				else
				{
					Behavior = BehaviorType.FlowChart;
				}
			}
		}

		private Behavior currentBehavior;
		private Behavior customBehavior;
		private BehaviorType behavior;

		/// <summary>
		/// Enable/disable reflexive links creation
		/// </summary>
		[Category("Behavior")]
		[DefaultValue(true)]
		[Description("Enables or disables creation of reflexive links.")]
		public bool AllowRefLinks
		{
			get
			{
				return allowRefLinks;
			}
			set
			{
				if (allowRefLinks != value)
				{
					allowRefLinks = value;
					setDirty();
				}
			}
		}

		private bool allowRefLinks;

		/// <summary>
		/// Default for the Route property of new arrows
		/// </summary>
		[Category("Routing")]
		[DefaultValue(false)]
		[Description("Initial value of the AutoRoute property of arrows.")]
		public bool RouteArrows
		{
			get
			{
				return routeArrows;
			}
			set
			{
				if (routeArrows != value)
				{
					routeArrows = value;
					setDirty();
				}
			}
		}

		private bool routeArrows;

		/// <summary>
		/// Settings for the link routing algorithm.
		/// </summary>
		[Category("Routing")]
		[Description("Settings for the link routing algorithm.")]
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
		[TypeConverter(typeof(ExpandableObjectConverter))]
		public RoutingOptions RoutingOptions
		{
			get
			{
				return routingOptions;
			}
		}

		private RoutingOptions routingOptions;

		/// <summary>
		/// A RoutingGrid instance used to allocate grid arrays
		/// </summary>
		internal RoutingGrid RoutingGrid
		{
			get
			{
				return routingGrid;
			}
		}

		private RoutingGrid routingGrid;

		/// <summary>
		/// Determines whether a link must be rerouted.
		/// </summary>
		internal bool rerouteArrow(Arrow arrow)
		{
			if ((routingOptions.TriggerRerouting & RerouteArrows.WhenModified) != 0)
				return true;

			if ((routingOptions.TriggerRerouting & RerouteArrows.WhenIntersectNode) != 0)
			{
				RectangleF arrowRect = arrow.BoundingRect;
				for (int j = 0; j < zOrder.Count; ++j)
				{
					if (zOrder[j] is Node)
					{
						Node node = zOrder[j] as Node;
						if (node == arrow.Origin || node == arrow.Destination)
							continue;
						RectangleF rect = node.getRotatedBounds();
						if (arrowRect.IntersectsWith(rect))
						{
							if (arrow.Intersects(node))
								return true;

						}	// if (arrowRect.IntersectsWith(rect))

					}	// if (zOrder[j] is Node)

				}	// for (int j = 0; j < zOrder.Count; ++j)
			}

			return false;
		}

		/// <summary>
		/// Reroutes all links.
		/// </summary>
		internal void routeAllArrows(ChartObject modified)
		{
			Node node = modified as Node;
			if (node != null && !node.Obstacle)
				return;

			undoManager.onStartRoute();

			RectangleF objRect = modified.getRotatedBounds();
			RectangleF invalid = objRect;
			if (node == null ||
				(routingOptions.TriggerRerouting & RerouteArrows.WhenModified) != 0)
			{
				foreach (Arrow arrow in arrows)
				{
					if (!arrow.AutoRoute) continue;

					RectangleF arrowRect = arrow.getBoundingRect();
					if (arrowRect.IntersectsWith(objRect))
					{
						invalid = Utilities.unionRects(invalid, arrowRect);
						undoManager.onRouteArrow(arrow);
						arrow.doRoute();
						invalid = Utilities.unionRects(invalid, arrow.getRepaintRect(false));
					}
				}
			}
			else
			{
				foreach (Arrow arrow in arrows)
				{
					if (rerouteArrow(arrow))
					{
						invalid = Utilities.unionRects(invalid, arrow.getRepaintRect(false));
						undoManager.onRouteArrow(arrow);
						arrow.doRoute();
						invalid = Utilities.unionRects(invalid, arrow.getBoundingRect());
					}
				}
			}

			undoManager.onEndRoute();
			invalidate(invalid);
		}

		/// <summary>
		/// Performs routing on all arrows ignoring their AutoRoute flag.
		/// </summary>
		public void RouteAllArrows()
		{
			foreach (Arrow a in arrows)
			{
				bool wasAutoRoute = a.AutoRoute;

				a.SilentAutoRoute = true;
				a.doRoute();
				a.SilentAutoRoute = wasAutoRoute;
			}

			Invalidate();
		}

		/// <summary>
		/// Used to disable routing during load time and when automatic
		/// layout routines run.
		/// </summary>
		internal bool DontRouteForAwhile
		{
			get { return dontRouteForAwhile; }
			set { dontRouteForAwhile = value; }
		}

		private bool dontRouteForAwhile;

		[Category("Behavior")]
		[DefaultValue(typeof(ExpandButtonAction), "ExpandTreeBranch")]
		[Description("Specifies the behavior of the +/- buttons displayed beside expandable nodes.")]
		public ExpandButtonAction ExpandButtonAction
		{
			get { return expandButtonAction; }
			set { expandButtonAction = value; }
		}

		private ExpandButtonAction expandButtonAction;

		#endregion

		#region scrolling and zooming

		[Category("Appearance")]
		[DefaultValue(true)]
		[Description("Specifies whether scrollbars are visible.")]
		public bool ShowScrollbars
		{
			get { return hScrollBar != null; }
			set
			{
				if (value)
				{
					if (hScrollBar != null)
						Controls.Remove(hScrollBar);
					if (vScrollBar != null)
						Controls.Remove(vScrollBar);

					vScrollBar = new VScrollBar();
					vScrollBar.Dock = DockStyle.Right;
					vScrollBar.Scroll += new ScrollEventHandler(ScrollEvent);
					Controls.Add(vScrollBar);

					hScrollBar = new HScrollBar();
					hScrollBar.Dock = DockStyle.Bottom;
					hScrollBar.Scroll += new ScrollEventHandler(ScrollEvent);
					Controls.Add(hScrollBar);
				}
				else
				{
					if (hScrollBar != null) Controls.Remove(hScrollBar);
					if (vScrollBar != null) Controls.Remove(vScrollBar);
					hScrollBar = null;
					vScrollBar = null;
				}
			}
		}

		[Category("Layout")]
		public RectangleF DocExtents
		{
			get { return docExtents; }
			set
			{
				if (!docExtents.Equals(value))
				{
					docExtents = value;
					docExtentsMin = value;

					resetDocSize();
					updateHostedControlsPos();
				}
			}
		}

		private void resetDocSize()
		{
			if (ScrollX < docExtents.Left || ScrollX > docExtents.Right)
				ScrollX = docExtents.Left;
			if (ScrollY < docExtents.Top || ScrollY > docExtents.Bottom)
				ScrollY = docExtents.Top;
			if (ShowScrollbars)
				resetScrollbars();

			if (DocExtentsChanged != null)
				DocExtentsChanged(this, new EventArgs());
		}

		private void resetScrollbars()
		{
			Graphics g = CreateGraphics();
			setTransforms(g);
			RectangleF vsblDoc = Utilities.deviceToDoc(g, ClientRectangle);
			g.Dispose();

			hScrollBar.Minimum = (int)docExtents.Left;
			hScrollBar.Maximum = (int)docExtents.Right;
			hScrollBar.Value = (int)valueInScrollRange(scrollX, hScrollBar);
			hScrollBar.LargeChange = (int)vsblDoc.Width;
			hScrollBar.Visible = hScrollBar.LargeChange <
				hScrollBar.Maximum - hScrollBar.Minimum;
			if (!hScrollBar.Visible)
				setScrollX(docExtents.Left);

			vScrollBar.Minimum = (int)docExtents.Top;
			vScrollBar.Maximum = (int)docExtents.Bottom;
			vScrollBar.Value = (int)valueInScrollRange(scrollY, vScrollBar);
			vScrollBar.LargeChange = (int)vsblDoc.Height;
			vScrollBar.Visible = vScrollBar.LargeChange <
				vScrollBar.Maximum - vScrollBar.Minimum;
			if (!vScrollBar.Visible)
				setScrollY(docExtents.Top);
		}

		private float valueInScrollRange(float value, ScrollBar scrollBar)
		{
			return Math.Min(Math.Max(value, scrollBar.Minimum), scrollBar.Maximum);
		}

		private RectangleF docExtents;
		private RectangleF docExtentsMin;

		public VScrollBar VScrollBar
		{
			get { return vScrollBar; }
		}

		public HScrollBar HScrollBar
		{
			get { return hScrollBar; }
		}

		VScrollBar vScrollBar = null;
		HScrollBar hScrollBar = null;
		
		private void ScrollEvent(object sender, ScrollEventArgs e)
		{
			endInplaceEdit(true);

			if (sender == hScrollBar)
				setScrollX((float)e.NewValue);
			if (sender == vScrollBar)
				setScrollY((float)e.NewValue);
		}

		/// <summary>
		/// Automatically scroll when dragging the mouse outside the visible area
		/// </summary>
		[Category("Behavior")]
		[DefaultValue(true)]
		[Description("Enables automatic scrolling when the mouse is dragged outside the document boundaries.")]
		public bool AutoScroll
		{
			get
			{
				return autoScroll;
			}
			set
			{
				if (autoScroll != value)
				{
					autoScroll = value;
					if (scrollTimer != null)
					{
						scrollTimer.Enabled = false;
						scrollTimer = null;
					}
					setDirty();
				}
			}
		}

		private bool autoScroll;
		private float autoScrDX;
		private float autoScrDY;

		private void checkAutoScroll(MouseEventArgs args)
		{
			Point ptDev = new Point(args.X, args.Y);

			if (!Utilities.pointInRect(ptDev, ClientRectangle))
			{
				autoScrDX = autoScrDY = 0;
				if (ptDev.X < ClientRectangle.Left) autoScrDX = -Constants.getAutoScroll(measureUnit);
				if (ptDev.Y < ClientRectangle.Top) autoScrDY = -Constants.getAutoScroll(measureUnit);
				if (ptDev.X > ClientRectangle.Right) autoScrDX = Constants.getAutoScroll(measureUnit);
				if (ptDev.Y > ClientRectangle.Bottom) autoScrDY = Constants.getAutoScroll(measureUnit);

				onAutoScrollTimer(this, null);
				if (scrollTimer == null)
				{
					scrollTimer = new Timer();
					scrollTimer.Interval = 100;
					scrollTimer.Tick += new EventHandler(onAutoScrollTimer);
					scrollTimer.Enabled = true;
				}
			}
			else
			{
				if (scrollTimer != null)
				{
					scrollTimer.Enabled = false;
					Invalidate();
				}
				scrollTimer = null;
			}
		}

		private Timer scrollTimer = null;

		private void scrollStayInDoc(float scrX, float scrY, RectangleF rcPage)
		{
			// ensure that the scroll position is inside the document
			if (scrX < docExtents.Left) scrX = docExtents.Left;
			if (scrX > docExtents.Right - rcPage.Width)
				scrX = Math.Max(docExtents.Right - rcPage.Width, docExtents.Left);

			if (scrY < docExtents.Top) scrY = docExtents.Top;
			if (scrY > docExtents.Bottom - rcPage.Height)
				scrY = Math.Max(docExtents.Bottom - rcPage.Height, docExtents.Top);

			// scroll to the new position
			ScrollTo(scrX, scrY);
		}

		private void onAutoScrollTimer(Object obj, EventArgs args)
		{
			Graphics g = CreateGraphics();
			setTransforms(g);

			RectangleF rcPage = Utilities.deviceToDoc(g, ClientRectangle);

			float scrX=ScrollX, scrY=ScrollY;
			if (rcPage.Width < docExtents.Width)
				scrX = ScrollX + autoScrDX;
			if (rcPage.Height < docExtents.Height)
				scrY = ScrollY + autoScrDY;

			scrollStayInDoc(scrX, scrY, rcPage);

			// redraw the control
			drawFlowChart(docBuffer, ClientRectangle, true);
			Invalidate();

			// update the modified object position
			if (interaction != null)
			{
				// get the current mouse position in document coordinates
				Point ptDev = PointToClient(Control.MousePosition);
				PointF ptDoc = Utilities.deviceToDoc(g, ptDev.X, ptDev.Y);

				redrawNonModified = false;
				mouseMoved = true;
				interaction.update(ptDoc, this);

				// update document size if needed
				if (autoSizeDoc != MindFusion.FlowChartX.AutoSize.None)
					sizeDocForItem(interaction.CurrentObject);

				if (redrawNonModified)
				{
					drawFlowChart(docBuffer, ClientRectangle, true);
					Invalidate();
					redrawNonModified = false;
				}
			}

			g.Dispose();
		}

		/// <summary>
		/// Gets the client coordinates of a document point
		/// </summary>
		public Point DocToClient(PointF docPt)
		{
			Graphics g = CreateGraphics();
			setTransforms(g);
			Point clientPt = Utilities.docToDevice(g, docPt);
			g.Dispose();
			return clientPt;
		}

		/// <summary>
		/// Gets the client coordinates of a document rectangle
		/// </summary>
		public Rectangle DocToClient(RectangleF docRect)
		{
			Graphics g = CreateGraphics();
			setTransforms(g);
			Rectangle clientRect = Utilities.docToDevice(g, docRect);
			g.Dispose();
			return clientRect;
		}

		/// <summary>
		/// Gets the document coordinates of a client point
		/// </summary>
		public PointF ClientToDoc(Point clientPt)
		{
			Graphics g = CreateGraphics();
			setTransforms(g);
			PointF docPt = Utilities.deviceToDoc(g, clientPt.X, clientPt.Y);
			g.Dispose();
			return docPt;
		}

		/// <summary>
		/// Gets the document coordinates of a client rectangle
		/// </summary>
		public RectangleF ClientToDoc(Rectangle clientRect)
		{
			Graphics g = CreateGraphics();
			setTransforms(g);
			RectangleF docRect = Utilities.deviceToDoc(g, clientRect);
			g.Dispose();
			return docRect;
		}

		public void BringIntoView(ChartObject obj)
		{
			if (obj != null)
			{
				// get the currently visible document area
				Graphics g = CreateGraphics();
				setTransforms(g);
				RectangleF rcClient = Utilities.deviceToDoc(g, this.ClientRectangle);

				// check if the objects is entirely in the visible area
				RectangleF rcObj = obj.getRepaintRect(false);
				if (rcObj.Left < rcClient.Left || rcObj.Right > rcClient.Right ||
					rcObj.Top < rcClient.Top || rcObj.Bottom > rcClient.Bottom)
				{
					// scroll to center the object in the visible area
					float cx = (rcObj.Left + rcObj.Right - rcClient.Width) / 2;
					float cy = (rcObj.Top + rcObj.Bottom - rcClient.Height) / 2;
					if (cx < docExtents.Left)
						cx = docExtents.Left; else
						if (cx > docExtents.Right - rcClient.Width)
						cx = docExtents.Right - rcClient.Width;
					if (cy < docExtents.Top)
						cy = docExtents.Top; else
						if (cy > docExtents.Bottom - rcClient.Height)
						cy = docExtents.Bottom - rcClient.Height;
					ScrollTo(cx, cy);
				}
			}
		}

		protected override void OnLayout(LayoutEventArgs e)
		{
			if (hScrollBar != null && vScrollBar != null)
			{
				resetScrollbars();
				base.OnLayout(e);
				hScrollBar.Width -= vScrollBar.Width;
			}
			else
			{
				base.OnLayout(e);
			}
		}

		internal RectangleF getDocRect(Rectangle rcDev)
		{
			Graphics g = CreateGraphics();
			setTransforms(g);

			RectangleF result = Utilities.deviceToDoc(g, rcDev);
			g.Dispose();
			return result;
		}

		private void updateHostedControlsPos()
		{
			if (!loading)
			{
				foreach (ChartObject obj in zOrder)
				{
					if (obj.getType() == ItemType.ControlHost)
						((ControlHost)obj).updateControlPosition();
				}
			}
		}

		/// <summary>
		/// The document horizontal scroll position
		/// </summary>
		[Category("Layout")]
		[DefaultValue(0.0f)]
		public float ScrollX
		{
			get
			{
				return scrollX;
			}
			set
			{
				if (scrollX != value)
				{
					setScrollX(value);
					if (hScrollBar != null)
						hScrollBar.Value = (int)valueInScrollRange(scrollX, hScrollBar);
				}
			}
		}

		void setScrollX(float scr)
		{
			scrollX = scr;
			Invalidate();
			updateHostedControlsPos();

			if (ScrollChanged != null)
				ScrollChanged(this, new EventArgs());
		}

		void setScrollY(float scr)
		{
			scrollY = scr;
			Invalidate();
			updateHostedControlsPos();

			if (ScrollChanged != null)
				ScrollChanged(this, new EventArgs());
		}

		private float scrollX;

		/// <summary>
		/// The document vertical scroll position
		/// </summary>
		[Category("Layout")]
		[DefaultValue(0.0f)]
		public float ScrollY
		{
			get
			{
				return scrollY;
			}
			set
			{
				if (scrollY != value)
				{
					setScrollY(value);
					if (vScrollBar != null)
						vScrollBar.Value = (int)valueInScrollRange(scrollY, vScrollBar);
				}
			}
		}

		private float scrollY;

		/// <summary>
		/// Scrolls the document
		/// </summary>
		public void ScrollTo(float x, float y)
		{
			ScrollX = x;
			ScrollY = y;
		}

		/// <summary>
		/// The zoom factor
		/// </summary>
		[Category("Layout")]
		[DefaultValue(100.0f)]
		public float ZoomFactor
		{
			get
			{
				return zoomFactor;
			}
			set
			{
				if (zoomFactor == value)
					return;

				zoomFactor = value;
				if (zoomFactor < 1)
					zoomFactor = 1;
				if (zoomFactor > 1500)
					zoomFactor = 1500;

				Invalidate();
				setDirty();
				updateHostedControlsPos();
				if (ShowScrollbars)
					resetScrollbars();
			
				if (ZoomFactorChanged != null)
					ZoomFactorChanged(this, new EventArgs());
			}
		}

		private float zoomFactor;

		/// <summary>
		/// Increases the zoom factor
		/// </summary>
		public void ZoomIn()
		{
			if (ZoomFactor < 10)
				ZoomFactor += 1;
			else if (ZoomFactor < 20)
				ZoomFactor += 5;
			else
				ZoomFactor += 10;
		}

		/// <summary>
		/// Decreases the zoom factor
		/// </summary>
		public void ZoomOut()
		{
			if (ZoomFactor > 20)
				ZoomFactor -= 10;
			else if (ZoomFactor > 10)
				ZoomFactor -= 5;
			else
				ZoomFactor -= 1;
		}

		public void ZoomToFit()
		{
			RectangleF rcCnt = getContentRect(false, true);
			zoomToRect(rcCnt, 2);
		}

		void zoomToRect(RectangleF rc, int zoomDecrease)
		{
			Graphics g = CreateGraphics();
			g.PageUnit = measureUnit;
			RectangleF ctrlRect = Utilities.deviceToDoc(g, ClientRectangle);
			g.Dispose();

			float ctrlWidth = ctrlRect.Width;
			float ctrlHeight = ctrlRect.Height;
			float cntWidth = rc.Width;
			float cntHeight = rc.Height;

			if (ctrlWidth > 0 && ctrlHeight > 0 && cntWidth > 0 && cntHeight > 0)
			{
				float rx = cntWidth / ctrlWidth;
				float ry = cntHeight / ctrlHeight;
				float ratio = Math.Max(rx, ry);

				ScrollTo(rc.X, rc.Y);
				float newZoom = 100.0f / ratio;
				if (newZoom > 10)
					newZoom -= zoomDecrease;
				ZoomFactor = newZoom;
			}
		}

		public void ZoomToRect(RectangleF rect)
		{
			zoomToRect(rect, 0);
		}

		internal RectangleF getContentRect(bool forPrint, bool onlyVisible)
		{
			RectangleF rcUnion = new RectangleF(0, 0, 0, 0);

			foreach (ChartObject item in zOrder)
			{
				if (onlyVisible && !item.Visible) continue;
				if (forPrint && !item.Printable) continue;

				RectangleF itemRect = item.getRepaintRect(false);
				rcUnion = Utilities.unionNonEmptyRects(rcUnion, itemRect);
			}

			if (forPrint)
			{
				rcUnion = RectangleF.FromLTRB(DocExtents.Left, DocExtents.Top,
					Math.Max(rcUnion.Right, DocExtents.Left),
					Math.Max(rcUnion.Bottom, DocExtents.Top));
			}

			return rcUnion;
		}

		public void FitDocToObjects(float borderGap)
		{
			FitDocToObjects(borderGap, false);
		}

		public void FitDocToObjects(float borderGap, bool onlyVisible)
		{
			if (zOrder.Count > 0)
			{
				RectangleF rcDoc = getContentRect(false, onlyVisible);
				rcDoc.Inflate(borderGap, borderGap);
				DocExtents = rcDoc;
			}
		}

		#endregion

		#region serialization

		[Serializable]
		private class DocInfoVer1 : IPersists
		{
			internal DocInfoVer1()
			{
			}

			internal DocInfoVer1(FlowChart fc)
			{
				backColor = fc.BackColor;
				behavior = fc.Behavior;
				arrowHead = fc.arrowHead;
				arrowBase = fc.arrowBase;
				arrowInterm = fc.arrowInterm;
				arrowHeadSize = fc.arrowHeadSize;
				arrowBaseSize = fc.arrowBaseSize;
				arrowIntermSize = fc.arrowIntermSize;
				shadowsStyle = fc.shadowsStyle;
				boxFillColor = fc.boxFillColor;
				arrowFillColor = fc.arrowFillColor;
				boxFrameColor = fc.boxFrameColor;
				arrowColor = fc.arrowColor;
				alignToGrid = fc.alignToGrid;
				showGrid = fc.showGrid;
				gridColor = fc.gridColor;
				gridSize = fc.gridSizeX;
				boxStyle = fc.boxStyle;
				shadowColor = fc.shadowColor;

				//textFormat = fc.textFormat;
				al = fc.textFormat.Alignment;
				dsl = fc.textFormat.DigitSubstitutionLanguage;
				dsm = fc.textFormat.DigitSubstitutionMethod;
				sff = fc.textFormat.FormatFlags;
				hp = fc.textFormat.HotkeyPrefix;
				lal = fc.textFormat.LineAlignment;
				tr = fc.textFormat.Trimming;
				ts = fc.textFormat.GetTabStops(out fto);

				imagePos = fc.imagePos;
				textColor = fc.textColor;
				activeMnpColor = fc.activeMnpColor;
				selMnpColor = fc.selMnpColor;
				disabledMnpColor = fc.disabledMnpColor;
				arrowStyle = fc.arrowStyle;
				arrowSegments = fc.arrowSegments;
				scrollX = fc.scrollX;
				scrollY = fc.scrollY;
				zoomFactor = fc.zoomFactor;
				penDashStyle = fc.penDashStyle;
				penWidth = fc.penWidth;
				docExtents = fc.docExtents;
				shadowOffsetX = fc.shadowOffsetX;
				shadowOffsetY = fc.shadowOffsetY;
				tableFillColor = fc.tableFillColor;
				tableFrameColor = fc.tableFrameColor;
				tableRowsCount = fc.tableRowsCount;
				tableColumnsCount = fc.tableColumnsCount;
				tableColWidth = fc.tableColWidth;
				tableRowHeight = fc.tableRowHeight;
				tableCaptionHeight = fc.tableCaptionHeight;
				tableCaption = fc.tableCaption;
				arrowCascadeOrientation = fc.arrowCascadeOrientation;
				tableCellBorders = fc.tableCellBorders;
				boxesExpandable = fc.boxesExpandable;
				tablesScrollable = fc.tablesScrollable;

				defPolyShape = new byte[] { 0, 0 };
			}

			internal void apply(FlowChart fc)
			{
				fc.BackColor = backColor;

				fc.Behavior = behavior;
				fc.mouseMoved = false;

				fc.arrowHead = arrowHead;
				fc.arrowBase = arrowBase;
				fc.arrowInterm = arrowInterm;
				fc.arrowHeadSize = arrowHeadSize;
				fc.arrowBaseSize = arrowBaseSize;
				fc.arrowIntermSize = arrowIntermSize;

				fc.shadowsStyle = shadowsStyle;
				fc.boxFillColor = boxFillColor;
				fc.arrowFillColor = arrowFillColor;
				fc.boxFrameColor = boxFrameColor;
				fc.arrowColor = arrowColor;
				fc.alignToGrid = alignToGrid;
				fc.showGrid = showGrid;
				fc.gridColor = gridColor;
				fc.boxStyle = boxStyle;
				fc.shadowColor = shadowColor;

				//fc.textFormat = textFormat;
				fc.textFormat = new StringFormat();
				fc.textFormat.Alignment = al;
				fc.textFormat.SetDigitSubstitution(dsl, dsm);
				fc.textFormat.FormatFlags = sff;
				fc.textFormat.HotkeyPrefix = hp;
				fc.textFormat.LineAlignment = lal;
				fc.textFormat.Trimming = tr;
				if (ts != null)
					fc.textFormat.SetTabStops(fto, ts);

				fc.imagePos = imagePos;
				fc.textColor = textColor;
				fc.activeMnpColor = activeMnpColor;
				fc.selMnpColor = selMnpColor;
				fc.disabledMnpColor = disabledMnpColor;
				fc.arrowStyle = arrowStyle;
				fc.arrowSegments = arrowSegments;
				fc.ScrollX = scrollX;
				fc.ScrollY = scrollY;
				fc.zoomFactor = zoomFactor;
				fc.penDashStyle = penDashStyle;
				fc.penWidth = penWidth;
				fc.DocExtents = docExtents;
				fc.shadowOffsetX = shadowOffsetX;
				fc.shadowOffsetY = shadowOffsetY;
				fc.tableFillColor = tableFillColor;
				fc.tableFrameColor = tableFrameColor;
				fc.tableRowsCount = tableRowsCount;
				fc.tableColumnsCount = tableColumnsCount;
				fc.tableColWidth = tableColWidth;
				fc.tableRowHeight = tableRowHeight;
				fc.tableCaptionHeight = tableCaptionHeight;
				fc.tableCaption = tableCaption;
				fc.arrowCascadeOrientation = arrowCascadeOrientation;
				fc.tableCellBorders = tableCellBorders;
				fc.boxesExpandable = boxesExpandable;
				fc.tablesScrollable = tablesScrollable;

				fc.boxPen.Color = boxFrameColor;
				fc.boxPen.Width = penWidth;
				//fc.boxPen.DashStyle = penDashStyle;
				fc.arrowPen.Color = arrowColor;
				fc.arrowPen.Width = penWidth;
				//fc.arrowPen.DashStyle = penDashStyle;
				fc.tablePen.Color = tableFrameColor;
				fc.tablePen.Width = penWidth;
				//fc.tablePen.DashStyle = penDashStyle;
				fc.BoxBrush = new SolidBrush(boxFillColor);
				fc.ArrowBrush = new SolidBrush(arrowFillColor);
				fc.TableBrush = new SolidBrush(tableFillColor);
				fc.BackBrush = new SolidBrush(backColor);
			}

			private Color backColor;
			private BehaviorType behavior;
			private ArrowHead arrowHead;
			private ArrowHead arrowBase;
			private ArrowHead arrowInterm;
			private float arrowHeadSize;
			private float arrowBaseSize;
			private float arrowIntermSize;
			private ShadowsStyle shadowsStyle;
			private Color boxFillColor;
			private Color arrowFillColor;
			private Color boxFrameColor;
			private Color arrowColor;
			private bool alignToGrid;
			private bool showGrid;
			private Color gridColor;
			private float gridSize;
			private BoxStyle boxStyle;
			private Color shadowColor;

			//private StringFormat textFormat;
			private StringAlignment al;
			private int dsl;
			private StringDigitSubstitute dsm;
			private StringFormatFlags sff;
			private HotkeyPrefix hp;
			private StringAlignment lal;
			private StringTrimming tr;
			float fto;
			float[] ts;

			private ImageAlign imagePos;
			private Color textColor;
			private Color activeMnpColor;
			private Color selMnpColor;
			private Color disabledMnpColor;
			private ArrowStyle arrowStyle;
			private short arrowSegments;
			private float scrollX;
			private float scrollY;
			private float zoomFactor;
			private DashStyle penDashStyle;
			float penWidth;
			byte[] defPolyShape;
			private RectangleF docExtents;
			private float shadowOffsetX;
			private float shadowOffsetY;
			private Color tableFillColor;
			private Color tableFrameColor;
			private int tableRowsCount;
			private int tableColumnsCount;
			private float tableColWidth;
			private float tableRowHeight;
			private float tableCaptionHeight;
			private string tableCaption;
			private Orientation arrowCascadeOrientation;
			private CellFrameStyle tableCellBorders;
			ArrowAnchor boxIncmAnchor;
			ArrowAnchor boxOutgAnchor;
			private bool boxesExpandable;
			private bool tablesScrollable;

			public virtual int getClassId()
			{
				return 17;
			}

			public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
			{
				ctx.saveColor(backColor);
				writer.Write((int)behavior);
				writer.Write((int)arrowHead);
				writer.Write((int)arrowBase);
				writer.Write((int)arrowInterm);
				writer.Write((double)arrowHeadSize);
				writer.Write((double)arrowBaseSize);
				writer.Write((double)arrowIntermSize);
				writer.Write((int)shadowsStyle);
				ctx.saveColor(boxFillColor);
				ctx.saveColor(arrowFillColor);
				ctx.saveColor(boxFrameColor);
				ctx.saveColor(arrowColor);
				writer.Write(alignToGrid);
				writer.Write(showGrid);
				ctx.saveColor(gridColor);
				writer.Write((double)gridSize);
				writer.Write((int)boxStyle);
				ctx.saveColor(shadowColor);

				writer.Write((int)imagePos);
				ctx.saveColor(textColor);
				ctx.saveColor(activeMnpColor);
				ctx.saveColor(selMnpColor);
				ctx.saveColor(disabledMnpColor);
				writer.Write((int)arrowStyle);
				writer.Write(arrowSegments);
				writer.Write((double)scrollX);
				writer.Write((double)scrollY);
				writer.Write(zoomFactor);
				writer.Write((int)penDashStyle);
				writer.Write((double)penWidth);
				writer.Write(defPolyShape.GetLength(0));
				writer.Write(defPolyShape);
				ctx.saveRect(docExtents);
				writer.Write((double)shadowOffsetX);
				writer.Write((double)shadowOffsetY);
				ctx.saveColor(tableFillColor);
				ctx.saveColor(tableFrameColor);
				writer.Write(tableRowsCount);
				writer.Write(tableColumnsCount);
				writer.Write((double)tableColWidth);
				writer.Write((double)tableRowHeight);
				writer.Write((double)tableCaptionHeight);
				writer.Write(tableCaption);
				writer.Write((int)arrowCascadeOrientation);
				writer.Write((int)tableCellBorders);
				writer.Write((int)boxIncmAnchor);
				writer.Write((int)boxOutgAnchor);
				writer.Write(boxesExpandable);
				writer.Write(tablesScrollable);
			}

			public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
			{
				backColor = ctx.loadColor();
				behavior = (BehaviorType)reader.ReadInt32();
				arrowHead = (ArrowHead)reader.ReadInt32();
				arrowBase = (ArrowHead)reader.ReadInt32();
				arrowInterm = (ArrowHead)reader.ReadInt32();
				arrowHeadSize = (float)reader.ReadDouble();
				arrowBaseSize = (float)reader.ReadDouble();
				arrowIntermSize = (float)reader.ReadDouble();
				shadowsStyle = (ShadowsStyle)reader.ReadInt32();
				boxFillColor = ctx.loadColor();
				arrowFillColor = ctx.loadColor();
				boxFrameColor = ctx.loadColor();
				arrowColor = ctx.loadColor();
				alignToGrid = reader.ReadBoolean();
				showGrid = reader.ReadBoolean();
				gridColor = ctx.loadColor();
				gridSize = (float)reader.ReadDouble();
				boxStyle = (BoxStyle)reader.ReadInt32();
				shadowColor = ctx.loadColor();
				imagePos = (ImageAlign)reader.ReadInt32();
				textColor = ctx.loadColor();
				activeMnpColor = ctx.loadColor();
				selMnpColor = ctx.loadColor();
				disabledMnpColor = ctx.loadColor();
				arrowStyle = (ArrowStyle)reader.ReadInt32();
				arrowSegments = reader.ReadInt16();
				scrollX = (float)reader.ReadDouble();
				scrollY = (float)reader.ReadDouble();

				// zoomFactor was a short, now it is a float
				if (ctx.FileVersion < 19)
					zoomFactor = reader.ReadInt16();
				else
					zoomFactor = reader.ReadSingle();

				penDashStyle = (DashStyle)reader.ReadInt32();
				penWidth = (float)reader.ReadDouble();
				int c = reader.ReadInt32();
				defPolyShape = reader.ReadBytes(c);
				docExtents = ctx.loadRectF();
				shadowOffsetX = (float)reader.ReadDouble();
				shadowOffsetY = (float)reader.ReadDouble();
				tableFillColor = ctx.loadColor();
				tableFrameColor = ctx.loadColor();
				tableRowsCount = reader.ReadInt32();
				tableColumnsCount = reader.ReadInt32();
				tableColWidth = (float)reader.ReadDouble();
				tableRowHeight = (float)reader.ReadDouble();
				tableCaptionHeight = (float)reader.ReadDouble();
				tableCaption = reader.ReadString();
				arrowCascadeOrientation = (Orientation)reader.ReadInt32();
				tableCellBorders = (CellFrameStyle)reader.ReadInt32();
				boxIncmAnchor = (ArrowAnchor)reader.ReadInt32();
				boxOutgAnchor = (ArrowAnchor)reader.ReadInt32();
				boxesExpandable = reader.ReadBoolean();
				tablesScrollable = reader.ReadBoolean();
			}

			public virtual void setReference(int refId, IPersists obj)
			{
			}
		}

		[Serializable]
		private class DocInfoVer2 : IPersists
		{
			internal DocInfoVer2()
			{
			}

			internal DocInfoVer2(FlowChart fc)
			{
				toolTip = fc.toolTip;
			}

			internal void apply(FlowChart fc)
			{
				fc.toolTip = toolTip;
			}

			private string toolTip;

			public virtual int getClassId()
			{
				return 18;
			}

			public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
			{
				writer.Write(toolTip);
//				writer.Write(arrowEndsMovable);
			}

			public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
			{
				toolTip = reader.ReadString();
//				arrowEndsMovable = reader.ReadBoolean();
			}

			public virtual void setReference(int refId, IPersists obj)
			{
			}
		}

		public virtual int getClassId()
		{
			return 19;
		}

		public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
		{
		}

		public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
		{
		}

		public virtual void setReference(int refId, IPersists obj)
		{
			switch(refId)
			{
			case 1:
				activeObject = (ChartObject)obj;
				break;
			case 2:
				boxBrush.Release();
				boxBrush = (Brush)obj;
				boxBrush.AddRef();
				break;
			case 3:
				arrowBrush.Release();
				arrowBrush = (Brush)obj;
				arrowBrush.AddRef();
				break;
			case 4:
				tableBrush.Release();
				tableBrush = (Brush)obj;
				tableBrush.AddRef();
				break;
			case 5:
				brush.Release();
				brush = (Brush)obj;
				brush.AddRef();
				break;
			case 6:
				if (exteriorBrush != null)
					exteriorBrush.Release();
				exteriorBrush = (Brush)obj;
				if (exteriorBrush != null)
					exteriorBrush.AddRef();
				break;
			default:
				throw new Exception("Unrecognized reference in FlowChart.setReference(...)");
			}
		}

		/// <summary>
		/// Saves the flowchart document into a file
		/// </summary>
		public void SaveToFile(string fileName)
		{
			SaveToFile(fileName, true);
		}

		/// <summary>
		/// Saves the flowchart document into a file
		/// </summary>
		public void SaveToFile(string fileName, bool clearDirty)
		{
			Stream stream = new FileStream(fileName,
				FileMode.Create, FileAccess.Write, FileShare.None);

			SaveToStream(stream, clearDirty);

			stream.Close();
		}

		/// <summary>
		/// Loads the flowchart document from a file
		/// </summary>
		public void LoadFromFile(string fileName)
		{
			// open the file for reading
			Stream stream = new FileStream(fileName,
				FileMode.Open, FileAccess.Read, FileShare.Read);

			LoadFromStream(stream);

			stream.Close();
		}

		/// <summary>
		/// Saves the flowchart document as an ASCII string
		/// </summary>
		public string SaveToString()
		{
			return SaveToString(true);
		}

		/// <summary>
		/// Saves the flowchart document as an ASCII string
		/// </summary>
		public string SaveToString(bool clearDirty)
		{
			int len = 0;

			// save to memory buffer
			MemoryStream stream = new MemoryStream();
			SaveToStream(stream, clearDirty);
			len = (int)stream.Position;
			stream.Close();
			byte[] buf = stream.GetBuffer();

			// encode as ASCII string
			System.Text.StringBuilder builder = new
				System.Text.StringBuilder(2*len + 1);
			for (uint cc = 0; cc < len; ++cc)
				builder.AppendFormat("{0:X2}", buf[cc]);

			return builder.ToString();
		}

		/// <summary>
		/// Loads the flowchart document from a string
		/// </summary>
		public void LoadFromString(string str)
		{
			int len = 0;

			// get the bytes
			len = str.Length / 2;
			byte[] buf = new byte[len];
			for (int cc = 0; cc < len; ++cc)
			{
				string hex = str.Substring(cc * 2, 2);
				buf[cc] = Byte.Parse(hex, System.Globalization.NumberStyles.HexNumber);
			}

			// create memory stream and load
			MemoryStream stream = new MemoryStream(buf);
			LoadFromStream(stream);
			stream.Close();
		}

		/// <summary>
		/// Saves the flowchart document into a stream
		/// </summary>
		public void SaveToStream(Stream stream)
		{
			SaveToStream(stream, true);
		}

		/// <summary>
		/// Saves the flowchart document into a stream
		/// </summary>
		public void SaveToStream(Stream stream, bool clearDirty)
		{
			PersistContext ctx = new PersistContext(stream, this);
			ctx.writeHeader();

			// write properties
			DocInfoVer1 docInfo1 = new DocInfoVer1(this);
			DocInfoVer2 docInfo2 = new DocInfoVer2(this);
			ctx.saveObject(docInfo1);
			ctx.saveObject(docInfo2);

			ctx.saveStringFormat(textFormat);
			ctx.saveFont(Font);
			ctx.saveImage(BackgroundImage);

			// write collections
			ctx.saveObject(zOrder);
			ctx.saveObject(groups);
			ctx.saveObject(selection);

			ctx.saveReference(this, activeObject, 1);

			ctx.saveObject(shapeTemplate);

			Brush.StoreBrushes(new BinaryWriter(stream), ctx);
			ctx.saveObject(boxPen);
			ctx.saveObject(arrowPen);
			ctx.saveObject(tablePen);
			ctx.saveReference(this, boxBrush, 2);
			ctx.saveReference(this, arrowBrush, 3);
			ctx.saveReference(this, tableBrush, 4);

			ctx.writer.Write((int)boxSelStyle);
			ctx.writer.Write((int)tableSelStyle);

			// new in save format 5
			ctx.writer.Write(allowLinksRepeat);

			// new in save format 6
			ctx.writer.Write(PolyTextLayout);
			ctx.writer.Write(ShapeOrientation);

			// new in save format 8
			ctx.writer.Write(routeArrows);
			ctx.writer.Write(arrowsRetainForm);
			ctx.writer.Write((int)arrowTextStyle);
			ctx.writer.Write((int)tableLinkStyle);

			// new in save format 12
			ctx.writer.Write((int)measureUnit);
			ctx.saveRect(docExtentsMin);

			// new in save format 14
			ctx.writer.Write((int)expandBtnPos);

			ctx.writer.Write((int)Orientation.Auto);	// not used anymore
			ctx.writer.Write((int)Orientation.Auto); // not used anymore

			ctx.writer.Write(enableStyledText);
			ctx.writer.Write(usePolyTextLt);
			ctx.saveReference(this, brush, 5);

			// new in save format 17
			ctx.writer.Write(arrowText);
			ctx.writer.Write(boxText);
			ctx.writer.Write((int)arrowCrossings);
			ctx.writer.Write((int)gridStyle);
			ctx.writer.Write(crossRadius);

			// new in save format 20
			ctx.saveReference(this, exteriorBrush, 6);
			ctx.writer.Write(arrowsSnapToBorders);
			ctx.writer.Write((int)arrowSelStyle);

			// new in save format 21
			bool tagSerializable = false;
			if (Tag != null)
				tagSerializable = Tag.GetType().IsSerializable;
			ctx.writer.Write(tagSerializable);

			if (tagSerializable)
				ctx.saveTag(Tag);

			// new in save format 27
			ctx.writer.Write(roundedArrows);
			ctx.writer.Write(roundedArrowsRadius);

			// write the chart objects
			ctx.writeReferencedObjects();

			if (clearDirty)
				Dirty = false;
		}

		/// <summary>
		/// Loads the FlowChart document from a stream.
		/// </summary>
		public void LoadFromStream(Stream stream)
		{
			try
			{
				loading = true;
				SuspendLayout();
				ClearAll();

				PersistContext ctx = new PersistContext(stream, this);
				ctx.readHeader();

				// read basic-type flowchart properties
				DocInfoVer1 docInfo1 = (DocInfoVer1)ctx.loadObject();
				DocInfoVer2 docInfo2 = (DocInfoVer2)ctx.loadObject();
				docInfo1.apply(this);
				docInfo2.apply(this);

				textFormat = ctx.loadStringFormat();
				Font = ctx.loadFont();
				BackgroundImage = ctx.loadImage();

				// read diagram objects
				zOrder = (ChartObjectCollection)ctx.loadObject();
				groups = (GroupCollection)ctx.loadObject();
				selection = (Selection)ctx.loadObject();

				ctx.loadReference(this);

				if (ctx.FileVersion > 2)
				{
					shapeTemplate = (ShapeTemplate)ctx.loadObject();

					if (ctx.FileVersion > 3)
					{
						Brush.RestoreBrushes(this, new BinaryReader(stream), ctx);
						boxPen = (Pen)ctx.loadObject();
						arrowPen = (Pen)ctx.loadObject();
						tablePen = (Pen)ctx.loadObject();
						ctx.loadReference(this);
						ctx.loadReference(this);
						ctx.loadReference(this);

						boxSelStyle = (HandlesStyle)ctx.reader.ReadInt32();
						tableSelStyle = (HandlesStyle)ctx.reader.ReadInt32();

						if (ctx.FileVersion > 4)
						{
							// new in save format 5
							allowLinksRepeat = ctx.reader.ReadBoolean();

							if (ctx.FileVersion > 5)
							{
								// new in save format 6
								PolyTextLayout = ctx.reader.ReadBoolean();
								ShapeOrientation = ctx.reader.ReadSingle();

								if (ctx.FileVersion > 7)
								{
									// new in save format 8
									routeArrows = ctx.reader.ReadBoolean();
									arrowsRetainForm = ctx.reader.ReadBoolean();
									arrowTextStyle = (ArrowTextStyle)ctx.reader.ReadInt32();
									tableLinkStyle = (TableLinkStyle)ctx.reader.ReadInt32();

									if (ctx.FileVersion >= 12)
									{
										// new in save format 12
										measureUnit = (GraphicsUnit)ctx.reader.ReadInt32();

										docExtentsMin = ctx.loadRectF();

										if (ctx.FileVersion > 13)
										{
											// new in save format 14
											expandBtnPos = (ExpandButtonPosition)ctx.reader.ReadInt32();

											// not used anymore
											ctx.reader.ReadInt32();
											ctx.reader.ReadInt32();

											enableStyledText = ctx.reader.ReadBoolean();
											usePolyTextLt = ctx.reader.ReadBoolean();
											ctx.loadReference(this);	// brush

											if (ctx.FileVersion > 16)
											{
												// new in save format 17
												arrowText = ctx.reader.ReadString();
												boxText = ctx.reader.ReadString();
												arrowCrossings = (ArrowCrossings)ctx.reader.ReadInt32();
												gridStyle = (GridStyle)ctx.reader.ReadInt32();
												crossRadius = ctx.reader.ReadSingle();

												if (ctx.FileVersion > 19)
												{
													// new in save format 20
													ctx.loadReference(this);	// exteriorBrush
													arrowsSnapToBorders = ctx.reader.ReadBoolean();
													arrowSelStyle = (HandlesStyle)ctx.reader.ReadInt32();

													if (ctx.FileVersion > 20)
													{
														// new in save format 21
														Tag = null;
														bool tagAvailable = ctx.reader.ReadBoolean();
														if (tagAvailable)
															Tag = ctx.loadTag();

														if (ctx.FileVersion > 26)
														{
															// new in save format 27
															roundedArrows = ctx.reader.ReadBoolean();
															roundedArrowsRadius = ctx.reader.ReadSingle();
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}

				// finish load
				ctx.loadRemainingObjects();
				ctx.setReferences();

				// add the objects to their specialized collections
				foreach (ChartObject obj in zOrder)
					addToObjColl(obj);
				foreach (Group group in groups)
					group.updateObjCol();

				Brush.freeUnusedBrushes();

				foreach (ChartObject obj in zOrder)
					obj.onLoad();

				dirty = false;
				Invalidate();
			}
			finally
			{
				loading = false;
				ResumeLayout(true);
			}
		}

		private bool loading = false;

		private int[] getArrowOrgs()
		{
			int[] res = new int[arrows.Count];
			int i = 0;
			foreach (ChartObject obj in zOrder)
			{
				if (obj.getType() != ItemType.Arrow) continue;
				Arrow arrow = (Arrow)obj;
				res[i++] = arrow.getOrgnLink().getNode().ZIndex;
			}
			return res;
		}

		private int[] getArrowTrgs()
		{
			int[] res = new int[arrows.Count];
			int i = 0;
			foreach (ChartObject obj in zOrder)
			{
				if (obj.getType() != ItemType.Arrow) continue;
				Arrow arrow = (Arrow)obj;
				res[i++] = arrow.getDestLink().getNode().ZIndex;
			}
			return res;
		}

		public virtual IPersists createObj(int clsId)
		{
			switch (clsId)
			{
				case 2:
					return new Arrow(this);
				case 3:
					return new Box(this);
				case 4:
					return new Table(this);
				case 6:
					return new BoxLink();
				case 7:
					return new TableLink();
				case 8:
					return new Selection(this);
				case 9:
					return new Group(this);
				case 10:
					return new ArrowCollection();
				case 11:
					return new Attachment();
				case 12:
					return new BoxCollection();
				case 13:
					return new ChartObjectCollection();
				case 14:
					return new GroupCollection();
				case 15:
					return new PointCollection(0);
				case 16:
					return new TableCollection();
				case 17:
					return new DocInfoVer1();
				case 18:
					return new DocInfoVer2();
				case 20:
					return new Table.Cell();
				case 21:
					return new Table.CellCollection();
				case 22:
					return new Table.Row();
				case 23:
					return new Table.RowCollection();
				case 24:
					return new Table.Column();
				case 25:
					return new Table.ColumnCollection();
				case 26:
					return new ShapeTemplate(null, FillMode.Winding);
				case 27:
					return new ShapeTemplate.PathData(1);
				case 28:
					return new ArcTemplate(0, 0, 0, 0, 0, 0);
				case 29:
					return new BezierTemplate(0, 0, 0, 0, 0, 0, 0, 0);
				case 30:
					return new LineTemplate(0, 0, 0, 0);
				case 31:
					return new Pen(Color.Black);
				case 32:
					return new HatchBrush(HatchStyle.BackwardDiagonal, Color.Black);
				case 33:
					return new LinearGradientBrush(Color.Black, Color.Black);
				case 34:
					return new SolidBrush(Color.Black);
				case 35:
					return new TextureBrush((Image)null);
				case 36:
					return new AnchorPoint(0, 0);
				case 37:
					return new AnchorPointCollection();
				case 38:
					return new AnchorPattern();
				case 39:
					return new ControlHost(this);
				case 40:
					return new ControlHostLink();
				case 41:
					return new ControlHostCollection();
				case 42:
					return new DummyLink(null, true, new PointF(0, 0));
				case 43:
					return new NodeConstraints();
				default:
					throw new Exception("shouldn't get here");
			}
		}

		[Browsable(false)]
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public bool Dirty
		{
			get { return dirty; }
			set
			{
				if (dirty != value)
				{
					dirty = value;
					if (DirtyChanged != null)
						DirtyChanged(this, new EventArgs());
				}
			}
		}

		internal void setDirty()
		{
			Dirty = true;
		}

		private bool dirty;

		#endregion

		#region clipboard support

		/// <summary>
		/// Copies the current selection of items to the clipboard.
		/// </summary>
		/// <param name="copy">true if you want data to remain on the Clipboard after this application exits; otherwise, false.</param>
		/// <returns>true if items were successfully copied; otherwise, false.</returns>
		public bool CopyToClipboard(bool copy)
		{
			return CopyToClipboard(copy, false);
		}

		/// <summary>
		/// Copies the current selection of items to the clipboard.
		/// </summary>
		/// <param name="copy">true if you want data to remain on the Clipboard after this application exits; otherwise, false.</param>
		/// <param name="groups">true to copy item groups; otherwise, false.</param>
		/// <returns>true if items were successfully copied; otherwise, false.</returns>
		public bool CopyToClipboard(bool copy, bool groups)
		{
			bool result = false;

			// create clones of selected items
			ItemsAndGroups data = copySelection(this, true, groups);
			if (data == null) return false;

			// add the clones to an empty flowchart document
			FlowChart clipHelper = new FlowChart();
			if (pasteSelection(clipHelper, data, null, 0, 0))
			{
				// save the clones into a memory stream
				MemoryStream stream = new MemoryStream(40960);
				clipHelper.SaveToStream(stream, true);

				// copy the memory stream to clipboard
				Clipboard.SetDataObject(stream, copy);

				result = true;
			}
			clipHelper.Dispose();

			return result;
		}

		public bool CutToClipboard(bool copy)
		{
			if (Selection.Objects.Count == 0) return false;

			if (CopyToClipboard(copy))
			{
				// that returns the active composite if somebody has already created one
				CompositeCmd composite = UndoManager.StartComposite("_fcnet_");

				// delete selected items
				ChartObjectCollection temp = new ChartObjectCollection();
				foreach (ChartObject item in Selection.Objects)
					temp.Add(item);

				Selection.Clear();

				foreach (ChartObject item in temp)
					DeleteObject(item);

				if (composite != null && composite.Title == "_fcnet_")
				{
					// this is our own composite cmd
					composite.Title = "Cut";
					composite.Execute();
				}

				return true;
			}

			return false;
		}

		public bool PasteFromClipboard(float dx, float dy)
		{
			return PasteFromClipboard(dx, dy, false);
		}

		public bool PasteFromClipboard(float dx, float dy, bool unconnectedArrows)
		{
			bool result = false;
			IDataObject dataObj = null;
			FlowChart clipHelper= null;
			
			try
			{
				// try getting clipboard data; might throw exceptions
				dataObj = Clipboard.GetDataObject();

				// is there anything of interest in the clipboard ?
				if (dataObj != null && dataObj.GetDataPresent(typeof(MemoryStream)))
				{
					MemoryStream stream = dataObj.
						GetData(typeof(MemoryStream)) as MemoryStream;
					stream.Seek(0, SeekOrigin.Begin);

					// load the stream into an empty flowchart document
					// might throw FileLoadException exceptions
					clipHelper = new FlowChart();
					clipHelper.LoadFromStream(stream);
					foreach (ChartObject item in clipHelper.Objects)
						item.Selected = true;

					// now copy items
					ItemsAndGroups data =
						copySelection(clipHelper, unconnectedArrows, true);
					if (data != null)
					{
						// that returns the active composite if somebody has already created one
						CompositeCmd composite = UndoManager.StartComposite("_fcnet_");

						// add the copied items to the document
						result = pasteSelection(this, data, composite, dx, dy);

						if (composite != null && composite.Title == "_fcnet_")
						{
							// this is our own composite cmd
							composite.Title = "Paste";
							composite.Execute();
						}

						// fire *pasted events
						foreach (ChartObject item in data.items)
							fireItemPasted(item);
					}
				}
			}
			catch (Exception)
			{
				// data could not be retrieved from the clipboard
				// or it was of unrecognized format
				return false;
			}
			finally
			{
				if (clipHelper != null)
					clipHelper.Dispose();
			}

			return result;
		}

		private ItemsAndGroups copySelection(
			FlowChart doc, bool unconnectedArrows, bool copyGroups)
		{
			if (doc.Selection.Objects.Count == 0)
				return null;

			// determine which items and groups to copy
			ChartObjectCollection items = new ChartObjectCollection();
			GroupCollection groups = new GroupCollection();
			Hashtable indexMap = new Hashtable();
			for (int i = 0; i < doc.Selection.Objects.Count; ++i)
			{
				ChartObject item = doc.Selection.Objects[i];

				// do not copy unconncted arrows if specified
				if (!unconnectedArrows && item is Arrow)
				{
					Arrow arrow = item as Arrow;
					if (!arrow.IsConnected) continue;
				}

				indexMap[item] = items.Count;
				items.Add(item);
				
				if (copyGroups && item.SubordinateGroup != null)
					groups.Add(item.SubordinateGroup);
			}

			// add subordinated group items
			foreach (Group group in groups)
			{
				foreach (ChartObject item in group.AttachedObjects)
				{
					if (!items.Contains(item))
					{
						indexMap[item] = items.Count;
						items.Add(item);
					}
				}
			}

			// copy nodes
			for (int i = 0; i < items.Count; ++i)
			{
				ChartObject item = items[i];

				if (item is Box) items[i] = new Box((Box)item);
				if (item is ControlHost) items[i] = new ControlHost((ControlHost)item);
				if (item is Table) items[i] = new Table((Table)item);
			}

			// copy arrows, linking them to node clones
			for (int i = 0; i <  items.Count; ++i)
			{
				if (items[i] is Arrow)
				{
					Arrow arrow = items[i] as Arrow;

					int srcIndex = indexMap.Contains(arrow.Origin) ?
						(int)indexMap[arrow.Origin] : -1;
					int dstIndex = indexMap.Contains(arrow.Destination) ?
						(int)indexMap[arrow.Destination] : -1;

					items[i] = new Arrow(arrow,
						srcIndex == -1 ? Dummy : items[srcIndex] as Node,
						dstIndex == -1 ? Dummy : items[dstIndex] as Node);
				}
			}

			// copy groups
			for (int i = 0; i < groups.Count; ++i)
			{
				Group group = new Group(groups[i]);
				groups[i] = group;
				group.setMainObject(items[(int)indexMap[group.MainObject]]);

				foreach (Attachment atc in group.Attachments)
				{
					atc.node = items[(int)indexMap[atc.node]] as Node;
					atc.node.putInGroup(group);
				}
				group.updateObjCol();
			}

			return new ItemsAndGroups(items, groups);
		}

		private bool pasteSelection(FlowChart doc, ItemsAndGroups data,
			CompositeCmd cmd, float dx, float dy)
		{
			if (data.items.Count == 0) return false;
			doc.Selection.Clear();

			// add nodes
			foreach (ChartObject item in data.items)
			{
				if (item is Node)
				{
					doc.Add(item);
					doc.Selection.AddObject(item);
				}
			}

			Hashtable arrowPoints = null;
			if (dx != 0 || dy != 0)
				arrowPoints = new Hashtable();

			foreach (ChartObject item in data.items)
			{
				if (item is Arrow)
				{
					Arrow arrow = item as Arrow;

					doc.Add(arrow);
					doc.Selection.AddObject(arrow);

					if (arrowPoints != null)
						arrowPoints.Add(arrow, arrow.ControlPoints.Clone());
				}
			}

			// add groups
			foreach (Group group in data.groups)
				doc.Add(group);

			if (dx != 0 || dy != 0)
			{
				// offset nodes
				foreach (ChartObject item in data.items)
				{
					if (item is Node && item.MasterGroup == null)
					{
						ModifyItemCmd mc = (cmd == null) ? null : new ModifyItemCmd(item);

						Node node = item as Node;
						RectangleF rect = node.BoundingRect;
						rect.Offset(dx, dy);
						node.BoundingRect = rect;

						if (mc != null)
						{
							mc.Execute(true);
							cmd.AddSubCmd(mc);
						}
					}
				}

				// offset arrows
				foreach (ChartObject item in data.items)
				{
					if (item is Arrow)
					{
						ModifyItemCmd mc = (cmd == null) ? null : new ModifyItemCmd(item);

						Arrow arrow = item as Arrow;
						arrow.Points = arrowPoints[arrow] as PointCollection;
						for (int i = 0; i < arrow.ControlPoints.Count; ++i)
						{
							PointF pt = arrow.ControlPoints[i];
							arrow.ControlPoints[i] = new PointF(pt.X + dx, pt.Y + dy);
						}
						arrow.UpdateFromPoints();

						if (mc != null)
						{
							mc.Execute(true);
							cmd.AddSubCmd(mc);
						}
					}
				}
			}

			return true;
		}

		#endregion

		#region default values

		/// <summary>
		/// Default number of rows for new tables
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(4)]
		[Description("Initial number of rows in new tables.")]
		public int TableRowCount
		{
			get
			{
				return tableRowsCount;
			}
			set
			{
				if (tableRowsCount != value && value >= 0)
				{
					tableRowsCount = value;
					setDirty();
				}
			}
		}

		private int tableRowsCount;

		/// <summary>
		/// Default number of columns for new tables
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(2)]
		[Description("Initial number of columns in new tables.")]
		public int TableColumnCount
		{
			get
			{
				return tableColumnsCount;
			}
			set
			{
				if (tableColumnsCount != value && value >= 0)
				{
					tableColumnsCount = value;
					setDirty();
				}
			}
		}

		private int tableColumnsCount;

		/// <summary>
		/// Sets the offset at which the shadows are drawn
		/// </summary>
		public void SetShadowOffset(float x, float y)
		{
			ShadowOffsetX = x;
			ShadowOffsetY = y;
		}

		/// <summary>
		/// Default for the CustomDraw property of new boxes
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(typeof(CustomDraw), "None")]
		[Description("The initial value of the CustomDraw property of boxes.")]
		public CustomDraw BoxCustomDraw
		{
			get
			{
				return boxCustomDraw;
			}
			set
			{
				if (boxCustomDraw != value)
				{
					boxCustomDraw = value;
					setDirty();
				}
			}
		}

		private CustomDraw boxCustomDraw;

		/// <summary>
		/// Gets or sets the default value for the CustomDraw property of new tables.
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(typeof(CustomDraw), "None")]
		[Description("Gets or sets the default value for the CustomDraw property of new tables.")]
		public CustomDraw TableCustomDraw
		{
			get
			{
				return tableCustomDraw;
			}
			set
			{
				if (tableCustomDraw != value)
				{
					tableCustomDraw = value;
					setDirty();
				}
			}
		}

		private CustomDraw tableCustomDraw;

		/// <summary>
		/// Gets or sets the default value for the CellCustomDraw property of new tables.
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(typeof(CustomDraw), "None")]
		[Description("Gets or sets the default value for the CellCustomDraw property of new tables.")]
		public CustomDraw CellCustomDraw
		{
			get
			{
				return cellCustomDraw;
			}
			set
			{
				if (cellCustomDraw != value)
				{
					cellCustomDraw = value;
					setDirty();
				}
			}
		}

		private CustomDraw cellCustomDraw;

		/// <summary>
		/// Default for the CustomDraw property of new boxes
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(typeof(CustomDraw), "None")]
		[Description("The initial value of the CustomDraw property of arrows.")]
		public CustomDraw ArrowCustomDraw
		{
			get
			{
				return arrowCustomDraw;
			}
			set
			{
				if (arrowCustomDraw != value)
				{
					arrowCustomDraw = value;
					setDirty();
				}
			}
		}

		private CustomDraw arrowCustomDraw;

		/// <summary>
		/// Horizontal offset of shadows from their items.
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(1.0f)]
		[Description("Horizontal offset of shadows from the items that cast them.")]
		public float ShadowOffsetX
		{
			get
			{
				return shadowOffsetX;
			}
			set
			{
				if (shadowOffsetX != value)
				{
					shadowOffsetX = value;
					setDirty();
				}
			}
		}

		private float shadowOffsetX;

		/// <summary>
		/// Vertical offset of the shadows
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(1.0f)]
		[Description("Vertical offset of shadows from the items that cast them.")]
		public float ShadowOffsetY
		{
			get
			{
				return shadowOffsetY;
			}
			set
			{
				if (shadowOffsetY != value)
				{
					shadowOffsetY = value;
					setDirty();
				}
			}
		}

		private float shadowOffsetY;

		/// <summary>
		/// Default for the Dynamic property of new arrows
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(false)]
		[Description("Initial value of arrows' Dynamic property, specifying whether arrows automatically adjust the coordinates of their end points to align them to node outlines.")]
		public bool DynamicArrows
		{
			get
			{
				return dynamicArrows;
			}
			set
			{
				if (dynamicArrows != value)
				{
					dynamicArrows = value;
					setDirty();
				}
			}
		}

		private bool dynamicArrows;

		/// <summary>
		/// Default for the Dynamic property of new arrows
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(false)]
		[Description("Initial value of arrows' SnapToNodeBorder property, specifying whether arrow end points are automatically aligned to node borders.")]
		public bool ArrowsSnapToBorders
		{
			get
			{
				return arrowsSnapToBorders;
			}
			set
			{
				if (arrowsSnapToBorders != value)
				{
					arrowsSnapToBorders = value;
					setDirty();
				}
			}
		}

		private bool arrowsSnapToBorders;

		/// <summary>
		/// Default for the Polar property of new arrows
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(false)]
		[Description("Initial value of arrows' RetainForm property, specifying whether arrows automatically adjust the coordinates of their points so their relative initial position remains the same.")]
		public bool ArrowsRetainForm
		{
			get
			{
				return arrowsRetainForm;
			}
			set
			{
				if (arrowsRetainForm != value)
				{
					arrowsRetainForm = value;
					setDirty();
				}
			}
		}

		private bool arrowsRetainForm;

		/// <summary>
		/// Default value for the FillColor of new tables
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(typeof(Color), "180, 160, 160")]
		[Description("Interior of tables is painted with this color.")]
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public Color TableFillColor
		{
			get
			{
				return tableFillColor;
			}
			set
			{
				if (!tableFillColor.Equals(value))
				{
					tableFillColor = value;

					// Reset the brush
					TableBrush = new SolidBrush(tableFillColor);

					setDirty();
				}
			}
		}

		private Color tableFillColor;

		/// <summary>
		/// Default value for the FrameColor of new tables
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(typeof(Color), "Black")]
		[Description("Color of table frame lines.")]
		public Color TableFrameColor
		{
			get
			{
				return tableFrameColor;
			}
			set
			{
				if (!tableFrameColor.Equals(value))
				{
					tableFrameColor = value;

					// Reset the pen
					tablePen.Color = tableFrameColor;

					setDirty();
				}
			}
		}

		private Color tableFrameColor;

		/// <summary>
		/// Default width of table columns
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(18.0f)]
		[Description("The default width of table columns.")]
		public float TableColWidth
		{
			get
			{
				return tableColWidth;
			}
			set
			{
				if (tableColWidth != value && value >= 0)
				{
					tableColWidth = value;
					setDirty();
				}
			}
		}

		private float tableColWidth;

		/// <summary>
		/// Default height of table rows
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(6.0f)]
		[Description("The default height of table rows.")]
		public float TableRowHeight
		{
			get
			{
				return tableRowHeight;
			}
			set
			{
				if (tableRowHeight != value && value >= 0)
				{
					tableRowHeight = value;
					setDirty();
				}
			}
		}

		private float tableRowHeight;

		/// <summary>
		/// Default height of table captions
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(5.0f)]
		[Description("Default height of table captions.")]
		public float TableCaptionHeight
		{
			get
			{
				return tableCaptionHeight;
			}
			set
			{
				if (tableCaptionHeight != value && value >= 0)
				{
					tableCaptionHeight = value;
					setDirty();
				}
			}
		}

		private float tableCaptionHeight;

		[Category("Defaults")]
		[DefaultValue(typeof(TableLinkStyle), "Rows")]
		[Description("Specifies how tables can be related one to another - as integral entities, by rows, or both.")]
		public TableLinkStyle TableLinkStyle
		{
			get
			{
				return tableLinkStyle;
			}
			set
			{
				if (tableLinkStyle != value)
				{
					tableLinkStyle = value;
					setDirty();
				}
			}
		}

		private TableLinkStyle tableLinkStyle;

		/// <summary>
		/// Defines the orientation of the first segment of cascading arrows
		/// </summary>
		[Category("Behavior")]
		[DefaultValue(typeof(Orientation), "Auto")]
		[Description("Orientation of the first segment of cascading arrows.")]
		public Orientation ArrowCascadeOrientation
		{
			get
			{
				return arrowCascadeOrientation;
			}
			set
			{
				if (arrowCascadeOrientation != value)
				{
					arrowCascadeOrientation = value;
					setDirty();
				}
			}
		}

		private Orientation arrowCascadeOrientation;
		
		/// <summary>
		/// Visual style of table cell borders
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(typeof(CellFrameStyle), "System3D")]
		[Description("Gets or sets a value specifying the visual style of cell border lines.")]
		public CellFrameStyle CellFrameStyle
		{
			get
			{
				return tableCellBorders;
			}
			set
			{
				tableCellBorders = value;
			}
		}

		private CellFrameStyle tableCellBorders;

		[Category("Defaults")]
		[DefaultValue(false)]
		[Description("Specifies the initial value of the Expandable property of new boxes.")]
		public bool BoxesExpandable
		{
			get { return boxesExpandable; }
			set
			{
				if (boxesExpandable != value)
				{
					boxesExpandable = value;
					setDirty();
				}
			}
		}

		private bool boxesExpandable;

		[Category("Defaults")]
		[DefaultValue(false)]
		public bool PolyTextLayout
		{
			get { return usePolyTextLt; }
			set { usePolyTextLt = value; }
		}

		private bool usePolyTextLt;

		[Category("Defaults")]
		[DefaultValue(0.0f)]
		public float ShapeOrientation
		{
			get { return shapeRotation; }
			set
			{
				shapeRotation = value;
				if (shapeRotation != 0 && shapeRotation != 90 &&
					shapeRotation != 180 && shapeRotation != 270)
					shapeRotation = 0;
			}
		}

		private float shapeRotation;

		[Browsable(false)]
		public System.Type DefaultControlType
		{
			get { return defaultControlType; }
			set
			{
				// null is unacceptible
				if (value == null)
				{
					defaultControlType = null;
					return;
				}

				// Check if the type is derived from System.Windows.Forms.Control
				if (!value.IsSubclassOf(typeof(Control)))
					throw new Exception("The control type must derive from System.Windows.Forms.Control");

				// Check if the type has 0-parameter constructor
				ConstructorInfo ctorInfo = value.GetConstructor(System.Type.EmptyTypes);
				if (ctorInfo == null)
					throw new Exception("The type required needs to supple constructor with 0 parameters");

				defaultControlType = value;
				setDirty();
			}
		}

		private System.Type defaultControlType;

		[Category("Defaults")]
		[DefaultValue(typeof(HostMouseAction), "SelectHost")]
		public HostMouseAction HostedCtrlMouseAction
		{
			get { return hostedCtrlMouseAction; }
			set
			{
				hostedCtrlMouseAction = value;
				setDirty();
			}
		}

		private HostMouseAction hostedCtrlMouseAction;

		[Category("Defaults")]
		[DefaultValue(false)]
		public bool EnableStyledText
		{
			get { return enableStyledText; }
			set { enableStyledText = value; }
		}

		private bool enableStyledText;

		[Category("Defaults")]
		[DefaultValue(typeof(ArrowHead), "Arrow")]
		[Description("Specifies what shape to display at arrows' end points.")]
		public ArrowHead ArrowHead
		{
			get
			{
				return arrowHead;
			}
			set
			{
				if (arrowHead != value)
				{
					arrowHead = value;
					setDirty();
				}
			}
		}

		[Category("Defaults")]
		[DefaultValue(typeof(ArrowHead), "None")]
		[Description("Specifies what shape to display at arrows' origin points.")]
		public ArrowHead ArrowBase
		{
			get
			{
				return arrowBase;
			}
			set
			{
				if (arrowBase != value)
				{
					arrowBase = value;
					setDirty();
				}
			}
		}

		[Category("Defaults")]
		[DefaultValue(typeof(ArrowHead), "None")]
		[Description("Specifies what shape to display at the middles of arrows segments.")]
		public ArrowHead IntermArrowHead
		{
			get
			{
				return arrowInterm;
			}
			set
			{
				if (arrowInterm != value)
				{
					arrowInterm = value;
					setDirty();
				}
			}
		}

		ArrowHead arrowHead;
		ArrowHead arrowBase;
		ArrowHead arrowInterm;

		[Category("Defaults")]
		[DefaultValue(5.0f)]
		[Description("Gets or sets the default size of ArrowHead shapes.")]
		public float ArrowHeadSize
		{
			get { return arrowHeadSize; }
			set
			{
				if (value >= Constants.getMinArrowheadSize(measureUnit) && arrowHeadSize != value)
				{
					arrowHeadSize = value;
					setDirty();
				}
			}
		}

		[Category("Defaults")]
		[DefaultValue(5.0f)]
		[Description("Size of ArrowBase shapes.")]
		public float ArrowBaseSize
		{
			get { return arrowBaseSize; }
			set
			{
				if (value >= Constants.getMinArrowheadSize(measureUnit) && arrowBaseSize != value)
				{
					arrowBaseSize = value;
					setDirty();
				}
			}
		}

		[Category("Defaults")]
		[DefaultValue(5.0f)]
		[Description("Gets or sets the default size of IntermArrowHead shapes.")]
		public float ArrowIntermSize
		{
			get { return arrowIntermSize; }
			set
			{
				if (value >= Constants.getMinArrowheadSize(measureUnit) && arrowIntermSize != value)
				{
					arrowIntermSize = value;
					setDirty();
				}
			}
		}

		float arrowHeadSize;
		float arrowBaseSize;
		float arrowIntermSize;

		/// <summary>
		/// Default value for the FillColor of new boxes
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(typeof(Color), "220, 220, 255")]
		[Description("Gets or sets the color used to paint the interior of boxes.")]
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public Color BoxFillColor
		{
			get
			{
				return boxFillColor;
			}
			set
			{
				if (!boxFillColor.Equals(value))
				{
					boxFillColor = value;

					// Reset the brush
					BoxBrush = new SolidBrush(boxFillColor);

					setDirty();
				}
			}
		}

		private Color boxFillColor;

		/// <summary>
		/// Default value for the FillColor of new arrows
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(typeof(Color), "120, 220, 255")]
		[Description("Gets or sets the color used to paint the interior of arrowheads.")]
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public Color ArrowFillColor
		{
			get
			{
				return arrowFillColor;
			}
			set
			{
				if (!arrowFillColor.Equals(value))
				{
					arrowFillColor = value;

					// Reset the brush
					ArrowBrush = new SolidBrush(arrowFillColor);

					setDirty();
				}
			}
		}

		private Color arrowFillColor;

		/// <summary>
		/// Default value for the FrameColor of new boxes
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(typeof(Color), "Black")]
		[Description("Gets or sets the color used to paint box frame lines.")]
		public Color BoxFrameColor
		{
			get
			{
				return boxFrameColor;
			}
			set
			{
				if (!boxFrameColor.Equals(value))
				{
					boxFrameColor = value;

					// Reset the pen
					boxPen.Color = boxFrameColor;

					setDirty();
				}
			}
		}

		private Color boxFrameColor;

		/// <summary>
		/// Default value for the the color of new arrows
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(typeof(Color), "Black")]
		[Description("Gets or sets the color used to paint arrow lines.")]
		public Color ArrowColor
		{
			get
			{
				return arrowColor;
			}
			set
			{
				if (!arrowColor.Equals(value))
				{
					arrowColor = value;

					// Reset the pen
					arrowPen.Color = arrowColor;

					setDirty();
				}
			}
		}

		private Color arrowColor;

		/// <summary>
		/// Default value for the the text orientation of arrows
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(typeof(ArrowTextStyle), "Center")]
		[Description("Specifies the orientation and placement of arrows text.")]
		public ArrowTextStyle ArrowTextStyle
		{
			get
			{
				return arrowTextStyle;
			}
			set
			{
				if (arrowTextStyle != value)
				{
					arrowTextStyle = value;
					setDirty();
				}
			}
		}

		private ArrowTextStyle arrowTextStyle;

		/// <summary>
		/// Default pen for new boxes.
		/// </summary>
		[Category("Defaults")]
		[Description("Pen used to paint box frame lines.")]
		[Editor(typeof(MindFusion.FlowChartX.Design.PenEditor),
			 typeof(System.Drawing.Design.UITypeEditor))]
		public MindFusion.FlowChartX.Pen BoxPen
		{
			get
			{
				return boxPen;
			}
			set
			{
				if (value == null)
					throw new ArgumentException("null is not acceptable Pen value", "value");

				boxPen = value;
				boxFrameColor = value.Color;

				setDirty();
			}
		}

		private MindFusion.FlowChartX.Pen boxPen;

		private bool ShouldSerializeBoxPen()
		{
			return !boxPen.equals(new Pen(Color.Black, 0));
		}

		/// <summary>
		/// Default brush for new boxes.
		/// </summary>
		[Category("Defaults")]
		[Description("Brush used to fill interior of boxes.")]
		[Editor(typeof(MindFusion.FlowChartX.Design.BrushEditor),
			 typeof(System.Drawing.Design.UITypeEditor))]
		public MindFusion.FlowChartX.Brush BoxBrush
		{
			get
			{
				return boxBrush;
			}
			set
			{
				if(value == null)
					throw new Exception("null is not acceptable Brush value");

				boxBrush.Release();
				boxBrush = value;
				boxBrush.AddRef();

				MindFusion.FlowChartX.SolidBrush solidBrush = value as
					MindFusion.FlowChartX.SolidBrush;
				if (solidBrush != null)
					boxFillColor = solidBrush.Color;

				setDirty();
			}
		}

		private MindFusion.FlowChartX.Brush boxBrush;

		private bool ShouldSerializeBoxBrush()
		{
			return !boxBrush.equals(new SolidBrush(Color.FromArgb(220, 220, 255)));
		}

		/// <summary>
		/// Default pen for new arrows.
		/// </summary>
		[Category("Defaults")]
		[Description("Pen used to paint arrow lines.")]
		[Editor(typeof(MindFusion.FlowChartX.Design.PenEditor),
			 typeof(System.Drawing.Design.UITypeEditor))]
		public MindFusion.FlowChartX.Pen ArrowPen
		{
			get
			{
				return arrowPen;
			}
			set
			{
				if (value == null)
					throw new ArgumentException("null is not acceptable Pen value", "value");

				arrowPen = value;
				arrowColor = value.Color;

				setDirty();
			}
		}

		private MindFusion.FlowChartX.Pen arrowPen;

		private bool ShouldSerializeArrowPen()
		{
			return !arrowPen.equals(new Pen(Color.Black, 0));
		}

		/// <summary>
		/// Default brush for new arrows.
		/// </summary>
		[Category("Defaults")]
		[Description("Brush used to fill interior of arrowheads.")]
		[Editor(typeof(MindFusion.FlowChartX.Design.BrushEditor),
			 typeof(System.Drawing.Design.UITypeEditor))]
		public MindFusion.FlowChartX.Brush ArrowBrush
		{
			get
			{
				return arrowBrush;
			}
			set
			{
				if(value == null)
					throw new Exception("null is not acceptable Brush value");

				arrowBrush.Release();
				arrowBrush = value;
				arrowBrush.AddRef();

				MindFusion.FlowChartX.SolidBrush solidBrush = value as
					MindFusion.FlowChartX.SolidBrush;
				if (solidBrush != null)
					arrowFillColor = solidBrush.Color;

				setDirty();
			}
		}

		private MindFusion.FlowChartX.Brush arrowBrush;

		private bool ShouldSerializeArrowBrush()
		{
			return !arrowBrush.equals(new SolidBrush(Color.FromArgb(120, 220, 255)));
		}

		/// <summary>
		/// Default pen for new tables.
		/// </summary>
		[Category("Defaults")]
		[Description("Pen used to paint table frame lines.")]
		[Editor(typeof(MindFusion.FlowChartX.Design.PenEditor),
			 typeof(System.Drawing.Design.UITypeEditor))]
		public MindFusion.FlowChartX.Pen TablePen
		{
			get
			{
				return tablePen;
			}
			set
			{
				if (value == null)
					throw new ArgumentException("null is not acceptable Pen value", "value");

				tablePen = value;
				tableFrameColor = value.Color;

				setDirty();
			}
		}

		private MindFusion.FlowChartX.Pen tablePen;

		private bool ShouldSerializeTablePen()
		{
			return !tablePen.equals(new Pen(Color.Black, 0));
		}

		/// <summary>
		/// Default brush for new tables.
		/// </summary>
		[Category("Defaults")]
		[Description("Brush used to fill interior of tables.")]
		[Editor(typeof(MindFusion.FlowChartX.Design.BrushEditor),
			 typeof(System.Drawing.Design.UITypeEditor))]
		public MindFusion.FlowChartX.Brush TableBrush
		{
			get
			{
				return tableBrush;
			}
			set
			{
				if(value == null)
					throw new Exception("null is not acceptable Brush value");

				tableBrush.Release();
				tableBrush = value;
				tableBrush.AddRef();

				MindFusion.FlowChartX.SolidBrush solidBrush = value as
					MindFusion.FlowChartX.SolidBrush;
				if (solidBrush != null)
					tableFillColor = solidBrush.Color;

				setDirty();
			}
		}

		private MindFusion.FlowChartX.Brush tableBrush;

		private bool ShouldSerializeTableBrush()
		{
			return !tableBrush.equals(new SolidBrush(Color.FromArgb(180, 160, 160)));
		}

		/// <summary>
		/// The default box style
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(typeof(BoxStyle), "RoundedRectangle")]
		[Description("Style of boxes.")]
		public BoxStyle BoxStyle
		{
			get
			{	
				return boxStyle;
			}
			set
			{
				if (boxStyle != value)
				{
					boxStyle = value;
					setDirty();
				
					if (BoxStyleChanged != null)
						BoxStyleChanged(this, new EventArgs());
				}
			}
		}

		private BoxStyle boxStyle;

		/// <summary>
		/// The default box style
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(typeof(TableStyle), "Rectangle")]
		[Description("Style of tables.")]
		public TableStyle TableStyle
		{
			get
			{	
				return tableStyle;
			}
			set
			{
				if (tableStyle != value)
				{
					tableStyle = value;
					setDirty();
				}
			}
		}

		private TableStyle tableStyle;

		/// <summary>
		/// The color of the objects shadows
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(typeof(Color), "110, 110, 140")]
		[Description("Shadows are painted using this color.")]
		public Color ShadowColor
		{
			get
			{
				return shadowColor;
			}
			set
			{
				if (shadowColor != value)
				{
					shadowColor = value;
					setDirty();
					Invalidate();
				}
			}
		}

		private Color shadowColor;

		/// <summary>
		/// Default value for the alignment of boxes text
		/// </summary>
		[Category("Defaults")]
		[Editor(typeof(MindFusion.FlowChartX.Design.StringFormatEditor),
			 typeof(System.Drawing.Design.UITypeEditor))]
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
		public StringFormat TextFormat
		{
			get
			{
				return textFormat;
			}
			set
			{
				if (!textFormat.Equals(value))
				{
					textFormat = (StringFormat)value.Clone();
					setDirty();
				}
			}
		}

		private StringFormat textFormat;

		private bool ShouldSerializeTextFormat()
		{
			return
				textFormat.Alignment != StringAlignment.Center ||
				textFormat.LineAlignment != StringAlignment.Center;
		}

		[Category("Defaults")]
		[DefaultValue(false)]
		[Description("Specifies the initial value of the Scrollable property of new tables.")]
		public bool TablesScrollable
		{
			get { return tablesScrollable; }
			set
			{
				if (tablesScrollable != value)
				{
					tablesScrollable = value;
					setDirty();
				}
			}
		}

		private bool tablesScrollable;

		[Category("Defaults")]
		[DefaultValue(false)]
		public bool TablesExpandable
		{
			get { return tablesExpandable; }
			set
			{
				if (tablesExpandable != value)
				{
					tablesExpandable = value;
					setDirty();
				}
			}
		}

		private bool tablesExpandable;

		[Category("Defaults")]
		[DefaultValue(false)]
		public bool ControlHostsExpandable
		{
			get { return controlHostsExpandable; }
			set
			{
				if (controlHostsExpandable != value)
				{
					controlHostsExpandable = value;
					setDirty();
				}
			}
		}

		private bool controlHostsExpandable;

		/// <summary>
		/// Default value of the TextColor property of new objects
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(typeof(Color), "Black")]
		[Description("Gets or sets the default text color.")]
		public Color TextColor
		{
			get
			{
				return textColor;
			}
			set
			{
				if (!textColor.Equals(value))
				{
					textColor = value;
					setDirty();
				}
			}
		}

		private Color textColor;

		/// <summary>
		/// Default arrow style
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(typeof(ArrowStyle), "Polyline")]
		[Description("The initial style of arrows.")]
		public ArrowStyle ArrowStyle
		{
			get
			{
				return arrowStyle;
			}
			set
			{
				if (arrowStyle != value)
				{
					if (value == ArrowStyle.Cascading &&
						arrowSegments == 1)
						arrowSegments = 2;

					arrowStyle = value;
					setDirty();
				}
			}
		}

		private ArrowStyle arrowStyle;

		/// <summary>
		/// Default segment count for new arrows
		/// </summary>
		[Category("Defaults")]
		[DefaultValue((short)1)]
		[Description("Specifies the initial number of segments of new arrows.")]
		public short ArrowSegments
		{
			get
			{
				return arrowSegments;
			}
			set
			{
				if (arrowSegments != value && value > 0)
				{
					arrowSegments = value;
					setDirty();

					if (arrowStyle == ArrowStyle.Cascading && value == 1)
						arrowSegments = 2;
				}
			}
		}

		private short arrowSegments;

		[Category("Defaults")]
		[DefaultValue(typeof(HandlesStyle), "SquareHandles")]
		[Description("Specifies the type and appearance of arrow selection handles.")]
		public HandlesStyle ArrowHandlesStyle
		{
			get { return arrowSelStyle; }
			set
			{
				if (value != arrowSelStyle)
				{
					arrowSelStyle = value;
					setDirty();
				}
			}
		}

		[Category("Defaults")]
		[DefaultValue(typeof(HandlesStyle), "SquareHandles")]
		[Description("Specifies the type and appearance of box selection handles.")]
		public HandlesStyle BoxHandlesStyle
		{
			get { return boxSelStyle; }
			set
			{
				if (value != boxSelStyle)
				{
					boxSelStyle = value;
					setDirty();
				}
			}
		}

		[Category("Defaults")]
		[DefaultValue(typeof(HandlesStyle), "HatchHandles")]
		[Description("Specifies the type and appearance of ControlHost selection handles.")]
		public HandlesStyle ControlHostHandlesStyle
		{
			get { return chostSelStyle; }
			set
			{
				if (value != chostSelStyle)
				{
					chostSelStyle = value;
					setDirty();
				}
			}
		}

		[Category("Defaults")]
		[DefaultValue(typeof(HandlesStyle), "DashFrame")]
		public HandlesStyle TableHandlesStyle
		{
			get { return tableSelStyle; }
			set
			{
				if (value != tableSelStyle)
				{
					tableSelStyle = value;
					setDirty();
				}
			}
		}

		private HandlesStyle arrowSelStyle;
		private HandlesStyle boxSelStyle;
		private HandlesStyle chostSelStyle;
		private HandlesStyle tableSelStyle;

		/// <summary>
		/// The default pen dash style of new objects
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(typeof(DashStyle), "Solid")]
		public DashStyle PenDashStyle
		{
			get
			{
				return penDashStyle;
			}
			set
			{
				if (!penDashStyle.Equals(value))
				{
					penDashStyle = value;

					arrowPen.DashStyle = penDashStyle;
					boxPen.DashStyle = penDashStyle;
					tablePen.DashStyle = penDashStyle;

					setDirty();
				}
			}
		}

		private DashStyle penDashStyle;

		/// <summary>
		/// The default pen width of new objects
		/// </summary>
		[Category("Defaults")]
		[DefaultValue(0.0f)]
		[Description("The default width of pens used to draw the outlines of items.")]
		public float PenWidth
		{
			get
			{
				return penWidth;
			}
			set
			{
				if (!penWidth.Equals(value) && value >= 0)
				{
					penWidth = value;

					arrowPen.Width = penWidth;
					boxPen.Width = penWidth;
					tablePen.Width = penWidth;

					setDirty();
				}
			}
		}

		float penWidth;

		[Browsable(false)]
		public ShapeTemplate DefaultShape
		{
			get { return shapeTemplate; }
			set
			{
				if(value.ID != "")
					shapeTemplate = value;
				else
					shapeTemplate = (ShapeTemplate)value.Clone();

				if (DefaultShapeChanged != null)
					DefaultShapeChanged(this, new EventArgs());
			}
		}

		private ShapeTemplate shapeTemplate;

		/// <summary>
		/// Default table caption
		/// </summary>
		[Category("Defaults")]
		[DefaultValue("Table")]
		[Description("Default table caption.")]
		public string TableCaption
		{
			get
			{
				return tableCaption;
			}
			set
			{
				if (!tableCaption.Equals(value))
				{
					if (value == null)
						tableCaption = "";
					else
						tableCaption = value;
					setDirty();
				}
			}
		}

		private string tableCaption;

		/// <summary>
		/// Default value for the Text property of boxes
		/// </summary>
		[Category("Defaults")]
		[DefaultValue("")]
		[Description("Default value of the Text property of boxes.")]
		public string BoxText
		{
			get
			{
				return boxText;
			}
			set
			{
				if (!boxText.Equals(value))
				{
					if (value == null)
						boxText = "";
					else
						boxText = value;
					setDirty();
				}
			}
		}

		private string boxText;

		/// <summary>
		/// Default value for the Text property of arrows
		/// </summary>
		[Category("Defaults")]
		[DefaultValue("")]
		[Description("Default value of the Text property of arrows.")]
		public string ArrowText
		{
			get
			{
				return arrowText;
			}
			set
			{
				if (!arrowText.Equals(value))
				{
					if (value == null)
						arrowText = "";
					else
						arrowText = value;
					setDirty();
				}
			}
		}

		private string arrowText;

		#endregion

		#region events

		[Category("Arrows")]
		[Description("Raised when a user starts drawing an arrow.")]
		public event ArrowEvent InitializeArrow;

		[Category("Arrows")]
		[Description("Raised when the user draws a new arrow.")]
		public event ArrowEvent ArrowCreated;

		[Category("Arrows")]
		[Description("Raised when an arrow is deleted.")]
		public event ArrowEvent ArrowDeleted;

		[Category("Arrows")]
		[Description("Raised when the user moves an arrow control point.")]
		public event ArrowMouseEvent ArrowModified;

		[Category("Arrows")]
		[Description("Raised while the user moves an arrow control point.")]
		public event ArrowConfirmation ArrowModifying;

		[Category("Arrows")]
		[Description("Raised when an arrow is activated.")]
		public event ArrowEvent ArrowActivated;

		[Category("Arrows")]
		[Description("Raised when an arrow is deselected.")]
		public event ArrowEvent ArrowDeselected;

		[Category("Arrows")]
		[Description("Raised when an arrow is deactivated.")]
		public event ArrowEvent ArrowDeactivated;

		[Category("Arrows")]
		[Description("Lets you cancel arrow creation.")]
		public event AttachConfirmation ArrowCreating;

		[Category("Arrows")]
		[Description("Lets you cancel arrow deletion.")]
		public event ArrowConfirmation ArrowDeleting;

		[Category("Arrows")]
		[Description("Lets you cancel arrow selection.")]
		public event ArrowConfirmation ArrowSelecting;

		[Category("Arrows")]
		[Description("Lets you cancel attaching arrow to a node.")]
		public event AttachConfirmation ArrowAttaching;

		[Category("Arrows")]
		[Description("Raised when an arrow is clicked.")]
		public event ArrowMouseEvent ArrowClicked;

		[Category("Arrows")]
		[Description("Raised when an arrow is double-clicked.")]
		public event ArrowMouseEvent ArrowDblClicked;

		[Category("Arrows")]
		[Description("Raised when an arrow is routed.")]
		public event ArrowEvent ArrowRouted;

		[Category("Arrows")]
		[Description("Lets you validate anchor points when an arrow connects to a node.")]
		public event AttachConfirmation ValidateAnchorPoint;

		[Category("Boxes")]
		[Description("Raised when a user starts drawing a box.")]
		public event BoxEvent InitializeBox;

		[Category("Boxes")]
		[Description("Raised when the user draws a new box.")]
		public event BoxEvent BoxCreated;

		[Category("Boxes")]
		[Description("Raised when a box is deleted.")]
		public event BoxEvent BoxDeleted;

		[Category("Boxes")]
		[Description("Raised when the user moves or resizes a box.")]
		public event BoxMouseEvent BoxModified;

		[Category("Boxes")]
		[Description("Raised while the user moves or resizes a box.")]
		public event BoxConfirmation BoxModifying;

		[Category("Boxes")]
		[Description("Raised when a box is activated.")]
		public event BoxEvent BoxActivated;

		[Category("Boxes")]
		[Description("Raised when a box is deselected.")]
		public event BoxEvent BoxDeselected;

		[Category("Boxes")]
		[Description("Raised when a box is deactivated.")]
		public event BoxEvent BoxDeactivated;

		[Category("Boxes")]
		[Description("Lets you cancel box creation.")]
		public event BoxConfirmation BoxCreating;

		[Category("Boxes")]
		[Description("Lets you cancel box deletion.")]
		public event BoxConfirmation BoxDeleting;

		[Category("Boxes")]
		[Description("Lets you cancel box selection.")]
		public event BoxConfirmation BoxSelecting;

		[Category("Boxes")]
		[Description("Raised when a box is clicked.")]
		public event BoxMouseEvent BoxClicked;

		[Category("Boxes")]
		[Description("Raised when a box is double-clicked.")]
		public event BoxMouseEvent BoxDblClicked;

		[Category("Clipboard")]
		[Description("Raised when an arrow is pasted from the clipboard.")]
		public event ArrowEvent ArrowPasted;

		[Category("Clipboard")]
		[Description("Raised when a box is pasted from the clipboard.")]
		public event BoxEvent BoxPasted;

		[Category("Clipboard")]
		[Description("Raised when a control host node is pasted from the clipboard.")]
		public event ControlHostEvent ControlHostPasted;

		[Category("Clipboard")]
		[Description("Raised when a table is pasted from the clipboard.")]
		public event TableEvent TablePasted;

		[Category("Control Hosts")]
		[Description("Raised when a user starts drawing a ControlHost node.")]
		public event ControlHostEvent InitializeControlHost;

		[Category("Control Hosts")]
		[Description("Raised when a control host is deleted.")]
		public event ControlHostEvent ControlHostDeleted;

		[Category("Control Hosts")]
		[Description("Raised when a control host is activated.")]
		public event ControlHostEvent ControlHostActivated;

		[Category("Control Hosts")]
		[Description("Raised when a control host is deselected.")]
		public event ControlHostEvent ControlHostDeselected;

		[Category("Control Hosts")]
		[Description("Raised when a control host is deactivated.")]
		public event ControlHostEvent ControlHostDeactivated;

		[Category("Control Hosts")]
		[Description("Raised when the user creates a new control host.")]
		public event ControlHostEvent ControlHostCreated;

		[Category("Control Hosts")]
		[Description("Lets you cancel control host deletion.")]
		public event ControlHostConfirmation ControlHostDeleting;

		[Category("Control Hosts")]
		[Description("Lets you cancel control host selection.")]
		public event ControlHostConfirmation ControlHostSelecting;

		[Category("Control Hosts")]
		[Description("Lets you cancel control host creation.")]
		public event ControlHostConfirmation ControlHostCreating;

		[Category("Control Hosts")]
		[Description("Raised when a control host is clicked.")]
		public event ControlHostMouseEvent ControlHostClicked;

		[Category("Control Hosts")]
		[Description("Raised when a control host is double-clicked.")]
		public event ControlHostMouseEvent ControlHostDblClicked;

		[Category("Control Hosts")]
		[Description("Raised when the user moves or resizes a control host.")]
		public event ControlHostMouseEvent ControlHostModified;

		[Category("Control Hosts")]
		[Description("Raised while the user moves or resizes a control host.")]
		public event ControlHostConfirmation ControlHostModifying;

		[Category("Control Hosts")]
		[Description("Lets you save additional data for control hosts when serializing the diagram into a file.")]
		public event ControlHostSerializeEvent ControlHostSerializing;

		[Category("Control Hosts")]
		[Description("Lets you load your custom data associated with a control host when deserializing a diagram from a file.")]
		public event ControlHostSerializeEvent ControlHostDeserializing;

		[Category("Custom Drawing")]
		[Description("Raised when a custom-drawn table cell must be painted.")]
		public event CellCustomDraw DrawCell;

		[Category("Custom Drawing")]
		[Description("Raised when a custom-drawn arrow must be painted.")]
		public event ArrowCustomDraw DrawArrow;

		[Category("Custom Drawing")]
		[Description("Raised when a custom-drawn anchor-point mark must be painted.")]
		public event MarkCustomDraw DrawMark;

		[Category("Custom Drawing")]
		[Description("Raised when a control host is painted in the Overview window or printed.")]
		public event ControlHostPaint PaintControlHost;

		[Category("Custom Drawing")]
		[Description("Raised when custom-drawn selection handles must be painted.")]
		public event CustomDrawEventHandler DrawSelHandles;

		[Category("Custom Drawing")]
		[Description("Raised when a custom-drawn box must be painted.")]
		public event BoxCustomDraw DrawBox;

		[Category("Custom Drawing")]
		[Description("Raised when a custom-drawn table must be painted.")]
		public event TableCustomDraw DrawTable;

		[Category("In-place Edit")]
		[Description("Raised when the text of a box is edited in-place by a user.")]
		public event BoxTextEditedEvent BoxTextEdited;

		[Category("In-place Edit")]
		[Description("Lets you prevent users from editing the text of a box.")]
		public event BoxConfirmation BoxInplaceEditing;

		[Category("In-place Edit")]
		[Description("Raised when the in-place edit TextBox is displayed.")]
		public event InplaceEditEvent EnterInplaceEditMode;

		[Category("In-place Edit")]
		[Description("Raised when leaving in-place edit mode.")]
		public event InplaceEditEvent LeaveInplaceEditMode;

		[Category("In-place Edit")]
		[Description("Raised when the caption of a table is edited in-place by a user.")]
		public event TableCaptionEditedEvent TableCaptionEdited;

		[Category("In-place Edit")]
		[Description("Raised when the text of a cell is edited in-place by a user.")]
		public event CellTextEditedEvent CellTextEdited;

		[Category("In-place Edit")]
		[Description("Lets you prevent users from editing the text of a table or a table's cell.")]
		public event TableConfirmation TableInplaceEditing;

		[Category("Miscellaneous")]
		[Description("Lets you implement custom hit-testing of selection handles.")]
		public event HitTestEventHandler HitTestSelHandles;

		[Category("Miscellaneous")]
		[Description("Raised when a tree branch expands after a user clicks the '+' button displayed by an expandable node.")]
		public event NodeEventHandler TreeExpanded;

		[Category("Miscellaneous")]
		[Description("Raised when a tree branch collapses after a user clicks the '-' button displayed by an expandable node.")]
		public event NodeEventHandler TreeCollapsed;

		[Category("Miscellaneous")]
		[Description("Raised when ExpandButtonAction is set to RaiseEvents and a user clicks the the +/- button.")]
		public event NodeEventHandler ExpandButtonClicked;

		[Category("Miscellaneous")]
		[Description("Raised when a user starts modifying an item.")]
		public event ItemEventHandler BeginModify;

		[Category("Miscellaneous")]
		[Description("Raised when a group is destroyed.")]
		public event GroupEvent GroupDestroyed;

		[Category("Mouse")]
		[Description("Raised when a user clicks an unoccupied area inside the diagram.")]
		public event DocMouseEvent DocClicked;

		[Category("Mouse")]
		[Description("Raised when a user double-clicks an unoccupied area inside the diagram.")]
		public event DocMouseEvent DocDblClicked;

		[Category("Property Changed")]
		[Description("Raised when the value of the Dirty flag changes.")]
		public event EventHandler DirtyChanged;

		[Category("Property Changed")]
		[Description("Raised when the scroll position changes.")]
		public event EventHandler ScrollChanged;

		[Category("Property Changed")]
		[Description("Raised when the zoom factor changes.")]
		public event EventHandler ZoomFactorChanged;

		[Category("Property Changed")]
		[Description("Raised when the document size changes.")]
		public event EventHandler DocExtentsChanged;

		[Category("Property Changed")]
		[Description("Raised when a new default shape is set.")]
		public event EventHandler DefaultShapeChanged;

		[Category("Property Changed")]
		[Description("Raised when the value of BoxStyle changes.")]
		public event EventHandler BoxStyleChanged;

		[Category("Property Changed")]
		[Description("Raised when the value of MeasureUnit changes.")]
		public event EventHandler MeasureUnitChanged;
		
		[Category("Selection")]
		[Description("Raised when a user moves a selection of items.")]
		public event SelectionEvent SelectionMoved;

		[Category("Selection")]
		[Description("Raised when an item is added to or removed from the selection.")]
		public event SelectionEvent SelectionChanged;

		[Category("Tables")]
		[Description("Raised when a user starts drawing a table.")]
		public event TableEvent InitializeTable;

		[Category("Tables")]
		[Description("Raised when the user draws a new table.")]
		public event TableEvent TableCreated;

		[Category("Tables")]
		[Description("Raised when a table is deleted.")]
		public event TableEvent TableDeleted;

		[Category("Tables")]
		[Description("Raised when the user moves or resizes a table.")]
		public event TableMouseEvent TableModified;

		[Category("Tables")]
		[Description("Raised while the user moves or resizes a table.")]
		public event TableConfirmation TableModifying;

		[Category("Tables")]
		[Description("Raised when a table is activated.")]
		public event TableEvent TableActivated;

		[Category("Tables")]
		[Description("Raised when a table is deselected.")]
		public event TableEvent TableDeselected;

		[Category("Tables")]
		[Description("Raised when a table is deactivated.")]
		public event TableEvent TableDeactivated;

		[Category("Tables")]
		[Description("Lets you cancel table creation.")]
		public event TableConfirmation TableCreating;

		[Category("Tables")]
		[Description("Lets you cancel table deletion.")]
		public event TableConfirmation TableDeleting;

		[Category("Tables")]
		[Description("Lets you cancel table selection.")]
		public event TableConfirmation TableSelecting;

		[Category("Tables")]
		[Description("Raised when a table is clicked.")]
		public event TableMouseEvent TableClicked;

		[Category("Tables")]
		[Description("Raised when a table is double-clicked.")]
		public event TableMouseEvent TableDblClicked;

		[Category("Tables")]
		[Description("Raised when a table cell is clicked.")]
		public event TableMouseEvent TableCellClicked;

		[Category("Tables")]
		[Description("Raised when a table cell is double-clicked.")]
		public event TableMouseEvent TableCellDblClicked;

		[Category("Tables")]
		[Description("Raised when a user clicks the + button in a header row of a table to expand the section of rows under the header.")]
		public event RowEventHandler TableSectionExpanded;

		[Category("Tables")]
		[Description("Raised when a user clicks the - button in a header row of a table to collapse the section of rows under the header.")]
		public event RowEventHandler TableSectionCollapsed;

		[Category("Undo")]
		[Description("Raised when an action is stored into undo history queue.")]
		public event UndoEventHandler ActionRecorded;

		[Category("Undo")]
		[Description("Raised when an action is undone.")]
		public event UndoEventHandler ActionUndone;

		[Category("Undo")]
		[Description("Raised when an action is redone.")]
		public event UndoEventHandler ActionRedone;

		[Category("Undo")]
		[Description("Raised when an action is about to be saved in undo history.")]
		public event UndoConfirmEventHandler ActionRecording;

		internal void raiseInitEvent(ChartObject item)
		{
			switch (item.getType())
			{
			case ItemType.Box:
				if (InitializeBox != null)
				{
					BoxEventArgs args = new BoxEventArgs(item as Box);
					InitializeBox(this, args);
				}
				break;
			case ItemType.ControlHost:
				if (InitializeControlHost != null)
				{
					ControlHostEventArgs args = new ControlHostEventArgs(item as ControlHost);
					InitializeControlHost(this, args);
				}
				break;
			case ItemType.Table:
				if (InitializeTable != null)
				{
					TableEventArgs args = new TableEventArgs(item as Table);
					InitializeTable(this, args);
				}
				break;
			case ItemType.Arrow:
				if (InitializeArrow != null)
				{
					ArrowEventArgs args = new ArrowEventArgs(item as Arrow);
					InitializeArrow(this, args);
				}
				break;
			}
		}

		internal void drawBox(Graphics g, Box box, bool shadow, RectangleF rc)
		{
			if (DrawBox != null)
				DrawBox(this, new BoxDrawArgs(g, box, shadow, rc));
		}

		internal void drawTable(Graphics g, Table table, bool shadow, RectangleF bounds)
		{
			if (DrawTable != null)
				DrawTable(this, new TableDrawArgs(g, table, shadow, bounds));
		}

		internal void drawCell(Graphics g, Table table, int col, int row, RectangleF bounds)
		{
			if (DrawCell != null)
				DrawCell(this, new CellDrawArgs(g, table, col, row, bounds));
		}

		internal void drawArrow(Graphics g, Arrow arrow, bool shadow, PointCollection points)
		{
			if (DrawArrow != null)
				DrawArrow(this, new ArrowDrawArgs(g, arrow, shadow, points));
		}

		internal void drawMark(MarkDrawArgs args)
		{
			if (DrawMark != null)
				DrawMark(this, args);
		}

		internal bool paintControlHost(Graphics g, ControlHost host, RectangleF rect)
		{
			if (PaintControlHost != null)
			{
				ControlPaintArgs args = new ControlPaintArgs(g, host, rect);
				PaintControlHost(this, args);
				return args.Handled;
			}

			return false;
		}

		internal void fireDrawSelHandles(Graphics graphics, ChartObject item)
		{
			if (DrawSelHandles != null)
				DrawSelHandles(this, new DrawEventArgs(graphics, item));
		}

		internal bool hitTestSelHandles(ChartObject item,
			PointF mousePosition, ref int hitResult)
		{
			if (HitTestSelHandles != null)
			{
				HitTestEventArgs args = new HitTestEventArgs(item, mousePosition);
				HitTestSelHandles(this, args);
				hitResult = args.HitResult;
				return hitResult != -1;
			}

			return false;
		}

		internal void fireActionRecorded(Command cmd)
		{
			if (ActionRecorded != null)
				ActionRecorded(this, new UndoEventArgs(cmd));
		}

		internal bool confirmRecordAction(Command cmd)
		{
			if (ActionRecording != null)
			{
				UndoConfirmEventArgs args = new UndoConfirmEventArgs(cmd);
				ActionRecording(this, args);
				return args.Confirm;
			}

			return true;
		}

		internal void fireActionUndone(Command cmd)
		{
			if (ActionUndone != null)
				ActionUndone(this, new UndoEventArgs(cmd));
		}

		internal void fireActionRedone(Command cmd)
		{
			if (ActionRedone != null)
				ActionRedone(this, new UndoEventArgs(cmd));
		}

		internal bool confirmDelete(ChartObject obj)
		{
			bool res = true;

			switch (obj.getType())
			{
				case ItemType.Box:
					if (BoxDeleting != null)
					{
						BoxConfirmArgs args = new BoxConfirmArgs((Box)obj);
						BoxDeleting(this, args);
						res = args.Confirm;
					}
					break;
				case ItemType.ControlHost:
					if (ControlHostDeleting != null)
					{
						ControlHostConfirmArgs args =
							new ControlHostConfirmArgs((ControlHost)obj);
						ControlHostDeleting(this, args);
						res = args.Confirm;
					}
					break;
				case ItemType.Table:
					if (TableDeleting != null)
					{
						TableConfirmArgs args = new TableConfirmArgs((Table)obj);
						TableDeleting(this, args);
						res = args.Confirm;
					}
					break;
				case ItemType.Arrow:
					if (ArrowDeleting != null)
					{
						ArrowConfirmArgs args = new ArrowConfirmArgs((Arrow)obj);
						ArrowDeleting(this, args);
						res = args.Confirm;
					}
					break;
			}

			return res;
		}

		internal bool confirmSelect(ChartObject obj)
		{
			bool res = true;

			switch (obj.getType())
			{
				case ItemType.Box:
					if (BoxSelecting != null)
					{
						BoxConfirmArgs args = new BoxConfirmArgs((Box)obj);
						BoxSelecting(this, args);
						res = args.Confirm;
					}
					break;
				case ItemType.ControlHost:
					if (ControlHostSelecting != null)
					{
						ControlHostConfirmArgs args = new ControlHostConfirmArgs((ControlHost)obj);
						ControlHostSelecting(this, args);
						res = args.Confirm;
					}
					break;
				case ItemType.Table:
					if (TableSelecting != null)
					{
						TableConfirmArgs args = new TableConfirmArgs((Table)obj);
						TableSelecting(this, args);
						res = args.Confirm;
					}
					break;
				case ItemType.Arrow:
					if (ArrowSelecting != null)
					{
						ArrowConfirmArgs args = new ArrowConfirmArgs((Arrow)obj);
						ArrowSelecting(this, args);
						res = args.Confirm;
					}
					break;
			}

			return res;
		}

		internal bool confirmCreate(ChartObject item)
		{
			PointF point = AlignPointToGrid(interaction.CurrentPoint);
			bool validated = true;

			switch (item.getType())
			{
				case ItemType.Box:
					if (BoxCreating != null)
					{
						BoxConfirmArgs args = new BoxConfirmArgs((Box)item, point, -1);
						BoxCreating(this, args);
						validated = args.Confirm;
					}
					break;
				case ItemType.ControlHost:
					if (ControlHostCreating != null)
					{
						ControlHostConfirmArgs args =
							new ControlHostConfirmArgs((ControlHost)item, point, -1);
						ControlHostCreating(this, args);
						validated = args.Confirm;
					}
					break;
				case ItemType.Table:
					if (TableCreating != null)
					{
						TableConfirmArgs args = new TableConfirmArgs((Table)item, point, -1);
						TableCreating(this, args);
						validated = args.Confirm;
					}
					break;
				case ItemType.Arrow:
					if (ArrowCreating != null)
					{
						Arrow arrow = (Arrow)item;

						PointF endPt = arrow.Points[arrow.Points.Count - 1];
						int id = 0;
						int row = -1;
						arrow.NewDest.getAnchor(endPt, arrow, true, ref id);
						if (arrow.NewDest is Table)
							row = ((Table)arrow.NewDest).rowFromPt(endPt);

						AttachConfirmArgs args = new AttachConfirmArgs(
							arrow, arrow.NewDest, false, id, row);
						ArrowCreating(this, args);

						validated = args.Confirm;
					}
					break;
			}

			return validated;
		}

		internal void fireObjModified(ChartObject obj, PointF pt, int mnpHandle)
		{
			switch (obj.getType())
			{
				case ItemType.Box:
					if (BoxModified != null)
					{
						Box box = (Box)obj;
						BoxMouseArgs args = new BoxMouseArgs(
							box, pt.X, pt.Y, mnpHandle);
						BoxModified(this, args);
					}
					break;
				case ItemType.ControlHost:
					if (ControlHostModified != null)
					{
						ControlHost host = (ControlHost)obj;
						ControlHostMouseArgs args = new ControlHostMouseArgs(
							host, pt.X, pt.Y, mnpHandle);
						ControlHostModified(this, args);
					}
					break;
				case ItemType.Table:
					if (TableModified != null)
					{
						Table table = (Table)obj;
						TableMouseArgs args = new TableMouseArgs(
							table, pt.X, pt.Y, mnpHandle);
						TableModified(this, args);
					}
					break;
				case ItemType.Arrow:
					if (ArrowModified != null)
					{
						Arrow arrow = (Arrow)obj;
						ArrowMouseArgs args = new ArrowMouseArgs(
							arrow, pt.X, pt.Y, mnpHandle);
						ArrowModified(this, args);
					}
					break;
			}
		}

		internal bool confirmModify(ChartObject item)
		{
			PointF point = AlignPointToGrid(interaction.CurrentPoint);
			int selHandle = interaction.SelectionHandle;
			bool validated = true;

			switch (item.getType())
			{
				case ItemType.Box:
					if (BoxModifying != null)
					{
						Box box = (Box)item;
						BoxConfirmArgs args = new BoxConfirmArgs(box, point, selHandle);
						BoxModifying(this, args);
						validated = args.Confirm;
					}
					break;
				case ItemType.ControlHost:
					if (ControlHostModifying != null)
					{
						ControlHost host = (ControlHost)item;
						ControlHostConfirmArgs args = new ControlHostConfirmArgs(host, point, selHandle);
						ControlHostModifying(this, args);
						validated = args.Confirm;
					}
					break;
				case ItemType.Table:
					if (TableModifying != null)
					{
						Table table = (Table)item;
						TableConfirmArgs args = new TableConfirmArgs(table, point, selHandle);
						TableModifying(this, args);
						validated = args.Confirm;
					}
					break;
				case ItemType.Arrow:
					if (ArrowModifying != null)
					{
						Arrow arrow = (Arrow)item;
						ArrowConfirmArgs args = new ArrowConfirmArgs(arrow, point, selHandle);
						ArrowModifying(this, args);
						validated = args.Confirm;
					}
					break;
			}

			return validated;
		}

		internal void fireBeginModifyEvent(ChartObject item, PointF pt, int selHandle)
		{
			if (BeginModify != null)
			{
				ItemEventArgs args = new ItemEventArgs(item);
				BeginModify(this, args);
			}
		}

		internal void fireItemPasted(ChartObject item)
		{
			switch (item.getType())
			{
				case ItemType.Box:
					if (BoxPasted != null)
					{
						Box box = item as Box;
						BoxEventArgs args = new BoxEventArgs(box);
						BoxPasted(this, args);
					}
					break;
				case ItemType.ControlHost:
					if (ControlHostPasted != null)
					{
						ControlHost host = item as ControlHost;
						ControlHostEventArgs args = new ControlHostEventArgs(host);
						ControlHostPasted(this, args);
					}
					break;
				case ItemType.Table:
					if (TablePasted != null)
					{
						Table table = item as Table;
						TableEventArgs args = new TableEventArgs(table);
						TablePasted(this, args);
					}
					break;
				case ItemType.Arrow:
					if (ArrowPasted != null)
					{
						Arrow arrow = item as Arrow;
						ArrowEventArgs args = new ArrowEventArgs(arrow);
						ArrowPasted(this, args);
					}
					break;
			}
		}

		internal void fireBoxDeleted(Box box)
		{
			if (BoxDeleted != null)
				BoxDeleted(this, new BoxEventArgs(box));
		}

		internal void fireControlHostDeleted(ControlHost host)
		{
			if (ControlHostDeleted != null)
				ControlHostDeleted(this, new ControlHostEventArgs(host));
		}

		internal void fireControlHostSerializing(ControlHost host, BinaryWriter writer)
		{
			// Consider user
			if (ControlHostSerializing != null)
			{
				MemoryStream stream = new MemoryStream(0);
				ControlHostSerializing(this,
					new ControlHostSerializeArgs(host, stream));

				// Append the stream
				byte[] b = stream.GetBuffer();
				writer.Write(b.Length);
				writer.Write(b);
			}
			else
			{
				// No user storage
				writer.Write((int)0);
			}
		}

		internal void fireControlHostDeserializing(ControlHost host, BinaryReader reader)
		{
			int len = reader.ReadInt32();
			if (len > 0)
			{
				byte[] buf = reader.ReadBytes(len);
				MemoryStream stream = new MemoryStream(buf, false);
				if (ControlHostDeserializing != null)
					ControlHostDeserializing(this,
						new ControlHostSerializeArgs(host, stream));
			}
		}

		internal void fireArrowDeleted(Arrow arrow)
		{
			if (ArrowDeleted != null)
				ArrowDeleted(this, new ArrowEventArgs(arrow));
		}

		internal void fireTableDeleted(Table table)
		{
			if (TableDeleted != null)
				TableDeleted(this, new TableEventArgs(table));
		}

		internal void fireActivationEvent()
		{
			switch (activeObject.getType())
			{
				case ItemType.Box:
					if (BoxActivated != null)
						BoxActivated(this, new BoxEventArgs((Box)activeObject));
					break;
				case ItemType.ControlHost:
					if (ControlHostActivated != null)
						ControlHostActivated(this, new ControlHostEventArgs((ControlHost)activeObject));
					break;
				case ItemType.Table:
					if (TableActivated != null)
						TableActivated(this, new TableEventArgs((Table)activeObject));
					break;
				case ItemType.Arrow:
					if (ArrowActivated != null)
						ArrowActivated(this, new ArrowEventArgs((Arrow)activeObject));
					break;
			}
		}

		internal void objectDeselected(ChartObject obj)
		{
			if (!userAction) return;
			switch (obj.getType())
			{
				case ItemType.Box:
					if (BoxDeselected != null)
						BoxDeselected(this, new BoxEventArgs((Box)obj));
					break;
				case ItemType.ControlHost:
					if (ControlHostDeselected != null)
						ControlHostDeselected(this, new ControlHostEventArgs((ControlHost)obj));
					break;
				case ItemType.Table:
					if (TableDeselected != null)
						TableDeselected(this, new TableEventArgs((Table)obj));
					break;
				case ItemType.Arrow:
					if (ArrowDeselected != null)
						ArrowDeselected(this, new ArrowEventArgs((Arrow)obj));
					break;
			}
		}

		internal void fireDeactivationEvent()
		{
			switch (activeObject.getType())
			{
				case ItemType.Box:
					if (BoxDeactivated != null)
						BoxDeactivated(this, new BoxEventArgs((Box)activeObject));
					break;
				case ItemType.ControlHost:
					if (ControlHostDeactivated != null)
						ControlHostDeactivated(this, new ControlHostEventArgs((ControlHost)activeObject));
					break;
				case ItemType.Table:
					if (TableDeactivated != null)
						TableDeactivated(this, new TableEventArgs((Table)activeObject));
					break;
				case ItemType.Arrow:
					if (ArrowDeactivated != null)
						ArrowDeactivated(this, new ArrowEventArgs((Arrow)activeObject));
					break;
			}
		}

		internal void fireGroupDestroyed(Group group)
		{
			if (GroupDestroyed != null)
				GroupDestroyed(this, new GroupEventArgs(group));
		}

		internal void fireObjCreated(ChartObject obj)
		{
			if (obj.getType() == ItemType.Box && BoxCreated != null)
				BoxCreated(this, new BoxEventArgs((Box)obj));

			if (obj.getType() == ItemType.ControlHost && ControlHostCreated != null)
				ControlHostCreated(this, new ControlHostEventArgs((ControlHost)obj));

			if (obj.getType() == ItemType.Table && TableCreated != null)
				TableCreated(this, new TableEventArgs((Table)obj));

			if (obj.getType() == ItemType.Arrow && ArrowCreated != null)
				ArrowCreated(this, new ArrowEventArgs((Arrow)obj));
		}

		internal void raiseExpandBtnClicked(Node node)
		{
			if (ExpandButtonClicked != null)
				ExpandButtonClicked(this, new NodeEventArgs(node));
		}

		internal void fireTreeExpanded(Node root)
		{
			if (TreeExpanded != null)
				TreeExpanded(this, new NodeEventArgs(root));
		}

		internal void fireTreeCollapsed(Node root)
		{
			if (TreeCollapsed != null)
				TreeCollapsed(this, new NodeEventArgs(root));
		}

		internal void fireSelectionChanged()
		{
			if (SelectionChanged != null)
				SelectionChanged(this, new EventArgs());
		}

		internal void fireSelectionMoved()
		{
			if (SelectionMoved != null)
				SelectionMoved(this, new EventArgs());

			try
			{
				if (BoxModified != null)
				{
					foreach (Box box in Selection.Boxes)
						fireObjModified(box, new PointF(0, 0), 8);
				}
				if (ControlHostModified != null)
				{
					foreach (ControlHost host in Selection.ControlHosts)
						fireObjModified(host, new PointF(0, 0), 8);
				}
				if (TableModified != null)
				{
					foreach (Table table in Selection.Tables)
						fireObjModified(table, new PointF(0, 0), 8);
				}
				if (ArrowModified != null)
				{
					foreach (Arrow arrow in Selection.Arrows)
						fireObjModified(arrow, new PointF(0, 0), -1);
				}
			}
			catch(InvalidOperationException)
			{
				// could happen if someone deletes a selected item from within
				// a xModified event handler; just ignore that, what else can we do ?
			}
		}

		internal void fireArrowRoutedEvent(Arrow arrow)
		{
			if (ArrowRouted != null)
				ArrowRouted(this, new ArrowEventArgs(arrow));
		}

		internal void fireClickedEvent(ChartObject obj, PointF pt, MouseButtons mb)
		{
			RectangleF rc = obj.getBoundingRect();
			float dx = pt.X - rc.X;
			float dy = pt.Y - rc.Y;
			int row = 0, col = 0;
			bool cellClicked;

			switch (obj.getType())
			{
				case ItemType.Box:
					if (BoxClicked != null)
						BoxClicked(this, new BoxMouseArgs((Box)obj, mb, dx, dy, pt));
					break;
				case ItemType.ControlHost:
					if (ControlHostClicked != null)
						ControlHostClicked(this, new ControlHostMouseArgs((ControlHost)obj, mb, dx, dy, pt));
					break;
				case ItemType.Table:
					// raise TableCellClicked if a cell has been clicked and 
					// an event handler is registered for the event
					cellClicked = ((Table)obj).cellFromPt(pt, ref row, ref col);
					if (cellClicked && TableCellClicked != null)
					{
						TableCellClicked(this, new TableMouseArgs(
							(Table)obj, mb, dx, dy, col, row));
					}
						// otherwise raise TableClicked
					else
					{
						TableMouseArgs args = new TableMouseArgs((Table)obj, mb, dx, dy, pt);
						if (cellClicked) { args.col = col; args.row = row; }
						if (TableClicked != null) TableClicked(this, args);
					}
					break;
				case ItemType.Arrow:
					if (ArrowClicked != null)
						ArrowClicked(this, new ArrowMouseArgs((Arrow)obj, mb, dx, dy, pt));
					break;
			}
		}

		private void fireDblClickedEvent(ChartObject obj, PointF pt, MouseButtons mb)
		{
			RectangleF rc = obj.getBoundingRect();
			float dx = pt.X - rc.X;
			float dy = pt.Y - rc.Y;
			int row = 0, col = 0;

			switch (obj.getType())
			{
				case ItemType.Box:
					Box box = (Box)obj;
					if (inplaceEditAllowed && Enabled && !box.notInteractive() &&
						confirmBoxInplaceEdit(box))
						startInplaceEdit(new NodeInplaceEditable(box), box.getEditRect());
					if (BoxDblClicked != null)
						BoxDblClicked(this, new BoxMouseArgs(box, mb, dx, dy, pt));
					break;
				case ItemType.ControlHost:
					ControlHost host = (ControlHost)obj;
					if (ControlHostDblClicked != null)
						ControlHostDblClicked(this, new ControlHostMouseArgs(host, mb, dx, dy, pt));
					break;
				case ItemType.Table:
					Table table = (Table)obj;
					if (table.cellFromPt(pt, ref row, ref col))
					{
						if (inplaceEditAllowed && Enabled && !table.notInteractive() &&
							confirmTableInplaceEdit(table, row, col))
						{
							startInplaceEdit(table[col, row], table[col, row].getEditRect());
							lastClickedCol = col;
							lastClickedRow = row;
						}
						if (TableCellDblClicked != null)
						{
							TableCellDblClicked(this, new TableMouseArgs(
								table, mb, dx, dy, col, row));
						}
					}
					else
					{
						if (inplaceEditAllowed && Enabled && !table.notInteractive() &&
							confirmTableInplaceEdit(table, -1, -1))
							startInplaceEdit(new NodeInplaceEditable(table), table.getEditRect());
						if (TableDblClicked != null)
							TableDblClicked(this, new TableMouseArgs(table, mb, dx, dy, pt));
					}
					break;
				case ItemType.Arrow:
					if (ArrowDblClicked != null)
						ArrowDblClicked(this, new ArrowMouseArgs((Arrow)obj, mb, dx, dy, pt));
					break;
			}
		}

		internal void fireTableSectionEvent(Table table, int rowIndex, bool expanded)
		{
			if (expanded && TableSectionExpanded != null)
			{
				RowEventArgs args = new RowEventArgs(table, rowIndex);
				TableSectionExpanded(this, args);
			}

			if (!expanded && TableSectionCollapsed != null)
			{
				RowEventArgs args = new RowEventArgs(table, rowIndex);
				TableSectionCollapsed(this, args);
			}
		}

		internal bool requestAttach(Arrow arrow, bool changingOrg, Node node)
		{
			if (ArrowAttaching != null)
			{
				PointF endPt = changingOrg ? arrow.Points[0] : arrow.Points[arrow.Points.Count - 1];
				int id = 0;
				int row = -1;
				node.getAnchor(endPt, arrow, !changingOrg, ref id);

				// get the row if attaching to table
				Table table = node as Table;
				if (table != null)
					row = table.rowFromPt(endPt);

				AttachConfirmArgs args = new AttachConfirmArgs(arrow,
					node, changingOrg, id, row);
				ArrowAttaching(this, args);

				return args.Confirm;
			}

			return true;
		}

		internal bool validateAnchor(Arrow arrow, bool outgoing, Node node, int pointIndex)
		{
			if (ValidateAnchorPoint != null)
			{
				AttachConfirmArgs args = new AttachConfirmArgs(arrow,
					node, outgoing, pointIndex, 0);
				ValidateAnchorPoint(this, args);

				return args.Confirm;
			}

			return true;
		}

		#endregion

		#region miscellaneous

		/// <summary>
		/// Checks whether this is a trial version of the control
		/// </summary>
		public bool IsTrialVersion
		{
			get
			{
#if DEMO_VERSION
				return true;
#else
				return false;
#endif
			}
		}

		/// <summary>
		/// Checks whether this is a Pro edition of the control
		/// </summary>
		public bool IsProEdition
		{
			get
			{
#if FCNET_STD
				return false;
#else
				return true;
#endif
			}
		}

		internal bool DisabledGroups
		{
			get { return disabledGroups; }
			set { disabledGroups = value; }
		}

		private bool disabledGroups = false;

		internal DummyNode dummy;

    [Browsable(false)]
    public DummyNode Dummy
    {
      get { return dummy; }
    }

		[Browsable(false)]
		public ScriptHelper ScriptHelper
		{
			get { return scriptHelper; }
		}

		private ScriptHelper scriptHelper;

		private static object syncRoot = new Object();

		private License license = null;

		[Browsable(false)]
#if FCNET_STD
		internal UndoManager UndoManager
#else
		public UndoManager UndoManager
#endif
		{
			get { return undoManager; }
		}

		private UndoManager undoManager;

		public void ExecuteCommand(Command cmd)
		{
			undoManager.executeCommand(cmd);
		}

#if VERTIGO
		private bool isVertigoAssembly(Assembly assembly)
		{
			if (assembly.FullName.IndexOf("CodePlex.Vertigo.Controls") == -1)
				return false;

			byte[] vertigoKeyToken = new byte[] {
				0x79, 0xfe, 0x4b, 0x0c, 0xb1, 0x27, 0x44, 0xb2 };
			byte[] assemblyKeyToken = assembly.GetName().GetPublicKeyToken();

			if (BitConverter.ToString(vertigoKeyToken) != 
				BitConverter.ToString(assemblyKeyToken))
				return false;

			return true;
		}

		private bool loadedFromVertigoAssembly()
		{
			Assembly inheritingAssembly = this.GetType().Assembly;
			if (isVertigoAssembly(inheritingAssembly))
				return true;

			if (Parent != null)
			{
				Assembly containingAssembly = Parent.GetType().Assembly;
				if (isVertigoAssembly(containingAssembly))
					return true;
			}

			return false;
		}
#endif
		#endregion

		#region fields

		private ChartObjectCollection zOrder;
		private BoxCollection boxes;
		private TableCollection tables;
		private ControlHostCollection controlHosts;
		private ArrowCollection arrows;
		private GroupCollection groups;

		private Selection selection;

		#endregion
	}
}