using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Collections.Generic;
using System;

namespace WPFTest
{

  public class Tank : Stencil
  {

    private Rectangle backRect;
    private Ellipse ellipseBottom;
    private Ellipse ellipseTop;
    private Rectangle fillRect;

    public override void Update()
    {
      Canvas.SetTop(fillRect, 85.0 - base.Value("Level"));
      fillRect.Height = base.Value("Level");

      fillRect.Fill = new SolidColorBrush(Color.FromRgb((byte)(base.Value("Temperature")*2.0), 0, 0));
    }

    //public override double Temperature
    //{
    //    set
    //    {
    //        base.Temperature = value;
    //        Canvas.SetTop(fillRect, 85.0 - base.Temperature);
    //        fillRect.Height = base.Temperature;
    //        bool flag = !MinTemperature;
    //        if (!flag)
    //        {
    //            fillRect.Fill = Brushes.Blue;
    //        }
    //        else
    //        {
    //            flag = !MaxTemperature;
    //            if (!flag)
    //                fillRect.Fill = Brushes.Red;
    //            else
    //                fillRect.Fill = Brushes.Green;
    //        }
    //    }
    //}

    public Tank()
      : base(new Dictionary<String, Parameter>() 
      { 
      { "Temperature", new Parameter(0.0, 0.0, 100.0, ".T") }, 
      { "Level", new Parameter(0.0, 0.0, 60.0, ".L") } 
      })
    {
      ellipseTop = new Ellipse();
      ellipseBottom = new Ellipse();
      backRect = new Rectangle();
      fillRect = new Rectangle();
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
      fillRect.Width = 90.0;
      
      Children.Add(ellipseTop);
      Children.Add(ellipseBottom);
      Children.Add(backRect);
      Children.Add(fillRect);

      Update();
    }

  } // class Tank

}

