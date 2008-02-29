using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Controls;
using System.Windows;
using System.Windows.Shapes;
using System.Windows.Media;
using System.Windows.Documents;
using Orbifold.Unfold.Core;
namespace Orbifold.Unfold.Main
{
    /// <summary>
    /// Keeps track of the selected shape(s).
    /// </summary>
    static class SelectionManager
    {
        #region Fields
        private static Brush defaultBrush = Application.Current.FindResource("NormalBackgroundBrush") as Brush;
        private static UIElement selectedShape;
        private static Canvas mCanvas;
        private static Brush previousBrush = Brushes.DimGray;
        private static Brush selectionBrush = Brushes.OrangeRed;
        /// <summary>
        /// Volatile adornment layer.
        /// </summary>
        private static AdornerLayer adornerLayer;
        #endregion

        #region Properties

        public static UIElement SelectedShape
        {
            get { return selectedShape; }
            set { selectedShape = value; }
        }

        public static Canvas Canvas
        {
            get { return mCanvas; }
            set { mCanvas = value; }
        }
        #endregion

        #region Constructor
        static SelectionManager()
        { 
            
        }
        #endregion

        #region Methods
        public static void UnselectShape(UIElement shape)
        {
            if (shape == null) return;
            //shape.Fill = defaultBrush;
            selectedShape = null;
        }
        public static void UnselectAll()
        {

        }
        public static void SelectShape(UIElement shape)
        {
            if (selectedShape != null)
            {
                //selectedShape.Fill = defaultBrush;
                //selectedShape.Stroke = previousBrush;
                //selectedShape.StrokeThickness = 1D;
                //aiaiaia, the stuff above does not work for the simple UIElement
                //you need to use the adorners in order to visualize selections
                RemoveAdorner(selectedShape);
            }
            selectedShape = shape;
            AddAdorner(selectedShape);
            //previousBrush = selectedShape.Stroke;
            //shape.Stroke = selectionBrush;
            //shape.StrokeThickness = 2D;
            //shape.Fill = Brushes.LightYellow;
        }
        #region Adornmental methods
        private static void AddAdorner(UIElement rec)
        {
            
            adornerLayer = AdornerLayer.GetAdornerLayer(rec);
            System.Diagnostics.Debug.Assert(adornerLayer != null, "Oops, you have not defined an AdornerDecorator in the template, check the generic.xaml file.");
            if (adornerLayer == null)
                return;
            if ( adornerLayer.GetAdorners(rec) == null)
            {
                ResizingAdorner rad = new ResizingAdorner(rec);

                if (rec is Connectable)
                {
                    rad.HeightEnabled = (rec as Connectable).ResizableHeight;
                    rad.WidthEnabled = (rec as Connectable).ResizableWidth;
                    rad.MinHeight = (rec as Connectable).MinHeight;
                    rad.MinWidth = (rec as Connectable).MinWidth;
                }

                adornerLayer.Add(rad);
            }

        }

        private static void RemoveAdorner(UIElement rec)
        {
            adornerLayer = AdornerLayer.GetAdornerLayer(rec);
            System.Diagnostics.Debug.Assert(adornerLayer != null, "Oops, you have not defined an AdornerDecorator in the template, check the generic.xaml file.");
            if (adornerLayer!=null && adornerLayer.GetAdorners(rec) != null)
                adornerLayer.Remove(adornerLayer.GetAdorners(rec)[0]);
        }
        #endregion

        #endregion
    }
}
