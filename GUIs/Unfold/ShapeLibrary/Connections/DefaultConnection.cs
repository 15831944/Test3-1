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
using Orbifold.Unfold.Core;
namespace Orbifold.Unfold.ShapeLibrary
{
    public class DefaultConnection : Connection
    {


        protected override System.Windows.Media.Geometry DefiningGeometry
        {
            get
            {
                StreamGeometry sg = new StreamGeometry();
                using (StreamGeometryContext ctx = sg.Open())
                {
                    ctx.BeginFigure(new Point(ArrowLength, ArrowWidth), true, true);
                    ctx.LineTo(new Point(Width - ArrowLength, ArrowWidth), true, true);
                    ctx.LineTo(new Point(Width - ArrowLength, 0), true, true);
                    ctx.LineTo(new Point(Width, ArrowWidth), true, true);
                    ctx.LineTo(new Point(Width - ArrowLength, ArrowWidth * 2), true, true);
                    ctx.LineTo(new Point(Width - ArrowLength, ArrowWidth), true, true);

                    ctx.BeginFigure(new Point(0, ArrowWidth), true, true);
                    ctx.LineTo(new Point(ArrowLength / 2, 2*ArrowWidth), true, true);
                    ctx.LineTo(new Point(ArrowLength, ArrowWidth), true, true);
                    ctx.LineTo(new Point(ArrowLength / 2, 0), true, true);
                    ctx.LineTo(new Point(0, ArrowWidth), true, true);

                }
                return sg;
            }
        }
    }
}
