using System;
using System.Windows.Forms;
using MindFusion.FlowChartX;


namespace MindFusion.Diagramming.Import
{
	public class ArrowImportedArgs : EventArgs
	{
		public ArrowImportedArgs(Arrow arrow, string shapeName)
		{
			this.arrow = arrow;
			this.shapeName = shapeName;
		}

		public Arrow Arrow
		{
			get { return arrow; }
		}

		public string ShapeName
		{
			get { return shapeName; }
		}

		private Arrow arrow;
		private string shapeName;
	}


	public class BoxImportedArgs : EventArgs
	{
		public BoxImportedArgs(Box box, string shapeName)
		{
			this.box = box;
			this.shapeName = shapeName;
		}

		public Box Box
		{
			get { return box; }
		}

		public string ShapeName
		{
			get { return shapeName; }
		}

		private Box box;
		private string shapeName;
	}


	public class GroupImportedArgs : EventArgs
	{
		public GroupImportedArgs(Group group)
		{
			this.group = group;
		}

		public Group Group
		{
			get { return group; }
		}

		private Group group;
	}


	public delegate void ArrowImportedEventHandler(object sender, ArrowImportedArgs e);
	public delegate void BoxImportedEventHandler(object sender, BoxImportedArgs e);
	public delegate void GroupImportedEventHandler(object sender, GroupImportedArgs e);
}