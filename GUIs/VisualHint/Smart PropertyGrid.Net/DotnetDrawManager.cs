// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

#region Using directives

using System.Drawing;
using System.Windows.Forms;
using Skybound.Windows.Forms;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Drawing manager that defines the overall appearance of the PropertyGrid like in Visual Studio.
    /// </summary>
    public class DotnetDrawManager : CustomDrawManager
    {
        private bool _drawSubCategoryBackground = true;

        private bool _drawSelectedStateAsFocusRect = true;

        public bool DrawSelectedStateAsFocusRect
        {
            get { return _drawSelectedStateAsFocusRect; }
            set { _drawSelectedStateAsFocusRect = value; }
        }

        public override void DrawButtonsBackground(Graphics graphics, Rectangle rect)
        {
            using(SolidBrush brush = new SolidBrush(Grid.BackColor))
                graphics.FillRectangle(brush, rect);
        }

        public override void DrawCommentsBackground(Graphics graphics, Rectangle commentRect)
        {
            using(Brush brush = new SolidBrush(Grid.CommentsBackColor))
                graphics.FillRectangle(brush, commentRect);

            commentRect.Width--;
            commentRect.Height--;
            graphics.DrawRectangle(SystemPens.FromSystemColor(SystemColors.GrayText), commentRect);
        }

        public override void DrawCommentsGap(Graphics graphics, Rectangle rect)
        {
            using(SolidBrush brush = new SolidBrush(Grid.BackColor))
                graphics.FillRectangle(brush, rect);
        }

        public override void DrawGridBackground(Graphics graphics, Rectangle rect)
        {
            Rectangle fillRect = Rectangle.Intersect(rect, Rectangle.Round(graphics.ClipBounds));
            if (fillRect.IsEmpty)
                return;

            using(Brush brush = new SolidBrush(Grid.GridBackColor))
                graphics.FillRectangle(brush, fillRect);
        }

        public override void DrawLeftColumnBackground(Graphics graphics, Rectangle rect, PropertyEnumerator propEnum)
        {
            Rectangle fillRect = Rectangle.Intersect(rect, Rectangle.Round(graphics.ClipBounds));
            if (fillRect.IsEmpty)
                return;

            using(SolidBrush brush = new SolidBrush(Grid.GridColor))
                graphics.FillRectangle(brush, fillRect);
        }

        public override void NcCalcSize(ref Rectangle rect)
        {
            rect.Inflate(-1, -1);
        }

        public override void DrawGridNonClientArea(Graphics graphics, Rectangle rect)
        {
            rect.Width--;
            rect.Height--;
            Pen pen = new Pen(SystemColors.GrayText);
            graphics.DrawRectangle(pen, rect);
            pen.Dispose();
        }

        public override void DrawCategoryLabelBackground(Graphics graphics, Rectangle rect, PropertyEnumerator enumSelf)
        {
            Rectangle fillRect = Rectangle.Intersect(rect, Rectangle.Round(graphics.ClipBounds));
            if (fillRect.IsEmpty)
                return;

            Brush brush;

            if (DrawSelectedStateAsFocusRect)
                brush = new SolidBrush(Grid.GridColor);
            else
            {
                if (enumSelf.Property.Selected)	// if selected
                {
                    Color bkgColor;

                    if (Grid.GridContainsFocus)
                    {
                        bkgColor = Grid.SelectedBackColor;
                    }
                    else
                    {
                        // If the control has not the focus, the background is light
                        bkgColor = Grid.SelectedNotFocusedBackColor;
                    }

                    brush = new SolidBrush(bkgColor);
                }
                else
                    brush = new SolidBrush(Grid.GridColor);
            }

            graphics.FillRectangle(brush, fillRect);

            brush.Dispose();
        }

        public override void DrawCategoryLabelText(Graphics graphics, Rectangle rect, Color textColor, PropertyEnumerator enumSelf)
        {
            // Set the font to bold for categories
            // We use the font of the properties control for that
            using (Font boldFont = new Font(Grid.Font, FontStyle.Bold))
            {
                if (DrawSelectedStateAsFocusRect)
                    textColor = enumSelf.Property.ForeColor;

                Win32Calls.DrawText(graphics, enumSelf.Property.DisplayName, ref rect, boldFont, textColor,
                    Win32Calls.DT_SINGLELINE | Win32Calls.DT_VCENTER | Win32Calls.DT_NOPREFIX |
                    ((Grid.EllipsisMode & PropertyGrid.EllipsisModes.EllipsisOnLabels) != 0 ? Win32Calls.DT_END_ELLIPSIS : 0));


                if (DrawSelectedStateAsFocusRect && enumSelf.Property.Selected && Grid.GridContainsFocus)	// if selected
                {
                    Size size = Win32Calls.GetTextExtent(graphics, enumSelf.Property.DisplayName, boldFont);

                    Rectangle focusRect = rect;
                    focusRect.Y++;
                    focusRect.Height -= 2;
                    int margin = enumSelf.Property.ParentGrid.GlobalTextMargin;
                    focusRect.X -= margin / 2 + 1;
                    focusRect.Width = margin + size.Width;
                    if (focusRect.Right > rect.Right)
                        focusRect.Width = rect.Right - focusRect.X;

                    ControlPaint.DrawFocusRectangle(graphics, focusRect);
                }
            }
        }

        public override void DrawSubCategoryLabelText(Graphics graphics, Rectangle rect, Color textColor, PropertyEnumerator enumSelf)
        {
            Property property = enumSelf.Property;

            // Draw the category label
            //------------------------

            if (DrawSelectedStateAsFocusRect)
                textColor = enumSelf.Property.ForeColor;

            Win32Calls.DrawText(graphics, property.DisplayName, ref rect, Grid.Font, textColor,
                Win32Calls.DT_SINGLELINE | Win32Calls.DT_VCENTER | Win32Calls.DT_END_ELLIPSIS | Win32Calls.DT_NOPREFIX);

            if (DrawSelectedStateAsFocusRect && enumSelf.Property.Selected && Grid.GridContainsFocus)	// if selected
            {
                Size size = Win32Calls.GetTextExtent(graphics, enumSelf.Property.DisplayName, Grid.Font);

                Rectangle focusRect = rect;
                focusRect.Y++;
                focusRect.Height -= 2;
                int margin = enumSelf.Property.ParentGrid.GlobalTextMargin;
                focusRect.X -= margin / 2 + 1;
                focusRect.Width = margin + size.Width;
                if (focusRect.Right > rect.Right)
                    focusRect.Width = rect.Right - focusRect.X;

                ControlPaint.DrawFocusRectangle(graphics, focusRect);
            }
        }

        public override void DrawSubCategoryLabelBackground(Graphics graphics, int labelRectX, Rectangle rect, PropertyEnumerator enumSelf)
        {
            Rectangle fillRect = Rectangle.Intersect(rect, Rectangle.Round(graphics.ClipBounds));
            if (fillRect.IsEmpty)
                return;

            Brush brush;
            if (DrawSelectedStateAsFocusRect)
            {
                if (_drawSubCategoryBackground)
                {
                    double H, S, L;
                    ColorUtils.RGBtoHSL(Grid.GridColor, out H, out S, out L);
                    Color newBkg = ColorUtils.HSLtoRGB(H, S, L + (1-L) / 2.0);
                    brush = new SolidBrush(newBkg);
                }
                else
                    brush = new SolidBrush(Grid.GridColor);
            }
            else
            {
                if (enumSelf.Property.Selected)	// if selected
                {
                    Color bkgColor;

                    if (Grid.GridContainsFocus)
                    {
                        bkgColor = Grid.SelectedBackColor;
                    }
                    else
                    {
                        // If the control has not the focus, the background is light
                        bkgColor = Grid.SelectedNotFocusedBackColor;
                    }

                    brush = new SolidBrush(bkgColor);
                }
                else
                {
                    if (_drawSubCategoryBackground)
                    {
                        double H, S, L;
                        ColorUtils.RGBtoHSL(Grid.GridColor, out H, out S, out L);
                        Color newBkg = ColorUtils.HSLtoRGB(H, S, 230.0 / 255.0);
                        brush = new SolidBrush(newBkg);
                    }
                    else
                        brush = new SolidBrush(Grid.GridColor);
                }
            }

            graphics.FillRectangle(brush, fillRect);

            brush.Dispose();
        }

        public override bool ShowCategoryVerticalLine
        {
			get { return false; }
		}

        public override void DrawCategoryValue(Graphics graphics, Rectangle valueRect, Color textColor, PropertyEnumerator enumSelf)
        {
            // Set the font to bold for categories
            // We use the font of the properties control for that
            using (Font boldFont = new Font(Grid.Font, FontStyle.Bold))
            {
                if (DrawSelectedStateAsFocusRect)
                    textColor = enumSelf.Property.ForeColor;

                Win32Calls.DrawText(graphics, ((RootCategory)enumSelf.Property).ValueText,
                    ref valueRect, boldFont, textColor,
                    Win32Calls.DT_SINGLELINE | Win32Calls.DT_VCENTER | Win32Calls.DT_NOPREFIX |
                    ((Grid.EllipsisMode & PropertyGrid.EllipsisModes.EllipsisOnValues) != 0 ? Win32Calls.DT_END_ELLIPSIS : 0));
            }
        }

        public override void DrawPlusMinusSign(Graphics graphics, Control control, Rectangle itemRect, PropertyEnumerator propEnum)
        {
            Property property = propEnum.Property;

            Rectangle signRect = property.GetSignRect(itemRect, propEnum);
            if (Rectangle.Intersect(signRect, Rectangle.Round(graphics.ClipBounds)).IsEmpty)
                return;

			if (ThemeRenderer.Enabled)
				base.DrawPlusMinusSign(graphics, control, itemRect, propEnum);
            else
            {
                // Draw a square
                Color color;
                if (property.Selected && (propEnum.Depth > 0))
                {
                    if (((Grid.GridContainsFocus || (propEnum.Property.InPlaceCtrlInAction != null)) &&
                        (!DrawSelectedStateAsFocusRect || !(propEnum.Property is SubCategory))))
                        color = Grid.HighlightedTextColor;
                    else
                        color = property.ForeColor;
                }
                else
                    color = property.ForeColor;

                using (Pen pen = new Pen(color))
                {
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
                }
            }
        }
    }
}
