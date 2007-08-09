// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.

#region Using directives

using System;
using System.Drawing;

#endregion

namespace VisualHint.SmartPropertyGrid
{
    internal class ColorUtils
    {
        public static double HueToRGB(double m1, double m2, double h)
        {
	        if (h < 0) h += 1.0;
	        if (h > 1) h -= 1.0;
	        if (6.0*h < 1.0) return (m1+(m2-m1)*h*6.0);
	        if (2.0*h < 1.0) return m2;
	        if (3.0*h < 2.0) return (m1+(m2-m1)*((2.0/3.0)-h)*6.0);
	        return m1;
        }

        public static Color HSLtoRGB(double H, double S, double L)
        {
	        double r, g, b;
	        double m1, m2;

	        if (S == 0.0)
		        r = g = b = L;
	        else
	        {
		        if (L <= 0.5)
			        m2 = L*(1.0+S);
		        else
			        m2 = L+S-L*S;

		        m1 = 2.0 * L-m2;
		        r = HueToRGB(m1, m2, H+1.0/3.0);
		        g = HueToRGB(m1, m2, H);
		        b = HueToRGB(m1, m2, H-1.0/3.0);
	        }

            return Color.FromArgb((int)Math.Round(r * 255.0), (int)Math.Round(g * 255.0), (int)Math.Round(b * 255.0));
        }

        public static void RGBtoHSL(Color rgb, out double H, out double S, out double L)
        {
            double r = rgb.R / 255.0;
            double g = rgb.G / 255.0;
            double b = rgb.B / 255.0;
            double cmax = Math.Max(r, Math.Max(g, b));
            double cmin = Math.Min(r, Math.Min(g, b));

            L = (cmax + cmin) / 2.0;

	        if (cmax == cmin) 
	        {
		        S = 0;      
		        H = 0; // it's really undefined   
	        } 
	        else 
	        {
		        double delta = cmax - cmin;

		        if (L < 0.5)
			        S = delta / (cmax+cmin);      
		        else
			        S = delta / (2.0-cmax-cmin);      
        		
		        if (r == cmax) 
			        H = (g-b) / delta;      
		        else if (g == cmax)
			        H = 2.0 + (b-r) / delta;
		        else          
			        H = 4.0 + (r-g) / delta;

		        H /= 6.0; 
		        if (H < 0.0)
			        H += 1.0;
	        }
        }

        internal static double GetMaximumValue(params double[] values)
        {
            double maxValue = values[0];
            if (values.Length >= 2)
            {
                for (int i = 1; i < values.Length; i++)
                    maxValue = Math.Max(maxValue, values[i]);
            }

            return maxValue;
        }
 

        internal static double GetMinimumValue(params double[] values)
        {
            double minValue = values[0];
            if (values.Length >= 2)
            {
                for (int i = 1; i < values.Length; i++)
                    minValue = Math.Min(minValue, values[i]);
            }

            return minValue;
        }

        public static void RgbToHsb(Color rgb, out double h, out double s, out double b)
        {
            double rValue = ((double)rgb.R) / 255;
            double gValue = ((double)rgb.G) / 255;
            double bValue = ((double)rgb.B) / 255;
            double[] colorComps = new double[3] { rValue, gValue, bValue };
            double minColorComp = GetMinimumValue(colorComps);
            double maxColorComp = GetMaximumValue(colorComps);
            double colorCompDiff = maxColorComp - minColorComp;
            h = 0;
            s = 0;
            b = maxColorComp * 100;

            if ((maxColorComp != 0) && (colorCompDiff != 0))
            {
                if (minColorComp == 0)
                    s = 100;
                else
                    s = (colorCompDiff / maxColorComp) * 100;

                if (Math.Abs(rValue - maxColorComp) < 4.94065645841247E-324)
                    h = (gValue - bValue) / colorCompDiff;
                else if (Math.Abs(gValue - maxColorComp) < 4.94065645841247E-324)
                    h = 2 + ((bValue - rValue) / colorCompDiff);
                else if (Math.Abs(bValue - maxColorComp) < 4.94065645841247E-324)
                    h = 4 + ((rValue - gValue) / colorCompDiff);
            }

            h *= 60;
            if (h < 0)
                h += 360;

            h = (int)Math.Round(h);
            s = (int)Math.Round(s);
            b = (int)Math.Round(b);
        }

        public static Color HsbToRgb(double h, double s, double b)
        {
            double rValue = 0;
            double gValue = 0;
            double bValue = 0;
            s /= 100.0;
            b /= 100.0;

            if (s == 0)
            {
                rValue = b;
                gValue = b;
                bValue = b;
            }
            else
            {
                h /= 60.0;
                int i = (int)Math.Floor(h);
                double h2 = h - i;
                double p = b * (1 - s);
                double q = b * (1 - (s * h2));
                double t = b * (1 - (s * (1 - h2)));
                switch (i)
                {
                    case 0:
                    {
                        rValue = b;
                        gValue = t;
                        bValue = p;
                        break;
                    }
                    case 1:
                    {
                        rValue = q;
                        gValue = b;
                        bValue = p;
                        break;
                    }
                    case 2:
                    {
                        rValue = p;
                        gValue = b;
                        bValue = t;
                        break;
                    }
                    case 3:
                    {
                        rValue = p;
                        gValue = q;
                        bValue = b;
                        break;
                    }
                    case 4:
                    {
                        rValue = t;
                        gValue = p;
                        bValue = b;
                        break;
                    }
                    case 5:
                    {
                        rValue = b;
                        gValue = p;
                        bValue = q;
                        break;
                    }
                }
            }

            return Color.FromArgb((int)Math.Round(rValue * 255.0), (int)Math.Round(gValue * 255.0),
                (int)Math.Round(bValue * 255.0));
        }
    }
}
