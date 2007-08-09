using System;
using System.Collections;
using System.Reflection;
using System.Windows.Forms;

namespace Skybound.VisualTips.External
{
	/// <summary>
	/// Provides access to Skybound VisualTips without an assembly reference.
	/// </summary>
	internal class VisualTipService
	{
		/// <summary>Private constructor.</summary>
		private VisualTipService() { }
		
		/*
		/// <summary>
		/// Gets a value that indicates whether VisualTip is referenced in the current application.
		/// </summary>
		/// <returns></returns>
		public static bool IsReferenced
		{
			get
			{
				if (_IsReferenced == 0)
				{
					_IsReferenced = (Type.GetType("Skybound.VisualTips.VisualTip, Skybound.VisualTips", false) == null) ? 1 : 2;
				}
				return _IsReferenced == 2;
			}
		}
		static int _IsReferenced;
		*/
		
		/// <summary>
		/// Installs a VisualTip extender.
		/// </summary>
		public static void SetExtender(Type controlType, IVisualTipExtender extender)
		{
			if (SetExtenderMethod == null && !SetExtenderChecked)
			{
				SetExtenderChecked = true;
				Type providerType = Type.GetType("Skybound.VisualTips.VisualTipProvider, Skybound.VisualTips");
				if (providerType != null)
				{
					SetExtenderMethod = providerType.GetMethod("SetExtender", BindingFlags.Static | BindingFlags.NonPublic, null,
						new Type [] { typeof(Type), typeof(object) }, null);
				}
			}
			
			if (SetExtenderMethod != null)
			{
				SetExtenderMethod.Invoke(null, new object [] { controlType, extender } );
			}
		}
		
		static MethodInfo SetExtenderMethod;
		static bool SetExtenderChecked;
	}
	
	/// <summary>
	/// Summary description for IVisualTipExtender.
	/// </summary>
	interface IVisualTipExtender
	{
		object GetChildAtPoint(Control control, int x, int y);
		object GetParent(object component);
		Type [] GetChildTypes();
	}
}
