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
	public sealed class ModifyItemCmd : Command
	{
		public ModifyItemCmd(ChartObject item) : base("Modify")
		{
			this.item = item;
			initialState = item.createState();
			item.saveState(initialState);
		}

		protected internal override void Execute(bool undoEnabled)
		{
			resultState = item.createState();
			item.saveState(resultState);
			foreach (Command subCmd in subCommands)
				subCmd.Execute(undoEnabled);
		}

		protected internal override void Undo()
		{
			for (int i = subCommands.Count - 1; i >= 0; i--)
				subCommands[i].Undo();
			item.restoreState(initialState);

			item.flowChart.fireActionUndone(this);
		}

		protected internal override void Redo()
		{
			item.restoreState(resultState);
			foreach (Command subCmd in subCommands)
				subCmd.Redo();

			item.flowChart.fireActionRedone(this);
		}

		public ChartObject Item
		{
			get { return item; }
		}

		private ChartObject item;
		ItemState initialState;
		ItemState resultState;
	}
}