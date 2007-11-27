using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Configuration;

namespace SysCAD.Service
{
  static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static int Main()
    {
      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);

      String stencilPath = "..\\BaseFiles\\Stencils";

      Dictionary<string, Project>  projects = new Dictionary<string, Project>();

      if (Properties.Service.Default.projects == null)
      {
        Properties.Service.Default.projects = new System.Collections.Specialized.StringCollection();
      }

      foreach (string projectString in Properties.Service.Default.projects)
      {
        string[] projectStrings = projectString.Split('\t');
        Project project = new Project(projectStrings[0], projectStrings[1]);
        projects.Add(project.Path, project);
      }
        
      Application.Run(new ServiceTemporaryWindow(stencilPath, projects));

      foreach (Project project in projects.Values)
      {
        Properties.Service.Default.projects.Clear();
        Properties.Service.Default.projects.Add(project.Name + '\t' + project.Path);
      }

      Properties.Service.Default.Save();

      return 0;
    }
  }
}
