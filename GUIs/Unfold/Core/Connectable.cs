using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Collections.ObjectModel;
using System.ComponentModel;
namespace Orbifold.Unfold.Core
{
    /// <summary>
    /// Abstract base class for connectable shapes, i.e. shapes which allow a connection.
    /// </summary>
    public abstract class Connectable : Control, IConnectable, INotifyPropertyChanged
    {
        #region Fields
        private List<Connection> mConnections = new List<Connection>();

        public List<Connection> Connections
        {
            get { return mConnections; }
          
        }
        private bool mAnimationEnabled = true;
        private bool mResizableHeight = true;
        private bool mResizableWidth = true;
        //public static DependencyProperty MessageProperty = DependencyProperty.Register("Message", typeof(string), typeof(CustomRender));
        #endregion

        #region Properties
        public static DependencyProperty WhoProperty = DependencyProperty.Register("Who", typeof(Connectable), typeof(Connectable));
        /// <summary>
        /// Tricky thing this one. It's a dependency property that returns the entity as a whole and is being used to notify the data binding of the connections
        /// that the entity has moved.
        /// </summary>        
        public Connectable Who
        {
            get { return (Connectable)GetValue(WhoProperty); }
            set
            {
                SetValue(WhoProperty, value);
                //raise the event

                //RaiseResizeBodyEvent();
            }
        }
        public static DependencyProperty ShapeColorProperty = DependencyProperty.Register("ShapeColor", typeof(Color), typeof(Connectable));

        /// <summary>
        /// Gets or sets the color of the shape.
        /// </summary>
        /// <value>The color of the shape.</value>
        public Color ShapeColor
        {
            get { return (Color)GetValue(ShapeColorProperty); }
            set { SetValue(ShapeColorProperty, value); }
        }
        /// <summary>
        /// Gets or sets a value indicating whether animation is enabled.
        /// </summary>
        /// <value><c>true</c> if [animation enabled]; otherwise, <c>false</c>.</value>
        public bool AnimationEnabled
        {
            get { return mAnimationEnabled; }
            set { mAnimationEnabled = value; }
        }
        /// <summary>
        /// Gets or sets a value indicating whether the height is resizable.
        /// </summary>
        /// <value><c>true</c> if the height is resizable; otherwise, <c>false</c>.</value>
        public bool ResizableHeight
        {
            get { return mResizableHeight; }
            set { mResizableHeight = value; }
        }
        /// <summary>
        /// Gets or sets a value indicating whether the width is resizable.
        /// </summary>
        /// <value><c>true</c> if width is resizable; otherwise, <c>false</c>.</value>
        public bool ResizableWidth
        {
            get { return mResizableWidth; }
            set { mResizableWidth = value; }
        }
        #endregion

        #region Constructor
        ///<summary>
        ///Default constructor
        ///</summary>
        public Connectable()
        {
            this.MinWidth = 50;
            Who = this;
        }
        #endregion
  
        #region Methods
        /// <summary>
        /// Updates the connections.
        /// </summary>
        public void RefreshConnections()
        {
            this.Connections.ForEach(
                            delegate(Connection cn)
                            {
                                //dirty trick here, let the class notify the binding that things have changed
                                //such that the converter can do its work and update the location of the connection
                                OnPropertyChanged("Who");
                                cn.InvalidateVisual();
                            }
                            );
        }
        /// <summary>
        /// Binds the given connection to the shape at the specified connector location.
        /// </summary>
        /// <param name="con">The connection.</param>
        /// <param name="property">The property of the connection to bind to.</param>
        /// <param name="location">The location (Right, 
        /// Left, Top or Bottom).</param>
        public void BindConnection(Connection con, DependencyProperty property, ConnectorLocation location)
        {
            //A multibinding is like a many-to-one function which returns a value for a dependency property 
            //in terms of multiple values.
            //The WidthConverter is the class that does the actual conversion of data to one Point-value.
            //MultiBinding mb = new MultiBinding();
            Binding bb = new Binding("Who");
            bb.Source = this;
            bb.Converter = new ConnectorConverter2();
            bb.NotifyOnSourceUpdated = true;
            bb.Mode = BindingMode.OneWay;
            bb.UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged;
            //mb.Converter = new ConnectorConverter();
            //this is nasty hack, don't like it but the connection will not be visible if the
            //converter does not return because width and/or height is NaN. Argh.
            //if (double.IsNaN(this.Width))
            //    this.Width = this.DesiredSize.Width;
            //if (double.IsNaN(this.Height))
            //    this.Height = this.DesiredSize.Height;

            //mb.Mode = BindingMode.TwoWay;
            //mb.NotifyOnSourceUpdated = true;

            //Because of the multiple locations we have to switch between the various possibilities.
            switch (location)
            {
                case ConnectorLocation.Top:
                    bb.ConverterParameter = "Top";
                    break;
                case ConnectorLocation.Right:
                    bb.ConverterParameter = "Right";
                    break;
                case ConnectorLocation.Bottom:
                    bb.ConverterParameter = "Bottom";
                    break;
                case ConnectorLocation.Left:
                    bb.ConverterParameter = "Left";
                    break;
                default:
                    break;
            }

            //Binding bx = new Binding("(Canvas.Left)");
            //bx.Source = this;
            //bx.Mode = BindingMode.TwoWay;
            //bx.NotifyOnSourceUpdated = true;
            //mb.Bindings.Add(bx);

            //Binding by = new Binding("(Canvas.Top)");
            //by.Source = this;
            ////bwh.Mode = BindingMode.TwoWay;
            //mb.Bindings.Add(by);

            //Binding bw = new Binding("Width");            
            //bw.Source = this;
            ////bww.Mode = BindingMode.TwoWay;
            //mb.Bindings.Add(bw);

            //Binding bh = new Binding("Height");
            //bh.Source = this;
            ////bww.Mode = BindingMode.TwoWay;
            //mb.Bindings.Add(bh);

            con.SetBinding(property, bb);
           
            this.mConnections.Add(con);
        }
        #endregion

        #region INotifyPropertyChanged Members


        public event PropertyChangedEventHandler PropertyChanged;
        public void OnPropertyChanged(string name)
        {

            PropertyChangedEventHandler handler = PropertyChanged;

            if (handler != null)
            {

                handler(this, new PropertyChangedEventArgs(name));

            }

        }
        #endregion

    }
}
