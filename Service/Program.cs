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

      if (args.Length == 2)
      {
        projectPath = args[0];
        configPath = args[1];
        //stencilPath = args[2]; /;/ hard-code this until we work out where to get the value.
      }
      else
      {
        projectPath = "C:\\SysCAD91\\Examples\\General Examples\\SS_Nickel\\NiCuDemo-00.spf\\";
        configPath = "C:\\SysCAD91\\Examples\\General Examples\\SS_Nickel\\CfgFiles\\";
      }

      stencilPath = "C:\\Documents and Settings\\pkh\\My Documents\\SysCAD\\BaseFiles\\";

      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);
      Application.Run(new ServiceTemporaryWindow(projectPath, configPath, stencilPath));

      return 0;
    }
  }
}