using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace Service
{
  static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static int Main(string[] args)
    {
      String projectPath;
      String configPath;
      String stencilPath;

      if (args.Length == 3)
      {
        projectPath = args[0].Trim();
        configPath = args[1].Trim();
        stencilPath = args[2].Trim();
      }
      else
      {
        // Fail to start...
        return -1;
      }

      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);
      Application.Run(new ServiceTemporaryWindow(projectPath, configPath, stencilPath));

      return 0;
    }
  }
}
