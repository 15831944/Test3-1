using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Shapes;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;

namespace Orbifold.Unfold.Core
{

    /// <summary>
    /// This is where WPF shines. While the resize of entities is a rather complex affair in GDI,
    /// it's way easier here by using adorners.
    /// See also following item; http://www.orbifold.net/default/?p=187
    /// </summary>
    public class ResizingAdorner : Adorner
    {
        #region Fields
        private bool mHeightEnabled = true;
        Rectangle mainrec;
        private bool mWidthEnabled = true;
        // Resizing adorner uses Thumbs for visual elements.  
        // The Thumbs have built-in mouse input handling.
        DefaultThumb topLeft, topRight, bottomLeft, bottomRight;

        // To store and manage the adorner's visual children.
        VisualCollection visualChildren;
        #endregion

        #region Properties
        /// <summary>
        /// Gets or sets a value indicating whether the adorner changes the width of the shape.
        /// </summary>
        /// <value><c>true</c> if the width can change; otherwise, <c>false</c>.</value>
        public bool WidthEnabled
        {
            get { return mWidthEnabled; }
            set { mWidthEnabled = value; CheckVisibility(); }
        }



        /// <summary>
        /// Gets or sets a value indicating whether the adorner changes the height of the shape.
        /// </summary>
        /// <value><c>true</c> if the height can change; otherwise, <c>false</c>.</value>
        public bool HeightEnabled
        {
            get { return mHeightEnabled; }
            set
            {
                mHeightEnabled = value;

                CheckVisibility();
            }
        }

        #endregion
        
        #region Constructor
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="adornedElement"></param>
        public ResizingAdorner(UIElement adornedElement)
            : base(adornedElement)
        {
            visualChildren = new VisualCollection(this);
            mainrec = new Rectangle();
            SolidColorBrush brush = new SolidColorBrush(Colors.DimGray);

            mainrec.Stroke = Brushes.Firebrick;
            DoubleCollection col = new DoubleCollection();
            col.Add(5.0);
            col.Add(2.30);
            mainrec.StrokeDashArray = col;
            visualChildren.Add(mainrec);

            // Call a helper method to initialize the Thumbs
            // with a customized cursors.
            BuildAdornerCorner(ref topLeft, Cursors.SizeNWSE);
            BuildAdornerCorner(ref topRight, Cursors.SizeNESW);
            BuildAdornerCorner(ref bottomLeft, Cursors.SizeNESW);
            BuildAdornerCorner(ref bottomRight, Cursors.SizeNWSE);

            // Add handlers for resizing.
            bottomLeft.DragDelta += new DragDeltaEventHandler(HandleBottomLeft);
            bottomRight.DragDelta += new DragDeltaEventHandler(HandleBottomRight);
            topLeft.DragDelta += new DragDeltaEventHandler(HandleTopLeft);
            topRight.DragDelta += new DragDeltaEventHandler(HandleTopRight);

            this.MinWidth = 50;

        }
        #endregion
        
        #region Methods

        /// <summary>
        /// Checks the visibility of the thumbs.
        /// </summary>
        private void CheckVisibility()
        {
            if (!mWidthEnabled && !mHeightEnabled)
            {
                topLeft.Visibility = Visibility.Hidden;
                topRight.Visibility = Visibility.Hidden;
                bottomLeft.Visibility = Visibility.Hidden;
                bottomRight.Visibility = Visibility.Hidden;
            }
            else
            {
                topLeft.Visibility = Visibility.Visible;
                topRight.Visibility = Visibility.Visible;
                bottomLeft.Visibility = Visibility.Visible;
                bottomRight.Visibility = Visibility.Visible;
            }
        }

        
        /// <summary>
        ///  Handler for resizing from the bottom-right.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="args">The <see cref="System.Windows.Controls.Primitives.DragDeltaEventArgs"/> instance containing the event data.</param>
        void HandleBottomRight(object sender, DragDeltaEventArgs args)
        {
            FrameworkElement adornedElement = this.AdornedElement as FrameworkElement;
            Thumb hitThumb = sender as Thumb;

            if (adornedElement == null || hitThumb == null) return;
            FrameworkElement parentElement = adornedElement.Parent as FrameworkElement;

            // Ensure that the Width and Height are properly initialized after the resize.
            EnforceSize(adornedElement);

            // Change the size by the amount the user drags the mouse, as long as it's larger 
            // than the width or height of an adorner, respectively.
            if (mWidthEnabled)
                adornedElement.Width = Math.Max(adornedElement.Width + args.HorizontalChange, hitThumb.DesiredSize.Width);
            if (mHeightEnabled)
                adornedElement.Height = Math.Max(args.VerticalChange + adornedElement.Height, hitThumb.DesiredSize.Height);
        }


        /// <summary>
        ///  Handler for resizing from the bottom-left.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="args">The <see cref="System.Windows.Controls.Primitives.DragDeltaEventArgs"/> instance containing the event data.</param>
        void HandleBottomLeft(object sender, DragDeltaEventArgs args)
        {
            FrameworkElement adornedElement = AdornedElement as FrameworkElement;
            Thumb hitThumb = sender as Thumb;

            if (adornedElement == null || hitThumb == null) return;

            // Ensure that the Width and Height are properly initialized after the resize.
            EnforceSize(adornedElement);

            adornedElement.Height = Math.Max(args.VerticalChange + adornedElement.Height, hitThumb.DesiredSize.Height);

            // Change the size by the amount the user drags the mouse, as long as it's larger 
            // than the width or height of an adorner, respectively.
            double xchange = adornedElement.Width - args.HorizontalChange;
            if (xchange >= adornedElement.MinWidth)
            {
                adornedElement.Width = xchange;
                Canvas.SetLeft(adornedElement, Canvas.GetLeft(adornedElement) + args.HorizontalChange);
            }


        }


        /// <summary>
        /// Handler for resizing from the top-right.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="args">The <see cref="System.Windows.Controls.Primitives.DragDeltaEventArgs"/> instance containing the event data.</param>
        void HandleTopRight(object sender, DragDeltaEventArgs args)
        {
            FrameworkElement adornedElement = this.AdornedElement as FrameworkElement;
            Thumb hitThumb = sender as Thumb;

            if (adornedElement == null || hitThumb == null) return;
            FrameworkElement parentElement = adornedElement.Parent as FrameworkElement;

            // Ensure that the Width and Height are properly initialized after the resize.
            EnforceSize(adornedElement);

            // Change the size by the amount the user drags the mouse, as long as it's larger 
            // than the width or height of an adorner, respectively.
            adornedElement.Width = Math.Max(adornedElement.Width + args.HorizontalChange, hitThumb.DesiredSize.Width);
            //adornedElement.Height = Math.Max(adornedElement.Height - args.VerticalChange, hitThumb.DesiredSize.Height);
            double ychange = adornedElement.Height - args.VerticalChange;
            if (ychange >= adornedElement.MinHeight)
            {
                adornedElement.Height = ychange;
                Canvas.SetTop(adornedElement, Canvas.GetTop(adornedElement) + args.VerticalChange);
            }
        }


        /// <summary>
        /// Handler for resizing from the top-left.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="args">The <see cref="System.Windows.Controls.Primitives.DragDeltaEventArgs"/> instance containing the event data.</param>
        void HandleTopLeft(object sender, DragDeltaEventArgs args)
        {
            FrameworkElement adornedElement = AdornedElement as FrameworkElement;
            Thumb hitThumb = sender as Thumb;

            if (adornedElement == null || hitThumb == null) return;

            // Ensure that the Width and Height are properly initialized after the resize.
            EnforceSize(adornedElement);

            // Change the size by the amount the user drags the mouse, as long as it's larger 
            // than the width or height of an adorner, respectively.
            double xchange = adornedElement.Width - args.HorizontalChange;
            if (xchange >= adornedElement.MinWidth)
            {
                adornedElement.Width = xchange;
                Canvas.SetLeft(adornedElement, Canvas.GetLeft(adornedElement) + args.HorizontalChange);
            }
            //adornedElement.Width = Math.Max(adornedElement.Width - args.HorizontalChange, hitThumb.DesiredSize.Width);
            double ychange = adornedElement.Height - args.VerticalChange;
            if (ychange >= adornedElement.MinHeight)
            {
                adornedElement.Height = ychange;
                Canvas.SetTop(adornedElement, Canvas.GetTop(adornedElement) + args.VerticalChange);
            }
            //adornedElement.Height = Math.Max(adornedElement.Height - args.VerticalChange, hitThumb.DesiredSize.Height);
        }


        /// <summary>
        /// When overridden in a derived class, positions child elements and determines a size for a <see cref="T:System.Windows.FrameworkElement"></see> derived class.
        /// </summary>
        /// <param name="finalSize">The final area within the parent that this element should use to arrange itself and its children.</param>
        /// <returns>The actual size used.</returns>
        protected override Size ArrangeOverride(Size finalSize)
        {
            // desiredWidth and desiredHeight are the width and height of the element that's being adorned.  
            // These will be used to place the ResizingAdorner at the corners of the adorned element.  
            double desiredWidth = AdornedElement.DesiredSize.Width;
            double desiredHeight = AdornedElement.DesiredSize.Height;
            // adornerWidth & adornerHeight are used for placement as well.
            double adornerWidth = this.DesiredSize.Width;
            double adornerHeight = this.DesiredSize.Height;

            topLeft.Arrange(new Rect(-adornerWidth / 2 - 5, -adornerHeight / 2 - 5, adornerWidth, adornerHeight));
            topRight.Arrange(new Rect(desiredWidth - adornerWidth / 2 + 5, -adornerHeight / 2 - 5, adornerWidth, adornerHeight));
            bottomLeft.Arrange(new Rect(-adornerWidth / 2 - 5, desiredHeight - adornerHeight / 2 + 5, adornerWidth, adornerHeight));
            bottomRight.Arrange(new Rect(desiredWidth - adornerWidth / 2 + 5, desiredHeight - adornerHeight / 2 + 5, adornerWidth, adornerHeight));
            if (adornerWidth + 10 >= this.MinWidth)
                mainrec.Width = adornerWidth + 10;
            mainrec.Height = adornerHeight + 10;

            mainrec.Arrange(new Rect(-5, -5, adornerWidth + 10, adornerHeight + 10));

            // Return the final size.
            return finalSize;
        }


        /// <summary>
        /// Helper method to instantiate the corner Thumbs, set the Cursor property, 
        /// set some appearance properties, and add the elements to the visual tree.
        /// </summary>
        /// <param name="cornerThumb">The corner thumb.</param>
        /// <param name="customizedCursor">The customized cursor.</param>
        void BuildAdornerCorner(ref DefaultThumb cornerThumb, Cursor customizedCursor)
        {
            if (cornerThumb != null) return;

            cornerThumb = new DefaultThumb();

            // Set some arbitrary visual characteristics.
            cornerThumb.Cursor = customizedCursor;
            cornerThumb.Height = cornerThumb.Width = 10;
            cornerThumb.Opacity = 1.0;
            //cornerThumb.BorderBrush = Brushes.DimGray;
            //cornerThumb.BorderThickness = new Thickness(0.5);

            cornerThumb.Background = new SolidColorBrush(Colors.Orange);

            visualChildren.Add(cornerThumb);
        }

        /// <summary>
        /// This method ensures that the Widths and Heights are initialized.  Sizing to content produces
        /// Width and Height values of Double.NaN.  Because this Adorner explicitly resizes, the Width and Height</summary>
        /// need to be set first.  It also sets the maximum size of the adorned element.<param name="adornedElement">The adorned element.</param>
        void EnforceSize(FrameworkElement adornedElement)
        {
            if (adornedElement.Width.Equals(Double.NaN))
                adornedElement.Width = adornedElement.DesiredSize.Width;
            if (adornedElement.Height.Equals(Double.NaN))
                adornedElement.Height = adornedElement.DesiredSize.Height;

            FrameworkElement parent = adornedElement.Parent as FrameworkElement;
            if (parent != null)
            {
                adornedElement.MaxHeight = parent.ActualHeight;
                adornedElement.MaxWidth = parent.ActualWidth;
            }
        }
        // Override the VisualChildrenCount and GetVisualChild properties to interface with 
        // the adorner's visual collection.
        protected override int VisualChildrenCount { get { return visualChildren.Count; } }
        /// <summary>
        /// Overrides <see cref="M:System.Windows.Media.Visual.GetVisualChild(System.Int32)"></see>, and returns a child at the specified index from a collection of child elements.
        /// </summary>
        /// <param name="index">The zero-based index of the requested child element in the collection.</param>
        /// <returns>
        /// The requested child element. This should not return null; if the provided index is out of range, an exception is raised.
        /// </returns>
        protected override Visual GetVisualChild(int index) { return visualChildren[index]; }
        #endregion
    }
    
}
