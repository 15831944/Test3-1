using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Input;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;
using Core = Orbifold.Unfold.Core;
using Orbifold.Unfold.ShapeLibrary;
namespace Orbifold.Unfold.Main
{
    /// <summary>
    /// Creates a fancy connection between two shapes. You can easily define your own connections by inheriting from the
    /// Connection class.
    /// </summary>
    class CreateConnectionTool : ToolBase, Core.IMouseListener
    {
        HitTestResult result;
        
        #region Constructor
        ///<summary>
        ///Default constructor
        ///</summary>
        public CreateConnectionTool()
            : base("Create Connection")
        {

        }
        #endregion

        #region Fields
        private Point initialPoint;
        #endregion


        #region IMouseListener Members

        /// <summary>
        /// Handles the MouseDown event.
        /// </summary>
        /// <param name="e">The <see cref="MouseEventArgs"/> instance containing the event data.</param>
        /// <returns></returns>
        public bool MouseDown(MouseEventArgs e)
        {

            if (IsActive)
            {
                //MessageBox.Show("Clicked at " + e.GetPosition(Canvas));
                initialPoint = e.GetPosition(Canvas);
                Canvas.CaptureMouse();
            }
            return true;
        }
        /// <summary>
        /// Handles the MouseMove event.
        /// </summary>
        /// <param name="e"></param>
        /// <returns></returns>
        public bool MouseMove(MouseEventArgs e)
        {
            if (IsActive)
            {
                Point currentPoint = e.GetPosition(Canvas);
                if (Canvas.IsMouseCaptured)
                {
                    GhostManager.DrawConnectionGhost(initialPoint, currentPoint);
                }
                return true;
            }
            return false;
        }
        /// <summary>
        /// Handles the MouseUp event.
        /// </summary>
        /// <param name="e"></param>
        /// <returns></returns>
        public bool MouseUp(MouseEventArgs e)
        {
            if (IsActive)
            {
                SelectionManager.UnselectAll();

                if (Canvas.IsMouseCaptured)
                {
                    CreateConnectionCommand cmd = new CreateConnectionCommand(initialPoint, e.GetPosition(Canvas), Canvas);
                    UndoManager.Execute(cmd);
                }
                DeactivateTool();
                GhostManager.Release();
                return true;
            }
            return false;
        }

        #endregion




        /// <summary>
        /// Override this method in order to add the drawn shape, i.e. the shape corresponding to the mouse action.
        /// </summary>
        /// <param name="rec"></param>
        //public abstract void AddShape(Rect rec);

        /// <summary>
        /// Function to execute the command.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public override void OnExecute(object sender, ExecutedRoutedEventArgs e)
        {
            ActivateTool();
        }

        #region CreateConnectionCommand
        class CreateConnectionCommand : Core.ICommand
        {
            Core.Connectable found;
            /// <summary>
            /// the FromShape field
            /// </summary>
            private Core.Connectable mFromShape;
            /// <summary>
            /// Gets or sets the FromShape
            /// </summary>
            public Core.Connectable FromShape
            {
                get { return mFromShape; }
                set { mFromShape = value; }
            }

            /// <summary>
            /// the ToShape field
            /// </summary>
            private Core.Connectable mToShape;
            /// <summary>
            /// Gets or sets the ToShape
            /// </summary>
            public Core.Connectable ToShape
            {
                get { return mToShape; }
                set { mToShape = value; }
            }
            Core.Connection con;

            /// <summary>
            /// the Canvas field
            /// </summary>
            private Canvas mCanvas;
            /// <summary>
            /// Gets or sets the Canvas
            /// </summary>
            public Canvas Canvas
            {
                get { return mCanvas; }
                set { mCanvas = value; }
            }


            public void Undo()
            {
                RemoveConnection();
            }

            public void Redo()
            {
                AddConnection();
            }

            public string Text
            {
                get
                {
                    return "Create connection";
                }
                set
                {
                    throw new Exception("The method or operation is not implemented.");
                }
            }

            #region Constructor
            ///<summary>
            ///Default constructor
            ///</summary>
            public CreateConnectionCommand(Point b, Point e, Canvas canvas)
            {
                if (canvas == null)
                    throw new ArgumentNullException("The canvas cannot be null");
                mCanvas = canvas;
                PointsToShapes(b, e);
            }
            public CreateConnectionCommand(Core.Connectable first, Core.Connectable second, Canvas canvas)
            {
                if (canvas == null)
                    throw new ArgumentNullException("The canvas cannot be null");
                mFromShape = first;
                mToShape = second;
                mCanvas = canvas;
            }
            #endregion

            #region Methods
            private void RemoveConnection()
            {
                mFromShape.Connections.Remove(con);
                mToShape.Connections.Remove(con);
                Canvas.Children.Remove(con);
            }
            private void AddConnection()
            {


                if (mFromShape != null && mToShape != null)
                {
                    //lets connect them
                    con = new DefaultConnection();
                    con.StartPoint = new Point(0, 0);
                    con.EndPoint = new Point(300, 150);
                    con.Stroke = Brushes.DimGray;
                    con.Width = 100;
                    con.Height = 100;
                    con.Fill = Brushes.White;
                    con.StrokeThickness = 1;
                    Canvas.Children.Add(con);

                    mFromShape.BindConnection(con, Core.Connection.StartProperty, Core.ConnectorLocation.Bottom);
                    mToShape.BindConnection(con, Core.Connection.EndProperty, Core.ConnectorLocation.Top);
                    con.InvalidateVisual();
                    Canvas.InvalidateVisual();//really necessary this one??
                }
            }
            private void AddConnection(Core.Connectable first, Core.Connectable second)
            {

                mFromShape = first;
                mToShape = second;
                AddConnection();

            }
            private void AddConnection(Point b, Point e)
            {
                PointsToShapes(b, e);
                AddConnection();
            }

            private void PointsToShapes(Point b, Point e)
            {
                mFromShape = GetConnectable(b);
                mToShape = GetConnectable(e);
            }

            private Core.Connectable GetConnectable(Point p)
            {
                VisualTreeHelper.HitTest(Canvas, new HitTestFilterCallback(ShapesFilter), new HitTestResultCallback(FoundShapes), new PointHitTestParameters(p));
                Core.Connectable elem;
                if (found != null)
                {
                    elem = GetConnectable(found as Visual);
                    if (ShapeFactory.Exists(elem))
                    {
                        return elem;
                    }
                }
                return null;
            }

            private Core.Connectable GetConnectable(Visual element)
            {
                if (element == null) return null;
                return Core.TreeHelper.SingleFindUpInTree(element, new Core.TreeHelper.FinderMatchType(typeof(Core.Connectable))) as Core.Connectable;
            }

            private HitTestResultBehavior FoundShapes(HitTestResult result)
            {
                System.Diagnostics.Debug.Assert(true, "This method never seems to be called. Until now...");
                return HitTestResultBehavior.Continue;
            }

            // Filter the hit test values for each object in the enumeration.
            public HitTestFilterBehavior ShapesFilter(DependencyObject o)
            {
                // Test for the object value you want to filter.
                if (o is Core.IConnectable)
                {
                    // Visual object and descendants are NOT part of hit test results enumeration.
                    //return HitTestFilterBehavior.ContinueSkipSelfAndChildren;
                    found = o as Core.Connectable;
                    return HitTestFilterBehavior.Stop;
                }
                return HitTestFilterBehavior.ContinueSkipSelf;
            }
            #endregion
        }
        #endregion
    }



}
