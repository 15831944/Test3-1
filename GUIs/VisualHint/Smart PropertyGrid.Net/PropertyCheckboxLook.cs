// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

#region Using directives

using System;
using System.Drawing;
using System.ComponentModel;
using System.Collections;
using System.Reflection;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Look class used to display one or several checkboxes (with text) inside the value column of a property.
    /// </summary>
    public class PropertyCheckboxLook : PropertyLook
    {
        public PropertyCheckboxLook()
        {
            mNoFeelForReadOnly = true;
        }

        public override void ControlHeightMultiplier()
        {
            if (Value.UnderlyingType == typeof(Boolean))
                Value.OwnerEnumerator.Property.HeightMultiplier = 1;
            else
                Value.OwnerEnumerator.Property.HeightMultiplier = Value.GetDisplayedValues().Length;

            if (Value.OwnerEnumerator.Property.HeightMultiplier == 0)
                Value.OwnerEnumerator.Property.HeightMultiplier = 1;
        }

        public override void OnDraw(Graphics graphics, Rectangle valueRect, Color textColor, PropertyEnumerator propEnum, string drawAnotherString)
        {
            if ((drawAnotherString != null) && (drawAnotherString.Length == 0))
                return;

            string[] displayedStrings = Value.GetDisplayedValues();
            if (displayedStrings.Length == 0)
                return;

            int margin = Value.Grid.GlobalTextMargin;

            valueRect.Height++;

            valueRect.X += margin - 1;
            valueRect.Width -= margin - 1;

            Rectangle oneLineRect = valueRect;

            bool hasMultipleValues = Value.HasMultipleValues;

            if (Value.UnderlyingType == typeof(Boolean))
            {
                bool value = (bool)Value.GetValue();

                Rectangle checkboxRect = oneLineRect;

                checkboxRect.Height -= 4;
                checkboxRect.Y += 2;
                checkboxRect.Width = checkboxRect.Height;

                // Draw checkbox
                if (hasMultipleValues)
                    propEnum.Property.ParentGrid.DrawManager.DrawCheckBoxIndeterminate(graphics,
                        propEnum.Property.ParentGrid.Grid, checkboxRect, propEnum.Property.Enabled);
                else
                {
                    propEnum.Property.ParentGrid.DrawManager.DrawCheckBox(graphics, propEnum.Property.ParentGrid.Grid,
                        checkboxRect, value, propEnum.Property.Enabled);

                    Rectangle textRect = oneLineRect;
                    textRect.X = checkboxRect.Right + margin;
                    textRect.Width = oneLineRect.Right - textRect.Left;

                    // Draw corresponding text
                    Win32Calls.DrawText(graphics, (value ? displayedStrings[0] : displayedStrings[1]), ref textRect,
                        Value.Font, textColor, Win32Calls.DT_VCENTER | Win32Calls.DT_SINGLELINE | Win32Calls.DT_NOPREFIX |
                        ((Value.Grid.EllipsisMode & PropertyGrid.EllipsisModes.EllipsisOnValues) != 0 ? Win32Calls.DT_END_ELLIPSIS : 0));
                }
            }
            else if (propEnum.Property.Value.UnderlyingType.IsEnum)
            {
                oneLineRect.Height = valueRect.Height / displayedStrings.Length;

                int currentValue = 0;

                if (hasMultipleValues == false)
                {
                    currentValue = propEnum.Property.Value.TypeConverter.ConvertFromString(
                        propEnum.Property.Value.GetTypeDescriptorContext(Value.OwnerEnumerator),
                        propEnum.Property.Value.CultureInfo,
                        (drawAnotherString != null ? drawAnotherString : propEnum.Property.Value.GetStringValue())).GetHashCode();
                }
                FieldInfo[] fis = Value.UnderlyingType.GetFields(BindingFlags.Public | BindingFlags.Static);
#if _DOTNET2
                // There is a bug in .Net 2.0 that prevents to use ArrayList.Sort (performance reasons)
                Array.Sort<object>(fis, 0, fis.Length, new FieldInfoEnumComparer(propEnum.Property.Value.UnderlyingType));
#else
                Array.Sort(fis, 0, fis.Length, new FieldInfoEnumComparer(propEnum.Property.Value.UnderlyingType));
#endif
                int i = 0;
                int displayedStringsIndex = 0;
                foreach (FieldInfo fi in fis)
                {
                    // If the field is not browsable, we continue to the next one
                    object[] attrs = fi.GetCustomAttributes(typeof(BrowsableAttribute), false);
                    if ((attrs.Length > 0) && (((BrowsableAttribute)attrs[0]).Browsable == false))
                    {
                        i++;
                        continue;
                    }

                    Rectangle checkboxRect = oneLineRect;

                    checkboxRect.Height -= 4;
                    checkboxRect.Y += 2;
                    checkboxRect.Width = checkboxRect.Height;

                    // Draw checkbox
                    if (hasMultipleValues)
                        propEnum.Property.ParentGrid.DrawManager.DrawCheckBoxIndeterminate(graphics,
                            propEnum.Property.ParentGrid.Grid, checkboxRect, propEnum.Property.Enabled);
                    else
                    {
                        int flagValue = fi.GetValue(propEnum.Property.Value.UnderlyingValue).GetHashCode();
                        bool check = (((currentValue & flagValue) == flagValue) && (flagValue != 0)) || ((currentValue == 0) && (flagValue == 0));
                        propEnum.Property.ParentGrid.DrawManager.DrawCheckBox(graphics, propEnum.Property.ParentGrid.Grid,
                            checkboxRect, check, propEnum.Property.Enabled);
                    }

                    Rectangle textRect = oneLineRect;
                    textRect.X = checkboxRect.Right + margin;
                    textRect.Width = oneLineRect.Right - textRect.Left;

                    // Draw corresponding text
                    Win32Calls.DrawText(graphics, displayedStrings[displayedStringsIndex], ref textRect, Value.Font, textColor,
                        Win32Calls.DT_VCENTER | Win32Calls.DT_SINGLELINE | Win32Calls.DT_NOPREFIX |
                        ((Value.Grid.EllipsisMode & PropertyGrid.EllipsisModes.EllipsisOnValues) != 0 ? Win32Calls.DT_END_ELLIPSIS : 0));

                    oneLineRect.Offset(0, oneLineRect.Height);

                    i++;
                    displayedStringsIndex++;
                }
            }
            else if (propEnum.Property.Value.UnderlyingValue is ICollection)
            {
                oneLineRect.Height = valueRect.Height / displayedStrings.Length;

                for (int i = 0; i < displayedStrings.Length; i++)
                {
                    Rectangle checkboxRect = oneLineRect;

                    checkboxRect.Height -= 4;
                    checkboxRect.Y += 2;
                    checkboxRect.Width = checkboxRect.Height;

                    // Draw checkbox
                    if (hasMultipleValues)
                        propEnum.Property.ParentGrid.DrawManager.DrawCheckBoxIndeterminate(graphics,
                            propEnum.Property.ParentGrid.Grid, checkboxRect, propEnum.Property.Enabled);
                    else
                    {
                        bool strFound = false;
                        ICollection collection = propEnum.Property.Value.UnderlyingValue as ICollection;
                        foreach (object o in collection)
                        {
                            if (o.ToString() == displayedStrings[i])
                                strFound = true;
                        }
                        propEnum.Property.ParentGrid.DrawManager.DrawCheckBox(graphics, propEnum.Property.ParentGrid.Grid,
                            checkboxRect, strFound, propEnum.Property.Enabled);
                    }

                    Rectangle textRect = oneLineRect;
                    textRect.X = checkboxRect.Right + margin;
                    textRect.Width = oneLineRect.Right - textRect.Left;

                    // Draw corresponding text
                    Win32Calls.DrawText(graphics, displayedStrings[i], ref textRect, Value.Font, textColor,
                        Win32Calls.DT_VCENTER | Win32Calls.DT_SINGLELINE | Win32Calls.DT_NOPREFIX |
                        ((Value.Grid.EllipsisMode & PropertyGrid.EllipsisModes.EllipsisOnValues) != 0 ? Win32Calls.DT_END_ELLIPSIS : 0));

                    oneLineRect.Offset(0, oneLineRect.Height);
                }
            }
        }

        public override Rectangle GetDisplayStringRect(Graphics graphics, Rectangle valueRect, Point point)
        {
            int margin = Value.Grid.GlobalTextMargin;
            valueRect.X += margin - 1;
            valueRect.Width -= margin - 1;

            Rectangle calcRect = Rectangle.Empty;
            string[] displayedStrings = Value.GetDisplayedValues();
            if (displayedStrings.Length == 0)
                return calcRect;

            if (Value.UnderlyingType == typeof(Boolean))
            {
                if (Value.HasMultipleValues)
                    return Rectangle.Empty;

                bool value = (bool)Value.GetValue();
                string str = (value ? displayedStrings[0] : displayedStrings[1]);

                // Radio button rectangle
                Rectangle checkboxRect = valueRect;
                checkboxRect.Height -= 4;
                checkboxRect.Y += 2;
                checkboxRect.Width = checkboxRect.Height;

                // Text rectangle
                Rectangle textRect = valueRect;
                textRect.X = checkboxRect.Right + margin;
                textRect.Width = valueRect.Right - textRect.Left;

                // Draw corresponding text
                calcRect = textRect;
                Win32Calls.DrawText(graphics, str, ref calcRect, Value.Font,
                    Color.Black, Win32Calls.DT_VCENTER | Win32Calls.DT_SINGLELINE | Win32Calls.DT_CALCRECT |
                    Win32Calls.DT_NOPREFIX);

                calcRect.Y = valueRect.Top;
                calcRect.Height = valueRect.Bottom - calcRect.Y;

                return calcRect;
            }
            else
            {
                Rectangle oneLineRect = valueRect;
                oneLineRect.Height = valueRect.Height / displayedStrings.Length;
                for (int i = 0; i < displayedStrings.Length; i++)
                {
                    string str = displayedStrings[i];

                    // Radio button rectangle
                    Rectangle checkboxRect = oneLineRect;
                    checkboxRect.Height -= 4;
                    checkboxRect.Y += 2;
                    checkboxRect.Width = checkboxRect.Height;

                    // Text rectangle
                    Rectangle textRect = oneLineRect;
                    textRect.X = checkboxRect.Right + margin;
                    textRect.Width = oneLineRect.Right - textRect.Left;

                    // Draw corresponding text
                    calcRect = textRect;
                    Win32Calls.DrawText(graphics, str, ref calcRect, Value.Font,
                        Color.Black, Win32Calls.DT_VCENTER | Win32Calls.DT_SINGLELINE | Win32Calls.DT_CALCRECT |
                        Win32Calls.DT_NOPREFIX);

                    if (calcRect.Contains(point))
                        break;

                    oneLineRect.Offset(0, oneLineRect.Height);
                }

                calcRect.Y = oneLineRect.Top;
                calcRect.Height = oneLineRect.Bottom - calcRect.Y;

                return calcRect;
            }
        }

        public override string GetTooltipString(int line)
        {
            string[] displayedStrings = Value.GetDisplayedValues();
            if (line >= displayedStrings.Length)
                return "";

            if (Value.UnderlyingType == typeof(Boolean))
            {
                bool value = (bool)Value.GetValue();
                return (value ? displayedStrings[0] : displayedStrings[1]);
            }
            else
                return displayedStrings[line];
        }

        public override Size GetTooltipStringSize(Graphics graphics, Rectangle valueRect, Point point, int line)
        {
            return Win32Calls.GetTextExtent(graphics, GetTooltipString(line), Value.Font);
        }
    }
}
