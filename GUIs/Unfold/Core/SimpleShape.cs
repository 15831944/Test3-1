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
namespace Orbifold.Unfold.Core
{
    /// <summary>
    /// The most basic shape type.
    /// It's templated, see the main application to see how you can style it both programmatically
    /// and in XAML.
    /// </summary>
    public abstract class SimpleShape : Connectable
    {
        #region Text property
        /// <summary>
        /// The Text dependency property.
        /// </summary>
        public static DependencyProperty TextProperty = DependencyProperty.Register("Text", typeof(string), typeof(SimpleShape));

       

        public string Text
        {
           get { return (string)this.GetValue(TextProperty); }
           set { this.SetValue(TextProperty, value); }
        }
        #endregion


        #region Constructor
        
        public SimpleShape()
        {
            this.ResizableHeight = true;
            this.ResizableWidth = true;
            this.MinHeight = 50;
            this.MinWidth = 120;
        }
        #endregion


    }
}
