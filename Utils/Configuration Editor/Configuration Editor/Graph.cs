using System;
using System.Drawing;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Forms;
using System.Drawing.Drawing2D;
using System.Text.RegularExpressions;

namespace Configuration_Editor
{
    public abstract class GraphSeries
    {
        protected Color m_Color;
        protected bool m_bSelected;
        protected double m_dLineLoc;
        protected float m_fMax = 100, m_fMin = 0;
        protected double m_dMinX = double.NaN, m_dMaxX = double.NaN;
        protected int m_nScanResolution = 100;
        protected bool m_bLogarithmic = false;

        public GraphSeries()
        {
            m_dLineLoc = 0.4 + 0.2 * Program.rnd.NextDouble();
        }

        public virtual double LineLoc { get { return m_dLineLoc; } }

        public bool Logarithmic
        {
            get { return m_bLogarithmic; }
            set
            {
                m_bLogarithmic = value;
                FireRedrawRequired(this, new EventArgs());
            }
        }

        public Color DefaultColour
        {
            get
            {
                return m_Color;
            }
            set
            {
                m_Color = value;
                if (RedrawRequired != null)
                    RedrawRequired(this, new EventArgs());
            }
        }
        public bool Selected
        {
            get
            {
                return m_bSelected;
            }
            set
            {
                m_bSelected = value;
                if (RedrawRequired != null)
                    RedrawRequired(this, new EventArgs());
            }
        }
        public abstract double Granularity
        {
            get;
        }

        #region Min / Max stuff
        public virtual float Max
        {
            get { return m_fMax; }
        }

        public virtual float Min
        {
            get { return m_fMin; }
        }

        public virtual double MinX
        {
            get { return m_dMinX; }
            set
            {
                m_dMinX = value;
                RecalculateMinMax();
            }
        }

        public virtual double MaxX
        {
            get { return m_dMaxX; }
            set
            {
                m_dMaxX = value;
                RecalculateMinMax();
            }
        }

        protected void RecalculateMinMax()
        {
            if (double.IsNaN(m_dMinX) || double.IsNaN(m_dMaxX))
                return;
            float min = float.PositiveInfinity; float max = float.NegativeInfinity;
            double step = (m_dMaxX - m_dMinX) / (m_nScanResolution - 1);
            for (int i = 0; i < m_nScanResolution; i++)
            {
                if (!ValidAt(m_dMinX + step * i))
                    continue;
                float v = ValueAt(m_dMinX + step * i);
                if (min > v)
                    min = v;
                if (max < v)
                    max = v;
            }
            m_fMin = min;
            m_fMax = max;
        }
        #endregion

        public abstract float ValueAt(double x);
        public abstract bool ValidAt(double x);

        public event EventHandler RedrawRequired;

        protected void FireRedrawRequired(object sender, EventArgs e)
        {
            if (RedrawRequired != null)
                RedrawRequired(sender, e);
            RecalculateMinMax();
        }
    }

    public class Graph : Panel
    {
        protected const int m_nSpacer = 3;
        protected const int m_nArrowSize = 3;

        #region Variables
        protected int m_nBorder = 5;
        protected int m_nCheckSize = 5;
        protected List<GraphSeries> m_Datasets = new List<GraphSeries>();
        protected double m_dMinXValue, m_dMaxXValue;
        protected int m_nUnselectedTransparency = 64;
        protected int m_nMaxChecks = 6;
        protected string m_sXLabel;
        protected bool m_bDrawYLabel = false;
        protected string m_sYLabel;
        protected Color m_YLabelColour;
        #endregion Variables

        #region Properties
        public System.Collections.ObjectModel.ReadOnlyCollection<GraphSeries> Series
        {
            get { return m_Datasets.AsReadOnly(); }
        }

        public int Border
        {
            get { return m_nBorder; }
            set
            {
                m_nBorder = value;
                this.Invalidate();
            }
        }

        public double MinXValue
        {
            get { return m_dMinXValue; }
            set
            {
                m_dMinXValue = value;
                this.Invalidate();
                foreach (GraphSeries gs in m_Datasets)
                    gs.MinX = value;
            }
        }
        public double MaxXValue
        {
            get { return m_dMaxXValue; }
            set
            {
                m_dMaxXValue = value;
                this.Invalidate();
                foreach (GraphSeries gs in m_Datasets)
                    gs.MaxX = value;
            }
        }

        public int UnselectedTransparency
        {
            get { return m_nUnselectedTransparency; }
            set
            {
                m_nUnselectedTransparency = value;
                this.Invalidate();
            }
        }

        public int MaxChecks
        {
            get { return m_nMaxChecks; }
            set
            {
                m_nMaxChecks = value;
                this.Invalidate();
            }
        }

        public int CheckSize
        {
            get { return m_nCheckSize; }
            set
            {
                m_nCheckSize = value;
                this.Invalidate();
            }
        }

        public string XLabel
        {
            get { return m_sXLabel; }
            set
            {
                m_sXLabel = value;
                this.Invalidate();
            }
        }

        public string YLabel
        {
            get { return m_sYLabel; }
            set
            {
                m_sYLabel = value;
                this.Invalidate();
            }
        }

        public Color YLabelColour
        {
            get { return m_YLabelColour; }
            set
            {
                m_YLabelColour = value;
                this.Invalidate();
            }
        }

        public bool DrawYLabel
        {
            get { return m_bDrawYLabel; }
            set
            {
                m_bDrawYLabel = value;
                this.Invalidate();
            }
        }
        #endregion Properties

        #region Helper Properties
        public float Range { get { return (float)(m_dMaxXValue - m_dMinXValue); } }
        #endregion Helper Properties

        #region Constructors
        public Graph()
        {
            this.DoubleBuffered = true;
            this.SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.OptimizedDoubleBuffer |
                ControlStyles.ResizeRedraw | ControlStyles.UserPaint, true);
        }
        #endregion Constructors

        #region Public functions
        public void AddDataset(GraphSeries d)
        {
            if (!m_Datasets.Contains(d))
            {
                m_Datasets.Add(d);
                d.RedrawRequired += new EventHandler(d_RedrawRequired);
                d.MinX = MinXValue;
                d.MaxX = MaxXValue;
            }
            this.Invalidate();
        }

        public void RemoveDataset(GraphSeries d)
        {
            m_Datasets.Remove(d);
            d.RedrawRequired -= new EventHandler(d_RedrawRequired);
            this.Invalidate();
        }
        #endregion Public Functions

        #region Protected Functions
        protected RectangleF PlotRect(Graphics gfx)
        {
            RectangleF rect = this.ClientRectangle;

            SizeF textSize = gfx.MeasureString("0.00E00", Font);

            float xAdj = 2 * (m_nBorder + m_nCheckSize + m_nSpacer) + m_nArrowSize;
            if (m_bDrawYLabel)
            {
                rect.X += m_nBorder + m_nCheckSize + m_nSpacer + textSize.Width;
                rect.Width -= 2 * (m_nBorder + m_nCheckSize + m_nSpacer) + textSize.Width;
            }
            else
            {
                rect.X += m_nBorder + m_nCheckSize + m_nSpacer;
                rect.Width -= 2 * (m_nBorder + m_nCheckSize + m_nSpacer) + m_nArrowSize;
            }

            rect.Y += m_nBorder + m_nCheckSize + m_nSpacer + m_nArrowSize;
            rect.Height -= 2 * (m_nBorder + m_nCheckSize + m_nSpacer) + textSize.Height;
            if (m_sXLabel != null && !string.IsNullOrEmpty(m_sXLabel.Trim()))
                rect.Height -= 2 * m_nSpacer + textSize.Height;


            return rect;
        }

        protected void DrawDataSet(GraphSeries d, Graphics gfx, RectangleF plotRect, int lineLoc)
        {
            Color c = d.Selected ? d.DefaultColour : Color.FromArgb(m_nUnselectedTransparency, d.DefaultColour);
            Pen dataPen = new Pen(c, 2);

            //Handle the extreme cases first:
            if (float.IsInfinity(d.Max) || float.IsInfinity(d.Min) ||
                float.IsNaN(d.Max) || float.IsNaN(d.Min) || Range == 0)
            {
                //gfx.DrawLine(dataPen, new PointF(plotRect.Bottom, plotRect.Left), new PointF(plotRect.Bottom, plotRect.Right));
                return;
            }

            
            gfx.SetClip(plotRect);

            double pixelGranularity = Range / plotRect.Width;
            double gran = pixelGranularity > d.Granularity ? pixelGranularity : d.Granularity;
            int dataPoints = (int)(Range / gran + 1);
            if (dataPoints < 2) dataPoints = 2; //Avoid divide by zero problems, and ensure that the graph at least draws a line across it.
            PointF lastPoint = PointF.Empty;

            float XFact = plotRect.Width / (dataPoints - 1);

            float dMin;
            if (d.Logarithmic && d.Min * d.Max <= 0)
                dMin = d.Max * 1E-6f;
            else
                dMin = d.Min;

            if (d.Max == dMin)
            {
                //This is the special case of the function not varying with x. However, the function may still not be valid for all x:
                float Y = (plotRect.Top + plotRect.Bottom) / 2 + lineLoc;
                for (int i = 0; i < dataPoints; i++)
                {
                    double x = i * gran + m_dMinXValue;
                    PointF newPoint = d.ValidAt(x) ? new PointF(plotRect.Left + i * XFact, Y) : PointF.Empty;
                    if (lastPoint != PointF.Empty && newPoint != PointF.Empty)
                        gfx.DrawLine(dataPen, lastPoint, newPoint);
                    lastPoint = newPoint;
                }
            }
            else
            {
                float YFact = (float) (d.Logarithmic ? plotRect.Height  / Math.Log(d.Max / dMin) : plotRect.Height / (d.Max - dMin));
                for (int i = 0; i < dataPoints; i++)
                {
                    double x = i * gran + m_dMinXValue;
                    double y = d.ValidAt(x) ? d.ValueAt(x) : 0;
                    float yScreenSpace = (float) (d.Logarithmic ? y * dMin > 0 ? Math.Log(y/dMin) : double.NaN : (y - dMin));
                    PointF newPoint = d.ValidAt(x) && !double.IsNaN(yScreenSpace) ? new PointF(plotRect.Left + i * XFact, plotRect.Bottom - YFact * yScreenSpace) : PointF.Empty;
                    if (lastPoint != PointF.Empty && newPoint != PointF.Empty)
                        gfx.DrawLine(dataPen, lastPoint, newPoint);
                    lastPoint = newPoint;
                }
            }

            gfx.SetClip(this.ClientRectangle);
        }
        #endregion Protected Functions

        private void d_RedrawRequired(object sender, EventArgs e)
        {
            this.Invalidate();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            Graphics gfx = e.Graphics;

            Pen axisPen = new Pen(this.ForeColor);
            Brush textBrush = new SolidBrush(this.ForeColor);
            StringFormat sfmt = new StringFormat();
            sfmt.Alignment = StringAlignment.Center;
            sfmt.LineAlignment = StringAlignment.Near;

            RectangleF plotRect = PlotRect(gfx);

            gfx.SmoothingMode = SmoothingMode.None; //Axes look worse when AA'd.

            //Then: Draw the axes:
            //(For now, we will simply draw lines without arrows - to be upgraded in future.)

            gfx.DrawLine(axisPen, new PointF(plotRect.Left, plotRect.Bottom), new PointF(plotRect.Right, plotRect.Bottom));
            gfx.DrawLine(axisPen, new PointF(plotRect.Left, plotRect.Bottom), new PointF(plotRect.Left, plotRect.Top));

            #region X-axis
            {
                //Determine if we can fit (MaxChecks) into the graph:
                string s = m_dMaxXValue.ToString("0");
                float sWidth = gfx.MeasureString(s, this.Font).Width;
                int maxPossibleChecks = (int)(plotRect.Width / sWidth) + 1;
                int numChecks = maxPossibleChecks > m_nMaxChecks ? m_nMaxChecks : maxPossibleChecks;
                bool labelChecks = numChecks > 1;

                if (numChecks < 2) numChecks = 2;

                float XFact = plotRect.Width / (numChecks - 1);
                double XFact2 = Range / (numChecks - 1);

                for (int i = 0; i < numChecks; i++)
                {
                    PointF checkTop = new PointF(plotRect.Left + XFact * i, plotRect.Bottom);
                    PointF checkBottom = new PointF(checkTop.X, checkTop.Y + m_nCheckSize);
                    gfx.DrawLine(axisPen, checkTop, checkBottom);
                    if (labelChecks)
                    {
                        string label = (m_dMinXValue + XFact2 * i).ToString("0");
                        PointF textTop = new PointF(checkBottom.X, checkBottom.Y + m_nSpacer);
                        gfx.DrawString(label, Font, textBrush, textTop, sfmt);
                    }
                }

                if (m_sXLabel != null && !string.IsNullOrEmpty(m_sXLabel.Trim()))
                {
                    PointF textTop = new PointF((plotRect.Left + plotRect.Right) / 2,
                        plotRect.Bottom + m_nCheckSize + 2 * m_nSpacer + gfx.MeasureString("fg", Font).Height);
                    gfx.DrawString(m_sXLabel, Font, textBrush, textTop, sfmt);
                }
            }
            #endregion X-Axis

            #region Y-Axis
            GraphSeries selectedSeries = null;
            foreach (GraphSeries gs in m_Datasets)
                if (gs.Selected)
                {
                    selectedSeries = gs; break;
                }

            if (m_bDrawYLabel && selectedSeries != null && !float.IsNaN(selectedSeries.Min) && !float.IsNaN(selectedSeries.Max)
                &&!float.IsInfinity(selectedSeries.Min) && !float.IsInfinity(selectedSeries.Max))
            {
                Brush textBrush2 = new SolidBrush(selectedSeries.DefaultColour);
                Pen yAxisPen = new Pen(textBrush2);

                sfmt.Alignment = StringAlignment.Far;
                sfmt.LineAlignment = StringAlignment.Center;

                if (selectedSeries.Logarithmic)
                {
                    if (selectedSeries.Min == selectedSeries.Max)
                    {
                        PointF checkRight = new PointF(plotRect.Left, (plotRect.Bottom + plotRect.Top) / 2);
                        PointF checkLeft = new PointF(checkRight.X - m_nCheckSize, checkRight.Y);
                        gfx.DrawLine(yAxisPen, checkRight, checkLeft);
                        string label = (selectedSeries.Min).ToString("G2");
                        PointF textRight = new PointF(checkLeft.X - m_nSpacer, checkLeft.Y);
                        gfx.DrawString(label, Font, textBrush2, textRight, sfmt);
                    }
                    else
                    {
                        float graphMin = selectedSeries.Min * selectedSeries.Max > 0 ? selectedSeries.Min : selectedSeries.Max * 1E-6f;
                        bool seriesNegative = graphMin < 0;
                        int min = (int)Math.Log10(seriesNegative ? -selectedSeries.Max : graphMin);
                        int max = (int)Math.Log10(seriesNegative ? -graphMin : selectedSeries.Max);
                        for (int i = max; i > min; i--)
                        {
                            if (!seriesNegative && Math.Pow(10, i) < graphMin)
                                continue;
                            else if (seriesNegative && -Math.Pow(10, i) < graphMin)
                                continue;

                            float y = plotRect.Bottom - plotRect.Height * (float) ((i - Math.Log10((seriesNegative ? -1 : 1) * graphMin)) / Math.Log10(selectedSeries.Max / graphMin));
                            PointF checkRight = new PointF(plotRect.Left, y);
                            PointF checkLeft = new PointF(checkRight.X - m_nCheckSize, checkRight.Y);
                            gfx.DrawLine(yAxisPen, checkRight, checkLeft);
                            string label = (seriesNegative ? "-" : "") + "10^" + i;
                            PointF textRight = new PointF(checkLeft.X - m_nSpacer, checkLeft.Y);
                            gfx.DrawString(label, Font, textBrush2, textRight, sfmt);
                        }
                    }
                }
                else
                {
                    double SeriesRange = selectedSeries.Max - selectedSeries.Min;

                    
                    float sHeight = gfx.MeasureString("0", this.Font).Height;
                    int maxPossibleChecks = (int)(plotRect.Height / sHeight) + 1;

                    int numChecks = maxPossibleChecks > m_nMaxChecks ? m_nMaxChecks : maxPossibleChecks;

                    bool labelChecks = numChecks > 1;

                    if (numChecks < 2) numChecks = 2;

                    float YFact = plotRect.Height / (numChecks - 1);
                    double YFact2 = SeriesRange / (numChecks - 1);

                    if (SeriesRange != 0)
                    {
                        for (int i = 0; i < numChecks; i++)
                        {
                            PointF checkRight = new PointF(plotRect.Left, plotRect.Bottom - YFact * i);
                            PointF checkLeft = new PointF(checkRight.X - m_nCheckSize, checkRight.Y);
                            gfx.DrawLine(yAxisPen, checkRight, checkLeft);
                            if (labelChecks)
                            {
                                //G2 seems to love to show 100 as 1E2, which looks like shit.
                                double val = selectedSeries.Min + YFact2 * i;
                                string label;
                                if (Math.Abs(val) < 10000 && Math.Abs(val) > 10)
                                    label = val.ToString("F0");
                                else
                                    label = val.ToString("G2");
                                PointF textRight = new PointF(checkLeft.X - m_nSpacer, checkLeft.Y);
                                gfx.DrawString(label, Font, textBrush2, textRight, sfmt);
                            }
                        }
                    }
                    else //Draw one check in the centre...
                    {
                        PointF checkRight = new PointF(plotRect.Left, (plotRect.Bottom + plotRect.Top) / 2);
                        PointF checkLeft = new PointF(checkRight.X - m_nCheckSize, checkRight.Y);
                        gfx.DrawLine(yAxisPen, checkRight, checkLeft);
                        string label = (selectedSeries.Min).ToString("G2");
                        PointF textRight = new PointF(checkLeft.X - m_nSpacer, checkLeft.Y);
                        gfx.DrawString(label, Font, textBrush2, textRight, sfmt);
                    }

                    /*gfx.TranslateTransform(plotRect.Left, plotRect.Bottom);
                    gfx.RotateTransform(-90);
                    gfx.DrawString("I'm a happy treestump", Font, textBrush, 0, 0);*/
                }
            }
            #endregion Y-Axis

            gfx.SmoothingMode = SmoothingMode.AntiAlias;

            //First: Draw the unselected items:
            foreach (GraphSeries d in m_Datasets)
                if (!d.Selected)
                    DrawDataSet(d, gfx, plotRect, 2 * m_Datasets.IndexOf(d));
            //Then, draw the selected items:
            foreach (GraphSeries d in m_Datasets)
                if (d.Selected)
                    DrawDataSet(d, gfx, plotRect, 2 * m_Datasets.IndexOf(d));

            base.OnPaint(e);
        }
    }

    public class EquationGraphSeries : GraphSeries
    {
        protected EquationFragment m_Frag;
        protected string m_GraphVariable;

        public string GraphVariable
        {
            get { return m_GraphVariable; }
            set
            {
                m_GraphVariable = value;
                FireRedrawRequired(this, new EventArgs());
                RecalculateMinMax();
            }
        }

        public override double Granularity
        {
            get { return 0; }
        }

        public override float ValueAt(double x)
        {
            if (!string.IsNullOrEmpty(m_GraphVariable))
                m_Frag.VariableValues[m_GraphVariable] = x;
            return (float)m_Frag.Value();
        }

        public override bool ValidAt(double x)
        {
            return true; //Currently Fragments cannot contain a function that is not valid everywhere.
        }

        public EquationFragment Frag
        {
            get { return m_Frag; }
            set
            {
                m_Frag = value;
                FireRedrawRequired(this, new EventArgs());
                RecalculateMinMax();
            }
        }

        public void SetVariable(string varName, double val)
        {
            m_Frag.VariableValues[varName] = val;
            FireRedrawRequired(this, new EventArgs());
        }

        public EquationGraphSeries(EquationFragment frag, string VariableName, double _MinX, double _MaxX)
        {
            m_Frag = frag;
            MinX = _MinX;
            MaxX = _MaxX;
            m_GraphVariable = VariableName;
        }

        public EquationGraphSeries(EquationFragment frag, string VariableName)
        {
            m_Frag = frag;
            m_GraphVariable = VariableName;
        }
    }

    public class PolyEquationSeries : GraphSeries
    {
        protected List<double> m_Coefficients = new List<double>();

        #region Graph Functions
        public override double Granularity
        {
            get { return 0; }
        }



        public override float ValueAt(double x)
        {
            double val = 1;
            double ret = 0;
            foreach (double coeff in m_Coefficients)
            {
                ret += val * coeff;
                val *= x;
            }
            return (float)ret;
        }

        public override bool ValidAt(double x)
        {
            return m_Coefficients.Count > 0;
        }
        #endregion Graph Functions

        public PolyEquationSeries(string s)
        {
            SetString(s);
        }

        public PolyEquationSeries(string s, double minX, double maxX)
        {
            SetString(s);
            m_dMinX = minX;
            m_dMaxX = maxX;
            RecalculateMinMax();
        }

        public int ScanResolution
        {
            get { return m_nScanResolution; }
            set
            {
                if (value < 2) value = 2;
                m_nScanResolution = value;
                RecalculateMinMax();
            }
        }

        static Regex numberGetter = new Regex(@"(\+|-)?(\d+(\.\d+)?|\.\d+)(e(\+|-)?\d+)?", RegexOptions.Compiled | RegexOptions.IgnoreCase);
        public void SetString(string s)
        {
            m_Coefficients.Clear();
            for (Match m = numberGetter.Match(s); m.Success; m = m.NextMatch())
                try
                {
                    m_Coefficients.Add(double.Parse(m.Value));
                }
                catch { } //for out of range problems.
            RecalculateMinMax();
        }


    }

    public class MultiEquationDataset : GraphSeries
    {
        protected List<GraphSeries> m_DataSets = new List<GraphSeries>();
        protected GraphSeries m_BackupSeries;

        public System.Collections.ObjectModel.ReadOnlyCollection<GraphSeries> SubSeries
        {
            get
            {
                List<GraphSeries> ret = new List<GraphSeries>(m_DataSets);
                if (m_BackupSeries != null)
                    ret.Add(m_BackupSeries);
                return ret.AsReadOnly();
            }
        }

        public GraphSeries BackupSeries
        {
            get { return m_BackupSeries; }
            set
            {
                m_BackupSeries = value;
                m_BackupSeries.MinX = MinX;
                m_BackupSeries.MaxX = MaxX;
                m_BackupSeries.RedrawRequired += new EventHandler(series_RedrawRequired);
                RecalculateMinMax();
                FireRedrawRequired(this, new EventArgs());
            }
        }

        public void AddSeries(GraphSeries series)
        {
            m_DataSets.Add(series);
            series.RedrawRequired += new EventHandler(series_RedrawRequired);
            RecalculateMinMax();
            FireRedrawRequired(this, new EventArgs());
        }

        public void RemoveSeries(GraphSeries series)
        {
            m_DataSets.Remove(series);
            series.RedrawRequired -= new EventHandler(series_RedrawRequired);
            FireRedrawRequired(this, new EventArgs());
        }

        public void ClearSeries()
        {
            m_DataSets.Clear();
            m_BackupSeries = null;
            this.m_fMin = this.m_fMax = float.NaN;
            FireRedrawRequired(this, new EventArgs());
        }

        public GraphSeries GetSeries(int i) { return m_DataSets[i]; }

        void series_RedrawRequired(object sender, EventArgs e)
        {
            FireRedrawRequired(this, e);
        }

        #region IDataSet Members
        public override double Granularity
        {
            get
            {
                double minVal = m_BackupSeries != null ? m_BackupSeries.Granularity : double.PositiveInfinity;
                foreach (GraphSeries d in m_DataSets)
                    if (d.Granularity < minVal)
                        minVal = d.Granularity;
                return minVal;
            }
        }

        public override float ValueAt(double x)
        {
            foreach (GraphSeries gs in m_DataSets)
                if (gs.MinX < x && gs.MaxX > x && gs.ValidAt(x))
                    return gs.ValueAt(x);
            return m_BackupSeries != null ? m_BackupSeries.ValueAt(x) : float.NaN;
        }

        public override bool ValidAt(double x)
        {
            foreach (GraphSeries gs in m_DataSets)
                if (gs.MinX < x && gs.MaxX > x && gs.ValidAt(x))
                    return true;
            return m_BackupSeries != null && m_BackupSeries.ValidAt(x);
        }
        #endregion
    }

}