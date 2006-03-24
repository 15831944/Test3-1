// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Collections;

namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Summary description for TreeEnumerator.
	/// </summary>
	public class TreeEnumerator : IEnumerator
	{
		private TreeEnumerator(Node root,
			bool enumArrows, bool strict, bool reversedDir)
		{
			objects = new ChartObjectCollection();
			this.enumArrows = enumArrows;
			this.reversedDir = reversedDir;
			this.strict = strict;
			buildTree(root);
			internalEnum = objects.GetEnumerator();
		}


		public TreeEnumerator(Node root, bool enumArrows, bool strict) :
			this(root, enumArrows, strict, false)
		{
		}

		IEnumerator internalEnum = null;
		ChartObjectCollection objects = null;
		bool enumArrows;
		bool reversedDir;
		bool strict;

		public bool MoveNext()
		{
			return internalEnum.MoveNext();
		}

		public void Reset()
		{
			internalEnum.Reset();
		}

		public object Current
		{
			get { return internalEnum.Current; }
		}

		private void buildTree(Node node)
		{
			if (objects.Contains(node))
			{
				if (!strict) return;
				throw new NotATreeException();
			}
			objects.Add(node);

			// get the appropriate arrows
			ArrowCollection arrows = new ArrowCollection();
			if (reversedDir)
				node.getAllIncomingArrows(arrows);
			else
				node.getAllOutgoingArrows(arrows);

			// build subtrees recursively
			foreach (Arrow a in arrows)
			{
				if (enumArrows) objects.Add(a);
				buildTree(reversedDir ? a.Origin : a.Destination);
			}
		}
	}

	public class NotATreeException : Exception
	{
	}
}
