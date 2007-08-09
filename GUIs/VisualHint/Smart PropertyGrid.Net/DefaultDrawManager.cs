// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

#region Using directives

using System.Drawing;
#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Drawing manager that sets the overall appearance of the PropertyGrid in a very simplistic way.
    /// </summary>
    public class DefaultDrawManager : CustomDrawManager
    {
        public override int CommentsGapHeight
        {
            get { return 6; }
        }

        public override void DrawCommentsBackground(Graphics graphics, Rectangle commentRect)
        {
            SolidBrush brush = new SolidBrush(Grid.CommentsBackColor);
            graphics.FillRectangle(brush, commentRect);
            brush.Dispose();
        }

        public override void DrawCommentsGap(Graphics graphics, Rectangle rect)
        {
            Pen whitePen = new Pen(Color.White);
            graphics.DrawLine(whitePen, rect.Left, rect.Top, rect.Right, rect.Top);
            graphics.DrawLine(whitePen, rect.Left, rect.Top + 2, rect.Right, rect.Top + 2);
            whitePen.Dispose();

            Pen lightPen = SystemPens.FromSystemColor(SystemColors.ControlLight);
            graphics.DrawLine(lightPen, rect.Left, rect.Top + 1, rect.Right, rect.Top + 1);
            graphics.DrawLine(lightPen, rect.Left, rect.Top + 3, rect.Right, rect.Top + 3);

            Pen darkPen = SystemPens.FromSystemColor(SystemColors.ControlDark);
            graphics.DrawLine(darkPen, rect.Left, rect.Top + 4, rect.Right, rect.Top + 4);

            Pen darkDarkPen = SystemPens.FromSystemColor(SystemColors.ControlDarkDark);
            graphics.DrawLine(darkDarkPen, rect.Left, rect.Top + 5, rect.Right, rect.Top + 5);
        }

        public override void DrawButtonsBackground(Graphics graphics, Rectangle rect)
        {
            SolidBrush brush = new SolidBrush(SystemColors.Control);
            graphics.FillRectangle(brush, rect);
            brush.Dispose();
        }

        public override void DrawButtonsGap(Graphics graphics, Rectangle rect)
        {
            Pen whitePen = new Pen(Color.White);
            graphics.DrawLine(whitePen, rect.Left, rect.Bottom - 3, rect.Right, rect.Bottom - 3);
            whitePen.Dispose();

            Pen lightPen = SystemPens.FromSystemColor(SystemColors.ControlLight);
            graphics.DrawLine(lightPen, rect.Left, rect.Bottom - 2, rect.Right, rect.Bottom - 2);

            Pen darkPen = SystemPens.FromSystemColor(SystemColors.ControlDark);
            graphics.DrawLine(darkPen, rect.Left, rect.Bottom - 1, rect.Right, rect.Bottom - 1);
        }

        public override void DrawGridBackground(Graphics graphics, Rectangle rect)
        {
            SolidBrush brush = new SolidBrush(Grid.GridBackColor);
            graphics.FillRectangle(brush, rect);
            brush.Dispose();
        }

        public override void DrawLeftColumnBackground(Graphics graphics, Rectangle rect, PropertyEnumerator propEnum)
        {
            Pen pen = new Pen(Grid.GridColor);
            graphics.DrawLine(pen, rect.Right - 1, rect.Top, rect.Right - 1, rect.Bottom);
            pen.Dispose();
        }

        public override void DrawCategoryLabelBackground(Graphics graphics, Rectangle rect, PropertyEnumerator enumSelf)
        {
            Rectangle fillRect = Rectangle.Intersect(rect, Rectangle.Round(graphics.ClipBounds));
            if (fillRect.IsEmpty)
                return;

            Brush brush;

            if (enumSelf.Property.Selected) 	// if selected
            {
                Color bkgColor;

                if (Grid.GridContainsFocus)
                    bkgColor = Grid.SelectedBackColor;
                else
                {
                    // If the control has not the focus, the background is light
                    bkgColor = Grid.SelectedNotFocusedBackColor;
                }

                brush = new SolidBrush(bkgColor);
            }
            else
                brush = new SolidBrush(Grid.GridBackColor);

            graphics.FillRectangle(brush, rect);
        }

        public override void DrawSubCategoryLabelBackground(Graphics graphics, int labelRectX, Rectangle rect, PropertyEnumerator enumSelf)
        {
            DrawCategoryLabelBackground(graphics, rect, enumSelf);
        }
    }
}
