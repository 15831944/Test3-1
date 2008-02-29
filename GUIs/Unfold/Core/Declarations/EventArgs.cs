using System;
using System.Collections.Generic;
using System.Text;
using Orbifold.Unfold.Core;
namespace Orbifold.Unfold.Core
{
    /// <summary>
    /// Passes Connectable info through events.
    /// </summary>
    public class ShapeEventArgs : EventArgs
    {


        /// <summary>
        /// the Shape field
        /// </summary>
        private Connectable mShape;
        /// <summary>
        /// Gets or sets the Shape
        /// </summary>
        public Connectable Shape
        {
            get { return mShape; }
            set { mShape = value; }
        }



        #region Constructor
        ///<summary>
        ///Default constructor
        ///</summary>
        public ShapeEventArgs(Connectable shape)
        {
            this.mShape = shape;
        }
        #endregion
  
    }
}
