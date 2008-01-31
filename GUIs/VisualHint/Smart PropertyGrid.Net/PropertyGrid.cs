// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using System.Reflection;
using System.Resources;
using System.Security.Permissions;
using System.IO;
using System.Drawing.Imaging;
using Microsoft.Win32;
using Skybound.Windows.Forms;

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// Central class of the package: this is the control that you drag on your forms to get the new PropertyGrid.
    /// </summary>
    /// <remarks>
    /// <para>Smart PropertyGrid is a replacement PropertyGrid that can be used in your client application in place of
    /// <see cref="System.Windows.Forms.PropertyGrid">System.Windows.Forms.PropertyGrid</see>. It provides an
    /// extended user interface for browsing the properties of an object or various properties from different objects.</para>
    /// <para>To use the property grid, you create a new instance of the PropertyGrid class on a parent control. Then
    /// you have two choices to add properties to its content:</para>
    /// <list type="bullet"><item><description>Use <see cref="SelectedObject"/> to attach the instance of
    /// an object whose properties will be displayed by the grid, or</description></item><item><description>Use dynamic
    /// methods to append, insert, remove, show/hide and enable/disable individual properties.</description></item></list>
    /// <para>With the first method, the PropertyGrid acts like the Microsoft one by using reflection to discover the
    /// properties of your object. You can still use the Microsoft mechanisms like ICustomTypeDescriptor,
    /// TypeDescriptionProvider and TypeConverter to customize your properties.</para>
    /// <para>With the second method, you control the content of the grid from the ground up. It can even be
    /// used after SelectedObject has been called to refine the content.</para>
    /// The new way to fill a grid is to use the following methods:
    /// <list type="bullet">
    /// <item><term><see cref="AppendRootCategory" /></term><description>Appends a first level category.</description></item>
    /// <item><term><see cref="InsertRootCategory" /></term><description>Inserts a first level category before another sibling category.</description></item>
    /// <item><term><see cref="AppendSubCategory" /></term><description>Appends a subcategory under a parent category.</description></item>
    /// <item><term><see cref="InsertSubCategory" /></term><description>Inserts a subcategory before another sibling property/subcategory.</description></item>
    /// <item><term><see href="/O_T_VisualHint_SmartPropertyGrid_PropertyGrid_AppendProperty.html">AppendProperty</see></term><description>Appends a property under a parent category/property.</description></item>
    /// <item><term><see href="/O_T_VisualHint_SmartPropertyGrid_PropertyGrid_InsertProperty.html">InsertProperty</see></term><description>Inserts a property before another sibling property/subcategory.</description></item>
    /// <item><term><see href="/O_T_VisualHint_SmartPropertyGrid_PropertyGrid_AppendManagedProperty.html">AppendManagedProperty</see></term><description>Appends a managed property under a parent category/property.</description></item>
    /// <item><term><see href="/O_T_VisualHint_SmartPropertyGrid_PropertyGrid_InsertManagedProperty.html">InsertManagedProperty</see></term><description>Inserts a managed property before another sibling property/subcategory.</description></item>
    /// <item><term><see cref="AppendHyperLinkProperty" /></term><description>Appends a HyperLink property under a parent category.</description></item>
    /// <item><term><see cref="InsertHyperLinkProperty" /></term><description>Inserts a HyperLink property before another sibling property/subcategory.</description></item>
    /// </list>
    /// </remarks>
    /// <example>
    /// </example>
    public class PropertyGrid : ContainerControl
    {
		#region Events

		public delegate void ValueValidationEventHandler(object sender, ValueValidationEventArgs e);
		public delegate void SelectedObjectChangedEventHandler(object sender, EventArgs e);
		public delegate void PropertySelectedEventHandler(object sender, PropertySelectedEventArgs e);
		public delegate void PropertyExpandedEventHandler(object sender, PropertyExpandedEventArgs e);
		public delegate void PropertyChangedEventHandler(object sender, PropertyChangedEventArgs e);
		public delegate void PropertyCreatedEventHandler(object sender, PropertyCreatedEventArgs e);
		public delegate void PropertyUpDownEventHandler(object sender, PropertyUpDownEventArgs e);
		public delegate void InPlaceCtrlVisibleEventHandler(object sender, InPlaceCtrlVisibleEventArgs e);
		public delegate void InPlaceCtrlCreatedEventHandler(object sender, InPlaceCtrlCreatedEventArgs e);
		public delegate void InPlaceCtrlHiddenEventHandler(object sender, InPlaceCtrlVisibleEventArgs e);
		public delegate void PropertyButtonClickedEventHandler(object sender, PropertyButtonClickedEventArgs e);
		public delegate void DisplayedValuesNeededEventHandler(object sender, DisplayedValuesNeededEventArgs e);
		public delegate void DisplayModeChangedEventHandler(object sender, EventArgs e);
		public delegate void HyperLinkPropertyClickedEventHandler(object sender, PropertyHyperLinkClickedEventArgs e);
		public delegate void PropertyEnabledEventHandler(object sender, PropertyEnabledEventArgs e);
		public delegate void DrawingManagerChangedEventHandler(object sender, EventArgs e);
		public delegate void PropertyPostFilterOutEventHandler(object sender, PropertyPostFilterOutEventArgs e);
		public delegate void PropertyPreFilterOutEventHandler(object sender, PropertyPreFilterOutEventArgs e);

		/// <summary>
		/// Occurs when a validator instance can't validate a new value for a property.
		/// </summary>
		/// <remarks>
		/// Instead of receiving this event you can also override <see cref="OnValueValidation"/>.
		/// </remarks>
		public event ValueValidationEventHandler ValueValidation;

		protected virtual void OnValueValidation(ValueValidationEventArgs e)
		{
            if (ValueValidation != null)
                ValueValidation(this, e);
		}

		/// <summary>
		/// Occurs when SelectedObject(s) has been called with a new target.
		/// </summary>
		/// <remarks>
		/// Instead of receiving this event you can also override <see cref="OnSelectedObjectChanged"/>.
		/// </remarks>
		public event SelectedObjectChangedEventHandler SelectedObjectChanged;

		/// <summary>
		/// Notifies a derived class that the target instance(s) has changed.
		/// </summary>
		/// <param name="e">No particular data is needed for this event.</param>
		/// <remarks>
		/// If you want to be notified of this event just in this instance, you can override this
		/// virtual method (don't forget to call the parent method). Otherwise, if you plan to have multiple
		/// listeners, rather subscribe to the <see cref="SelectedObjectChanged"/> event.
		/// </remarks>
		protected internal virtual void OnSelectedObjectChanged(EventArgs e)
		{
			if (SelectedObjectChanged != null)
				SelectedObjectChanged(this, e);
		}

		/// <summary>
		/// Occurs when a new property is selected.
		/// </summary>
		/// <remarks>
		/// The event is generated when the user clicks a new property or when a call to the <see cref="SelectProperty"/>
		/// method is made.
		/// <para>It is possible to access the selected property by using the passed <see cref="PropertySelectedEventArgs"/>
		/// argument.</para>
		/// <para>Instead of receiving this event you can also override <see cref="OnPropertySelected"/>.</para>
		/// </remarks>
		public event PropertySelectedEventHandler PropertySelected;

		/// <summary>
		/// Notifies a derived class that a property has been selected.
		/// </summary>
		/// <param name="e">Contains data pertaining to the event.</param>
		/// <remarks>
		/// The notification is generated when the user clicks a new property or when a call to the
		/// <see cref="SelectProperty"/> method is made.
		/// <para>It is possible to access the selected property by using the passed <see cref="PropertySelectedEventArgs"/>
		/// argument.</para>
		/// <para>If you want to be notified of this event just in this instance, you can override this
		/// virtual method (don't forget to call the parent method). Otherwise, if you plan to have multiple
		/// listeners, rather subscribe to the <see cref="PropertySelected"/> event.</para>
		/// </remarks>
		protected internal virtual void OnPropertySelected(PropertySelectedEventArgs e)
		{
			if (PropertySelected != null)
				PropertySelected(this, e);
		}

		/// <summary>
		/// Occurs after a property has been expanded or collapsed.
		/// </summary>
		/// <remarks>
		/// To know what property has been expanded or collapsed, you use the
		/// <see cref="PropertyEventArgs.PropertyEnum"/> property of the
		/// <see cref="PropertyExpandedEventArgs"/> received in argument.
		/// <para>Instead of receiving this event you can also override <see cref="OnPropertyExpanded"/>.</para>
		/// </remarks>
		public event PropertyExpandedEventHandler PropertyExpanded;

		/// <summary>
		/// Notifies a derived class that a property had been expanded or collapsed.
		/// </summary>
		/// <param name="e">Contains data pertaining to the event.</param>
		/// <remarks>
		/// To know what property has been expanded or collapsed, you use the
		/// <see cref="PropertyEventArgs.PropertyEnum"/> property of the
		/// <see cref="PropertyExpandedEventArgs"/> received in argument.
		/// <para>If you want to be notified of this event just in this instance, you can override this
		/// virtual method (don't forget to call the parent method). Otherwise, if you plan to have multiple
		/// listeners, rather subscribe to the <see cref="PropertyExpanded"/> event.</para>
		/// </remarks>
		protected internal virtual void OnPropertyExpanded(PropertyExpandedEventArgs e)
		{
			if (PropertyExpanded != null)
				PropertyExpanded(this, e);
		}

		/// <summary>
		/// Occurs when the value of a property has been changed.
		/// </summary>
		/// <remarks>
		/// To know what property has changed, you use the <see cref="PropertyEventArgs.PropertyEnum"/>
		/// property of the <see cref="PropertyChangedEventArgs"/> received in argument.
		/// <para>Instead of receiving this event you can also override <see cref="OnPropertyChanged"/>. See this
		/// method for an example.</para>
		/// </remarks>
		public event PropertyChangedEventHandler PropertyChanged;

		/// <summary>
		/// Notifies a derived class that the value of a property has changed.
		/// </summary>
		/// <param name="e">Contains data pertaining to the event.</param>
		/// <remarks>
		/// To know what property has changed, you use the <see cref="PropertyEventArgs.PropertyEnum"/>
		/// property of the <see cref="PropertyChangedEventArgs"/> received in argument.
		/// <para>If you want to be notified of this event just in this instance, you can override this
		/// virtual method (don't forget to call the parent method). Otherwise, if you plan to have multiple
		/// listeners, rather subscribe to the <see cref="PropertyChanged"/> event.</para>
		/// </remarks>
		protected virtual void OnPropertyChanged(PropertyChangedEventArgs e)
		{
			if (PropertyChanged != null)
				PropertyChanged(this, e);
		}

		/// <summary>
		/// Occurs after a property and all its descendants have been created.
		/// </summary>
		/// <remarks>
		/// To know what property has been created, you use the <see cref="PropertyEventArgs.PropertyEnum"/>
		/// property of the <see cref="PropertyCreatedEventArgs"/> received in argument.
		/// <para>This event is especially useful when you want to modify a property after it has been created by
		/// a call to <see cref="PropertyGrid.SelectedObject"/>.</para>
		/// <para>Instead of receiving this event you can also override <see cref="OnPropertyCreated"/>. See this
		/// method for an example.</para>
		/// </remarks>
		public event PropertyCreatedEventHandler PropertyCreated;

		/// <summary>
		/// Notifies a derived class that a property and all its descendants have been created.
		/// </summary>
		/// <param name="e">Contains data pertaining to the event.</param>
		/// <remarks>
		/// To know what property has been created, you use the <see cref="PropertyEventArgs.PropertyEnum"/>
		/// property of the <see cref="PropertyCreatedEventArgs"/> received in argument.
		/// <para>This event is especially useful when you want to modify a property after it has been created by
		/// a call to <see cref="PropertyGrid.SelectedObject"/>.</para>
		/// </remarks>
		protected internal virtual void OnPropertyCreated(PropertyCreatedEventArgs e)
		{
			if (PropertyCreated != null)
				PropertyCreated(this, e);
		}

		/// <summary>
		/// Occurs when the user presses the arrows of an updown inplace control.
		/// </summary>
		/// <remarks>
		/// To know what property is concerned, you use the <see cref="PropertyEventArgs.PropertyEnum"/>
		/// property of the <see cref="PropertyUpDownEventArgs"/> received in argument.
		/// <para>To know what button was pressed, you use the <see cref="PropertyUpDownEventArgs.ButtonPressed"/>
		/// property.</para>
		/// <para>This event gives you a hook to change the value of the inplace control dynamically depending
		/// on the clicked button.</para>
		/// <para>Instead of receiving this event you can also override <see cref="OnPropertyUpDown"/>. See this
		/// method for an example.</para>
		/// </remarks>
		public event PropertyUpDownEventHandler PropertyUpDown;

		/// <summary>
		/// Notifies a derived class that the user has clicked the up or down button of an inplace control.
		/// </summary>
		/// <param name="e">Contains data pertaining to the event.</param>
		/// <remarks>
		/// To know what property is concerned, you use the <see cref="PropertyEventArgs.PropertyEnum"/>
		/// property of the <see cref="PropertyUpDownEventArgs"/> received in argument.
		/// <para>To know what button was pressed, you use the <see cref="PropertyUpDownEventArgs.ButtonPressed"/>
		/// property.</para>
		/// <para>This event gives you a hook to change the value of the inplace control dynamically depending
		/// on the clicked button.</para>
		/// <para>If you want to be notified of this event just in this instance, you can override this
		/// virtual method (don't forget to call the parent method). Otherwise, if you plan to have multiple
		/// listeners, rather subscribe to the <see cref="PropertyUpDown"/> event.</para>
		/// </remarks>
		/// <example>
		/// <code>
		/// protected override void OnPropertyUpDown(PropertyUpDownEventArgs e)
		/// {
		///    // Incremets/decrements by 0.05
		///    if (e.PropertyEnum.Property.Id == _myId)
		///        e.Value = (Double.Parse(e.Value) +
		///            (e.ButtonPressed == PropertyUpDownEventArgs.UpDownButtons.Up ? 0.05 : -0.05)).ToString();
		///
		///    base.OnPropertyUpDown(e);
		/// }
		/// </code>
		/// </example>
		protected internal virtual void OnPropertyUpDown(PropertyUpDownEventArgs e)
		{
			if (PropertyUpDown != null)
				PropertyUpDown(this, e);
		}

		/// <summary>
		/// Occurs when an inplace control has been made visible.
		/// </summary>
		/// <remarks>
		/// This is sometimes useful to know that an inplace control is made visible and to receive its reference. 
		/// An inplace control is a singleton for a given PropertyGrid. So you may want different behaviours for
		/// different properties. The reference of the inplace control passed in the parameter of type
		/// <see cref="InPlaceCtrlVisibleEventArgs"/> allows you to change those behaviours based on the currently
		/// selected property.
		/// <para>Instead of receiving this event you can also override <see cref="OnInPlaceCtrlVisible"/>. See this
		/// method for an example.</para>
		/// </remarks>
		public event InPlaceCtrlVisibleEventHandler InPlaceCtrlVisible;

		/// <summary>
		/// Notifies a derived class that an inplace control has been activated on a selected property.
		/// </summary>
		/// <param name="e">Contains data pertaining to the event.</param>
		/// <remarks>
		/// This is sometimes useful to know that an inplace control is made visible and to receive its reference. 
		/// An inplace control is a singleton for a given PropertyGrid. So you may want different behaviours for
		/// different properties. The reference of the inplace control passed in the parameter of type
		/// <see cref="InPlaceCtrlVisibleEventArgs"/> allows you to change those behaviours based on the currently
		/// selected property.
		/// <para>If you want to be notified of this event just in this instance, you can override this
		/// virtual method (don't forget to call the parent method). Otherwise, if you plan to have multiple
		/// listeners, rather subscribe to the <see cref="InPlaceCtrlVisible"/> event.</para>
		/// </remarks>
		/// <example>
		/// <code>
		/// protected override void OnInPlaceCtrlVisible(InPlaceCtrlVisibleEventArgs e)
		/// {
		///    if (e.PropertyEnum.Property.Id == myId)
		///    {
		///        ((PropInPlaceUpDown)e.InPlaceCtrl).UpdateByNotifications = false;
		///        ((PropInPlaceUpDown)e.InPlaceCtrl).RealtimeChange = true;
		///    }
		/// }
		/// </code>
		/// </example>
		protected internal virtual void OnInPlaceCtrlVisible(InPlaceCtrlVisibleEventArgs e)
		{
			if (InPlaceCtrlVisible != null)
				InPlaceCtrlVisible(this, e);
		}

		/// <summary>
		/// Occurs when an inplace control has just been created, before being visible.
		/// </summary>
		/// <remarks>
		/// This is sometimes useful to know that an inplace control has been created and to receive its reference. 
		/// An inplace control is a singleton for a given PropertyGrid. So you may want different behaviours for
		/// different properties. The reference of the inplace control passed in the parameter of type
		/// <see cref="InPlaceCtrlCreatedEventArgs"/> allows you to change those behaviours based on the currently
		/// selected property.
		/// <para>Instead of receiving this event you can also override <see cref="OnInPlaceCtrlCreated"/>. See this
		/// method for an example.</para>
		/// </remarks>
		public event InPlaceCtrlCreatedEventHandler InPlaceCtrlCreated;

		/// <summary>
		/// Notifies a derived class that an inplace control has been created on a selected property.
		/// </summary>
		/// <param name="e">Contains data pertaining to the event.</param>
		/// <remarks>
		/// This is sometimes useful to know that an inplace control has been created and to receive its reference. 
		/// An inplace control is a singleton for a given PropertyGrid. So you may want different behaviours for
		/// different properties. The reference of the inplace control passed in the parameter of type
		/// <see cref="InPlaceCtrlCreatedEventArgs"/> allows you to change those behaviours based on the currently
		/// selected property.
		/// <para>If you want to be notified of this event just in this instance, you can override this
		/// virtual method (don't forget to call the parent method). Otherwise, if you plan to have multiple
		/// listeners, rather subscribe to the <see cref="InPlaceCtrlCreated"/> event.</para>
		/// <para>This callback happens before OnInPlaceCtrlCreated</para>
		/// </remarks>
		/// <example>
		/// <code>
		/// protected override void OnInPlaceCtrlCreated(InPlaceCtrlCreatedEventArgs e)
		/// {
		///    if (e.PropertyEnum.Property.Id == myId)
		///    {
		///        // Do something on the inplace control
		///    }
		/// }
		/// </code>
		/// </example>
		protected internal virtual void OnInPlaceCtrlCreated(InPlaceCtrlCreatedEventArgs e)
		{
			if (InPlaceCtrlCreated != null)
				InPlaceCtrlCreated(this, e);
		}

		/// <summary>
		/// Occurs when an inplace control has been hidden.
		/// </summary>
		/// <remarks>
		/// Instead of receiving this event you can also override <see cref="OnInPlaceCtrlHidden"/>.
		/// </remarks>
		public event InPlaceCtrlHiddenEventHandler InPlaceCtrlHidden;

		/// <summary>
		/// Notifies a derived class that an inplace control has been hidden on a selected property.
		/// </summary>
		/// <param name="e">Contains data pertaining to the event.</param>
		/// <remarks>If you want to be notified of this event just in this instance, you can override this
		/// virtual method (don't forget to call the parent method). Otherwise, if you plan to have multiple
		/// listeners, rather subscribe to the <see cref="InPlaceCtrlHidden"/> event.</remarks>
		protected internal virtual void OnInPlaceCtrlHidden(InPlaceCtrlVisibleEventArgs e)
		{
			if (InPlaceCtrlHidden != null)
				InPlaceCtrlHidden(this, e);
		}

		/// <summary>
		/// Occurs when the user clicks on the button of an inplace control.
		/// </summary>
		/// <remarks>
		/// To know what property is concerned, you use the <see cref="PropertyEventArgs.PropertyEnum"/>
		/// property of the <see cref="PropertyButtonClickedEventArgs"/> received in argument.
		/// <para>This is the place to popup your own modal editor for the property. Usually there is a "Ok" and a
		/// "Cancel" button in the dialog, so depending on the user choice, you must tell the PropertyGrid if
		/// the value actually was changed during the user interaction. You can do this by setting the 
		/// <see cref="PropertyButtonClickedEventArgs.PropertyChanged">PropertyChanged</see> property of the
		/// PropertyButtonClickedEventArgs to true or false.</para>
		/// <para>Instead of receiving this event you can also override <see cref="OnPropertyButtonClicked"/>.</para>
		/// </remarks>
		public event PropertyButtonClickedEventHandler PropertyButtonClicked;

		/// <summary>
		/// Notifies a derived class that the user has clicked the button of an inplace control.
		/// </summary>
		/// <param name="e">Contains data pertaining to the event.</param>
		/// <remarks>
		/// To know what property is concerned, you use the <see cref="PropertyEventArgs.PropertyEnum"/>
		/// property of the <see cref="PropertyButtonClickedEventArgs"/> received in argument.
		/// <para>This is the place to popup your own modal editor for the property. Usually there is a "Ok" and a
		/// "Cancel" button in the dialog, so depending on the user choice, you must tell the PropertyGrid if
		/// the value actually was changed during the user interaction. You can do this by setting the 
		/// <see cref="PropertyButtonClickedEventArgs.PropertyChanged">PropertyChanged</see> property of the
		/// PropertyButtonClickedEventArgs to true or false.</para>
		/// <para>If you want to be notified of this event just in this instance, you can override this
		/// virtual method (don't forget to call the parent method). Otherwise, if you plan to have multiple
		/// listeners, rather subscribe to the <see cref="PropertyButtonClicked"/> event.</para>
		/// </remarks>
		protected internal virtual void OnPropertyButtonClicked(PropertyButtonClickedEventArgs e)
		{
			if (PropertyButtonClicked != null)
				PropertyButtonClicked(this, e);
		}

		/// <summary>
		/// Occurs when a listbox inplace control has been dropped down and the framework needs to know what to
		/// show inside.
		/// </summary>
		/// <remarks>
		/// Usually, given the type of an object attached to the property, the control knows what to display in a list.
		/// This is the case for an enumeration. But it is also possible to use a listbox for types that have no list
		/// of values to display. This is for example the case for a simple strings and what you want to show in the
		/// listbox is dynamic.
		/// <para>A <see cref="DisplayedValuesNeededEventArgs"/> is passed and the
		/// <see cref="DisplayedValuesNeededEventArgs.DisplayedValues"/> method must be given the list of objects to
		/// display.</para>
		/// <para>Instead of receiving this event you can also override <see cref="OnDisplayedValuesNeeded"/>.</para>
		/// </remarks>
		public event DisplayedValuesNeededEventHandler DisplayedValuesNeeded;

		/// <summary>
		/// Requests to the client application the strings to display in a dropdown list whose content
		/// is not known in advance.
		/// </summary>
		/// <param name="e">Contains data pertaining to the event. Use the
		/// <see cref="DisplayedValuesNeededEventArgs.DisplayedValues"/> property of the event to supply your
		/// custom list of strings.</param>
		/// <remarks>
		/// <para>If you want to be notified of this event just in this instance, you can override this
		/// virtual method (don't forget to call the parent method). Otherwise, if you plan to have multiple
		/// listeners, rather subscribe to the <see cref="DisplayedValuesNeeded"/> event.</para>
		/// </remarks>
		/// <example>
		/// <code>
		/// protected override void OnDisplayedValuesNeeded(DisplayedValuesNeededEventArgs e)
		/// {
		///    e.DisplayedValues = new string[] { "Item1", "Item2", "Item3" };
		///    base.OnDisplayedValuesNeeded(e);
		/// }
		/// </code>
		/// </example>
		protected internal virtual void OnDisplayedValuesNeeded(DisplayedValuesNeededEventArgs e)
		{
			if (DisplayedValuesNeeded != null)
				DisplayedValuesNeeded(this, e);
		}

		/// <summary>
		/// Occurs when the display mode (flat, flat sorted or categorized) has changed.
		/// </summary>
		/// <remarks>The display mode changes when the user clicks on a corresponding button in the toolbar or when
		/// <see cref="PropertyGrid.DisplayMode"/> is called.
		/// <para>Instead of receiving this event you can also override <see cref="OnDisplayModeChanged"/>.</para>
		/// </remarks>
		/// <seealso cref="PropertyGrid.DisplayMode"/>
		public event DisplayModeChangedEventHandler DisplayModeChanged;

		/// <summary>
		/// Notifies a derived class that the user has changed the display mode to flat, flat sorted or categorized.
		/// </summary>
		/// <param name="e">Contains data pertaining to the event.</param>
		/// <remarks>The display mode changes when the user clicks on a corresponding button in the toolbar or when
		/// <see cref="PropertyGrid.DisplayMode"/> is called.
		/// <para>If you want to be notified of this event just in this instance, you can override this
		/// virtual method (don't forget to call the parent method). Otherwise, if you plan to have multiple
		/// listeners, rather subscribe to the <see cref="DisplayModeChanged"/> event.</para>
		/// </remarks>
		protected internal virtual void OnDisplayModeChanged(EventArgs e)
		{
            if (!_insideValueRecreate && !_insideSelectedObjects)
            {
                if (DisplayModeChanged != null)
                    DisplayModeChanged(this, e);
            }
		}

		/// <summary>
		/// Occurs when the user has clicked a HyperLink property.
		/// </summary>
		/// <remarks>
		/// This event allows you to react to such a click and do whatever action the HyperLink may lead.
		/// <para>Instead of receiving this event you can also override <see cref="OnHyperLinkPropertyClicked"/>.</para>
		/// </remarks>
		public event HyperLinkPropertyClickedEventHandler HyperLinkPropertyClicked;

		/// <summary>
		/// Notifies a derived class that the user has clicked a HyperLink property.
		/// </summary>
		/// <param name="e">Contains data pertaining to the event.</param>
		/// <remarks>
		/// This event allows you to react to such a click and do whatever action the HyperLink may lead.
		/// <para>If you want to be notified of this event just in this instance, you can override this
		/// virtual method (don't forget to call the parent method). Otherwise, if you plan to have multiple
		/// listeners, rather subscribe to the <see cref="HyperLinkPropertyClicked"/> event.</para>
		/// </remarks>
		protected internal virtual void OnHyperLinkPropertyClicked(PropertyHyperLinkClickedEventArgs e)
		{
			if (HyperLinkPropertyClicked != null)
				HyperLinkPropertyClicked(this, e);
		}

		/// <summary>
		/// Occurs after a property has been disabled or enabled.
		/// </summary>
		/// <remarks>
		/// To know what property is concerned, you use the <see cref="PropertyEventArgs.PropertyEnum"/>
		/// property of the <see cref="PropertyEnabledEventArgs"/> received in argument. To know the new state of
		/// the property use the <see cref="Property.Enabled" /> property.
		/// <para>It is possible to know if the property state was changed directly or indirectly (because its
		/// parent state was changed). For this, use <see cref="PropertyEnabledEventArgs.DirectlyModified"/>.
		/// </para>
		/// </remarks>
		public event PropertyEnabledEventHandler PropertyEnabled;

		/// <summary>
		/// Notifies a derived class that the enabled/disabled state of a property has changed.
		/// </summary>
		/// <param name="e">Contains data pertaining to the event.</param>
		/// <remarks>
		/// The parameter of type <see cref="PropertyEnabledEventArgs"/> allows you to know if the property
		/// was enabled/disabled directly or indirectly. Indirectly means that the new state was applied on an
		/// ancestor.
		/// <para>Be aware that if the property has children, this notification will first be sent to the parent
		/// (itself), then the state of the children will be modified recursively and this notification will be
		/// sent again for each of the descendants.</para>
		/// <para>If you want to be notified of this event just in this instance, you can override this
		/// virtual method (don't forget to call the parent method). Otherwise, if you plan to have multiple
		/// listeners, rather subscribe to the <see cref="PropertyEnabled"/> event.</para>
		/// </remarks>
		protected internal virtual void OnPropertyEnabled(PropertyEnabledEventArgs e)
		{
			if (PropertyEnabled != null)
				PropertyEnabled(this, e);
		}

		/// <summary>
		/// Occurs when the drawing manager has changed.
		/// </summary>
		/// <remarks>The drawing manager changes when <see cref="DrawingManager"/> or
		/// <see cref="DrawManager"/> is called.</remarks>
		/// <para>Instead of receiving this event you can also override <see cref="OnDrawingManagerChanged"/>.</para>
		public event DrawingManagerChangedEventHandler DrawingManagerChanged;

		/// <summary>
		/// Notifies a derived class that the drawing manager of the grid has been changed.
		/// </summary>
		/// <param name="e">Contains data pertaining to the event.</param>
		/// <remarks>The drawing manager changes when <see cref="DrawingManager"/> or
		/// <see cref="DrawManager"/> is called.
		/// <para>If you want to be notified of this event just in this instance, you can override this
		/// virtual method (don't forget to call the parent method). Otherwise, if you plan to have multiple
		/// listeners, rather subscribe to the <see cref="DrawingManagerChanged"/> event.</para>
		/// </remarks>
		protected internal virtual void OnDrawingManagerChanged(EventArgs e)
		{
			if (DrawingManagerChanged != null)
				DrawingManagerChanged(this, e);
		}

		/// <summary>
		/// Occurs when a new property has been created and you have the opportunity to filter it out.
		/// </summary>
		/// <remarks>The event <see cref="PropertyCreated"/> is triggered before this event.</remarks>
		/// <para>Instead of receiving this event you can also override <see cref="OnPropertyPostFilterOut"/>.</para>
		public event PropertyPostFilterOutEventHandler PropertyPostFilterOut;

		/// <summary>
		/// Notifies a derived class that a property and all its descendants have been created and that you have
		/// the opportunity to filter them all out.
		/// </summary>
		/// <param name="e">Contains data pertaining to the event.</param>
		/// <remarks>
		/// To know what property has been created, you use the <see cref="PropertyEventArgs.PropertyEnum"/>
		/// property of the <see cref="PropertyPostFilterOutEventArgs"/> received in argument.
		/// <para>To remove the property from the grid, just set <see cref="PropertyPostFilterOutEventArgs.FilterOut"/>
		/// to true.</para>
		/// </remarks>
		protected internal virtual void OnPropertyPostFilterOut(PropertyPostFilterOutEventArgs e)
		{
			if (PropertyPostFilterOut != null)
				PropertyPostFilterOut(this, e);
		}

		/// <summary>
		/// Occurs when a new property is about to be created and you have the opportunity to filter it out.
		/// </summary>
		/// <remarks>The event <see cref="PropertyPostFilterOut"/> is triggered after this event.</remarks>
		/// <para>Instead of receiving this event you can also override <see cref="OnPropertyPreFilterOut"/>.</para>
		public event PropertyPreFilterOutEventHandler PropertyPreFilterOut;

		/// <summary>
		/// Notifies a derived class that a property is about to be created and that you have the opportunity
		/// to filter it out.
		/// </summary>
		/// <param name="e">Contains data pertaining to the event.</param>
		/// <remarks>
		/// To know what property is about to be created, you use the <see cref="PropertyPreFilterOutEventArgs.PropertyDescriptor"/>
		/// property of the <see cref="PropertyPreFilterOutEventArgs"/> received in argument.
		/// <para>To remove the property from the grid, just set <see cref="PropertyPreFilterOutEventArgs.FilterOut"/>
		/// to true.</para>
		/// </remarks>
		protected internal virtual void OnPropertyPreFilterOut(PropertyPreFilterOutEventArgs e)
		{
			if (PropertyPreFilterOut != null)
				PropertyPreFilterOut(this, e);
		}

		#endregion

        #region PropertyFeels constants (strings)
        // The PropertyFeel IDs are not an enumeration because the user of this library
        // must be able to add his own feel classes.
        /// <exclude />
        public const string FeelNone = "none";
        /// <exclude />
        public const string FeelEdit = "edit";
#if _DOTNET2
        /// <exclude />
        public const string FeelMaskedEdit = "maskededit";
#endif
        /// <exclude />
        public const string FeelUpDown = "updown";
        /// <exclude />
        public const string FeelEditUpDown = "editupdown";
        /// <exclude />
        public const string FeelEditPassword = "editpwd";
        /// <exclude />
        public const string FeelList = "list";
        /// <exclude />
        public const string FeelEditList = "editlist";
        /// <exclude />
        public const string FeelButton = "button";
        /// <exclude />
        public const string FeelFontButton = "fontbutton";
        /// <exclude />
        public const string FeelEditButton = "editbutton";
        /// <exclude />
        public const string FeelDateTime = "datetime";
        /// <exclude />
        public const string FeelCheckbox = "checkbox";
        /// <exclude />
        public const string FeelRadioButton = "radiobutton";
        /// <exclude />
        public const string FeelTrackbar = "trackbar";
        /// <exclude />
        public const string FeelTrackbarEdit = "trackbaredit";
        /// <exclude />
        public const string FeelMultilineEdit = "multilineedit";
        /// <exclude />
        public const string FeelEditUnit = "editunit";
        #endregion

        #region Fields
        internal Hashtable _registeredFeels = new Hashtable();  // <string, PropertyFeel>

        internal Hashtable _registeredFeelAttachments = new Hashtable();    // <Type, string>

        internal Hashtable _registeredLookAttachments = new Hashtable();  // <Type, Type>

        internal Hashtable _registeredDropDownContent = new Hashtable();  // <Type, Type>

        private object[] _selectedObjects = null;

        private bool _clickThroughInProgress = false;

        private int _beginUpdateCounter = 0;

        private PropertyFeel _defaultFeel;

        internal int _globalTextMargin = 0;

        private int MinCommentsAreaHeight;

        private bool _resizingCommentsInProgress = false;

        private Point _resizingCommentsPoint;

#if _DOTNET2
        private ToolStrip _toolbar;
#else
        private ToolBar _toolbar;
#endif
        private bool _showComments = false;

        private int _commentsHeight;

        private DisplayModes _reportedDisplayMode = DisplayModes.Categorized;

        private InternalGrid _grid;

        private ValueNotValidBehaviorModes _valueNotValidBehaviorMode = ValueNotValidBehaviorModes.KeepFocus;

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private Container components = null;

        /// <summary>
        /// Contains the style of a border used on the non client area
        /// </summary>
        private BorderStyle _borderStyle = BorderStyle.None;

        private EllipsisModes _ellipsisMode = EllipsisModes.EllipsisOnValuesAndLabels;

        private int _propertyId = 0;

        private int _categoryId;

        private Hashtable _categoryMap = new Hashtable();

        private IComparer _propertyComparer;

        private Color _commentsBackColor;

        private Color _commentsForeColor;

        private bool _commentsLock = false;

        private bool _insideValueRecreate = false;

        private PropertyGrid.NavigationKeyModes _navigationKeyMode = PropertyGrid.NavigationKeyModes.ArrowKeys;

        private PropertyGrid.TabKeyNavigationModes _tabKeyNavigationMode = TabKeyNavigationModes.NoSubMode;
        
        #endregion

        #region Types

        /// <summary>
        /// Lists various modes used to set the way the user can navigate from property to property using the keyboard.
        /// </summary>
        /// <remarks>Use the <see cref="NavigationKeyMode"/> property to set this mode.</remarks>
        public enum NavigationKeyModes
        {
            /// <summary>
            /// Only the up, down, page up, page down, home and end keys can be used to browse the properties.
            /// </summary>
            ArrowKeys,

            /// <summary>
            /// This mode enables the TAB key to browse properties. Various submodes modify this navigation and can
            /// be set with the <see cref="TabKeyNavigationModes"/> property. Arrow keys can still be used.
            /// </summary>
            TabKey
        }

        /// <summary>
        /// Lists the various submodes that can be used when <see cref="NavigationKeyModes"/> has been set to TabKey.
        /// </summary>
        /// <remarks>Use the <see cref="TabKeyNavigationMode"/> property to set this mode.</remarks>
        [Flags]
        public enum TabKeyNavigationModes
        {
            /// <summary>
            /// Submode used in conjunction with TabKey. It sets no paricular submode.
            /// </summary>
            NoSubMode = 0,

            /// <summary>
            /// Submode used in conjunction with TabKey. As soon as a property is selected, its inplace control is
            /// focused and all the text is selected in the Textbox if any.
            /// </summary>
            TabKeyWithAutoFocus = 1,

            /// <summary>
            /// Submode used in conjunction with TabKey. When the selection is at the far top or bottom, the next
            /// TAB or shift+TAB press transfers the focus to a next control in the parent container. Without this
            /// mode, TAB cycles in the grid.
            /// </summary>
            TabKeyLeavesAtExtremes = 2,

            /// <summary>
            /// Submode used in conjunction with TabKey. By default, TAB gives the focus to the Textbox if any or
            /// the first child of the inplace control. Under this mode, TAB continues to give the focus to other
            /// child controls of the inplace control.
            /// </summary>
            TabKeyInSubControls = 4,

            /// <summary>
            /// Submode used in conjunction with TabKey. Under this mode, if the next property to be focused is not
            /// visible because in a collapsed hierarchy, the property is made visible by expanding all parent
            /// categories and properties.
            /// </summary>
            TabKeyInChildProperties = 8,

            /// <summary>
            /// Submode used in conjunction with TabKey. If this mode is set, the next property will be skipped
            /// if it is readonly. In the other case, a readonly property can take the focus to ensure that a
            /// clipboard copy operation is possible while passing here.
            /// </summary>
            TabKeySkipReadonlyProperties = 16
        }

        /// <exclude />
        public enum MultiSelectModes
        {
            /// <summary>The control works in single selection mode. This is the default.</summary>
            None,
            /// <summary>The control works in multi selection mode. Only properties under a same ancestor can be selected.</summary>
            SameLevel,
            /// <summary>The control works in multi selection mode. Any property can be selected.</summary>
            Global
        }

        /// <summary>
        /// List various modes to set the behavior of the grid when a value does not validate.
        /// </summary>
        /// <remarks>Use the <see cref="ValueNotValidBehaviorMode"/> property to set this mode.</remarks>
        [Flags]
        public enum ValueNotValidBehaviorModes
        {
            /// <summary>The previous valid value is restored and the focus is lost.</summary>
            IgnoreAndRestore,
            /// <summary>
            /// The invalid value is kept and the focus stays on the inplace control.</summary>
            KeepFocus
        }

        /// <summary>
        /// The different sorting options for the displayed properties.
        /// </summary>
        public enum DisplayModes
        {
            Categorized,
            Flat,
            FlatSorted
        }

        /// <summary>
        /// Identifiers defining the logical locations of the mouse inside the PropertyGrid.
        /// </summary>
        public enum HitTests
        {
            CommentSplit,
            CommentArea,
            ColumnSplit,
            TreeGlyph,
            ManualDisable,
            Value,
            Label,
            LeftColumn,
            Client
        }

        /// <summary>
        /// Defines the built-in drawing managers that can be assigned to the PropertyGrid.
        /// </summary>
        /// <remarks>
        /// If the assigned drawing manager is a custom one, then CustomDrawManager is returned.
        /// </remarks>
        public enum DrawManagers
        {
            /// <summary>
            /// For a standard flat look.
            /// </summary>
            DefaultDrawManager,
            /// <summary>
            /// Fot the same look as the Microsoft PropertyGrid.
            /// </summary>
            DotnetDrawManager,
            /// <summary>
            /// For the same look as in Sony DVD Architect.
            /// </summary>
            LightColorDrawManager,
            /// <summary>
            /// For any custom drawing manager you may create.
            /// </summary>
            [Browsable(false)]
            CustomDrawManager
        }
        
        /// <summary>
        /// The various ways to display a tooltip on the property label and/or value.
        /// </summary>
        [Flags]
        public enum ToolTipModes
        {
            None = 0,
            ToolTipsOnLabels = 1,
            ToolTipsOnValues = 2,
            ToolTipsOnValuesAndLabels = 3
        }

        /// <summary>
        /// The various ways to display the ellipsis at the end of the property label and/or value.
        /// </summary>
        [Flags]
        public enum EllipsisModes
        {
            None = 0,
            EllipsisOnLabels = 1,
            EllipsisOnValues = 2,
            EllipsisOnValuesAndLabels = 3
        }

        /// <summary>
        /// Defines the two possible behaviours that a readonly property can have.
        /// </summary>
        public enum ReadOnlyVisuals
        {
            ReadOnlyFeel = 1,
            Disabled = 2
        }

        /// <summary>
        /// These modes are used with the <see cref="DisableMode"/> property to set the behaviour of the grid when
        /// it is set to disabled with the <see cref="Enabled"/> property.
        /// </summary>
        /// <remarks>
        /// Read the page for <see cref="DisableMode"/> to have a detailed explanation of every mode.
        /// </remarks>
        public enum DisableModes
        {
            None,
            Simple,
            NoValueSelection,
            NoPropertySelection,
            Full
        }

        #endregion

        #region Properties

        /// <summary>
        /// Overriden to handle the border style of the control.
        /// </summary>
        /// <seealso cref="BorderStyle"/>
        protected override CreateParams CreateParams
        {
            [SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.UnmanagedCode)]
            get
            {
                const int WS_BORDER = 0x00800000;
                const int WS_EX_STATICEDGE = 0x00020000;

                CreateParams cp = base.CreateParams;

                switch (_borderStyle)
                {
                    case BorderStyle.FixedSingle:
                        cp.Style |= WS_BORDER;
                        break;
                    case BorderStyle.Fixed3D:
                        cp.ExStyle |= WS_EX_STATICEDGE;
                        break;
                }

                return cp;
            }
        }

        internal bool InsideValueRecreate
        {
            set { _insideValueRecreate = value; }
        }

        /// <summary>
        /// Gets or sets a value indicating whether the control can accept data that the user drags onto it.
        /// </summary>
        public override bool AllowDrop
        {
            set
            {
                _grid.AllowDrop = value;
            }

            get
            {
                return base.AllowDrop;
            }
        }

        /// <summary>
        /// Sets the IComparer instance that sorts the properties PropertyDescriptors when using SelectedObject.
        /// </summary>
        /// <remarks>The comparer is used only when calling <see cref="SelectedObject"/>. By default it used the class
        /// <see cref="PropertyComparerDefaultSort"/> to sort the categories and properties in alphabetical order.
        /// </remarks>
		[EditorBrowsable(EditorBrowsableState.Never), Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public IComparer PropertyComparer
        {
            set { _propertyComparer = value; }
            get { return _propertyComparer; }
        }

        /// <exclude />
        [EditorBrowsable(EditorBrowsableState.Never), Browsable(false)]
        public override Image BackgroundImage
        {
            get { return base.BackgroundImage; }
            set { base.BackgroundImage = value; }
        }

        /// <exclude />
#if _DOTNET2
        [EditorBrowsable(EditorBrowsableState.Never), Browsable(false)]
        public override ImageLayout BackgroundImageLayout
        {
            get { return base.BackgroundImageLayout; }
            set { base.BackgroundImageLayout = value; }
        }
#endif
        /// <summary>
        /// Sets or gets the width in pixels of the thin left column, the one that contains the expand/collapse
        /// glyph of root categories.
        /// </summary>
        /// <remarks> By default it measures 16 pixels, but it can be changed at runtime. There is no real benefit
        /// except maybe to assign it to 0 to completely hide it, giving to the control the look of a simple two-
        /// columns grid. In this case the expand/collapse glyphs won't be visible for the root nodes.
        /// </remarks>
        [Category("Appearance")]
        [Browsable(false)]
        [DefaultValue(16)]
        public int LeftColumnWidth
        {
            set { _grid.LeftColumnWidth = value; }
            get { return _grid.LeftColumnWidth; }
        }

        /// <summary>
        /// Sets or gets the color of the horizontal and vertical lines of the grid.
        /// </summary>
        /// <remarks>Each drawing manager is free to use this color as a reference for other parts of the control.
        /// For example, the <see cref="DotnetDrawManager"/> will use it for the left column, comment area and
        /// categories background and will use its hue for subcategories background.</remarks>
        [
        Category("Appearance"),
        DefaultValue(typeof(Color), "ActiveBorder")
        ]
        public Color GridColor
        {
            get { return _grid.GridColor; }
            set { _grid.GridColor = value; Invalidate(); }
        }

        /// <summary>
        /// Sets or gets the color used for the background of the grid.
        /// </summary>
        /// <remarks>This is a global value that you can override on individual properties by using
        /// <see cref="Property.BackColor"/>Property.BackColor and <see cref="PropertyValue.BackColor"/>
        /// Value.BackColor.</remarks>
        [
        Category("Appearance"),
        DefaultValue(typeof(Color), "Window")
        ]
        public Color GridBackColor
        {
            get { return _grid.BackColor; }
            set { _grid.BackColor = value; }
        }

        /// <summary>
        /// Sets or gets the color used for the text of the properties.
        /// </summary>
        /// <remarks>This is a global value that you can override on individual properties by using
        /// <see cref="Property.ForeColor"/>Property.ForeColor and <see cref="PropertyValue.ForeColor"/>
        /// Value.BackColor.</remarks>
        [
        Category("Appearance"),
        DefaultValue(typeof(Color), "ControlText")
        ]
        public Color GridForeColor
        {
            get { return _grid.ForeColor; }
            set { _grid.ForeColor = value; }
        }

        /// <summary>
        /// Sets or gets the color used to display disabled property labels and values.
        /// </summary>
        /// <remarks>This is a global value that you can override on individual properties by using
        /// <see cref="Property.DisabledForeColor">Property.DisabledForeColor</see> and
        /// <see cref="PropertyValue.DisabledForeColor"> PropertyValue.DisabledForeColor</see>.</remarks>
        [
        Category("Appearance"),
        DefaultValue(typeof(Color), "GrayText")
        ]
        public Color DisabledForeColor
        {
            get { return _grid.DisabledForeColor; }
            set { _grid.DisabledForeColor = value; }
        }

        /// <summary>
        /// Sets or gets the color used to display readonly property labels and values.
        /// </summary>
        /// <remarks>This is a global value that you can override on individual properties by using
        /// <see cref="Property.ReadOnlyForeColor">Property.ReadOnlyForeColor</see> and
        /// <see cref="PropertyValue.ReadOnlyForeColor">PropertyValue.ReadOnlyForeColor</see>.</remarks>
        [
        Category("Appearance"),
        DefaultValue(typeof(Color), "GrayText")
        ]
        public Color ReadOnlyForeColor
        {
            get { return _grid.ReadOnlyForeColor; }
            set { _grid.ReadOnlyForeColor = value; }
        }

        /// <summary>
        /// Sets or gets the color used to paint the background of the comments area.
        /// </summary>
        /// <remarks>By default, it is equal to the back color of the PropertyGrid.</remarks>
        [
        Category("Appearance"),
        DefaultValue(typeof(Color), "Control")
        ]
        public Color CommentsBackColor
        {
            get { return _commentsBackColor; }
            set
            {
                _commentsBackColor = value;
                Invalidate();
            }
        }

        /// <summary>
        /// Sets or gets the color to paint the text of the comments area.
        /// </summary>
        /// <remarks>By default, it is equal to the fore color of the PropertyGrid.</remarks>
        [
        Category("Appearance"),
        DefaultValue(typeof(Color), "ControlText")
        ]
        public Color CommentsForeColor
        {
            get { return _commentsForeColor; }

            set
            {
                _commentsForeColor = value;
                Invalidate();
            }
        }

        /// <summary>
        /// Sets or gets a value that instructs the control to draw or not ellipsis at the end of property label
        /// and/or value texts.
        /// </summary>
        [Category("Appearance")]
        [DefaultValue(PropertyGrid.EllipsisModes.EllipsisOnValuesAndLabels)]
        public EllipsisModes EllipsisMode
        {
            set
            {
                _ellipsisMode = value;
                Refresh();
            }

            get { return _ellipsisMode; }
        }

        /// <summary>
        /// Set or gets a value that indicates if tooltips are activated on the Value column, on the Label column
        /// or on both, or if they are desactivated.
        /// </summary>
        [
        Category("Appearance"),
        DefaultValue(PropertyGrid.ToolTipModes.None)
        ]
        public ToolTipModes ToolTipMode
        {
            set { _grid.ToolTipMode = value; }
            get { return _grid.ToolTipMode; }
        }

        /// <summary>
        /// Sets or gets the way the ReadOnly properties are rendered.
        /// </summary>
        /// <remarks>When using <see cref="ReadOnlyVisuals.Disabled">ReadOnlyVisuals.Disabled</see> the properties are
        /// really disabled and no inplace control is associated with them. When using <see cref="ReadOnlyVisuals.ReadOnlyFeel">
        /// ReadOnlyVisuals.ReadOnlyFeel</see> and the inplace control is or contains a textbox, it is made visible but is
        /// readonly. As a result, the text can be copied in the clipboard.</remarks>
        [Category("Appearance"),
         DefaultValue(PropertyGrid.ReadOnlyVisuals.Disabled)]
        public ReadOnlyVisuals ReadOnlyVisual
        {
            set { _grid.ReadOnlyVisual = value; }
            get { return _grid.ReadOnlyVisual; }
        }

        /// <summary>
        /// Sets or gets the border style of the control.
        /// </summary>
        [
        Category("Appearance"),
        Description("Border style that can be 3D (2 pixels), flat (1 pixel) or none.")
        ]
        public BorderStyle BorderStyle
        {
            get
            {
                return _borderStyle;
            }
            set
            {
                BeginUpdate();
                _borderStyle = value;
                RecreateHandle();
                EndUpdate();
            }
        }

        /// <summary>
        /// Sets or gets the main font of the PropertyGrid.
        /// </summary>
        /// <remarks>This font is used in every text displayed by the control except for properties where
        /// a specific font has been assigned.</remarks>
        public override Font Font
        {
            get { return base.Font; }
            set
            {
                base.Font = value;
                _grid.Font = value;
            }
        }

        /// <summary>
        /// Sets the width ratio between the label and value columns.
        /// </summary>
        /// <remarks>This value is a percentage and must be comprised between 0.0 and 1.0</remarks>
        [Category("Appearance")]
        [DefaultValue(0.5)]
        public double LabelColumnWidthRatio
        {
            set { _grid.LabelColumnWidthRatio = value; }
            get { return _grid.LabelColumnWidthRatio; }
        }

        /// <summary>
        /// Returns the width of the label column.
        /// </summary>
        /// <remarks>This number is calculated based on the <see cref="LabelColumnWidthRatio"/> property.</remarks>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public int LabelColumnWidth
        {
            get { return _grid.LabelColumnWidth; }
        }

        /// <summary>
        /// Gets or sets the default PropertyFeel that will be attached to properties that are not explicitely
        /// linked to a feel.
        /// </summary>
        /// <remarks><para>The default feel is applied when:</para>
        /// <list type="bullet">
        /// <item><description>You don't attach a feel to the property with a <see cref="PropertyFeelAttribute"/>.</description></item>
        /// <item><description>You don't attach a feel to the property with a PropertyFeelAttribute on its parent.</description></item>
        /// <item><description>You don't define a registered feel for the property type (see <see cref="RegisterFeelAttachment"/>).</description></item>
        /// <item><description>There is no built-in registered feel for the property type.</description></item>
        /// </list>
        /// <para>You can set the feel to null. In this case, no inplace control will be shown, i.e. the property value
        /// will be displayed as a static text.</para></remarks>
        /// <example>
        /// <code>
        /// // Almost all my properties have a textbox
        /// this.DefaultFeel = this.GetRegisteredFeel(PropertyGrid.FeelEdit);
        /// </code>
        /// </example>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public PropertyFeel DefaultFeel
        {
            set { _defaultFeel = value; }
            get { return _defaultFeel; }
        }

        /// <summary>
        /// This boolean variable is set to true when the user clicks in the value part while an inplace control is not
        /// shown for this value. This variable is then used by Textbox type inplace controls to know if they must
        /// select the whole text or reset the caret at the position 0.
        /// </summary>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public bool ClickThroughInProgress
        {
            get { return _clickThroughInProgress; }
            set { _clickThroughInProgress = value; }
        }

        /// <summary>
        /// Gets the basic property height in pixels.
        /// </summary>
        /// <remarks>The basic property height is simply the whole height of a property which has only one line
        /// of text. Properties with several lines have a height that is a multiple of this value. It is
        /// calculated based on the font that has been assigned to the control. A margin is added to the font
        /// height and can be changed with the <see cref="PropertyVerticalMargin"/> property.
        /// <para>The default value is 3 to mimic the Microsoft PropertyGrid. 4 is also a good choice. 2 could
        /// be used for a kind of ToolWindow.</para>
        /// </remarks>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public int BasicPropertyHeight
        {
            get { return _grid.BasicPropertyHeight; }
        }

        /// <summary>
        /// Sets or gets the display mode of the grid.
        /// </summary>
        /// <remarks>
        /// The grid can be displayed in three ways:
        /// <list type="bullet">
        /// <item><term>Categorized</term><description>This is the usual mode where all properties are
        /// categorized in categories and subcategories.</description></item>
        /// <item><term>Flat</term><description>All properties are displayed linearly in their order of
        /// appearance under the categorized mode.</description></item>
        /// <item><term>FlatSorted</term><description>All properties are displayed linearly in alphabetical order.</description></item>
        /// </list>
        /// The optional toolbar allows to switch between Categorized and FlatSorted mode. If you want to set the
        /// Flat mode, you have to do it programatically or by customizing the toolbar.
        /// </remarks>
        [Category("Appearance")]
        [DefaultValue(PropertyGrid.DisplayModes.Categorized)]
        public DisplayModes DisplayMode
        {
            set
            {
                _grid.DisplayMode = value;
                _reportedDisplayMode = value;

                if (_toolbar != null)
                {
#if _DOTNET2
					((ToolStripButton)_toolbar.Items[0]).Checked = (value == DisplayModes.Categorized);
					((ToolStripButton)_toolbar.Items[1]).Checked = (value == DisplayModes.FlatSorted);
#else
					(_toolbar.Buttons[0]).Pushed = (value == DisplayModes.Categorized);
					(_toolbar.Buttons[1]).Pushed = (value == DisplayModes.FlatSorted);
#endif
                }
            }

            get { return _reportedDisplayMode; }
        }

        /// <exclude />
        /// <summary>
        /// TBD
        /// </summary>
        [Category("Behavior")]
        [DefaultValue(PropertyGrid.MultiSelectModes.None)]
        public MultiSelectModes MultiSelectMode
        {
            set { _grid.MultiSelectMode = value; }
            get { return _grid.MultiSelectMode; }
        }

        [Category("Behavior")]
        [DefaultValue(PropertyGrid.ValueNotValidBehaviorModes.KeepFocus)]
        public ValueNotValidBehaviorModes ValueNotValidBehaviorMode
        {
            set { _valueNotValidBehaviorMode = value; }
            get { return _valueNotValidBehaviorMode; }
        }

        /// <summary>
        /// Sets or gets the space in pixels between the text of a property and the boundaries of its row.
        /// </summary>
        [Category("Appearance")]
        [DefaultValue(4)]
        public int PropertyVerticalMargin
        {
            set
            {
                _grid.PropertyVerticalMargin = value;

                // TODO : faire kekchose si inplace control visible
            }

            get { return _grid.PropertyVerticalMargin; }
        }

        /// <summary>
        /// Represents the margin used all over the control to separate texts from the horizontal and vertical borders.
        /// </summary>
        /// <remarks>
        /// <para>Contrarily to the Microsoft PropertyGrid, SPG tries to offer an overall pleasant and balanced
        /// appearance. That's why instead of displaying texts and various items too close to the left and
        /// right borders, the natural margin of a TextBox is used to calculate a relevant spacing. Each time
        /// the font of the control is changed, this value is recalculated.</para>
        /// <para>When a secondary font is applied to only one property, this is still the global font that is used
        /// to calculate the margin.</para>
        /// <para>When you develop a custom <see cref="PropertyLook"/>, a <see cref="PropertyFeel"/> or a
        /// <see cref="CustomDrawManager"/>, use this value for homogeneity.</para>
        /// </remarks>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public int GlobalTextMargin
        {
            get { return _globalTextMargin; }
        }

        internal void SetGlobalTextMargin(int value)
        {
            _globalTextMargin = value;
        }

        /*
            // Code to generate the byte arrays from the images
            ResourceManager resourceManager = new ResourceManager("VisualHint.SmartPropertyGrid.Toolbar", Assembly.GetExecutingAssembly());
            ToolStripButton button = new ToolStripButton();
            button.Image = (Bitmap)resourceManager.GetObject("bycategories");

            MemoryStream ms = new MemoryStream();
            button.Image.Save(ms, ImageFormat.Bmp);
            byte[] toto = ms.ToArray();
            StreamWriter sw = new StreamWriter(path);
            foreach(byte b in toto)
            sw.Write(b.ToString() + ", ");
            sw.Close();
        */
        byte[] byCategoriesBmp = { 66, 77, 54, 3, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, 40, 0, 0, 0, 16, 0, 0, 0,
            16, 0, 0, 0, 1, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 11, 0, 0, 18, 11, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 255, 0, 255, 181, 152, 120, 181, 152, 120, 181, 152, 120, 181, 152, 120, 181, 152, 120, 255,
            0, 255, 255, 0, 255, 199, 177, 165, 191, 166, 153, 180, 153, 139, 169, 142, 126, 255, 0, 255, 191,
            166, 153, 180, 153, 139, 169, 142, 126, 181, 152, 120, 191, 204, 210, 174, 190, 198, 168, 184,
            194, 167, 184, 193, 167, 184, 193, 181, 152, 120, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0,
            255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 181, 152, 120, 217, 225,
            228, 207, 216, 220, 0, 0, 0, 201, 211, 216, 198, 209, 214, 181, 152, 120, 255, 0, 255, 118, 118,
            118, 100, 99, 99, 81, 82, 81, 64, 65, 64, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 181,
            152, 120, 238, 242, 242, 0, 0, 0, 0, 0, 0, 0, 0, 0, 227, 233, 234, 181, 152, 120, 255, 0, 255,
            255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0,
            255, 181, 152, 120, 241, 245, 245, 241, 245, 245, 0, 0, 0, 241, 245, 245, 241, 245, 245, 181, 152,
            120, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255,
            255, 0, 255, 255, 0, 255, 181, 152, 120, 245, 247, 247, 245, 247, 247, 244, 247, 247, 244, 247,
            247, 244, 246, 246, 181, 152, 120, 255, 0, 255, 198, 177, 165, 190, 166, 152, 180, 153, 138, 170,
            142, 127, 255, 0, 255, 190, 166, 152, 180, 153, 138, 170, 142, 127, 255, 0, 255, 181, 152, 120,
            181, 152, 120, 181, 152, 120, 181, 152, 120, 181, 152, 120, 255, 0, 255, 255, 0, 255, 255, 0, 255,
            255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0,
            255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255,
            199, 178, 165, 191, 166, 153, 179, 153, 139, 169, 142, 126, 255, 0, 255, 191, 166, 153, 179, 153,
            139, 169, 142, 126, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255,
            255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255,
            0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 181, 152, 120, 181, 152, 120, 181, 152, 120, 181,
            152, 120, 181, 152, 120, 255, 0, 255, 255, 0, 255, 199, 177, 166, 190, 166, 153, 179, 153, 138,
            170, 142, 126, 255, 0, 255, 190, 166, 153, 179, 153, 138, 170, 142, 126, 181, 152, 120, 191, 204,
            210, 174, 190, 198, 168, 184, 194, 167, 184, 193, 167, 184, 193, 181, 152, 120, 255, 0, 255, 255,
            0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0,
            255, 181, 152, 120, 217, 225, 228, 207, 216, 220, 0, 0, 0, 201, 211, 216, 198, 209, 214, 181, 152,
            120, 255, 0, 255, 199, 177, 165, 191, 166, 153, 180, 153, 139, 169, 142, 126, 255, 0, 255, 191,
            166, 153, 180, 153, 139, 169, 142, 126, 181, 152, 120, 238, 242, 242, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            227, 233, 234, 181, 152, 120, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255,
            255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 181, 152, 120, 241, 245, 245, 241, 245, 245,
            0, 0, 0, 241, 245, 245, 241, 245, 245, 181, 152, 120, 255, 0, 255, 118, 118, 118, 100, 99, 99, 81,
            82, 81, 64, 65, 64, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 181, 152, 120, 245, 247,
            247, 245, 247, 247, 244, 247, 247, 244, 247, 247, 244, 246, 246, 181, 152, 120, 255, 0, 255, 255,
            0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255,
            255, 0, 255, 181, 152, 120, 181, 152, 120, 181, 152, 120, 181, 152, 120, 181, 152, 120, 255, 0,
            255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255,
            255, 0, 255, 255, 0, 255 };

        byte[] bySortedNamesBmp = { 66, 77, 54, 3, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, 40, 0, 0, 0, 16, 0, 0, 0,
            16, 0, 0, 0, 1, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 11, 0, 0, 18, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255,
            0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 0, 255, 255, 0,
            255, 255, 0, 255, 255, 0, 255, 50, 60, 141, 43, 52, 124, 123, 128, 176, 112, 119, 173, 54, 62,
            132, 120, 126, 174, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 132, 132,
            132, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 173, 162, 198, 62, 63, 143, 170, 174,
            208, 255, 0, 255, 255, 0, 255, 116, 123, 179, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255,
            226, 226, 226, 0, 0, 0, 218, 218, 218, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 131,
            111, 162, 60, 70, 149, 216, 211, 229, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0,
            255, 255, 255, 255, 22, 22, 22, 0, 0, 0, 10, 10, 10, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255,
            0, 255, 216, 211, 229, 77, 71, 146, 110, 116, 169, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0,
            255, 255, 0, 255, 224, 224, 224, 0, 0, 0, 0, 0, 0, 0, 0, 0, 224, 224, 224, 255, 0, 255, 255, 0,
            255, 255, 0, 255, 255, 0, 255, 159, 151, 194, 37, 47, 126, 182, 185, 212, 255, 0, 255, 255, 0,
            255, 255, 0, 255, 255, 255, 255, 126, 126, 126, 180, 180, 180, 0, 0, 0, 186, 186, 186, 117, 117,
            117, 255, 255, 255, 255, 0, 255, 131, 125, 178, 255, 0, 255, 255, 0, 255, 92, 89, 159, 65, 73,
            142, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 255, 0, 255, 255, 255, 255, 0, 0, 0,
            255, 255, 255, 255, 0, 255, 255, 255, 255, 255, 0, 255, 157, 142, 183, 146, 132, 178, 168, 154,
            191, 151, 135, 178, 109, 104, 165, 171, 175, 210, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0,
            255, 255, 255, 255, 0, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255,
            255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0,
            255, 255, 0, 255, 255, 255, 255, 0, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 198, 139, 110,
            148, 90, 60, 213, 189, 177, 255, 0, 255, 144, 90, 64, 136, 79, 51, 157, 111, 88, 255, 0, 255, 255,
            0, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 0, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255,
            255, 0, 255, 187, 114, 79, 255, 0, 255, 255, 0, 255, 224, 205, 195, 136, 79, 51, 238, 228, 222,
            255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 0, 0, 0, 255, 255, 255, 255, 0,
            255, 255, 0, 255, 255, 0, 255, 198, 137, 107, 186, 123, 93, 191, 156, 137, 144, 89, 61, 173, 127,
            104, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 0, 0, 0, 255,
            255, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 239, 218, 207, 185, 112, 75, 255, 0, 255, 137,
            80, 51, 223, 203, 193, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 255,
            255, 0, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 194, 128, 94,
            184, 120, 89, 150, 98, 69, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255,
            0, 255, 255, 255, 255, 0, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255,
            214, 161, 134, 183, 107, 69, 207, 178, 165, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255,
            255, 0, 255, 255, 0, 255, 255, 255, 255, 0, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0, 255, 255, 0,
            255, 255, 0, 255, 255, 0, 255, 194, 118, 79, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255,
            255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 255, 255, 0, 0, 0, 255, 255, 255, 255, 0, 255, 255, 0,
            255 };

        /// <summary>
        /// Sets or gets a boolean that controls the visibility of the top toolbar.
        /// </summary>
        /// <remarks>
        /// <para>By default, the toolbar contains two buttons and a separator: the first button sorts the properties
        /// by category. Inside categories, properties are displayed in the order they were created. The second sorts
        /// them by alphabetical order.</para>
        /// <para>You can completely customize the content of the toolbar by getting a reference on it with the
        /// <see cref="Toolbar"/> property.</para></remarks>
        [Category("Appearance")]
        [DefaultValue(false)]
        public bool ToolbarVisibility
        {
            set
            {
				BeginUpdate();
				
				if (value && (_toolbar == null))
                {
#if _DOTNET2
                    _toolbar = new ToolStrip();
                    _toolbar.GripStyle = ToolStripGripStyle.Hidden;
                    _toolbar.ShowItemToolTips = true;
                    _toolbar.TabStop = true;
                    _toolbar.Text = "PropertyGridToolBar";
                    _toolbar.AutoSize = false;
                    _toolbar.TabIndex = 1;
                    _toolbar.CanOverflow = false;
                    _toolbar.GripStyle = ToolStripGripStyle.Hidden;
                    _toolbar.Cursor = Cursors.Arrow;
                    Padding padding = _toolbar.Padding;
                    padding.Left = 2;
                    _toolbar.Padding = padding;
                    _toolbar.Renderer = new ToolStripSystemRenderer();
                    //                    ProfessionalColorTable table = new ProfessionalColorTable();
                      //                  table.UseSystemColors = true;
                        //                _toolbar.Renderer = new ToolStripProfessionalRenderer(table);
                    Controls.Add(_toolbar);

                    ToolStripButton button = new ToolStripButton();
                    MemoryStream ms = new MemoryStream(byCategoriesBmp);
                    button.Image = Bitmap.FromStream(ms);

                    ResourceManager systemRM = new ResourceManager("System.Windows.Forms", Assembly.GetAssembly(typeof(Form)));

                    button.ImageTransparentColor = Color.Magenta;
                    button.ToolTipText = systemRM.GetString("PBRSToolTipCategorized");
                    button.Click += new EventHandler(OnSortByCategoy);
                    button.Checked = (DisplayMode == DisplayModes.Categorized);
                    _toolbar.Items.Add(button);

                    button = new ToolStripButton();
                    ms = new MemoryStream(bySortedNamesBmp);
                    button.Image = Bitmap.FromStream(ms);

                    button.ImageTransparentColor = Color.Magenta;
                    button.ToolTipText = systemRM.GetString("PBRSToolTipAlphabetic");
                    button.Click += new EventHandler(OnSortAlphabetical);
                    button.Checked = (DisplayMode == DisplayModes.FlatSorted);
                    _toolbar.Items.Add(button);

                    ToolStripSeparator sep = new ToolStripSeparator();
                    _toolbar.Items.Add(sep);
#else
                    _toolbar = new ToolBar();

					_toolbar.BackColor = BackColor;
                    _toolbar.TabStop = true;
                    _toolbar.Text = "PropertyGridToolBar";
                    _toolbar.Dock = DockStyle.None;
                    _toolbar.AutoSize = true;
                    _toolbar.TabIndex = 1;
                    _toolbar.Cursor = Cursors.Arrow;
                    _toolbar.Appearance = ToolBarAppearance.Flat;
                    _toolbar.Divider = false;
                    _toolbar.ButtonClick += new ToolBarButtonClickEventHandler(OnToolbarButtonClicked);
					Controls.Add(_toolbar);

					ImageList imageList = new ImageList();
					MemoryStream ms = new MemoryStream(byCategoriesBmp);
					Bitmap image = new Bitmap(Image.FromStream(ms));
					image.MakeTransparent();
					imageList.Images.Add(image);

                    ms = new MemoryStream(bySortedNamesBmp);
					image = new Bitmap(Image.FromStream(ms));
					image.MakeTransparent();
					imageList.Images.Add(image);

					_toolbar.ImageList = imageList;

					ToolBarButton[] buttonArray = new ToolBarButton[2];
					ToolBarButton button = new ToolBarButton();
					button.Style = ToolBarButtonStyle.PushButton;
                    button.ImageIndex = 0;
                    button.ToolTipText = "Categorized";
                    button.Pushed = (DisplayMode == DisplayModes.Categorized);
                    buttonArray[0] = button;

                    button = new ToolBarButton();
                    button.Style = ToolBarButtonStyle.PushButton;
                    button.ImageIndex = 1;
                    button.ToolTipText = "Alphabetic";
                    button.Pushed = (DisplayMode == DisplayModes.FlatSorted);
					buttonArray[1] = button;

                    _toolbar.Buttons.AddRange(buttonArray);
#endif
                    RepositionInternalGrid();
                }
                else if (!value && (_toolbar != null))
                {
                    Controls.Remove(_toolbar);
                    _toolbar.Dispose();
                    _toolbar = null;

                    RepositionInternalGrid();
                }

				EndUpdate();
				Refresh();
            }
            get { return (_toolbar != null); }
        }

        /// <summary>
        /// Gets a reference on the toolbar in order to change its content or appearance.
        /// </summary>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
#if _DOTNET2
        public ToolStrip Toolbar
#else
		public ToolBar Toolbar
#endif
        {
            get { return _toolbar; }
        }

        /// <summary>
        /// Sets or gets a boolean that controls the visibility of the comments area.
        /// </summary>
        /// <remarks>To set the height of the comments area, use the <see cref="CommentsHeight"/> property.
        /// </remarks>
        [Category("Appearance")]
        [DefaultValue(false)]
        public bool CommentsVisibility
        {
            set
            {
                _showComments = value;
                Invalidate();
                RepositionInternalGrid();
            }

            get { return _showComments; }
        }

        /// <summary>
        /// Sets or gets the height in pixels of the comments area.
        /// </summary>
        /// <remarks><para>You can also get its client rectangle by using the <see cref="CommentsRect"/> property.</para>
        /// <para>To show/hide the comments area, use the <see cref="CommentsVisibility"/> property.</para>
        /// </remarks>
        [Category("Appearance")]
        [DefaultValue(40)]
        public int CommentsHeight
        {
            set
            {
                if ((value >= MinCommentsAreaHeight) && ((ClientRectangle.Height == 0) || (value <= ClientRectangle.Height - 2 * BasicPropertyHeight - ToolbarHeight)))
                {
                    _commentsHeight = value;
                    Invalidate();
                    RepositionInternalGrid();
                }
            }
            get { return _commentsHeight; }
        }

        /// <summary>
        /// Gets the client rectangle of the comments area.
        /// </summary>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public Rectangle CommentsRect
        {
            get
            {
                Rectangle clientRect = ClientRectangle;
                clientRect.Y = clientRect.Y + clientRect.Height - CommentsHeight;
                clientRect.Height = CommentsHeight;

                return clientRect;
            }
        }

        /// <summary>
        /// Allows to prevent the user from resizing the comments area.
        /// </summary>
        [Category("Behavior")]
        [DefaultValue(false)]
        public bool CommentsLock
        {
            set { _commentsLock = value; }
            get { return _commentsLock; }
        }

        /// <summary>
        /// Sets or gets the height in pixels of the comments area.
        /// </summary>
        /// <remarks>Contrarily to <see cref="CommentsHeight"/>, it returns 0 if the comments area is not visible.</remarks>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        private int CurrentCommentsHeight
        {
            get { return (CommentsVisibility ? _commentsHeight : 0); }
        }

        /// <summary>
        /// Allows to prevent the user from resizing the label and value columns.
        /// </summary>
        [Category("Behavior")]
        [DefaultValue(false)]
        public bool ColumnsLock
        {
            set { _grid.ColumnsLock = value; }
            get { return _grid.ColumnsLock; }
        }

        /// <summary>
        /// Returns true if the application is active and the PropertyGrid or one of its descendants contains
        /// the focus.
        /// </summary>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public bool GridContainsFocus
        {
            get
            {
                Control topLevelWindow = FromHandle(Win32Calls.GetForegroundWindow());
                return ((topLevelWindow != null) && Grid.ContainsFocus);
            }
        }

        /// <summary>
        /// Sets or gets the drawing manager attached to the PropertyGrid.
        /// </summary>
        /// <remarks>
        /// A drawing manager is a class that takes care of applying a consistent look to the whole PropertyGrid.
        /// It is possible to use the three buil-in managers. They are: <see cref="DefaultDrawManager"/>,
        /// <see cref="DotnetDrawManager"/> and <see cref="LightColorDrawManager"/>. You can also create your
        /// own one by deriving a class from one of them or from their ancestor <see cref="CustomDrawManager"/>.
        /// </remarks>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public CustomDrawManager DrawManager
        {
            get { return _grid.DrawManager; }

            set
            {
                _grid.DrawManager = value;
                OnDrawingManagerChanged(EventArgs.Empty);
                Refresh();
            }
        }

        /// <summary>
        /// Sets or gets the built-in drawing manager attached to the PropertyGrid.
        /// </summary>
        /// <remarks>
        /// When dealing with a built-in drawing manager, use this property instead of using the <see cref="DrawManager"/>
        /// property.<para>If the drawing manager is a custom one created by you, <see cref="DrawManagers.CustomDrawManager">CustomDrawManager</see> is returned
        /// by this property.</para>
        /// </remarks>
        [DefaultValue(DrawManagers.DotnetDrawManager)]
        [Category("Appearance")]
        public DrawManagers DrawingManager
        {
            set
            {
                if (value == DrawManagers.CustomDrawManager)
                    return;

                if (value == DrawManagers.DotnetDrawManager)
                    DrawManager = new DotnetDrawManager();
                else if (value == DrawManagers.LightColorDrawManager)
                    DrawManager = new LightColorDrawManager();
                else
                    DrawManager = new DefaultDrawManager();
            }

            get
            {
                if (DrawManager.GetType() == typeof(DotnetDrawManager))
                    return DrawManagers.DotnetDrawManager;
                else if (DrawManager.GetType() == typeof(LightColorDrawManager))
                    return DrawManagers.LightColorDrawManager;
                else if (DrawManager.GetType() == typeof(DefaultDrawManager))
                    return DrawManagers.DefaultDrawManager;

                return DrawManagers.CustomDrawManager;
            }
        }

        /// <summary>
        /// Returns a reference to the internal child control used to manage the grid.
        /// </summary>
        /// <remarks>
        /// The PropertyGrid control uses an internal child control to display the grid itself. Its API is not public
        /// but it is still possible to access it as a Control class for various reasons.
        /// </remarks>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public Control InternalGrid
        {
            get { return _grid; }
        }

        internal InternalGrid Grid
        {
            get { return _grid; }
        }

        /// <summary>
        /// Gets a reference to the currently active inplace control.
        /// </summary>
        /// <remarks>
        /// The reference is returned as a <see cref="Control"/>. You can cast it to the right type of inplace
        /// control if you know precisely which one it is and then you can customize it by calling methods and
        /// properties on it.
        /// </remarks>
        [Browsable(false)]
        public Control InPlaceControl
        {
            get { return _grid.InPlaceControl; }
        }

        /// <summary>
        /// Set or get the way the user can navigate from property to property using the keyboard.
        /// </summary>
        /// <remarks>Each mode is explained in the description of the <see cref="NavigationKeyModes"/> enumeration.
        /// </remarks>
        [Category("Behavior")]
        [DefaultValue(PropertyGrid.NavigationKeyModes.ArrowKeys)]
        public PropertyGrid.NavigationKeyModes NavigationKeyMode
        {
            get { return _navigationKeyMode; }
            set { _navigationKeyMode = value; }
        }

        [Category("Behavior")]
        [DefaultValue(PropertyGrid.TabKeyNavigationModes.NoSubMode)]
        public PropertyGrid.TabKeyNavigationModes TabKeyNavigationMode
        {
            get { return _tabKeyNavigationMode; }
            set { _tabKeyNavigationMode = value; }
        }

        /// <summary>
        /// Sets of gets the color used as the background of a property when this property is selected by the user.
        /// </summary>
        [
        Category("Appearance"),
        DefaultValue(typeof(Color), "Highlight")
        ]
        public Color SelectedBackColor
        {
            set { _grid.SelectedBackColor = value; }
            get { return _grid.SelectedBackColor; }
        }
        
        /// <summary>
        /// Sets of gets the color used as the background of a property when this property is selected by the user
        /// and the grid has not the focus.
        /// </summary>
        [
        Category("Appearance"),
        DefaultValue(typeof(Color), "ActiveBorder")
        ]
        public Color SelectedNotFocusedBackColor
        {
            set { _grid.SelectedNotFocusedBackColor = value; }
            get { return _grid.SelectedNotFocusedBackColor; }
        }

        /// <summary>
        /// Sets of gets the color used as the text color of a property when this property is selected by the user.
        /// </summary>
        [
        Category("Appearance"),
        DefaultValue(typeof(Color), "HighlightText")
        ]
        public Color HighlightedTextColor
        {
            set { _grid.HighlightedTextColor = value; }
            get { return _grid.HighlightedTextColor; }
        }

        private int ToolbarHeight
        {
            get
            {
                if (_toolbar == null)
                    return 0;
                else
                    return _toolbar.Height;
            }
        }

        /// <summary>
        /// Gets an enumerator on the currently selected property.
        /// </summary>
        /// <remarks>When there is no selection, <see cref="PropertyEnumerator.RightBound">RightBound</see> is returned.</remarks>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public PropertyVisibleDeepEnumerator SelectedPropertyEnumerator
        {
            get { return _grid.SelectedPropertyEnumerator; }
        }

        /// <summary>
        /// Sets or gets the imagelist containing icons placed in front of category or property labels.
        /// </summary>
        /// <remarks>To associate one of the icons in the imagelist to a property or category, just pass its index
        /// (0 based) to the methods that add manually a property or category (eg:
        /// <see href="/VisualHint.SmartPropertyGrid.PropertyGrid.AppendProperty_overloads.html">AppendProperty</see>).
        /// <note type="caution">You have to create the ImageList yourself with new.</note>
        /// </remarks>
        /// <example>
        /// <code>
        /// this.ImageList = new ImageList();
        /// this.ImageList.Images.Add(myImage);
        /// </code>
        /// </example>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public ImageList ImageList
        {
            set { _grid.ImageList = value; }
            get { return _grid.ImageList; }
        }

        /// <summary>
        /// Gets an enumerator pointing after the last element of the PropertyGrid.
        /// </summary>
        /// <remarks>The enumerator returned relates to the current display of the grid, i.e. if you apply
        /// <see cref="PropertyEnumerator.MovePrev"/> on the enumerator returned it will usually be different
        /// regarding the display mode (categorized or sorted).</remarks>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public PropertyEnumerator RightBound
        {
            get { return _grid.RightBound; }
        }

        /// <summary>
        /// Gets an enumerator pointing before the first property of the PropertyGrid.
        /// </summary>
        /// <remarks>The enumerator returned relates to the current display of the grid, i.e. if you apply
        /// <see cref="PropertyEnumerator.MoveNext"/> on the enumerator returned it will usually be different
        /// regarding the display mode (categorized or sorted).</remarks>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public PropertyEnumerator LeftBound
        {
            get { return _grid.LeftBound; }
        }

        /// <summary>
        /// Gets an enumerator pointing on the first property of the PropertyGrid.
        /// </summary>
        /// <remarks>The enumerator returned is a deep enumerator (see <see cref="PropertyDeepEnumerator"/>). If you want
        /// to browse only visible properties, use the <see cref="FirstDisplayedProperty"/> method.
        /// <para>The enumerator returned relates to the current display of the grid, i.e. it will usually
        /// be different regarding the display mode (categorized or sorted).</para>
        /// </remarks>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public PropertyEnumerator FirstProperty
        {
            get { return _grid.FirstProperty; }
        }

        /// <summary>
        /// Gets an enumerator pointing on the last property of the PropertyGrid.
        /// </summary>
        /// <remarks>The enumerator returned relates to the current display of the grid, i.e. it will usually
        /// be different regarding the display mode (categorized or sorted).</remarks>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public PropertyEnumerator LastProperty
        {
            get { return _grid.LastProperty; }
        }

        /// <summary>
        /// Gets an enumerator pointing on the first property that is visible in the control when it is
        /// scrolled to the top.
        /// </summary>
        /// <remarks>The enumerator returned relates to the current display of the grid, i.e. it will usually
        /// be different regarding the display mode (categorized or sorted).</remarks>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public PropertyEnumerator FirstDisplayedProperty
        {
            get { return _grid.FirstDisplayedProperty; }
        }

        private class SelectedObjectConverter : ReferenceConverter
        {
            public SelectedObjectConverter() : base(typeof(IComponent)) { }
        }

        /// <summary>
        /// This method saves in a hastable the states (expanded / selected) of all the properties in the grid.
        /// </summary>
        /// <remarks>Call this method, then <see cref="SelectedObject"/> or <see cref="SelectedObjects"/>, then
        /// <see cref="RestorePropertiesStates(object)"/>. This is very convenient when selecting a new target instance that
        /// has a lot of resembling properties with the previous one.
        /// </remarks>
        /// <returns>The hashtable of all saved states.</returns>
        public object SavePropertiesStates()
        {
            return SavePropertiesStates(FirstProperty);
        }

        /// <summary>
        /// This method saves in a hastable the states (expanded / selected) of some properties in the grid.
        /// </summary>
        /// <param name="propEnum">A reference to the property on which to begin the backup.</param>
        /// <remarks>Call this method, then <see cref="SelectedObject"/> or <see cref="SelectedObjects"/>, then
        /// <see cref="RestorePropertiesStates(object)"/>. This is very convenient when selecting a new target instance that
        /// has a lot of resembling properties with the previous one.
        /// </remarks>
        /// <returns>The hashtable of all saved states.</returns>
        public object SavePropertiesStates(PropertyEnumerator propEnum)
        {
            return _grid.SavePropertiesStates(propEnum);
        }

        /// <summary>
        /// This method restores some previously saved states (expanded / selected) of all the properties in the grid.
        /// </summary>
        /// <param name="states">The object returned by <see cref="SavePropertiesStates()"/>.</param>
        /// <remarks>Call <see cref="SavePropertiesStates()"/>, then <see cref="SelectedObject"/> or
        /// <see cref="SelectedObjects"/>, then this method. This is very convenient when selecting a new target
        /// instance that has a lot of resembling properties with the previous one.
        /// </remarks>
        public void RestorePropertiesStates(object states)
        {
            RestorePropertiesStates(FirstProperty, states);
        }

        /// <summary>
        /// This method restores some previously saved states (expanded / selected) of some properties in the grid.
        /// </summary>
        /// <param name="propEnum">A reference to the property on which to begin the restoration.</param>
        /// <param name="propertiesStates">The object returned by <see cref="SavePropertiesStates()"/>.</param>
        /// <remarks>Call <see cref="SavePropertiesStates()"/>, then <see cref="SelectedObject"/> or
        /// <see cref="SelectedObjects"/>, then this method. This is very convenient when selecting a new target
        /// instance that has a lot of resembling properties with the previous one.
        /// </remarks>
        public void RestorePropertiesStates(PropertyEnumerator propEnum, object propertiesStates)
        {
            if (!(propertiesStates is PropertiesStates))
                throw new ArgumentException("The parameter must have been returned by SavePropertiesStates.");

            _grid.RestorePropertiesStates(propEnum, propertiesStates);
        }

        /// <summary>
        /// Takes an instance of an object and displays its properties the same way microsoft does.
        /// </summary>
        /// <value>A reference to the object to be displayed in the grid.</value>
        /// <remarks>
        /// Smart PropertyGrid is first of all a PropertyGrid whose content can be set dynamically. This is what
        /// makes its strength. But for the ease of integration in your application where you were maybe using the
        /// Microsoft grid previously, SelectedObject has been kept.
        /// <para>This is less easy to customize the grid when using SelectedObject but it can be done by
        /// using specific attributes on the properties when you have access to the source code of the target
        /// instance.</para>
        /// </remarks>
        [Category("Behavior")]
        [TypeConverter(typeof(SelectedObjectConverter))]
        [DefaultValue((string)null)]
        public object SelectedObject
        {
            set
            {
                if (value == null)
                    SelectedObjects = new object[0];
                else
                    SelectedObjects = new object[] { value };
            }

            get
            {
                if ((_selectedObjects != null) && (_selectedObjects.Length != 0))
                    return _selectedObjects[0];
                return null;
            }
        }

        private bool _insideSelectedObjects = false;

        internal bool IsInsideSelectedObjects
        {
            get { return _insideSelectedObjects; }
        }

        /// <summary>
        /// Takes an array of instances and displays their properties the same way microsoft does.
        /// </summary>
        /// <value>A reference to the array of objects to be displayed in the grid.</value>
        /// <remarks>
        /// Smart PropertyGrid is first of all a PropertyGrid whose content can be set dynamically. This is what
        /// makes its strength. But for the ease of integration in your application where you were maybe using the
        /// Microsoft grid previously, SelectedObject has been kept.
        /// <para>This is less easy to customize the grid when using SelectedObjects but it can be done by
        /// using specific attributes on the properties when you have access to the source code of the target
        /// instances.</para>
        /// </remarks>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public object[] SelectedObjects
        {
            set
            {
                _insideSelectedObjects = true;

                BeginUpdate();

                if (value == null)
                    _selectedObjects = new object[0];
                else
                    _selectedObjects = (object[])value.Clone();

                Clear();
                _categoryMap.Clear();
                _categoryId = 1;
                _propertyId = 1;

                // Set the grid in categorized mode
                DisplayModes dispMode = DisplayMode;
                if (dispMode != DisplayModes.Categorized)
                    _grid.DisplayMode = DisplayModes.Categorized;

                if ((_selectedObjects != null) && (_selectedObjects.Length != 0))
                {
                    ArrayList collection = new ArrayList(); // <List<PropertyDescriptor>>

                    // If there is only one object, get the PropertyDescriptors and sort them
                    if (_selectedObjects.Length == 1)
                    {
                        bool toBeSorted;
                        PropertyDescriptorCollection pdCollection = GetPropertyDescriptors(_selectedObjects[0], null, out toBeSorted);
                        if (toBeSorted)
                            pdCollection = pdCollection.Sort(_propertyComparer);

                        foreach (PropertyDescriptor pd in pdCollection)
                        {
                            ArrayList pdList = new ArrayList(1);    // <PropertyDescriptor>
                            pdList.Add(pd);
                            collection.Add(pdList);
                        }
                    }

                    // If there are several target instances, grab the common PropertyDescriptors
                    else
                    {
                        Hashtable propertyDescriptorDefs = new Hashtable(); // <PropertyDescriptor, List<PropertyDescriptor>>

                        bool firstTarget = true;
                        bool firstToBeSorted = true;
                        foreach (object targetInstance in _selectedObjects)
                        {
                            bool toBeSorted;
                            PropertyDescriptorCollection pdCollection = GetPropertyDescriptors(targetInstance, null, out toBeSorted);
                            if (firstTarget)
                                firstToBeSorted = toBeSorted;

                            foreach (PropertyDescriptor propertyDescriptor in pdCollection)
                            {
                                MergablePropertyAttribute mergeAttr = propertyDescriptor.Attributes[typeof(MergablePropertyAttribute)]
                                    as MergablePropertyAttribute;
                                if ((mergeAttr != null) && (mergeAttr.AllowMerge == false))
                                    continue;

                                if (firstTarget)
                                {
                                    // For the first target instance, we store all the PropertyDescriptors
                                    ArrayList pdList = new ArrayList();
                                    pdList.Add(propertyDescriptor);

                                    propertyDescriptorDefs.Add(propertyDescriptor, pdList);
                                }
                                else
                                {
                                    // For all the remaining target instances, we compare the PropertyDescriptors
                                    // to the first set of stored PropertyDescriptors. Their name and property type must
                                    // correspond
                                    ArrayList existPdList = (ArrayList)propertyDescriptorDefs[propertyDescriptor];
                                    if (existPdList != null)
                                    {
                                        // If the propertyDesriptors match, we increment a count
                                        if (MatchPropertyDescriptors((PropertyDescriptor)existPdList[0], propertyDescriptor))
                                            existPdList.Add(propertyDescriptor);
                                        // else the PropertyDescriptor is not eligible and is removed
                                        else
                                            propertyDescriptorDefs.Remove(propertyDescriptor);
                                    }
                                }
                            }

                            firstTarget = false;
                        }

                        // Ensure that we count only propertyDescriptors that are present in all target instances
                        foreach (ArrayList pdList in propertyDescriptorDefs.Values)
                        {
                            if (pdList.Count == _selectedObjects.Length)
                                collection.Add(pdList);
                        }

                        // Sort the collection with a bubble sort
                        if (firstToBeSorted)
                        {
                            for (int j = collection.Count - 1; j > 0; j--)
                            {
                                for (int i = 0; i < j; i++)
                                {
                                    if (_propertyComparer.Compare((PropertyDescriptor)((ArrayList)collection[i])[0],
                                        (PropertyDescriptor)((ArrayList)collection[i + 1])[0]) == 1)
                                    {
                                        ArrayList pdList = (ArrayList)collection[i];
                                        collection.RemoveAt(i);
                                        collection.Insert(i + 1, pdList);
                                    }
                                }
                            }
                        }
                    }

                    Hashtable addValueDescriptors = new Hashtable();    // <string, List<KeyValuePair<string, PropertyDescriptor>>>

                    ResourceManager systemRM = new ResourceManager("System", Assembly.GetAssembly(typeof(CategoryAttribute)));
                    string miscCategoryName = (string)systemRM.GetObject("PropertyCategoryDefault", System.Globalization.CultureInfo.CurrentUICulture);

                    ArrayList _addTargetInstancePdList;  // <PropertyDescriptor>

                    foreach (ArrayList pdList in collection)
                    {
                        _addTargetInstancePdList = pdList;

                        int addValueDescriptorsLen = addValueDescriptors.Count;
                        foreach (PropertyDescriptor pd in pdList)
                        {
                            // Check if the descriptor points to a property that has to be added as a value of another property
                            // Right now only ONE additional value per property is supported.
                            ValueAddedToPropertyAttribute addValueAttr = pd.Attributes[typeof(ValueAddedToPropertyAttribute)]
                                as ValueAddedToPropertyAttribute;
                            if (addValueAttr != null)
                            {
                                // Store the descriptor to process it at the end
                                ArrayList pds = (ArrayList)addValueDescriptors[addValueAttr.PropertyName];
                                if (pds == null)
                                {
                                    pds = new ArrayList();
                                    addValueDescriptors.Add(addValueAttr.PropertyName, pds);
                                }
                                pds.Add(new Utilities.KeyValuePair(addValueAttr.ValueKey, pd));
                            }
                            else
                                break;
                        }

                        // No further processing of the current property if it is a secondary value of another property
                        if (addValueDescriptorsLen != addValueDescriptors.Count)
                            continue;

                        PropertyDescriptor propertyDescriptor = (PropertyDescriptor)pdList[0];

                        PropertyEnumerator categoryEnum;

                        // Check if the property belongs to a sorted category
                        SortedCategoryAttribute sortedCategoryAttr = propertyDescriptor.Attributes[typeof(SortedCategoryAttribute)]
                            as SortedCategoryAttribute;

                        if (sortedCategoryAttr != null)
                        {
                            //Find to which category name the property has been assigned
                            string categoryName = sortedCategoryAttr.Category;

                            // Find if the parent category has already been created in the grid
                            categoryEnum = (PropertyEnumerator)_categoryMap[categoryName];
                            if (categoryEnum == null)
                            {
                                // If not, then the category is created as a root one at the right place
                                PropertyEnumerator siblingEnum = FirstProperty.GetSiblingEnumerator();
                                while (siblingEnum != siblingEnum.RightBound)
                                {
                                    int sortedIndex = siblingEnum.Property.SortedIndex;
                                    if ((sortedIndex == int.MaxValue) || (sortedIndex > sortedCategoryAttr.SortedIndex))
                                        break;

                                    siblingEnum.MoveNext();
                                }

                                categoryEnum = InsertRootCategory(siblingEnum, -(_categoryId++), categoryName);
                                categoryEnum.Property.SortedIndexInternal = sortedCategoryAttr.SortedIndex;

                                _categoryMap[categoryName] = categoryEnum;
                            }
                        }
                        else
                        {
                            //Find to which category name the property has been assigned
                            string categoryName = propertyDescriptor.Category;
                            if (categoryName == null)
                                categoryName = miscCategoryName;

                            // Find if the parent category has already been created in the grid
                            categoryEnum = (PropertyEnumerator)_categoryMap[categoryName];
                            if (categoryEnum == null)
                            {
                                // If not, then the category is created as a root one
                                categoryEnum = AppendRootCategory(-(_categoryId++), categoryName);
                                _categoryMap[categoryName] = categoryEnum;
                            }
                        }

                        // Add the new property under its category
                        PropertyIdAttribute attr = propertyDescriptor.Attributes[typeof(PropertyIdAttribute)] as PropertyIdAttribute;
                        int id = (attr != null ? attr.Id : _propertyId++);

                        SortedPropertyAttribute propertySortAttr = propertyDescriptor.Attributes[typeof(SortedPropertyAttribute)]
                            as SortedPropertyAttribute;

                        PropertyEnumerator newPropEnum;
                        if (propertySortAttr == null)
                        {
                            newPropEnum = _grid.AppendProperty(categoryEnum, id, propertyDescriptor.DisplayName,
                                _selectedObjects, _addTargetInstancePdList, propertyDescriptor.Description, null);
                        }
                        else
                        {
                            PropertyEnumerator siblingEnum = categoryEnum.Children;
                            while (siblingEnum != siblingEnum.RightBound)
                            {
                                int sortedIndex = siblingEnum.Property.SortedIndex;
                                if ((sortedIndex == int.MaxValue) || (sortedIndex > propertySortAttr.SortedIndex))
                                    break;

                                siblingEnum.MoveNext();
                            }

                            siblingEnum = _grid.InsertProperty(siblingEnum, id, propertyDescriptor.DisplayName,
                                _selectedObjects, _addTargetInstancePdList, propertyDescriptor.Description, null);
                            siblingEnum.Property.SortedIndexInternal = propertySortAttr.SortedIndex;

                            newPropEnum = siblingEnum;
                        }

                        PropertyPostFilterOutEventArgs e = new PropertyPostFilterOutEventArgs(newPropEnum);
                        OnPropertyPostFilterOut(e);

                        if (e.GetFilterOutInternal())
                        {
                            if (newPropEnum.Count == 1)
                            {
                                _categoryMap.Remove(newPropEnum.Parent.Property.DisplayName);
                                DeleteProperty(newPropEnum.Parent);
                            }
                            else
                                DeleteProperty(newPropEnum);
                        }
                    }

                    // Process the properties that must be added as values of other properties
                    if (addValueDescriptors.Count > 0)
                    {
                        PropertyEnumerator propEnum = FirstProperty;
                        while (propEnum != RightBound)
                        {
                            string name = propEnum.Property.Name;
                            if (name.Length > 0)
                            {
                                ArrayList pdList = (ArrayList)addValueDescriptors[name];
                                if (pdList != null)
                                {
                                    int i = 0;
                                    foreach (Utilities.KeyValuePair pair in pdList)
                                    {
                                        propEnum.Property.AddValue(pair.Key,
                                            _selectedObjects[i++], (PropertyDescriptor)pair.Value, null);
                                    }
                                }
                            }

                            propEnum.MoveNext();
                        }
                    }

                    _grid.FirstDisplayedProperty = FirstProperty.GetVisibleDeepEnumerator();
                }

                // Set the grid in non categorized mode if it was as such before calling SelectedObject(s)
                if (dispMode != DisplayModes.Categorized)
                    _grid.DisplayMode = dispMode;

                EndUpdate();

//                Refresh();

                _insideSelectedObjects = false;

                OnSelectedObjectChanged(EventArgs.Empty);
            }

            get
            {
                if (_selectedObjects == null)
                    return new object[0];

                return (object[])_selectedObjects.Clone();
            }
        }

        /// <summary>
        /// Sets or gets a value that indicates what is the behavior of the grid when it is disabled after setting
        /// the <see cref="Enabled"/> property to false.
        /// </summary>
        /// <remarks>
        /// The following modes are available for all tastes:
        /// <list type="bullet">
        /// <item><term><see cref="DisableModes.None" /></term><description>In this mode, the grid is temporarily made
        /// enabled again. This is the same as calling <see cref="Enabled"/> again with true.</description></item>
        /// <item><term><see cref="DisableModes.Simple" /></term><description>This is the default mode. The whole
        /// grid is manageable but all the textboxes are readonly. Copy in the clipboard is therefore possible.</description></item>
        /// <item><term><see cref="DisableModes.NoValueSelection" /></term><description>This mode is like the Simple
        /// mode except that inplace controls are never shown. Copy in the clipboard is therefore impossible.</description></item>
        /// <item><term><see cref="DisableModes.NoPropertySelection" /></term><description>This mode is like the
        /// NoValueSelection except that this time no property at all can be selected. Nodes can still be expanded
        /// and collapsed and the grid can be scrolled.</description></item>
        /// <item><term><see cref="DisableModes.Full" /></term><description>In full mode, as it is expected, nothing is selectable and
        /// nothing operates.</description></item>
        /// </list>
        /// <para>Use the <see cref="DisableModeGrayedOut"/> to gray out every property when the mode has been set
        /// to something different from None.</para>
        /// </remarks>
        [DefaultValue(PropertyGrid.DisableModes.Simple)]
        [Category("Behavior")]
        public PropertyGrid.DisableModes DisableMode
        {
            set
            {
                if (_grid.DisableMode != value)
                {
                    _grid.DisableMode = value;
                    base.Enabled = (Enabled || (value != DisableModes.Full));
                }
            }
            get { return _grid.DisableMode; }
        }

        /// <summary>
        /// Sets or gets a boolean indicating if everything is grayed out when <see cref="DisableMode"/> has been
        /// set to something other than None.
        /// </summary>
        [DefaultValue(true)]
        [Category("Behavior")]
        public bool DisableModeGrayedOut
        {
            set { _grid.DisableModeGrayedOut = value; }
            get { return _grid.DisableModeGrayedOut; }
        }

        /// <summary>
        /// Sets or gets a boolean that indicates whether the whole grid is enabled or disabled.
        /// </summary>
        /// <remarks>Several ways of being disabled can be set by calling the <see cref="DisableMode"/> property.</remarks>
        [DefaultValue(true)]
        public new bool Enabled
        {
            set
            {
                if (_grid.Enabled != value)
                {
                    _grid.Enabled = value;

                    if (DisableMode == DisableModes.Full)
                        base.Enabled = value;
                }
            }

            get { return _grid.Enabled; }
        }

        protected override Size DefaultSize
        {
            get
            {
                return new Size(150, 200);
            }
        }

        #endregion

        #region Constructors
        /// <summary>
        /// Initializes a new instance of the PropertyGrid class.
        /// </summary>
        public PropertyGrid()
        {
            _grid = new InternalGrid(this);
            _grid.TabIndex = 2;
            _grid.ForeColor = ForeColor;

            _commentsBackColor = BackColor;
            _commentsForeColor = ForeColor;

#if _SPGEVAL
            MinCommentsAreaHeight = 70;
            CommentsHeight = 70;
#else
            MinCommentsAreaHeight = 35;
            CommentsHeight = 40;
#endif

            DrawingManager = DrawManagers.DotnetDrawManager;

            _grid.Font = Font;

#if _DOTNET2
            SetStyle(ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint |
                Utilities.DoubleBufferStyle | ControlStyles.Opaque, true);
#else
			SetStyle(ControlStyles.UserPaint | Utilities.DoubleBufferStyle , true);
#endif
            UpdateStyles();

            _propertyComparer = new PropertyComparerDefaultSort();

            Controls.Add(_grid);

            RegisterFeel(FeelNone, PropertyFeel.Empty);
            RegisterFeel(FeelEdit, new PropertyEditFeel(this, false));
#if _DOTNET2
            RegisterFeel(FeelMaskedEdit, new PropertyMaskedEditFeel(this));
#endif
            RegisterFeel(FeelEditPassword, new PropertyEditFeel(this, true));
            RegisterFeel(FeelMultilineEdit, new PropertyMultilineEditFeel(this));
            RegisterFeel(FeelUpDown, new PropertyUpDownFeel(this, false));
            RegisterFeel(FeelEditUpDown, new PropertyUpDownFeel(this, true));
            RegisterFeel(FeelTrackbar, new PropertyTrackBarFeel(this, false));
            RegisterFeel(FeelTrackbarEdit, new PropertyTrackBarFeel(this, true));
            RegisterFeel(FeelButton, new PropertyButtonFeel(this, false));
            RegisterFeel(FeelFontButton, new PropertyFontFeel(this));
            RegisterFeel(FeelEditButton, new PropertyButtonFeel(this, true));
            RegisterFeel(FeelList, new PropertyListFeel(this, false));
            RegisterFeel(FeelEditList, new PropertyListFeel(this, true));
            RegisterFeel(FeelRadioButton, new PropertyRadioButtonFeel(this));
            //            RegisterFeel(FeelColorChooser, new PropertyColorChooserFeel(this, false));
            RegisterFeel(FeelCheckbox, new PropertyCheckboxFeel(this));
            RegisterFeel(FeelEditUnit, new PropertyUnitFeel(this));
            RegisterFeel(FeelDateTime, new PropertyDateTimeFeel(this));

            // DOC: If new relations are added, modify the doc for RegisterFeelAttachment
            RegisterFeelAttachment(typeof(Font), FeelFontButton);
//            RegisterFeelAttachment(typeof(DateTime), FeelDateTime);
            RegisterFeelAttachment(typeof(Pen), FeelNone);
            //TODO:            RegisterFeelAttachment(typeof(Font), "Size", FeelEditUpDown);

            // DOC: If new relations are added, modify the doc for RegisterLookAttachment
            RegisterLookAttachment(typeof(Pen), typeof(PropertyPenLook));
//            RegisterLookAttachment(typeof(DateTime), typeof(PropertyDateTimeLook));
            //TODO:            RegisterLookAttachment(typeof(Font), "Name", typeof(PropertyFontNameLook));

            // DOC: If new relations are added, modify the doc for RegisterDropDownContent
            RegisterDropDownContent(typeof(Enum), typeof(DropDownContentListBox));
            RegisterDropDownContent(typeof(bool), typeof(DropDownContentListBox));
            RegisterDropDownContent(typeof(string), typeof(DropDownContentListBox));

            DefaultFeel = GetRegisteredFeel(FeelEdit);

            _grid.MouseDown += new MouseEventHandler(OnGridMouseDown);
            _grid.MouseMove += new MouseEventHandler(OnGridMouseMove);
            _grid.MouseUp += new MouseEventHandler(OnGridMouseUp);
            _grid.MouseEnter += new EventHandler(OnGridMouseEnter);
            _grid.MouseHover += new EventHandler(OnMouseHover);
            _grid.MouseLeave += new EventHandler(OnMouseLeave);
#if _DOTNET2
            _grid.MouseClick += new MouseEventHandler(OnGridMouseClick);
            _grid.MouseDoubleClick += new MouseEventHandler(OnGridMouseDoubleClick);
#endif
            _grid.Click += new EventHandler(OnGridClick);
            _grid.KeyDown += new KeyEventHandler(OnGridKeyDown);
            _grid.KeyUp += new KeyEventHandler(OnGridKeyUp);
            _grid.KeyPress += new KeyPressEventHandler(OnGridKeyPress);
            _grid.DragOver += new DragEventHandler(InternalGrid_DragOver);
            _grid.DragEnter += new DragEventHandler(InternalGrid_DragEnter);
            _grid.DragDrop += new DragEventHandler(InternalGrid_DragDrop);
            _grid.DragLeave += new EventHandler(InternalGrid_DragLeave);
            _grid.QueryContinueDrag += new QueryContinueDragEventHandler(InternalGrid_QueryContinueDrag);
            _grid.GiveFeedback += new GiveFeedbackEventHandler(InternalGrid_GiveFeedback);

            SystemEvents.UserPreferenceChanged += new UserPreferenceChangedEventHandler(OnUserPreferenceChanged);
        }

        #endregion

        #region methods

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (components != null)
                    components.Dispose();

                SystemEvents.UserPreferenceChanged -= new UserPreferenceChangedEventHandler(OnUserPreferenceChanged);
            }

            base.Dispose(disposing);
        }

        public void SetDropDownTopWindow(Control control)
        {
            _grid.SetDropDownTopWindow(control);
        }

        /// <summary>
        /// Call this function to register a <see cref="PropertyFeel"/> you have written.
        /// </summary>
        /// <param name="feelName">A unique name ID given to your feel class.</param>
        /// <param name="feel">A reference to a unique instance of the feel you have created.</param>
        /// <seealso cref="PropertyGrid.GetRegisteredFeel"/>
        public void RegisterFeel(string feelName, PropertyFeel feel)
        {
            if (feel != null)
                feel.Name = feelName;

            _registeredFeels[feelName] = feel;
        }

        /// <summary>
        /// Retrieves a previously registered PropertyFeel instance.
        /// </summary>
        /// <param name="feelName">Unique name of the PropertyFeel instance to search.</param>
        /// <returns>A reference to the requested PropertyFeel instance. If the feel does not exist, an exception
        /// is thrown.</returns>
        /// <seealso cref="PropertyGrid.RegisterFeel"/>
        public PropertyFeel GetRegisteredFeel(string feelName)
        {
            PropertyFeel feel = null;

            // Return null for an invalid feel name
            if ((feelName != null) && (feelName.Length > 0))
                feel = (PropertyFeel)_registeredFeels[feelName];

            if (feel == null)
                throw new Exception("The feel (" + feelName + ") does not exist!");

            return feel;
        }

        /// <summary>
        /// Defines a default <see cref="PropertyFeel"/> for properties of a given type.
        /// </summary>
        /// <param name="valueType">The type of the properties that will be attached to the feel.</param>
        /// <param name="feelName">The unique name of the PropertyFeel.</param>
        /// <remarks>
        /// By calling this method, you can associate a data type (a property type displayed by the grid) with
        /// a PropertyFeel. When adding a new property, the feel will automatically be assigned to it.
        /// PropertyGrid defines some default relations for fonts and pens with respectively
        /// <see cref="FeelFontButton"/> and <see cref="FeelNone"/>.
        /// </remarks>
        public void RegisterFeelAttachment(Type valueType, string feelName)
        {
            _registeredFeelAttachments[valueType] = feelName;
        }

        /// <summary>
        /// Returns a PropertyFeel instance that is attached by default to a Property of a given type
        /// when the user doesn't specify a specific feel.
        /// </summary>
        /// <param name="valueType">The type of the properties that will be attached to the feel.</param>
        /// <returns>An instance to a previously registered PropertyFeel, null otherwise.</returns>
        public PropertyFeel GetRegisteredFeelAttachment(Type valueType)
        {
            string feelName = (string)_registeredFeelAttachments[valueType];
            if ((feelName == null) && (valueType.IsEnum))
                feelName = (string)_registeredFeelAttachments[typeof(Enum)];

            if ((feelName == null) || (feelName.Length == 0))
                return null;

            return GetRegisteredFeel(feelName);
        }

        /// <summary>
        /// Defines a default <see cref="PropertyLook"/> for properties of a given type.
        /// </summary>
        /// <param name="valueType">The type of the properties that will be attached to the look.</param>
        /// <param name="lookType">The type of the PropertyLook.</param>
        /// <remarks>
        /// By calling this method, you can associate a data type (a property type displayed by the grid) with
        /// a PropertyLook. When adding a new property, the look will automatically be assigned to it.
        /// PropertyGrid defines one default relation for pens with <see cref="PropertyPenLook"/>.
        /// </remarks>
        public void RegisterLookAttachment(Type valueType, Type lookType)
        {
            _registeredLookAttachments[valueType] = lookType;
        }

        /// <summary>
        /// Returns a PropertyLook instance that is attached by default to a Property of a given type
        /// when the user doesn't specify a specific look.
        /// </summary>
        /// <param name="valueType">The type of the properties that will be attached to the look.</param>
        /// <returns>An instance to a previously registered PropertyLook, null otherwise.</returns>
        public PropertyLook GetRegisteredLookAttachment(Type valueType)
        {
            Type lookType = (Type)_registeredLookAttachments[valueType];
            if (lookType != null)
                return (PropertyLook)Activator.CreateInstance(lookType);

            return null;
        }

        /// <summary>
        /// Defines the content of the dropdown window that will be displayed by default for a given type.
        /// </summary>
        /// <param name="valueType">The type of the properties that will be attached to a specific dropdown control.</param>
        /// <param name="listBoxType">The type of the control that is displayed in the dropdown window.</param>
        /// <remarks>
        /// By calling this method, you can associate a data type (a property type displayed by the grid) with
        /// a control type displayed in a dropdown window.
        /// PropertyGrid defines some default relations for enumerations, booleans and strings with the supplied
        /// <see cref="DropDownContentListBox" />.
        /// The content of the dropdown window can also be defined:
        /// <list type="bullet">
        /// <item><description>By attaching a PropertyDropDownContentAttribute to a property.</description></item>
        /// <item><description>By attaching a PropertyDropDownContentAttribute to the parent of a property.</description></item>
        /// </list>
        /// </remarks>
        public void RegisterDropDownContent(Type valueType, Type listBoxType)
        {
            _registeredDropDownContent[valueType] = listBoxType;
        }

        /// <summary>
        /// Returns the control that has to be displayed in the dropdown window for a given property.
        /// </summary>
        /// <param name="propEnum">An enumerator to the property whose dropdown content is requested.</param>
        /// <param name="valueKey">A key allowing to retrieve a PropertyValue supplying the content of the listbox.</param>
        /// <returns>The control that is displayed in the dropdown window for a given property.</returns>
        /// <remarks>
        /// The content of the dropdown window can be defined by the client application in several ways:
        /// <list type="bullet">
        /// <item><description>By attaching a <see cref="PropertyDropDownContentAttribute"/> to a property.</description></item>
        /// <item><description>By attaching a PropertyDropDownContentAttribute to the parent of a property.</description></item>
        /// <item><description>By registering a control type with a property type.</description></item>
        /// </list>
        /// <para> This method is virtual so that you can override the mechanism by which a control is placed
        /// in the dropdown content.
        /// </para>
        /// </remarks>
        internal Control GetDropDownContent(PropertyEnumerator propEnum, object valueKey)
        {
            PropertyValue propertyValue;

            if (valueKey == null)
                propertyValue = propEnum.Property.Value;
            else
                propertyValue = propEnum.Property.GetValue(valueKey);

            Type type = propertyValue.UnderlyingType;

            // Let's check first if the user assigned a particular dropdown content to the property
            ArrayList pddcAttrs = propertyValue.GetAttributes(typeof(PropertyDropDownContentAttribute));
            PropertyDropDownContentAttribute pddcAttr = null;
            foreach (PropertyDropDownContentAttribute attr in pddcAttrs)
            {
                if (attr.ChildPropertyName.Length == 0)
                {
                    pddcAttr = attr;
                    break;
                }
            }
            if (pddcAttr == null)
            {
                // If this is not the case, the attribute is searched on its parent property
                if (propEnum.Parent.Property != null)
                {
                    PropertyValue parentValue = propEnum.Parent.Property.Value;
                    if (parentValue != null)
                    {
                        pddcAttrs = parentValue.GetAttributes(typeof(PropertyDropDownContentAttribute));
                        foreach (PropertyDropDownContentAttribute attr in pddcAttrs)
                        {
                            if (attr.ChildPropertyName.CompareTo(propertyValue.DisplayName) == 0)
                            {
                                pddcAttr = attr;
                                break;
                            }
                        }
                        if (pddcAttr != null)
                        {
                            if (pddcAttr.ControlType != null)
                            {
                                Control control = (Control)Activator.CreateInstance(pddcAttr.ControlType);
                                if (control != null)
                                    return control;
                            }
                        }
                    }
                }
            }
            else
                return (Control)Activator.CreateInstance(pddcAttr.ControlType, false);

            // Then let's find a default dropdown content already registered
            Type listBoxType;
            if (type.IsEnum)
                listBoxType = (Type)_registeredDropDownContent[typeof(Enum)];
            else
                listBoxType = (Type)_registeredDropDownContent[type];

            if (listBoxType == null)
                listBoxType = typeof(DropDownContentListBox);

            return (Control)Activator.CreateInstance(listBoxType, false);
        }

        /// <summary>
        /// Returns the rectangle occupied by the top toolbar.
        /// </summary>
        /// <param name="clientRect">The rectangle defining the client area of the PropertyGrid.</param>
        /// <returns>The rectangle occupied by the top toolbar.</returns>
        private Rectangle GetToolbarRect(Rectangle clientRect)
        {
            if (_toolbar == null)
                clientRect.Height = 0;
            else
                clientRect.Height = _toolbar.Height;

            return clientRect;
        }

        /// <summary>
        /// Draws the comment area at a specified location.
        /// </summary>
        /// <param name="graphics">A graphics object used for painting.</param>
        /// <param name="commentRect">The bounding rectangle of the comment area.</param>
        private void DrawCommentsArea(Graphics graphics, Rectangle commentRect)
        {
            // Draw the separation line
            Rectangle rect = commentRect;
            rect.Height = DrawManager.CommentsGapHeight;

            DrawManager.DrawCommentsGap(graphics, rect);

            rect.Y = rect.Bottom;
            rect.Height = commentRect.Height - DrawManager.CommentsGapHeight;

            // Draw the background
            DrawManager.DrawCommentsBackground(graphics, rect);

            // Display the comment

            PropertyEnumerator propEnum = SelectedPropertyEnumerator;
            if (propEnum != propEnum.RightBound)
            {
                rect.Inflate(-GlobalTextMargin - 1, -GlobalTextMargin - 1);

                DrawManager.DrawCommentText(graphics, rect, SelectedPropertyEnumerator);
            }
        }

        /// <exclude />
        protected override void OnPaint(PaintEventArgs pe)
        {
            Rectangle clientRect = ClientRectangle;

            // Preselect a default font
            //			CFont* oldFont = dc.SelectObject(GetCtrlFont());

            // Draw the top buttons
            //---------------------
            if (ToolbarVisibility)
            {
                // Compute the rect where we paint the buttons
                Rectangle buttonRect = GetToolbarRect(clientRect);

                DrawManager.DrawButtonsBackground(pe.Graphics, buttonRect);
                DrawManager.DrawButtonsGap(pe.Graphics, buttonRect);
            }

            // Draw the comments box
            //----------------------

            if (_showComments)
                DrawCommentsArea(pe.Graphics, CommentsRect);

            base.OnPaint(pe);
        }

        /// <summary>
        /// Called each time the layout of the grid has changed.
        /// </summary>
        /// <remarks>This happens when the grid is resized, when the buttons area is shown or hidden,
        /// when the comment area is shown or hidden, and when the comment area is resized.
        /// </remarks>
        internal void RepositionInternalGrid()
        {
            _grid.Location = new Point(0, ToolbarHeight);
            _grid.Size = new Size(ClientRectangle.Width, ClientRectangle.Height - ToolbarHeight - CurrentCommentsHeight);

#if _SPGEVAL
            Refresh();
#endif
        }

        /// <exclude />
        protected override void OnResize(EventArgs e)
        {
            int height = ClientRectangle.Height - ToolbarHeight - CurrentCommentsHeight;
            if (height < 2 * BasicPropertyHeight)
                CommentsHeight = ClientRectangle.Height - ToolbarHeight - 2 * BasicPropertyHeight;

            RepositionInternalGrid();
            Refresh();

            base.OnResize(e);
        }

        /// <exclude />
        protected override void OnMouseDown(MouseEventArgs e)
        {
            if (_grid.Bounds.Contains(new Point(e.X, e.Y)))
            {
                base.OnMouseDown(e);
                return;
            }

            // Focus must be regained if lost
            if ((_grid.Focused == false) && _grid.TabStop)  // TabStop is set to false in disableModes.NoPropertySelection
                _grid.Focus();

            Invalidate();

            // Find the location of the mouse cursor
            HitTests hitTest = HitTest(new Point(e.X, e.Y));

            if (hitTest == HitTests.CommentSplit)
            {
                if (_commentsLock == false)
                {
                    _resizingCommentsInProgress = true;
                    _resizingCommentsPoint = new Point(e.X, e.Y);
                    Capture = true;
                }
            }

            base.OnMouseDown(e);
        }

        /// <summary>
        /// Returns a code representing the kind of location a given point lies in the PropertyGrid.
        /// </summary>
        /// <param name="point">The point being queried.</param>
        /// <returns>A code representing the kind of location a given point lies in the PropertyGrid.</returns>
        /// <remarks>Two codes are currently returned: <see cref="HitTests.CommentSplit"/> if the point lies
        /// in the zone allowing to resize the comment area and <see cref="HitTests.Client"/> otherwise.</remarks>
        public HitTests HitTest(Point point)
        {
            if (CommentsVisibility)
            {
                Rectangle rect = CommentsRect;

                if (_commentsLock == false)
                {
                    rect.Height = DrawManager.CommentsGapHeight;

                    if (rect.Contains(point))
                        return HitTests.CommentSplit;
                }

                rect = CommentsRect;
                rect.Y += DrawManager.CommentsGapHeight;
                rect.Height -= DrawManager.CommentsGapHeight;
                if (rect.Contains(point))
                    return HitTests.CommentArea;
            }

            if (_grid.Bounds.Contains(point))
            {
                Point pt = PointToInternalGrid(point);
                PropertyEnumerator propEnum;
                Rectangle itemRect;
                return _grid.HitTest(pt, out propEnum, out itemRect);
            }

            return HitTests.Client;
        }

        /// <summary>
        /// Returns an enumerator on the property that is under a certain point.
        /// </summary>
        /// <param name="point">Point in coordinates relative to the top/left corner of the PropertyGrid client
        /// area.</param>
        /// <param name="itemRect">If a property is found, stores its containing rectangle. It uses coordinates
        /// relative to the top/left corner of the internal grid.</param>
        /// <returns>An enumerator on the property that is under a certain point.</returns>
        public PropertyEnumerator PropertyItemFromPoint(Point point, out Rectangle itemRect)
        {
            Point pt = PointToInternalGrid(point);
            return _grid.PropertyItemFromPoint(pt, out itemRect);
        }

        /// <summary>
        /// Returns the rectangle, in coordinates relative to the internal grid, of a property.
        /// </summary>
        /// <param name="propEnum">An enumerator to the property.</param>
        /// <returns>The rectangle, in coordinates relative to the internal grid, of a property.</returns>
        public Rectangle GetItemRect(PropertyEnumerator propEnum)
        {
            return _grid.GetItemRect(propEnum);
        }

        /// <exclude />
        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);

            if (_grid.Bounds.Contains(new Point(e.X, e.Y)) && !Capture)
                return;

            // Change the mouse cursor if it is on the comments resizing area
            //---------------------------------------------------------------

            HitTests hitTest = HitTest(new Point(e.X, e.Y));

            if ((hitTest == HitTests.CommentSplit) || _resizingCommentsInProgress)
                Cursor = Cursors.HSplit;
            else
                Cursor = Cursors.Arrow;

            // Perform a resizing of the comments area
            //----------------------------------------

            if (_resizingCommentsInProgress)
            {
                Rectangle clientRect = ClientRectangle;

                int oldHeight = CommentsHeight;
                int height = CommentsHeight + (_resizingCommentsPoint.Y - e.Y);

                if (height < MinCommentsAreaHeight)
                {
                    _resizingCommentsPoint = new Point(e.X, e.Y);
                    _resizingCommentsPoint.Y += (height - MinCommentsAreaHeight);
                    height = MinCommentsAreaHeight;
                }
                else if (height > clientRect.Height - 2 * BasicPropertyHeight - ToolbarHeight)
                {
                    _resizingCommentsPoint = new Point(e.X, e.Y);
                    _resizingCommentsPoint.Y += (height - (clientRect.Height - 2 * BasicPropertyHeight - ToolbarHeight));
                    height = clientRect.Height - 2 * BasicPropertyHeight - ToolbarHeight;
                }
                else
                    _resizingCommentsPoint = new Point(e.X, e.Y);

                if (height != oldHeight)
                    CommentsHeight = height;

            }
        }

        /// <exclude />
        protected override void OnMouseUp(MouseEventArgs e)
        {
            if (_resizingCommentsInProgress)
            {
                _resizingCommentsInProgress = false;
                Capture = false;
            }

            base.OnMouseUp(e);
        }

        /// <exclude />
        protected override void OnGotFocus(EventArgs e)
        {
            if ((NavigationKeyMode == PropertyGrid.NavigationKeyModes.ArrowKeys) ||
                ((TabKeyNavigationMode & PropertyGrid.TabKeyNavigationModes.TabKeyWithAutoFocus) == 0))
            {
                if (ActiveControl == null)
                    ActiveControl = _grid;
            }

            base.OnGotFocus(e);
        }

        /// <summary>
        /// Returns true if a given property has at least one visible child property.
        /// </summary>
        /// <param name="propEnum">An enumerator to the property being tested.</param>
        /// <returns>true if a given property has at least one visible child property, false otherwise.</returns>
        public bool HasOneVisibleChild(PropertyEnumerator propEnum)
        {
            return _grid.HasOneVisibleChild(propEnum);
        }

        /// <summary>
        /// Appends a new category at the root of the PropertyGrid.
        /// </summary>
        /// <param name="id">A unique identifier of the new property.</param>
        /// <param name="catName">The label of the new category.</param>
        /// <returns>An enumerator to the newly created category.</returns>
        public PropertyEnumerator AppendRootCategory(int id, string catName)
        {
            return _grid.AppendRootCategory(id, catName);
        }

        /// <summary>
        /// Inserts a new category at the root of the PropertyGrid.
        /// </summary>
        /// <param name="beforeCategory">The root category before which the new category will be inserted.</param>
        /// <param name="id">A unique identifier of the new property.</param>
        /// <param name="catName">The label of the new category.</param>
        /// <returns>An enumerator to the newly created category.</returns>
        public PropertyEnumerator InsertRootCategory(PropertyEnumerator beforeCategory, int id, string catName)
        {
            return _grid.InsertRootCategory(beforeCategory, id, catName);
        }

        /// <summary>
        /// Appends a new subcategory to the PropertyGrid.
        /// </summary>
        /// <param name="underCategory">The parent category under which the new property will be added.</param>
        /// <param name="id">A unique identifier of the new property.</param>
        /// <param name="catName">The label of the new category.</param>
        /// <returns>An enumerator to the newly created property.</returns>
        public PropertyEnumerator AppendSubCategory(PropertyEnumerator underCategory, int id, string catName)
        {
            return _grid.AppendSubCategory(underCategory, id, catName);
        }

        /// <summary>
        /// Inserts a new subcategory in the PropertyGrid.
        /// </summary>
        /// <param name="beforeProperty">The sibling property before which the new category will be inserted.</param>
        /// <param name="id">A unique identifier of the new property.</param>
        /// <param name="catName">The label of the new category.</param>
        /// <returns>An enumerator to the newly created property.</returns>
        public PropertyEnumerator InsertSubCategory(PropertyEnumerator beforeProperty, int id, string catName)
        {
            return _grid.InsertSubCategory(beforeProperty, id, catName);
        }

        /// <summary>
        /// Appends a new property to the PropertyGrid.
        /// </summary>
        /// <param name="underCategory">The parent category under which the new property will be added.</param>
        /// <param name="id">A unique identifier of the new property.</param>
        /// <param name="propName">The label of the new property.</param>
        /// <param name="container">The instance of an object containing the C# property displayed in this new property.</param>
        /// <param name="memberName">The name of the C# property whose value is displayed in this new property.</param>
        /// <param name="comment">The string displayed in the comment area for the new property.</param>
        /// <returns>An enumerator to the newly created property.</returns>
        /// <remarks>The new property is appended to the list of child properties of the category passed in argument.</remarks>
        public PropertyEnumerator AppendProperty(PropertyEnumerator underCategory, int id, string propName, object container, string memberName, string comment)
        {
            if (comment == null)
                throw new ArgumentNullException("The comment can't be null. Use an empty string instead.");

            return _grid.AppendProperty(underCategory, id, propName, container, memberName, comment, null);
        }

        /// <summary>
        /// Appends a new property with custom attributes to the PropertyGrid.
        /// </summary>
        /// <param name="underCategory">The parent category under which the new property will be added.</param>
        /// <param name="id">A unique identifier of the new property.</param>
        /// <param name="propName">The label of the new property.</param>
        /// <param name="container">The instance of an object containing the C# property displayed in this new property.</param>
        /// <param name="memberName">The name of the C# property whose value is displayed in this new property.</param>
        /// <param name="comment">The string displayed in the comment area for the new property.</param>
        /// <param name="attributes">Since the new property is created dynamically, it is possible to attach
        /// attributes not know at design-time to the property.</param>
        /// <returns>An enumerator to the newly created property.</returns>
        public PropertyEnumerator AppendProperty(PropertyEnumerator underCategory, int id, string propName, object container, string memberName, string comment, params Attribute[] attributes)
        {
            if (comment == null)
                throw new ArgumentNullException("The comment can't be null. Use an empty string instead.");

            return _grid.AppendProperty(underCategory, id, propName, container, memberName, comment, attributes);
        }

        private PropertyEnumerator AppendProperty(PropertyEnumerator underCategory, int id, string propName, object container, PropertyDescriptor propertyDescriptor, string comment, params Attribute[] attributes)
        {
            return _grid.AppendProperty(underCategory, id, propName, container, propertyDescriptor, comment, attributes);
        }

        /// <summary>
        /// Inserts a new property in the PropertyGrid.
        /// </summary>
        /// <param name="beforeProperty">The sibling property before which the new property will be inserted.</param>
        /// <param name="id">A unique identifier of the new property.</param>
        /// <param name="propName">The label of the new property.</param>
        /// <param name="container">The instance of an object containing the C# property displayed in this new property.</param>
        /// <param name="memberName">The name of the C# property whose value is displayed in this new property.</param>
        /// <param name="comment">The string displayed in the comment area for the new property.</param>
        /// <returns>An enumerator to the newly created property.</returns>
        public PropertyEnumerator InsertProperty(PropertyEnumerator beforeProperty, int id, string propName, object container, string memberName, string comment)
        {
            if (comment == null)
                throw new ArgumentNullException("The comment can't be null. Use an empty string instead.");

            return InsertProperty(beforeProperty, id, propName, container, memberName, comment, null);
        }

        /// <summary>
        /// Inserts a new property with custom attributes in the PropertyGrid.
        /// </summary>
        /// <param name="beforeProperty">The sibling property before which the new property will be inserted.</param>
        /// <param name="id">A unique identifier of the new property.</param>
        /// <param name="propName">The label of the new property.</param>
        /// <param name="container">The instance of an object containing the C# property displayed in this new property.</param>
        /// <param name="memberName">The name of the C# property whose value is displayed in this new property.</param>
        /// <param name="comment">The string displayed in the comment area for the new property.</param>
        /// <param name="attributes">Since the new property is created dynamically, it is possible to attach
        /// attributes not know at design-time to the property.</param>
        /// <returns>An enumerator to the newly created property.</returns>
        public PropertyEnumerator InsertProperty(PropertyEnumerator beforeProperty, int id, string propName, object container, string memberName, string comment, params Attribute[] attributes)
        {
            if (comment == null)
                throw new ArgumentNullException("The comment can't be null. Use an empty string instead.");

            return _grid.InsertProperty(beforeProperty, id, propName, container, memberName, comment, attributes);
        }

        private PropertyEnumerator InsertProperty(PropertyEnumerator beforeProperty, int id, string propName, object container, PropertyDescriptor propertyDescriptor, string comment, params Attribute[] attributes)
        {
            return _grid.InsertProperty(beforeProperty, id, propName, container, propertyDescriptor, comment, attributes);
        }

        /// <summary>
        /// Appends a new HyperLink property to the PropertyGrid.
        /// </summary>
        /// <param name="underCategory">The parent category under which the new property will be added.</param>
        /// <param name="id">A unique identifier of the new property.</param>
        /// <param name="propName">The label of the new property.</param>
        /// <param name="comment">The string displayed in the comment area for the new property.</param>
        /// <returns>An enumerator to the newly created property.</returns>
        /// <remarks>The new property is appended to the list of child properties of the category passed in argument.
        /// </remarks>
        public PropertyEnumerator AppendHyperLinkProperty(PropertyEnumerator underCategory, int id, string propName, string comment)
        {
            if (comment == null)
                throw new ArgumentNullException("The comment can't be null. Use an empty string instead.");

            return _grid.AppendHyperLinkProperty(underCategory, id, propName, comment);
        }

        /// <summary>
        /// Inserts a new HyperLink property in the PropertyGrid.
        /// </summary>
        /// <param name="beforeProperty">The sibling property before which the new property will be inserted.</param>
        /// <param name="id">A unique identifier of the new property.</param>
        /// <param name="propName">The label of the new property.</param>
        /// <param name="comment">The string displayed in the comment area for the new property.</param>
        /// <returns>An enumerator to the newly created property.</returns>
        /// <remarks>The new property is inserted before the sibling property passed in argument.</remarks>
        public PropertyEnumerator InsertHyperLinkProperty(PropertyEnumerator beforeProperty, int id, string propName, string comment)
        {
            if (comment == null)
                throw new ArgumentNullException("The comment can't be null. Use an empty string instead.");

            return _grid.InsertHyperLinkProperty(beforeProperty, id, propName, comment);
        }

        /// <summary>
        /// Appends a new self-managed property to the PropertyGrid.
        /// </summary>
        /// <param name="underCategory">The parent category under which the new property will be added.</param>
        /// <param name="id">A unique identifier of the new property.</param>
        /// <param name="propName">The label of the new property.</param>
        /// <param name="valueType">The type of the value stored by the property.</param>
        /// <param name="initialValue">The initial value displayed by the property.</param>
        /// <param name="comment">The string displayed in the comment area for the new property.</param>
        /// <returns>An enumerator to the newly created property.</returns>
        /// <remarks> A self-managed property is a property that doesn't "point" to one of your C# properties. In
        /// other terms, when you create it, you don't pass the instance of an object and the name of a property
        /// that belongs to that object. Instead, you supply the type of a variable and its initial value and this
        /// variable will be created internally.
        /// </remarks>
        /// <example>To read or modify the variable, you will need to access the underlying Value class:
        /// <code>
        /// PropertyEnumerator propEnum = AppendManagedProperty(categoryEnum, 10, "Label", typeof(int), 100, "");
        /// propEnum.Property.Value.SetValue(200);
        /// </code>
        /// </example>
        public PropertyEnumerator AppendManagedProperty(PropertyEnumerator underCategory, int id, string propName, Type valueType, object initialValue, string comment)
        {
            if (comment == null)
                throw new ArgumentNullException("The comment can't be null. Use an empty string instead.");

            return AppendManagedProperty(underCategory, id, propName, valueType, initialValue, comment, null);
        }

        /// <summary>
        /// Appends a new self-managed property with custom attributes to the PropertyGrid.
        /// </summary>
        /// <param name="underCategory">The parent category under which the new property will be added.</param>
        /// <param name="id">A unique identifier of the new property.</param>
        /// <param name="propName">The label of the new property.</param>
        /// <param name="valueType">The type of the value stored by the property.</param>
        /// <param name="initialValue">The initial value displayed by the property.</param>
        /// <param name="comment">The string displayed in the comment area for the new property.</param>
        /// <param name="attributes">Since the new property is created dynamically, it is possible to attach
        /// attributes not know at design-time to the property.</param>
        /// <returns>An enumerator to the newly created property.</returns>
        /// <remarks> A self-managed property is a property that doesn't "point" to one of your C# properties. In
        /// other terms, when you create it, you don't pass the instance of an object and the name of a property
        /// that belongs to that object. Instead, you supply the type of a variable and its initial value and this
        /// variable will be created internally.
        /// </remarks>
        /// <example>To read or modify the variable, you will need to access the underlying Value class:
        /// <code>
        /// PropertyEnumerator propEnum = AppendManagedProperty(categoryEnum, 10, "Label", typeof(int), 100, "");
        /// propEnum.Property.Value.SetValue(200);
        /// </code>
        /// </example>
        public PropertyEnumerator AppendManagedProperty(PropertyEnumerator underCategory, int id, string propName, Type valueType, object initialValue, string comment, params Attribute[] attributes)
        {
            if (comment == null)
                throw new ArgumentNullException("The comment can't be null. Use an empty string instead.");

            return _grid.AppendManagedProperty(underCategory, id, propName, valueType, initialValue, comment, attributes);
        }

        /// <summary>
        /// Inserts a self-managed property in the PropertyGrid.
        /// </summary>
        /// <param name="beforeProperty">The sibling property before which the new property will be inserted.</param>
        /// <param name="id">A unique identifier of the new property.</param>
        /// <param name="propName">The label of the new property.</param>
        /// <param name="valueType">The type of the value stored by the property.</param>
        /// <param name="initialValue">The initial value displayed by the property.</param>
        /// <param name="comment">The string displayed in the comment area for the new property.</param>
        /// <returns>An enumerator to the newly created property.</returns>
        /// <remarks> A self-managed property is a property that doesn't "point" to one of your C# properties. In
        /// other terms, when you create it, you don't pass the instance of an object and the name of a property
        /// that belongs to that object. Instead, you supply the type of a variable and its initial value and this
        /// variable will be created internally.
        /// </remarks>
        /// <example>To read or modify the variable, you will need to access the underlying Value class:
        /// <code>
        /// PropertyEnumerator propEnum = InsertManagedProperty(beforePropEnum, 10, "Label", typeof(int), 100, "");
        /// propEnum.Property.Value.SetValue(200);
        /// </code>
        /// </example>
        public PropertyEnumerator InsertManagedProperty(PropertyEnumerator beforeProperty, int id, string propName, Type valueType, object initialValue, string comment)
        {
            if (comment == null)
                throw new ArgumentNullException("The comment can't be null. Use an empty string instead.");

            return InsertManagedProperty(beforeProperty, id, propName, valueType, initialValue, comment, null);
        }

        /// <summary>
        /// Inserts a new self-managed property with custom attributes in the PropertyGrid.
        /// </summary>
        /// <param name="beforeProperty">The sibling property before which the new property will be inserted.</param>
        /// <param name="id">A unique identifier of the new property.</param>
        /// <param name="propName">The label of the new property.</param>
        /// <param name="valueType">The type of the value stored by the property.</param>
        /// <param name="initialValue">The initial value displayed by the property.</param>
        /// <param name="comment">The string displayed in the comment area for the new property.</param>
        /// <param name="attributes">Since the new property is created dynamically, it is possible to attach
        /// attributes not know at design-time to the property.</param>
        /// <returns>An enumerator to the newly created property.</returns>
        /// <remarks> A self-managed property is a property that doesn't "point" to one of your C# properties. In
        /// other terms, when you create it, you don't pass the instance of an object and the name of a property
        /// that belongs to that object. Instead, you supply the type of a variable and its initial value and this
        /// variable will be created internally.
        /// </remarks>
        /// <example>To read or modify the variable, you will need to access the underlying Value class:
        /// <code>
        /// PropertyEnumerator propEnum = InsertManagedProperty(beforePropEnum, 10, "Label", typeof(int), 100, "");
        /// propEnum.Property.Value.SetValue(200);
        /// </code>
        /// </example>
        public PropertyEnumerator InsertManagedProperty(PropertyEnumerator beforeProperty, int id, string propName, Type valueType, object initialValue, string comment, params Attribute[] attributes)
        {
            if (comment == null)
                throw new ArgumentNullException("The comment can't be null. Use an empty string instead.");

            return _grid.InsertManagedProperty(beforeProperty, id, propName, valueType, initialValue, comment, attributes);
        }

        /// <summary>
        /// Clears the whole content of the PropertyGrid.
        /// </summary>
        /// <remarks>When the grid has been filled using <see cref="SelectedObject"/>, it is preferable to use
        /// this call to clear the grid:
        /// <code>
        /// SelectedObject = null;
        /// </code>
        /// </remarks>
        public void Clear()
        {
            _grid.Clear();
        }

        /// <summary>
        /// Deletes any property under a parent property.
        /// </summary>
        /// <param name="propEnum">The parent property under which all properties will be deleted.</param>
        public void ClearUnderProperty(PropertyEnumerator propEnum)
        {
            BeginUpdate();
            _grid.ClearUnderProperty(propEnum);
            EndUpdate();
        }

        internal bool DrawImage(int imageIndex, Graphics graphics, int x, int y)
        {
            return _grid.DrawImage(imageIndex, graphics, x, y);
        }

        /// <summary>
        /// Enables or disables a property or category.
        /// </summary>
        /// <param name="propEnum">An enumerator to the property being enabled/disabled.</param>
        /// <param name="enable">True if the property is being enabled, false otherwise.</param>
        /// <remarks>A disabled property is rendered grayed out and its inplace control is not available.</remarks>
        public void EnableProperty(PropertyEnumerator propEnum, bool enable)
        {
            _grid.EnableProperty(propEnum, enable);
        }

        /// <summary>
        /// Let's the end-user disable a property.
        /// </summary>
        /// <param name="propEnum">An enumerator to the property being configured.</param>
        /// <param name="disabled">A boolean indicating if the checkbox is visible (true) or not (false).</param>
        /// <remarks>
        /// Enables to display a checkbox at the left of a property/category label so that the end-user can
        /// enable/disable the property and any descendants it contains.
        /// <para><see cref="Property.GetManuallyDisabledVariable">Property.GetManuallyDisabledVariable</see> returns
        /// the state of the checkbox.</para>
        /// <para>Another override of this method lets you specify the boolean variable attached to the checkbox.</para>
        /// </remarks>
        public void SetManuallyDisabled(PropertyEnumerator propEnum, bool disabled)
        {
            propEnum.Property.SetManuallyDisabled(propEnum, disabled);
            _grid.Invalidate();
        }

        /// <summary>
        /// Let's the end-user disable a property.
        /// </summary>
        /// <param name="propEnum">An enumerator to the property being configured.</param>
        /// <param name="containerInstance">The target instance that contains a boolean property that will control the
        /// value of the checkbox.</param>
        /// <param name="memberName">The name of the boolean property.</param>
        /// <remarks>
        /// Enables to display a checkbox at the left of a property/category label so that the end-user can
        /// enable/disable the property and any descendants it contains. The initial value of the checkbox is controlled
        /// by the value of the boolean passed indirectly in argument. The boolean will change when the user modifies the
        /// state of the checkbox.
        /// <para><see cref="Property.GetManuallyDisabledVariable">Property.GetManuallyDisabledVariable</see> returns
        /// the state of the checkbox.</para>
        /// </remarks>
        public void SetManuallyDisabled(PropertyEnumerator propEnum, object containerInstance, string memberName)
        {
            propEnum.Property.SetManuallyDisabled(propEnum, containerInstance, memberName);
            _grid.Invalidate();
        }

        /// <summary>
        /// Expands a property or a category so that all its direct children are made visible, or collapses it
        /// to hide all descendants.
        /// </summary>
        /// <param name="propEnum">An enumerator to the property being expanded/collapsed.</param>
        /// <param name="expand">True if the property has to be expanded, false if it has to be collapsed.</param>
        public void ExpandProperty(PropertyEnumerator propEnum, bool expand)
        {
            _grid.ExpandProperty(propEnum, expand);
        }

        /// <summary>
        /// Expands or collapses all the categories and properties in the grid.
        /// </summary>
        /// <param name="expand">true to expand all the properties, false to collapse them all.</param>
        public void ExpandAllProperties(bool expand)
        {
            _grid.ExpandAllProperties(expand);
        }

        /// <summary>
        /// Selects/deselects one property or category.
        /// </summary>
        /// <param name="enumerator">An enumerator to the property being selected/deselected.</param>
        /// <remarks>To deselect the currently selected property, use the following code:
        /// <para><c>SelectProperty(RightBound.GetVisibleDeepEnumerator());</c></para>
        /// </remarks>
        public void SelectProperty(PropertyEnumerator enumerator)
        {
            _grid.SelectProperty(enumerator);
        }

        /// <summary>
        /// Selects/deselects one property and give it the focus. Optionally, the text can be selected
        /// if a textbox exists.
        /// </summary>
        /// <param name="enumerator">An enumerator to the property being selected/deselected.</param>
        /// <param name="selectAllText">True if the text must be selected, false otherwise.</param>
        /// <remarks>To simply change the selected property without giving the focus to the
        /// inplace control, use <see cref="SelectProperty"/> instead.</remarks>
        public void SelectAndFocusProperty(PropertyEnumerator enumerator, bool selectAllText)
        {
            _grid.SelectAndFocusProperty(enumerator, selectAllText);
        }

        /// <summary>
        /// This method should be called only from custom inplace controls to generate the PropertyChanged event
        /// event from the <see cref="OnPropertyChanged"/> callback method. In other cases, don't call it.
        /// </summary>
        /// <param name="e">Contains data pertaining to the event.</param>
        public void NotifyPropertyChanged(PropertyChangedEventArgs e)
        {
            PropertyValue propValue = e.PropertyEnum.Property.Value;
            RefreshPropertiesAttribute attr = (RefreshPropertiesAttribute)propValue.GetAttribute(
                typeof(RefreshPropertiesAttribute));

            if ((SelectedObjects != null) && (SelectedObjects.Length > 0))
            {
                if ((propValue.GetValue() is ICustomTypeDescriptor) ||
                    propValue.TypeConverter.GetPropertiesSupported() ||
                    ((attr != null) && (attr.RefreshProperties == System.ComponentModel.RefreshProperties.All)))
                {
                    ActiveControl = _grid;
                    _grid.Focus();

                    RefreshProperties();

                    // e.PropertyEnum is not part of the grid anymore, so a new eventargs must be created, pointing
                    // to the new property enumerator
                    OnPropertyChanged(new PropertyChangedEventArgs(SelectedPropertyEnumerator, e.AdditionalValueKey));
                }
                else
                    OnPropertyChanged(e);
            }
            else
            {
                if ((propValue.GetValue() is ICustomTypeDescriptor) || propValue.TypeConverter.GetPropertiesSupported())
                {
                    ActiveControl = _grid;
                    _grid.Focus();

                    propValue.Recreate(true);
                }

                OnPropertyChanged(e);
            }
        }

        /// <summary>
        /// Clears and repopulates the PropertyGrid.
        /// </summary>
        /// <remarks>When the grid has been filled with SelectedObject(s), call this method to force a
        /// reconstruction of its content. This can be useful when a property's value has been changed
        /// externally and the grid is not aware of the change.
        /// <para>This is equivalent to the Refresh() call in the Microsoft PropertyGrid.</para>
        /// </remarks>
        public void RefreshProperties()
        {
            BeginUpdate();
            object states = SavePropertiesStates();
            SelectedObjects = SelectedObjects;
            RestorePropertiesStates(states);
            EndUpdate();
        }
        
        static private bool _insideNotifyValueValidation = false;

        static internal bool IsInsideNotifyValueValidation
        {
            get { return _insideNotifyValueValidation; }
        }

        /// <summary>
        /// This method should be called only from custom inplace controls to generate the ValueValidation event
        /// event from the <see cref="OnPropertyChanged"/> callback method. In other cases, don't call it.
        /// </summary>
        /// <param name="e">Contains data pertaining to the event.</param>
        public void NotifyValueValidation(ValueValidationEventArgs e)
        {
            if (_insideNotifyValueValidation)
                return;

            try
            {
                _insideNotifyValueValidation = true;

                // We don't call the callback when the app is about to loose its active state with an invalid value
                if (!_grid.ValidationWhileAppLoosingFocus ||
                    ((e.ValueValidationResult & PropertyValue.ValueValidationResult.ErrorCode) == 0))
                {
                    OnValueValidation(e);
                }
            }
            catch(Exception) {}
            finally
            {
                _insideNotifyValueValidation = false;
            }
        }

        public void OnInPlaceCtrlFinishedEdition()
        {
            _grid.OnInPlaceCtrlFinishedEdition();
        }

        /// <summary>
        /// This method should be called only from custom inplace controls to generate the InPlaceCtrlCreated event
        /// event from the <see cref="OnInPlaceCtrlCreated"/> callback method. In other cases, don't call it.
        /// </summary>
        /// <param name="e">Contains data pertaining to the event.</param>
        public void NotifyInPlaceControlCreated(InPlaceCtrlCreatedEventArgs e)
        {
            OnInPlaceCtrlCreated(e);
        }

        /// <exclude />
        protected internal virtual bool OnValidateMultiSelection(PropertyVisibleDeepEnumerator mainSelectedEnum, PropertyVisibleDeepEnumerator selectEnum)
        {
            return true;
        }

        /// <summary>
        /// Returns an enumerator to a property or category having a given identifier.
        /// </summary>
        /// <param name="propertyId">The identifier of the property or category being searched.</param>
        /// <returns>An enumerator to the property or category being searched. If it is not found, RightBound
        /// is returned.</returns>
        public PropertyEnumerator FindProperty(int propertyId)
        {
            return _grid.FindProperty(propertyId);
        }

        /// <summary>
        /// Returns an enumerator to a property or category having a given identifier.
        /// </summary>
        /// <param name="afterEnumerator">The enumerator on a property after which the search will begin.</param>
        /// <param name="propertyId">The identifier of the property or category being searched.</param>
        /// <returns>An enumerator to the property or category being searched.</returns>
        public PropertyEnumerator FindPropertyAfter(PropertyEnumerator afterEnumerator, int propertyId)
        {
            return _grid.FindPropertyAfter(afterEnumerator, propertyId);
        }

        /// <summary>
        /// Returns true if a property is currently being displayed in the visible part of the grid, false otherwise.
        /// </summary>
        /// <param name="propEnum">An enumerator to the property to be checked.</param>
        /// <returns>True if a property is currently being displayed in the visible part of the grid, false otherwise.</returns>
        public bool IsPropertyDisplayed(PropertyEnumerator propEnum)
        {
            return _grid.IsPropertyDisplayed(propEnum);
        }

        /// <summary>
        /// Removes a property or category from the PropertyGrid.
        /// </summary>
        /// <param name="propEnum">An enumerator to the property or category being removed.</param>
        /// <remarks> If the property or category being removed has descendants, they are all removed.</remarks>
        public void DeleteProperty(PropertyEnumerator propEnum)
        {
            _grid.DeleteProperty(propEnum);
        }

        /// <summary>
        /// Makes sure that a given property is actually visible and instantly available to the user in the grid.
        /// </summary>
        /// <param name="propEnum">An enumerator to the property being shown in the grid.</param>
        /// <returns></returns>
        /// <remarks>If the property is not currently available, the grid is scrolled until it is visible.</remarks>
        public bool EnsureVisible(PropertyEnumerator propEnum)
        {
            return _grid.EnsureVisible(propEnum);
        }

        /// <summary>
        /// Shows or hides a property.
        /// </summary>
        /// <param name="propEnum">An enumerator to the property being shown or hidden.</param>
        /// <param name="show">true to show the property, false to hide it.</param>
        public void ShowProperty(PropertyEnumerator propEnum, bool show)
        {
            _grid.ShowProperty(propEnum, show);
        }

        /// <summary>
        /// Updates the content of the inplace control based on the current data stored in the actual
        /// property of a target instance.
        /// </summary>
        /// <remarks>
        /// This call is necessary whe you manually update a property value of the client application
        /// and the inplace control for this property is already visible. One example is when the
        /// user selects "Reset" from a contextual menu.
        /// </remarks>
        public void UpdateInPlaceControlFromValue()
        {
            if (InPlaceControl != null)
                SelectedPropertyEnumerator.Property.Feel.UpdateInPlaceControlFromValue(
                    SelectedPropertyEnumerator);
        }

#if !_DOTNET2
        void OnToolbarButtonClicked(object sender, ToolBarButtonClickEventArgs e)
        {
            if (_toolbar.Buttons.IndexOf(e.Button) == 0)
            {
//                (_toolbar.Buttons[0]).Pushed = true;
  //              (_toolbar.Buttons[1]).Pushed = false;

                DisplayMode = DisplayModes.Categorized;
    //            _reportedDisplayMode = DisplayModes.Categorized;
                _grid.Focus();
            }
            else if (_toolbar.Buttons.IndexOf(e.Button) == 1)
            {
//                (_toolbar.Buttons[0]).Pushed = false;
  //              (_toolbar.Buttons[1]).Pushed = true;

                DisplayMode = DisplayModes.FlatSorted;
    //            _reportedDisplayMode = DisplayModes.FlatSorted;
                _grid.Focus();
            }
        }
#else
        private void OnSortByCategoy(object sender, EventArgs e)
        {
            DisplayMode = DisplayModes.Categorized;
            _grid.Focus();
        }

        private void OnSortAlphabetical(object sender, EventArgs e)
        {
            DisplayMode = DisplayModes.FlatSorted;
            _grid.Focus();
        }
#endif

        /// <summary>
        /// Freezes all drawing on the PropertyGrid.
        /// </summary>
        /// <remarks>This method can be called several times. In that case, the method <see cref="EndUpdate"/>
        /// will have to be called the same number of times.</remarks>
        public void BeginUpdate()
        {
            if (Visible)
            {
                if (_beginUpdateCounter == 0)
                    Win32Calls.SendMessage(Handle, Win32Calls.WM_SETREDRAW, 0, 0);

                _beginUpdateCounter++;
            }
        }

        /// <summary>
        /// Unfreezes drawing on the PropertyGrid.
        /// </summary>
        /// <remarks>This method must be called after <see cref="BeginUpdate"/> has been called.</remarks>
        public void EndUpdate()
        {
            if (Visible && (_beginUpdateCounter > 0))
            {
                _beginUpdateCounter--;

                if (_beginUpdateCounter == 0)
                {
                    Win32Calls.SendMessage(Handle, Win32Calls.WM_SETREDRAW, 1, 0);
                    Refresh();
                }
            }
        }

        /// <summary>
        /// 
        /// </summary>
        public void BeginVerticalFreeze()
        {
            BeginUpdate();
            _grid.BeginVerticalFreeze();
        }

        /// <summary>
        /// 
        /// </summary>
        public void EndVerticalFreeze()
        {
            _grid.EndVerticalFreeze();
            EndUpdate();
            Refresh();
        }

        /// <summary>
        /// Resizes the columns of the grid so that all property labels are completely visible.
        /// </summary>
        /// <remarks>The same effect can be obtained by the user by double-clicking on the vertical line
        /// that separates labels and values.</remarks>
        public void AdjustLabelColumn()
        {
            _grid.AdjustLabelColumn();
        }

        Point PointFromInternalGrid(Point pt)
        {
            Win32Calls.POINT internalPt = new Win32Calls.POINT();
            internalPt.x = pt.X;
            internalPt.y = pt.Y;
            Win32Calls.MapWindowPoints(Grid.Handle, Handle, ref internalPt, 1);
            pt.X = internalPt.x;
            pt.Y = internalPt.y;

            return pt;
        }

        Point PointToInternalGrid(Point pt)
        {
            Win32Calls.POINT internalPt = new Win32Calls.POINT();
            internalPt.x = pt.X;
            internalPt.y = pt.Y;
            Win32Calls.MapWindowPoints(Handle, Grid.Handle, ref internalPt, 1);
            pt.X = internalPt.x;
            pt.Y = internalPt.y;

            return pt;
        }

        void OnGridMouseMove(object sender, MouseEventArgs e)
        {
            Point pt = PointFromInternalGrid(new Point(e.X, e.Y));
            OnMouseMove(new MouseEventArgs(e.Button, e.Clicks, pt.X, pt.Y, e.Delta));
        }

        void OnGridMouseDown(object sender, MouseEventArgs e)
        {
            Point pt = PointFromInternalGrid(new Point(e.X, e.Y));
            OnMouseDown(new MouseEventArgs(e.Button, e.Clicks, pt.X, pt.Y, e.Delta));
        }

        void OnGridMouseUp(object sender, MouseEventArgs e)
        {
            Point pt = PointFromInternalGrid(new Point(e.X, e.Y));
            OnMouseUp(new MouseEventArgs(e.Button, e.Clicks, pt.X, pt.Y, e.Delta));
        }
#if _DOTNET2
        void OnGridMouseClick(object sender, MouseEventArgs e)
        {
            Point pt = PointFromInternalGrid(new Point(e.X, e.Y));
            OnMouseClick(new MouseEventArgs(e.Button, e.Clicks, pt.X, pt.Y, e.Delta));
        }

        void OnGridMouseDoubleClick(object sender, MouseEventArgs e)
        {
            Point pt = PointFromInternalGrid(new Point(e.X, e.Y));
            OnMouseDoubleClick(new MouseEventArgs(e.Button, e.Clicks, pt.X, pt.Y, e.Delta));
        }
#endif

        void OnGridMouseEnter(object sender, EventArgs e)
        {
            OnMouseEnter(e);
        }

        void OnMouseHover(object sender, EventArgs e)
        {
            OnMouseHover(e);
        }

        void OnMouseLeave(object sender, EventArgs e)
        {
            OnMouseLeave(e);
        }

        void OnGridClick(object sender, EventArgs e)
        {
            OnClick(EventArgs.Empty);
        }

        void OnGridKeyPress(object sender, KeyPressEventArgs e)
        {
            OnKeyPress(e);
        }

        void OnGridKeyDown(object sender, KeyEventArgs e)
        {
            OnKeyDown(e);
        }

        void OnGridKeyUp(object sender, KeyEventArgs e)
        {
            OnKeyUp(e);
        }

        /// <exclude />
        public new DragDropEffects DoDragDrop(object data, DragDropEffects allowedEffects)
        {
            return _grid.DoDragDrop(data, allowedEffects);
        }

        private void InternalGrid_GiveFeedback(object sender, GiveFeedbackEventArgs e)
        {
            OnGiveFeedback(e);
        }

        private void InternalGrid_QueryContinueDrag(object sender, QueryContinueDragEventArgs e)
        {
            OnQueryContinueDrag(e);
        }

        private void InternalGrid_DragLeave(object sender, EventArgs e)
        {
            OnDragLeave(e);
        }

        private void InternalGrid_DragDrop(object sender, DragEventArgs e)
        {
            OnDragDrop(e);
        }

        private void InternalGrid_DragEnter(object sender, DragEventArgs e)
        {
            OnDragEnter(e);
        }

        private void InternalGrid_DragOver(object sender, DragEventArgs e)
        {
            OnDragOver(e);
        }

        public void AddTargetInstance(PropertyEnumerator propEnum, object targetInstance, string memberName)
        {
            _grid.AddTargetInstance(propEnum, targetInstance, memberName);
        }

        /// <exclude />
        protected override bool ProcessTabKey(bool forward)
        {
            if (_grid.ContainsFocus && (NavigationKeyMode == PropertyGrid.NavigationKeyModes.TabKey))
            {
                if (forward)
                {
                    if ((_grid.InPlaceControl != null) && (_grid.InPlaceControl.ContainsFocus))
                    {
                        if (((TabKeyNavigationMode & PropertyGrid.TabKeyNavigationModes.TabKeyInSubControls) == 0) ||
                            (_grid.InPlaceControl as IInPlaceControl).GoToNextPropertyOnTab(true))
                        {
                            if (TabToNextProperty())
                                return true;
                        }
                    }
                    else if (SelectedPropertyEnumerator.Property != null)
                    {
                        if ((SelectedPropertyEnumerator.Property is RootCategory) ||
                        (SelectedPropertyEnumerator.Property.Feel == null) ||
                        (SelectedPropertyEnumerator.Property.Feel == PropertyFeel.Empty) ||
                        !SelectedPropertyEnumerator.Property.Enabled)
                        {
                            if (TabToNextProperty())
                                return true;
                        }
                    }
                }
                else
                {
                    if ((_grid.InPlaceControl != null) && (_grid.InPlaceControl.ContainsFocus))
                    {
                        if ((_grid.InPlaceControl as IInPlaceControl).GoToNextPropertyOnTab(false))
                        {
                            if (TabToPrevProperty())
                                return true;
                            else
                                ActiveControl = _grid;
                        }
                    }
                    else
                    {
                        if (TabToPrevProperty())
                            return true;
                    }
                }
            }

            return base.ProcessTabKey(forward);
        }

        private bool CanTakeTabKey(PropertyEnumerator propEnum, PropertyEnumerator startEnum)
        {
            Property property = propEnum.Property;

            if ((property.Feel == null) || (property.Feel == PropertyFeel.Empty))
                return false;

            if (property.Feel.DontShowInPlaceCtrl(propEnum))
                return false;

            if (property is RootCategory)
                return false;

            if (!property.Enabled)
                return false;

            if (((TabKeyNavigationMode & PropertyGrid.TabKeyNavigationModes.TabKeySkipReadonlyProperties) != 0) &&
                (property.Value != null) && property.Value.ReadOnly)
                return false;

            return true;
        }

        internal bool TabToNextProperty()
        {
            // Find a good candidate (below the current selected property) to put the focus in its inplace control
            PropertyEnumerator nextEnum;
            PropertyEnumerator startEnum;
            if ((TabKeyNavigationMode & TabKeyNavigationModes.TabKeyInChildProperties) != 0)
            {
                nextEnum = SelectedPropertyEnumerator.GetDeepEnumerator();
                startEnum = SelectedPropertyEnumerator.GetDeepEnumerator();
            }
            else
            {
                nextEnum = SelectedPropertyEnumerator.GetVisibleDeepEnumerator();
                startEnum = SelectedPropertyEnumerator.GetVisibleDeepEnumerator();
            }
            bool dontTab = false;
            do
            {
                nextEnum.MoveNext();
                if (nextEnum == RightBound)
                {
                    if ((TabKeyNavigationMode & PropertyGrid.TabKeyNavigationModes.TabKeyLeavesAtExtremes) == 0)
                    {
                        if ((TabKeyNavigationMode & TabKeyNavigationModes.TabKeyInChildProperties) != 0)
                            nextEnum = FirstProperty.GetDeepEnumerator();
                        else
                            nextEnum = FirstProperty.GetVisibleDeepEnumerator();
                    }
                    else
                    {
                        dontTab = true;
                        break;
                    }
                }

                if (nextEnum == startEnum)
                {
                    dontTab = true;
                    break;
                }
            }
            while (!CanTakeTabKey(nextEnum, startEnum));

            // If a property was found
            if (dontTab == false)
            {
                _grid.Focus();

                // without the following line, there could be a blink of the previously selected property value
                // (CheckScrollBar shows the inplace control of the selected property when scrolling), that's why
                // we reset the selected property to none.
                SelectProperty(RightBound);

                // Select it and show it
                EnsureVisible(nextEnum);
                SelectProperty(nextEnum);

                // The focus must then be placed in the inplace control, or its first sub-control
                if (InPlaceControl != null)
                {
                    Control nextControl = InPlaceControl.GetNextControl(InPlaceControl, true);
                    ActiveControl = (nextControl == null ? InPlaceControl : nextControl);
                    _grid.SelectAllText();
                }
            }
            // If a property was not found, it means we are at the bottom. Do we TAB to a next control in the form ?
            else
                return (nextEnum == startEnum);

            return true;
        }

        private bool TabToPrevProperty()
        {
            // Find a good candidate (below the current selected property) to put the focus in its inplace control
            PropertyEnumerator prevEnum;
            PropertyEnumerator startEnum;
            if ((TabKeyNavigationMode & TabKeyNavigationModes.TabKeyInChildProperties) != 0)
            {
                prevEnum = SelectedPropertyEnumerator.GetDeepEnumerator();
                startEnum = SelectedPropertyEnumerator.GetDeepEnumerator();
            }
            else
            {
                prevEnum = SelectedPropertyEnumerator.GetVisibleDeepEnumerator();
                startEnum = SelectedPropertyEnumerator.GetVisibleDeepEnumerator();
            }
            bool dontTab = false;
            do
            {
                prevEnum.MovePrev();
                if (prevEnum == LeftBound)
                {
                    if ((TabKeyNavigationMode & PropertyGrid.TabKeyNavigationModes.TabKeyLeavesAtExtremes) == 0)
                    {
                        if ((TabKeyNavigationMode & TabKeyNavigationModes.TabKeyInChildProperties) != 0)
                            prevEnum = LastProperty.GetDeepEnumerator();
                        else
                            prevEnum = LastProperty.GetVisibleDeepEnumerator();
                    }
                    else
                    {
                        dontTab = true;
                        break;
                    }
                }

                if (prevEnum == startEnum)
                {
                    dontTab = true;
                    break;
                }
            }
            while (!CanTakeTabKey(prevEnum, startEnum));

            // If a property was found
            if (dontTab == false)
            {
                _grid.Focus();

                // without the following line, there could be a blink of the previously selected property value
                // (CheckScrollBar shows the inplace control of the selected property when scrolling), that's why
                // we reset the selected property to none.
                SelectProperty(RightBound);

                // Select it and show it
                EnsureVisible(prevEnum);
                SelectProperty(prevEnum);

                // The focus must then be placed in the inplace control, or its first sub-control
                if (InPlaceControl != null)
                {
                    Control nextControl = InPlaceControl.GetNextControl(InPlaceControl, true);
                    ActiveControl = (nextControl == null ? InPlaceControl : nextControl);
                    _grid.SelectAllText();
                }
            }
            // If a property was not found, it means we are at the top. Do we TAB to a previous control in the form ?
            else
                return (prevEnum == startEnum);

            return true;
        }

        internal PropertyDescriptorCollection GetPropertyDescriptors(object targetInstance,
            PropertyDescriptor propertyDescriptor, out bool toBeSorted)
        {
            toBeSorted = false;

            if (targetInstance == null)
                return new PropertyDescriptorCollection(new PropertyDescriptor[0]);

            // Grab the collection of PropertyDescriptor. It can come from the object itself,
            // from a TypeConverter, from the object being a ICustomTypeDescriptor or from a
            // TypeDescriptionProvider. Only browsable attributes can be seen.

            PropertyDescriptorCollection collection;
            PropertyTypeDescriptorContext context = new PropertyTypeDescriptorContext(propertyDescriptor, targetInstance, null, this);
            TypeConverter converter;
            if (propertyDescriptor == null)
                converter = TypeDescriptor.GetConverter(targetInstance, true);
            else
                converter = propertyDescriptor.Converter;

            if (converter.GetPropertiesSupported(context))
            {
                collection = converter.GetProperties(context, targetInstance, new Attribute[0] {});
                if (!converter.GetCreateInstanceSupported(context))
                    toBeSorted = true;
            }
            else
            {
                collection = TypeDescriptor.GetProperties(targetInstance);
                if (targetInstance is ICustomTypeDescriptor == false)
                    toBeSorted = true;
            }

            ArrayList validPropertyDescriptors = new ArrayList();
            foreach(PropertyDescriptor pd in collection)
            {
                PropertyPreFilterOutEventArgs e = new PropertyPreFilterOutEventArgs(pd, targetInstance);
                OnPropertyPreFilterOut(e);

                PropertyPreFilterOutEventArgs.FilterModes filter = e.GetFilterOutInternal();
                if (filter == PropertyPreFilterOutEventArgs.FilterModes.DontFilter)
                    validPropertyDescriptors.Add(pd);
                else if (filter == PropertyPreFilterOutEventArgs.FilterModes.FilterDefault)
                {
                    BrowsableAttribute attr = pd.Attributes[typeof(BrowsableAttribute)] as BrowsableAttribute;
                    if ((attr == null) || attr.Browsable)
                        validPropertyDescriptors.Add(pd);
                }
            }

			if (validPropertyDescriptors.Count == collection.Count)
				return collection;

            PropertyDescriptor[] pdArray = new PropertyDescriptor[validPropertyDescriptors.Count];
            validPropertyDescriptors.CopyTo(pdArray);

            return new PropertyDescriptorCollection(pdArray);
        }

        internal bool MatchPropertyDescriptors(PropertyDescriptor pd1, PropertyDescriptor pd2)
        {
            if ((pd1.IsReadOnly != pd1.IsReadOnly) ||
                (pd1.Category != pd2.Category) ||      // Categories are not tested by MSPG. If they don't match it goes to the MISC category
                (pd1.DisplayName != pd2.DisplayName))
                return false;

            foreach (Attribute attr in pd1.Attributes)
            {
                if ((attr.GetType() == typeof(PropertyFeelAttribute)) ||
                    (attr.GetType() == typeof(PropertyLookAttribute)) ||
                    (attr.GetType() == typeof(ValueAddedToPropertyAttribute)) ||
                    (attr.GetType() == typeof(PropertyDisableAttribute)) ||
                    (attr.GetType() == typeof(PropertyDropDownContentAttribute)) ||
                    (attr.GetType() == typeof(PropertyHideAttribute)) ||
                    (attr.GetType() == typeof(PropertyValidatorAttribute)) ||
                    (attr.GetType() == typeof(PropertyValueDisplayedAsAttribute)) ||
                    (attr.GetType() == typeof(ShowChildPropertiesAttribute)))
                {
                    if (pd2.Attributes.Matches(attr) == false)
                        return false;
                }
            }

            return true;
        }

        void OnUserPreferenceChanged(object sender, UserPreferenceChangedEventArgs e)
        {
#if _DOTNET2
            if (e.Category == UserPreferenceCategory.VisualStyle)
#else
			if (e.Category == UserPreferenceCategory.Window)
#endif
                ThemeRenderer.Reset();
        }

        protected override void WndProc(ref Message m)
        {
            if (m.Msg == Win32Calls.WM_CUT)
                _grid.DoCut();
            else if (m.Msg == Win32Calls.WM_COPY)
                _grid.DoCopy();
            else if (m.Msg == Win32Calls.WM_PASTE)
                _grid.DoPaste();

            base.WndProc(ref m);
        }

        #endregion

        #region Compatibility with Skybound VisualTips (www.skybound.ca)
        private class VisualTipExtender : Skybound.VisualTips.External.IVisualTipExtender
        {
            public object GetChildAtPoint(Control control, int x, int y)
            {
                Rectangle itemRect;
                return (control as InternalGrid).PropertyItemFromPoint(new Point(x, y), out itemRect);
            }

            /// <summary>
            /// Returns the parent control of the specified component.
            /// </summary>
            public object GetParent(object component)
            {
                if (component is PropertyEnumerator)
                    return (component as PropertyEnumerator).Property.ParentGrid;

                return null;
            }

            /// <summary>
            /// Returns an array containing the type of child components which are supported by this IVisualTipExtender.
            /// </summary>
            public Type[] GetChildTypes()
            {
                return new Type[] { typeof(PropertyEnumerator) };
            }
        }

        static PropertyGrid()
        {
            Skybound.VisualTips.External.VisualTipService.SetExtender(
                typeof(InternalGrid), new VisualTipExtender());
        }
        #endregion
    }
}
