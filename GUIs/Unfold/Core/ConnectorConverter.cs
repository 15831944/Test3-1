using System;
using System.Windows.Media.Effects;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Shapes;
using System.Windows.Input;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Globalization;

namespace Orbifold.Unfold.Core
{
    
    public class ConnectorConverter : IMultiValueConverter
    {
        public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
        {
            if (targetType.Equals(typeof(Point)))
            {
                if (values.Length != 4)
                {
                    throw new ArgumentException("You need to supply the following values: X, Y, Width and Height.");
                }
                if (string.Equals(parameter.ToString(), "top", StringComparison.CurrentCultureIgnoreCase))
                {
                    return new Point((double)values[0] + (double)values[2] / 2, (double)values[1]);
                }
                else if (string.Equals(parameter.ToString(), "left", StringComparison.CurrentCultureIgnoreCase))
                {
                    return new Point((double)values[0], (double)values[1] + (double)values[3] / 2);
                }
                else if (string.Equals(parameter.ToString(), "right", StringComparison.CurrentCultureIgnoreCase))
                {
                    return new Point((double)values[0] + (double)values[2], (double)values[1] + (double)values[3] / 2);
                }
                else if (string.Equals(parameter.ToString(), "bottom", StringComparison.CurrentCultureIgnoreCase))
                {
                    return new Point((double)values[0] + (double)values[2] / 2, (double)values[1] + (double)values[3]);
                }
                else
                    throw new ArgumentException("The converter parameter can be either 'Width' or 'Height' and indicates in which direction the attachment location is shifted.");
            }
            else
            {
                throw new ArgumentException("This converter only converts a double[4] to a Point value.");
            }
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
        {
            //string[] splitValues = ((string)value).Split(' ');
            //return splitValues;
            Point p = (Point)value;

            return new object[] { p.X, p.Y };
        }
    }

    /// <summary>
    /// Converts a connector location (as a string value) to a Point location.
    /// </summary>
    public class ConnectorConverter2 : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (targetType.Equals(typeof(Point)))
            {
                Control elem = value as Control; 
                
                if (string.Equals(parameter.ToString(), "top", StringComparison.CurrentCultureIgnoreCase))
                {
                    return new Point((double)VisualTreeHelper.GetOffset(elem).X + (double)elem.DesiredSize.Width / 2, (double)VisualTreeHelper.GetOffset(elem).Y);
                }
                else if (string.Equals(parameter.ToString(), "left", StringComparison.CurrentCultureIgnoreCase))
                {
                    return new Point((double)VisualTreeHelper.GetOffset(elem).X, VisualTreeHelper.GetOffset(elem).X + (double)elem.DesiredSize.Height / 2);
                }
                else if (string.Equals(parameter.ToString(), "right", StringComparison.CurrentCultureIgnoreCase))
                {
                    return new Point((double)VisualTreeHelper.GetOffset(elem).X + elem.DesiredSize.Width, (double)VisualTreeHelper.GetOffset(elem).Y + (double)elem.DesiredSize.Height / 2);
                }
                else if (string.Equals(parameter.ToString(), "bottom", StringComparison.CurrentCultureIgnoreCase))
                {
                    return new Point((double)VisualTreeHelper.GetOffset(elem).X + (double)elem.DesiredSize.Width / 2, (double)VisualTreeHelper.GetOffset(elem).Y + elem.DesiredSize.Height);
                }
                else
                    throw new ArgumentException("The converter parameter can be either 'Width' or 'Height' and indicates in which direction the attachment location is shifted.");
            }
            else
            {
                throw new ArgumentException("This converter only converts a double[4] to a Point value.");
            }
        }

        public object ConvertBack(object value, Type targetTypes, object parameter, CultureInfo culture)
        {
            //string[] splitValues = ((string)value).Split(' ');
            //return splitValues;
            Point p = (Point)value;

            return new object[] { p.X, p.Y };
        }
    }
}
