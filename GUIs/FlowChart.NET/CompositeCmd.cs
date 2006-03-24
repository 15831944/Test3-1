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
	public class CompositeCmd : Command
	{
		public CompositeCmd(FlowChart flowChart, string title) :
			this(flowChart, title, false)
		{
		}

		public CompositeCmd(FlowChart flowChart, string title, bool saveZOrder) :
			base(title)
		{
			this.flowChart = flowChart;
			zOrder = null;

			if (saveZOrder)
				zOrder = flowChart.Objects.Clone();
		}

		public void Execute()
		{
			flowChart.UndoManager.executeCommand(this);
		}

		public override void Execute(bool undoEnabled)
		{
			foreach (Command cmd in subCommands)
				cmd.Execute(undoEnabled);
		}

		public override void Undo()
		{
			for (int i = subCommands.Count - 1; i >= 0; i--)
				subCommands[i].Undo();

			if (zOrder != null && zOrder.Count == flowChart.Objects.Count)
				flowChart.setZOrder(zOrder.Clone());

			flowChart.fireActionUndone(this);
		}

		public override void Redo()
		{
			foreach (Command cmd in subCommands)
				cmd.Redo();

			flowChart.fireActionRedone(this);
		}

		public CommandCollection SubCommands
		{
			get { return subCommands; }
		}

		private FlowChart flowChart;
		private ChartObjectCollection zOrder;
	}
}