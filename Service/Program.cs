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
    static int Main(string[] args)
    {
      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);

      if (args.Length == 3)
      {
        String projectPath;
        String configPath;
        String stencilPath;

        projectPath = args[0].Trim();
        configPath = args[1].Trim();
        stencilPath = args[2].Trim();

        Dictionary<string, Project> projects = new Dictionary<string, Project>();

        {
          Project project = new Project(System.IO.Path.GetFileNameWithoutExtension(System.IO.Path.GetDirectoryName(projectPath)), projectPath);
          projects.Add(project.Name, project);
        }

        {
          Project project = new Project("!!Depreciated Parameter List Used!!", "!!Depreciated Parameter List Used!!");
          projects.Add(project.Name, project);
        }

        Application.Run(new ServiceTemporaryWindow(stencilPath, projects));
      }
      else if (args.Length == 1) // New startup.
      {
        String stencilPath;

        stencilPath = args[0].Trim();

        Properties.Service serviceSettings = Properties.Service.Default;
        if (Properties.Service.Default.projects == null)
        {
          //Properties.Service.Default.projects = new System.Collections.Specialized.StringCollection();

          //{
          //  Project project = new Project("Nickel Copper Leach Demo (Test 1)", @"E:\Kenwalt 07-09-25\Projects\SysCAD 10\Nickel Copper Leach Demo\Nickel Copper Leach Demo.spf\");
          //  Properties.Service.Default.projects.Add(project.Name + '\t' + project.Path);
          //}

          //{
          //  Project project = new Project("Nickel Copper Leach Demo (Test 2)", @"E:\Kenwalt 07-09-25\Projects\SysCAD 10\Nickel Copper Leach Demo\Nickel Copper Leach Demo.spf\");
          //  Properties.Service.Default.projects.Add(project.Name + '\t' + project.Path);
          //}

          //{
          //  Project project = new Project("Nickel Copper Leach Demo (Test 3)", @"E:\Kenwalt 07-09-25\Projects\SysCAD 10\Nickel Copper Leach Demo\Nickel Copper Leach Demo.spf\");
          //  Properties.Service.Default.projects.Add(project.Name + '\t' + project.Path);
          //}

          //Properties.Service.Default.Save();
        }

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
      }
      else
      {
        // Fail to start...
        return -1;
      }

      return 0;
    }
  }
}
