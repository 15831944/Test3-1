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

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Look class used to display a date or time in the value column of a property.
    /// </summary>
    public class PropertyDateTimeLook : PropertyLook
    {
        private DateTimePickerFormat _format = DateTimePickerFormat.Time;

        public DateTimePickerFormat Format
        {
            set { _format = value; }
            get { return _format; }
        }

        private string _customFormat = "";

        public string CustomFormat
        {
            get { return _customFormat; }

            set
            {
                _format = DateTimePickerFormat.Custom;
                _customFormat = value;
            }
        }

        public PropertyDateTimeLook() { }

        public PropertyDateTimeLook(DateTimePickerFormat format)
        {
            _format = format;
        }

        public PropertyDateTimeLook(string customFormat)
        {
            _format = DateTimePickerFormat.Custom;
            _customFormat = customFormat;
        }

        public override void OnDraw(Graphics graphics, Rectangle valueRect, Color textColor, PropertyEnumerator propEnum, string drawAnotherString)
        {
            if ((drawAnotherString != null) && (drawAnotherString.Length == 0))
                return;

            valueRect.Height++;

            Rectangle strRect = GetDisplayStringRect(graphics, valueRect, Point.Empty);
            strRect.Width = valueRect.Right - strRect.Left;

            Win32Calls.DrawText(graphics, (drawAnotherString == null ? GetDisplayStringInternal() : drawAnotherString),
                ref strRect, Value.Font, textColor,
                Win32Calls.DT_VCENTER | Win32Calls.DT_SINGLELINE | Win32Calls.DT_NOPREFIX |
                ((Value.Grid.EllipsisMode & PropertyGrid.EllipsisModes.EllipsisOnValues) != 0 ? Win32Calls.DT_END_ELLIPSIS : 0));
        }

        public override string DisplayString
        {
            get
            {
                return GetDisplayStringInternal();
            }
        }

        protected string GetDisplayStringInternal()
        {
            PropertyDateTimeFeel feel = Value.Feel as PropertyDateTimeFeel;
            if (feel != null)
            {
                DateTime dt = (DateTime)Value.UnderlyingValue;
                // If we use this line instead we loose the "seconds" because the TypeConverter uses the shortTimePattern:
                // DateTime dt = (DateTime)Value.TypeConverter.ConvertFromString(Value.DisplayStringWithoutLook);
                if (_format == DateTimePickerFormat.Custom)
                    return dt.ToString(_customFormat);
                if (_format == DateTimePickerFormat.Time)
                    return dt.ToLongTimeString();
                else if (_format == DateTimePickerFormat.Short)
                    return dt.ToShortDateString();
                else
                    return dt.ToLongDateString();
            }
            else
                return "";
        }

        public override Rectangle GetDisplayStringRect(Graphics graphics, Rectangle valueRect, Point point)
        {
            Size size = Win32Calls.GetTextExtent(graphics, GetDisplayStringInternal(), Value.Font);

            Rectangle rect = valueRect;
            rect.X += Value.Grid.GlobalTextMargin;
            rect.Width = Size.Round(size).Width;

            return rect;
        }
    }
}
