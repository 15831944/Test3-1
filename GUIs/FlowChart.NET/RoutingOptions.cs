// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.ComponentModel;
using System.Drawing;

namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Provides properties that define the behaviour of the link routing algorithm.
	/// </summary>
	public class RoutingOptions
	{
		internal RoutingOptions(FlowChart flowChart)
		{
			this.flowChart = flowChart;

			startOrientation = Orientation.Auto;
			endOrientation = Orientation.Auto;
			triggerRerouting = RerouteArrows.WhenModified;

			SetDefaultMode();
		}

		/// <summary>
		/// Set routing costs and measures to their default values
		/// </summary>
		public void SetDefaultMode()
		{
			SetDefaultMeasures();
			
			crossingCost = 4;
			lengthCost = 2;
			nodeVicinityCost = 2;
			turnCost = 4;
			dontOptimizeLongRoutes = false;

			routeHeuristics = distanceDelegate;
		}

		/// <summary>
		/// Set routing costs to values that quickly find routes
		/// however with lower route quality
		/// </summary>
		public void SetQuickMode()
		{
			SetDefaultMeasures();

			crossingCost = 0;
			lengthCost = 2;
			nodeVicinityCost = 1;
			turnCost = 1;
			dontOptimizeLongRoutes = true;

			routeHeuristics = distSquareDelegate;
		}

		/// <summary>
		/// Set routing costs to values that find routes having low number of segments
		/// </summary>
		public void SetMinSegmentsMode()
		{
			SetDefaultMeasures();

			crossingCost = 8;
			lengthCost = 0;
			nodeVicinityCost = 8;
			turnCost = 100;
			dontOptimizeLongRoutes = false;

			routeHeuristics = maxAbsDelegate;
		}

		[DefaultValue(4.0f)]
		[Description("The link routing algorithm finds routes accross grid whose pieces are marked as used if they intersect any node. This property defines the size of the grid pieces.")]
		public float GridSize
		{
			get { return gridSize; }
			set { gridSize = value; }
		}

		[DefaultValue(12.0f)]
		[Description("Size of node surrounding areas in which a penalty cost is assigned to routes passing there.")]
		public float NodeVicinitySize
		{
			get { return nodeVicinitySize; }
			set { nodeVicinitySize = value; }
		}

		[DefaultValue((byte)4)]
		[Description("A value added to the total cost of a route which crosses another route.")]
		public byte CrossingCost
		{
			get { return crossingCost; }
			set { crossingCost = value; }
		}

		[DefaultValue((byte)2)]
		[Description("A value added to a route total cost for each grid piece occipied by the route.")]
		public byte LengthCost
		{
			get { return lengthCost; }
			set { lengthCost = value; }
		}

		[DefaultValue((byte)2)]
		[Description("A value added to the total cost of routes passing too close to a node.")]
		public byte NodeVicinityCost
		{
			get { return nodeVicinityCost; }
			set { nodeVicinityCost = value; }
		}

		[DefaultValue((byte)4)]
		[Description("A value added to the total cost of a route for each turn it makes.")]
		public byte TurnCost
		{
			get { return turnCost; }
			set { turnCost = value; }
		}

		[DefaultValue(false)]
		[Description("Find low quality routes for very long links in order to improve overall routing speed. Turn minimization is disabled for such links.")]
		public bool DontOptimizeLongRoutes
		{
			get { return dontOptimizeLongRoutes; }
			set { dontOptimizeLongRoutes = value; }
		}

		[DefaultValue(typeof(RerouteArrows), "WhenModified")]
		[Description("Indicates in what circumstances links must be rerouted automatically.")]
		public RerouteArrows TriggerRerouting
		{
			get { return triggerRerouting; }
			set { triggerRerouting = value; }
		}

		[DefaultValue(typeof(Orientation),"Auto")]
		[Description("Specifies the orientation of the first segment in a route.")]
		public Orientation StartOrientation
		{
			get { return startOrientation; }
			set { startOrientation = value; }
		}

		[DefaultValue(typeof(Orientation),"Auto")]
		[Description("Specifies the orientation of the last segment in a route.")]
		public Orientation EndOrientation
		{
			get { return endOrientation; }
			set { endOrientation = value; }
		}

		[Browsable(false)]
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public RouteHeuristics RouteHeuristics
		{
			get { return routeHeuristics; }
			set { routeHeuristics = value; }
		}

		public static RouteHeuristics MaxAbs
		{
			get { return maxAbsDelegate; }
		}

		public static RouteHeuristics DistSquare
		{
			get { return distSquareDelegate; }
		}

		public static RouteHeuristics Distance
		{
			get { return distanceDelegate; }
		}

		internal void SetDefaultMeasures()
		{
			gridSize = Constants.getRouteGridSize(flowChart.MeasureUnit);
			nodeVicinitySize = 3 * gridSize;
		}

		internal static int maxAbs(Point start, Point end)
		{
			return Math.Max(Math.Abs(end.X - start.X), Math.Abs(end.Y - start.Y));
		}

		internal static int distSquare(Point start, Point end)
		{
			return (end.X - start.X) * (end.X - start.X) +
				(end.Y - start.Y) * (end.Y - start.Y);	
		}

		internal static int distance(Point start, Point end)
		{
			return (int)Math.Sqrt((double)((end.X - start.X) * (end.X - start.X) +
				(end.Y - start.Y) * (end.Y - start.Y)));
		}

		private float gridSize;
		private float nodeVicinitySize;

		private byte crossingCost;
		private byte lengthCost;
		private byte nodeVicinityCost;
		private byte turnCost;

		private bool dontOptimizeLongRoutes;

		private RerouteArrows triggerRerouting;
		private Orientation startOrientation;
		private Orientation endOrientation;

		private RouteHeuristics routeHeuristics;

		private static RouteHeuristics maxAbsDelegate = new RouteHeuristics(maxAbs);
		private static RouteHeuristics distSquareDelegate = new RouteHeuristics(distSquare);
		private static RouteHeuristics distanceDelegate = new RouteHeuristics(distance);

		private FlowChart flowChart;
	}

	public delegate int RouteHeuristics(Point start, Point end);
}