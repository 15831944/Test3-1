
using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace SysCAD.Editor
{
  static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static void Main()
    {
      try
      {
        Application.EnableVisualStyles();
        Application.SetCompatibleTextRenderingDefault(false);
        Application.Run(new EditorForm());
      }
      catch (Exception e)
      {
        ShowStackTraceBox(e);
      }
    }

    private static void ShowStackTraceBox(Exception e)
    {
      string messagePre = string.Empty;
      string messageBody = string.Empty;
      string messagePost = string.Empty;
      messagePre += "An error has occurred, please add this information to bugzilla\n";
      messagePre += "or email a copy of this debug information along with what you\n";
      messagePre += "were doing to paul.hannah@syscad.net:\n\n";
      messageBody += "Exception message:\n" + e.Message + "\n\n";
      if ((e.Data != null) && (e.Data.Count > 0))
      {
        messageBody += "Extra details:\n";
        foreach (System.Collections.DictionaryEntry de in e.Data)
          messageBody += "    The key is '" + de.Key + "' and the value is: " + de.Value + "\n";
        messageBody += "\n";
      }
      if (e.InnerException != null)
      {
        messageBody += "Inner exception message:\n" + e.InnerException.Message + "\n\n";
      if (e.InnerException.Data != null)
      {
        messageBody += "  Extra details:";
        foreach (System.Collections.DictionaryEntry de in e.InnerException.Data)
          messageBody += "    The key is '" + de.Key + "' and the value is: " + de.Value + "\n";
        messageBody += "\n";
      }
      }
      messageBody += "Stack trace:\n" + e.StackTrace;
      messagePost += "\n\n\n(A copy of this debug information has already been pasted into the clipboard.)";

      Clipboard.SetText(messageBody);

      MessageBox.Show(messagePre + messageBody + messagePost, "An error has occurred.");
    }
  }
}
