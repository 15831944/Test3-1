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
	/// Summary description for TableStructure.
	/// </summary>
	internal class TableStructure
	{
		public TableStructure()
		{
			rows = null;
			cols = null;
			cells = null;
		}

		internal Table.RowCollection rows;
		internal Table.ColumnCollection cols;
		internal Table.CellCollection cells;

		internal int columnsCount;
		internal int rowsCount;
	}
}
