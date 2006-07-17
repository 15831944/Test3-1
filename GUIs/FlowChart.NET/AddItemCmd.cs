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
	public sealed class AddItemCmd : Command
	{
		public AddItemCmd(ChartObject item) : base("Create")
		{
			this.item = item;
			this.oldSelState = null;
			this.newSelState = null;
			this.zIndex = 0;
		}

		public void Execute()
		{
			item.fcParent.UndoManager.executeCommand(this);
		}

		protected internal override void Execute(bool undoEnabled)
		{
			if (undoEnabled)
				oldSelState = item.fcParent.Selection.saveState();

			cmdContext.Document.addItem(item);
			zIndex = item.ZIndex;

			if (item is Node)
				cmdContext.Document.routeAllArrows(item);
		}

		protected internal override void Undo()
		{
			for (int i = subCommands.Count - 1; i >= 0; i--)
				subCommands[i].Undo();
			cmdContext.Document.removeItem(item);

			if (oldSelState != null)
				item.fcParent.Selection.restoreState(oldSelState);

			cmdContext.Document.fireActionUndone(this);
		}

		protected internal override void Redo()
		{
			cmdContext.Document.addItem(item);

			foreach (Command cmd in subCommands)
				cmd.Redo();
			item.ZIndex = zIndex;

			if (newSelState != null)
				item.fcParent.Selection.restoreState(newSelState);

			cmdContext.Document.fireActionRedone(this);
		}

		internal override void freeRedoResources()
		{
			base.freeRedoResources();
			item.freeResources();
		}

		internal void saveSelState()
		{
			newSelState = item.fcParent.Selection.saveState();
		}

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