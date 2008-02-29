using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Diagnostics;
using System.Globalization;
using System.Windows.Threading;
using System.Windows.Media.Animation;
using System.ComponentModel;
using System.Windows.Media.Effects;
using Orbifold.Unfold.Core;

namespace Orbifold.Unfold.ShapeLibrary
{
    [ValueConversion(typeof(string), typeof(ImageSource))]
    public class ShapeImageConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
           
            ImageSourceConverter imageSourceConverter = new ImageSourceConverter();
            return imageSourceConverter.ConvertFromString("collapse.gif");
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            
            return null;
        }
    }
}
