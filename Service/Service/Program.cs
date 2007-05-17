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
    static void Main(string[] args)
    {
      String projectFilename;
      String configPath;

      if (args.Length == 2)
      {
        projectFilename = args[0];
        configPath = args[1];
      }
      else
      {
        projectFilename = "C:\\SysCAD91\\Examples\\General Examples\\SS_Nickel\\NiCuDemo-00.spf\\Project.spj";
        configPath = "C:\\Documents and Settings\\pkh\\My Documents\\SysCAD\\BaseFiles\\";
      }

      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);
      Application.Run(new ServiceTemporaryWindow(projectFilename, configPath));
    }
  }
}