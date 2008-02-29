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
    /// <summary>
    /// Abstract base class for a diagram connection.
    /// </summary>
    public abstract class Connection : Shape
    {
       
        #region Fields
        public static DependencyProperty StartProperty = DependencyProperty.Register("StartPoint", typeof(Point), typeof(Connection));
        public static DependencyProperty EndProperty = DependencyProperty.Register("EndPoint", typeof(Point), typeof(Connection));
        private double angle;
        private double length;       
        protected const int ArrowWidth = 5;
        protected const int ArrowLength = 15;
        #endregion

        #region Properties
        /// <summary>
        /// Gets or sets the starting point of the connection.
        /// </summary>
        public  Point StartPoint
        {
            get { return (Point)GetValue(StartProperty); }
            set { SetValue(StartProperty, value); }
        }
        /// <summary>
        /// Gets or sets the endpoint of the connection.
        /// </summary>
        public  Point EndPoint
        {
            get { return (Point)GetValue(EndProperty); }
            set { SetValue(EndProperty, value);
           
            }
        }
       /// <summary>
       /// Required overriden geometry.
       /// The arrow is painted at the top of the bouding rectangle and is shift/rotated in the overriden layout method.
       /// </summary>
        protected abstract override Geometry DefiningGeometry
        {
            get;

        }        
        /// <summary>
        /// Overriden layout
        /// </summary>
        /// <param name="finalSize"></param>
        /// <returns></returns>
        protected sealed override Size ArrangeOverride(Size finalSize)
        {
            //the computed distance between start and end point
            length = Math.Sqrt((EndPoint.X - StartPoint.X) * (EndPoint.X - StartPoint.X) + (EndPoint.Y - StartPoint.Y) * (EndPoint.Y - StartPoint.Y));
            //we make the size of this shape precisely big enough to contain the arrow
            this.Width = length;
            this.Height = 2*ArrowWidth+2;

            //using the inverse tangent to know how much we have to rotate the total shape in the RenderTransform
            angle = Math.Atan2(Math.Abs(EndPoint.Y - StartPoint.Y), Math.Abs(EndPoint.X - StartPoint.X));
            //the actual angle depends on the respective positions of the two connected shapes
            if ((EndPoint.X >= StartPoint.X) && (EndPoint.Y < StartPoint.Y))
            {
                angle = -angle;
            }
            else if ((EndPoint.X <= StartPoint.X) && (EndPoint.Y > StartPoint.Y))
            {
                angle = Math.PI - angle;
            }
            else if ((EndPoint.X > StartPoint.X) && (EndPoint.Y >= StartPoint.Y))
            {
                //the angle is OK in this case
            }
            else if ((EndPoint.X < StartPoint.X) && (EndPoint.Y <= StartPoint.Y))
            {
                angle = angle - Math.PI;
            }
            //now here's a bit a bias; the width of the arrow shifts/resizes the actual bounding rectangle,
            //to make the arrow's position correspond to the start and end position we have to cos/sin the width.
            //In essence this is caused by the fact that it's not the begin/end of the arrow that is rotating but
            //the real bounding rectangle and this one depends on the width of the arrow!
            Canvas.SetLeft(this, StartPoint.X + ArrowWidth * Math.Cos(Math.PI / 2 - angle));
            Canvas.SetTop(this, StartPoint.Y - ArrowWidth * Math.Sin(Math.PI / 2 - angle));
            //do the rotation
            this.RenderTransform = new RotateTransform(angle * 180 / Math.PI, 0, 0);            
            //return the expected stuff to the engine.
            base.ArrangeOverride(new Size(Width, Height));
            return new Size(Width, Height);
          
        }
    
        
        #endregion

        #region Constructor
        /// <summary>
        /// Just a static constructor to make sure something is visible even if the required params are not defined
        /// </summary>
        static Connection()
        {
            // Make *something* appear when first adding it.
            Shape.StretchProperty.OverrideMetadata(typeof(Connection), new FrameworkPropertyMetadata(Stretch.Uniform));            
            Shape.FillProperty.OverrideMetadata(typeof(Connection), new FrameworkPropertyMetadata(Brushes.Gray));
        }
       
        #endregion

        

    }
}
