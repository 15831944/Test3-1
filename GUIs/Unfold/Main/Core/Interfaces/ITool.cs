using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Input;
using System.Windows.Controls;
namespace Orbifold.Unfold.Main
{
    interface ITool
    {

        Canvas Canvas { get;set;}
        /// <summary>
        /// Gets or sets a value indicating whether this tool is active. If true the tool is actually performing work via the various mouse or keyboard event handlers.
        /// If <see cref="Enabled"/> is false the tool can never be active. Furthermore, if the tool <see cref="IsSuspended"/> it means another tool has suspended the activity of this tool.
        /// 
        /// </summary>
        /// <value><c>true</c> if this instance is active; otherwise, <c>false</c>.</value>
        bool IsActive { get; set;}

        /// <summary>
        /// Gets or sets a value indicating whether this tool can activated.
        /// </summary>
        /// <value>
        /// 	<c>true</c> if this instance can activate; otherwise, <c>false</c>.
        /// </value>
        bool CanActivate { get; }
        /// <summary>
        /// Gets or sets a value indicating whether this <see cref="T:ITool"/> is enabled.
        /// </summary>
        /// <value><c>true</c> if enabled; otherwise, <c>false</c>.</value>
        bool Enabled { get; set; }
        // <summary>
        /// Gets or sets a value indicating whether this instance is suspended. A tool enters in a suspended mode when another tool has been activated and disallows another to continue its normal activity. For example, the <see cref="MoveTool"/> and <see cref="SelectionTool"/> are 
        /// mutually exclusive and similarly for the drawing tools and the selection tool. 
        /// <para>This suspended state is independent of the <see cref="IsActive"/> and the <see cref="Enabled"/> states.</para>
        /// </summary>
        /// <value>
        /// 	<c>true</c> if this instance is suspended; otherwise, <c>false</c>.
        /// </value>
        bool IsSuspended { get; set;}
        RoutedUICommand Command { get;}
        string Name { get;}
        /// <summary>
        /// Called when the UICommand is refreshed.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="System.Windows.Input.CanExecuteRoutedEventArgs"/> instance containing the event data.</param>
        void OnQueryEnabled(object sender, CanExecuteRoutedEventArgs e);
        /// <summary>
        /// Starts the tool
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void OnExecute(object sender, ExecutedRoutedEventArgs e);
        /// <summary>
        /// Deactivates the tool.
        /// </summary>
        /// <returns></returns>
        bool DeactivateTool();

        /// <summary>
        /// Activates the tool.
        /// </summary>
        /// <returns></returns>
        bool ActivateTool();
    }
}
