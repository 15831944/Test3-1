using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Net.Mail;
using System.Text.RegularExpressions;

namespace NightlyBuild
{
  class Program
  {
    static public void SendGMail(string username, string password, string to, string subject, string body, string attachmentPath)
    {
      System.Net.Mail.MailMessage msg = new System.Net.Mail.MailMessage(username, to, subject, body);

      SmtpClient client = new SmtpClient();
      client.Credentials = new System.Net.NetworkCredential(username, password);
      client.Port = 587;//or use 587            

      client.Host = "smtp.gmail.com";
      client.EnableSsl = true;

      msg.IsBodyHtml = true;
      msg.Attachments.Add(new Attachment(attachmentPath));
      //client.SendCompleted += new SendCompletedEventHandler(client_SendCompleted);
      //object userState = msg;
      try
      {
        //you can also call client.Send(msg)

        client.Send(msg);
      }
      catch (System.Net.Mail.SmtpException)
      {
        Console.Out.WriteLine();
        Console.Out.WriteLine("!!!!!!!!!! Send Mail Error");
        Console.Out.WriteLine();
      }
    }

    static void Main(string[] args)
    {
      string rootPath = Directory.GetCurrentDirectory();
      if (args.Count() == 1)
        rootPath = args[0];

      string logPath = rootPath + @"\log";
      string batPath = rootPath + @"\bat";

      string[] gmailSecrets = new string[0];
      try
      {
        gmailSecrets = File.OpenText("gmail.secrets").ReadLine().Split('|');
        if (gmailSecrets.Count() != 2) throw new DataMisalignedException();
      }
      catch (Exception)
      {
        Console.Out.WriteLine("Error reading gmail.secrets file (format is: Gmail.Account@Address|password)");
        return;
      }

      try
      {
        //foreach (string txtFile in Directory.GetFiles(Directory.GetCurrentDirectory() + "\log", "*.txt"))
        //{
        //  File.Delete(txtFile);
        //}
        if (Directory.Exists(logPath))
          Directory.Delete(logPath, true);

        Directory.CreateDirectory(logPath);
      }
      catch (Exception)
      {
        Console.Out.WriteLine("Error deleting *.txt files (is there another instance running?)");
        return;
      }

      try
      {
        //foreach (string batFile in Directory.GetFiles(Directory.GetCurrentDirectory() + "\bat", "*.bat"))
        //{
        //  File.Delete(batFile);
        //}
        if (Directory.Exists(batPath))
          Directory.Delete(batPath, true);

        Directory.CreateDirectory(batPath);
      }
      catch (Exception)
      {
        Console.Out.WriteLine("Error deleting *.bat files (is there another instance running?)");
        return;
      }


      List<string> slnList = new List<string>();
      SlnFiles(rootPath, ref slnList);

      int i = 0;
      foreach (string slnFile in slnList)
      {
        string argPathSafe = slnFile.Remove(0, rootPath.Length + 1).Replace('\\', '-');
        string sln = System.IO.File.OpenText(slnFile).ReadToEnd().Replace('\n', '\t').Replace('\r', '\t');
        Regex versionRegex = new Regex(@"Microsoft Visual Studio Solution File, Format Version 10.00");
        Regex sectionRegex = new Regex(@"\tGlobalSection\(SolutionConfigurationPlatforms\) = preSolution\t(?<configurations>.*)\tEndGlobalSection");
        if ((versionRegex.IsMatch(sln)) && (sectionRegex.IsMatch(sln)))
        {
          string configurations = sectionRegex.Match(sln).Groups["configurations"].Value;
          Regex configurationRegex = new Regex(@"\t+(?<configuration>[^=]*)");
          Match configurationMatch = configurationRegex.Match(configurations);
          while (configurationMatch.Success)
          {
            string configuration = configurationMatch.Groups["configuration"].Value.TrimEnd(' ').Replace("|", " -- ");
            if ((configuration.Length > 0) && (!configuration.Contains("HideSolutionNode")) && (!configuration.Contains("GlobalSection")) && (!configuration.Contains("{")))
            {
              i++;
              StreamWriter batStream = new StreamWriter(batPath + @"\" + i.ToString("000") + " - " + argPathSafe + " -- " + configuration + ".bat");
              batStream.WriteLine(@"@echo off");
              batStream.WriteLine(@"Mode Con: Cols=120 Lines=1");
              batStream.WriteLine(@"color 8f");
              batStream.WriteLine(@"TITLE NightlyBuild: " + slnFile + " -- " + configuration);
              batStream.WriteLine(@"call ""C:\Program Files\Microsoft Visual Studio 9.0\Common7\Tools\vsvars32.bat""");
              batStream.WriteLine();
              batStream.WriteLine(@"devenv """ + slnFile + @""" /Clean");
              batStream.WriteLine(@"devenv """ + slnFile + @""" /Rebuild """ + configuration.Replace(" -- ", "|") + @""" /Out ""log\" + i.ToString("000") + " - " + argPathSafe + @" -- " + configuration + @".txt""");
              batStream.Close();
            }
            configurationMatch = configurationRegex.Match(configurations, configurationMatch.Groups["configuration"].Index);
          }
        }
      }

      foreach (string batFile in Directory.GetFiles(batPath, "*.bat"))
      {
        Console.Out.WriteLine("Starting:  " + batFile);

        // populate process environment
        System.Diagnostics.ProcessStartInfo psi = new System.Diagnostics.ProcessStartInfo();

        psi.FileName = batFile;
        psi.ErrorDialog = true;
        psi.CreateNoWindow = true;

        // start process
        System.Diagnostics.Process p = System.Diagnostics.Process.Start(psi);

        p.PriorityClass = System.Diagnostics.ProcessPriorityClass.Idle;
        // instruct process to wait for exit
        p.WaitForExit();

        // free process resources
        p.Close();


        Console.Out.WriteLine("Parsing:   " + batFile);

        bool sendEmail = false;

        string output = System.IO.File.OpenText(logPath + @"\" + Path.GetFileNameWithoutExtension(batFile) + ".txt").ReadToEnd();



        {
          Regex compileCompleteRegex = new Regex(@"Compile complete -- (?<errors>[0123456789]+) errors, (?<warnings>[0123456789]+) warnings");
          Match compileCompleteMatch = compileCompleteRegex.Match(output);
          while (compileCompleteMatch.Success)
          {
            int warnings = Convert.ToInt32(compileCompleteMatch.Groups["warnings"].Value);
            int errors = Convert.ToInt32(compileCompleteMatch.Groups["errors"].Value);
            if ((warnings > 0) || (errors > 0))
            {
              sendEmail = true;
              Console.Out.WriteLine("-  " + compileCompleteMatch.Value);
            }

            compileCompleteMatch = compileCompleteRegex.Match(output, compileCompleteMatch.Index + 1);
          }
        }

        {
          Regex compileCompleteRegex = new Regex(@"[^-]* - (?<errors>[0123456789]+) error(s), (?<warnings>[0123456789]+) warning(s)");
          Match compileCompleteMatch = compileCompleteRegex.Match(output);
          while (compileCompleteMatch.Success)
          {
            int warnings = Convert.ToInt32(compileCompleteMatch.Groups["warnings"].Value);
            int errors = Convert.ToInt32(compileCompleteMatch.Groups["errors"].Value);
            if ((warnings > 0) || (errors > 0))
            {
              sendEmail = true;
              Console.Out.WriteLine("-  " + compileCompleteMatch.Value);
            }

            compileCompleteMatch = compileCompleteRegex.Match(output, compileCompleteMatch.Index + 1);
          }
        }


        string body = "<H1>Result:</H1>";
        Regex rebuildAllRegex = new Regex(@"========== Rebuild All: [0123456789]+ succeeded, (?<failed>[0123456789]+) failed, [0123456789]+ skipped ==========");
        if ((rebuildAllRegex.IsMatch(output)) && (rebuildAllRegex.Match(output).Groups["failed"].Success))
        {
          int failed = Convert.ToInt32(rebuildAllRegex.Match(output).Groups["failed"].Value);
          if (failed > 0)
          {
            sendEmail = true;
            body += "<P>" + rebuildAllRegex.Match(output).Value + "</P>";
            Console.Out.WriteLine("-  " + rebuildAllRegex.Match(output).Value);
          }
        }
        else
          sendEmail = true;



        body += "<H2>Error:</H2><UL>";
        Regex errorRegex = new Regex(@"[^:\r\n]*: error[^\r\n]*");
        Match errorMatch = errorRegex.Match(output);
        while (errorMatch.Success)
        {
          body += "<LI>" + errorMatch.Value;
          errorMatch = errorRegex.Match(output, errorMatch.Index + errorMatch.Length);
        }
        body += "</UL>";

        body += "<H2>Warning:</H2><UL>";
        Regex warningRegex = new Regex(@"[^:\r\n]*: warning[^\r\n]*");
        Match warningMatch = warningRegex.Match(output);
        while (warningMatch.Success)
        {
          body += "<LI>" + warningMatch.Value;
          warningMatch = warningRegex.Match(output, warningMatch.Index + warningMatch.Length);
        }
        body += "</UL>";

        body += "<H2>Check:</H2><UL>";
        Regex checkRegex = new Regex(@"[^:\r\n]*: check[^\r\n]*");
        Match checkMatch = checkRegex.Match(output);
        while (checkMatch.Success)
        {
          body += "<LI>" + checkMatch.Value;
          checkMatch = checkRegex.Match(output, checkMatch.Index + checkMatch.Length);
        }
        body += "</UL>";

        body += "<H2>Note:</H2><UL>";
        Regex noteRegex = new Regex(@"[^:\r\n]*: note[^\r\n]*");
        Match noteMatch = noteRegex.Match(output);
        while (noteMatch.Success)
        {
          body += "<LI>" + noteMatch.Value;
          noteMatch = noteRegex.Match(output, noteMatch.Index + noteMatch.Length);
        }
        body += "</UL>";




        if (sendEmail)
        {
          Console.Out.WriteLine("Emailing:  " + batFile);
          SendGMail(gmailSecrets[0], gmailSecrets[1], "nightlybuild@syscad.net", "NightlyBuild Fault: " + Path.GetFileNameWithoutExtension(batFile), body, @"log\" + Path.GetFileNameWithoutExtension(batFile) + ".txt");
        }

        Console.Out.WriteLine("Finishing: " + batFile);
        Console.Out.WriteLine();
      }
    }

    private static void SlnFiles(string rootPath, ref List<string> slnList)
    {
      foreach (string slnFile in Directory.GetFiles(rootPath, "*.sln"))
      {
        slnList.Add(slnFile);
      }
      foreach (string subFolder in Directory.GetDirectories(rootPath))
      {
        SlnFiles(subFolder, ref slnList);
      }
    }
  }
}
