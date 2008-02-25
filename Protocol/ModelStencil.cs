
using System;
using System.Collections.Generic;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Soap;
using System.Xml.Serialization;
using System.Drawing;
using System.Drawing.Drawing2D;

//using MindFusion.FlowChartX;
using System.Collections;
using System.IO;

namespace SysCAD.Protocol
{
  sealed class ModelStencilSerializationSurrogate : ISerializationSurrogate
  {
    public void GetObjectData(Object obj, SerializationInfo info, StreamingContext context)
    {
      ModelStencil modelStencil = (ModelStencil)obj;
      info.AddValue("groups", modelStencil.Groups);
      info.AddValue("anchors", modelStencil.Anchors);
      info.AddValue("decorations", modelStencil.Decorations);
      info.AddValue("elements", modelStencil.Elements);
      info.AddValue("tag", modelStencil.Tag);
    }

    public object SetObjectData(Object obj, SerializationInfo info, StreamingContext context, ISurrogateSelector selector)
    {
      ModelStencil modelStencil = (ModelStencil)obj;
      try
      {
        modelStencil.Groups = (ArrayList)info.GetValue("groups", typeof(ArrayList));
      }
      catch
      {
        modelStencil.Groups = new ArrayList();
        modelStencil.Groups.Add((String)info.GetValue("groupName", typeof(String)));
      }
      modelStencil.Anchors = (ArrayList)info.GetValue("anchors", typeof(ArrayList));
      modelStencil.Decorations = (ArrayList)info.GetValue("decorations", typeof(ArrayList));
      modelStencil.Elements = (ArrayList)info.GetValue("elements", typeof(ArrayList));
      modelStencil.Tag = (String)info.GetValue("tag", typeof(String));
      return modelStencil;
    }
  }


  /// <summary>
  /// Summary description for Class1.
  /// </summary>
  [Serializable]
  [XmlInclude(typeof(Line)), XmlInclude(typeof(Arc)), XmlInclude(typeof(Bezier))]
  public class ModelStencil
  {

    private ArrayList groups;

    private ArrayList anchors;
    private ArrayList decorations;

    private ArrayList elements;
    private String tag;

    public ModelStencil()
    {
    }

    public ArrayList Groups
    {
      get { return groups; }
      set { groups = value; }
    }

    public ArrayList Anchors
    {
      get { return anchors; }
      set { anchors = value; }
    }

    public ArrayList Decorations
    {
      get { return decorations; }
      set { decorations = value; }
    }

    public ArrayList Elements
    {
      get { return elements; }
      set { elements = value; }
    }

    public String Tag
    {
      get { return tag; }
      set { tag = value; }
    }


    public static void Serialize(string path, ModelStencil modelStencil)
    {
      SoapFormatter sf = new SoapFormatter();
      StreamWriter streamWriter = new StreamWriter(path);
      Stream stream = streamWriter.BaseStream;

      SurrogateSelector ss = new SurrogateSelector();
      ss.AddSurrogate(typeof(ModelStencil), new StreamingContext(StreamingContextStates.All), new ModelStencilSerializationSurrogate());
      sf.SurrogateSelector = ss;

      sf.Serialize(stream, modelStencil);
      stream.Close();
    }

    public static ModelStencil Deserialize(String path)
    {
      ModelStencil modelStencil;
      Stream stream = null;

      //try
      //{
        SoapFormatter sf = new SoapFormatter();
        StreamReader streamReader = new StreamReader(path);
        stream = (streamReader).BaseStream;

        SurrogateSelector ss = new SurrogateSelector();
        ss.AddSurrogate(typeof(ModelStencil), new StreamingContext(StreamingContextStates.All), new ModelStencilSerializationSurrogate());
        sf.SurrogateSelector = ss;

        modelStencil = (ModelStencil)sf.Deserialize(stream);
        stream.Close();
        //Serialize(path, modelStencil);
      //}
      //catch
      //{
      //  stream.Close();

      //  SoapFormatter sf = new SoapFormatter();
      //  StreamReader streamReader = new StreamReader(path);
      //  stream = (streamReader).BaseStream;

      //  modelStencil = (ModelStencil)sf.Deserialize(stream);
      //  stream.Close();
      //  modelStencil.tag = Path.GetFileNameWithoutExtension(path);
      //  Serialize(path, modelStencil);
      //}

      return modelStencil;
    }

  }
}
