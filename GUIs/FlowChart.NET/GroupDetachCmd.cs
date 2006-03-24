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
	internal sealed class GroupDetachCmd : Command
	{
		internal GroupDetachCmd(Group group, Attachment attachment) : base("Detach from group")
		{
			this.group = group;
			this.attachment = attachment;
		}

		public override void Execute(bool undoEnabled)
		{
			group.removeAttachment(attachment);
		}

		public override void Undo()
		{
			group.addAttachment(attachment, true);

			cmdContext.Document.fireActionUndone(this);
		}

		public void Execute()
		{
			group.fcParent.UndoManager.executeCommand(this);
		}

		public Group Group
		{
			get { return group; }
		}

		public ChartObject Item
		{
			get { return attachment.node; }
		}

		private Group group;
		private Attachment attachment;
	}
}