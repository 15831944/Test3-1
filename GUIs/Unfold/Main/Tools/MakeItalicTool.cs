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
    /// 'BodyText' in the template and sets the FontStyle to Italic. 
    /// </summary>
    class MakeItalicTool : ToolBase
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
                            //doesn't make make much sense to accumulate the italic command in the undo stack...
                            if (!text.FontStyle.Equals(FontStyles.Italic))
                            {
                                //make it undoable
                                MakeItalicCommand cmd = new MakeItalicCommand(text);
                                UndoManager.Execute(cmd);
                            }
                        }
                    }
                }
            }
            
        }

        public MakeItalicTool()
            : base("Make italic")
        {

        }

        #region ItalicCommand
        class MakeItalicCommand : Core.ICommand
        {
            #region Fields
            private TextBlock mText;
            private FontStyle style;
            #endregion

            #region Constructor
            ///<summary>
            ///Default constructor
            ///</summary>
            public MakeItalicCommand(TextBlock text)
            {
                mText = text;
            }
            #endregion


            #region ICommand Members

            public void Undo()
            {
                mText.FontStyle = style;
            }

            public void Redo()
            {
                style = mText.FontStyle;
                mText.FontStyle = FontStyles.Italic;
            }

            public string Text
            {
                get
                {
                    return "Italic text";
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
