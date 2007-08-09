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
    /// Look class used to display a Pen value as a 2D line with the correct color and width.
    /// </summary>
    public class PropertyPenLook : PropertyLook
    {
        public override void OnDraw(Graphics graphics, Rectangle valueRect, Color textColor, PropertyEnumerator propEnum, string drawAnotherString)
        {
            if ((drawAnotherString != null) && (drawAnotherString.Length == 0))
                return;

            Pen pen = (Pen)propEnum.Property.Value.GetValue();
            if (pen.Width > valueRect.Height - 2)
            {
                pen = (Pen)pen.Clone();
                pen.Width = valueRect.Height - 2;
            }

            int margin = Value.Grid.GlobalTextMargin;
            graphics.DrawLine(pen, valueRect.Left + margin, valueRect.Top + valueRect.Height / 2,
                valueRect.Right - margin, valueRect.Top + valueRect.Height / 2);
        }
    }
}
