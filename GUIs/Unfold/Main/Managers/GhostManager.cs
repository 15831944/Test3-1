using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Shapes;
using System.Windows.Media;
namespace Orbifold.Unfold.Main
{
    /// <summary>
    /// A 'ghost' is the volatile things that appears when you draw a new shape or connection on the canvas, it gives a preview of what
    /// or how the addition will look like. You can play with many ideas here, this is the simplest and most traditional approach (the so-called marching ants).
    /// </summary>
    static class GhostManager
    {
        #region Fields
        private static Canvas mCanvas;
        private static Shape rubberBand;
        private static Rect mRectangle;
        private static DoubleCollection db = new DoubleCollection();
        #endregion

        #region Properties

        public static Rect Rectangle
        {
            get { return mRectangle; }
        }

        public static Canvas Canvas
        {
            get { return mCanvas; }
            set { mCanvas = value; }
        }

        #endregion

        #region Constructor
        static GhostManager()
        {
            db.Add(5D);
            db.Add(3D);
        }
        #endregion

        #region Methods
        public static void DrawConnectionGhost(Point b, Point e)
        {
            if (rubberBand == null)
            {
                rubberBand = new Line();

                (rubberBand as Line).Stroke = new SolidColorBrush(Colors.DimGray);
                (rubberBand as Line).StrokeThickness = 2;

                rubberBand.StrokeDashArray = db;
                Canvas.Children.Add(rubberBand);
            }
            (rubberBand as Line).X1 = b.X;
            (rubberBand as Line).Y1 = b.Y;
            (rubberBand as Line).X2 = e.X;
            (rubberBand as Line).Y2 = e.Y;            
            //Canvas.SetLeft(rubberBand, rec.Left);
            //Canvas.SetTop(rubberBand, rec.Top);
            mRectangle = Rect.Empty;
        }
        public static void DrawRectangularGhost(Rect rec)
        {
            if (rubberBand == null)
            {
                rubberBand = new Rectangle();
                rubberBand.Stroke = new SolidColorBrush(Colors.LightGray);

                rubberBand.StrokeDashArray = db;
                Canvas.Children.Add(rubberBand);
            }

            rubberBand.Width = rec.Width;
            rubberBand.Height = rec.Height;
            Canvas.SetLeft(rubberBand, rec.Left);
            Canvas.SetTop(rubberBand, rec.Top);
            mRectangle = rec;
        }
        public static void DrawEllipticGhost(Rect rec)
        {
            if (rubberBand == null)
            {
                rubberBand = new Ellipse();
                rubberBand.Stroke = new SolidColorBrush(Colors.LightGray);

                rubberBand.StrokeDashArray = db;
                Canvas.Children.Add(rubberBand);
            }

            rubberBand.Width = rec.Width;
            rubberBand.Height = rec.Height;
            Canvas.SetLeft(rubberBand, rec.Left);
            Canvas.SetTop(rubberBand, rec.Top);
            mRectangle = rec;
        }

        public static void Release()
        {
            Canvas.Children.Remove(rubberBand);
            rubberBand = null;
            Canvas.ReleaseMouseCapture();
            mRectangle = Rect.Empty;
        }
        #endregion
    }
}
