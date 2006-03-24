// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Collections;
using System.Drawing;
using System.IO;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Collection of chart objects.
	/// </summary>
	[Serializable]
	public class ChartObjectCollection : CollectionBase, IPersists
	{
		public ChartObjectCollection()
		{
		}

		public ChartObjectCollection Clone()
		{
			ChartObjectCollection oc = new ChartObjectCollection();
			foreach (ChartObject co in this)
				oc.Add(co);
			return oc;
		}

		public void Add(ChartObject a)
		{
			List.Add(a);
		}

		public void Insert(int i, ChartObject a)
		{
			List.Insert(i, a);
		}

		public ChartObject GetAt(int i)
		{
			if(i >= Count)
				return null;

			return (ChartObject)List[i];
		}

		public void Remove(ChartObject a)
		{
			List.Remove(a);
		}

		public new void Clear()
		{
			List.Clear();
		}

		public bool Contains(ChartObject obj)
		{
			return List.Contains(obj);
		}

		public int Capacity
		{
			get { return InnerList.Capacity; }
			set { InnerList.Capacity = value; }
		}

		public ChartObject this[int index]
		{
			get
			{
				return (ChartObject)List[index];
			}
			set
			{
				List[index] = value;
			}
		}

		public virtual int getClassId()
		{
			return 13;
		}

		public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			writer.Write(List.Count);
			for (int i = 0; i < List.Count; ++i)
			{
				ctx.saveReference(this, (ChartObject)List[i], i);
			}
		}

		public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			int count = reader.ReadInt32();
			InnerList.Capacity = count;
			for (int i = 0; i < count; ++i)
			{
				InnerList.Add(null);
				ctx.loadReference(this);
			}
		}

		public virtual void setReference(int refId, IPersists obj)
		{
			InnerList[refId] = obj;
		}
	}

	/// <summary>
	/// Collection of arrows.
	/// </summary>
	public class ArrowCollection : CollectionBase, IPersists
	{
		public ArrowCollection()
		{
		}

		public ArrowCollection Clone()
		{
			ArrowCollection ac = new ArrowCollection();
			foreach (Arrow a in this)
				ac.Add(a);
			return ac;
		}

		public void Add(Arrow a)
		{
			List.Add(a);
		}

		public void Insert(int i, Arrow a)
		{
			List.Insert(i, a);
		}

		public Arrow GetAt(int i)
		{
			if(i >= Count)
				return null;

			return (Arrow)List[i];
		}

		public void Remove(Arrow a)
		{
			List.Remove(a);
		}

		public new void Clear()
		{
			List.Clear();
		}

		public int Capacity
		{
			get { return InnerList.Capacity; }
			set { InnerList.Capacity = value; }
		}

		public Arrow this[int index]
		{
			get
			{
				return (Arrow)List[index];
			}
			set
			{
				List[index] = value;
			}
		}

		public virtual int getClassId()
		{
			return 10;
		}

		public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			writer.Write(List.Count);
			for (int i = 0; i < List.Count; ++i)
			{
				ctx.saveReference(this, (Arrow)List[i], i);
			}
		}

		public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			int count = reader.ReadInt32();
			InnerList.Capacity = count;
			for (int i = 0; i < count; ++i)
			{
				InnerList.Add(null);
				ctx.loadReference(this);
			}
		}

		public virtual void setReference(int refId, IPersists obj)
		{
			InnerList[refId] = obj;
		}

		public bool Contains(Arrow obj)
		{
			return List.Contains(obj);
		}
	}

	/// <summary>
	/// Collection of boxes.
	/// </summary>
	public class BoxCollection : CollectionBase, IPersists
	{
		public BoxCollection()
		{
		}

		public BoxCollection Clone()
		{
			BoxCollection bc = new BoxCollection();
			foreach (Box b in this)
				bc.Add(b);
			return bc;
		}

		public void Add(Box b)
		{
			List.Add(b);
		}

		public void Insert(int i, Box b)
		{
			List.Insert(i, b);
		}

		public Box GetAt(int i)
		{
			if(i >= Count)
				return null;

			return (Box)List[i];
		}

		public void Remove(Box b)
		{
			List.Remove(b);
		}

		public new void Clear()
		{
			List.Clear();
		}

		public int Capacity
		{
			get { return InnerList.Capacity; }
			set { InnerList.Capacity = value; }
		}

		public Box this[int index]
		{
			get
			{
				return (Box)List[index];
			}
			set
			{
				List[index] = value;
			}
		}

		public virtual int getClassId()
		{
			return 12;
		}

		public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			writer.Write(List.Count);
			for (int i = 0; i < List.Count; ++i)
			{
				ctx.saveReference(this, (Box)List[i], i);
			}
		}

		public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			int count = reader.ReadInt32();
			InnerList.Capacity = count;
			for (int i = 0; i < count; ++i)
			{
				InnerList.Add(null);
				ctx.loadReference(this);
			}
		}

		public virtual void setReference(int refId, IPersists obj)
		{
			InnerList[refId] = obj;
		}
	}

	/// <summary>
	/// Collection of control hosts.
	/// </summary>
	public class ControlHostCollection : CollectionBase, IPersists
	{
		public ControlHostCollection()
		{
		}

		public ControlHostCollection Clone()
		{
			ControlHostCollection chc = new ControlHostCollection();
			foreach (ControlHost ch in this)
				chc.Add(ch);
			return chc;
		}

		public void Add(ControlHost host)
		{
			List.Add(host);
		}

		public void Insert(int i, ControlHost host)
		{
			List.Insert(i, host);
		}

		public ControlHost GetAt(int i)
		{
			if(i >= Count)
				return null;

			return (ControlHost)List[i];
		}

		public void Remove(ControlHost host)
		{
			List.Remove(host);
		}

		public new void Clear()
		{
			List.Clear();
		}

		public ControlHost this[int index]
		{
			get
			{
				return (ControlHost)List[index];
			}
			set
			{
				List[index] = value;
			}
		}


		// Serialization
		public virtual int getClassId()
		{
			return 41;
		}

		public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			writer.Write(List.Count);
			for (int i = 0; i < List.Count; ++i)
				ctx.saveReference(this, (ControlHost)List[i], i);
		}

		public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			int count = reader.ReadInt32();
			InnerList.Capacity = count;
			for (int i = 0; i < count; ++i)
			{
				InnerList.Add(null);
				ctx.loadReference(this);
			}
		}

		public virtual void setReference(int refId, IPersists obj)
		{
			InnerList[refId] = obj;
		}
	}

	/// <summary>
	/// Collection of groups.
	/// </summary>
	[Serializable]
	public class GroupCollection : CollectionBase, IPersists
	{
		public GroupCollection()
		{
		}

		public void Add(Group g)
		{
			List.Add(g);
		}

		public void Insert(int i, Group g)
		{
			List.Insert(i, g);
		}

		public Group GetAt(int i)
		{
			if(i >= Count)
				return null;

			return (Group)List[i];
		}

		public void Remove(Group g)
		{
			List.Remove(g);
		}

		public new void Clear()
		{
			List.Clear();
		}

		public Group this[int index]
		{
			get
			{
				return (Group)List[index];
			}
			set
			{
				List[index] = value;
			}
		}

		public virtual int getClassId()
		{
			return 14;
		}

		public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			writer.Write(List.Count);
			for (int i = 0; i < List.Count; ++i)
			{
				ctx.saveReference(this, (Group)List[i], i);
			}
		}

		public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			int count = reader.ReadInt32();
			InnerList.Capacity = count;
			for (int i = 0; i < count; ++i)
			{
				InnerList.Add(null);
				ctx.loadReference(this);
			}
		}

		public virtual void setReference(int refId, IPersists obj)
		{
			InnerList[refId] = obj;
		}
	}

	/// <summary>
	/// Collection of tables.
	/// </summary>
	public class TableCollection : CollectionBase, IPersists
	{
		public TableCollection()
		{
		}

		public TableCollection Clone()
		{
			TableCollection tc = new TableCollection();
			foreach (Table t in this)
				tc.Add(t);
			return tc;
		}

		public void Add(Table t)
		{
			List.Add(t);
		}

		public void Insert(int i, Table t)
		{
			List.Insert(i, t);
		}

		public Table GetAt(int i)
		{
			if(i >= Count)
				return null;

			return (Table)List[i];
		}

		public void Remove(Table t)
		{
			List.Remove(t);
		}

		public new void Clear()
		{
			List.Clear();
		}

		public Table this[int index]
		{
			get
			{
				return (Table)List[index];
			}
			set
			{
				List[index] = value;
			}
		}

		public virtual int getClassId()
		{
			return 16;
		}

		public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			writer.Write(List.Count);
			for (int i = 0; i < List.Count; ++i)
			{
				ctx.saveReference(this, (Table)List[i], i);
			}
		}

		public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			int count = reader.ReadInt32();
			InnerList.Capacity = count;
			for (int i = 0; i < count; ++i)
			{
				InnerList.Add(null);
				ctx.loadReference(this);
			}
		}

		public virtual void setReference(int refId, IPersists obj)
		{
			InnerList[refId] = obj;
		}
	}

	/// <summary>
	/// Collection of points.
	/// </summary>
	internal class PtCollection : CollectionBase
	{
		public PtCollection(int size)
		{
			InnerList.Clear();
			for (int i = 0; i < size; ++i)
				InnerList.Add(new Point(0, 0));
		}

		public PtCollection Clone()
		{
			PtCollection copy = new PtCollection(0);
			foreach (Point pt in InnerList)
				copy.Add(pt);

			return copy;
		}

		public void Add(Point p)
		{
			List.Add(p);
		}

		public void Insert(int i, Point p)
		{
			List.Insert(i, p);
		}

		public Point GetAt(int i)
		{
			if (i >= Count)
				return new Point(0, 0);

			return (Point)List[i];
		}

		public void SetAt(int i, Point pt)
		{
			List[i] = pt;
		}

		public void Remove(Point p)
		{
			List.Remove(p);
		}

		public new void Clear()
		{
			List.Clear();
		}

		public Point this[int index]
		{
			get
			{
				return (Point)List[index];
			}
			set
			{
				List[index] = value;
			}
		}
	}

	/// <summary>
	/// Collection of points.
	/// </summary>
	[Serializable]
	public class PointCollection : CollectionBase, IPersists
	{
		public PointCollection(int size)
		{
			InnerList.Clear();
			for (int i = 0; i < size; ++i)
				InnerList.Add(new PointF(0, 0));
		}

		public PointCollection Clone()
		{
			PointCollection copy = new PointCollection(0);
			foreach (PointF pt in InnerList)
				copy.Add(pt);

			return copy;
		}

		internal void Sort(IComparer comparer)
		{
			InnerList.Sort(comparer);
		}

		public void Add(PointF p)
		{
			List.Add(p);
		}

		public void AddRange(ICollection c)
		{
			InnerList.AddRange(c);
		}

		public void Insert(int i, PointF p)
		{
			List.Insert(i, p);
		}

		public PointF GetAt(int i)
		{
			if (i >= Count)
				return new PointF(0, 0);

			return (PointF)List[i];
		}

		public void SetAt(int i, PointF pt)
		{
			List[i] = pt;
		}

		public void Remove(PointF p)
		{
			List.Remove(p);
		}

		public new void Clear()
		{
			List.Clear();
		}

		public PointF this[int index]
		{
			get
			{
				return (PointF)List[index];
			}
			set
			{
				List[index] = value;
			}
		}

		public PointF[] getArray()
		{
			PointF[] pts = new PointF[this.Count];
			for (int i = 0; i < this.Count; ++i)
				pts[i] = this[i];
			return pts;
		}

		public virtual int getClassId()
		{
			return 15;
		}

		public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			writer.Write(List.Count);
			for (int i = 0; i < List.Count; ++i)
			{
				writer.Write((double)this[i].X);
				writer.Write((double)this[i].Y);
			}
		}

		public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			int count = reader.ReadInt32();
			InnerList.Capacity = count;
			for (int i = 0; i < count; ++i)
			{
				float x, y;
				x = (float)reader.ReadDouble();
				y = (float)reader.ReadDouble();
				InnerList.Add(new PointF(x, y));
			}
		}

		public virtual void setReference(int refId, IPersists obj)
		{
		}
	}

	/// <summary>
	/// Collection of chart objects.
	/// </summary>
	[Serializable]
	public class AnchorPointCollection : CollectionBase, IPersists
	{
		public AnchorPointCollection()
		{
		}

		public AnchorPointCollection(AnchorPoint[] array)
		{
			foreach (AnchorPoint anchor in array)
				List.Add(anchor);
		}

		public void Add(AnchorPoint a)
		{
			List.Add(a);
		}

		public void Insert(int i, AnchorPoint a)
		{
			List.Insert(i, a);
		}

		public AnchorPoint GetAt(int i)
		{
			if(i >= Count)
				return null;

			return (AnchorPoint)List[i];
		}

		public void Remove(AnchorPoint a)
		{
			List.Remove(a);
		}

		public new void Clear()
		{
			List.Clear();
		}

		public bool Contains(AnchorPoint obj)
		{
			return List.Contains(obj);
		}

		public AnchorPoint this[int index]
		{
			get
			{
				return (AnchorPoint)List[index];
			}
			set
			{
				List[index] = value;
			}
		}

		public virtual int getClassId()
		{
			return 37;
		}

		public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			writer.Write(List.Count);
			for (int i = 0; i < List.Count; ++i)
			{
				ctx.saveReference(this, (AnchorPoint)List[i], i);
			}
		}

		public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			int count = reader.ReadInt32();
			InnerList.Capacity = count;
			for (int i = 0; i < count; ++i)
			{
				InnerList.Add(null);
				ctx.loadReference(this);
			}
		}

		public virtual void setReference(int refId, IPersists obj)
		{
			InnerList[refId] = obj;
		}
	}

	/// <summary>
	/// Read-only collection of shapes.
	/// </summary>
	public class ShapeCollection : ICollection
	{
		internal ShapeCollection()
		{
			inner = new ArrayList();
		}

		internal ShapeCollection(ICollection array)
		{
			inner = new ArrayList(array);
		}

		internal ShapeCollection(ShapeTemplate[] array)
		{
			inner = new ArrayList();

			foreach (ShapeTemplate shape in array)
				inner.Add(shape);
		}

		public ShapeTemplate GetAt(int i)
		{
			if(i >= Count)
				return null;

			return (ShapeTemplate)inner[i];
		}

		public bool Contains(ShapeTemplate obj)
		{
			return inner.Contains(obj);
		}

		public ShapeTemplate this[int index]
		{
			get { return (ShapeTemplate)inner[index]; }
		}

		private ArrayList inner;

		#region ICollection Members

		public bool IsSynchronized
		{
			get { return inner.IsSynchronized; }
		}

		public int Count
		{
			get { return inner.Count; }
		}

		public void CopyTo(Array array, int index)
		{
			inner.CopyTo(array, index);
		}

		public object SyncRoot
		{
			get { return inner.SyncRoot; }
		}

		#endregion

		#region IEnumerable Members

		public IEnumerator GetEnumerator()
		{
			return inner.GetEnumerator();
		}

		#endregion
	}
}