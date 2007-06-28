
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

    private Dictionary<String, GraphicStencil> graphicStencils = new Dictionary<String, GraphicStencil>();

    private Dictionary<String, ModelStencil> modelStencils = new Dictionary<String, ModelStencil>();
    private StringCollection projectList = new StringCollection();
    private Dictionary<String, ThingStencil> thingStencils = new Dictionary<String, ThingStencil>();

    public ConfigData()
    {
    }

    //[SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.Infrastructure)]
    public override Object InitializeLifetimeService()
    {
      return null;
    }

    public Dictionary<String, GraphicStencil> GraphicStencils
    {
      get { return graphicStencils; }
      set { this.graphicStencils = value; }
    }

    public Dictionary<String, ModelStencil> ModelStencils
    {
      get { return modelStencils; }
      set { this.modelStencils = value; }
    }

    public StringCollection ProjectList
    {
      get { return projectList; }
      set { projectList = value; }
    }

    public Dictionary<String, ThingStencil> ThingStencils
    {
      get { return thingStencils; }
      set { this.thingStencils = value; }
    }
  }

  public class Config : ConfigData
  {

    public String connectionError = "";

    private ConfigData remoteConfig;

    public void GetProjectList()
    {
      ProjectList = remoteConfig.ProjectList;
    }

    public void Syncxxx()
    {
      MemoryStream memoryStream;
      System.Runtime.Serialization.Formatters.Binary.BinaryFormatter bf = new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter();

      ProjectList = remoteConfig.ProjectList;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, remoteConfig.ModelStencils);
      memoryStream.Seek(0, SeekOrigin.Begin);
      ModelStencils = bf.Deserialize(memoryStream) as Dictionary<String, ModelStencil>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, remoteConfig.GraphicStencils);
      memoryStream.Seek(0, SeekOrigin.Begin);
      GraphicStencils = bf.Deserialize(memoryStream) as Dictionary<String, GraphicStencil>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, remoteConfig.ThingStencils);
      memoryStream.Seek(0, SeekOrigin.Begin);
      ThingStencils = bf.Deserialize(memoryStream) as Dictionary<String, ThingStencil>;
    }

    public bool TestUrl(Uri url)
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
  }
}
