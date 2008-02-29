using System;
using System.Collections.Generic;
using System.Text;
using System.Collections.ObjectModel;
using System.Windows.Input;
namespace Orbifold.Unfold.Main
{
    /// <summary>
    /// Manages the undo/redo stack. Can be refined, this is a basic implementation.
    /// <remarks>The ICommand name is unfortunate and already in use in the System.Windows.Input namespace.</remarks>
    /// </summary>
    public static class UndoManager
    {
        #region Fields
        /// <summary>
        /// the undo stack
        /// </summary>
        private static Stack<Core.ICommand> undoStack = new Stack<Core.ICommand>();

        /// <summary>
        /// the redo stack
        /// </summary>
        private static Stack<Core.ICommand> redoStack = new Stack<Core.ICommand>();
        /// <summary>
        /// the <c>ObservableCollection</c> of undo's you can bind to some UIElement
        /// </summary>
        private static ObservableCollection<string> undoStackNames = new ObservableCollection<string>();
        /// <summary>
        /// the <c>ObservableCollection</c> of redo's you can bind to some UIElement
        /// </summary>
        private static ObservableCollection<string> redoStackNames = new ObservableCollection<string>();
        #endregion

        #region Properties
        /// <summary>
        /// Gets the undo stack.
        /// </summary>
        /// <value>The undo stack.</value>
        internal static Stack<Core.ICommand> UndoStack
        {
            get { return UndoManager.undoStack; }
            //set { UndoManager.undoStack = value; }
        }
        /// <summary>
        /// Gets the redo stack.
        /// </summary>
        /// <value>The redo stack.</value>
        internal static Stack<Core.ICommand> RedoStack
        {
            get { return UndoManager.redoStack; }
            //set { UndoManager.redoStack = value; }
        }
        /// <summary>
        /// Gets a value indicating whether there is anything that can be undone.
        /// </summary>
        /// <value><c>true</c> if this instance can undo; otherwise, <c>false</c>.</value>
        public static bool CanUndo
        {
            get { return undoStack.Count > 0; }
        }

        /// <summary>
        /// Gets a value indicating whether there is anything that can be rolled forward
        /// </summary>
        /// <value><c>true</c> if this instance can redo; otherwise, <c>false</c>.</value>
        public static bool CanRedo
        {
            get { return redoStack.Count > 0; }
        }

        /// <summary>
        /// Gets the undo stack info.
        /// </summary>
        /// <value>The undo stack info.</value>
        public static ObservableCollection<string> UndoStackInfo
        {
            get
            {
                return undoStackNames;
            }
        }

        /// <summary>
        /// Gets the redo stack info.
        /// </summary>
        /// <value>The redo stack info.</value>
        public static ObservableCollection<string> RedoStackInfo
        {
            get
            {
                return redoStackNames;
            }
        }
        #endregion

        #region Constructor
        static UndoManager()
        {

        }
        #endregion

        #region Methods

       

        #region UndoRedo command binding methods

        public static void OnUndoExecute(object sender, ExecutedRoutedEventArgs e)
        {
            Undo();
        }
        public static void OnQueryUndoEnabled(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = (undoStack.Count > 0);
        }
        public static void OnRedoExecute(object sender, ExecutedRoutedEventArgs e)
        {
            Redo();
        }
        public static void OnQueryRedoEnabled(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = (redoStack.Count > 0);
        }
        #endregion
        
        /// <summary>
        /// All-in-one method to add a command to the stack.
        /// The method will be executed after it's been added to the stack.
        /// </summary>
        /// <param name="cmd">The command record.</param>
        internal static void Execute(Core.ICommand cmd)
        {
            cmd.Redo();
            if (cmd is Core.ICommand)
            {
                undoStack.Push(cmd);
                undoStackNames.Insert(0, cmd.Text);
            }
        }

        /// <summary>
        /// If there is anything in the undo stack the top is popped and undone.
        /// </summary>
        internal static void Undo()
        {
            if (undoStack.Count > 0)
            {
                Core.ICommand cmd = undoStack.Pop();
                cmd.Undo();
                undoStackNames.RemoveAt(0);
                redoStack.Push(cmd);
                redoStackNames.Insert(0, cmd.Text);
            }
        }

        /// <summary>
        /// If there is anything in the redo stack the top is popped and executed
        /// </summary>
        internal static void Redo()
        {
            if (redoStack.Count > 0)
            {
                Core.ICommand commandrecord = redoStack.Pop();
                Execute(commandrecord);
                redoStackNames.RemoveAt(0);
            }
        }
        #endregion


      
    }

}
