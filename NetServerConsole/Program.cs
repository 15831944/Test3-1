using System;
using System.Collections.Generic;
using System.Text;

using SysCAD.Protocol;
using System.Threading;

namespace NetServer
{
  class Program
  {
    static void Main(string[] args)
    {
      NetServer netServer = new NetServer();

      Thread thread = new Thread(new ParameterizedThreadStart(netServer.Startup));

      thread.Start("C:\\SysCAD91\\Examples\\General Examples\\SS_Nickel\\NiCuDemo-00.spf\\Project.spj",
                        "C:\\Documents and Settings\\pkh\\My Documents\\SysCAD\\BaseFiles\\");

      //netServer.Startup("C:\\SysCAD91\\Examples\\General Examples\\SS_Nickel\\NiCuDemo-00.spf\\Project.spj",
      //                  "C:\\Documents and Settings\\pkh\\My Documents\\SysCAD\\BaseFiles\\");
    }
  }
}
