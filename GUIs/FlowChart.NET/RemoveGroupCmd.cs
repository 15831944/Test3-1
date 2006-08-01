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
	public sealed class RemoveGroupCmd : Command
	{
		public RemoveGroupCmd(ChartObject item, Group group) : base("Ungroup")
		{
			this.item = item;
			this.group = group;
		}

		protected internal override void Execute(bool undoEnabled)
		{
			group.destroySelf(false);
		}

		protected internal override void Undo()
		{
			group.setMainObject(item);
			cmdContext.Document.Groups.Add(group);

			for (int i = subCommands.Count - 1; i >= 0; i--)
				subCommands[i].Undo();

			cmdContext.Document.fireActionUndone(this);
		}

		protected internal override void Redo()
		{
			foreach (Command cmd in subCommands)
				cmd.Undo();
			group.destroySelf(true);

			cmdContext.Document.fireActionRedone(this);
		}

		public void Execute()
		{
			group.flowChart.UndoManager.executeCommand(this);
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