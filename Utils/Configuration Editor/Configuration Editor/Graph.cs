using System;
using System.Drawing;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Forms;
using System.Drawing.Drawing2D;

namespace Configuration_Editor
{
    public abstract class GraphSeries
    {
        protected Color m_Color;
        protected bool m_bSelected;
        protected double m_dLineLoc;

        public GraphSeries()
        {
            m_dLineLoc = 0.4 + 0.2 * Program.rnd.NextDouble();
        }

        public virtual double LineLoc { get { return m_dLineLoc; } }

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
        public abstract float Max
        {
            get;
        }
        public abstract float Min
        {
            get;
        }

        public abstract double MinX { set; get; }
        public abstract double MaxX { set; get; }

        public abstract float ValueAt(double x);
        public abstract bool ValidAt(double x);

        public event EventHandler RedrawRequired;

        protected void FireRedrawRequired(object sender, EventArgs e)
        {
            if (RedrawRequired != null)
                RedrawRequired(sender, e);
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
            if (d.Max == d.Min)
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
                float YFact = plotRect.Height / (d.Max - d.Min);
                for (int i = 0; i < dataPoints; i++)
                {
                    double x = i * gran + m_dMinXValue;
                    PointF newPoint = d.ValidAt(x) ? new PointF(plotRect.Left + i * XFact, plotRect.Bottom - (d.ValueAt(x) - d.Min) * YFact) : PointF.Empty;
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
                double SeriesRange = selectedSeries.Max - selectedSeries.Min;

                Brush textBrush2 = new SolidBrush(selectedSeries.DefaultColour);
                Pen yAxisPen = new Pen(textBrush2);
                float sHeight = gfx.MeasureString("0", this.Font).Height;
                int maxPossibleChecks = (int)(plotRect.Height / sHeight) + 1;
                
                int numChecks = maxPossibleChecks > m_nMaxChecks ? m_nMaxChecks : maxPossibleChecks;

                bool labelChecks = numChecks > 1;

                if (numChecks < 2) numChecks = 2;

                float YFact = plotRect.Height / (numChecks - 1);
                double YFact2 = SeriesRange / (numChecks - 1);
                sfmt.Alignment = StringAlignment.Far;
                sfmt.LineAlignment = StringAlignment.Center;

                if (SeriesRange != 0)
                {
                    for (int i = 0; i < numChecks; i++)
                    {
                        PointF checkRight = new PointF(plotRect.Left, plotRect.Bottom - YFact * i);
                        PointF checkLeft = new PointF(checkRight.X - m_nCheckSize, checkRight.Y);
                        gfx.DrawLine(yAxisPen, checkRight, checkLeft);
                        if (labelChecks)
                        {
                            string label = (selectedSeries.Min + YFact2 * i).ToString("G2");
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
}