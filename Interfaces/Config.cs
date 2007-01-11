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
//using System.Security.Permissions;


namespace SysCAD.Interface
{
  public class ConfigData : MarshalByRefObject
  {
    protected StringCollection protectedProjectList;

    protected Dictionary<String, ModelStencil> protectedModelStencils = new Dictionary<String, ModelStencil>();
    protected Dictionary<String, GraphicStencil> protectedGraphicStencils = new Dictionary<String, GraphicStencil>();
    protected Dictionary<String, ThingStencil> protectedThingStencils = new Dictionary<String, ThingStencil>();

    public StringCollection ProjectList
    {
      get { return protectedProjectList; }
    }

    public Dictionary<String, ModelStencil> ModelStencils
    {
      get { return protectedModelStencils; }
    }

    public Dictionary<String, GraphicStencil> GraphicStencils
    {
      get { return protectedGraphicStencils; }
    }

    public Dictionary<String, ThingStencil> ThingStencils
    {
      get { return protectedThingStencils; }
    }

    public ConfigData()
    {
      protectedProjectList = new StringCollection();
      protectedModelStencils = new Dictionary<String, ModelStencil>();
      protectedGraphicStencils = new Dictionary<String, GraphicStencil>();
    }

    //[SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.Infrastructure)]
    public override Object InitializeLifetimeService()
    {
      return null;
    }
  }

  public class Config : ConfigData
  {
    private ConfigData remoteConfig;

    public String connectionError = "";

    public bool Connect(Uri url)
    {
      try
      {
        remoteConfig = Activator.GetObject(typeof(ConfigData), url.ToString()) as ConfigData;
        StringCollection test = remoteConfig.ProjectList; // Check that the connection is up.
        connectionError = "";
        return true;
      }
      catch (System.Runtime.Remoting.RemotingException remotingException)
      {
        connectionError = remotingException.Message;
        return false;
      }
    }

    public void Sync()
    {
      MemoryStream memoryStream;
      System.Runtime.Serialization.Formatters.Binary.BinaryFormatter bf = new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter();

      protectedProjectList = remoteConfig.ProjectList;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, remoteConfig.ModelStencils);
      memoryStream.Seek(0, SeekOrigin.Begin);
      protectedModelStencils = bf.Deserialize(memoryStream) as Dictionary<String, ModelStencil>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, remoteConfig.GraphicStencils);
      memoryStream.Seek(0, SeekOrigin.Begin);
      protectedGraphicStencils = bf.Deserialize(memoryStream) as Dictionary<String, GraphicStencil>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, remoteConfig.ThingStencils);
      memoryStream.Seek(0, SeekOrigin.Begin);
      protectedThingStencils = bf.Deserialize(memoryStream) as Dictionary<String, ThingStencil>;
    }

  }
}
