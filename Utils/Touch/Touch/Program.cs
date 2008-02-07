using System;
using System.Linq;
using System.IO;
using System.Threading;

namespace Touch
{
  class Program
  {

    static void Main(string[] args)
    {
      if (args.Count() == 1)
      {
        if (new DirectoryInfo(args[0]).Exists)
        {
          Touch(new DirectoryInfo(args[0]), DateTime.Now);
        }
        else
        {
          Console.WriteLine("Directory " + args[0] + " doesn't exist.");
          Console.ReadKey();
        }
      }
      else
      {
        Console.WriteLine("Usage: touch path");
        Console.ReadKey();
      }
    }

    private static void Touch(DirectoryInfo directoryInfo, DateTime now)
    {
      try
      {
        foreach (DirectoryInfo subDirectoryInfo in directoryInfo.GetDirectories())
        {
          //new Thread(delegate() { DoTouch(subDirectoryInfo, now); }).Start();
          DoTouch(subDirectoryInfo, now);
          Touch(subDirectoryInfo, now);
        }
        foreach (FileInfo fileInfo in directoryInfo.GetFiles())
        {
          new Thread(delegate() { DoTouch(fileInfo, now); }).Start();
          //DoTouch(fileInfo, now);
        }
      }
      catch (System.Exception e)
      {
        Console.WriteLine(e.Message);
      }
    }

    private static void DoTouch(FileSystemInfo fileSystemInfo, DateTime now)
    {
      FileAttributes fileAttributes = fileSystemInfo.Attributes;

      try
      {
        fileSystemInfo.Attributes = FileAttributes.Normal;
        fileSystemInfo.CreationTime = now;
        fileSystemInfo.LastWriteTime = now;
        fileSystemInfo.LastAccessTime = now;
      }
      catch (System.Exception e)
      {
        Console.WriteLine(e.Message);
      }
      finally
      {
        //Restore Attributes in case anything happens
        try
        {
          fileSystemInfo.Attributes = fileAttributes;
        }
        finally
        {
        }
      }
    }
  }
}