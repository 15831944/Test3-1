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
    /// <summary>
    /// Example of a <see cref="ShiftShape"/> which encapsulates a thought or meme.
    /// </summary>
    public class MemeShape : ShiftShape
    {
        #region Properties
        public static DependencyProperty TextProperty = DependencyProperty.Register("Text", typeof(string), typeof(ClassShape));
        public string Text
        {
            get { return (string)GetValue(TextProperty); }
            set { SetValue(TextProperty, value); }
        }
        #endregion

        #region Constructors
        /// <summary>
        /// Static constructor
        /// </summary>
        static MemeShape()
        {
            //This OverrideMetadata call tells the system that this element wants to provide a style that is different than its base class.
            //This style is defined in themes\generic.xaml
            DefaultStyleKeyProperty.OverrideMetadata(typeof(MemeShape), new FrameworkPropertyMetadata(typeof(MemeShape)));

        }
        /// <summary>
        /// Default constructor
        /// </summary>
        public MemeShape()
        {
            this.MinWidth = 100;
            IsExpanded = false;
            ShapeColor = Colors.Green;
            InitializeCommands();

        }
        #endregion
    }
}
