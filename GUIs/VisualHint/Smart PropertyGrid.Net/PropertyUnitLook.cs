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
    /// Look class typically used to display a numerical value and its unit as two editable fields.
    /// </summary>
    public class PropertyUnitLook : PropertyLook
    {
        public const string UnitValue = "unit";

        public override void OnDraw(Graphics graphics, Rectangle valueRect, Color textColor, PropertyEnumerator propEnum, string drawAnotherString)
        {
            if ((drawAnotherString != null) && (drawAnotherString.Length == 0))
                return;

            valueRect.Height++;

            Rectangle strRect = GetDisplayStringRect(graphics, valueRect, Point.Empty /* not used */);
            strRect.Width = valueRect.Right - strRect.Left;

            Win32Calls.DrawText(graphics, (drawAnotherString == null ? GetDisplayStringInternal() : drawAnotherString),
                ref strRect, Value.Font, textColor,
                Win32Calls.DT_VCENTER | Win32Calls.DT_SINGLELINE | Win32Calls.DT_NOPREFIX |
                ((Value.Grid.EllipsisMode & PropertyGrid.EllipsisModes.EllipsisOnValues) != 0 ? Win32Calls.DT_END_ELLIPSIS : 0));
        }

        protected string GetDisplayStringInternal()
        {
            PropertyValue unitValue = Value.OwnerEnumerator.Property.GetValue(UnitValue);
            if (Value.HasMultipleValues)
            {
                if (unitValue.HasMultipleValues)
                    return "";

                return unitValue.DisplayString;
            }
            else
            {
                if (unitValue.HasMultipleValues)
                    return Value.GetDisplayString(Value.GetStringValue());

                return Value.GetDisplayString(Value.GetStringValue()) + " " + unitValue.DisplayString;
            }
        }

        public override Rectangle GetDisplayStringRect(Graphics graphics, Rectangle valueRect, Point point)
        {
            Size size = Win32Calls.GetTextExtent(graphics, GetDisplayStringInternal(), Value.Font);

            Rectangle rect = valueRect;
            rect.X += Value.Grid.GlobalTextMargin;
            rect.Width = Size.Round(size).Width;

            return rect;
        }

        public override string GetTooltipString(int line)
        {
            return GetDisplayStringInternal();
        }

        public override Size GetTooltipStringSize(Graphics graphics, Rectangle valueRect, Point point, int line)
        {
            return Win32Calls.GetTextExtent(graphics, GetTooltipString(0), Value.Font);
        }
    }
}
