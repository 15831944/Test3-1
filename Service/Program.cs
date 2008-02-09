using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Configuration;
using System.Security.Permissions;

[assembly: CLSCompliant(true)]
[assembly: SecurityPermission(SecurityAction.RequestMinimum, Execution = true)]
[assembly: UIPermission(SecurityAction.RequestMinimum, Unrestricted = true)]
namespace SysCAD.Service
{
  static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static int Main(string[] args)
    {
      //try
      //{
        Application.EnableVisualStyles();
        Application.SetCompatibleTextRenderingDefault(false);

        String stencilPath = "..\\BaseFiles\\Stencils"; 

        Dictionary<string, Project> projects = new Dictionary<string, Project>();

        if (Properties.Service.Default.projects == null)
        {
          Properties.Service.Default.projects = new System.Collections.Specialized.StringCollection();
        }

      if (args.Length == 2)
        {
        Project project = new Project(args[1], args[0]);
        projects.Add(project.Path, project);
        }
      
      foreach (string projectString in Properties.Service.Default.projects)
        {
          string[] projectStrings = projectString.Split('\t');
          Project project = new Project(projectStrings[0], projectStrings[1]);
          projects.Add(project.Path, project);
        }

        ServiceTemporaryWindow serviceTemporaryWindow = new ServiceTemporaryWindow(stencilPath, projects);
        Application.Run(serviceTemporaryWindow);
        serviceTemporaryWindow.Dispose();

        foreach (Project project in projects.Values)
        {
          Properties.Service.Default.projects.Clear();
          Properties.Service.Default.projects.Add(project.Name + '\t' + project.Path);
        }

        Properties.Service.Default.Save();

        return 0;
      //}
      //catch (Exception e)
      //{
      //  ShowStackTraceBox(e);
      //  return -1;
      //}
    }

    private static void ShowStackTraceBox(Exception e)
    {
      string messagePre = string.Empty;
      string messageBody = string.Empty;
      string messagePost = string.Empty;
      messagePre += "3An error has occurred, please add this information to bugzilla\n";
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

      MessageBox.Show(messagePre + messageBody + messagePost, "4An error has occurred.");
    }
  }
}
