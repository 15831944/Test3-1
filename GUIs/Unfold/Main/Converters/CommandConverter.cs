using System;
using System.Windows;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Documents;
using System.Windows.Controls;
namespace Orbifold.Unfold.Main
{
    /// <summary>
    /// Converts button names of the ribbon to ICommand objects.
    /// </summary>
    public class CommandConverter : IMultiValueConverter
    {
        Button button;

        public Object Convert(Object[] value, Type targetType, Object parameter, System.Globalization.CultureInfo culture)
        {
            String name = value[0] as String;
            ITool tool;
            switch (name)
            {
                case "CreateConnection":
                    button = value[1] as Button;
                    tool = ToolManager.FindTool("Create Connection");
                    if (tool != null)
                    {
                        button.CommandBindings.Add(new CommandBinding(tool.Command, tool.OnExecute, tool.OnQueryEnabled));
                        return tool.Command;
                    }
                    return null;
                case "CreateClassShape":
                    button = value[1] as Button;
                    tool = ToolManager.FindTool("Create ClassShape");
                    if (tool != null)
                    {
                        button.CommandBindings.Add(new CommandBinding(tool.Command, tool.OnExecute, tool.OnQueryEnabled));
                        return tool.Command;
                    }
                    return null;
                case "CreateRectangle":
                    button = value[1] as Button;
                    tool = ToolManager.FindTool("Create rectangle");
                    if(tool!=null)
                    {
                        button.CommandBindings.Add(new CommandBinding(tool.Command, tool.OnExecute, tool.OnQueryEnabled));
                        return tool.Command;
                    }
                    return null;
                    
                case "CreateEllipse":
                    button = value[1] as Button;
                    tool = ToolManager.FindTool("Create ellipse");
                    if (tool != null)
                    {
                        button.CommandBindings.Add(new CommandBinding(tool.Command, tool.OnExecute, tool.OnQueryEnabled));
                        return tool.Command;
                    }
                    return null;
                case "ToggleItalic":
                    button = value[1] as Button;
                    tool = ToolManager.FindTool("Make italic");
                    if (tool != null)
                    {
                        button.CommandBindings.Add(new CommandBinding(tool.Command, tool.OnExecute, tool.OnQueryEnabled));
                        return tool.Command;
                    }
                    return null;
                    // could have used the predefined EditingCommands.ToggleItalic;
                case "ToggleUnderline":
                    button = value[1] as Button;
                    tool = ToolManager.FindTool("Make underline");
                    if (tool != null)
                    {
                        button.CommandBindings.Add(new CommandBinding(tool.Command, tool.OnExecute, tool.OnQueryEnabled));
                        return tool.Command;
                    }
                    return null;
                    //return EditingCommands.ToggleUnderline;
                case "AlignLeft":
                    return EditingCommands.AlignLeft;
                case "AlignCenter":
                    return EditingCommands.AlignCenter;
                case "AlignRight":
                    return EditingCommands.AlignRight;
                case "AlignJustify":
                    return EditingCommands.AlignJustify;
                case "IncreaseIndentation":
                    return EditingCommands.IncreaseIndentation;
                case "DecreaseIndentation":
                    return EditingCommands.DecreaseIndentation;
                case "ToggleBullets":
                    return EditingCommands.ToggleBullets;
                case "ToggleNumbering":
                    return EditingCommands.ToggleNumbering;
                case "Undo":
                    button = value[1] as Button;
                    button.CommandBindings.Add(new CommandBinding(ApplicationCommands.Undo, UndoManager.OnUndoExecute, UndoManager.OnQueryUndoEnabled));
                    return ApplicationCommands.Undo;
                case "Redo":
                    button = value[1] as Button;
                    button.CommandBindings.Add(new CommandBinding(ApplicationCommands.Redo, UndoManager.OnRedoExecute, UndoManager.OnQueryRedoEnabled));
                    return ApplicationCommands.Redo;
                default:
                    return null;
            }
        }

        public Object[] ConvertBack(Object value, Type[] targetType, Object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }

        #region IMultiValueConverter Members


      

        #endregion
    }
}
