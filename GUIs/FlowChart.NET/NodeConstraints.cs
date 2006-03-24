using System;
using System.IO;

namespace MindFusion.FlowChartX
{
	public enum DirectionConstraint
	{
		None,
		Horizontal,
		Vertical
	}

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
		}

		public object Clone()
		{
			NodeConstraints clone = new NodeConstraints();
			clone.moveDirection = this.moveDirection;
			clone.minWidth = this.minWidth;
			clone.minHeight = this.minHeight;
			clone.maxWidth = this.maxWidth;
			clone.maxHeight = this.maxHeight;
			return clone;
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

		internal static int Count { get { return 5; } }

		private DirectionConstraint moveDirection;
		private float minWidth;
		private float minHeight;
		private float maxWidth;
		private float maxHeight;

		// Implement IPersist
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
		}

		public void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			// new in save format 17
			moveDirection = (DirectionConstraint)reader.ReadInt32();
			minWidth = reader.ReadSingle();
			minHeight = reader.ReadSingle();
			maxWidth = reader.ReadSingle();
			maxHeight = reader.ReadSingle();
		}

		public void setReference(int refId, IPersists obj)
		{
		}
	}
}