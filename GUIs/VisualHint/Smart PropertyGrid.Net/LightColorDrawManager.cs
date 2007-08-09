// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

#region Using directives
#endregion

using System.Drawing.Drawing2D;
using System.Drawing;
using System.Windows.Forms;
namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Drawing manager that defines the overall appearance of the PropertyGrid like in the Sony Vegas
    /// video editing software.
    /// </summary>
    public class LightColorDrawManager : DefaultDrawManager
    {
        private Color _categoryBkgColor1;
        private Color _categoryBkgColor2;
        private Color _subCategoryBkgColor1;
        private Color _subCategoryBkgColor2;

        private bool _useBoldFontForCategories = false;

        public bool UseBoldFontForCategories
        {
            set { _useBoldFontForCategories = value; }
            get { return _useBoldFontForCategories; }
        }

        public LightColorDrawManager()
        {
    	    _categoryBkgColor1 = SystemColors.ControlDark;
            _categoryBkgColor2 = Color.White;
            _subCategoryBkgColor1 = SystemColors.Control;
            _subCategoryBkgColor2 = Color.White;
        }

        public void SetCategoryBackgroundColors(Color clr1, Color clr2)
        {
            _categoryBkgColor1 = clr1;
            _categoryBkgColor2 = clr2;
        }

        public void SetSubCategoryBackgroundColors(Color clr1, Color clr2)
        {
            _subCategoryBkgColor1 = clr1;
            _subCategoryBkgColor2 = clr2;
        }

        protected Color CategoryLeftBackgroundColor { get { return _categoryBkgColor1; } }
        protected Color CategoryRightBackgroundColor { get { return _categoryBkgColor2; } }
        protected Color SubCategoryLeftBackgroundColor { get { return _subCategoryBkgColor1; } }
        protected Color SubCategoryRightBackgroundColor { get { return _subCategoryBkgColor2; } }

        public override bool ShowCategoryVerticalLine
        {
            get { return false; }
        }

        public override void DrawLeftColumnBackground(Graphics graphics, Rectangle rect, PropertyEnumerator propEnum)
        {
            Rectangle fillRect = Rectangle.Intersect(rect, Rectangle.Round(graphics.ClipBounds));
            if (fillRect.IsEmpty)
                return;

            SolidBrush brush = new SolidBrush(Grid.GridBackColor);
            graphics.FillRectangle(brush, fillRect);
            brush.Dispose();
        }

        public override void DrawCategoryLabelBackground(Graphics graphics, Rectangle rect, PropertyEnumerator enumSelf)
        {
            Rectangle fillRect = Rectangle.Intersect(rect, Rectangle.Round(graphics.ClipBounds));
            if (fillRect.IsEmpty)
                return;

            Rectangle labelRect = enumSelf.Property.GetLabelColumnRect(rect, enumSelf);
            rect.Y++;
            rect.Height -= 3;

            fillRect = rect;
            int margin = enumSelf.Property.ParentGrid.GlobalTextMargin;
            fillRect.X = labelRect.Left + margin / 2;
            fillRect.Width = rect.Right - fillRect.Left;

            Brush brush;
            if (enumSelf.Property.Selected)	// if selected
                brush = new SolidBrush(Grid.GridContainsFocus ? Grid.SelectedBackColor : Grid.SelectedNotFocusedBackColor);
            else
                brush = new LinearGradientBrush(fillRect, _categoryBkgColor1, _categoryBkgColor2, 0.0f);

            graphics.FillRectangle(brush, fillRect);

            brush.Dispose();
        }

        public override void DrawSubCategoryLabelBackground(Graphics graphics, int labelRectX, Rectangle rect, PropertyEnumerator enumSelf)
        {
            Rectangle fillRect = Rectangle.Intersect(rect, Rectangle.Round(graphics.ClipBounds));
            if (fillRect.IsEmpty)
                return;

            Rectangle labelRect = enumSelf.Property.GetLabelColumnRect(rect, enumSelf);
            rect.Y++;
            rect.Height -= 3;

            int margin = enumSelf.Property.ParentGrid.GlobalTextMargin;
            fillRect = rect;
            Rectangle signRect = enumSelf.Property.GetSignRect(rect, enumSelf);
            if (signRect != Rectangle.Empty)
                fillRect.X = signRect.Right + margin / 2;
            else
                fillRect.X = labelRect.Left + margin / 2;
            fillRect.Width = rect.Right - fillRect.Left;

            Brush brush;
            if (enumSelf.Property.Selected)	// if selected
                brush = new SolidBrush(Grid.GridContainsFocus ? Grid.SelectedBackColor : Grid.SelectedNotFocusedBackColor);
            else
                brush = new LinearGradientBrush(fillRect, _subCategoryBkgColor1, _subCategoryBkgColor2, 0.0f);

            graphics.FillRectangle(brush, fillRect);

            brush.Dispose();
        }

        public override void DrawPlusMinusSign(Graphics graphics, Control control, Rectangle itemRect, PropertyEnumerator propEnum)
        {
            Property property = propEnum.Property;

            Rectangle signRect = property.GetSignRect(itemRect, propEnum);
            if (Rectangle.Intersect(signRect, Rectangle.Round(graphics.ClipBounds)).IsEmpty)
                return;

		    // Draw a square
            Brush brush = new SolidBrush(propEnum.Property.ParentGrid.GridBackColor);
            Rectangle fillRect = signRect; fillRect.X++; fillRect.Y++; fillRect.Width--; fillRect.Height--;
            graphics.FillRectangle(brush, fillRect);
            brush.Dispose();

            Pen pen = new Pen(_categoryBkgColor1);
            graphics.DrawRectangle(pen, signRect);
            pen.Dispose();

		    // Find the center of this square
            Point ptCenter = new Point(signRect.X + signRect.Width / 2, signRect.Y + signRect.Height / 2);

            pen = new Pen(Color.Black);
            graphics.DrawLine(pen, ptCenter.X - 2, ptCenter.Y, ptCenter.X + 2, ptCenter.Y);

		    // If the children are hidden, we have to draw the +
		    if (!property.Expanded)
		    {
			    // Draw a vertical bar (plus)
                graphics.DrawLine(pen, ptCenter.X, ptCenter.Y - 2, ptCenter.X, ptCenter.Y + 2);
		    }

            pen.Dispose();
        }

        public override void DrawCategoryLabelText(Graphics graphics, Rectangle rect, Color textColor, PropertyEnumerator enumSelf)
        {
            // Set the font to bold for categories if needed.
            // We use the font of the properties control for that
            Font usedFont = (_useBoldFontForCategories ? new Font(Grid.Font, FontStyle.Bold) : Grid.Font);

            // Draw the category label
            //------------------------

            Win32Calls.DrawText(graphics, enumSelf.Property.DisplayName, ref rect, usedFont, textColor,
                Win32Calls.DT_SINGLELINE | Win32Calls.DT_VCENTER | Win32Calls.DT_NOPREFIX |
                ((Grid.EllipsisMode & PropertyGrid.EllipsisModes.EllipsisOnLabels) != 0 ? Win32Calls.DT_END_ELLIPSIS : 0));

            if (_useBoldFontForCategories)
                usedFont.Dispose();
        }

        public override void DrawCategoryValue(Graphics graphics, Rectangle valueRect, Color textColor, PropertyEnumerator enumSelf)
        {
            // Set the font to bold for categories if needed.
            // We use the font of the properties control for that
            Font usedFont = (_useBoldFontForCategories ? new Font(Grid.Font, FontStyle.Bold) : Grid.Font);

            // Draw the category info text
            //----------------------------

            Win32Calls.DrawText(graphics, ((RootCategory)enumSelf.Property).ValueText,
                ref valueRect, usedFont, textColor,
                Win32Calls.DT_SINGLELINE | Win32Calls.DT_VCENTER | Win32Calls.DT_NOPREFIX |
                ((Grid.EllipsisMode & PropertyGrid.EllipsisModes.EllipsisOnValues) != 0 ? Win32Calls.DT_END_ELLIPSIS : 0));

            if (_useBoldFontForCategories)
                usedFont.Dispose();
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

        public override void DrawButtonsBackground(Graphics graphics, Rectangle rect)
        {
            SolidBrush brush = new SolidBrush(Grid.BackColor);
            graphics.FillRectangle(brush, rect);
            brush.Dispose();
        }

        public override void DrawCommentsBackground(Graphics graphics, Rectangle commentRect)
        {
            Brush brush = new SolidBrush(Grid.CommentsBackColor);
            graphics.FillRectangle(brush, commentRect);

            commentRect.Width--;
            commentRect.Height--;
            graphics.DrawRectangle(SystemPens.FromSystemColor(SystemColors.GrayText), commentRect);

            brush.Dispose();
        }

        public override void DrawCommentsGap(Graphics graphics, Rectangle rect)
        {
            SolidBrush brush = new SolidBrush(Grid.BackColor);
            graphics.FillRectangle(brush, rect);
            brush.Dispose();
        }

        public override void DrawButtonsGap(Graphics graphics, Rectangle commentRect) { }

        public override void DrawPropertyLabelBackground(Graphics graphics, Rectangle labelRect, PropertyEnumerator enumSelf)
        {
            Rectangle fillRect = Rectangle.Intersect(labelRect, Rectangle.Round(graphics.ClipBounds));
            if (fillRect.IsEmpty)
                return;

            Brush brush = new SolidBrush(Grid.GridBackColor);
            graphics.FillRectangle(brush, labelRect);
            brush.Dispose();

            Rectangle textRect = enumSelf.Property.GetLabelTextRect(labelRect, enumSelf);
            int margin = enumSelf.Property.ParentGrid.GlobalTextMargin;
            textRect.X -= margin / 2;
            textRect.Width = labelRect.Right - textRect.Left - 1;
            textRect.Y += 1;
            textRect.Height -= 3;

            if (enumSelf.Property.Selected)	// if selected
            {
                Color bkgColor;

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

            graphics.FillRectangle(brush, textRect);
            brush.Dispose();
        }

        public override void DrawSeparationLines(Graphics graphics, Rectangle itemRect, Rectangle labelRect, PropertyEnumerator enumSelf)
        {
			Pen pen = new Pen(Grid.GridColor);

	        // Draw the lines
			Property property = enumSelf.Property;
			if ((enumSelf.Depth > 0) || ShowCategoryVerticalLine)
				graphics.DrawLine(pen, labelRect.Right, labelRect.Top, labelRect.Right, labelRect.Bottom);

            int margin = enumSelf.Property.ParentGrid.GlobalTextMargin;
            Rectangle checkboxRect = property.GetManuallyDisableRect(itemRect, enumSelf);
            if (checkboxRect != Rectangle.Empty)
            {
                graphics.DrawLine(pen, checkboxRect.Left - margin, itemRect.Bottom - 1,
                    itemRect.Right, itemRect.Bottom - 1);
            }
            else
            {
                Rectangle imgRect = property.GetImageRect(itemRect, enumSelf);
                if (imgRect != Rectangle.Empty)
                {
                    graphics.DrawLine(pen, imgRect.Left - margin, itemRect.Bottom - 1,
                        itemRect.Right, itemRect.Bottom - 1);
                }
                else
                {
                    Rectangle labelTextRect = property.GetLabelTextRect(itemRect, enumSelf);
                    graphics.DrawLine(pen, labelTextRect.Left - margin, itemRect.Bottom - 1,
                        itemRect.Right, itemRect.Bottom - 1);
                }
            }

            pen.Dispose();
        }
    }
}
