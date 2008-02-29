using System;
using System.Collections.Generic;
using System.Text;
using Orbifold.Unfold.ShapeLibrary;
using System.Windows.Media;
using System.Windows;
using System.Windows.Controls;
namespace Orbifold.Unfold.Main
{
    /// <summary>
    /// Assuming that a shape is inheriting from Control this tool looks for a TextBlock with the name
    /// 'BodyText' in the template and sets the TextDecoration collection to Underline. 
    /// </summary>
    class MakeUnderlineTool : ToolBase
    {
        public override void OnExecute(object sender, System.Windows.Input.ExecutedRoutedEventArgs e)
        {           
            if (SelectionManager.SelectedShape != null)
            {
                if (SelectionManager.SelectedShape is Control)
                {
                    Control ctrl = SelectionManager.SelectedShape as Control;
                    ControlTemplate template = ctrl.Template;
                    if (template != null)
                    {
                        TextBlock text = template.FindName("BodyText", ctrl) as TextBlock;
                        if (text != null)
                        {
                            //doesn't make make much sense to accumulate the underline command in the undo stack...
                            if (!text.TextDecorations.Contains(TextDecorations.Underline[0]))
                            {
                                //make it undoable
                                MakeUnderlineCommand cmd = new MakeUnderlineCommand(text);
                                UndoManager.Execute(cmd);
                            }
                        }
                    }
                }
            }
            
        }

        public MakeUnderlineTool()
            : base("Make underline")
        {

        }

        #region UnderlineCommand
        class MakeUnderlineCommand : Core.ICommand
        {
            #region Fields
            private TextBlock mText;
            private TextDecorationCollection deco;
            #endregion

            #region Constructor
            ///<summary>
            ///Default constructor
            ///</summary>
            public MakeUnderlineCommand(TextBlock text)
            {
                mText = text;
            }
            #endregion


            #region ICommand Members

            public void Undo()
            {
                mText.TextDecorations = deco;
            }

            public void Redo()
            {
                deco = mText.TextDecorations;
                mText.TextDecorations = System.Windows.TextDecorations.Underline;
            }

            public string Text
            {
                get
                {
                    return "Underline text";
                }
                set
                {
                    throw new Exception("The method or operation is not implemented.");
                }
            }

            #endregion
        }
        #endregion
    }
    
}
