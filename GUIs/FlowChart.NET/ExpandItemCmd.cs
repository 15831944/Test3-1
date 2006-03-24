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
	public sealed class ExpandItemCmd : Command
	{
		public ExpandItemCmd(ChartObject item) : base("Expand")
		{
			this.item = item;
			intlVisible = item.Visible;
			intlExpanded = item.isExpanded();
		}

		public override void Execute(bool undoEnabled)
		{
			rsltVisible = item.Visible;
			rsltExpanded = item.isExpanded();
		}

		public override void Undo()
		{
			item.Visible = intlVisible;
			item.setExpanded(intlExpanded);

			cmdContext.Document.fireActionUndone(this);
		}

		public override void Redo()
		{
			item.Visible = rsltVisible;
			item.setExpanded(rsltExpanded);

			cmdContext.Document.fireActionRedone(this);
		}

		public ChartObject Item
		{
			get { return item; }
		}

		private ChartObject item;
		private bool intlVisible;
		private bool intlExpanded;
		private bool rsltVisible;
		private bool rsltExpanded;
	}
}