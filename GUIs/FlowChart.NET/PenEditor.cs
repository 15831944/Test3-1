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
	/// Design-time editor for MindFusion.FlowChartX.Pen objects.
	/// </summary>
	internal class PenEditor : UITypeEditor
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
			PenEdit editForm = new PenEdit();
			editForm.Pen = value as MindFusion.FlowChartX.Pen;
			edSvc.DropDownControl(editForm);
			Pen newValue = editForm.Pen;

			editForm.Dispose();
			GC.Collect();

			return newValue;
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
			MindFusion.FlowChartX.Pen pen =
				e.Value as MindFusion.FlowChartX.Pen;

			if (pen == null)
				return;

			try
			{
				// Be careful with pens...
				System.Drawing.Pen gdiPen = pen.CreateGDIPen();
				e.Graphics.DrawLine(gdiPen,
					e.Bounds.Left, e.Bounds.Top + e.Bounds.Height / 2,
					e.Bounds.Right - 1, e.Bounds.Top + e.Bounds.Height / 2);
				gdiPen.Dispose();
			}
			catch
			{
#if DEBUG
				System.Diagnostics.Debug.WriteLine("Shift!");
#endif
			}
		}
	}
}
