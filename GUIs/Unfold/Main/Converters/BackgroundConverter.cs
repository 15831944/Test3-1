using System;
using System.Windows;
using System.Windows.Data;

namespace Orbifold.Unfold.Main
{
    /// <summary>
    /// Converts button names to icon resources.
    /// </summary>
    public class BackgroundConverter : IValueConverter
    {
        public Object Convert(Object value, Type targetType, Object parameter, System.Globalization.CultureInfo culture)
        {
            String name = value as String;
            switch (name)
            {
                case "CreateConnection":
                    return Application.Current.FindResource("DefaultConnection");
                case "CreateClassShape":
                    return Application.Current.FindResource("ClassShapeBrush");
                case "CreateRectangle":
                    return Application.Current.FindResource("RectangleBrush");
                case "CreateEllipse":
                    return Application.Current.FindResource("EllipseBrush");
                case "ToggleItalic":
                    return Application.Current.FindResource("ItalicIcon");
                case "ToggleUnderline":
                    return Application.Current.FindResource("UnderlineIcon");
                case "AlignLeft":
                    return Application.Current.FindResource("AlignLeftIcon");
                case "AlignCenter":
                    return Application.Current.FindResource("AlignCenterIcon");
                case "AlignRight":
                    return Application.Current.FindResource("AlignRightIcon");
                case "AlignJustify":
                    return Application.Current.FindResource("AlignJustifyIcon");
                case "IncreaseIndentation":
                    return Application.Current.FindResource("IncreaseIndentationIcon");
                case "DecreaseIndentation":
                    return Application.Current.FindResource("DecreaseIndentationIcon");
                case "ToggleBullets":
                    return Application.Current.FindResource("BulletsIcon");
                case "ToggleNumbering":
                    return Application.Current.FindResource("NumberingIcon");
                case "Undo":
                    return Application.Current.FindResource("UndoIcon");
                case "Redo":
                    return Application.Current.FindResource("RedoIcon");
                default:
                    return null;
            }
        }

        public Object ConvertBack(Object value, Type targetType, Object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
