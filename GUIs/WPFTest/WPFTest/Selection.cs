using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;

namespace WPFApplication1
{

    internal class Selection : Canvas
    {

        private Canvas canvas;
        private double initialAngle;
        private Point initialCenter;
        private Size initialExtent;
        private Point initialMousePosition;
        public bool moving;
        private Rectangle resizeBL;
        private Rectangle resizeBR;
        private Rectangle resizeTL;
        private Rectangle resizeTR;
        public bool resizingBL;
        public bool resizingBR;
        public bool resizingTL;
        public bool resizingTR;
        private Ellipse rotate;
        public bool rotating;
        private Item selectedItem;

        public Selection(Canvas canvas)
        {
            selectedItem = null;
            this.canvas = null;
            moving = false;
            resizingTL = false;
            resizingTR = false;
            resizingBL = false;
            resizingBR = false;
            rotating = false;
            this.canvas = canvas;
            Visibility = Visibility.Hidden;
            Background = new SolidColorBrush(Color.FromArgb(150, 200, 200, 200));
            Panel.SetZIndex(this, 2147483647);
            resizeTL = new Rectangle();
            resizeTL.Height = 5.0;
            resizeTL.Width = 5.0;
            resizeTL.Stroke = new SolidColorBrush(Color.FromArgb(255, 0, 0, 0));
            resizeTL.Fill = new SolidColorBrush(Color.FromArgb(255, 200, 200, 200));
            resizeTL.MouseLeftButtonDown += new MouseButtonEventHandler(resizeTL_MouseLeftButtonDown);
            Children.Add(resizeTL);
            resizeTR = new Rectangle();
            resizeTR.Height = 5.0;
            resizeTR.Width = 5.0;
            resizeTR.Stroke = new SolidColorBrush(Color.FromArgb(255, 0, 0, 0));
            resizeTR.Fill = new SolidColorBrush(Color.FromArgb(255, 200, 200, 200));
            resizeTR.MouseLeftButtonDown += new MouseButtonEventHandler(resizeTR_MouseLeftButtonDown);
            Children.Add(resizeTR);
            resizeBL = new Rectangle();
            resizeBL.Height = 5.0;
            resizeBL.Width = 5.0;
            resizeBL.Stroke = new SolidColorBrush(Color.FromArgb(255, 0, 0, 0));
            resizeBL.Fill = new SolidColorBrush(Color.FromArgb(255, 200, 200, 200));
            resizeBL.MouseLeftButtonDown += new MouseButtonEventHandler(resizeBL_MouseLeftButtonDown);
            Children.Add(resizeBL);
            resizeBR = new Rectangle();
            resizeBR.Height = 5.0;
            resizeBR.Width = 5.0;
            resizeBR.Stroke = new SolidColorBrush(Color.FromArgb(255, 0, 0, 0));
            resizeBR.Fill = new SolidColorBrush(Color.FromArgb(255, 200, 200, 200));
            resizeBR.MouseLeftButtonDown += new MouseButtonEventHandler(resizeBR_MouseLeftButtonDown);
            Children.Add(resizeBR);
            rotate = new Ellipse();
            rotate.Height = 5.0;
            rotate.Width = 5.0;
            rotate.Stroke = new SolidColorBrush(Color.FromArgb(255, 0, 0, 0));
            rotate.Fill = new SolidColorBrush(Color.FromArgb(255, 200, 200, 200));
            rotate.MouseLeftButtonDown += new MouseButtonEventHandler(rotate_MouseLeftButtonDown);
            Children.Add(rotate);
            MouseLeftButtonDown += new MouseButtonEventHandler(Selection_MouseLeftButtonDown);
        }

        private void AllMouseLeftButtonDown()
        {
            // trial
        }

        internal void mouseMove()
        {
            bool flag = Mouse.LeftButton != MouseButtonState.Pressed;
            if (!flag)
            {
                flag = !moving;
                if (!flag)
                {
                    Move();
                }
                else
                {
                    flag = !resizingTL;
                    if (!flag)
                    {
                        ResizeTL();
                    }
                    else
                    {
                        flag = !resizingTR;
                        if (!flag)
                        {
                            ResizeTR();
                        }
                        else
                        {
                            flag = !resizingBL;
                            if (!flag)
                            {
                                ResizeBL();
                            }
                            else
                            {
                                flag = !resizingBR;
                                if (!flag)
                                {
                                    ResizeBR();
                                }
                                else
                                {
                                    flag = !rotating;
                                    if (!flag)
                                        Rotate();
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                Stop();
            }
        }

        public void Move()
        {
            // trial
        }

        internal void ResizeBL()
        {
            Point point1 = Mouse.GetPosition(canvas);
            Point point2 = new Point(point1.X - initialMousePosition.X, point1.Y - initialMousePosition.Y);
            Size size = selectedItem.Extent;
            bool flag = (size.Width - point1.X) >= 10.0;
            if (!flag)
            {
                size = selectedItem.Extent;
                point1.X = size.Width - 10.0;
            }
            size = selectedItem.Extent;
            flag = (size.Height + point1.Y) >= 10.0;
            if (!flag)
            {
                size = selectedItem.Extent;
                point1.Y = 10.0 - size.Height;
            }
        }

        private void resizeBL_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            resizingBL = true;
            AllMouseLeftButtonDown();
            e.Handled = true;
        }

        internal void ResizeBR()
        {
            // trial
        }

        private void resizeBR_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            // trial
        }

        internal void ResizeTL()
        {
            Point point1 = Mouse.GetPosition(selectedItem);
            Point point2 = new Point(point1.X - initialMousePosition.X, point1.Y - initialMousePosition.Y);
            selectedItem.Center = new Point(initialCenter.X + (point2.X / 2.0), initialCenter.Y + (point2.Y / 2.0));
            selectedItem.Extent = new Size(initialExtent.Width - (point2.X / 2.0), initialExtent.Height - (point2.Y / 2.0));
            Update();
        }

        private void resizeTL_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            resizingTL = true;
            AllMouseLeftButtonDown();
            e.Handled = true;
        }

        internal void ResizeTR()
        {
            // trial
        }

        private void resizeTR_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            // trial
        }

        internal void Rotate()
        {
            Point point = Mouse.GetPosition(canvas);
        }

        private void rotate_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            rotating = true;
            AllMouseLeftButtonDown();
            e.Handled = true;
        }

        internal void Select(Item item)
        {
            selectedItem = item;
            Update();
            Visibility = Visibility.Visible;
        }

        private void Selection_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            // trial
        }

        internal void Stop()
        {
            // trial
        }

        public void Update()
        {
            // trial
        }

    } // class Selection

}

