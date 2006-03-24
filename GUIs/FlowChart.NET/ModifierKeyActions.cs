
using System;
using System.Windows.Forms;


namespace MindFusion.FlowChartX
{
	public enum ModifierKeyAction
	{
		None,
		Pan,
		Select,
		OverrideBehavior
	}

	/// <summary>
	/// Summary description for ModifierKeyActions.
	/// </summary>
	public sealed class ModifierKeyActions
	{
		internal ModifierKeyActions()
		{
			_control = ModifierKeyAction.Select;
			_shift = ModifierKeyAction.OverrideBehavior;
			_alt = ModifierKeyAction.Pan;
		}


		public ModifierKeyAction Control
		{
			get { return _control; }
			set { _control = value; }
		}

		public ModifierKeyAction Shift
		{
			get { return _shift; }
			set { _shift = value; }
		}

		public ModifierKeyAction Alt
		{
			get { return _alt; }
			set { _alt = value; }
		}


		internal Keys GetKeys(ModifierKeyAction action)
		{
			Keys keys = Keys.None;

			if (_control == action)
				keys |= Keys.Control;

			if (_shift == action)
				keys |= Keys.Shift;

			if (_alt == action)
				keys |= Keys.Alt;

			return keys;
		}


		private ModifierKeyAction _control;
		private ModifierKeyAction _shift;
		private ModifierKeyAction _alt;
	}
}
