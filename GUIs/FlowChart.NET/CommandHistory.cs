// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Collections;
using MindFusion.FlowChartX.Commands;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// 
	/// </summary>
	public class CommandHistory
	{
		internal CommandHistory()
		{
			maxDepth = 10;
			commands = new CommandCollection(maxDepth);
			commandList = new CommandList(commands);
			currCmd = -1;
		}

		internal void saveCommand(Command cmd, FlowChart flowChart)
		{
			if (maxDepth == 0 || !flowChart.confirmRecordAction(cmd))
			{
				cmd.freeUndoResources();
				return;
			}

			if (currCmd < commands.Count - 1)
			{
				for (int i = currCmd + 1; i < commands.Count; ++i)
					commands[i].freeRedoResources();
				commands.RemoveRange(currCmd + 1, commands.Count - currCmd - 1);
			}

			commands.Add(cmd);
			currCmd++;

			if (commands.Count > maxDepth)
			{
				commands[0].freeUndoResources();
				commands.RemoveAt(0);
				currCmd--;
			}

			flowChart.fireActionRecorded(cmd);
		}

		internal void undo()
		{
			if (currCmd >= 0)
			{
				Command cmd = commands[currCmd--];
				cmd.Undo();
			}
		}

		internal void redo()
		{
			if (currCmd < commands.Count - 1)
			{
				Command cmd = commands[++currCmd];
				cmd.Redo();
			}
		}

		public void Clear()
		{
			for (int i = 0; i <= currCmd; ++i)
				commands[i].freeUndoResources();
			for (int j = currCmd + 1; j < commands.Count; ++j)
				commands[j].freeRedoResources();

			commands.Clear();
			currCmd = -1;
		}

		public Command NextUndo
		{
			get
			{
				if (currCmd >= 0)
					return commands[currCmd];
				return null;
			}
		}

		public Command NextRedo
		{
			get
			{
				if (currCmd < commands.Count - 1)
					return commands[currCmd + 1];
				return null;
			}
		}

		public int Capacity
		{
			get { return maxDepth; }
			set
			{
				if (maxDepth != value && value > 0)
				{
					Clear();
					maxDepth = value;
				}
			}
		}

		public CommandList Commands
		{
			get { return commandList; }
		}

		CommandCollection commands;
		CommandList commandList;
		int maxDepth;
		int currCmd;
	}
}