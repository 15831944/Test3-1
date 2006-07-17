// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;

namespace MindFusion.FlowChartX.Commands
{
	/// <summary>
	/// undoable command to add new item to a diagram
	/// </summary>
	public sealed class AddGroupCmd : Command
	{
		public AddGroupCmd(ChartObject item, Group group) : base("Group")
		{
			this.item = item;
			this.group = group;
		}

		protected internal override void Execute(bool undoEnabled)
		{
			cmdContext.Document.Groups.Add(group);
		}

		protected internal override void Undo()
		{
			group.onDelete();
			cmdContext.Document.Groups.Remove(group);

			cmdContext.Document.fireActionUndone(this);
		}

		protected internal override void Redo()
		{
			group.setMainObject(item);
			cmdContext.Document.Groups.Add(group);

			cmdContext.Document.fireActionRedone(this);
		}

		public Group Group
		{
			get { return group; }
		}

		public ChartObject Item
		{
			get { return item; }
		}

		private ChartObject item;
		private Group group;
	}
}