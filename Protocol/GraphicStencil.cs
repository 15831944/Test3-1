
using System;
using System.Collections;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Soap;
using System.Xml.Serialization;

//using MindFusion.FlowChartX;
using System.Drawing;
using System.Collections.Generic;
using System.Drawing.Drawing2D;
using System.IO;

namespace SysCAD.Protocol
{
  sealed class GraphicStencilSerializationSurrogate : ISerializationSurrogate
  {
    public void GetObjectData(Object obj, SerializationInfo info, StreamingContext context)
    {
      GraphicStencil graphicStencil = (GraphicStencil)obj;
      info.AddValue("defaultSize", graphicStencil.DefaultSize);
      info.AddValue("decorations", graphicStencil.Decorations);
      info.AddValue("elements", graphicStencil.Elements);
      info.AddValue("tags", graphicStencil.Tags);
      info.AddValue("tagArea", graphicStencil.TagArea);
    }

    public object SetObjectData(Object obj, SerializationInfo info, StreamingContext context, ISurrogateSelector selector)
    {
      GraphicStencil graphicStencil = (GraphicStencil)obj;
      graphicStencil.DefaultSize = (Size)info.GetValue("defaultSize", typeof(Size));
      graphicStencil.Decorations = (ArrayList)info.GetValue("decorations", typeof(ArrayList));
      graphicStencil.Elements = (ArrayList)info.GetValue("elements", typeof(ArrayList));
      graphicStencil.Tags = (ArrayList)info.GetValue("tags", typeof(ArrayList));
      graphicStencil.TagArea = (Rectangle)info.GetValue("tagArea", typeof(Rectangle));
      return graphicStencil;
    }
  }


  /// <summary>
  /// Summary description for Class1.
  /// </summary>
  [Serializable]
  [XmlInclude(typeof(Line)), XmlInclude(typeof(Arc)), XmlInclude(typeof(Bezier))]
  public class GraphicStencil
  {
    private Size defaultSize;

    private ArrayList decorations;

    private ArrayList elements;
    private ArrayList tags;
    private Rectangle tagArea;

    public GraphicStencil()
    {
    }

    public Size DefaultSize
    {
      get { return defaultSize; }

      set { defaultSize = value; }
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

    public ArrayList Tags
    {
      get { return tags; }

      set { tags = value; }
    }

    public Rectangle TagArea
    {
      get { return tagArea; }

      set { tagArea = value; }
    }

    public static void Serialize(string path, GraphicStencil graphicStencil)
    {
      SoapFormatter sf = new SoapFormatter();
      StreamWriter streamWriter = new StreamWriter(path);
      Stream stream = streamWriter.BaseStream;

      SurrogateSelector ss = new SurrogateSelector();
      ss.AddSurrogate(typeof(GraphicStencil), new StreamingContext(StreamingContextStates.All), new GraphicStencilSerializationSurrogate());
      sf.SurrogateSelector = ss;

      sf.Serialize(stream, graphicStencil);
      stream.Close();
    }

    public static GraphicStencil Deserialize(String path)
    {
      GraphicStencil graphicStencil;
      Stream stream = null;

      //try
      //{
        SoapFormatter sf = new SoapFormatter();
        StreamReader streamReader = new StreamReader(path);
        stream = (streamReader).BaseStream;

        SurrogateSelector ss = new SurrogateSelector();
        ss.AddSurrogate(typeof(GraphicStencil), new StreamingContext(StreamingContextStates.All), new GraphicStencilSerializationSurrogate());
        sf.SurrogateSelector = ss;

        graphicStencil = (GraphicStencil)sf.Deserialize(stream);
        stream.Close();
        Serialize(path, graphicStencil);
      //}
      //catch
      //{
      //  stream.Close();

      //  SoapFormatter sf = new SoapFormatter();
      //  StreamReader streamReader = new StreamReader(path);
      //  stream = (streamReader).BaseStream;

      //  graphicStencil = (GraphicStencil)sf.Deserialize(stream);
      //  stream.Close();
      //  graphicStencil.tag = Path.GetFileNameWithoutExtension(path);
      //  Serialize(path, graphicStencil);
      //}

      return graphicStencil;
    }

  }
}
