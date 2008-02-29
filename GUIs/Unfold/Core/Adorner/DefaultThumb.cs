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
    /// Default thumb (hook) with which the adorner can be manipulated (and hence resize the shape).
    /// </summary>
    class DefaultThumb : Thumb
    {
        protected override Visual GetVisualChild(int index)
        {
            //return base.GetVisualChild(index);
            return null;
        }
        protected override void OnRender(DrawingContext drawingContext)
        {
            //base.OnRender(drawingContext);
            drawingContext.DrawRoundedRectangle(Brushes.White, new Pen(Brushes.Red, 1), new Rect(this.DesiredSize), 1, 1);

        }
    }
}
