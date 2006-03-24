// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;

namespace MindFusion.FlowChartX.Visitors
{
	/// <summary>
	/// 
	/// </summary>
	
	internal delegate void VisitOperation(ChartObject obj);
	internal delegate void VisitOperationIntr(ChartObject obj, InteractionState ist);

	internal class MethodCallVisitor : CollectionVisitor
	{
		public MethodCallVisitor(VisitOperation opr)
		{
			operation = opr;
			operationIntr = null;
			ist = null;
		}

		public MethodCallVisitor(VisitOperationIntr opr, InteractionState ist)
		{
			operation = null;
			operationIntr = opr;
			this.ist = ist;
		}

		internal override void accept(ChartObject obj)
		{
			if (operation != null)
				operation(obj);

			if (operationIntr != null)
				operationIntr(obj, ist);
		}

		private VisitOperation operation;
		private VisitOperationIntr operationIntr;
		private InteractionState ist;
	}
}
