// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;

namespace MindFusion.FlowChartX
{
	public enum Action
	{
		None,
		Create,
		Modify,
		Split
	}

	public enum Anchoring
	{
		Ignore,
		Keep,
		Reassign
	}

	internal enum ArrowAnchor
	{
		NoAnchor,
		HorzCenter,
		VertCenter,
		LeftCenter,
		RightCenter,
		TopCenter,
		BottomCenter,
		Pattern
	}

	public enum ArrowCrossings
	{
		Straight = 0,
		Arcs,
		BreakOff
	}

	public enum ArrowHead
	{
		None = 0,
		Arrow,
		Triangle,
		Circle,
		Tetragon,
		Rhombus,
		BowArrow,
		PointerArrow,
		Pentagon,
		DoubleArrow,
		Reversed,
		RevTriangle,
		Quill,
		RevWithLine,
		RevWithCirc,
		BackSlash,
		Slash,
		DefaultFlow
	}

	public enum ArrowStyle
	{
		Bezier = 0,
		Polyline,
		Cascading
	}

	public enum ArrowTextStyle
	{
		Center,
		Rotate,
		OverLongestSegment,
		Follow
	}

	public enum AttachTo
	{
		FixedCorner = 0,
		Proportional,
		ArrowPoint,
		ArrowSegment,
		LongestHSegment,
		SideMiddle
	}

	public enum AttachToArrow
	{
		Point,
		Segment,
		LongestHSegment,
	}

	public enum AttachToNode
	{
		TopLeft,
		TopRight,
		BottomLeft,
		BottomRight,
		TopCenter,
		MiddleRight,
		BottomCenter,
		MiddleLeft
	}

	public enum AutoSize
	{
		None,
		RightAndDown,
		AllDirections
	}

	public enum BehaviorType
	{
		Modify = 0,
		CreateBox,
		CreateArrow,
		FlowChart,
		TableRelations,
		CreateTable,
		CreateControlHost,
		LinkedControls,
		DoNothing,
		Custom
	}

	public enum BoxStyle
	{
		Rectangle = 0,
		Ellipse = 1,
		RoundedRectangle = 2,
		Rhombus = 3,

		/* This member is now obsolete
		Polygon = 4,
		*/

		Delay = 5,
		Shape = 6
	}

	public enum CellFrameStyle
	{
		None,
		Simple,
		System3D
	}

	public enum ColumnStyle
	{
		FixedWidth,
		AutoWidth
	}

	public enum CustomDraw
	{
		None = 0,
		Additional,
		Full,
		ShadowOnly,
		Additional2
	}

	public enum DirectionConstraint
	{
		None,
		Horizontal,
		Vertical
	}

	public enum ExpandButtonPosition
	{
		OuterRight,
		OuterLowerRight,
		OuterUpperRight,
		OuterBottom,
		OuterLeft,
		OuterLowerLeft,
		OuterUpperLeft,
		OuterTop
	}

	public enum FitSize
	{
		KeepWidth,
		KeepHeight,
		KeepRatio
	}

	public enum GridStyle
	{
		Points,
		Lines
	}

	[Flags]
	public enum Handles
	{
		None = 0,
		ResizeTopLeft = 1,
		ResizeTopRight = 2,
		ResizeBottomRight = 4,
		ResizeBottomLeft = 8,
		ResizeTopCenter = 16,
		ResizeMiddleRight = 32,
		ResizeBottomCenter = 64,
		ResizeMiddleLeft = 128,
		Move = 256,
		Rotate = 512,
		All = ResizeTopLeft | ResizeTopRight | ResizeBottomRight | ResizeBottomLeft |
			ResizeTopCenter | ResizeMiddleRight | ResizeBottomCenter | ResizeMiddleLeft |
			Move | Rotate
	}

	public enum HandlesStyle
	{
		Invisible,
		SquareHandles,
		DashFrame,
		HatchFrame,
		HatchHandles,
		HatchHandles2,
		HatchHandles3,
		MoveOnly,
		EasyMove,
		SquareHandles2,
		Custom,
		InvisibleMove
	}

	public enum HitTestPriority
	{
		ZOrder,
		NodesBeforeArrows
	}

	public enum HostMouseAction
	{
		PassToControl,	// pass to control
		SelectHost,		// just select
		IgnoreControl	// select or draw arrow
	}

	public enum ImageAlign
	{
		Center=0,
		Fit,
		Stretch,
		Tile,
		TopLeft,
		BottomLeft,
		TopRight,
		BottomRight,
		TopCenter,
		BottomCenter,
		MiddleLeft,
		MiddleRight,
		Document = 0x1000
	}

	public enum ItemType
	{
		None = 0,
		Box,
		Arrow,
		Table,
		Selection,
		ControlHost,
		Dummy
	}

	public enum MarkStyle
	{
		None,
		Cross,
		X,
		Circle,
		Rectangle,
		Custom
	}

	public enum ModificationStyle
	{
		SelectedOnly,
		AutoHandles
	}

	public enum ModifierKeyAction
	{
		None,
		Pan,
		Select,
		OverrideBehavior
	}

	public enum Orientation
	{
		Auto,
		Horizontal,
		Vertical
	}

	public enum Relationship
	{
		OneToMany,
		ManyToOne
	}

	public enum RerouteArrows
	{
		Never = 0,
		WhenModified = 1,
		WhenIntersectNode = 2,
		WhileCreating = 4
	}

	public enum RestrictToDoc
	{
		NoRestriction = 0,
		Intersection,
		InsideOnly
	}

	public enum ShadowsStyle
	{
		None,
		OneLevel,
		ZOrder
	}

	public enum ShowAnchors
	{
		Always = 0,
		Never = 1,
		Auto = 2,
		Selected = 4
	}

	public enum SnapToAnchor
	{
		OnCreate,
		OnCreateOrModify
	}

	public enum TableLinkStyle
	{
		Table = 0,
		Rows,
		Both
	}

	public enum TableStyle
	{
		Rectangle = 0,
		RoundedRectangle
	}	
}