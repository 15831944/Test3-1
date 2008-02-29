using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using Orbifold.Unfold.ShapeLibrary;
using Orbifold.Unfold.Core;

using System.Reflection;
using System.Collections.ObjectModel;
namespace Orbifold.Unfold.Main
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : System.Windows.Window
    {
        Connection con;
        //a sample list of types to display
        Type[] sampleTypes = new Type[] { typeof(UndoManager), typeof(ShapeFactory), 
            typeof(MoveTool), typeof(GhostManager), typeof(BackgroundConverter), 
            typeof(Shape), typeof(GeometryCollection), typeof(RadialGradientBrush), typeof(DependencyProperty),
            typeof(TableCell), typeof(LineSegment)};
        #region Constructor
        public Window1()
        {
            InitializeComponent();
            ToolManager.Canvas = canvas;
            ShapeFactory.Canvas = canvas;
            SelectionManager.Canvas = canvas;
            GhostManager.Canvas = canvas;
            this.WindowStartupLocation = WindowStartupLocation.CenterScreen;
            canvas.MouseLeftButtonDown += OnLeftDown;
            canvas.MouseLeftButtonUp += OnLeftUp;
            canvas.MouseMove += OnMouseMove;
            canvas.KeyDown += new KeyEventHandler(canvas_KeyDown);
            canvas.ClipToBounds = true;

            canvas.Focusable = true;
            canvas.Focus();

            ShapeFactory.OnShapeAdded += new EventHandler<ShapeEventArgs>(ShapeFactory_OnShapeAdded);

        }

        void ShapeFactory_OnShapeAdded(object sender, ShapeEventArgs e)
        {
            if (e.Shape is ClassShape)
                InjectReflectedData(e.Shape as ClassShape);
        }
        private void InjectReflectedData(ClassShape newShape)
        {
            Random rnd = new Random();

            Type type = sampleTypes[rnd.Next(0, sampleTypes.Length)];


            MethodInfo[] meths = type.GetMethods();
            PropertyInfo[] props = type.GetProperties();

            ObservableCollection<ClassMember> methods = new ObservableCollection<ClassMember>();
            for (int i = 0; i < meths.Length && i<10; i++)
            {
                methods.Add(new ClassMember(meths[i].Name, "PublicMethod"));
            }

           
            ClassCategory methodSection = new ClassCategory("Methods", methods);

            ObservableCollection<ClassMember> properties = new ObservableCollection<ClassMember>();
            for (int i = 0; i < props.Length && i<10; i++)
            {
                properties.Add(new ClassMember(props[i].Name, "PublicProperty"));
            }
            ;
            ClassCategory propertySection = new ClassCategory("Properties", properties);

            ObservableCollection<ClassCategory> categories = new ObservableCollection<ClassCategory>();

            categories.Add(propertySection);
            categories.Add(methodSection);

            newShape.Title = type.Name;
            if (type.IsPublic)
                newShape.SubTitle += "public ";
            //if (type.IsSealed)
            //    newShape.SubTitle += "sealed ";
            if(type.IsAbstract)
                newShape.SubTitle += "abstract ";
            if(type.IsInterface)
                newShape.SubTitle += "interface ";

            newShape.SubTitle += "class";
            
            newShape.TreeData = categories;
        }
        #endregion

        #region Methods

        void OnLoad(object sender, EventArgs e)
        {
            //RunDemo();
            
        }

        #region Pass through user interaction
        /// <summary>
        /// Canvas KeyDown handler
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void canvas_KeyDown(object sender, KeyEventArgs e)
        {
            //if (selectedShape != null && e.Key == Key.Delete)
            //{
            //    elements.Remove(selectedShape);
            //    canvas.Children.Remove(selectedShape);
            //}
        }
        /// <summary>
        /// Canvas LeftDown handler
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        protected void OnLeftDown(object sender, MouseEventArgs args)
        {
            ToolManager.MouseDown(args);
        }
        /// <summary>
        /// Canvas MouseMove handler
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        protected void OnMouseMove(object sender, MouseEventArgs args)
        {
            ToolManager.MouseMove(args);
        }
        /// <summary>
        /// Canvas LeftUp handler
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        protected void OnLeftUp(object sender, MouseEventArgs args)
        {
            ToolManager.MouseUp(args);
        }
        #endregion

     

        #endregion
    }
}