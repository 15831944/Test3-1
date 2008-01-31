using System.Windows.Controls;
using System.Windows.Media;

namespace Root
{

    public class Stencil : Canvas
    {

        private double temperature;
        private double temperatureMax;
        private double temperatureMin;

        public virtual bool MaxTemperature
        {
            get
            {
                return temperature >= temperatureMax;
            }
        }

        public virtual bool MinTemperature
        {
            get
            {
                return temperature <= temperatureMin;
            }
        }

        public virtual double Temperature
        {
            get
            {
                double d;

                bool flag = temperature >= temperatureMin;
                if (!flag)
                {
                    d = temperatureMin;
                }
                else
                {
                    flag = temperature <= temperatureMax;
                    if (!flag)
                        d = temperatureMax;
                    else
                        d = temperature;
                }
                return d;
            }
            set
            {
                temperature = value;
            }
        }

        public virtual double TemperatureMax
        {
            get
            {
                return temperatureMax;
            }
            set
            {
                temperatureMax = value;
            }
        }

        public virtual double TemperatureMin
        {
            get
            {
                return temperatureMin;
            }
            set
            {
                temperatureMin = value;
            }
        }

        public Stencil()
        {
            temperatureMin = 0.0;
            temperatureMax = 0.0;
            temperature = 0.0;
            Width = 100.0;
            Height = 100.0;
            Background = Brushes.Transparent;
        }

    } // class Stencil

}

