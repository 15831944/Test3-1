using System;
using System.Xml;
using System.Collections.Generic;
using System.Text;

namespace SVNAutomate
{
  class Program
  {
    static int RunSVN(string argument, out string output)
    {
      System.Diagnostics.Process proc = new System.Diagnostics.Process();
      proc.EnableRaisingEvents = false;
      proc.StartInfo.UseShellExecute = false;
      proc.StartInfo.RedirectStandardOutput = true;
      proc.StartInfo.RedirectStandardError = true;
      proc.StartInfo.CreateNoWindow = true;

      if (proc.StartInfo.EnvironmentVariables.ContainsKey("SVN_SSH"))
      {
        proc.StartInfo.EnvironmentVariables.Remove("SVN_SSH");
      }
      proc.StartInfo.EnvironmentVariables.Add("SVN_SSH", @"C:\\Program Files\\TortoiseSVN\\bin\\TortoisePlink.exe");

      proc.StartInfo.FileName = "svn";
      proc.StartInfo.Arguments = argument;
      proc.Start();

      output = proc.StandardOutput.ReadToEnd();
      string error = proc.StandardError.ReadToEnd();

      if (error.Length > 0)
      {
        Console.Write(error);
        Console.ReadKey(false);
      }
     
      return proc.ExitCode;
    }

    static int Main(string[] args)
    {
      string update, info, log;

      int exitCode;

      exitCode = RunSVN("update --non-interactive", out update);
      if (exitCode != 0)
        return exitCode;

      Console.Write(update);
      Console.ReadKey(true);

      exitCode = RunSVN("info  --non-interactive --xml", out info);
      if (exitCode != 0)
        return exitCode;

      XmlDocument xmlInfo = new XmlDocument();
      xmlInfo.LoadXml(info);

      Console.Write(info);
      Console.ReadKey(true);

      exitCode = RunSVN("log  --non-interactive --xml --limit 10", out log);
      if (exitCode != 0)
        return exitCode;

      XmlDocument xmlLog = new XmlDocument();
      xmlLog.LoadXml(log);

      Console.Write(log);
      Console.ReadKey(true);

      return 0;
    }
  }
}
