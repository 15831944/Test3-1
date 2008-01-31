using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;

namespace Root
{

    public class Tank : Stencil
    {

        private Rectangle backRect;
        private Ellipse ellipseBottom;
        private Ellipse ellipseTop;
        private Rectangle fillRect;

        public override double Temperature
        {
            set
            {
                base.Temperature = value;
                Canvas.SetTop(fillRect, 85.0 - base.Temperature);
                fillRect.Height = base.Temperature;
                bool flag = !MinTemperature;
                if (!flag)
                {
                    fillRect.Fill = Brushes.Blue;
                }
                else
                {
                    flag = !MaxTemperature;
                    if (!flag)
                        fillRect.Fill = Brushes.Red;
                    else
                        fillRect.Fill = Brushes.Green;
                }
            }
        }

        public Tank()
        {
            ellipseTop = new Ellipse();
            ellipseBottom = new Ellipse();
            backRect = new Rectangle();
            fillRect = new Rectangle();
            TemperatureMin = 0.0;
            TemperatureMax = 70.0;
            Canvas.SetLeft(ellipseTop, 0.0);
            Canvas.SetTop(ellipseTop, 0.0);
            ellipseTop.Width = 100.0;
            ellipseTop.Height = 20.0;
            GradientStopCollection gradientStopCollection = new GradientStopCollection();
            gradientStopCollection.Add(new GradientStop(Color.FromArgb(255, 200, 190, 160), 0.0));
            gradientStopCollection.Add(new GradientStop(Color.FromArgb(255, 100, 90, 60), 1.0));
            ellipseTop.Fill = new LinearGradientBrush(gradientStopCollection);
            Canvas.SetLeft(ellipseBottom, 0.0);
            Canvas.SetTop(ellipseBottom, 80.0);
            ellipseBottom.Width = 100.0;
            ellipseBottom.Height = 20.0;
            gradientStopCollection = new GradientStopCollection();
            gradientStopCollection.Add(new GradientStop(Color.FromArgb(255, 200, 190, 160), 0.0));
            gradientStopCollection.Add(new GradientStop(Color.FromArgb(255, 100, 90, 60), 1.0));
            ellipseBottom.Fill = new LinearGradientBrush(gradientStopCollection);
            Canvas.SetLeft(backRect, 0.0);
            Canvas.SetTop(backRect, 10.0);
            backRect.Width = 100.0;
            backRect.Height = 80.0;
            gradientStopCollection = new GradientStopCollection();
            gradientStopCollection.Add(new GradientStop(Color.FromArgb(255, 200, 190, 160), 0.0));
            gradientStopCollection.Add(new GradientStop(Color.FromArgb(255, 100, 90, 60), 1.0));
            backRect.Fill = new LinearGradientBrush(gradientStopCollection);
            Canvas.SetLeft(fillRect, 5.0);
            Canvas.SetTop(fillRect, 85.0 - base.Temperature);
            fillRect.Width = 90.0;
            fillRect.Height = base.Temperature;
            bool flag = !MinTemperature;
            if (!flag)
            {
                fillRect.Fill = Brushes.Blue;
            }
            else
            {
                flag = !MaxTemperature;
                if (!flag)
                    fillRect.Fill = Brushes.Red;
                else
                    fillRect.Fill = Brushes.Green;
            }
            Children.Add(ellipseTop);
            Children.Add(ellipseBottom);
            Children.Add(backRect);
            Children.Add(fillRect);
        }

    } // class Tank

}

