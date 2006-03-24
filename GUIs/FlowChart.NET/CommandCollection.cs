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
	/// Collection of commands.
	/// </summary>
	public class CommandCollection : CollectionBase
	{
		public CommandCollection(int capacity)
		{
			InnerList.Capacity = capacity;
		}

		internal ArrayList getInnerList()
		{
			return InnerList;
		}

		public CommandCollection()
		{
		}

		public void Add(Command cmd)
		{
			List.Add(cmd);
		}

		public Command GetAt(int i)
		{
			if(i >= Count)
				return null;

			return (Command)List[i];
		}

		public void Remove(Command cmd)
		{
			List.Remove(cmd);
		}

		public void RemoveRange(int index, int count)
		{
			InnerList.RemoveRange(index, count);
		}

		public new void Clear()
		{
			List.Clear();
		}

		public Command this[int index]
		{
			get
			{
				return (Command)List[index];
			}
			set
			{
				List[index] = value;
			}
		}

		public bool Contains(Command cmd)
		{
			return List.Contains(cmd);
		}
	}
}
