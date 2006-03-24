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
	/// undoable command to ser table rows as headers
	/// </summary>
	public sealed class ExpandHeaderRowCmd : Command
	{
		public ExpandHeaderRowCmd(Table table, int rowIndex) :
			base("Expand header row")
		{
			this.table = table;
			this.rowIndex = rowIndex;
			this.oldExpanded = table.Rows[rowIndex].Expanded;
			newExpanded = false;

			ArrowCollection ac = table.GetAllArrows();
			foreach (Arrow arrow in ac)
				AddSubCmd(new ModifyItemCmd(arrow));
		}

		public override void Execute(bool undoEnabled)
		{
			newExpanded = table.Rows[rowIndex].Expanded;

			foreach (Command cmd in subCommands)
				cmd.Execute(undoEnabled);
		}

		public override void Undo()
		{
			table.Rows[rowIndex].setExpanded(oldExpanded, rowIndex);

			for (int i = subCommands.Count - 1; i >= 0; i--)
				subCommands[i].Undo();

			cmdContext.Document.fireActionUndone(this);
		}

		public override void Redo()
		{
			table.Rows[rowIndex].setExpanded(newExpanded, rowIndex);

			foreach (Command cmd in subCommands)
				cmd.Redo();

			cmdContext.Document.fireActionRedone(this);
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
		private bool oldExpanded;
		private bool newExpanded;
	}
}