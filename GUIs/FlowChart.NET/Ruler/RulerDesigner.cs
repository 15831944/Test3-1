using System;
using System.ComponentModel;
using System.Windows.Forms;
using System.Windows.Forms.Design;

namespace MindFusion.FlowChartX.Design
{
	/// <summary>
	/// Designer for the Ruler control.
	/// </summary>
	public class RulerDesigner : ParentControlDesigner
	{
		/// <summary>
		/// Allow only FlowChart controls to be parented,
		/// and one at a time at most.
		/// </summary>
		public override bool CanParent(Control control)
		{
			Ruler ruler = this.Control as Ruler;

			if (ruler.Document != null)
				return false;

			if (control is FlowChart)
				return true;

			return false;
		}

		/// <summary>
		/// Add the design-time only properties to the list.
		/// </summary>
		protected override void PreFilterProperties(
			System.Collections.IDictionary properties)
		{
			// It is much likely that "Document" is already
			// in the dictionary, so use unique key for the
			// surrogate
			properties.Add("Document_Surrogate",
				TypeDescriptor.CreateProperty(typeof(RulerDesigner),
				"Document", typeof(FlowChart), null));
		}

		protected override bool EnableDragRect
		{
			get { return false; }
		}

		protected override bool DrawGrid
		{
			get { return false; }
			set {}
		}

		/// <summary>
		/// A design-time only surrogate for the Ruler.Document property.
		/// </summary>
		[Browsable(true)]
		[Category("Data")]
		[Description("Gets or sets the document associated with this ruler.")]
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public FlowChart Document
		{
			get { return (this.Control as Ruler).Document; }
			set { (this.Control as Ruler).Document = value; }
		}
	}
}
