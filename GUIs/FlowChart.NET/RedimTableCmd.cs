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
	/// undoable command to redim a table
	/// </summary>
	public sealed class RedimTableCmd : Command
	{
		public RedimTableCmd(Table table, string title) : base(title)
		{
			this.table = table;
		}

		internal void recordState()
		{
			initialStr = new TableStructure();
			table.saveStructure(initialStr);
		}

		protected internal override void Execute(bool undoEnabled)
		{
			resultStr = new TableStructure();
			table.saveStructure(resultStr);
			foreach (Command cmd in subCommands)
				if (!(cmd is RemoveItemCmd))
					cmd.Execute(undoEnabled);
		}

		protected internal override void Undo()
		{
			table.restoreStructure(initialStr);
			table.updateLinksIndices();
			for (int i = subCommands.Count - 1; i >= 0; i--)
				subCommands[i].Undo();

			cmdContext.Document.fireActionUndone(this);
		}

		protected internal override void Redo()
		{
			foreach (Command cmd in subCommands)
				if (cmd is RemoveItemCmd)
					cmd.Redo();
			table.restoreStructure(resultStr);
			table.updateLinksIndices();
			foreach (Command cmd in subCommands)
				if (!(cmd is RemoveItemCmd))
					cmd.Redo();

			cmdContext.Document.fireActionRedone(this);
		}

		internal override ChartObject getItem() { return table; }

		public Table Table
		{
			get { return table; }
		}

		private Table table;
		private TableStructure initialStr;
		private TableStructure resultStr;
	}
}