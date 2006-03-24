// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;

namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Stores the state of multiple-selection. Used in undo/redo of some commands.
	/// </summary>
	internal class SelectionState
	{
		public ChartObjectCollection SelectedItems
		{
			get { return selectedItems; }
			set { selectedItems = value; }
		}

		public BoxCollection SelectedBoxes
		{
			get { return selectedBoxes; }
			set { selectedBoxes = value; }
		}

		public TableCollection SelectedTables
		{
			get { return selectedTables; }
			set { selectedTables = value; }
		}

		public ArrowCollection SelectedArrows
		{
			get { return selectedArrows; }
			set { selectedArrows = value; }
		}

		public ControlHostCollection SelectedHosts
		{
			get { return selectedHosts; }
			set { selectedHosts = value; }
		}

		public ChartObject ActiveItem
		{
			get { return activeItem; }
			set { activeItem = value; }
		}

		private ChartObjectCollection selectedItems;
		private BoxCollection selectedBoxes;
		private TableCollection selectedTables;
		private ArrowCollection selectedArrows;
		private ControlHostCollection selectedHosts;
		private ChartObject activeItem;
	}
}
