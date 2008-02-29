using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Input;
using System.Windows;
using System.Windows.Controls;
using Core = Orbifold.Unfold.Core;
namespace Orbifold.Unfold.Main
{
    /// <summary>
    /// This base implementation of a tool serves for shape creation on the basis of
    /// a drawn rubberband on the canvas. Not every shape is created in this way, certain shape have a fixed size
    /// or are polygonal.
    /// </summary>
    abstract class RubberCreationToolBase : ToolBase, Core.IMouseListener
    {
        #region Fields
        private Point initialPoint;
        #endregion

        #region Constructor
        ///<summary>
        ///Default constructor
        ///</summary>
        public RubberCreationToolBase(string toolName) : base(toolName)
        {
            
        }
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
                    double width = Math.Abs(initialPoint.X - currentPoint.X);
                    double height = Math.Abs(initialPoint.Y - currentPoint.Y);
                    double left = Math.Min(initialPoint.X, currentPoint.X);
                    double top = Math.Min(initialPoint.Y, currentPoint.Y);
                    Rect rec = new Rect(left, top, width, height);
                    DrawGhost(rec);
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

                if (Canvas.IsMouseCaptured && !GhostManager.Rectangle.IsEmpty)
                {
                    AddShape(GhostManager.Rectangle);
                }
                DeactivateTool();
                GhostManager.Release();
                return true;
            }
            return false;
        }

        #endregion

        /// <summary>
        /// Override this method in order to display a visual feedback to the mouse drawing action.
        /// </summary>
        /// <param name="rec"></param>
        public abstract void DrawGhost(Rect rec);
        /// <summary>
        /// Override this method in order to add the drawn shape, i.e. the shape corresponding to the mouse action.
        /// </summary>
        /// <param name="rec"></param>
        public abstract void AddShape(Rect rec);

        /// <summary>
        /// Function to execute the command.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public override void OnExecute(object sender, ExecutedRoutedEventArgs e)
        {
            ActivateTool();
        }
        
  
    }
}
