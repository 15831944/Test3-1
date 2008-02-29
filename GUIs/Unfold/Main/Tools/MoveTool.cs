using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Input;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;
using Orbifold.Unfold.Core;
using Orbifold.Unfold.ShapeLibrary;
namespace Orbifold.Unfold.Main
{
    /// <summary>
    /// Tool that implemts the translation of selected shapes on the canvas
    /// </summary>
    class MoveTool : ToolBase, IMouseListener
    {
        #region Fields
        private Point currentPoint;
        bool moving = false;
        /// <summary>
        /// relative shift inside the shape between the left-up corner 
        /// and the point where the user clicked
        /// </summary>
        Point shift;
        Point initialPoint;
        Connectable movedShape;
        #endregion

        #region Constructor
        ///<summary>
        ///Default constructor
        ///</summary>
        public MoveTool() : base("Move tool")
        {
            
        }
        #endregion

        #region Methods

        public override void OnExecute(object sender, ExecutedRoutedEventArgs e)
        {
            //MessageBox.Show("The execution of the create rectangle was called.");
            ActivateTool();
        }

        #endregion

        private Connectable GetConnectable(Visual element)
        {
            if (element == null) return null;
            return TreeHelper.SingleFindUpInTree(element, new TreeHelper.FinderMatchType(typeof(Connectable))) as Connectable;
        }

        #region IMouseListener Members

        public bool MouseDown(MouseEventArgs e)
        {
            if (IsSuspended) return false;
            Connectable elem = null;
            
             HitTestResult result = VisualTreeHelper.HitTest(Canvas, e.GetPosition(Canvas));
             if (result.VisualHit != null)
             {
                
                 elem = GetConnectable(result.VisualHit as Visual);
                 if (ShapeFactory.Exists(elem))
                 {
                     SelectionManager.SelectShape(elem);
                     movedShape = SelectionManager.SelectedShape as Connectable;
                     moving = true;
                     shift = e.GetPosition(movedShape);
                     initialPoint = new Point(Canvas.GetLeft(movedShape), Canvas.GetTop(movedShape));
                     ActivateTool();
                     return true;
                 }
             }
             return false;
        }

        public bool MouseMove(MouseEventArgs e)
        {
            if (IsActive)
            {
                if (moving)
                {
                    currentPoint = e.GetPosition(Canvas);
                    if ((currentPoint.X - shift.X) <= 0 || (currentPoint.X - shift.X)>= (Canvas.ActualWidth - movedShape.ActualWidth)) return true;
                    if ((currentPoint.Y - shift.Y) <= 0 || (currentPoint.Y - shift.Y) >= (Canvas.ActualHeight -  movedShape.ActualHeight)) return true; 
                    Canvas.SetLeft(movedShape, currentPoint.X - shift.X);
                    Canvas.SetTop(movedShape, currentPoint.Y - shift.Y);
                    //HACK: don't know why the binding does not refresh the visual...
                    if (movedShape is Connectable)
                    {
                        
                        (movedShape as Connectable).RefreshConnections();
                    }
                }
                return true;
            }
            return false;
        }

        public bool MouseUp(MouseEventArgs e)
        {
            if (IsActive)
            {               
                moving = false;
                //the undoredo trick, but watch out not to execute it since the MouseMove was the execution!
                MoveCommand cmd = new MoveCommand(movedShape, Point.Subtract(currentPoint, initialPoint));
                UndoManager.UndoStack.Push(cmd);
                UndoManager.UndoStackInfo.Insert(0,cmd.Text);
                DeactivateTool();
                return true;
                
            }
            return false;
        }

        #endregion

        #region MoveCommand
        class MoveCommand : Core.ICommand
        {
            private Vector mDelta;

            public Vector Delta
            {
                get { return mDelta; }
                set { mDelta = value; }
            }
            private UIElement mShape;

            public UIElement Shape
            {
                get { return mShape; }
                set { mShape = value; }
            }


            #region Constructor
            public MoveCommand(UIElement shape, Vector delta)
            {
                mDelta = delta;
                mShape = shape;
            }
            #endregion


            public void Undo()
            {
                Canvas.SetLeft(mShape, Canvas.GetLeft(mShape) - Delta.X);
                Canvas.SetTop(mShape, Canvas.GetTop(mShape) - Delta.Y);
            }

            public void Redo()
            {
                Canvas.SetLeft(mShape, Canvas.GetLeft(mShape)+ Delta.X);
                Canvas.SetTop(mShape, Canvas.GetTop(mShape)+ Delta.Y);
            }

            public string Text
            {
                get
                {
                    return "Shape moved";
                }
                set
                {
                    throw new Exception("The method or operation is not implemented.");
                }
            }

        }
        #endregion
    }

}
