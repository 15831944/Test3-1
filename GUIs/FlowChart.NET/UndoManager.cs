// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using MindFusion.FlowChartX.Commands;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Kind of mediator to synchronize and/or embed undoable commands
	/// across command sequences and command-composition hierarchy
	/// </summary>
	public class UndoManager
	{
		internal UndoManager(FlowChart document)
		{
			this.document = document;

			defaultCmdContext = new CommandContext(document);
			history = new CommandHistory();

			resetContext();

			// disable undo by default, it might be too expensive
			// in memory and CPU cycles
			undoEnabled = false;
		}

		internal void resetContext()
		{
			currCommand = null;
			currModifyCmd = null;
			currRedimCmd = null;
			currChangeTableCmd = null;
			currRouteCmd = null;
			currLayoutCmd = null;
			currExpandCmd = null;
			currCompositeCmd = null;
			currProgrModifyCmd = null;
		}

		public bool UndoEnabled
		{
			get { return undoEnabled; }
			set
			{
				if (undoEnabled != value)
				{
					history.Clear();
					undoEnabled = value;
					resetContext();
				}
			}
		}

		internal void enable(bool enabled) { undoEnabled = enabled; }

		public void Undo()
		{
			if (!undoEnabled) return;

			history.undo();
			document.Invalidate();
		}

		public void Redo()
		{
			if (!undoEnabled) return;

			history.redo();
			document.Invalidate();
		}

		public CommandHistory History
		{
			get { return history; }
		}

		public CompositeCmd StartComposite(string title)
		{
			return StartComposite(title, false);
		}

		public CompositeCmd StartComposite(string title, bool saveZOrder)
		{
			if (!undoEnabled) return null;

			if (currCompositeCmd != null)
				return currCompositeCmd;

			currCompositeCmd = new CompositeCmd(document, title, saveZOrder);
			currCompositeCmd.setContext(defaultCmdContext);
			return currCompositeCmd;
		}

		internal void executeCommand(Command cmd)
		{
			if (cmd == null) return;

			// just execute the action if no later undo is needed
			if (!undoEnabled)
			{
				cmd.setContext(defaultCmdContext);
				cmd.Execute(false);
				cmd.freeUndoResources();
				return;
			}

			// if executing the ChangeItem comand on table, clear last reference
			if (cmd == currChangeTableCmd)
				currChangeTableCmd = null;

			// if redimensioning a table, handle arrow removals
			RemoveItemCmd removeItemCmd = cmd as RemoveItemCmd;
			if (currRedimCmd != null && cmd != currRedimCmd &&
				removeItemCmd != null)
			{
				if (removeItemCmd.getItem() is Arrow)
				{
					Arrow arr = (Arrow)removeItemCmd.getItem();
					if (arr.Origin == currRedimCmd.getItem() ||
						arr.Destination == currRedimCmd.getItem())
					{
						cmd.setContext(defaultCmdContext);
						cmd.Execute(true);
						currRedimCmd.AddSubCmd(cmd);
						return;
					}
				}
			}

			// handle current composite command
			if (currCompositeCmd != null)
			{
				if (cmd == currCompositeCmd)
				{
					history.saveCommand(currCompositeCmd, document);
					currCompositeCmd = null;
					return;
				}
				else
				{
					cmd.setContext(defaultCmdContext);
					cmd.Execute(true);
					currCompositeCmd.AddSubCmd(cmd);
					return;
				}
			}

			// create a single composite command for modify or remove commands
			// that lead to modification or removal of more items
			if (currCommand == null)
			{
				currCommand = cmd;
				cmd.setContext(defaultCmdContext);
				cmd.Execute(true);
				history.saveCommand(cmd, document);
				currCommand = null;
			}
			else
			{
				cmd.setContext(defaultCmdContext);
				cmd.Execute(true);
				currCommand.AddSubCmd(cmd);
			}
		}


		// modifications might be compositted, e.g. dragging a box would
		// probably modify the arrows attached to it

		internal void onModifyItem(ChartObject item)
		{
			if (!undoEnabled) return;

			// during layout operations all items already have a ModifyItemCmd created
			if (currLayoutCmd != null)
				return;

			// might be called for group children while in BoundginRect.set
			if (currProgrModifyCmd != null)
			{
				currProgrModifyCmd.AddSubCmd(new ModifyItemCmd(item));
			}
			else

			// or when users start moving an item around
			if (currModifyCmd == null)
			{
				currModifyCmd = new ModifyItemCmd(item);
			}
			else

			// or when dependent items positions are updated while
			// moving another item around
			{
				currModifyCmd.AddSubCmd(new ModifyItemCmd(item));
			}
		}

		internal void onCompleteModify()
		{
			if (!undoEnabled) return;

			executeCommand(currModifyCmd);
			currModifyCmd = null;
		}

		internal void onCancelModify()
		{
			if (!undoEnabled) return;

			currModifyCmd = null;
		}


		// redimensioning a table could be either stand-alone or part of
		// ChangeCommand when changing programmatically table properties;
		// redim command might lead to following additional changes:
		//   arrows might be removed if they link to the removed rows
		//   arrows might be scrolled up/down -> ModifyItemCmd

		internal void onRedimTable(Table table)
		{
			if (!undoEnabled) return;

			currRedimCmd = new RedimTableCmd(table, "Change table");
		}

		internal void onRedimArrowsDeleted(Table table)
		{
			if (!undoEnabled) return;

			// record table structure after arrows are deleted
			currRedimCmd.recordState();

			// remaining arrows will be possibly scrolled up or down
			// record their state here; better optimize this to include
			// just affected arrows (TODO)
			ArrowCollection ac = table.GetAllArrows();
			foreach (Arrow arrow in ac)
				currRedimCmd.AddSubCmd(new ModifyItemCmd(arrow));
		}

		internal void onCompleteRedim()
		{
			if (!undoEnabled) return;

			if (currChangeTableCmd == null)
				executeCommand(currRedimCmd);
			else
				currChangeTableCmd.AddSubCmd(currRedimCmd);
			currRedimCmd = null;
		}


		// this is needed for redimensioning tables too

		internal void onChangeItem(ChartObject item, ChangeItemCmd cmd)
		{
			if (!undoEnabled) return;

			if (item is Table)
				currChangeTableCmd = cmd;
		}


		// arrow routing - just create a composite of arrow modifications

		internal void onStartRoute()
		{
			if (!undoEnabled) return;
			currRouteCmd = new CompositeCmd(document, "Route");
		}

		internal void onRouteArrow(Arrow arrow)
		{
			if (!undoEnabled) return;
			currRouteCmd.AddSubCmd(new ModifyItemCmd(arrow));
		}

		internal void onEndRoute()
		{
			if (!undoEnabled) return;

			if (currRouteCmd.SubCommands.Count > 0)
			{
				if (currModifyCmd == null)
					executeCommand(currRouteCmd);
				else
					currModifyCmd.AddSubCmd(currRouteCmd);
			}
			
			currRouteCmd = null;
		}


		// save positions of all items on layout

		internal void onStartLayout(string title)
		{
			if (!undoEnabled) return;

			currLayoutCmd = new CompositeCmd(document, title);
			foreach (ChartObject item in document.Objects)
				currLayoutCmd.AddSubCmd(new ModifyItemCmd(item));
		}

		internal void onEndLayout()
		{
			if (!undoEnabled) return;

			executeCommand(currLayoutCmd);
			currLayoutCmd = null;
		}


		// tree collapse or expand

		internal void onStartExpand()
		{
			if (!undoEnabled) return;

			currExpandCmd = new CompositeCmd(document, "Expand");
		}

		internal void onExpandItem(ChartObject item)
		{
			if (!undoEnabled) return;

			currExpandCmd.AddSubCmd(new ExpandItemCmd(item));
		}

		internal void onEndExpand()
		{
			if (!undoEnabled) return;

			executeCommand(currExpandCmd);
			currExpandCmd = null;
		}


		// programmatic position changes

		internal void onStartPlacementChange(ChartObject item)
		{
			if (!undoEnabled) return;

			currProgrModifyCmd = new ModifyItemCmd(item);
		}

		internal void onPlacementChange(ChartObject item)
		{
			if (!undoEnabled) return;

			if (currProgrModifyCmd != null)
			{
				currProgrModifyCmd.AddSubCmd(new ModifyItemCmd(item));
			}
		}

		internal void onEndPlacementChange()
		{
			if (!undoEnabled) return;

			executeCommand(currProgrModifyCmd);
			currProgrModifyCmd = null;
		}

		// save commands here for later undo or redo
		private CommandHistory history;

		// flag indicating whether undo is enanled
		bool undoEnabled;

		// this is the flowchart
		private FlowChart document;

		// the commands can get the flowchart and other contextual
		// information through this object
		private CommandContext defaultCmdContext;

		// commands might be executed in the context of other commands
		// the variables before form the current context
		private Command currCommand;
		private ModifyItemCmd currModifyCmd;
		private RedimTableCmd currRedimCmd;
		private ChangeItemCmd currChangeTableCmd;
		private CompositeCmd currRouteCmd;
		private CompositeCmd currLayoutCmd;
		private CompositeCmd currExpandCmd;
		private CompositeCmd currCompositeCmd;
		private ModifyItemCmd currProgrModifyCmd;
	}
}