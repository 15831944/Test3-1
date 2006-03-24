// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Design;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Windows.Forms.Design;

namespace MindFusion.FlowChartX.Design
{
	/// <summary>
	/// MindFusion.FlowChartX.Brush editing form.
	/// </summary>
	internal class BrushEdit : System.Windows.Forms.UserControl
	{
		private System.ComponentModel.IContainer components;
		private System.Windows.Forms.RadioButton _btnSolid;
		private System.Windows.Forms.RadioButton _btnGradient;
		private System.Windows.Forms.RadioButton _btnHatch;
		private System.Windows.Forms.Panel _pnlSolid;
		private System.Windows.Forms.Panel _pnlGradient;
		private System.Windows.Forms.ToolTip _toolTip;
		private MindFusion.Controls.AngleEdit _angle;
		private MindFusion.Controls.HatchCombo _hatchCombo;
		private System.Windows.Forms.Label _lblHatch;
		private System.Windows.Forms.Label _lblColor;
		private System.Windows.Forms.TextBox _textColor;
		private System.Windows.Forms.Label _lblAngleValue;
		private System.Windows.Forms.Label _lblAngle;
		private System.Windows.Forms.Label _lblBackColor;
		private System.Windows.Forms.TextBox _textBackColor;
		private System.Windows.Forms.Label _lblForeColor;
		private System.Windows.Forms.TextBox _textForeColor;
		private System.Windows.Forms.Label _lblEndColor;
		private System.Windows.Forms.TextBox _textEndColor;
		private System.Windows.Forms.Label _lblStartColor;
		private System.Windows.Forms.TextBox _textStartColor;
		private MindFusion.Controls.BrushView _preview;
		private System.Windows.Forms.Label _line;
		private System.Windows.Forms.RadioButton _btnTexture;
		private System.Windows.Forms.Panel _pnlTexture;
		private System.Windows.Forms.Label _lblInfo;
		private System.Windows.Forms.Panel _pnlHatch;

		public BrushEdit()
		{
			InitializeComponent();
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose(bool disposing)
		{
			if (_temp != null)
			{
				_temp.Dispose();
				_temp = null;
			}

			if (disposing)
			{
				if (components != null)
					components.Dispose();
			}

			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(BrushEdit));
			this._btnSolid = new System.Windows.Forms.RadioButton();
			this._btnGradient = new System.Windows.Forms.RadioButton();
			this._btnHatch = new System.Windows.Forms.RadioButton();
			this._pnlSolid = new System.Windows.Forms.Panel();
			this._lblColor = new System.Windows.Forms.Label();
			this._textColor = new System.Windows.Forms.TextBox();
			this._pnlGradient = new System.Windows.Forms.Panel();
			this._lblEndColor = new System.Windows.Forms.Label();
			this._textEndColor = new System.Windows.Forms.TextBox();
			this._textStartColor = new System.Windows.Forms.TextBox();
			this._lblAngle = new System.Windows.Forms.Label();
			this._lblAngleValue = new System.Windows.Forms.Label();
			this._angle = new MindFusion.Controls.AngleEdit();
			this._lblStartColor = new System.Windows.Forms.Label();
			this._pnlHatch = new System.Windows.Forms.Panel();
			this._lblBackColor = new System.Windows.Forms.Label();
			this._textBackColor = new System.Windows.Forms.TextBox();
			this._lblForeColor = new System.Windows.Forms.Label();
			this._lblHatch = new System.Windows.Forms.Label();
			this._textForeColor = new System.Windows.Forms.TextBox();
			this._hatchCombo = new MindFusion.Controls.HatchCombo();
			this._toolTip = new System.Windows.Forms.ToolTip(this.components);
			this._btnTexture = new System.Windows.Forms.RadioButton();
			this._preview = new MindFusion.Controls.BrushView();
			this._line = new System.Windows.Forms.Label();
			this._pnlTexture = new System.Windows.Forms.Panel();
			this._lblInfo = new System.Windows.Forms.Label();
			this._pnlSolid.SuspendLayout();
			this._pnlGradient.SuspendLayout();
			this._pnlHatch.SuspendLayout();
			this._pnlTexture.SuspendLayout();
			this.SuspendLayout();
			// 
			// _btnSolid
			// 
			this._btnSolid.Appearance = System.Windows.Forms.Appearance.Button;
			this._btnSolid.BackColor = System.Drawing.Color.Lime;
			this._btnSolid.Checked = true;
			this._btnSolid.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this._btnSolid.Image = ((System.Drawing.Image)(resources.GetObject("_btnSolid.Image")));
			this._btnSolid.Location = new System.Drawing.Point(4, 4);
			this._btnSolid.Name = "_btnSolid";
			this._btnSolid.Size = new System.Drawing.Size(21, 21);
			this._btnSolid.TabIndex = 1;
			this._btnSolid.TabStop = true;
			this._toolTip.SetToolTip(this._btnSolid, "Solid");
			this._btnSolid.Click += new System.EventHandler(this.OnSolid);
			// 
			// _btnGradient
			// 
			this._btnGradient.Appearance = System.Windows.Forms.Appearance.Button;
			this._btnGradient.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this._btnGradient.Image = ((System.Drawing.Image)(resources.GetObject("_btnGradient.Image")));
			this._btnGradient.Location = new System.Drawing.Point(28, 4);
			this._btnGradient.Name = "_btnGradient";
			this._btnGradient.Size = new System.Drawing.Size(21, 21);
			this._btnGradient.TabIndex = 2;
			this._toolTip.SetToolTip(this._btnGradient, "Gradient");
			this._btnGradient.Click += new System.EventHandler(this.OnGradient);
			// 
			// _btnHatch
			// 
			this._btnHatch.Appearance = System.Windows.Forms.Appearance.Button;
			this._btnHatch.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this._btnHatch.Image = ((System.Drawing.Image)(resources.GetObject("_btnHatch.Image")));
			this._btnHatch.Location = new System.Drawing.Point(52, 4);
			this._btnHatch.Name = "_btnHatch";
			this._btnHatch.Size = new System.Drawing.Size(21, 21);
			this._btnHatch.TabIndex = 3;
			this._toolTip.SetToolTip(this._btnHatch, "Hatch");
			this._btnHatch.Click += new System.EventHandler(this.OnHatch);
			// 
			// _pnlSolid
			// 
			this._pnlSolid.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this._pnlSolid.Controls.Add(this._lblColor);
			this._pnlSolid.Controls.Add(this._textColor);
			this._pnlSolid.Location = new System.Drawing.Point(0, 28);
			this._pnlSolid.Name = "_pnlSolid";
			this._pnlSolid.Size = new System.Drawing.Size(144, 68);
			this._pnlSolid.TabIndex = 2;
			// 
			// _lblColor
			// 
			this._lblColor.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
			this._lblColor.Location = new System.Drawing.Point(12, 4);
			this._lblColor.Name = "_lblColor";
			this._lblColor.Size = new System.Drawing.Size(60, 12);
			this._lblColor.TabIndex = 6;
			this._lblColor.Text = "Color:";
			// 
			// _textColor
			// 
			this._textColor.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this._textColor.Location = new System.Drawing.Point(12, 16);
			this._textColor.Name = "_textColor";
			this._textColor.Size = new System.Drawing.Size(80, 17);
			this._textColor.TabIndex = 5;
			this._textColor.Text = "255, 255, 255";
			this._textColor.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
			this._textColor.Leave += new System.EventHandler(this.OnColorChanged);
			// 
			// _pnlGradient
			// 
			this._pnlGradient.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this._pnlGradient.Controls.Add(this._lblEndColor);
			this._pnlGradient.Controls.Add(this._textEndColor);
			this._pnlGradient.Controls.Add(this._textStartColor);
			this._pnlGradient.Controls.Add(this._lblAngle);
			this._pnlGradient.Controls.Add(this._lblAngleValue);
			this._pnlGradient.Controls.Add(this._angle);
			this._pnlGradient.Controls.Add(this._lblStartColor);
			this._pnlGradient.Location = new System.Drawing.Point(0, 28);
			this._pnlGradient.Name = "_pnlGradient";
			this._pnlGradient.Size = new System.Drawing.Size(144, 68);
			this._pnlGradient.TabIndex = 3;
			// 
			// _lblEndColor
			// 
			this._lblEndColor.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
			this._lblEndColor.Location = new System.Drawing.Point(60, 36);
			this._lblEndColor.Name = "_lblEndColor";
			this._lblEndColor.Size = new System.Drawing.Size(60, 12);
			this._lblEndColor.TabIndex = 11;
			this._lblEndColor.Text = "End color:";
			// 
			// _textEndColor
			// 
			this._textEndColor.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this._textEndColor.Location = new System.Drawing.Point(60, 48);
			this._textEndColor.Name = "_textEndColor";
			this._textEndColor.Size = new System.Drawing.Size(80, 17);
			this._textEndColor.TabIndex = 10;
			this._textEndColor.Text = "255, 255, 255";
			this._textEndColor.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
			this._textEndColor.Leave += new System.EventHandler(this.OnColorChanged);
			// 
			// _textStartColor
			// 
			this._textStartColor.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this._textStartColor.Location = new System.Drawing.Point(60, 16);
			this._textStartColor.Name = "_textStartColor";
			this._textStartColor.Size = new System.Drawing.Size(80, 17);
			this._textStartColor.TabIndex = 8;
			this._textStartColor.Text = "255, 255, 255";
			this._textStartColor.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
			this._textStartColor.Leave += new System.EventHandler(this.OnColorChanged);
			// 
			// _lblAngle
			// 
			this._lblAngle.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
			this._lblAngle.Location = new System.Drawing.Point(12, 4);
			this._lblAngle.Name = "_lblAngle";
			this._lblAngle.Size = new System.Drawing.Size(36, 12);
			this._lblAngle.TabIndex = 7;
			this._lblAngle.Text = "Angle:";
			// 
			// _lblAngleValue
			// 
			this._lblAngleValue.Location = new System.Drawing.Point(12, 52);
			this._lblAngleValue.Name = "_lblAngleValue";
			this._lblAngleValue.Size = new System.Drawing.Size(36, 16);
			this._lblAngleValue.TabIndex = 3;
			this._lblAngleValue.Text = "0°";
			this._lblAngleValue.TextAlign = System.Drawing.ContentAlignment.TopCenter;
			// 
			// _angle
			// 
			this._angle.Angle = 0F;
			this._angle.Location = new System.Drawing.Point(12, 16);
			this._angle.Name = "_angle";
			this._angle.Size = new System.Drawing.Size(36, 36);
			this._angle.TabIndex = 2;
			this._angle.AngleChanged += new System.EventHandler(this.OnAngleChanged);
			// 
			// _lblStartColor
			// 
			this._lblStartColor.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
			this._lblStartColor.Location = new System.Drawing.Point(60, 4);
			this._lblStartColor.Name = "_lblStartColor";
			this._lblStartColor.Size = new System.Drawing.Size(60, 12);
			this._lblStartColor.TabIndex = 9;
			this._lblStartColor.Text = "Start color:";
			// 
			// _pnlHatch
			// 
			this._pnlHatch.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this._pnlHatch.Controls.Add(this._lblBackColor);
			this._pnlHatch.Controls.Add(this._textBackColor);
			this._pnlHatch.Controls.Add(this._lblForeColor);
			this._pnlHatch.Controls.Add(this._lblHatch);
			this._pnlHatch.Controls.Add(this._textForeColor);
			this._pnlHatch.Controls.Add(this._hatchCombo);
			this._pnlHatch.Location = new System.Drawing.Point(0, 28);
			this._pnlHatch.Name = "_pnlHatch";
			this._pnlHatch.Size = new System.Drawing.Size(144, 68);
			this._pnlHatch.TabIndex = 0;
			this._pnlHatch.Visible = false;
			// 
			// _lblBackColor
			// 
			this._lblBackColor.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this._lblBackColor.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
			this._lblBackColor.Location = new System.Drawing.Point(60, 36);
			this._lblBackColor.Name = "_lblBackColor";
			this._lblBackColor.Size = new System.Drawing.Size(60, 12);
			this._lblBackColor.TabIndex = 6;
			this._lblBackColor.Text = "Back color:";
			// 
			// _textBackColor
			// 
			this._textBackColor.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this._textBackColor.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this._textBackColor.Location = new System.Drawing.Point(60, 48);
			this._textBackColor.Name = "_textBackColor";
			this._textBackColor.Size = new System.Drawing.Size(80, 17);
			this._textBackColor.TabIndex = 5;
			this._textBackColor.Text = "255, 255, 255";
			this._textBackColor.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
			this._textBackColor.Leave += new System.EventHandler(this.OnColorChanged);
			// 
			// _lblForeColor
			// 
			this._lblForeColor.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this._lblForeColor.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
			this._lblForeColor.Location = new System.Drawing.Point(60, 4);
			this._lblForeColor.Name = "_lblForeColor";
			this._lblForeColor.Size = new System.Drawing.Size(60, 12);
			this._lblForeColor.TabIndex = 4;
			this._lblForeColor.Text = "Fore color:";
			// 
			// _lblHatch
			// 
			this._lblHatch.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
			this._lblHatch.Location = new System.Drawing.Point(8, 4);
			this._lblHatch.Name = "_lblHatch";
			this._lblHatch.Size = new System.Drawing.Size(44, 12);
			this._lblHatch.TabIndex = 3;
			this._lblHatch.Text = "Style:";
			// 
			// _textForeColor
			// 
			this._textForeColor.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this._textForeColor.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this._textForeColor.Location = new System.Drawing.Point(60, 16);
			this._textForeColor.Name = "_textForeColor";
			this._textForeColor.Size = new System.Drawing.Size(80, 17);
			this._textForeColor.TabIndex = 2;
			this._textForeColor.Text = "255, 255, 255";
			this._textForeColor.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
			this._textForeColor.Leave += new System.EventHandler(this.OnColorChanged);
			// 
			// _hatchCombo
			// 
			this._hatchCombo.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
			this._hatchCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this._hatchCombo.ItemHeight = 30;
			this._hatchCombo.Items.AddRange(new object[] {
															 "Horizontal",
															 "Horizontal",
															 "Vertical",
															 "ForwardDiagonal",
															 "BackwardDiagonal",
															 "LargeGrid",
															 "LargeGrid",
															 "LargeGrid",
															 "DiagonalCross",
															 "Percent05",
															 "Percent10",
															 "Percent20",
															 "Percent25",
															 "Percent30",
															 "Percent40",
															 "Percent50",
															 "Percent60",
															 "Percent70",
															 "Percent75",
															 "Percent80",
															 "Percent90",
															 "LightDownwardDiagonal",
															 "LightUpwardDiagonal",
															 "DarkDownwardDiagonal",
															 "DarkUpwardDiagonal",
															 "WideDownwardDiagonal",
															 "WideUpwardDiagonal",
															 "LightVertical",
															 "LightHorizontal",
															 "NarrowVertical",
															 "NarrowHorizontal",
															 "DarkVertical",
															 "DarkHorizontal",
															 "DashedDownwardDiagonal",
															 "DashedUpwardDiagonal",
															 "DashedHorizontal",
															 "DashedVertical",
															 "SmallConfetti",
															 "LargeConfetti",
															 "ZigZag",
															 "Wave",
															 "DiagonalBrick",
															 "HorizontalBrick",
															 "Weave",
															 "Plaid",
															 "Divot",
															 "DottedGrid",
															 "DottedDiamond",
															 "Shingle",
															 "Trellis",
															 "Sphere",
															 "SmallGrid",
															 "SmallCheckerBoard",
															 "LargeCheckerBoard",
															 "OutlinedDiamond",
															 "SolidDiamond",
															 "Horizontal",
															 "Horizontal",
															 "Vertical",
															 "ForwardDiagonal",
															 "BackwardDiagonal",
															 "LargeGrid",
															 "LargeGrid",
															 "LargeGrid",
															 "DiagonalCross",
															 "Percent05",
															 "Percent10",
															 "Percent20",
															 "Percent25",
															 "Percent30",
															 "Percent40",
															 "Percent50",
															 "Percent60",
															 "Percent70",
															 "Percent75",
															 "Percent80",
															 "Percent90",
															 "LightDownwardDiagonal",
															 "LightUpwardDiagonal",
															 "DarkDownwardDiagonal",
															 "DarkUpwardDiagonal",
															 "WideDownwardDiagonal",
															 "WideUpwardDiagonal",
															 "LightVertical",
															 "LightHorizontal",
															 "NarrowVertical",
															 "NarrowHorizontal",
															 "DarkVertical",
															 "DarkHorizontal",
															 "DashedDownwardDiagonal",
															 "DashedUpwardDiagonal",
															 "DashedHorizontal",
															 "DashedVertical",
															 "SmallConfetti",
															 "LargeConfetti",
															 "ZigZag",
															 "Wave",
															 "DiagonalBrick",
															 "HorizontalBrick",
															 "Weave",
															 "Plaid",
															 "Divot",
															 "DottedGrid",
															 "DottedDiamond",
															 "Shingle",
															 "Trellis",
															 "Sphere",
															 "SmallGrid",
															 "SmallCheckerBoard",
															 "LargeCheckerBoard",
															 "OutlinedDiamond",
															 "SolidDiamond",
															 "Horizontal",
															 "Horizontal",
															 "Vertical",
															 "ForwardDiagonal",
															 "BackwardDiagonal",
															 "LargeGrid",
															 "LargeGrid",
															 "LargeGrid",
															 "DiagonalCross",
															 "Percent05",
															 "Percent10",
															 "Percent20",
															 "Percent25",
															 "Percent30",
															 "Percent40",
															 "Percent50",
															 "Percent60",
															 "Percent70",
															 "Percent75",
															 "Percent80",
															 "Percent90",
															 "LightDownwardDiagonal",
															 "LightUpwardDiagonal",
															 "DarkDownwardDiagonal",
															 "DarkUpwardDiagonal",
															 "WideDownwardDiagonal",
															 "WideUpwardDiagonal",
															 "LightVertical",
															 "LightHorizontal",
															 "NarrowVertical",
															 "NarrowHorizontal",
															 "DarkVertical",
															 "DarkHorizontal",
															 "DashedDownwardDiagonal",
															 "DashedUpwardDiagonal",
															 "DashedHorizontal",
															 "DashedVertical",
															 "SmallConfetti",
															 "LargeConfetti",
															 "ZigZag",
															 "Wave",
															 "DiagonalBrick",
															 "HorizontalBrick",
															 "Weave",
															 "Plaid",
															 "Divot",
															 "DottedGrid",
															 "DottedDiamond",
															 "Shingle",
															 "Trellis",
															 "Sphere",
															 "SmallGrid",
															 "SmallCheckerBoard",
															 "LargeCheckerBoard",
															 "OutlinedDiamond",
															 "SolidDiamond"});
			this._hatchCombo.Location = new System.Drawing.Point(4, 16);
			this._hatchCombo.Name = "_hatchCombo";
			this._hatchCombo.SelectedStyle = System.Drawing.Drawing2D.HatchStyle.Horizontal;
			this._hatchCombo.Size = new System.Drawing.Size(52, 36);
			this._hatchCombo.TabIndex = 1;
			this._hatchCombo.SelectedIndexChanged += new System.EventHandler(this.OnHatchChanged);
			// 
			// _btnTexture
			// 
			this._btnTexture.Appearance = System.Windows.Forms.Appearance.Button;
			this._btnTexture.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this._btnTexture.Image = ((System.Drawing.Image)(resources.GetObject("_btnTexture.Image")));
			this._btnTexture.Location = new System.Drawing.Point(76, 4);
			this._btnTexture.Name = "_btnTexture";
			this._btnTexture.Size = new System.Drawing.Size(21, 21);
			this._btnTexture.TabIndex = 7;
			this._toolTip.SetToolTip(this._btnTexture, "Texture");
			this._btnTexture.Click += new System.EventHandler(this.OnTexture);
			// 
			// _preview
			// 
			this._preview.Brush = null;
			this._preview.Location = new System.Drawing.Point(108, 4);
			this._preview.Name = "_preview";
			this._preview.Size = new System.Drawing.Size(32, 21);
			this._preview.TabIndex = 5;
			// 
			// _line
			// 
			this._line.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this._line.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this._line.Location = new System.Drawing.Point(4, 28);
			this._line.Name = "_line";
			this._line.Size = new System.Drawing.Size(136, 1);
			this._line.TabIndex = 6;
			// 
			// _pnlTexture
			// 
			this._pnlTexture.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this._pnlTexture.Controls.Add(this._lblInfo);
			this._pnlTexture.Location = new System.Drawing.Point(0, 28);
			this._pnlTexture.Name = "_pnlTexture";
			this._pnlTexture.Size = new System.Drawing.Size(144, 68);
			this._pnlTexture.TabIndex = 8;
			this._pnlTexture.Visible = false;
			// 
			// _lblInfo
			// 
			this._lblInfo.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this._lblInfo.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
			this._lblInfo.Location = new System.Drawing.Point(12, 4);
			this._lblInfo.Name = "_lblInfo";
			this._lblInfo.Size = new System.Drawing.Size(124, 24);
			this._lblInfo.TabIndex = 12;
			this._lblInfo.Text = "Design-time editing for texture brushes is not supported.";
			// 
			// BrushEdit
			// 
			this.Controls.Add(this._pnlTexture);
			this.Controls.Add(this._btnTexture);
			this.Controls.Add(this._line);
			this.Controls.Add(this._preview);
			this.Controls.Add(this._btnGradient);
			this.Controls.Add(this._btnSolid);
			this.Controls.Add(this._btnHatch);
			this.Controls.Add(this._pnlSolid);
			this.Controls.Add(this._pnlHatch);
			this.Controls.Add(this._pnlGradient);
			this.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.Name = "BrushEdit";
			this.Size = new System.Drawing.Size(144, 96);
			this._pnlSolid.ResumeLayout(false);
			this._pnlGradient.ResumeLayout(false);
			this._pnlHatch.ResumeLayout(false);
			this._pnlTexture.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion


		[Browsable(false)]
		[Category("Data")]
		[Description("Gets or sets the brush to be edited.")]
		public MindFusion.FlowChartX.Brush Brush
		{
			get
			{
				if (_btnSolid.Checked)
				{
					return new MindFusion.FlowChartX.SolidBrush(
						TextToColor(_textColor.Text));
				}

				else if (_btnHatch.Checked)
				{
					return new MindFusion.FlowChartX.HatchBrush(
						_hatchCombo.SelectedStyle,
						TextToColor(_textForeColor.Text),
						TextToColor(_textBackColor.Text));
				}

				else if (_btnGradient.Checked)
				{
					return new MindFusion.FlowChartX.LinearGradientBrush(
						TextToColor(_textStartColor.Text),
						TextToColor(_textEndColor.Text),
						360 - _angle.Angle);
				}

				else if (_btnTexture.Checked)
				{
					return null;
				}

				return null;
			}
			set
			{
				if (value is MindFusion.FlowChartX.SolidBrush)
				{
					_btnSolid.Checked = true;
					UpdateUI();

					MindFusion.FlowChartX.SolidBrush brush =
						value as MindFusion.FlowChartX.SolidBrush;
					string color = ColorToText(brush.Color);

					_textColor.Text = color;
					_textBackColor.Text = color;
					_textForeColor.Text = color;
					_textStartColor.Text = color;
					_textEndColor.Text = color;
				}

				else if (value is MindFusion.FlowChartX.HatchBrush)
				{
					_btnHatch.Checked = true;
					UpdateUI();

					MindFusion.FlowChartX.HatchBrush brush =
						value as MindFusion.FlowChartX.HatchBrush;
					string fcolor = ColorToText(brush.ForegroundColor);
					string bcolor = ColorToText(brush.BackgroundColor);

					_hatchCombo.SelectedStyle = brush.HatchStyle;

					_textColor.Text = fcolor;
					_textBackColor.Text = bcolor;
					_textForeColor.Text = fcolor;
					_textStartColor.Text = bcolor;
					_textEndColor.Text = fcolor;
				}

				else if (value is MindFusion.FlowChartX.LinearGradientBrush)
				{
					_btnGradient.Checked = true;
					UpdateUI();

					MindFusion.FlowChartX.LinearGradientBrush brush =
						value as MindFusion.FlowChartX.LinearGradientBrush;
					string scolor = ColorToText(brush.LinearColors[0]);
					string ecolor = ColorToText(brush.LinearColors[1]);

					_angle.Angle = 360 - brush.Angle;
					_lblAngleValue.Text = ((int)_angle.Angle).ToString() + "°";

					_textColor.Text = scolor;
					_textBackColor.Text = scolor;
					_textForeColor.Text = ecolor;
					_textStartColor.Text = scolor;
					_textEndColor.Text = ecolor;
				}

				else if (value is MindFusion.FlowChartX.TextureBrush)
				{
					_btnTexture.Checked = true;
					UpdateUI();
				}
			}
		}

		
		private void OnAngleChanged(object sender, System.EventArgs e)
		{
			_lblAngleValue.Text = ((int)_angle.Angle).ToString() + "°";

			UpdatePreview();
		}

		private void OnSolid(object sender, System.EventArgs e)
		{
			UpdateUI();
		}

		private void OnGradient(object sender, System.EventArgs e)
		{
			UpdateUI();
		}

		private void OnHatch(object sender, System.EventArgs e)
		{
			UpdateUI();
		}

		private void OnTexture(object sender, System.EventArgs e)
		{
			UpdateUI();
		}

		private void OnHatchChanged(object sender, System.EventArgs e)
		{
			UpdatePreview();
		}

		private void OnColorChanged(object sender, System.EventArgs e)
		{
			TextBox text = sender as TextBox;

			text.Text = ColorToText(TextToColor(text.Text));

			UpdatePreview();
		}


		private void UpdatePreview()
		{
			MindFusion.FlowChartX.Brush brush = Brush;
			if (brush == null)
				return;

			if (_temp != null)
				_temp.Dispose();

			_temp = brush.CreateGDIBrush(_preview.ClientRectangle);
			_preview.Brush = _temp;
		}

		private void UpdateUI()
		{
			_btnSolid.BackColor = _btnSolid.Checked ? Color.Lime : this.BackColor;
			_btnGradient.BackColor = _btnGradient.Checked ? Color.Lime : this.BackColor;
			_btnHatch.BackColor = _btnHatch.Checked ? Color.Lime : this.BackColor;
			_btnTexture.BackColor = _btnTexture.Checked ? Color.Lime : this.BackColor;

			_pnlSolid.Visible = _btnSolid.Checked;
			_pnlGradient.Visible = _btnGradient.Checked;
			_pnlHatch.Visible = _btnHatch.Checked;
			_pnlTexture.Visible = _btnTexture.Checked;

			UpdatePreview();
		}

		private string ColorToText(Color color)
		{
			string result = "";

			if (color.A != 255)
				result += color.A.ToString() + ", ";
			result += color.R.ToString() + ", ";
			result += color.G.ToString() + ", ";
			result += color.B.ToString();

			return result;
		}

		private Color TextToColor(string text)
		{
			string[] channels = text.Split(new char[] { ',' });

			if (channels.Length != 3 && channels.Length != 4)
				return Color.White;

			int a = 255, r, g, b;
			try
			{
				int i = 0;
				if (channels.Length == 4)
					a = int.Parse(channels[i++]);

				r = int.Parse(channels[i++]);
				g = int.Parse(channels[i++]);
				b = int.Parse(channels[i++]);

				if (a > 255)
					a = 255;
				if (a < 0)
					a = 0;
				if (r > 255)
					r = 255;
				if (r < 0)
					r = 0;
				if (g > 255)
					g = 255;
				if (g < 0)
					g = 0;
				if (b > 255)
					b = 255;
				if (b < 0)
					b = 0;
			}
			catch
			{
				a = r = g = b = 255;
			}

			return Color.FromArgb(a, r, g, b);
		}


		private System.Drawing.Brush _temp = null;
	}
}
