using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using Root;
using System;

namespace WPFApplication1
{

    internal class Item : Canvas
    {

        private double angle;
        private Point center;
        private Size extent;
        public Stencil stencil;
        public Viewbox viewbox;

        public double Angle
        {
            get
            {
                return angle;
            }
            set
            {
                angle = value;
                Update();
            }
        }

        public Point Center
        {
            get
            {
                return center;
            }
            set
            {
                center = value;
                Update();
            }
        }

        public Size Extent
        {
            get
            {
                return extent;
            }
            set
            {
                extent = value;
                Update();
            }
        }

        public Item(Point center, Size extent, double angle, Selection selection)
        {
            this.center = new Point(0.0, 0.0);
            this.extent = new Size(50.0, 50.0);
            this.angle = 0.0;
            stencil = new Tank();
            viewbox = new Viewbox();
            viewbox.StretchDirection = StretchDirection.Both;
            viewbox.Stretch = Stretch.Fill;
            viewbox.MaxWidth = 400.0;
            viewbox.MaxHeight = 400.0;
            viewbox.Child = stencil;
            this.center = center;
            this.extent = extent;
            this.angle = angle;
            Children.Add(viewbox);
            Update();
        }

        private void Update()
        {
          if (!Double.IsNaN(center.X))
            Margin = new Thickness(center.X, center.Y, 0.0, 0.0);

            Width = 0.0;
            Height = 0.0;
            viewbox.Margin = new Thickness(-extent.Width / 2.0, -extent.Height / 2.0, 0.0, 0.0);
            viewbox.Width = extent.Width;
            viewbox.Height = extent.Height;
            viewbox.RenderTransform = new RotateTransform(angle);
        }

    } // class Item

}

