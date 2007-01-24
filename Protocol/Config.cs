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


namespace SysCAD.Protocol
{
  public class ConfigData : MarshalByRefObject
  {
    private StringCollection projectList = new StringCollection();

    private Dictionary<String, ModelStencil> modelStencils = new Dictionary<String, ModelStencil>();
    private Dictionary<String, GraphicStencil> graphicStencils = new Dictionary<String, GraphicStencil>();
    private Dictionary<String, ThingStencil> thingStencils = new Dictionary<String, ThingStencil>();

    public StringCollection ProjectList
    {
      get { return projectList; }
      set { projectList = value; }
    }

    public Dictionary<String, ModelStencil> ModelStencils
    {
      get { return modelStencils; }
    }

    public Dictionary<String, GraphicStencil> GraphicStencils
    {
      get { return graphicStencils; }
    }

    public Dictionary<String, ThingStencil> ThingStencils
    {
      get { return thingStencils; }
    }

    protected void SetModelStencils(Dictionary<String, ModelStencil> modelStencils)
    {
      this.modelStencils = modelStencils;
    }

    protected void SetGraphicStencils(Dictionary<String, GraphicStencil> graphicStencils)
    {
      this.graphicStencils = graphicStencils;
    }

    protected void SetThingStencils(Dictionary<String, ThingStencil> thingStencils)
    {
      this.thingStencils = thingStencils;
    }

    public ConfigData()
    {
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

      ProjectList = remoteConfig.ProjectList;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, remoteConfig.ModelStencils);
      memoryStream.Seek(0, SeekOrigin.Begin);
      SetModelStencils(bf.Deserialize(memoryStream) as Dictionary<String, ModelStencil>);

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, remoteConfig.GraphicStencils);
      memoryStream.Seek(0, SeekOrigin.Begin);
      SetGraphicStencils(bf.Deserialize(memoryStream) as Dictionary<String, GraphicStencil>);

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, remoteConfig.ThingStencils);
      memoryStream.Seek(0, SeekOrigin.Begin);
      SetThingStencils(bf.Deserialize(memoryStream) as Dictionary<String, ThingStencil>);
    }

  }
}
