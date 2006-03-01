using System;
using System.IO;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Soap;
using System.Runtime.Remoting.Lifetime;
using System.Runtime.Remoting.Channels;
using System.Collections;
using System.Runtime.Serialization.Formatters;
using System.Runtime.Remoting.Channels.Tcp;

namespace SysCAD.Interface
{
  public class ConfigData : MarshalByRefObject
  {
    public StringCollection projectList;

    public Dictionary<string, ModelStencil> modelStencils = new Dictionary<string, ModelStencil>();
    public Dictionary<string, GraphicStencil> graphicStencils = new Dictionary<string, GraphicStencil>();

    public ConfigData()
    {
      projectList = new StringCollection();
      modelStencils = new Dictionary<string, ModelStencil>();
      graphicStencils = new Dictionary<string, GraphicStencil>();
    }
  }

  public class Config : ConfigData
  {
    private ConfigData remoteConfig;

    public string connectionError = "";

    public bool Connect(string URL)
    {
      try
      {
        BinaryClientFormatterSinkProvider clientProvider = new BinaryClientFormatterSinkProvider();
        BinaryServerFormatterSinkProvider serverProvider = new BinaryServerFormatterSinkProvider();
        serverProvider.TypeFilterLevel = System.Runtime.Serialization.Formatters.TypeFilterLevel.Full;

        IDictionary props = new Hashtable();
        props["port"] = 0;
        string s = System.Guid.NewGuid().ToString();
        props["name"] = s;
        props["typeFilterLevel"] = TypeFilterLevel.Full;
        TcpChannel chan = new TcpChannel(props, clientProvider, serverProvider);
        ChannelServices.RegisterChannel(chan, false);

        remoteConfig = Activator.GetObject(typeof(ConfigData), URL) as ConfigData;
        StringCollection test = remoteConfig.projectList; // Check that the connection is up.
        connectionError = "";
        return true;
      }
      catch (Exception exception)
      {
        connectionError = exception.Message;
        return false;
      }
    }

    public void Sync()
    {
      MemoryStream memoryStream;
      System.Runtime.Serialization.Formatters.Binary.BinaryFormatter bf = new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter();

      projectList = remoteConfig.projectList;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, remoteConfig.modelStencils);
      memoryStream.Seek(0, SeekOrigin.Begin);
      modelStencils = bf.Deserialize(memoryStream) as Dictionary<string, ModelStencil>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, remoteConfig.graphicStencils);
      memoryStream.Seek(0, SeekOrigin.Begin);
      graphicStencils = bf.Deserialize(memoryStream) as Dictionary<string, GraphicStencil>;
    }
  }
}
