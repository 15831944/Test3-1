using System;
using System.Collections;
using System.Drawing;
using System.Windows.Forms;
using System.IO;

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
	}
}
