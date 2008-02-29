using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Controls;
using System.Windows.Input;
using System.Diagnostics;
namespace Orbifold.Unfold.Main
{
    /// <summary>
    /// The toolbox, this tells the application which tools are available.
    /// </summary>
    static class ToolManager
    {
        #region Fields
        static List<ITool> toolList;
        static bool eventsEnabled = true;
        static Canvas mCanvas;
        #endregion

        #region Properties
        public static Canvas Canvas
        {
            get { return mCanvas; }
            set
            {
                Debug.Assert(value != null, "ToolManager cannot work with a null canvas.");
                mCanvas = value;
                toolList.ForEach(
                    delegate(ITool tool)
                    {
                        tool.Canvas = value;
                    }
                    );
            }
        }
        #endregion

        #region Constructor
        ///<summary>
        ///Default constructor
        ///</summary>
        static ToolManager()
        {
            //I think certain tools should be loaded dynamically the first time they are needed
            //Loading all tools at startup is a bit expensive I think but for the moment this is the easiest way to go.

            toolList = new List<ITool>();

            toolList.Add(new MoveTool());//has to precede all other tools!

            toolList.Add(new CreateRectangleTool());
            
            toolList.Add(new CreateEllipseTool());

            toolList.Add(new CreateClassShapeTool());
            
            toolList.Add(new MakeItalicTool());

            toolList.Add(new MakeUnderlineTool());

            toolList.Add(new CreateConnectionTool());

        }
        #endregion

        #region Methods
        /// <summary>
        /// Returns the tool with the specified name, if it exists.
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        public static ITool FindTool(string name)
        {
            return toolList.Find(delegate(ITool tool)
            {
                return tool.Name == name;

            });
        }
        #endregion

        #region Methods
        #region MouseHandlers
        public static void MouseDown(MouseEventArgs e)
        {

            toolList.ForEach(
                delegate(ITool tool)
                {
                    if (tool is Core.IMouseListener)
                        if ((tool as Core.IMouseListener).MouseDown(e))
                            return;
                }
                );
        }
        public static void MouseMove(MouseEventArgs e)
        {
            toolList.ForEach(
                delegate(ITool tool)
                {
                    if (tool is Core.IMouseListener)
                        if ((tool as Core.IMouseListener).MouseMove(e))
                            return;
                }
                );
        }
        public static void MouseUp(MouseEventArgs e)
        {
            toolList.ForEach(
                delegate(ITool tool)
                {
                    if (tool is Core.IMouseListener)
                        if ((tool as Core.IMouseListener).MouseUp(e))
                            return;
                }
                );
        }
        #endregion
        /// <summary>
        /// Suspends the other tools.
        /// </summary>
        public static void SuspendOtherTools(ITool tool)
        {
            toolList.ForEach(
               delegate(ITool t)
               {
                   if (!t.Equals(tool))
                       t.IsSuspended = true;
               }
           );
        }
        #region Tool (de)activation methods
        /// <summary>
        /// Deactivates the given tool
        /// </summary>
        /// <param name="tool">a registered ITool</param>
        /// <returns></returns>
        public static bool DeactivateTool(ITool tool)
        {
            bool flag = false;
            if (tool != null && tool.Enabled && tool.IsActive)
            {
                flag = tool.DeactivateTool();
                //if (flag && eventsEnabled)
                //{
                //    RaiseOnToolDeactivate(new ToolEventArgs(tool));
                //}
            }
            return flag;
        }
        /// <summary>
        /// Activates the tool with the given name
        /// </summary>
        /// <param name="toolName"></param>
        /// <returns></returns>
        public static void ActivateTool(string toolName)
        {
            //if (!controllerEnabled)
            //    return;

            ITool foundTool = FindTool(toolName);

            ActivateTool(foundTool);
        }

        /// <summary>
        /// Suspends all tools
        /// </summary>
        public static void SuspendAllTools()
        {
            toolList.ForEach(
                delegate(ITool tool)
                {
                    tool.IsSuspended = true;
                }
            );
        }
        /// <summary>
        /// Unsuspends all tools.
        /// </summary>
        public static void UnsuspendAllTools()
        {
            toolList.ForEach(
                 delegate(ITool tool)
                 {
                     tool.IsSuspended = false;
                 }
             );
        }

        /// <summary>
        /// Activates a registered tool
        /// </summary>
        /// <param name="tool">a registered ITool</param>
        /// <returns></returns>
        private static bool ActivateTool(ITool tool)
        {
            //if (!controllerEnabled)
            //    return false;
            bool flag = false;
            if (tool != null && tool.CanActivate)
            {
                flag = tool.ActivateTool();
                //if (flag && eventsEnabled)
                //{
                //    RaiseOnToolActivate(new ToolEventArgs(tool));
                //}
            }
            return flag;
        }
        #endregion

        #endregion
    }


}
