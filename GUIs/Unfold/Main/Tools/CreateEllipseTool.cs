using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Input;
using System.Windows;
using System.Windows.Controls;
namespace Orbifold.Unfold.Main
{
    class CreateEllipseTool : RubberCreationToolBase
    {

        #region Constructor
        ///<summary>
        ///Default constructor
        ///</summary>
        public CreateEllipseTool()
            : base("Create ellipse")
        {

        }
        #endregion

        #region Methods
        /// <summary>
        /// Draws a rectangular ghost on the canvas.
        /// </summary>
        /// <param name="rec"></param>
        public override void DrawGhost(Rect rec)
        {
            GhostManager.DrawEllipticGhost(rec);
        }

        public override void AddShape(Rect rec)
        {
            ShapeFactory.AddShape(rec, ShapeTypes.Ellipse);
        }
        #endregion

    }

}
