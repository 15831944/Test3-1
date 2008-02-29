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
using System.Diagnostics;
using System.Globalization;
using System.Windows.Threading;
using System.Windows.Media.Animation;
using System.ComponentModel;
using System.Windows.Media.Effects;
using Orbifold.Unfold.Core;

namespace Orbifold.Unfold.Core
{
    /// <summary>
    /// Base class for a shape with an expandable part of which the ClassShape is a typical example.
    /// </summary>
    public abstract class ShiftShape : Connectable
    {
        #region Events
        /// <summary>
        /// Occurs after the shape is expanded
        /// </summary>
        public static readonly RoutedEvent ExpandEvent = EventManager.RegisterRoutedEvent("Expand", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(ShiftShape));
        /// <summary>
        /// Occurs after the shape is collapsed
        /// </summary>
        public static readonly RoutedEvent CollapseEvent = EventManager.RegisterRoutedEvent("Collapse", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(ShiftShape));
        /// <summary>
        /// Standard handler code for the <see cref="ExpandEvent"/> event
        /// </summary>
        public event RoutedEventHandler ExpandEventHandler
        {
            add { AddHandler(ExpandEvent, value); }
            remove { RemoveHandler(ExpandEvent, value); }
        }
        #endregion

        #region Fields
        
        #endregion

        #region Properties
        /// <summary>
        /// Whether the shape is in expanded state
        /// </summary>
        public static DependencyProperty IsExpandedProperty = DependencyProperty.Register("IsExpanded", typeof(bool), typeof(ShiftShape));


        /// <summary>
        /// Gets or sets whether the shape is expanded.
        /// When changed this class will automatically raise the <see cref="CollapseEvent"/> or <see cref="ExpandEvent"/>.
        /// </summary>
        public bool IsExpanded
        {
            get { return (bool)GetValue(IsExpandedProperty); }
            set
            {
                SetValue(IsExpandedProperty, value);
                //raise the event
                if (value)
                    RaiseExpandEvent();
                else
                    RaiseCollapseEvent();
            }
        }
        #endregion



        #region Methods


        /// <summary>
        /// Raises the expand event.
        /// </summary>
        void RaiseExpandEvent()
        {
            RoutedEventArgs newEventArgs = new RoutedEventArgs(ExpandEvent);
            RaiseEvent(newEventArgs);
        }
        /// <summary>
        /// Raises the collapse event.
        /// </summary>
        void RaiseCollapseEvent()
        {
            RoutedEventArgs newEventArgs = new RoutedEventArgs(CollapseEvent);
            RaiseEvent(newEventArgs);
        }
              
        #endregion

        #region Shift Command
        
        private static RoutedCommand shiftCommand = new RoutedCommand("ShiftCommand", typeof(ShiftShape));
        protected static void InitializeCommands()
        {
            CommandManager.RegisterClassCommandBinding(typeof(ShiftShape), new CommandBinding(shiftCommand, OnShiftExecute));
        }
        /// <summary>
        /// This code is executed when some element passed the <see cref="ShiftCommand"/>.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        public static void OnShiftExecute(object sender, ExecutedRoutedEventArgs e)
        {
            ShiftShape control = sender as ShiftShape;
            if (control != null)
            {
                
                control.OnShift();
            }
        }
       
        /// <summary>
        /// Gets the routed command to bubble when some element of the template tells to do so.
        /// </summary>
        public static RoutedCommand ShiftCommand
        {
            get
            {
                return shiftCommand;
            }
        }
        /// <summary>
        /// This code converts the state of the shape between expanded/collapsed.
        /// </summary>
        protected virtual void OnShift()
        {         
            IsExpanded = !IsExpanded;
        }
        

      
    
        #endregion
      
    }
}
