// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.IO;
using System.Drawing;
using System.Windows.Forms;
using System.Reflection;
using MindFusion.FlowChartX.Visitors;


namespace MindFusion.FlowChartX
{
	public class ControlHost : Node
	{
		public ControlHost(FlowChart parent) : base(parent)
		{
			ctrlMouseAction = parent.HostedCtrlMouseAction;

			mouseDownHandler = null;
			mouseUpHandler = null;

			// ChartObject expects these to be valid
			pen = (Pen)parent.BoxPen.Clone();
			brush = parent.BoxBrush;
			brush.AddRef();

			System.Type controlType = parent.DefaultControlType;
			_control = null;
			if (controlType != null)
			{
				// Instantiate the control
				ConstructorInfo ctorInfo =
					controlType.GetConstructor(System.Type.EmptyTypes);

				Control ctrl = (System.Windows.Forms.Control)ctorInfo.Invoke(null);
				if (ctrl != null)
					ctrl.Visible = false;
				setControl(ctrl);
			}

			setExpandable(parent.ControlHostsExpandable);
			setExpanded(true);

			selStyle = parent.ControlHostHandlesStyle;
		}

		public ControlHost(ControlHost prototype) : base(prototype)
		{
			ctrlMouseAction = prototype.ctrlMouseAction;

			_control = null;
			if (prototype._control != null)
			{
				System.Type controlType = prototype._control.GetType();
				if (controlType != null)
				{
					// Instantiate the control
					ConstructorInfo ctorInfo =
						controlType.GetConstructor(System.Type.EmptyTypes);

					_control = (System.Windows.Forms.Control)ctorInfo.Invoke(null);
					if (_control != null)
						_control.Visible = false;
				}
			}

			setExpandable(prototype.Expandable);
			setExpanded(prototype.Expanded);

			selStyle = prototype.selStyle;
		}

		public ControlHost(FlowChart parent, System.Windows.Forms.Control control) :
			base(parent)
		{
			ctrlMouseAction = parent.HostedCtrlMouseAction;

			// ChartObject expects these to be valid
			pen = (Pen)parent.BoxPen.Clone();
			brush = parent.BoxBrush;
			brush.AddRef();

			if (control != null)
				control.Visible = false;
			setControl(control);

			setExpandable(parent.ControlHostsExpandable);
			setExpanded(true);
		}

		private void setControl(Control ctrl)
		{
			if (_control != null)
			{
				// Detach event listeners
				_control.MouseDown -= mouseDownHandler;
				_control.MouseUp -= mouseUpHandler;
				mouseDownHandler = null;
				mouseUpHandler = null;
			}

			_control = ctrl;

			if (_control != null)
			{
				// when the host is added to the flowchart,
				// add the contained control to the flowchart children list
				if (!fcParent.Controls.Contains(_control))
					fcParent.Controls.Add(_control);

				// Listen to mouse clicks over the control
				_control.MouseDown += (mouseDownHandler = new MouseEventHandler(OnControlMouseDown));
				_control.MouseUp += (mouseUpHandler = new MouseEventHandler(OnControlMouseUp));
			}
		}

		private MouseEventHandler mouseDownHandler;
		private MouseEventHandler mouseUpHandler;

		/// <summary>
		/// Called when the ControlHost is added to the flowchart
		/// either by users, programmatically or via undo / redo.
		/// </summary>
		internal override void onAdd()
		{
			// Remove control from flowchart
			if (_control != null)
			{
				if (!fcParent.Controls.Contains(_control))
					fcParent.Controls.Add(_control);

				if (mouseDownHandler == null)
				{
					_control.MouseDown += (mouseDownHandler = new MouseEventHandler(OnControlMouseDown));
					_control.MouseUp += (mouseUpHandler = new MouseEventHandler(OnControlMouseUp));
				}
			}

			_control.Visible = Visible;
		}

		/// <summary>
		/// Called when the ControlHost is removed from the flowchart
		/// either by users, programmatically or via undo / redo.
		/// </summary>
		internal override void onRemove()
		{
			base.onRemove();

			// Remove control from flowchart
			if (_control != null)
				if (fcParent.Controls.Contains(_control))
					fcParent.Controls.Remove(_control);

			// Fire a ControlHostDeleted event
			if (constructed)
				fcParent.fireControlHostDeleted(this);
		}

		// called when the ControlHost is completely removed, i.e. when it
		// is no longer in the chart or in the undo/redo hostory
		internal override void freeResources()
		{
			base.freeResources();

			// cleanup
			if (_control != null)
			{
				_control.Dispose();
				_control = null;
			}
		}

		// Serialization
		public override int getClassId()
		{
			return 39;
		}

		public override void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			base.saveTo(writer, ctx);

			writer.Write((long)enabledHandles);
			writer.Write((int)ctrlMouseAction);
			ctx.saveReference(this, incomingArrows, 1);
			ctx.saveReference(this, outgoingArrows, 2);

			if (_control != null)
			{
				System.Type type = _control.GetType();

				string assemblyName = type.Assembly.GetName().Name;
				string typeName = type.FullName;

				writer.Write(true);
				writer.Write(assemblyName);
				writer.Write(typeName);

				// Save control serializable properties
				PropertyInfo[] props = type.GetProperties();
				foreach (PropertyInfo prop in props)
				{
					// Will not serialize indexed properties
					ParameterInfo[] iparams = prop.GetIndexParameters();
					if (iparams == null || iparams.Length > 0)
						continue;
					if (!prop.CanRead || !prop.CanWrite)
						continue;

					object val = null;
					try
					{
						val = prop.GetValue(_control, null);
					}
					catch
					{
						// Some properties cannot be read
						continue;
					}
					if (val == null)
						continue;
					if (!(val.GetType().IsSerializable))
						continue;
					if (val is ChartObject)
						continue;

					writer.Write(true);
					writer.Write(prop.Name);
					ctx.saveTag(val);
				}
				writer.Write(false);

				fcParent.fireControlHostSerializing(this, writer);
			}
			else
			{
				writer.Write(false);
			}

			// new in save format 14
			writer.Write(Expanded);
			writer.Write(Expandable);

			// new in save format 22
			writer.Write((int)selStyle);
		}

		public override void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			base.loadFrom(reader, ctx);

			enabledHandles = (Handles)reader.ReadInt64();
			ctrlMouseAction = (HostMouseAction)reader.ReadInt32();
			ctx.loadReference(this); // incoming arrows
			ctx.loadReference(this); // outgoing arrows

			// Load control
			if (reader.ReadBoolean())
			{
				string assemblyName = reader.ReadString();
				string typeName = reader.ReadString();

				System.Type type = Utilities.getLoadedType(typeName, assemblyName);
				if (type == null)
					throw new FileLoadException("Cannot load hosted control of type " + typeName);

				ConstructorInfo ctorInfo = type.GetConstructor(System.Type.EmptyTypes);

				// Instantiate
				Control = (System.Windows.Forms.Control)ctorInfo.Invoke(null);

				// Read properties
				while (reader.ReadBoolean())
				{
					string propName = reader.ReadString();
					object val = ctx.loadTag();
					try
					{
						PropertyInfo prop = type.GetProperty(propName);
						prop.SetValue(_control, val, null);
					}
					catch
					{
						// Exceptions happen
					}
				}

				fcParent.fireControlHostDeserializing(this, reader);
			}
			else
			{
				Control = null;
			}

			if (ctx.FileVersion > 13)
			{
				// new in save format 14
				setExpanded(reader.ReadBoolean());
				setExpandable(reader.ReadBoolean());

				if (ctx.FileVersion > 21)
				{
					// new in save format 22
					selStyle = (HandlesStyle)reader.ReadInt32();
				}
			}
		}

		public override void setReference(int refId, IPersists obj)
		{
			base.setReference(refId, obj);

			switch (refId)
			{
				case 1:
					incomingArrows = (ArrowCollection)obj;
					break;
				case 2:
					outgoingArrows = (ArrowCollection)obj;
					break;
			}
		}


		internal override bool containsPoint(System.Drawing.PointF pt)
		{
			return Utilities.pointInRect(pt, rect);
		}

		public override bool HitTestHandle(System.Drawing.PointF pt, ref int handle)
		{
			if (selStyle != HandlesStyle.Custom)
			{
				RectangleF rc = Utilities.normalizeRect(rect);

				return Utilities.pointInHandle(pt,
					ref handle, rc, 0, enabledHandles & ~Handles.Rotate, 0,
					selStyle, fcParent.SelHandleSize, fcParent.MeasureUnit);
			}
			else
			{
				// let the application do hit testing
				return fcParent.hitTestSelHandles(this, pt, ref handle);
			}
		}

		internal override int getHandleIdx(PointF pt)
		{
			int handle = -1;

			RectangleF rc = Utilities.normalizeRect(rect);
			if (Utilities.pointInHandle(pt, ref handle, rc, 0, Handles.All,
				0, selStyle, fcParent.SelHandleSize, fcParent.MeasureUnit))
				return handle;

			return -1;
		}

		internal override PointF getIntersection(PointF ptOrg, PointF ptEnd)
		{
			if (ptOrg == ptEnd) return ptOrg;
			PointF pt = getCenter();
			if (rect.Bottom == rect.Top || rect.Left == rect.Right)
				return pt;

			RectangleF rc = Utilities.normalizeRect(rect);

			if (Utilities.pointInRect(ptOrg, rc) &&
				Utilities.pointInRect(ptEnd, rc)) return getCenter();

			Utilities.getRectIntr(rc, ptOrg, ptEnd, ref pt);

			return pt;
		}

		internal override Region getRegion()
		{
			return new Region(Utilities.normalizeRect(rect));
		}

		internal PointF getHostPtPercent(PointF pt)
		{
			return Utilities.getRectPtPercent(pt, getBoundingRect());
		}

		internal PointF hostPtFromPercent(PointF ptPer)
		{
			return Utilities.rectPtFromPercent(ptPer, getBoundingRect());
		}

		internal void updateControlPosition()
		{
			onRectUpdate();
		}

		protected override void onRectUpdate()
		{
			// Fit the contained control
			if (_control != null)
			{
				RectangleF rc = Utilities.normalizeRect(rect);
				rc.Inflate(-Constants.getMillimeter(fcParent.MeasureUnit), -Constants.getMillimeter(fcParent.MeasureUnit));

				Point ptLeftTop = fcParent.DocToClient(new PointF(rc.Left, rc.Top));
				Point ptRightBottom = fcParent.DocToClient(new PointF(rc.Right, rc.Bottom));

				_control.Location = new Point(ptLeftTop.X + 1, ptLeftTop.Y + 1);
				_control.Size = new Size(
					Math.Max(ptRightBottom.X - ptLeftTop.X, 4),
					Math.Max(ptRightBottom.Y - ptLeftTop.Y, 4));
				_control.Visible = Visible;
			}
		}

		// In-place editing
		internal override string getTextToEdit()
		{
			return "";
		}

		internal override void setEditedText(string newText)
		{
		}

		internal override RectangleF getEditRect()
		{
			return getBoundingRect();
		}

		public override void Draw(System.Drawing.Graphics g, bool shadow)
		{
			if (!Visible)
				return;
			if (fcParent.NowPrinting && !Printable) return;

			if (shadow)
			{
				// draw a shadow rectangle
				System.Drawing.Brush br = new System.Drawing.SolidBrush(ShadowColor);
				RectangleF rc = Utilities.normalizeRect(rect);
				rc.Inflate(
					-Constants.getMillimeter(fcParent.MeasureUnit),
					-Constants.getMillimeter(fcParent.MeasureUnit));
				rc.Offset(ShadowOffsetX, ShadowOffsetY);
				g.FillRectangle(br, rc);
				br.Dispose();
			}
			else
			{
				// Draw something if the contained control is null
				if (_control == null)
				{
					System.Drawing.Brush brush =
						new System.Drawing.SolidBrush(fcParent.BoxFillColor);
					System.Drawing.Pen pen = 
						new System.Drawing.Pen(fcParent.BoxFrameColor, 0);

					RectangleF rcf = Utilities.normalizeRect(rect);
					StringFormat textFormat = new StringFormat();

					textFormat.Alignment = StringAlignment.Center;
					textFormat.LineAlignment = StringAlignment.Center;

					g.FillRectangle(brush, rcf);
					g.DrawRectangle(pen, rcf.X, rcf.Y, rcf.Width, rcf.Height);

					pen.Dispose();
					brush.Dispose();
				}

				// this is used for printing, print preview and in Overview control.
				// in these cases it is not always possible to make a request
				// to the hosted control to paint itself in our own Graphics instance
				if (fcParent.RenderOptions.PaintControls)
				{
					RectangleF rc = Utilities.normalizeRect(rect);
					rc.Inflate(
						-Constants.getMillimeter(fcParent.MeasureUnit),
						-Constants.getMillimeter(fcParent.MeasureUnit));

					// let applications handle painting
					bool painted = fcParent.paintControlHost(g, this, rc);

					if (!painted)
					{
						// the PaintControlHost event was not handled so try calling 
						// Control's OnPaint method; that does not work for all controls
						Image image = createImage();
						if (image != null)
						{
							g.DrawImage(image, rc);
							image.Dispose();
						}
					}
				}

				drawManipulators(g, false);
				if (fcParent.RenderOptions.EnableAnchors) drawAnchors(g);
			}
		}

		internal override void drawSelHandles(Graphics g, Color color)
		{
			if (selStyle != HandlesStyle.Custom)
			{
				RectangleF rc = Utilities.normalizeRect(rect);
				Utilities.drawSelHandles(g, color, fcParent.DisabledMnpColor,
					rc, 0, enabledHandles & ~Handles.Rotate, fcParent.ShowDisabledHandles,
					selStyle, fcParent.SelHandleSize);
			}
			else
			{
				// let applications custom draw the selection handles
				fcParent.fireDrawSelHandles(g, this);
			}
		}

		public HandlesStyle HandlesStyle
		{
			get	{ return selStyle; }

			set
			{
				if (value != selStyle)
				{
					selStyle = value;
					fcParent.invalidate(getRepaintRect(false));
					fcParent.setDirty();
				}
			}
		}

		internal override Cursor getCannotDropCursor()
		{
			return fcParent.CurCannotCreate;
		}

		internal override Cursor getCanDropCursor()
		{
			return fcParent.CurPointer;
		}


		internal override Link createLink(Arrow arrow, PointF pt, bool incoming)
		{
			return new ControlHostLink(this, arrow, incoming);
		}


		public override ItemType getType()
		{
			return ItemType.ControlHost;
		}

		internal override bool canModify(int handle)
		{
			return ((long)enabledHandles & (1 << handle)) != 0;
		}

		public System.Windows.Forms.Control Control
		{
			get { return _control; }
			set
			{
				if (_control != value)
				{
					// Remove the previous control (if any)
					if (_control != null)
						fcParent.Controls.Remove(_control);

					setControl(value);
					updateControlPosition();
				}
			}
		}

		public override bool Visible
		{
			get { return base.Visible; }
			set
			{
				base.Visible = value;

				// Toggle contained control visibility
				if (_control != null)
					_control.Visible = value;
			}
		}

		/// <summary>
		/// Control mouse up handler -> activate the host.
		/// </summary>
		private void OnControlMouseUp(object sender, MouseEventArgs e)
		{
			// Fire clicked event
			Graphics g = fcParent.CreateGraphics();
			fcParent.setTransforms(g);
			PointF point = Utilities.deviceToDoc(g,
				e.X + _control.Location.X, e.Y + _control.Location.Y);
			g.Dispose();
			fcParent.fireClickedEvent(this, point, e.Button);

			if (ctrlMouseAction == HostMouseAction.SelectHost &&
				fcParent.confirmSelect(this))
			{
				if ((fcParent.ModifierKeyActions.GetKeys(ModifierKeyAction.Select) &
					Control.ModifierKeys) != 0)
					fcParent.Selection.Toggle(this);
				else
					fcParent.Selection.Change(this);

				fcParent.Invalidate();
			}
		}

		private void OnControlMouseDown(object sender, MouseEventArgs e)
		{
			// if the control host mouse action is hmaIgnoreControl, behave as if
			// the flowchart has been clicked (i.e. start arrow)
			if (ctrlMouseAction == HostMouseAction.IgnoreControl)
			{
				MouseEventArgs newArgs = new MouseEventArgs(e.Button, e.Clicks,
					e.X + _control.Location.X, e.Y + _control.Location.Y, e.Delta);
				fcParent.RaiseMouseDown(newArgs);
			}
		}

		public HostMouseAction CtrlMouseAction
		{
			get { return ctrlMouseAction; }
			set { ctrlMouseAction = value; }
		}

		internal override ItemProperties createProperties()
		{
			return new ControlHostProperties();
		}

		internal override void saveProperties(ItemProperties props)
		{
			base.saveProperties(props);

			ControlHostProperties hprops = (ControlHostProperties)props;

			hprops.control = Control;
			hprops.ctrlMouseAction = ctrlMouseAction;
			hprops.selStyle = selStyle;
		}

		internal override void restoreProperties(ItemProperties props)
		{
			ControlHostProperties hprops = (ControlHostProperties)props;

			Control = hprops.control;
			ctrlMouseAction = hprops.ctrlMouseAction;
			selStyle = hprops.selStyle;

			base.restoreProperties(props);
		}

		private Image createImage()
		{
			if (_control == null)
				return null;

			// call protected OnPaint to get the control paint itself
			// that does not work with wrapped native Windows controls
			System.Type type = _control.GetType();
			MethodInfo mi = type.GetMethod("OnPaint",
				BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.InvokeMethod,
				null, CallingConventions.Any, new Type[] { typeof(PaintEventArgs)}, null);
			if (mi != null)
			{
				// create bitmap compatible with the flowchart's resolution
				Graphics graphics = fcParent.CreateGraphics();
				Bitmap bmp = new Bitmap(
					_control.ClientRectangle.Width,
					_control.ClientRectangle.Height,
					graphics);

				// tell the hosted control to paint itself
				Graphics memGraphics = Graphics.FromImage(bmp);
				PaintEventArgs pea = new PaintEventArgs(
					memGraphics, _control.ClientRectangle);
				mi.Invoke(_control, BindingFlags.InvokeMethod | BindingFlags.Instance | BindingFlags.NonPublic, null, new object[] { pea }, System.Globalization.CultureInfo.InstalledUICulture);
				memGraphics.Dispose();

				graphics.Dispose();

				return bmp;
			}

			return null;
		}

		private System.Windows.Forms.Control _control;
		private HostMouseAction ctrlMouseAction;
		private HandlesStyle selStyle;
	}
}