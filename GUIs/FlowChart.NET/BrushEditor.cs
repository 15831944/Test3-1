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
	/// Design-time editor for MindFusion.FlowChartX.Brush objects.
	/// </summary>
	internal class BrushEditor : UITypeEditor
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
			BrushEdit editForm = new BrushEdit();
			editForm.Brush = value as MindFusion.FlowChartX.Brush;
			edSvc.DropDownControl(editForm);
			Brush newValue = editForm.Brush;

			editForm.Dispose();
			GC.Collect();

			if (newValue != null)
				return newValue;

			return value;
		}

		public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context) 
		{
			if (context != null && context.Instance != null) 
				return UITypeEditorEditStyle.DropDown;

			return base.GetEditStyle(context);
		}

		public override bool GetPaintValueSupported(ITypeDescriptorContext context)
		{
			return true;
		}

		public override void PaintValue(PaintValueEventArgs e)
		{
			MindFusion.FlowChartX.Brush brush =
				e.Value as MindFusion.FlowChartX.Brush;

			if (brush == null)
				return;

			System.Drawing.Brush gdiBrush = brush.CreateGDIBrush(e.Bounds);
			e.Graphics.FillRectangle(gdiBrush, e.Bounds);
			gdiBrush.Dispose();
		}
	}
}
