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
using System.Collections.ObjectModel;
namespace Orbifold.Unfold.ShapeLibrary
{
    /// <summary>
    /// Customizable backend code for the so-called class-shape that
    /// first appeared in VS2005 in the class designer.
    /// </summary>
    public class ClassShape : ShiftShape
    {
        #region Dependency properties
        /// <summary>
        /// The main title of the shape
        /// </summary>
        public static DependencyProperty TitleProperty = DependencyProperty.Register("Title", typeof(string), typeof(ClassShape));
        /// <summary>
        /// The subtitle of the shape
        /// </summary>
        public static DependencyProperty SubTitleProperty = DependencyProperty.Register("SubTitle", typeof(string), typeof(ClassShape));
       
        #endregion
        #region Properties





        /// <summary>
        /// Gets or sets the main title of the shape.
        /// </summary>
        /// <value>The title.</value>
        public string Title
        {
            get { return (string)GetValue(TitleProperty); }
            set { SetValue(TitleProperty, value); }
        }

        /// <summary>
        /// Gets or sets the subtitle of the shape.
        /// </summary>
        /// <value>The sub title.</value>
        public string SubTitle
        {
            get { return (string)GetValue(SubTitleProperty); }
            set { SetValue(SubTitleProperty, value); }
        }
       
        #endregion

        public static DependencyProperty CurrentResizeProperty = DependencyProperty.Register("CurrentResize", typeof(double), typeof(ClassShape));
        public double CurrentResize
        {
            get { return (double)GetValue(CurrentResizeProperty); }
            set
            {
                SetValue(CurrentResizeProperty, value);
                
            }
        }


        
        public static readonly RoutedEvent ResizeBodyEvent = EventManager.RegisterRoutedEvent("ResizeBody", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(ClassShape));
        void RaiseResizeBodyEvent()
        {
            RoutedEventArgs newEventArgs = new RoutedEventArgs(ClassShape.ResizeBodyEvent);
            RaiseEvent(newEventArgs);
        }
        #region Fields
        /// <summary>
        /// the Text field
        /// </summary>
        private string mText;
        private Border mx;

        #endregion

        private ObservableCollection<ClassCategory> mTreeData;

        public ObservableCollection<ClassCategory> TreeData
        {
            get { return mTreeData; }
            set { mTreeData = value; }
        }

	
        #region Properties
        /// <summary>
        /// Gets or sets the body text, i.e. the text that shifts up and down.
        /// </summary>
        public string Text
        {
            get { return mText; }
            set { mText = value; }
        }
        #endregion

        #region Constructor


        static ClassShape()
        {
            //This OverrideMetadata call tells the system that this element wants to provide a style that is different than its base class.
            //This style is defined in themes\generic.xaml
            DefaultStyleKeyProperty.OverrideMetadata(typeof(ClassShape), new FrameworkPropertyMetadata(typeof(ClassShape)));
            
        }
       
        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();
            ControlTemplate template = this.Template;
            if (template != null)
            {
                TreeView tree = template.FindName("BodyText", this) as TreeView;
                if (tree != null)
                {
                    tree.SizeChanged += new SizeChangedEventHandler(OnTreeSizeChanged);
                }
                mx = template.FindName("Boordje", this) as Border;
             
            }

        }
        public ClassShape()
        {
            this.MinWidth = 100;
            
            IsExpanded = false;
            ShapeColor = Colors.Green;
            ResizableHeight = false;
            ResizableWidth = false;
            InitializeCommands();
            CurrentResize = 300;
            ClipToBounds = true;           
           
        }
      
        #endregion

        /*
         * Procedural version of the shift animation, just as an illustration here.
         * 
        protected override void OnMouseDown(MouseButtonEventArgs e)
        {
            base.OnMouseDown(e);
            if (sb != null)
            {
                sb.Stop(mx);
                this.Body.Visibility = Visibility.Hidden;
                this.mx.Visibility = Visibility.Hidden;
                //rb.Begin(mx);
            }
        }

        protected override void OnMouseLeave(MouseEventArgs e)
        {
            if (!AnimationEnabled) return;
            base.OnMouseLeave(e);
            if (sb != null)
            {
                sb.Stop(mx);
                this.Body.Visibility = Visibility.Hidden;
                rb.Begin(mx);
            }

            //object obj = sb.GetCurrentValueAsFrozen().GetValue(FrameworkElement.OpacityProperty);
            //this.mx.Visibility = Visibility.Hidden;
            //this.Body.Visibility = Visibility.Hidden;
        }
        protected override void OnMouseEnter(MouseEventArgs e)
        {
            if (!AnimationEnabled) return;
            base.OnMouseEnter(e);
            this.mx.Visibility = Visibility.Visible;
            sb.AutoReverse = false;
            this.Body.Visibility = Visibility.Visible;
            //animate it:
            sb.Begin(mx);
        }
        protected override void OnRenderSizeChanged(SizeChangedInfo sizeInfo)
        {
            base.OnRenderSizeChanged(sizeInfo);
            BaseRectangle.Width = sizeInfo.NewSize.Width;
            mx.Width = sizeInfo.NewSize.Width;
        }
        */

        #region Methods
        protected override void OnShift()
        {
            base.OnShift();
            RefreshConnections();
        }
        private void OnTreeSizeChanged(object sender, SizeChangedEventArgs e)
        {
            RefreshConnections();
        }

      

        #endregion

        


       
    }
}
