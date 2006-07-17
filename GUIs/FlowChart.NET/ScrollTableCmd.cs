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
	public sealed class ScrollTableCmd : Command
	{
		public ScrollTableCmd(Table table) : base("Scroll table")
		{
			this.table = table;
			oldRow = table.CurrScrollRow;

			ArrowCollection ac = table.GetAllArrows();
			foreach (Arrow arrow in ac)
				AddSubCmd(new ModifyItemCmd(arrow));
		}

		protected internal override void Execute(bool undoEnabled)
		{
			newRow = table.CurrScrollRow;

			foreach (Command cmd in subCommands)
				cmd.Execute(undoEnabled);
		}

		protected internal override void Undo()
		{
			table.setCurrScrollRow(oldRow);

			for (int i = subCommands.Count - 1; i >= 0; i--)
				subCommands[i].Undo();

			cmdContext.Document.fireActionUndone(this);
		}

		protected internal override void Redo()
		{
			table.setCurrScrollRow(newRow);

			foreach (Command cmd in subCommands)
				cmd.Redo();

			cmdContext.Document.fireActionRedone(this);
		}

		public Table Table
		{
			get { return table; }
		}

		private Table table;
		int oldRow;
		int newRow;
	}
}