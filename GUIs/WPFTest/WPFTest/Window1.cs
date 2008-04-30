using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Shapes;
using WPFTest;
using System.Windows.Threading;
using System.Timers;

namespace WPFApplication1
{

  public class Window1 : Window, IComponentConnector
  {
    Timer redrawTimer;

    //private bool _contentLoaded;
    private Canvas canvas;
    private Dictionary<Guid, Item> items;
    internal Window1 modelWindow;
    public Point mouseLeftButtonDownPosition;
    private Random r;
    private ScrollViewer scrollViewer;
    private Selection selection = null;

    public Window1()
    {
      canvas = new Canvas();

      redrawTimer = new Timer(100);
      redrawTimer.Elapsed += new ElapsedEventHandler(redrawTimer_Elapsed);
      redrawTimer.Start();

      bool flag = true;

      items = new Dictionary<Guid, Item>();
      r = new Random();
      InitializeComponent();
      
      scrollViewer = new ScrollViewer();
      scrollViewer.HorizontalScrollBarVisibility = ScrollBarVisibility.Auto;
      scrollViewer.VerticalScrollBarVisibility = ScrollBarVisibility.Auto;
      Content = scrollViewer;
      canvas.Background = new SolidColorBrush(Color.FromArgb(255, 230, 230, 230));
      scrollViewer.Content = canvas;
      //Rectangle rectangle = new Rectangle();
      //rectangle.Width = Width - 8.0;
      //rectangle.Height = Height - 34.0;
      //rectangle.StrokeThickness = 10.0;
      //rectangle.Stroke = new SolidColorBrush(Color.FromArgb(127, 255, 255, 0));
      //canvas.Children.Add(rectangle);
      //selection = new Selection(canvas);
      //canvas.Children.Add(selection);
      canvas.MouseLeftButtonDown += new MouseButtonEventHandler(canvas_MouseLeftButtonDown);
      canvas.MouseMove += new MouseEventHandler(canvas_MouseMove);
      canvas.MouseLeftButtonUp += new MouseButtonEventHandler(canvas_MouseLeftButtonUp);
      canvas.MouseWheel += new MouseWheelEventHandler(canvas_MouseWheel);
      int i = 0;
      while (flag)
      {
        Item item = new Item(new Point(0.0, 0.0), new Size(0.0, 0.0), 0.0, selection);
        canvas.Children.Add(item);
        i++;
        flag = i < 500;
      }
    }

    void redrawTimer_Elapsed(object sender, ElapsedEventArgs e)
    {
      Redraw();
    }

    private void canvas_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
      Redraw();
    }

    public delegate void RedrawDelegate();

    DateTime previousNow = DateTime.Now;

    private void Redraw()
    {
      if (canvas.Dispatcher.CheckAccess())
      {
        canvas.Width = ActualWidth - 8.0;
        canvas.Height = ActualHeight - 34.0;
        Title = DateTime.Now.Subtract(previousNow).ToString();
        previousNow = DateTime.Now;
        foreach (Item item in canvas.Children)
        {
          item.Opacity = r.NextDouble();
          item.Center = new Point(r.NextDouble() * canvas.ActualWidth, r.NextDouble() * canvas.ActualHeight);
          item.Extent = new Size(r.NextDouble() * canvas.ActualWidth/10.0, r.NextDouble() * canvas.ActualHeight/10.0);
          //item.Angle = r.NextDouble() * 360.0;
          item.stencil.SetValue("Level", (r.NextDouble() * 120.0) - 20.0);
          item.stencil.SetValue("Temperature", (r.NextDouble() * 120.0) - 20.0);
        }
      }
      else
      {
        canvas.Dispatcher.Invoke(DispatcherPriority.Normal, new RedrawDelegate(Redraw));
      }
    }

    private void canvas_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
    {
      //selection.Stop();
    }

    private void canvas_MouseMove(object sender, MouseEventArgs e)
    {
      // trial
    }

    private void canvas_MouseWheel(object sender, MouseWheelEventArgs e)
    {
      // trial
    }

    [DebuggerNonUserCode]
    [EditorBrowsable(EditorBrowsableState.Never)]
    void IComponentConnector.Connect(int connectionId, object target)
    {
      int i = connectionId;
      if (i != 1)
      {
      }
      else
      {
        modelWindow = (Window1)target;
        return;
      }
      //_contentLoaded = true;
    }

    [DebuggerNonUserCode]
    public void InitializeComponent()
    {
      // trial
    }

    private void Item_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
    {
      selection.Select(sender as Item);
      e.Handled = false;
    }

  } // class Window1

}

