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

      if (args.Length == 2)
      {
        projectPath = args[0];
        configPath = args[1];
      }
      else
      {
        projectPath = "C:\\SysCAD91\\Examples\\General Examples\\SS_Nickel\\NiCuDemo-00.spf\\";
        configPath = "C:\\Documents and Settings\\pkh\\My Documents\\SysCAD\\BaseFiles\\";
        // return -1; // Die gracefully when started with incorrect number of parameters.
        // TODO: Handle other failures gracefully (e.g. no *.10 files, no config files.)
      }

      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);
      Application.Run(new ServiceTemporaryWindow(projectPath, configPath));

      return 0;
    }
  }
}