// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Drawing;
using System.Drawing.Design;
using System.ComponentModel;
using System.Windows.Forms.Design;

namespace MindFusion.FlowChartX.Design
{
	/// <summary>
	/// Design-time editor for selecting
	/// MindFusion.FlowChartX.ShapeTemplate objects.
	/// </summary>
	internal class ShapeEditor : UITypeEditor
	{
		public override object EditValue(
			ITypeDescriptorContext context,
			IServiceProvider provider, object value)
		{
			// Attempts to obtain an IWindowsFormsEditorService
			IWindowsFormsEditorService edSvc =
				provider.GetService(typeof(IWindowsFormsEditorService))
				as IWindowsFormsEditorService;
			if (edSvc == null)
				return value;

			// Displays a drop-down control
			ShapeEdit editForm = new ShapeEdit(edSvc);
			editForm.Shape = value as MindFusion.FlowChartX.ShapeTemplate;
			edSvc.DropDownControl(editForm);

			return editForm.Shape;
		}

		public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
		{
			if (context != null && context.Instance != null) 
				return UITypeEditorEditStyle.DropDown;

			return base.GetEditStyle (context);
		}

		public override bool GetPaintValueSupported(ITypeDescriptorContext context)
		{
			return true;
		}

		public override void PaintValue(PaintValueEventArgs e)
		{
			MindFusion.FlowChartX.ShapeTemplate shape = 
				e.Value as MindFusion.FlowChartX.ShapeTemplate;

			if (shape == null)
				return;

			// Draw the shape
			RectangleF rect = new RectangleF(
				(float)e.Bounds.Left, (float)e.Bounds.Top,
				(float)e.Bounds.Width - 1, (float)e.Bounds.Height - 1);
			rect.Inflate(-2, -2);
			MindFusion.FlowChartX.ShapeTemplate.PathData data =
				shape.initData(rect, 0);
			System.Drawing.Brush brush =
				new System.Drawing.SolidBrush(Color.LightSteelBlue);
			System.Drawing.Pen pen =
				new System.Drawing.Pen(Color.Black);

			System.Drawing.Drawing2D.SmoothingMode mode =
				e.Graphics.SmoothingMode;
			e.Graphics.SmoothingMode = 
				System.Drawing.Drawing2D.SmoothingMode.AntiAlias;

			System.Drawing.Drawing2D.GraphicsPath path =
				shape.getPath(data, 0);
			e.Graphics.FillPath(brush, path);
			e.Graphics.DrawPath(pen, path);
			path.Dispose();

			path = shape.getDecorationPath(data, 0);
			if (path != null)
			{
				e.Graphics.DrawPath(pen, path);
				path.Dispose();
			}
			
			e.Graphics.SmoothingMode = mode;

            pen.Dispose();
			brush.Dispose();
		}
	}
}
