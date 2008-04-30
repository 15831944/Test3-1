using System.Windows.Controls;
using System.Windows.Media;
using System.Collections.Generic;
using System;

namespace WPFTest
{
  public abstract class Stencil : Canvas
  {
    private Dictionary<String, Parameter> parameters;

    //protected void AddParameter(String key, double value, double min, double max, String tag, String description)
    //{
//
//    }

    public bool ContainsKey(String key)
    {
      return parameters.ContainsKey(key);
    }

    public double Value(String key)
    {
      return parameters[key].Value;
    }

    public double Min(String key)
    {
      return parameters[key].Min;
    }

    public double Max(String key)
    {
      return parameters[key].Max;
    }

    //public double Tag(String key)
    //{
    //  return parameters[key].Tag;
    //}

    public double Portion(String key)
    {
      return parameters[key].Portion;
    }

    public void Set(String key, double value, double min, double max, String tag)
    {
      parameters[key].Value = value;
      parameters[key].Min = min;
      parameters[key].Max = max;
      parameters[key].Tag = tag;
      Update();
    }

    public void SetValue(String key, double value)
    {
      Set(key, value, parameters[key].Min, parameters[key].Max, parameters[key].Tag);
    }

    public void SetMin(String key, double min)
    {
      Set(key, parameters[key].Value, min, parameters[key].Max, parameters[key].Tag);
    }

    public void SetMax(String key, double max)
    {
      Set(key, parameters[key].Value, parameters[key].Min, max, parameters[key].Tag);
    }

    public void SetTag(String key, String tag)
    {
      Set(key, parameters[key].Value, parameters[key].Min, parameters[key].Max, tag);
    }

    public abstract void Update();

    public Stencil(Dictionary<String, Parameter> parameters)
    {
      this.parameters = parameters;
      Width = 100.0;
      Height = 100.0;
      Background = Brushes.Transparent;
    }

  } // class Stencil

}

