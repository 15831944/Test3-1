using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Input;
using System.Windows.Controls;
namespace Orbifold.Unfold.Main
{
    /// <summary>
    /// Abstract base class for a tool.
    /// </summary>
    abstract class ToolBase : ITool
    {
        #region Fields
        private Canvas mCanvas;
        /// <summary>
        /// keeps a reference to the previous cursor
        /// </summary>
        private Cursor prevCursor;
        /// <summary>
        /// the suspend state of the tool
        /// </summary>
        private bool mIsSuspended;
        /// <summary>
        /// whether the tool is currently active
        /// </summary>
        private bool mIsActive;
        /// <summary>
        /// the command attached to this tool
        /// </summary>
        protected RoutedUICommand mCommand;
        /// <summary>
        /// the Enabled field
        /// </summary>
        private bool mEnabled = true;

        #endregion

        #region Properties
        /// <summary>
        /// Bit of a hack here, this points to the diagram canvas.
        /// Normally, one would use the View or Controler here.
        /// </summary>
        public  Canvas Canvas
        {
            get { return mCanvas; }
            set { mCanvas = value; }
        }
        /// <summary>
        /// Gets or sets a value indicating whether this tool can activated.
        /// </summary>
        /// <value>
        /// 	<c>true</c> if this instance can activate; otherwise, <c>false</c>.
        /// </value>
        public virtual bool CanActivate
        {
            get
            {
                if (mEnabled)
                {
                    return !IsActive;
                }
                else
                {
                    return false;
                }
            }

        }
        /// <summary>
        /// Gets or sets the Enabled
        /// </summary>
        public bool Enabled
        {
            get
            {
                return this.mEnabled;
            }

            set
            {
                //disable the tool first if it is active
                if (!value && IsActive)
                {
                    DeactivateTool();
                }
                mEnabled = value;
            }
        }
        /// <summary>
        /// Gets or sets a value indicating whether this instance is suspended. A tool enters in a suspended mode when another tool has been activated and disallows another to continue its normal activity. For example, the <see cref="MoveTool"/> and <see cref="SelectionTool"/> are
        /// mutually exclusive and similarly for the drawing tools and the selection tool.
        /// <para>This suspended state is independent of the <see cref="IsActive"/> and the <see cref="Enabled"/> states.</para>
        /// </summary>
        /// <value>
        /// 	<c>true</c> if this instance is suspended; otherwise, <c>false</c>.
        /// </value>
        public bool IsSuspended
        {
            get { return mIsSuspended; }
            set { mIsSuspended = value; }
        }

        /// <summary>
        /// Gets or sets a value indicating whether this tool is active.
        /// </summary>
        /// <value><c>true</c> if this instance is active; otherwise, <c>false</c>.</value>
        public bool IsActive
        {
            get { return mIsActive; }
            set { mIsActive = value; }
        }
        /// <summary>
        /// Gets the routed command.
        /// </summary>
        /// <value>The command.</value>
        public RoutedUICommand Command
        {
            get { return mCommand; }
        }

        /// <summary>
        /// the Name field
        /// </summary>
        private string mName;
        /// <summary>
        /// Gets or sets the Name
        /// </summary>
        public string Name
        {
            get { return mName; }
            set { mName = value; }
        }

        #endregion

        #region Constructor
        ///<summary>
        ///Default constructor
        ///</summary>
        public ToolBase(string name)
        {
            this.mName = name;
            mCommand = new RoutedUICommand();

        }
        #endregion

        #region Methods
        protected void RestoreCursor()
        {
            if (prevCursor != null)
            {
                Mouse.OverrideCursor = prevCursor;                
                prevCursor = null;
            }
        }
        #region Activation & deactivation

        /// <summary>
        /// Deactivates the tool.
        /// </summary>
        /// <returns></returns>
        public bool DeactivateTool()
        {

            if (IsActive)
            {
                //OnDeactivateTool();
                IsActive = false;
                RestoreCursor();
                ToolManager.UnsuspendAllTools();
                return true;
            }
            return false;
        }

        /// <summary>
        /// Activates the tool.
        /// </summary>
        /// <returns></returns>
        public bool ActivateTool()
        {
            //halt other actions
            ToolManager.SuspendOtherTools(this);

            if (Enabled && !IsActive)
            {
                prevCursor = Mouse.OverrideCursor;
                IsActive = true;
                //OnActivateTool();
            }
            return IsActive;
        }
       
       
        #endregion
        /// <summary>
        /// Determines if a command is enabled. Override to provide custom behavior. Do not call the
        /// base version when overriding.
        /// </summary>
        public virtual void OnQueryEnabled(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;
            e.Handled = true;
        }

        /// <summary>
        /// Function to execute the command.
        /// </summary>
        public abstract void OnExecute(object sender, ExecutedRoutedEventArgs e);
        #endregion

    }

}
