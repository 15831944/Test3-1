using System;
using System.Collections;
using System.Drawing;
using System.Windows.Forms;
using System.IO;

using SysCAD.Interface;

namespace SysCAD.ThingEditor.ThingEditorCtrl
{
	/// <summary>
	/// 
	/// </summary>
	public class PolyLine : SegmentedOutline
	{
// Constructor
		public PolyLine(Segment init)
		{
            shapeSegments = new ArrayList();
            shapeSegments.Add(init);
        }

        public PolyLine(BinaryReader br)
        {
            shapeSegments = new ArrayList();
            Load(br);
        }

    public PolyLine(Line line)
    {
      LineSegment lineSegment = new LineSegment(new PointF(line.x1, line.y1), new PointF(line.x2, line.y2));
      lineSegment.isDecorationSegment = true;

      shapeSegments = new ArrayList();
      shapeSegments.Add(lineSegment);
    }
// end Constructor

        internal override bool hitTest(PointF p)
        {
            if (!base.hitTest(p))
            {
                belongSegment = null;
				Segment s1 = (Segment)shapeSegments[0];
				Segment s2 = (Segment)shapeSegments[shapeSegments.Count-1];
				if (s1.hitTest(p) && s1.checkCommonPointStart())
				{
					currSegment	= s1;
					return  true;
				}
				if (s2.hitTest(p) && s2.checkCommonPointEnd())
				{
					currSegment	= s2;
					return  true;
				}

				return false;
            }
            return true;
        }

        internal bool Add(Line line)
        {
          LineSegment lineSegment = new LineSegment(new PointF(line.x1, line.y1), new PointF(line.x2, line.y2));
          lineSegment.isDecorationSegment = true;

          if ((shapeSegments != null) && (shapeSegments.Count > 0) && (shapeSegments[shapeSegments.Count - 1] is LineSegment))
          {
            LineSegment oldLineSegment = shapeSegments[shapeSegments.Count - 1] as LineSegment;
            if (oldLineSegment.GetEndPoint() == lineSegment.GetStartPoint())
            {
              shapeSegments.Add(lineSegment);
              return true;
            }
          }
          return false;
        }
      }
}
