// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Collections;


namespace MindFusion.FlowChartX.Commands
{
	/// <summary>
	/// Summary description for CommandList.
	/// </summary>
	public class CommandList : ICollection, IEnumerable
	{
		public CommandList(CommandCollection innerCollection)
		{
			innerList = innerCollection.getInnerList();
		}

		public Command this[int index]
		{
			get
			{
				return (Command)innerList[index];
			}
		}

		public int IndexOf(Command value)
		{
			return innerList.IndexOf(value);
		}

		public bool Contains(Command value)
		{
			return innerList.Contains(value);
		}

		public void CopyTo(Array array, int index)
		{
			innerList.CopyTo(array, index);
		}

		public int Count
		{
			get { return innerList.Count; }
		}

		public object SyncRoot
		{
			get { return innerList.SyncRoot; }
		}

		public bool IsSynchronized
		{
			get { return innerList.IsSynchronized; }
		}

		public IEnumerator GetEnumerator()
		{
			return innerList.GetEnumerator();
		}

		private ArrayList innerList;
	}
}