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
	/// undoable command to programmatically change item properties
	/// </summary>
	public sealed class ChangeItemCmd : Command
	{
		public ChangeItemCmd(ChartObject item, string title) : base(title)
		{
			this.item = item;
			initialProps = item.createProperties();
			item.saveProperties(initialProps);

			item.fcParent.UndoManager.onChangeItem(item, this);
		}

		public void Execute()
		{
			item.fcParent.UndoManager.executeCommand(this);
		}

		public override void Execute(bool undoEnabled)
		{
			foreach (Command cmd in subCommands)
				cmd.Execute(undoEnabled);
			resultProps = item.createProperties();
			item.saveProperties(resultProps);
		}

		public override void Undo()
		{
			for (int i = subCommands.Count - 1; i >= 0; i--)
				subCommands[i].Undo();
			item.restoreProperties(initialProps);

			item.fcParent.fireActionUndone(this);
		}

		public override void Redo()
		{
			item.restoreProperties(resultProps);
			foreach (Command cmd in subCommands)
				cmd.Redo();

			item.fcParent.fireActionRedone(this);
		}

		public ChartObject Item
		{
			get { return item; }
		}

		private ChartObject item;
		ItemProperties initialProps;
		ItemProperties resultProps;
	}
}