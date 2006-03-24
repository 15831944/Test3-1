// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Drawing;
using MindFusion.FlowChartX.LayoutSystem;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Allows calling some static FlowChart.NET methods from JavaScript and VBScript.
	/// It seems the script languages do not allow calling class static methods,
	/// so a ScriptHelper instance property is exposed in FlowChart
	/// </summary>
	public class ScriptHelper
	{
		public ScriptHelper(FlowChart flowchart)
		{
			this.flowchart = flowchart;
		}

		public ShapeTemplate ShapeTemplateFromId(string shapeId)
		{
			return ShapeTemplate.FromId(shapeId);
		}

		public bool NodesIntersect(Node n1, Node n2)
		{
			return Node.nodesIntersect(n1, n2);
		}

		public Brush CreateSolidBrush(int alpha, int red, int green, int blue)
		{
			return new MindFusion.FlowChartX.SolidBrush(
				Color.FromArgb(alpha, red, green, blue));
		}

		public object CreateObject(string className)
		{
#if !FCNET_STD
			if (className == "GridLayout")
				return new MindFusion.FlowChartX.LayoutSystem.GridLayout();

			if (className == "LayeredLayout")
				return new MindFusion.FlowChartX.LayoutSystem.LayeredLayout();

			if (className == "SpringLayout")
				return new MindFusion.FlowChartX.LayoutSystem.SpringLayout();
#endif

			if (className == "TreeLayout")
				return new MindFusion.FlowChartX.LayoutSystem.TreeLayout();

			if (className == "HtmlBuilder")
				return new MindFusion.FlowChartX.HtmlBuilder(flowchart);

			if (className == "XmlReader")
				return new MindFusion.FlowChartX.Xml.XmlReader(flowchart);

			if (className == "XmlWriter")
				return new MindFusion.FlowChartX.Xml.XmlWriter(flowchart);

			return null;
		}

		public void ApplyLayout(object layout)
		{
			TreeLayout tl = layout as TreeLayout;
			if (tl != null)
			{
				tl.Arrange(flowchart);
				return;
			}

#if !FCNET_STD
			SpringLayout sl = layout as SpringLayout;
			if (sl != null)
			{
				sl.Arrange(flowchart);
				return;
			}

			LayeredLayout ll = layout as LayeredLayout;
			if (ll != null)
			{
				ll.Arrange(flowchart);
				return;
			}

			GridLayout gl = layout as GridLayout;
			if (gl != null)
			{
				gl.Arrange(flowchart);
				return;
			}
#endif
		}

		private FlowChart flowchart;
	}
}