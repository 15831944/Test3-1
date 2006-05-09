using System;
using System.Drawing.Printing;

namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Summary description for PrintPreview.
	/// </summary>
	public interface IPrintPreview
	{
		PrintDocument Document { get; set; }
	}
}