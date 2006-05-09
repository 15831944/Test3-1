using System;
using System.Diagnostics;
using System.Collections;
using System.Windows.Forms;
using MindFusion.FlowChartX;

namespace MindFusion.Diagramming.Export
{
	

	/// <summary>
	/// Main class of the DxfExport assembly
	/// encapsulates functionality of Flowchart.NET to DXF
	/// exporting
	/// </summary>
	public class DxfExporter
	{
		public DxfExporter()
		{
		
		}

		 ~DxfExporter()
		{
		
		}

		/// <summary>
		/// Method exports Flowchart.NET drawing into Autodesk DXF version 14 format
		/// </summary>
		/// <param name="flowChart">Flowchart reference</param>
		/// <param name="filePath">Output DXF file path to be saved</param>
		/// <returns>Empty string if call was succeded error description otherwise</returns>
		public void Export(FlowChart flowChart, string filePath)
		{
			string sResult = "";

			try
			{

				// Validating input parameters
				if ( flowChart == null )
					throw new Exception("Empty chart reference passed");

				if ( filePath == null )
					throw new Exception("Empty path string passed");

				
				// Parsing input file path extracting directory and file names
				int nIdx = filePath.LastIndexOf("\\");
				int nLen = filePath.Length;
				FileNamePattern = filePath.Substring(nIdx + 1, nLen - ( nIdx + 5));
				BasedPath = filePath.Substring(0, nIdx);
				
				// Creating DxfHelper object passing filePath as a parameter
				m_DxfHelper = new DxfHelper(filePath, flowChart, m_ExportTextAsMultiline, m_ExportExternalImages,
					                        FileNamePattern, BasedPath );
				if ( m_DxfHelper == null )
					throw new Exception("Error creating DXF helper object");

				if (!m_DxfHelper.IsValid())
					throw new Exception(String.Format("DXF helper object isn't valid({0})", m_DxfHelper.GetStatus()));

				// Storing Flowchart control reference for future use
				m_flowChart = flowChart;

				
				// Sorting chart shapes by 'ZIndex' key
				ArrayList.Adapter(flowChart.Boxes).Sort(new DxfHelper.BoxComparer(true));

				// Processing shapes
				foreach ( Box oBox in flowChart.Boxes)
				{

					
					if ( oBox == null )
						continue;
			
					m_DxfHelper.ChartObject2String(oBox);
				
				}

				// Processing arrows
				foreach ( Arrow oArrow in flowChart.Arrows)
				{

					
					if ( oArrow == null )
						continue;
			
					m_DxfHelper.ChartObject2String(oArrow);
					
				
				}
				
				// Processing tables
				foreach ( Table oTable in flowChart.Tables)
				{

					
					if ( oTable == null )
						continue;
			
					m_DxfHelper.ChartObject2String(oTable);
					
				
				}
				
				// Saving prepared DXF file to disk and cleaning up
				m_DxfHelper.Save();
				m_DxfHelper = null;
                m_flowChart = null;

			}
			catch ( Exception ex )
			{
				sResult = String.Format("Error occured when exporting: {0}",ex.Message);
				Trace.WriteLine(String.Format("{0} error {1}\n","Export.DXF",ex.Message));
			}
		}

		/// <summary>
		/// ExportExternalImages - if true embedded images to be
		/// included into DXF as external files and stored into the same folder
		/// as main DXF file
		/// </summary>
		public bool ExportImages
		{
			get
			{
				return m_ExportExternalImages;
			}
			set
			{
				m_ExportExternalImages = value;
			}
		}

		/// <summary>
		/// ExportTextAsMultiline - if true then multiline text groups (MTEXT) is inserted into DXF
		/// single line text groups ( TEXT) used otherwise ( if false )
		/// </summary>
		public bool ExportTextAsMultiline
		{
			get
			{
				return m_ExportTextAsMultiline;
			}
			set
			{
				m_ExportTextAsMultiline = value;
			}
		}

		// Member variables definition
		protected FlowChart m_flowChart = null;
		private DxfHelper m_DxfHelper = null;
		protected bool m_ExportTextAsMultiline = true;
		protected bool m_ExportExternalImages = false;
		protected string FileNamePattern = "";
		protected string BasedPath = "";
	}
}
