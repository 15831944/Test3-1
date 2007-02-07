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
using System.Drawing;
using System.Drawing.Drawing2D;
using System.IO;
using System.Windows.Forms;
using MindFusion.FlowChartX.Visitors;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Summary description for ChartObject.
	/// </summary>
	public abstract class ChartObject : IPersists
	{
		/// <summary>
		/// ChartObject constructor
		/// </summary>
		public ChartObject(FlowChart parent)
		{
			flowChart = parent;
			rect = new RectangleF(0, 0, 20, 20);
			tag = null;
			constructed = false;
			modifying = false;
			locked = false;
			subordinateGroup = null;
			masterGroup = null;
			cycleProtect = false;
			cycleDetected = false;
			zIndex = 0;

			font = (Font)parent.Font.Clone();

			penDashStyle = parent.PenDashStyle;
			penWidth = parent.PenWidth;

			shadowOffsetX = flowChart.ShadowOffsetX;
			shadowOffsetY = flowChart.ShadowOffsetY;
			shadowColor = parent.ShadowColor;

			invisible = false;
			selected = false;
			printable = true;

			manipulators = new ArrayList();
			toolTip = "";

			ignoreLayout = false;

			runtimeData = new SortedList();

			weight = 1;
			hyperLink = "";
		}

		public ChartObject(ChartObject prototype)
		{
			flowChart = prototype.flowChart;

			rect = prototype.rect;
			rcSaved = rect;

			tag = prototype.tag;

			constructed = false;
			modifying = false;
			modifyHandle = 0;
			modifyDX = modifyDY = 0;

			locked = prototype.locked;
			subordinateGroup = null;
			masterGroup = null;
			cycleProtect = false;
			cycleDetected = false;
			zIndex = 0;

			font = (Font)prototype.font.Clone();

			penDashStyle = prototype.PenDashStyle;
			penWidth = prototype.PenWidth;

			shadowOffsetX = prototype.shadowOffsetX;
			shadowOffsetY = prototype.shadowOffsetY;
			shadowColor = prototype.shadowColor;

			invisible = prototype.invisible;
			selected = false;
			printable = prototype.printable;

			manipulators = new ArrayList();
			toolTip = prototype.toolTip;

			ptOrg = prototype.ptOrg;
			ptEnd = prototype.ptEnd;
			ptSavedOrg = prototype.ptSavedOrg;
			ptSavedEnd = prototype.ptSavedEnd;
			ptMdfLast = ptEnd;

			frameColor = prototype.frameColor;
			fillColor = prototype.fillColor;

			pen = (Pen)prototype.pen.Clone();
			brush = prototype.brush;
			brush.AddRef();

			subordinateGroup = masterGroup = null;

			ignoreLayout = prototype.ignoreLayout;

			runtimeData = new SortedList();

			weight = prototype.weight;
			hyperLink = prototype.hyperLink;
		}

		internal void setParent(FlowChart fc)
		{
			flowChart = fc;
		}

		// called when an item is added to the flowchart
		// either by users, programmatically or via undo / redo
		internal virtual void onAdd()
		{
		}

		/// <summary>
		/// Called after successful load.
		/// </summary>
		internal virtual void onLoad()
		{
		}

		// called when an item is removed from the flowchart
		// either by users, programmatically or via undo / redo
		internal virtual void onRemove()
		{
			// remove from all groups
			if (subordinateGroup != null) subordinateGroup.notifyObjDeleted(this);
			if (masterGroup != null) masterGroup.notifyObjDeleted(this);
			subordinateGroup = null;
			masterGroup = null;
		}

		// called when an item is completely removed, i.e. when it
		// is no longer in the chart or in the undo/redo history
		internal virtual void freeResources()
		{
			removeAllManipulators();

			if (brush != null)
				brush.Release();
			brush = null;

			runtimeData.Clear();
		}

		internal void setSelected(bool newVal)
		{
			if (selected != newVal)
			{
				selected = newVal;
				if (!selected)
					flowChart.objectDeselected(this);
			}
		}

		internal bool getSelected() { return selected; }

		private bool selected;


		// ************ creation ************

		internal virtual void startCreate(PointF org)
		{
			ptOrg = ptEnd = org;
			rectFromPoints();
		}

		internal virtual void updateCreate(PointF current)
		{
			ptEnd = current;
			rectFromPoints();
		}

		internal virtual void completeCreate(PointF end)
		{
			updateCreate(end);
			constructed = true;
		}

		internal void completeCreate()
		{
			constructed = true;
		}

		internal abstract bool allowCreate(PointF current);


		// ************ modification ************

		internal virtual void startModify(PointF org, int handle, InteractionState ist)
		{
			flowChart.UndoManager.onModifyItem(this);

			modifyHandle = handle;
			ptMdfLast = org;
			modifyDX = modifyDY = 0;

			ptSavedOrg = ptOrg;
			ptSavedEnd = ptEnd;
			rcSaved = rect;

			modifying = true;
		}

		internal virtual void updateModify(PointF current, InteractionState ist)
		{
			modifyDX = current.X - ptMdfLast.X;
			modifyDY = current.Y - ptMdfLast.Y;
			ptMdfLast = current;
		}

		internal virtual void modifyTranslate(float x, float y, bool arrows)
		{
			modifyDX = x; modifyDY = y;
		}

		internal virtual void completeModify(PointF end, InteractionState ist)
		{
			modifyDX = end.X - ptMdfLast.X;
			modifyDY = end.Y - ptMdfLast.Y;
			ptMdfLast = end;

			modifying = false;
		}

		internal virtual void completeModify()
		{
			modifyDX = 0;
			modifyDY = 0;
			modifying = false;
		}

		internal abstract bool allowModify(PointF current);

		internal virtual bool canModify(int handle) { return true; }

		internal virtual void cancelModify(InteractionState ist)
		{
			rect = rcSaved;
			ptOrg = ptSavedOrg;
			ptEnd = ptSavedEnd;

			modifyDX = modifyDY = 0;
			modifying = false;
		}

		internal float getModX()
		{
			return modifyDX;
		}

		internal float getModY()
		{
			return modifyDY;
		}

		internal bool getModifying()
		{
			return modifying;
		}


		// ************ drawing ************

		public abstract void Draw(Graphics g, bool shadow);

		internal abstract void drawSelHandles(Graphics g, Color color);

		internal void drawMark(MarkDrawArgs args)
		{
			flowChart.drawMark(args);
		}

		// ************ placement ************

		internal virtual RectangleF getBoundingRect()
		{
			return Utilities.normalizeRect(rect);
		}

		internal virtual RectangleF getRotatedBounds()
		{
			return Utilities.normalizeRect(rect);
		}

		internal virtual RectangleF getRepaintRect(bool includeConnected)
		{
			RectangleF rc = getRotatedBounds();

			// accommodate for pen width
			rc.Inflate(pen.Width, pen.Width);

			// and selection markers
			rc.Inflate(flowChart.SelHandleSize, flowChart.SelHandleSize);

			// add the shadow offset
			float sx = Math.Abs(shadowOffsetX);
			float sy = Math.Abs(shadowOffsetY);
			float addx = 0, addy = 0;
			if (sx > flowChart.SelHandleSize)
			{
				addx = shadowOffsetX >= 0 ?
					shadowOffsetX - flowChart.SelHandleSize :
					shadowOffsetX + flowChart.SelHandleSize;
			}
			if (sy > flowChart.SelHandleSize)
			{
				addy = shadowOffsetY >= 0 ?
					shadowOffsetY - flowChart.SelHandleSize :
					shadowOffsetY + flowChart.SelHandleSize;
			}
			Utilities.addToRect(ref rc, addx, addy);

			return rc;
		}

		internal abstract bool containsPoint(PointF pt);

		public abstract bool HitTestHandle(PointF pt, ref int handle);

		internal virtual int getHandleIdx(PointF pt)
		{
			return -1;
		}

		internal virtual Region getRegion()
		{
			return null;
		}


		// ************ mouse pointers ************

		internal abstract Cursor getCannotDropCursor();

		internal abstract Cursor getCanDropCursor();


		// ************ group of objects attached to this one ************

		internal bool setGroup(Group group)
		{
			if (subordinateGroup == group) return true;
			if (group != null && subordinateGroup != null) return false;

			subordinateGroup = group;
			return true;
		}

		internal bool hasAttachedGroup()
		{
			return subordinateGroup != null;
		}

		internal Group getAttachedGroup()
		{
			return subordinateGroup;
		}


		// ************* group to which this object is attached ************

		internal void putInGroup(Group group)
		{
			masterGroup = group;
		}

		internal bool isAttached()
		{
			return masterGroup != null;
		}

		internal Group getContainingGroup()
		{
			return masterGroup;
		}

		internal ChartObject getGroupMaster()
		{
			if (cycleDetected)
				return this;

			if (masterGroup != null)
				return masterGroup.MainObject.getGroupMaster();
			else
				return this;
		}

		internal ChartObject getGroupMasterCC()
		{
			if (cycleProtect)
			{
				cycleProtect = false;
				return this;
			}
			cycleProtect = true;

			ChartObject theMaster = masterGroup != null ?
				masterGroup.MainObject.getGroupMasterCC() : this;

			cycleProtect = false;
			return theMaster;
		}

		#region interface: appearance

		/// <summary>
		/// The font used to render the object text
		/// </summary>
		public virtual System.Drawing.Font Font
		{
			get
			{
				return font;
			}
			set
			{
				if (font == null || !font.Equals(value))
				{
					font = (System.Drawing.Font)value.Clone();
					flowChart.setDirty();
					flowChart.invalidate(getRepaintRect(false));
				}
			}
		}

		#endregion


		#region data: appearance

		private System.Drawing.Font font;

		#endregion

		/// <summary>
		/// Parent FlowChart
		/// </summary>
		[Browsable(false)]
		public FlowChart Parent
		{
			get
			{
				return flowChart;
			}
		}

		/// <summary>
		/// User-defined data
		/// </summary>
		[Browsable(false)]
		public object Tag
		{
			get
			{
				return tag;
			}
			set
			{
				if (tag == null || !tag.Equals(value))
				{
					tag = value;
					flowChart.setDirty();
				}
			}
		}

		private object tag;

		/// <summary>
		/// The style of the pen for drawing the object frame
		/// </summary>
		public virtual DashStyle PenDashStyle
		{
			get
			{
				if (pen != null)
					return pen.DashStyle;
				return penDashStyle;
			}
			set
			{
				if (!penDashStyle.Equals(value))
				{
					penDashStyle = value;
					pen.DashStyle = PenDashStyle;

					flowChart.setDirty();
					flowChart.invalidate(getRepaintRect(false));
				}
			}
		}

		private DashStyle penDashStyle;

		/// <summary>
		/// The width of the pen for drawing the object
		/// </summary>
		public virtual float PenWidth
		{
			get
			{
				return penWidth;
			}
			set
			{
				if (penWidth != value && value >= 0)
				{
					penWidth = value;

					// Adjust the width of the pen;
					pen.Width = penWidth;

					flowChart.setDirty();
					flowChart.invalidate(getRepaintRect(false));
				}
			}
		}

		private float penWidth;

		/// <summary>
		/// Color of the table interior.
		/// </summary>
		public Color FillColor
		{
			get
			{
				return fillColor;
			}
			set
			{
				if (!fillColor.Equals(value))
				{
					fillColor = value;

					// Reset the brush
					Brush = new SolidBrush(fillColor);

					flowChart.setDirty();
          if (!flowChart.LayoutSuspended)
            flowChart.invalidate(getRepaintRect(false));
				}
			}
		}

		/// <summary>
		/// Color of the table borders.
		/// </summary>
		public virtual Color FrameColor
		{
			get
			{
				return frameColor;
			}
			set
			{
				if (!frameColor.Equals(value))
				{
					frameColor = value;	

					// Reset the pen
					pen.Color = frameColor;

					flowChart.setDirty();
          if (!flowChart.LayoutSuspended)
            flowChart.invalidate(getRepaintRect(false));
				}
			}
		}

		protected internal Color fillColor;
		protected internal Color frameColor;

		protected internal MindFusion.FlowChartX.Pen pen;
		protected internal MindFusion.FlowChartX.Brush brush;

		/// <summary>
		/// The pen used to draw the object.
		/// </summary>
		[Editor(typeof(MindFusion.FlowChartX.Design.PenEditor),
			 typeof(System.Drawing.Design.UITypeEditor))]
		public MindFusion.FlowChartX.Pen Pen
		{
			get
			{
				return pen;
			}
			set
			{
				pen = value;
				frameColor = value.Color;

				flowChart.setDirty();
				flowChart.invalidate(getRepaintRect(false));
			}
		}

		/// <summary>
		/// The brush used to fill the object's interior.
		/// </summary>
		[Editor(typeof(MindFusion.FlowChartX.Design.BrushEditor),
			 typeof(System.Drawing.Design.UITypeEditor))]
		public MindFusion.FlowChartX.Brush Brush
		{
			get
			{
				return brush;
			}
			set
			{
				if(value == null)
					throw new Exception("null is not acceptable Brush value");

				if(brush != null)
					brush.Release();
				brush = value;
				brush.AddRef();

				if (value is MindFusion.FlowChartX.SolidBrush)
					fillColor = ((MindFusion.FlowChartX.SolidBrush)value).Color;

				flowChart.setDirty();
        if (!flowChart.LayoutSuspended)
          flowChart.invalidate(getRepaintRect(false));
			}
		}

		/// <summary>
		/// Prevents the user from modifying the object
		/// </summary>
		public bool Locked
		{
			get
			{
				return locked;
			}
			set
			{
				if (locked != value)
				{
					locked = value;
					if (locked)
						flowChart.Selection.RemoveObject(this);
					flowChart.setDirty();
				}
			}
		}

		private bool locked;

		/// <summary>
		/// True if the object is selected
		/// </summary>
		public bool Selected
		{
			get
			{
				return selected;
			}
			set
			{
				if (value)
					flowChart.Selection.AddObject(this);
				else
					flowChart.Selection.RemoveObject(this);
			}
		}


		public abstract ItemType getType();


		/// <summary>
		/// Moves the object up in the Z order
		/// </summary>
		public void ZLevelUp()
		{
			flowChart.zLevelUp(this);
			flowChart.invalidate(getRepaintRect(false));
		}

		/// <summary>
		/// Moves the object down in the Z order
		/// </summary>
		public void ZLevelDown()
		{
			flowChart.zLevelDown(this);
			flowChart.invalidate(getRepaintRect(false));
		}

		/// <summary>
		/// Moves the object at the bottom of the Z order
		/// </summary>
		public void ZBottom()
		{
			flowChart.zBottom(this);
			flowChart.invalidate(getRepaintRect(false));
		}

		/// <summary>
		/// Moves the object at the top of the Z order
		/// </summary>
		public void ZTop()
		{
			flowChart.zTop(this);
			flowChart.invalidate(getRepaintRect(false));
		}

		/// <summary>
		/// The index of the object in the Z order
		/// </summary>
		public int ZIndex
		{
			get
			{
				return zIndex;
			}
			set
			{
				flowChart.setZIndex(this, value);
				onChangeZ();
				flowChart.invalidate(getRepaintRect(false));
			}
		}

		internal virtual void onChangeZ() {}

		private int zIndex;

		internal void updateZIndex(int index)
		{
			zIndex = index;
		}


		// ************ connected arrows *************

		internal virtual void visitHierarchy(CollectionVisitor visitor)
		{
			if (cycleProtect) return;
			cycleProtect = true;

			visitor.accept(this);
			if (subordinateGroup != null)
				subordinateGroup.visitHierarchy(visitor);

			cycleProtect = false;
		}

		internal void visitAttachedItems(CollectionVisitor visitor)
		{
			if (cycleProtect) return;
			cycleProtect = true;

			visitor.accept(this);
			if (subordinateGroup != null)
				subordinateGroup.visitAttachedItems(visitor);

			cycleProtect = false;
		}

		internal void setConstructed() { constructed = true; }
		internal FlowChart flowChart;
		protected internal bool constructed;
		protected internal bool modifying;

		[Browsable(false)]
		public Group SubordinateGroup
		{
			get { return subordinateGroup; }
		}

		[Browsable(false)]
		public Group MasterGroup
		{
			get { return masterGroup; }
		}

		protected internal Group subordinateGroup;
		protected internal Group masterGroup;

		protected internal bool cycleProtect;
		protected internal bool cycleDetected;

		protected internal PointF ptOrg;
		protected internal PointF ptEnd;
		protected internal PointF ptSavedOrg;
		protected internal PointF ptSavedEnd;

		protected internal RectangleF rect;
		protected internal RectangleF rcSaved;

		protected internal PointF ptMdfLast;
		protected internal float modifyDX;
		protected internal float modifyDY;
		protected internal int modifyHandle;

		internal protected void rectFromPoints()
		{
			// fix the rect coordinates
			rect.X = Math.Min(ptOrg.X, ptEnd.X);
			rect.Width = Math.Abs(ptOrg.X - ptEnd.X);
			rect.Y = Math.Min(ptOrg.Y, ptEnd.Y);
			rect.Height = Math.Abs(ptOrg.Y - ptEnd.Y);
		}


		/// <summary>
		/// Color of item's shadow
		/// </summary>
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
					flowChart.setDirty();
					flowChart.invalidate(getRepaintRect(false));
				}
			}
		}

		private Color shadowColor;


		/// <summary>
		/// Horizontal offset of the object shadow
		/// </summary>
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
					flowChart.invalidate(getRepaintRect(false));
					shadowOffsetX = value;
					flowChart.setDirty();
					flowChart.invalidate(getRepaintRect(false));
				}
			}
		}

		private float shadowOffsetX;

		/// <summary>
		/// Vertical offset of the object shadow
		/// </summary>
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
					flowChart.invalidate(getRepaintRect(false));
					shadowOffsetY = value;
					flowChart.setDirty();
					flowChart.invalidate(getRepaintRect(false));
				}
			}
		}

		private float shadowOffsetY;


		public virtual bool Visible
		{
			get
			{
				return !invisible;
			}

			set
			{
				if (value != !invisible)
				{
					invisible = !value;
					if (invisible)
						flowChart.Selection.RemoveObject(this);
					flowChart.setDirty();
          if (!flowChart.LayoutSuspended)
            flowChart.invalidate(getRepaintRect(false));
				}
			}
		}

		protected internal bool invisible;

		public bool Printable
		{
			get
			{
				return printable;
			}

			set
			{
				if (value != printable)
				{
					printable = value;
					flowChart.setDirty();
				}
			}
		}

		private bool printable;


		internal void addManipulator(Manipulator mnp)
		{
			manipulators.Add(mnp);
		}


		internal void removeManipulator(Manipulator mnp)
		{
			if (manipulators.Contains(mnp))
			{
				mnp.onDelete();
				manipulators.Remove(mnp);
			}
		}


		internal void removeAllManipulators()
		{
			foreach (Manipulator manipulator in manipulators)
				manipulator.onDelete();

			manipulators.Clear();
		}


		internal void drawManipulators(Graphics g, bool clip)
		{
			if (flowChart.NowPrinting) return;

			foreach (Manipulator manipulator in manipulators)
			{
				if (clip && manipulator.supportClipping() ||
					(!clip && !manipulator.supportClipping()))
					manipulator.draw(g);
			}
		}

		internal virtual bool manipulatorEnacted(PointF pt)
		{
			foreach (Manipulator manipulator in manipulators)
				if (manipulator.hitTest(pt))
					return true;

			return false;
		}

		internal void addIntrRepaintRect(ref RectangleF rc)
		{
			foreach (Manipulator manipulator in manipulators)
				manipulator.addToRepaintRect(ref rc);
		}

		internal virtual bool ptInManipulator(PointF pt)
		{
			foreach (Manipulator manipulator in manipulators)
				if (manipulator.ptInManipulator(pt))
					return true;

			return false;
		}

		internal ArrayList manipulators;

		internal virtual bool notInteractive()
		{
			if (invisible) return true;
			if (locked) return true;

			return false;
		}

		internal bool Invisible
		{
			get { return invisible; }
		}

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

					flowChart.setDirty();
				}
			}
		}

		internal virtual string getToolTip(PointF point)
		{
			return ToolTip;
		}

		private string toolTip;

		public virtual int getClassId()
		{
			return 1;
		}

		public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			ctx.saveRect(rect);
			writer.Write(constructed);
			writer.Write(locked);
			writer.Write(zIndex);
			writer.Write((int)penDashStyle);
			writer.Write((double)penWidth);
			writer.Write((double)shadowOffsetX);
			writer.Write((double)shadowOffsetY);
			writer.Write(invisible);
			writer.Write(selected);

			bool tagSerializable = false;
			if (tag != null)
				tagSerializable = tag.GetType().IsSerializable;
			writer.Write(tagSerializable);

			if (tagSerializable)
				ctx.saveTag(tag);

			ctx.saveFont(Font);
			writer.Write(toolTip);

			ctx.saveReference(this, subordinateGroup, 100);
			ctx.saveReference(this, masterGroup, 101);

			// new in file format 10
			ctx.saveColor(shadowColor);
			writer.Write(ignoreLayout);

			// new in file format 15
			writer.Write(printable);

			// new in file format 16
			writer.Write(weight);
		}

		public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			rect = ctx.loadRectF();
			constructed = reader.ReadBoolean();
			locked = reader.ReadBoolean();
			zIndex = reader.ReadInt32();
			penDashStyle = (DashStyle)reader.ReadInt32();
			penWidth = (float)reader.ReadDouble();
			shadowOffsetX = (float)reader.ReadDouble();
			shadowOffsetY = (float)reader.ReadDouble();
			invisible = reader.ReadBoolean();
			selected = reader.ReadBoolean();

			tag = null;
			bool tagAvailable = reader.ReadBoolean();
			if (tagAvailable)
				tag = ctx.loadTag();

			manipulators = new ArrayList();

			font = ctx.loadFont();
			toolTip = reader.ReadString();

			ctx.loadReference(this);
			ctx.loadReference(this);

			if (ctx.FileVersion > 9)
			{
				// new in file format 10
				shadowColor = ctx.loadColor();
				ignoreLayout = reader.ReadBoolean();

				if (ctx.FileVersion > 14)
				{
					// new in file format 15
					printable = reader.ReadBoolean();

					if (ctx.FileVersion > 15)
					{
						// new in file format 16
						weight = reader.ReadSingle();
					}
				}
			}
		}

		public virtual void setReference(int refId, IPersists obj)
		{
			switch (refId)
			{
			case 100:
				subordinateGroup = (Group)obj;
				break;
			case 101:
				masterGroup = (Group)obj;
				break;
			}
		}

		public bool IgnoreLayout
		{
			get { return ignoreLayout; }
			set
			{
				ignoreLayout = value;
				flowChart.setDirty();
			}
		}

		private bool ignoreLayout;

		internal virtual ItemState createState()
		{
			return new ItemState(this);
		}

		internal virtual void saveState(ItemState state)
		{
			state.ptOrg = ptOrg;
			state.ptEnd = ptEnd;
			state.rect = rect;
			state.selected = selected;
		}

		internal virtual void restoreState(ItemState state)
		{
			ptOrg = state.ptOrg;
			ptEnd = state.ptEnd;
			rect = state.rect;
			Selected = state.selected;
		}

		internal virtual ItemProperties createProperties()
		{
			return new ItemProperties();
		}

		internal virtual void saveProperties(ItemProperties props)
		{
			props.itemState = createState();
			saveState(props.itemState);

			props.fillColor = fillColor;
			props.frameColor = frameColor;
			props.ignoreLayout = ignoreLayout;
			props.invisible = invisible;
			props.locked = locked;
			props.penDashStyle = penDashStyle;
			props.penWidth = penWidth;
			props.shadowColor = shadowColor;
			props.shadowOffsetX = shadowOffsetX;
			props.shadowOffsetY = shadowOffsetY;
			props.tag = tag;
			props.toolTip = toolTip;

			props.pen = (Pen)pen.Clone();
			props.brush = brush;
			props.brush.AddRef();

			props.printable = printable;
			props.weight = weight;
			props.hyperLink = hyperLink;
		}

		internal virtual void restoreProperties(ItemProperties props)
		{
			fillColor = props.fillColor;
			frameColor = props.frameColor;
			ignoreLayout = props.ignoreLayout;
			invisible = props.invisible;
			locked = props.locked;
			penDashStyle = props.penDashStyle;
			penWidth = props.penWidth;
			shadowColor = props.shadowColor;
			shadowOffsetX = props.shadowOffsetX;
			shadowOffsetY = props.shadowOffsetY;
			tag = props.tag;
			toolTip = props.toolTip;

			pen = (Pen)props.pen.Clone();
			brush.Release();
			brush = props.brush;
			brush.AddRef();

			printable = props.printable;
			weight = props.weight;
			hyperLink = props.hyperLink;

			restoreState(props.itemState);
		}

		internal virtual bool isExpanded() { return true; }
		internal virtual void setExpanded(bool e) {}

		internal virtual void expand(bool incm) {}
		internal virtual void collapse(bool incm) {}

		private SortedList runtimeData;

		internal void setData(int key, object data)
		{
			runtimeData[key] = data;
		}

		internal object getData(int key)
		{
			return runtimeData[key];
		}

		internal void freeData(int key)
		{
			runtimeData.Remove(key);
		}

		private float weight;

		public float Weight
		{
			get { return weight; }
			set
			{
				weight = value;
				flowChart.setDirty();
			}
		}

		internal virtual PointCollection getOutlinePoly()
		{
			return new PointCollection(0);
		}

		public string HyperLink
		{
			get
			{
				return hyperLink;
			}
			set
			{
				if (hyperLink != value)
				{
					if (value == null)
						hyperLink = "";
					else
						hyperLink = value;

					flowChart.setDirty();
				}
			}
		}

		protected internal string hyperLink;
	}
}