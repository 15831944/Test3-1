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
	/// 
	/// </summary>
	public abstract class Command
	{
		public Command(string title)
		{
			subCommands = new CommandCollection();
			this.title = title;
		}

		public abstract void Execute(bool undoEnabled);
		public abstract void Undo();
		public virtual void Redo() { Execute(true); }

		internal void setContext(CommandContext cmdContext)
		{
			this.cmdContext = cmdContext;
			foreach (Command cmd in subCommands)
				cmd.setContext(cmdContext);
		}

		protected internal CommandContext cmdContext;

		public void AddSubCmd(Command cmd)
		{
			subCommands.Add(cmd);
		}

		internal virtual void freeRedoResources()
		{
			foreach (Command cmd in subCommands)
				cmd.freeRedoResources();
		}

		internal virtual void freeUndoResources()
		{
			foreach (Command cmd in subCommands)
				cmd.freeUndoResources();
		}

		internal virtual ChartObject getItem() { return null; }

		protected internal CommandCollection subCommands;

		public string Title
		{
			get { return title; }
			set
			{
				title = value;
			}
		}

		// might use this title in Undo/Redo menu item labels
		// e.g. if title == 'Create' -> 'Undo Create' or 'Redo Create'
		private string title;
	}
}