
using System;
using System.IO;
using System.Resources;
using System.Windows.Forms;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Provides a collection of cursor object to use with the FlowChart control.
	/// </summary>
	public sealed class CustomCursors
	{
		private CustomCursors()
		{
		}

		/// <summary>
		/// Loads the cursors from the specified resource manager.
		/// </summary>
		internal static void Init(ResourceManager manager)
		{
			if (_rotate != null)
				return;

			try
			{
				MemoryStream stream = null;
			
				stream = new MemoryStream((byte[])manager.GetObject("cur_rotate"));
				_rotate = new Cursor(stream);
				stream.Close();
			}
			catch (Exception)
			{
				_rotate = Cursors.Arrow;
			}
		}


		/// <summary>
		/// Gets the cursor, which is displayed when the mouse
		/// is positioned over the rotation handle.
		/// </summary>
		public static Cursor Rotate
		{
			get { return _rotate; }
		}


		private static Cursor _rotate;
	}
}
