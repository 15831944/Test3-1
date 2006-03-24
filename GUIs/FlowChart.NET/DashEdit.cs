// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace MindFusion.Controls
{
	/// <summary>
	/// Allows interactive dash pattern editing.
	/// </summary>
	internal class DashEdit : System.Windows.Forms.Control
	{
		public DashEdit()
		{
			SetStyle(ControlStyles.AllPaintingInWmPaint, true);
			SetStyle(ControlStyles.DoubleBuffer, true);
			SetStyle(ControlStyles.UserPaint, true);
			SetStyle(ControlStyles.ResizeRedraw, true);
			SetStyle(ControlStyles.Selectable, true);
		}


		protected override void OnPaint(PaintEventArgs e)
		{
			base.OnPaint(e);

			Rectangle rect = this.ClientRectangle;

			System.Drawing.Brush brush =
				new System.Drawing.SolidBrush(this.BackColor);
			e.Graphics.FillRectangle(brush, rect);
			brush.Dispose();

			System.Drawing.Brush brush2 =
				new System.Drawing.SolidBrush(
					this.Enabled ? this.ForeColor : SystemColors.ControlDarkDark);
			for (int i = 0; i < _tiles.Length; i++)
			{
				if (_tiles[i])
				{
					e.Graphics.FillRectangle(brush2,
						rect.X + i * (_tileSize + 1),
						rect.Y + rect.Height / 2 - _tileSize / 2,
						_tileSize, _tileSize);
				}
			}
			brush2.Dispose();
		}

		protected override void OnMouseDown(MouseEventArgs e)
		{
			base.OnMouseDown(e);

			if (e.Button == MouseButtons.Left)
			{
				this.Capture = true;
				_dragging = true;

				_toggled = new bool[_tiles.Length];
				for (int i = 0; i < _toggled.Length; i++)
					_toggled[i] = false;

				ToggleAt(new Point(e.X, e.Y));
			}
		}

		protected override void OnMouseMove(MouseEventArgs e)
		{
			base.OnMouseMove(e);

			if (_dragging)
				ToggleAt(new Point(e.X, e.Y));
		}

		protected override void OnMouseUp(MouseEventArgs e)
		{
			base.OnMouseUp(e);

			if (_dragging)
			{
				this.Capture = false;
				_dragging = false;
			}
		}

		protected override void OnResize(EventArgs e)
		{
			base.OnResize(e);

			RedimTiles();
		}


		/// <summary>
		/// Updates the dash pattern of the specified pen
		/// to the one currently displayed in the control.
		/// </summary>
		public void GetDashPattern(MindFusion.FlowChartX.Pen pen)
		{
			float[] pattern = GetDashPattern(true);

			if (pattern == null)
			{
				pen.DashStyle = System.Drawing.Drawing2D.DashStyle.Solid;
				return;
			}

			pen.DashPattern = pattern;
		}

		/// <summary>
		/// Updates the currently displayed dash pattern
		/// to match the one of the specified pen.
		/// </summary>
		public void SetDashPattern(MindFusion.FlowChartX.Pen pen)
		{
			if (pen.DashPattern != null)
				SetDashPattern(pen.DashPattern);
		}

		private float[] GetDashPattern(bool normalize)
		{
			if (_tiles.Length == 0)
				return null;

			bool hasEmpty = false;
			foreach (bool b in _tiles)
			{
				if (!b)
				{
					hasEmpty = true;
					break;
				}
			}
			if (!hasEmpty)
				return null;

			bool hasFull = false;
			foreach (bool b in _tiles)
			{
				if (b)
				{
					hasFull = true;
					break;
				}
			}
			if (!hasFull)
				return null;

			bool[] normalized = new bool[_tiles.Length];
			int nonEmpty = 0;
			while (!_tiles[nonEmpty++]); nonEmpty--;
			if (!normalize)
				nonEmpty = 0;
			for (int n = 0 ; n < _tiles.Length; n++)
				normalized[(normalized.Length - nonEmpty + n) % normalized.Length] = _tiles[n];

			int transitions = 0;
			int i = 0;
			bool prev = normalized[i++];

			while (i < normalized.Length)
			{
				if (prev != normalized[i])
				{
					transitions++;
					prev = normalized[i];
				}

				i++;
			}

			float[] result = new float[transitions + 1];

			float len = 1;
			int j = 0;
			i = 0;
			prev = normalized[i++];

			while (i < normalized.Length)
			{
				if (prev != normalized[i])
				{
					result[j++] = len;
					len = 1;

					prev = normalized[i];
				}
				else
				{
					len++;
				}

				i++;
			}

			if (len > 0)
				result[j] = len;

			if (normalize && (result.Length % 2 == 1))
			{
				float[] copy = result;
				result = new float[copy.Length - 1];

				for (i = 0; i < result.Length; i++)
					result[i] = copy[i];
				result[0] += copy[copy.Length - 1];
			}

			return result;
		}

		private void SetDashPattern(float[] pattern)
		{
			// Accomodate the pattern
			bool val = true;
			float total = 0;
			for (int i = 0; i < pattern.Length; i++)
			{
				for (int j = (int)total; j < (int)(total + Math.Round(pattern[i])); j++)
					if (j < _tiles.Length)
						_tiles[j] = val;

                total += pattern[i];
				val = !val;
			}

			Invalidate();
		}

		public float[] GetCompoundArray()
		{
			float[] dash = GetDashPattern(false);
			if (dash == null)
				return null;

			float total = 0;
			foreach (float f in dash)
				total += f;

			int start = _tiles[0] ? 0 : 1;
			float[] result = new float[(dash.Length + (_tiles[0] ? 1 : 0)) / 2 * 2];
			float cur = 0;
			if (start > 0)
				cur = dash[0];
			for (int i = start; i < result.Length; i += 2)
			{
				result[i - start] = cur / total;
				cur += dash[i];
				result[i - start + 1] = cur / total;
				if (i + 1 < dash.Length)
					cur += dash[i + 1];
			}

			return result;
		}

		public void SetCompoundArray(float[] array)
		{
			float total = _tiles.Length;

			for (int i = 0; i < _tiles.Length; i++)
				_tiles[i] = false;

			for (int i = 0; i < array.Length; i += 2)
			{
				int l = (int)Math.Round(array[i] * total);
				int r = (int)Math.Round(array[i + 1] * total);

				for (int j = l; j < r; j++)
				{
					_tiles[j] = true;
				}
			}

			Invalidate();
		}


		[Browsable(false)]
		public new string Text
		{
			get { return base.Text; }
			set { base.Text = value; }
		}


		[Category("Property Changed")]
		public event EventHandler DashChanged;


		/// <summary>
		/// Recalculates the number of tiles which would
		/// fit in the client area, based on the tile size
		/// and the client area itself.
		/// </summary>
		private void RedimTiles()
		{
			int tileCount = this.ClientRectangle.Width / (_tileSize + 1);
			_tiles = new bool[tileCount];
			for (int i = 0; i < _tiles.Length; i++)
				_tiles[i] = true;
		}

		/// <summary>
		/// Toggles the tile at the specified
		/// client point.
		/// </summary>
		private void ToggleAt(Point point)
		{
			int tile = point.X / (_tileSize + 1);
			if (tile >= 0 && tile < _tiles.Length)
			{
				if (!_toggled[tile])
				{
					_toggled[tile] = true;
					_tiles[tile] = !_tiles[tile];

					if (DashChanged != null)
						DashChanged(this, EventArgs.Empty);

					Invalidate();
				}
			}
		}



		private bool[] _tiles = new bool[0];
		private bool[] _toggled = null;
		private bool _dragging = false;
		private int _tileSize = 6;
	}
}
