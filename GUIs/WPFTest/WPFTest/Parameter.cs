using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WPFTest
{
  public class Parameter
  {
    private double val;
    private double min;
    private double max;

    private string tag;

    public double Value
    {
      get
      {
        if (val < min)
          return min;
        else if (val > max)
          return max;
        else
          return val;
      }
      set { val = value; }
    }

    public double Min
    {
      get { return min; }
      set { min = value; }
    }

    public double Max
    {
      get { return max; }
      set { max = value; }
    }

    public double Portion
    {
      get { return (val - min) / (max - min); }
      set { val = value * (max - min) + min; }
    }

    public string Tag
    {
      get { return tag; }
      set { tag = value; }
    }

    public Parameter(double val, double min, double max, String tag)
    {
      this.val = val;
      this.min = min;
      this.max = max;
      this.tag = tag;
    }
  }
}
