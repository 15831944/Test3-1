// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

using MindFusion.FlowChartX.Commands;

namespace MindFusion.FlowChartX
{
	public class NodeEventArgs : EventArgs
	{
		public NodeEventArgs(Node node)
		{
			this.node = node;
		}

		public Node Node
		{
			get { return node; }
		}

		private Node node;
	}

	public class BoxEventArgs : EventArgs
	{
		public BoxEventArgs(Box b)
		{
			box = b;
		}

		public Box Box
		{
			get { return box; }
		}

		private Box box;
	}

	public class ControlHostEventArgs : EventArgs
	{
		public ControlHostEventArgs(ControlHost host)
		{
			controlHost = host;
		}

		public ControlHost ControlHost
		{
			get { return controlHost; }
		}

		private ControlHost controlHost;
	}

	public class TableEventArgs : EventArgs
	{
		public TableEventArgs(Table t)
		{
			table = t;
		}

		public Table Table
		{
			get { return table; }
		}

		private Table table;
	}

	public class ArrowEventArgs : EventArgs
	{
		public ArrowEventArgs(Arrow a)
		{
			arrow = a;
		}

		public Arrow Arrow
		{
			get { return arrow; }
		}

		private Arrow arrow;
	}

	public class GroupEventArgs : EventArgs
	{
		public GroupEventArgs(Group g)
		{
			group = g;
		}

		public Group Group
		{
			get { return group; }
		}

		private Group group;
	}

	public class BoxConfirmArgs : EventArgs
	{
		public BoxConfirmArgs(Box box)
		{
			this.box = box;
			this.point = new PointF(0, 0);
			this.handle = -1;
			confirm = true;
		}

		public BoxConfirmArgs(Box box, PointF point, int handle)
		{
			this.box = box;
			this.point = point;
			this.handle = handle;
			confirm = true;
		}

		public Box Box
		{
			get { return box; }
		}

		public PointF Point
		{
			get { return point; }
		}

		public int SelectionHandle
		{
			get { return handle; }
		}

		public bool Confirm
		{
			get { return confirm; }
			set { confirm = value; }
		}

		public RectangleF OldBounds
		{
			get { return box.rcSaved; }
		}

		private Box box;
		private PointF point;
		private int handle;
		private bool confirm;
	}

	public class ControlHostConfirmArgs : EventArgs
	{
		public ControlHostConfirmArgs(ControlHost host)
		{
			this.controlHost = host;
			this.point = new PointF(0, 0);
			this.handle = -1;
			confirm = true;
		}

		public ControlHostConfirmArgs(ControlHost host, PointF point, int handle)
		{
			this.controlHost = host;
			this.point = point;
			this.handle = handle;
			confirm = true;
		}

		public ControlHost ControlHost
		{
			get { return controlHost; }
		}

		public PointF Point
		{
			get { return point; }
		}

		public int SelectionHandle
		{
			get { return handle; }
		}

		public bool Confirm
		{
			get { return confirm; }
			set { confirm = value; }
		}

		public RectangleF OldBounds
		{
			get { return controlHost.rcSaved; }
		}

		private ControlHost controlHost;
		public PointF point;
		public int handle;
		private bool confirm;
	}

	// Control host serialization event args
	public class ControlHostSerializeArgs : EventArgs
	{
		public ControlHostSerializeArgs(ControlHost host, Stream stream)
		{
			this.host= host;
			this.stream = stream;
		}

		public ControlHost ControlHost
		{
			get { return host; }
		}

		public Stream Stream
		{
			get { return stream; }
		}


		private ControlHost host;
		private Stream stream;
	}

	public class RowEventArgs : EventArgs
	{
		public RowEventArgs(Table table, int rowIndex)
		{
			this.table = table;
			this.rowIndex = rowIndex;
		}

		public Table Table
		{
			get { return table; }
		}

		public int RowIndex
		{
			get { return rowIndex; }
		}

		private Table table;
		private int rowIndex;
	}

	public class TableConfirmArgs : EventArgs
	{
		public TableConfirmArgs(Table table)
		{
			this.table = table;
			this.point = new PointF(0, 0);
			this.handle = -1;
			confirm = true;
			column = row = -1;
		}

		public TableConfirmArgs(Table table, PointF point, int handle)
		{
			this.table = table;
			this.point = point;
			this.handle = handle;
			confirm = true;
			column = row = -1;
		}

		public TableConfirmArgs(Table table, int row, int column)
		{
			this.table = table;
			this.point = new PointF(0, 0);
			this.handle = -1;
			this.confirm = true;
			this.column = column;
			this.row = row;
		}

		public TableConfirmArgs(Table table, int row, int column, PointF point, int handle)
		{
			this.table = table;
			this.point = point;
			this.handle = handle;
			this.confirm = true;
			this.column = column;
			this.row = row;
		}

		public Table Table
		{
			get { return table; }
		}

		public PointF Point
		{
			get { return point; }
		}

		public int SelectionHandle
		{
			get { return handle; }
		}

		public bool Confirm
		{
			get { return confirm; }
			set { confirm = value; }
		}

		public int Column
		{
			get { return column; }
		}

		public int Row
		{
			get { return row; }
		}

		public RectangleF OldBounds
		{
			get { return table.rcSaved; }
		}

		private Table table;
		private PointF point;
		private int handle;
		private bool confirm;
		private int column;
		private int row;
	}

	public class ArrowConfirmArgs : EventArgs
	{
		public ArrowConfirmArgs(Arrow arrow)
		{
			this.arrow = arrow;
			this.point = new PointF(0, 0);
			this.handle = -1;
			confirm = true;
		}

		public ArrowConfirmArgs(Arrow arrow, PointF point, int handle)
		{
			this.arrow = arrow;
			this.point = point;
			this.handle = handle;
			confirm = true;
		}

		public Arrow Arrow
		{
			get { return arrow; }
		}

		public PointF Point
		{
			get { return point; }
		}

		public int SelectionHandle
		{
			get { return handle; }
		}

		public bool Confirm
		{
			get { return confirm; }
			set { confirm = value; }
		}

		private Arrow arrow;
		private PointF point;
		private int handle;
		private bool confirm;
	}

	public class AttachConfirmArgs : EventArgs
	{
		public AttachConfirmArgs(Arrow a, Node node,
			bool org, int anchorIndex, int tableRow)
		{
			arrow = a;
			confirm = true;
			this.node = node;
			changingOrg = org;
			this.anchorIndex = anchorIndex;
			this.tableRow = tableRow;
		}

		public Arrow Arrow
		{
			get { return arrow; }
		}

		public Node Node
		{
			get { return node; }
		}

		public bool ChangingOrigin
		{
			get { return changingOrg; }
		}

		public bool Confirm
		{
			get { return confirm; }
			set { confirm = value; }
		}

		public int AnchorIndex
		{
			get { return anchorIndex; }
		}

		public int TableRow
		{
			get { return tableRow; }
		}

		public Node Origin
		{
			get
			{
				if (changingOrg)
					return node;
				else
					return arrow == null ? null : arrow.Origin;
			}
		}

		public Node Destination
		{
			get
			{
				if (changingOrg)
					return arrow == null ? null : arrow.Destination;
				else
					return node;
			}
		}

		private Arrow arrow;
		private Node node;
		private bool changingOrg;
		private bool confirm;
		private int anchorIndex;
		private int tableRow;
	}

	public class MousePosArgs : EventArgs
	{
		public MousePosArgs(MouseButtons btn, float x, float y)
		{
			this.button = btn;
			this.x = x;
			this.y = y;
			mnpHandle = -1;
		}

		public MousePosArgs(float x, float y, int mnpHandle)
		{
			this.button = MouseButtons.Left;
			this.x = x;
			this.y = y;
			this.mnpHandle = mnpHandle;
		}

		public MouseButtons Button
		{
			get { return button; }
		}

		public PointF RelativePos
		{
			get { return new PointF(x, y); }
		}

		public float X
		{
			get { return x; }
		}

		public float Y
		{
			get { return y; }
		}

		public int SelectionHandle
		{
			get { return mnpHandle; }
		}

		private MouseButtons button;
		private float x;
		private float y;
		protected internal int mnpHandle;
	}

	public class BoxMouseArgs : MousePosArgs
	{
		public BoxMouseArgs(Box b, MouseButtons btn, float x, float y, PointF documentPos) :
			base(btn, x, y)
		{
			box = b;

			this.documentPos = documentPos;
			mnpHandle = b.getHandleIdx(documentPos);
		}

		public BoxMouseArgs(Box b, float x, float y, int mnpHandle) :
			base(x, y, mnpHandle)
		{
			box = b;
		}

		public Box Box
		{
			get { return box; }
		}

		public PointF DocumentPos
		{
			get { return documentPos; }
		}

		private Box box;
		private PointF documentPos;
	}

	public class ControlHostMouseArgs : MousePosArgs
	{
		public ControlHostMouseArgs(ControlHost host, MouseButtons btn, float x, float y, PointF documentPos) :
			base(btn, x, y)
		{
			controlHost = host;

			this.documentPos = documentPos;
			mnpHandle = host.getHandleIdx(documentPos);
		}

		public ControlHostMouseArgs(ControlHost host, float x, float y, int mnpHandle) :
			base(x, y, mnpHandle)
		{
			controlHost = host;
		}

		public ControlHost ControlHost
		{
			get { return controlHost; }
		}

		public PointF DocumentPos
		{
			get { return documentPos; }
		}

		private ControlHost controlHost;
		private PointF documentPos;
	}

	public class TableMouseArgs : MousePosArgs
	{
		public TableMouseArgs(Table t, MouseButtons btn, float x, float y, PointF documentPos) :
			base(btn, x, y)
		{
			table = t;
			col = row = -1;

			this.documentPos = documentPos;
			mnpHandle = t.getHandleIdx(documentPos);
		}

		public TableMouseArgs(Table t, float x, float y, int mnpHandle) :
			base(x, y, mnpHandle)
		{
			table = t;
			col = row = -1;
		}

		public TableMouseArgs(Table t, MouseButtons btn, float x, float y, int col, int row) :
			base(btn, x, y)
		{
			table = t;
			this.col = col;
			this.row = row;
		}

		public Table Table
		{
			get { return table; }
		}

		public int Column
		{
			get { return col; }
		}

		public int Row
		{
			get { return row; }
		}

		public PointF DocumentPos
		{
			get { return documentPos; }
		}

		private Table table;
		internal int col, row;
		private PointF documentPos;
	}

	public class ArrowMouseArgs : MousePosArgs
	{
		public ArrowMouseArgs(Arrow a, MouseButtons btn, float x, float y, PointF documentPos) :
			base(btn, x, y)
		{
			arrow = a;

			this.documentPos = documentPos;
			mnpHandle = a.getHandleIdx(documentPos);
		}

		public ArrowMouseArgs(Arrow a, float x, float y, int mnpHandle) :
			base(x, y, mnpHandle)
		{
			arrow = a;
		}

		public Arrow Arrow
		{
			get { return arrow; }
		}

		public PointF DocumentPos
		{
			get { return documentPos; }
		}

		private Arrow arrow;
		private PointF documentPos;
	}

	/// <summary>
	/// Holds the arguments for the DrawArrow event.
	/// </summary>
	public class ArrowDrawArgs : ArrowEventArgs
	{
		public ArrowDrawArgs(Graphics g, Arrow arrow, bool shadow, PointCollection points) :
			base(arrow)
		{
			this.graphics = g;
			this.shadow = shadow;
			this.points = points;
		}

		public Graphics Graphics { get { return graphics; } }
		private Graphics graphics;

		public bool Shadow { get { return shadow; } }
		private bool shadow;

		public PointCollection Points { get { return points; } }
		private PointCollection points;
	}

	/// <summary>
	/// Holds the arguments for the DrawBox event.
	/// </summary>
	public class BoxDrawArgs : BoxEventArgs
	{
		public BoxDrawArgs(Graphics g, Box box, bool shadow, RectangleF bounds) :
			base(box)
		{
			this.graphics = g;
			this.shadow = shadow;
			this.bounds = bounds;
		}

		public Graphics Graphics { get { return graphics; } }
		private Graphics graphics;

		public bool Shadow { get { return shadow; } }
		private bool shadow;

		public RectangleF Bounds { get { return bounds; } }
		private RectangleF bounds;
	}

	/// <summary>
	/// Holds the arguments for the DrawTable event.
	/// </summary>
	public class TableDrawArgs : TableEventArgs
	{
		public TableDrawArgs(Graphics g, Table table, bool shadow, RectangleF bounds) :
			base(table)
		{
			this.graphics = g;
			this.shadow = shadow;
			this.bounds = bounds;
		}

		public Graphics Graphics { get { return graphics; } }
		private Graphics graphics;

		public bool Shadow { get { return shadow; } }
		private bool shadow;

		public RectangleF Bounds { get { return bounds; } }
		private RectangleF bounds;
	}

	/// <summary>
	/// Holds the arguments for the DrawCell event.
	/// </summary>
	public class CellDrawArgs : TableEventArgs
	{
		public CellDrawArgs(Graphics g, Table table, int col, int row, RectangleF bounds) :
			base(table)
		{
			this.graphics = g;
			this.column = col;
			this.row = row;
			this.bounds = bounds;
		}

		public Graphics Graphics { get { return graphics; } }
		private Graphics graphics;

		public RectangleF Bounds { get { return bounds; } }
		private RectangleF bounds;

		public int Column { get { return column; } }
		private int column;

		public int Row { get { return row; } }
		private int row;
	}

	public class MarkDrawArgs : EventArgs
	{
		public MarkDrawArgs(Graphics g, PointF location,
			Node node, AnchorPattern anchorPattern, int anchorIndex)
		{
			this.graphics = g;
			this.location = location;
			this.node = node;
			this.anchorPattern = anchorPattern;
			this.anchorIndex = anchorIndex;
		}

		public Graphics Graphics
		{
			get { return graphics; }
		}

		public PointF Location
		{
			get { return location; }
		}

		public Node Node
		{
			get { return node; }
		}

		public AnchorPattern AnchorPattern
		{
			get { return anchorPattern; }
		}

		public int AnchorIndex
		{
			get { return anchorIndex; }
		}

		private Graphics graphics;
		internal PointF location;
		private Node node;
		private AnchorPattern anchorPattern;
		private int anchorIndex;
	}

	public class ControlPaintArgs : ControlHostEventArgs
	{
		public ControlPaintArgs(Graphics g, ControlHost host, RectangleF rect) :
			base(host)
		{
			this.graphics = g;
			this.rect = rect;
			this.handled = true;
		}

		public Graphics Graphics { get { return graphics; } }
		private Graphics graphics;

		public RectangleF PaintRect { get { return rect; } }
		private RectangleF rect;

		public bool Handled
		{
			get { return handled; }
			set { handled = value; }
		}
		private bool handled;
	}

	public class BoxTextArgs : BoxEventArgs
	{
		public BoxTextArgs(Box box, string oldText, string newText) :
			base(box)
		{
			this.oldText = oldText;
			this.newText = newText;
		}

		public string OldText
		{
			get { return oldText; }
		}

		public string NewText
		{
			get { return newText; }
		}

		private string oldText;
		private string newText;
	}

	public class TableCaptionArgs : TableEventArgs
	{
		public TableCaptionArgs(Table table, string oldText, string newText) :
			base(table)
		{
			this.oldText = oldText;
			this.newText = newText;
		}

		public string OldText
		{
			get { return oldText; }
		}

		public string NewText
		{
			get { return newText; }
		}

		protected internal string oldText;
		protected internal string newText;
	}

	public class CellTextArgs : TableCaptionArgs
	{
		public CellTextArgs(Table table, string oldText, string newText, int col, int row) :
			base(table, oldText, newText)
		{
			this.oldText = oldText;
			this.newText = newText;
			this.column = col;
			this.row = row;
		}

		public int Column
		{
			get { return column; }
		}

		public int Row
		{
			get { return row; }
		}

		private int column;
		private int row;
	}

	public class UndoEventArgs : EventArgs
	{
		public UndoEventArgs(Command command)
		{
			this.command = command;
		}

		public Command Command
		{
			get { return command; }
		}

		private Command command;
	}

	public class UndoConfirmEventArgs : UndoEventArgs
	{
		public UndoConfirmEventArgs(Command command) : base(command)
		{
			this.confirm = true;
		}

		public bool Confirm
		{
			get { return confirm; }
			set { confirm = value; }
		}

		private bool confirm;
	}

	public class InplaceEditArgs : EventArgs
	{
		public InplaceEditArgs(InplaceEditable item, TextBox textBox)
		{
			this.item = item;
			this.textBox = textBox;
		}

		public TextBox TextBox
		{
			get { return textBox; }
		}

		public InplaceEditable Item
		{
			get { return item; }
		}

		private TextBox textBox;
		private InplaceEditable item;
	}

	/// <summary>
	/// Provides data for the DrawSelHandles event
	/// </summary>
	public class DrawEventArgs : EventArgs
	{
		public DrawEventArgs(Graphics graphics, ChartObject item)
		{
			this.graphics = graphics;
			this.item = item;
		}

		public Graphics Graphics
		{
			get { return graphics; }
		}
		
		public ChartObject Item
		{
			get { return item; }
		}

		private Graphics graphics;
		private ChartObject item;
	}

	/// <summary>
	/// Provides data for the StartModify event
	/// </summary>
	public class ItemEventArgs : EventArgs
	{
		public ItemEventArgs(ChartObject item)
		{
			this.item = item;
		}

		public ChartObject Item
		{
			get { return item; }
		}

		private ChartObject item;
	}

	/// <summary>
	/// Provides data for the HitTestSelHandles event
	/// </summary>
	public class HitTestEventArgs : EventArgs
	{
		public HitTestEventArgs(ChartObject item, PointF mousePosition)
		{
			this.item = item;
			this.mousePosition = mousePosition;
			this.hitResult = -1;
		}

		/// <summary>
		/// the diagram item for which to perform hit testing
		/// </summary>
		public ChartObject Item
		{
			get { return item; }
		}

		/// <summary>
		/// mouse location in logical diagram coordinates
		/// </summary>
		public PointF MousePosition
		{
			get { return mousePosition; }
		}

		/// <summary>
		/// index of the selection handle that was hit
		/// </summary>
		public int HitResult
		{
			get { return hitResult; }
			set { hitResult = value; }
		}

		private ChartObject item;
		private PointF mousePosition;
		private int hitResult;
	}

	// general event delegates
	public delegate void NodeEventHandler(object sender, NodeEventArgs e);
	public delegate void ItemEventHandler(object sender, ItemEventArgs e);
	public delegate void ArrowEvent(object sender, ArrowEventArgs e);
	public delegate void BoxEvent(object sender, BoxEventArgs e);
	public delegate void ControlHostEvent(object sender, ControlHostEventArgs e);
	public delegate void TableEvent(object sender, TableEventArgs e);
	public delegate void GroupEvent(object sender, GroupEventArgs e);
	public delegate void SelectionEvent(object sender, EventArgs e);

	// delegates related to mouse operations
	public delegate void ArrowMouseEvent(object sender, ArrowMouseArgs e);
	public delegate void BoxMouseEvent(object sender, BoxMouseArgs e);
	public delegate void ControlHostMouseEvent(object sender, ControlHostMouseArgs e);
	public delegate void TableMouseEvent(object sender, TableMouseArgs e);
	public delegate void DocMouseEvent(object sender, MousePosArgs e);

	// delegates related to validation of interactive actions
	public delegate void ArrowConfirmation(object sender, ArrowConfirmArgs e);
	public delegate void BoxConfirmation(object sender, BoxConfirmArgs e);
	public delegate void ControlHostConfirmation(object sender, ControlHostConfirmArgs e);
	public delegate void TableConfirmation(object sender, TableConfirmArgs e);
	public delegate void AttachConfirmation(object sender, AttachConfirmArgs e);
	public delegate void HitTestEventHandler(object sender, HitTestEventArgs e);

	// delegates related to inplace editing 
	public delegate void BoxTextEditedEvent(object sender, BoxTextArgs e);
	public delegate void CellTextEditedEvent(object sender, CellTextArgs e);
	public delegate void TableCaptionEditedEvent(object sender, TableCaptionArgs e);
	public delegate void InplaceEditEvent(object sender, InplaceEditArgs e);

	// delegates related to custom drawing
	public delegate void ArrowCustomDraw(object sender, ArrowDrawArgs e);
	public delegate void BoxCustomDraw(object sender, BoxDrawArgs e);
	public delegate void TableCustomDraw(object sender, TableDrawArgs e);
	public delegate void CellCustomDraw(object sender, CellDrawArgs e);
	public delegate void MarkCustomDraw(object sender, MarkDrawArgs e);
	public delegate void ControlHostPaint(object sender, ControlPaintArgs e);
	public delegate void CustomDrawEventHandler(object sender, DrawEventArgs e);

	// miscellaneous
	public delegate void ControlHostSerializeEvent(object sender, ControlHostSerializeArgs e);
	public delegate void UndoEventHandler(object sender, UndoEventArgs e);
	public delegate void UndoConfirmEventHandler(object sender, UndoConfirmEventArgs e);
	public delegate void RowEventHandler(object sender, RowEventArgs e);
}