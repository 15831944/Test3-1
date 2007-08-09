// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

using System;
using System.Drawing;
using System.Windows.Forms;
using Skybound.Windows.Forms;

namespace VisualHint.SmartPropertyGrid
{
	/// <summary>
	/// Base class for drawing manager classes that are used to modify the overall appearance of the PropertyGrid.
	/// </summary>
	public class CustomDrawManager
	{
		private PropertyGrid _grid;

        public char PasswordChar
        {
            get
            {
                return (Environment.OSVersion.Version.Major > 4 ? '\u25CF' /*'\u2022'*/ : '*');
            }
        }

        public virtual int CommentsGapHeight
        {
            get { return 6; }
        }

        public PropertyGrid Grid
		{
			set { _grid = value; }
			get { return _grid; }
		}

		public virtual void DrawCommentsGap(Graphics graphics, Rectangle commentRect) {}

		public virtual void DrawCommentsBackground(Graphics graphics, Rectangle commentRect) {}

		public virtual void DrawCommentText(Graphics graphics, Rectangle rect, PropertyVisibleDeepEnumerator enumerator)
		{
            Font font = Grid.Font;

            using (Font boldFont = new Font(font, FontStyle.Bold))
            {
                Win32Calls.DrawText(graphics, enumerator.Property.DisplayName, ref rect, boldFont, Grid.CommentsForeColor,
                    Win32Calls.DT_LEFT | Win32Calls.DT_SINGLELINE | Win32Calls.DT_TOP | Win32Calls.DT_NOPREFIX);

                Size size = Win32Calls.GetTextExtent(graphics, enumerator.Property.DisplayName, boldFont);
                rect.Y += size.Height + 3;
                rect.Height -= size.Height + 3;
            }

            string comment = enumerator.Property.Comment;
#if _SPGEVAL
            comment += "\nEvaluation version from VisualHint.\nPurchase it at http://www.visualhint.com";
#endif
            Win32Calls.DrawText(graphics, comment, ref rect, font, Grid.CommentsForeColor,
                Win32Calls.DT_LEFT | Win32Calls.DT_WORDBREAK | Win32Calls.DT_TOP | Win32Calls.DT_NOPREFIX);
        }

        public virtual void DrawButtonsGap(Graphics graphics, Rectangle commentRect) {}

        public virtual void DrawButtonsBackground(Graphics graphics, Rectangle rect) {}

        public virtual void DrawGridBackground(Graphics graphics, Rectangle rect) { }

        public virtual void DrawPropertyValueBackground(Graphics graphics, Rectangle rect, PropertyEnumerator enumerator)
        {
            Rectangle fillRect = Rectangle.Intersect(rect, Rectangle.Round(graphics.ClipBounds));
            if (fillRect.IsEmpty)
                return;

            Brush brush = new SolidBrush(enumerator.Property.Value.BackColor);
            graphics.FillRectangle(brush, fillRect);
            brush.Dispose();
        }

        public virtual void DrawLeftColumnBackground(Graphics graphics, Rectangle rect, PropertyEnumerator propEnum) { }

        public virtual void NcCalcSize(ref Rectangle rect) { }

        public virtual void DrawGridNonClientArea(Graphics graphics, Rectangle rect) { }

        public virtual void DrawSeparationLines(Graphics graphics, Rectangle itemRect, Rectangle labelRect, PropertyEnumerator enumSelf)
        {
            using (Pen pen = new Pen(Grid.GridColor))
            {
                // Draw the lines
                Property property = enumSelf.Property;
                if ((enumSelf.Depth > 0) || ShowCategoryVerticalLine)
                    graphics.DrawLine(pen, labelRect.Right, labelRect.Top, labelRect.Right, labelRect.Bottom - 1);
                graphics.DrawLine(pen, itemRect.Left + Grid.LeftColumnWidth + 1, itemRect.Bottom - 1,
                    itemRect.Right, itemRect.Bottom - 1);
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="graphics"></param>
        /// <param name="control"></param>
        /// <param name="itemRect"></param>
        /// <param name="propEnum"></param>
        public virtual void DrawPlusMinusSign(Graphics graphics, Control control, Rectangle itemRect, PropertyEnumerator propEnum)
        {
            Property property = propEnum.Property;

            Rectangle signRect = property.GetSignRect(itemRect, propEnum);
            if (Rectangle.Intersect(signRect, Rectangle.Round(graphics.ClipBounds)).IsEmpty)
                return;

			if (ThemeRenderer.Enabled)
			{
                signRect.Inflate(2, 2);
                int state = (property.Expanded ? ThemeTreeView.GlyphOpened : ThemeTreeView.GlyphClosed);

				ThemeTreeView.Glyph.Draw(graphics, state, signRect);
            }
            else
            {
                // Draw a square
                Color color;
                if (property.Selected && (propEnum.Depth > 0))
//                if (property.Selected && enumerator.HasParent())
                {
                    if (Grid.GridContainsFocus || (propEnum.Property.InPlaceCtrlInAction != null))
                        color = Grid.HighlightedTextColor;
                    else
                        color = property.ForeColor;
                }
                else
                    color = property.ForeColor;

                Pen pen = new Pen(color);
                graphics.DrawRectangle(pen, signRect);

                // Find the center of this square
                Point ptCenter = new Point(signRect.X + signRect.Width / 2, signRect.Y + signRect.Height / 2);

                // Draw a horizontal bar (minus)
                graphics.DrawLine(pen, ptCenter.X - 2, ptCenter.Y, ptCenter.X + 2, ptCenter.Y);

                // If the children are hidden, we have to draw the +
                if (!property.Expanded)
                {
                    // Draw a vertical bar (plus)
                    graphics.DrawLine(pen, ptCenter.X, ptCenter.Y - 2, ptCenter.X, ptCenter.Y + 2);
                }

                pen.Dispose();
            }
        }

        public virtual void DrawCategoryLabelBackground(Graphics graphics, Rectangle rect, PropertyEnumerator enumSelf)
        {
		}

        public virtual void DrawCategoryLabelText(Graphics graphics, Rectangle rect, Color textColor, PropertyEnumerator enumSelf)
        {
            // Set the font to bold for categories
            // We use the font of the properties control for that
            using (Font boldFont = new Font(Grid.Font, FontStyle.Bold))
            {
                Win32Calls.DrawText(graphics, enumSelf.Property.DisplayName, ref rect, boldFont, textColor,
                    Win32Calls.DT_SINGLELINE | Win32Calls.DT_VCENTER | Win32Calls.DT_NOPREFIX |
                    ((Grid.EllipsisMode & PropertyGrid.EllipsisModes.EllipsisOnLabels) != 0 ? Win32Calls.DT_END_ELLIPSIS : 0));
            }
        }

        public virtual void DrawCategoryValue(Graphics graphics, Rectangle valueRect, Color textColor, PropertyEnumerator enumSelf)
        {
            // Set the font to bold for categories
            // We use the font of the properties control for that
            using (Font boldFont = new Font(Grid.Font, FontStyle.Bold))
            {
                Win32Calls.DrawText(graphics, ((RootCategory)enumSelf.Property).ValueText,
                    ref valueRect, boldFont, textColor,
                    Win32Calls.DT_SINGLELINE | Win32Calls.DT_VCENTER | Win32Calls.DT_NOPREFIX |
                    ((_grid.EllipsisMode & PropertyGrid.EllipsisModes.EllipsisOnValues) != 0 ? Win32Calls.DT_END_ELLIPSIS : 0));
            }
        }

        public virtual void DrawSubCategoryLabelBackground(Graphics graphics, int labelRectX, Rectangle rect, PropertyEnumerator enumSelf)
        {
        }

        public virtual void DrawSubCategoryLabelText(Graphics graphics, Rectangle rect, Color textColor, PropertyEnumerator enumSelf)
        {
            Win32Calls.DrawText(graphics, enumSelf.Property.DisplayName, ref rect, Grid.Font, textColor,
                Win32Calls.DT_SINGLELINE | Win32Calls.DT_VCENTER | Win32Calls.DT_NOPREFIX |
                ((Grid.EllipsisMode & PropertyGrid.EllipsisModes.EllipsisOnLabels) != 0 ? Win32Calls.DT_END_ELLIPSIS : 0));
        }

		public virtual bool ShowCategoryVerticalLine
		{
			get { return true; }
		}


        public virtual void DrawPropertyLabelBackground(Graphics graphics, Rectangle labelRect, PropertyEnumerator enumSelf)
        {
            Rectangle fillRect = Rectangle.Intersect(labelRect, Rectangle.Round(graphics.ClipBounds));
            if (fillRect.IsEmpty)
                return;

            Brush brush;

            if (enumSelf.Property.Selected) 	// if selected
            {
                Color bkgColor;

                Control topLevelWindow = Control.FromHandle(Win32Calls.GetForegroundWindow());
                bool b = Grid.Grid.ContainsFocus;

                if ((Grid.GridContainsFocus) || (enumSelf.Property.InPlaceCtrlInAction != null))
                    bkgColor = Grid.SelectedBackColor;
                else
                {
                    // If the control has not the focus, the background is light
                    bkgColor = Grid.SelectedNotFocusedBackColor;
                }

                brush = new SolidBrush(bkgColor);
            }
            else
                brush = new SolidBrush(enumSelf.Property.BackColor);

            graphics.FillRectangle(brush, fillRect);

            brush.Dispose();
        }

        public virtual void DrawPropertyLabelText(Graphics graphics, Rectangle rect, Color textColor, PropertyVisibleDeepEnumerator enumSelf)
        {
            if (Rectangle.Intersect(rect, Rectangle.Round(graphics.ClipBounds)).IsEmpty)
                return;

            rect.Height = rect.Height / enumSelf.Property.HeightMultiplier;

            // Draw the text, clipped by the given area
            Win32Calls.DrawText(graphics, enumSelf.Property.DisplayName, ref rect, enumSelf.Property.Font, textColor,
                Win32Calls.DT_SINGLELINE | Win32Calls.DT_VCENTER | Win32Calls.DT_NOPREFIX |
                ((_grid.EllipsisMode & PropertyGrid.EllipsisModes.EllipsisOnLabels) != 0 ? Win32Calls.DT_END_ELLIPSIS : 0));
        }

        public virtual void DrawRadioButton(Graphics graphics, Control control, Rectangle radioRect, bool drawChecked, bool drawEnabled)
        {
			if (ThemeRenderer.Enabled)
			{
                int state = (drawEnabled ?
                    (drawChecked ? ThemeButton.RadioButtonCheckedNormal : ThemeButton.RadioButtonUncheckedNormal) :
                    (drawChecked ? ThemeButton.RadioButtonCheckedDisabled : ThemeButton.RadioButtonUncheckedDisabled));

				ThemeButton.RadioButton.Draw(graphics, state, radioRect);
            }
            else
            {
                ButtonState state = (drawChecked ? ButtonState.Checked : 0);
                state |= (!drawEnabled ? ButtonState.Inactive : ButtonState.Normal);
                ControlPaint.DrawRadioButton(graphics, radioRect, state);
            }
        }

        public virtual void DrawCheckBox(Graphics graphics, Control control, Rectangle buttonRect, bool drawChecked, bool drawEnabled)
        {
            if (Rectangle.Intersect(buttonRect, Rectangle.Round(graphics.ClipBounds)).IsEmpty)
                return;

			if (ThemeRenderer.Enabled)
			{
				int state = (drawEnabled ?
					(drawChecked ? ThemeButton.CheckBoxCheckedNormal : ThemeButton.CheckBoxUncheckedNormal) :
					(drawChecked ? ThemeButton.CheckBoxCheckedDisabled : ThemeButton.CheckBoxUncheckedDisabled));

				ThemeButton.CheckBox.Draw(graphics, state, buttonRect);
			}
			else
            {
                ButtonState state = ButtonState.Normal;
                if (drawChecked)
                    state |= ButtonState.Checked;
                if (!drawEnabled)
                    state |= ButtonState.Inactive;

                ControlPaint.DrawCheckBox(graphics, buttonRect, state);
            }
        }

        public virtual void DrawCheckBoxIndeterminate(Graphics graphics, Control control, Rectangle buttonRect, bool drawEnabled)
        {
            if (Rectangle.Intersect(buttonRect, Rectangle.Round(graphics.ClipBounds)).IsEmpty)
                return;

			if (ThemeRenderer.Enabled)
			{
				int state = (drawEnabled ? ThemeButton.CheckBoxMixedNormal : ThemeButton.CheckBoxMixedDisabled);
				ThemeButton.CheckBox.Draw(graphics, state, buttonRect);
			}
            else
                ControlPaint.DrawCheckBox(graphics, buttonRect, ButtonState.Inactive);
        }

        public virtual void DrawProgressBar(Graphics graphics, Control control, Rectangle valueRect, bool drawFrame, int value, int minValue, int maxValue)
        {
			if (ThemeRenderer.Enabled)
			{
                if (drawFrame)
                {
                    ThemeRenderer renderer = ThemeProgress.Bar;
                    
                    renderer.Draw(graphics, 0, valueRect);

                    IntPtr hdc = graphics.GetHdc();
                    valueRect = renderer.GetBackgroundContentRectangle(hdc, 0, valueRect);
                    graphics.ReleaseHdc(hdc);
                }

                valueRect.Width = (int)Math.Round((double)(value - minValue) / (double)(maxValue - minValue) * valueRect.Width);
				ThemeProgress.Chunk.Draw(graphics, 0, valueRect);
            }
            else
            {
                Rectangle barRect;
                if (drawFrame)
                    barRect = Rectangle.Inflate(valueRect, -2, -2);
                else
                {
                    valueRect.Width -= 2;
                    barRect = Rectangle.Inflate(valueRect, 0, -2);
                }
                
                int maxRight = barRect.Right - 1;

                int segmentWidth = (int)Math.Round(valueRect.Height * .6666);
                int newSegmentCount = (int)Math.Ceiling(((double)barRect.Width / (double)segmentWidth) *
                    value / (maxValue - minValue));

                Brush barBrush = new SolidBrush(SystemColors.Highlight);

                for(int i = 0; i < newSegmentCount; i++)
				{
					barRect.Width = Math.Min(maxRight - barRect.X, segmentWidth - 2);
					graphics.FillRectangle(barBrush, barRect);
					barRect.X += segmentWidth;
				}

                barBrush.Dispose();

                if (drawFrame)
                    ControlPaint.DrawBorder3D(graphics, valueRect, Border3DStyle.Flat);
            }
        }
    }
}
