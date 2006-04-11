using System;
using System.IO;
using System.Collections;

using System.Runtime.Remoting;
using System.Runtime.Remoting.Channels;
using System.Runtime.Remoting.Channels.Ipc;
using System.Runtime.Remoting.Channels.Http;
using System.Runtime.Remoting.Channels.Tcp;

using System.Data;
using System.Data.OleDb;

using SysCAD.Interface;
using System.Runtime.Serialization.Formatters;
using System.Runtime.Serialization.Formatters.Soap;

namespace SysCAD.Service
{
	class Service
	{
    [STAThread]
    static void Main(string[] args) 
    {
      BinaryServerFormatterSinkProvider serverProv = new BinaryServerFormatterSinkProvider();
      serverProv.TypeFilterLevel = System.Runtime.Serialization.Formatters.TypeFilterLevel.Full;

      BinaryClientFormatterSinkProvider clientProv = new BinaryClientFormatterSinkProvider();

      IDictionary ipcProps = new Hashtable();
      ipcProps["portName"] = "SysCAD.Service";
      //ipcProps["typeFilterLevel"] = TypeFilterLevel.Full;
      IpcChannel ipcChannel = new IpcChannel(ipcProps, clientProv, serverProv);
      ChannelServices.RegisterChannel(ipcChannel, false);

      //IDictionary tcpProps = new Hashtable();
      //tcpProps["port"] = "8065";
      //tcpProps["typeFilterLevel"] = TypeFilterLevel.Full;
      //TcpChannel tcpChannel = new TcpChannel(tcpProps, null, null);
      //ChannelServices.RegisterChannel(tcpChannel, false);

      //IDictionary httpProps = new Hashtable();
      //httpProps["port"] = "8066";
      //httpProps["typeFilterLevel"] = TypeFilterLevel.Full;
      //HttpChannel httpChannel = new HttpChannel(httpProps, null, null);
      //ChannelServices.RegisterChannel(httpChannel, false);

      ConfigData config = new ConfigData();

      string args0;
      if (args.Length == 1)
        args0 = args[0];
      else
        args0 = ".\\";

      foreach (string fullpath in Directory.GetFiles(args0, "*.modelstencil"))
      {
        SoapFormatter sf = new SoapFormatter();
        Stream stream = new StreamReader(fullpath).BaseStream;
        ModelStencil modelStencil = (ModelStencil)sf.Deserialize(stream);
        modelStencil.id = Path.GetFileNameWithoutExtension(fullpath);
        config.modelStencils.Add(Path.GetFileNameWithoutExtension(fullpath), modelStencil);
        stream.Close();
        Console.WriteLine("Added modelstencil {0} to ProjectList.", Path.GetFileNameWithoutExtension(fullpath));
      }

      Console.WriteLine("\n");

      foreach (string fullpath in Directory.GetFiles(args0, "*.graphicstencil"))
      {
        SoapFormatter sf = new SoapFormatter();
        Stream stream = new StreamReader(fullpath).BaseStream;
        GraphicStencil graphicStencil = (GraphicStencil)sf.Deserialize(stream);
        graphicStencil.id = Path.GetFileNameWithoutExtension(fullpath);
        config.graphicStencils.Add(Path.GetFileNameWithoutExtension(fullpath), graphicStencil);
        stream.Close();
        Console.WriteLine("Added graphicstencil {0} to ProjectList.", Path.GetFileNameWithoutExtension(fullpath));
      }

      Console.WriteLine("\n");

      string[] directoryList = Directory.GetFiles(args0, "*.mdb");

      foreach (string fullpath in directoryList)
      {
        string filename = Path.GetFileNameWithoutExtension(fullpath);

        Graphic graphic = new Graphic();
        graphic.name = filename;

        {
          OleDbConnection connection = new OleDbConnection(@"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + fullpath);
          connection.Open();

          OleDbDataReader itemReader = (new OleDbCommand("SELECT DISTINCT Tag FROM GraphicsUnits", connection)).ExecuteReader(CommandBehavior.SingleResult);
          while (itemReader.Read())
          {
            Item item = new Item(itemReader.GetString(0));
            item.Populate(connection);
            graphic.items.Add(itemReader.GetString(0), item);
          }
          itemReader.Close();

          OleDbDataReader linkReader = (new OleDbCommand("SELECT DISTINCT Tag FROM ModelLinks", connection)).ExecuteReader(CommandBehavior.SingleResult);
          while (linkReader.Read())
          {
            Link link = new Link(linkReader.GetString(0));
            link.Populate(connection);
            graphic.links.Add(linkReader.GetString(0), link);
          }
          linkReader.Close();

          int pages = 0;
          OleDbDataReader areaCountReader = (new OleDbCommand("SELECT DISTINCT Page FROM GraphicsUnits ORDER BY Page", connection)).ExecuteReader(CommandBehavior.SingleResult);
          while (areaCountReader.Read())
          {
            pages++;
          }
          areaCountReader.Close();

          Area rootArea = new Area(filename);
          graphic.___areas.Add(filename, rootArea);

          OleDbDataReader areaReader = (new OleDbCommand("SELECT DISTINCT Page FROM GraphicsUnits ORDER BY Page", connection)).ExecuteReader(CommandBehavior.SingleResult);
          
          int sqrtPages = (int)System.Math.Round(System.Math.Sqrt((double)pages)+0.5);
          int i = 0;
          int j = 0;
          float dX = 0.0F;
          float dY = 0.0F;
          while (areaReader.Read())
          {
            Area area = new Area(areaReader.GetString(0));
            area.Populate(connection, graphic.items, graphic.links, ref dX, ref dY);

            i++;
            dX += 400.0F;
            if (i > sqrtPages-1)
            {
              i = 0;
              dX = 0.0F;
              j++;
              dY += 320.0F;
            }

            rootArea.areas.Add(areaReader.GetString(0), area);
          }
          areaReader.Close();
        }

        RemotingServices.Marshal(graphic, filename);
        config.projectList.Add(filename);
        Console.WriteLine("Added project {0} to ProjectList.", filename);

      }

      RemotingServices.Marshal(config, "Global");

      Console.WriteLine();
      Console.Write("All projects added successfully, press <enter> to end.");
      Console.ReadLine();
    }	
  }
}
