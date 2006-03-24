// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.CodeDom;
using System.ComponentModel.Design.Serialization;

namespace MindFusion.FlowChartX.Design.Serialization
{
	/// <summary>
	/// Performs design-time code generation for
	/// MindFusion.FlowChartX.Brush properties of the FlowChart.
	/// </summary>
	internal class FlowChartSerializer : CodeDomSerializer
	{
		public FlowChartSerializer()
		{
		}

		public override object Serialize(
			IDesignerSerializationManager manager, object value)
		{
			CodeDomSerializer baseSerializer = manager.GetSerializer(
				typeof(FlowChart).BaseType, typeof(CodeDomSerializer)) as CodeDomSerializer;

			object codeObject = baseSerializer.Serialize(manager, value);

			if (codeObject is CodeStatementCollection)
			{
				CodeStatementCollection code =
					codeObject as CodeStatementCollection;
				FlowChart chart = value as FlowChart;

				// Brushes
				SerializeBrush(code, "BackBrush", chart, chart.BackBrush);
				SerializeBrush(code, "BoxBrush", chart, chart.BoxBrush);
				SerializeBrush(code, "TableBrush", chart, chart.TableBrush);
				SerializeBrush(code, "ArrowBrush", chart, chart.ArrowBrush);

				// Pens
				SerializePen(code, "BoxPen", chart, chart.BoxPen);
				SerializePen(code, "TablePen", chart, chart.TablePen);
				SerializePen(code, "ArrowPen", chart, chart.ArrowPen);

				// Text alignment
				SerializeTextFormat(code, "TextFormat", chart, chart.TextFormat);
			}

			return codeObject;
		}

		public override object Deserialize(
			IDesignerSerializationManager manager, object codeObject)
		{
			// Simply invoke the base class' method

			CodeDomSerializer baseSerializer = manager.GetSerializer(
				typeof(FlowChart).BaseType, typeof(CodeDomSerializer)) as CodeDomSerializer;

			object obj = baseSerializer.Deserialize(manager, codeObject);

			return obj;
		}


		private void SerializeTextFormat(CodeStatementCollection code,
			string property, FlowChart chart, System.Drawing.StringFormat format)
		{
			// this.<fc>.<property> = 
			CodeExpression left = null;
			CodeExpression right = null;
			CodeStatement line = null;
			string temp = "";

			left =
				new CodePropertyReferenceExpression(
					new CodeFieldReferenceExpression(
						new CodeThisReferenceExpression(), chart.Name),
					property);
			right =
				new CodeObjectCreateExpression(
					typeof(System.Drawing.StringFormat),
					new CodeExpression[] {});
			line = new CodeAssignStatement(left, right);

			code.Add(line);

			switch (format.Alignment)
			{
				case System.Drawing.StringAlignment.Near:
					temp = "Near";
					break;
				case System.Drawing.StringAlignment.Center:
					temp = "Center";
					break;
				case System.Drawing.StringAlignment.Far:
					temp = "Far";
					break;
			}
			left =
				new CodePropertyReferenceExpression(
					new CodePropertyReferenceExpression(
						new CodeFieldReferenceExpression(
							new CodeThisReferenceExpression(), chart.Name),
						property),
					"Alignment");
			right =
				new CodeFieldReferenceExpression(
					new CodeTypeReferenceExpression("System.Drawing.StringAlignment"),
					temp);
			line = new CodeAssignStatement(left, right);

            code.Add(line);

			switch (format.LineAlignment)
			{
				case System.Drawing.StringAlignment.Near:
					temp = "Near";
					break;
				case System.Drawing.StringAlignment.Center:
					temp = "Center";
					break;
				case System.Drawing.StringAlignment.Far:
					temp = "Far";
					break;
			}
			left =
				new CodePropertyReferenceExpression(
					new CodePropertyReferenceExpression(
						new CodeFieldReferenceExpression(
							new CodeThisReferenceExpression(), chart.Name),
						property),
					"LineAlignment");
			right =
				new CodeFieldReferenceExpression(
					new CodeTypeReferenceExpression("System.Drawing.StringAlignment"),
					temp);
			line = new CodeAssignStatement(left, right);

			code.Add(line);
		}

		private void SerializeBrush(CodeStatementCollection code,
			string property, FlowChart chart, MindFusion.FlowChartX.Brush brush)
		{
			string representation;
			CodeExpression left = null;
			CodeExpression right = null;
			CodeStatement line = null;

			representation = Brush.Serialize(brush);

			left = 
				new CodePropertyReferenceExpression(
					new CodeFieldReferenceExpression(
						new CodeThisReferenceExpression(), chart.Name),
					property);
			right =
				new CodeMethodInvokeExpression(
					new CodeMethodReferenceExpression(
						new CodeTypeReferenceExpression("MindFusion.FlowChartX.Brush"),
						"Deserialize"),
					new CodeExpression[]
						{
							new CodePrimitiveExpression(representation)
						});
			line = new CodeAssignStatement(left, right);

			code.Add(line);
		}

		private void SerializePen(CodeStatementCollection code,
			string property, FlowChart chart, MindFusion.FlowChartX.Pen pen)
		{
			string representation;
			CodeExpression left = null;
			CodeExpression right = null;
			CodeStatement line = null;

			representation = Pen.Serialize(pen);

			left = 
				new CodePropertyReferenceExpression(
					new CodeFieldReferenceExpression(
						new CodeThisReferenceExpression(), chart.Name),
					property);
			right = 
				new CodeMethodInvokeExpression(
					new CodeMethodReferenceExpression(
						new CodeTypeReferenceExpression("MindFusion.FlowChartX.Pen"),
						"Deserialize"),
					new CodeExpression[]
						{
							new CodePrimitiveExpression(representation)
						});
			line = new CodeAssignStatement(left, right);

			code.Add(line);
		}
	}
}
