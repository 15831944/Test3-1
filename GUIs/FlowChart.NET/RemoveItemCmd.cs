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
	public sealed class RemoveItemCmd : Command
	{
		public RemoveItemCmd(ChartObject item) : base("Delete")
		{
			this.item = item;
			this.oldSelState = null;
			this.newSelState = null;
			this.zIndex = 0;
		}

		public void Execute()
		{
			item.flowChart.UndoManager.executeCommand(this);
		}

		protected internal override void Execute(bool undoEnabled)
		{
			if (undoEnabled)
				oldSelState = item.flowChart.Selection.saveState();

			zIndex = item.ZIndex;
			cmdContext.Document.removeItem(item);

			if (undoEnabled)
				newSelState = item.flowChart.Selection.saveState();
		}

		protected internal override void Undo()
		{
			cmdContext.Document.addItem(item);
			
			for (int i = subCommands.Count - 1; i >= 0; i--)
				subCommands[i].Undo();
			item.ZIndex = zIndex;

			if (oldSelState != null)
				item.flowChart.Selection.restoreState(oldSelState);

			cmdContext.Document.fireActionUndone(this);
		}

		protected internal override void Redo()
		{
			foreach (Command subCmd in subCommands)
				subCmd.Redo();
			cmdContext.Document.removeItem(item);

			if (newSelState != null)
				item.flowChart.Selection.restoreState(newSelState);

			cmdContext.Document.fireActionRedone(this);
		}

		internal override void freeUndoResources()
		{
			base.freeUndoResources();
			item.freeResources();
		}

		internal override ChartObject getItem() { return item; }

		public ChartObject Item
		{
			get { return item; }
		}

		private ChartObject item;
		private SelectionState oldSelState;
		private SelectionState newSelState;
		private int zIndex;
	}
}