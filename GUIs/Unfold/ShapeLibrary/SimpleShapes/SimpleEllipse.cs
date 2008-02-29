using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using Orbifold.Unfold.Core;
namespace Orbifold.Unfold.ShapeLibrary
{
    /// <summary>
    /// The most basic, elliptic shape type.
    /// It's templated, see the main application to see how you can style it both programmatically
    /// and in XAML.
    /// </summary>
    public class SimpleEllipse : SimpleShape
    {

        static SimpleEllipse()
        {
            //This OverrideMetadata call tells the system that this element wants to provide a style that is different than its base class.
            //This style is defined in themes\generic.xaml
            DefaultStyleKeyProperty.OverrideMetadata(typeof(SimpleEllipse), new FrameworkPropertyMetadata(typeof(SimpleEllipse)));
            
        }

      
    }
}
