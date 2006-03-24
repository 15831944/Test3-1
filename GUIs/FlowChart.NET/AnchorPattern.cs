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
	/// Represents a set of anchor points which specify the
	/// exact locations where links are allowed to connect to nodes.
	/// </summary>
	public class AnchorPattern : ICloneable, IPersists
	{
		public AnchorPattern()
		{
			points = new AnchorPointCollection();
			regIndex = -1;
		}

		public AnchorPattern(AnchorPoint[] points)
		{
			this.points = new AnchorPointCollection(points);
			regIndex = -1;
		}

		public AnchorPattern(AnchorPointCollection points)
		{
			this.points = points;
			regIndex = -1;
		}

		private AnchorPattern(int idx, AnchorPointCollection points)
		{
			this.points = points;
			regIndex = idx;
		}

		public AnchorPointCollection Points
		{
			get { return points; }
			set { points = value; }
		}

		internal bool allowArrowsDir(bool outgoing, Node node)
		{
			if (points == null) return false;

			for (int i = 0; i < points.Count; ++i)
			{
				AnchorPoint pt = points[i];
				if (outgoing)
				{
					if (pt.AllowOutgoing && node.Parent.validateAnchor(null, true, node, i))
						return true;
				}
				else
				{
					if (pt.AllowIncoming && node.Parent.validateAnchor(null, false, node, i))
						return true;
				}
			}

			return false;
		}

		public static void Register(AnchorPattern pattern)
		{
			registeredPatterns.Add(pattern);
			pattern.regIndex = registeredPatterns.Count - 1;
		}

		internal static AnchorPattern getRegisteredPattern(int index)
		{
			if (index >= 0)
				return (AnchorPattern)registeredPatterns[index];

			if (index == -2) return decision2In2Out;
			if (index == -3) return decision1In3Out;
			if (index == -4) return leftInRightOut;
			if (index == -5) return topInBottomOut;

			return null;
		}

		private static ArrayList registeredPatterns = new ArrayList();

		internal int getRegIndex()
		{
			return regIndex;
		}

		internal bool isRegistered
		{
			get { return regIndex != -1; }
		}

		private AnchorPointCollection points;
		private int regIndex;

		public static AnchorPattern Decision2In2Out
		{
			get { return decision2In2Out; }
		}

		private static AnchorPattern decision2In2Out =
			new AnchorPattern(-2, new AnchorPointCollection(
				new AnchorPoint[] {
					new AnchorPoint(50, 0, true, false, MarkStyle.Circle, Color.Blue),
					new AnchorPoint(0, 50, true, false, MarkStyle.Circle, Color.Blue),
					new AnchorPoint(50, 100, false, true, MarkStyle.Rectangle, Color.Green),
					new AnchorPoint(100, 50, false, true, MarkStyle.Rectangle, Color.Red)
				}));

		public static AnchorPattern Decision1In3Out
		{
			get { return decision1In3Out; }
		}

		private static AnchorPattern decision1In3Out =
			new AnchorPattern(-3, new AnchorPointCollection(
				new AnchorPoint[] {
					new AnchorPoint(50, 0, true, false, MarkStyle.Circle, Color.Blue),
					new AnchorPoint(0, 50, false, true, MarkStyle.Rectangle, Color.Red),
					new AnchorPoint(50, 100, false, true, MarkStyle.Rectangle, Color.Green),
					new AnchorPoint(100, 50, false, true, MarkStyle.Rectangle, Color.Red)
				}));

		public static AnchorPattern LeftInRightOut
		{
			get { return leftInRightOut; }
		}

		private static AnchorPattern leftInRightOut =
			new AnchorPattern(-4, new AnchorPointCollection(
				new AnchorPoint[] {
					new AnchorPoint(0, 50, true, false, MarkStyle.Circle, Color.Blue),
					new AnchorPoint(100, 50, false, true, MarkStyle.Rectangle, Color.Green)
				}));

		public static AnchorPattern TopInBottomOut
		{
			get { return topInBottomOut; }
		}

		private static AnchorPattern topInBottomOut =
			new AnchorPattern(-5, new AnchorPointCollection(
				new AnchorPoint[] {
						new AnchorPoint(50, 0, true, false, MarkStyle.Circle, Color.Blue),
						new AnchorPoint(50, 100, false, true, MarkStyle.Rectangle, Color.Green)
					}));

		public object Clone()
		{
			AnchorPattern clone = new AnchorPattern();

			foreach (AnchorPoint ap in this.points)
				clone.points.Add((AnchorPoint)ap.Clone());

			return clone;
		}

		public virtual int getClassId()
		{
			return 38;
		}

		public virtual void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			points.saveTo(writer, ctx);
		}

		public virtual void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			points.Clear();
			points.loadFrom(reader, ctx);
		}

		public virtual void setReference(int refId, IPersists obj)
		{
		}
	}
}
