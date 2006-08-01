// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.IO;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Summary description for NodeConstraints.
	/// </summary>
	public class NodeConstraints : IPersists, ICloneable
	{
		public NodeConstraints()
		{
			moveDirection = DirectionConstraint.None;
			minWidth = 0;
			minHeight = 0;
			maxWidth = 0;
			maxHeight = 0;
			boolValues = 0;
		}

		public object Clone()
		{
			NodeConstraints clone = new NodeConstraints();
			clone.moveDirection = this.moveDirection;
			clone.minWidth = this.minWidth;
			clone.minHeight = this.minHeight;
			clone.maxWidth = this.maxWidth;
			clone.maxHeight = this.maxHeight;
			clone.boolValues = this.boolValues;
			return clone;
		}

		internal bool isNodeConstrained()
		{
			if (moveDirection != DirectionConstraint.None ||
				minWidth != 0 ||
				minHeight != 0 ||
				maxWidth != 0 ||
				maxHeight != 0 ||
				boolValues != 0)
				return true;

			return false;
		}

		public DirectionConstraint MoveDirection
		{
			get { return moveDirection; }
			set { moveDirection = value; }
		}

		public float MinWidth
		{
			get { return minWidth; }
			set { minWidth = value; }
		}

		public float MinHeight
		{
			get { return minHeight; }
			set { minHeight = value; }
		}

		public float MaxWidth
		{
			get { return maxWidth; }
			set { maxWidth = value; }
		}

		public float MaxHeight
		{
			get { return maxHeight; }
			set { maxHeight = value; }
		}

		public bool KeepInsideParent
		{
			get { return (boolValues & fKeepInsideParent) != 0; }
			set
			{
				if (value)
					boolValues |= fKeepInsideParent;
				else
					if (KeepInsideParent)
						boolValues -= fKeepInsideParent;
			}
		}

		public bool KeepRatio
		{
			get { return (boolValues & fKeepRatio) != 0; }
			set
			{
				if (value)
					boolValues |= fKeepRatio;
				else
					if (KeepRatio)
						boolValues -= fKeepRatio;
			}
		}

		internal static int Count
		{
			get { return 7; }
		}

		public int getClassId()
		{
			return 43;
		}

		public void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			// new in save format 17
			writer.Write((int)moveDirection);
			writer.Write(minWidth);
			writer.Write(minHeight);
			writer.Write(maxWidth);
			writer.Write(maxHeight);

			// new in file format 29
			writer.Write(boolValues);
		}

		public void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			// new in save format 17
			moveDirection = (DirectionConstraint)reader.ReadInt32();
			minWidth = reader.ReadSingle();
			minHeight = reader.ReadSingle();
			maxWidth = reader.ReadSingle();
			maxHeight = reader.ReadSingle();

			if (ctx.FileVersion > 28)
			{
				// new in file format 29
				boolValues = reader.ReadUInt16();
			}
		}

		public void setReference(int refId, IPersists obj)
		{
		}

		private DirectionConstraint moveDirection;
		private float minWidth;
		private float minHeight;
		private float maxWidth;
		private float maxHeight;
		private ushort boolValues;

		private const ushort fKeepInsideParent = 1;
		private const ushort fKeepRatio = 2;
	}
}