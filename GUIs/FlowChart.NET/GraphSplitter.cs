// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Collections;

namespace MindFusion.LayoutSystem
{
	[Access(AccessType.Public)]
	internal sealed class GraphSplitter
	{
		/// <summary>
		/// Divides the supplied graph to its connected subgraphs.
		/// </summary>
		public static IGraph[] Split(IGraph initial, IGraphBuilder builder)
		{
			// A list with the nodes awaiting to be processed
			NodeCollection nodes = initial.Nodes.Clone() as NodeCollection;

			// A list with the subgraphs
			IGraph subgraph = null;
			ArrayList subgraphs = new ArrayList();

			while (nodes.Count > 0)
			{
				INode node = nodes[0];
				nodes.RemoveAt(0);

				subgraph = builder.Create();
				subgraph.Nodes.Add(node);

				int i = 0;
				while (i < subgraph.Nodes.Count)
				{
					node = subgraph.Nodes[i];

					foreach (ILink link in node.InLinks)
					{
						if (!subgraph.Nodes.Contains(link.Origin))
						{
							subgraph.Nodes.Add(link.Origin);
							nodes.Remove(link.Origin);
						}

						if (!subgraph.Links.Contains(link))
							subgraph.Links.Add(link);
					}

					foreach (ILink link in node.OutLinks)
					{
						if (!subgraph.Nodes.Contains(link.Destination))
						{
							subgraph.Nodes.Add(link.Destination);
							nodes.Remove(link.Destination);
						}

						if (!subgraph.Links.Contains(link))
							subgraph.Links.Add(link);
					}

					i++;
				}

				subgraphs.Add(subgraph);
			}

			IGraph[] result = new IGraph[subgraphs.Count];
			for (int i = 0; i < subgraphs.Count; i++)
				result[i] = subgraphs[i] as IGraph;

			return result;
		}
	}
}
