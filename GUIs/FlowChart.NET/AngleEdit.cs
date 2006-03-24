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
using MindFusion.Geometry.Geometry2D;

namespace MindFusion.Controls
{
	/// <summary>
	/// Angle editing control.
	/// </summary>
	internal class AngleEdit : System.Windows.Forms.Control
	{
		private System.ComponentModel.Container components = null;

		public AngleEdit()
		{
			InitializeComponent();

			SetStyle(ControlStyles.AllPaintingInWmPaint, true);
			SetStyle(ControlStyles.DoubleBuffer, true);
			SetStyle(ControlStyles.UserPaint, true);
			SetStyle(ControlStyles.ResizeRedraw, true);
			SetStyle(ControlStyles.Selectable, true);
		}

		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			components = new System.ComponentModel.Container();
		}
		#endregion


		protected override void OnPaint(PaintEventArgs e)
		{
			// Paint the background
			System.Drawing.Brush brush =
				new System.Drawing.SolidBrush(BackColor);
			e.Graphics.FillRectangle(brush, this.ClientRectangle);
			brush.Dispose();

			// Setup graphics
			System.Drawing.Drawing2D.GraphicsState state = e.Graphics.Save();
			e.Graphics.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;

			Rectangle rect = GetSquareRect();
			System.Drawing.Pen pen =
				new System.Drawing.Pen(ForeColor, 2);
			System.Drawing.Brush brush2 =
				new System.Drawing.SolidBrush(ForeColor);

			// Draw the circle
			e.Graphics.DrawEllipse(pen, rect);

			// Draw angle arrow
			PointF begin = PointF.Empty;
			PointF end = PointF.Empty;
			MindFusion.Geometry.Geometry2D.Convert.PolarToDekart(
				new PointF(
					rect.X + (float)rect.Width / 2,
					rect.Y + (float)rect.Height / 2),
				_angle, rect.Width / 3, ref begin);
			MindFusion.Geometry.Geometry2D.Convert.PolarToDekart(
				new PointF(
					rect.X + (float)rect.Width / 2,
					rect.Y + (float)rect.Height / 2),
				_angle + 180, rect.Width / 3, ref end);
			e.Graphics.DrawLine(pen, begin, end);
			e.Graphics.FillEllipse(brush2, begin.X - 3, begin.Y - 3, 6, 6);

			brush2.Dispose();
			pen.Dispose();

			// Reset graphics
			e.Graphics.Restore(state);
		}

		protected override void OnPaintBackground(PaintEventArgs pevent)
		{
		}

		protected override void OnMouseDown(MouseEventArgs e)
		{
			base.OnMouseDown(e);

			if (e.Button == MouseButtons.Left)
			{
				this.Focus();

				this.Capture = true;
				_dragging = true;
				_reset = _angle;

				float a = 0, r = 0;
				Rectangle rect = GetSquareRect();
				MindFusion.Geometry.Geometry2D.Convert.DekartToPolar(
					new PointF(
						rect.X + (float)rect.Width / 2,
						rect.Y + (float)rect.Height / 2),
					new PointF(e.X, e.Y), ref a, ref r);
				_angleDif = a - _angle;
			}
			else if (e.Button == MouseButtons.Right)
			{
				if (_dragging)
				{
					_dragging = false;
					this.Capture = false;

					SetAngle(_reset);
				}
			}
		}

		protected override void OnMouseMove(MouseEventArgs e)
		{
			base.OnMouseMove(e);

			if (_dragging)
			{
				float a = 0, r = 0;
				Rectangle rect = GetSquareRect();
				MindFusion.Geometry.Geometry2D.Convert.DekartToPolar(
					new PointF(
						rect.X + (float)rect.Width / 2,
						rect.Y + (float)rect.Height / 2),
					new PointF(e.X, e.Y), ref a, ref r);

				float newAngle = a - _angleDif;
				if (newAngle < 0)
					newAngle += 360;
				if (_controlDown)
				{
					float[] angles = new float[]
						{
							0, 45, 90, 135, 180, 225, 270, 315, 360
						};

					foreach (float angle in angles)
					{
						if (Math.Abs(angle - newAngle) < 15)
						{
							newAngle = angle;
							break;
						}
					}
				}
				SetAngle(newAngle);
			}
		}

		protected override void OnMouseUp(MouseEventArgs e)
		{
			base.OnMouseUp(e);

			if (_dragging)
			{
				_dragging = false;
				this.Capture = false;
			}
		}

		protected override void OnKeyDown(KeyEventArgs e)
		{
			base.OnKeyDown(e);

			if (e.Control)
				_controlDown = true;
		}

		protected override void OnKeyUp(KeyEventArgs e)
		{
			base.OnKeyUp(e);

			_controlDown = false;
		}


		[Browsable(true)]
		[Category("Data")]
		[Description("Specifies the current angle value.")]
		public float Angle
		{
			get { return _angle; }
			set
			{
				_angle = value;
				Invalidate();
			}
		}

		[Browsable(false)]
		public new string Text
		{
			get { return base.Text; }
			set { base.Text = value; }
		}


		[Browsable(true)]
		[Category("Property Changed")]
		[Description("Occurs when the value of the angle has changed.")]
		public event EventHandler AngleChanged;


		private Rectangle GetSquareRect()
		{
			Rectangle rect = this.ClientRectangle;

			if (rect.Width > rect.Height)
				rect.Width = rect.Height;
			if (rect.Height > rect.Width)
				rect.Height = rect.Width;
			rect.Inflate(-2, -2);
			rect.Width -= 2;
			rect.Height -= 2;

			return rect;
		}

		private void SetAngle(float angle)
		{
			if (angle < 0)
				angle += 360;
			while (angle >= 360)
				angle -= 360;

			if (Math.Abs(_angle - angle) < float.Epsilon)
				return;

			_angle = angle;

			if (AngleChanged != null)
				AngleChanged(this, EventArgs.Empty);
			Invalidate();
		}


		private float _angle = 0;
		private float _reset = 0;
		private bool _dragging = false;
		private float _angleDif = 0;
		private bool _controlDown = false;
	}
}
