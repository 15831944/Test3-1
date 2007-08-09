// This is a part of the Smart PropertyGrid Library
// Copyright (C) 2001-2007 VisualHint
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions
// of the accompanying license agreement.

using System;
using System.Collections;

namespace VisualHint.SmartPropertyGrid
{
    /// <summary>
    /// PropertyTree is the entry point class to access the tree structure of the PropertyGrid.
    /// Mainly, the principle is to get an enumerator from it, and then parse the tree with it or use it
    /// to append or insert properties into the collection.
    /// </summary>
    internal class PropertyTree
    {
        /// <summary>
        /// This double linked list is the collection containing the root properties of the control.
        /// Its parent is set to null since this is the root.
        /// </summary>
        private LinkedList _PropertyTree;

        public PropertyTree(int baseDepth)
        {
            _PropertyTree = new LinkedList(null, baseDepth);
        }

        /// <summary>
        /// Returns an enumerator able to parse all the properties of the property tree in deep order.
        /// </summary>
        /// <returns>A new PropertyDeepEnumerator pointing before the first property of the collection.</returns>
        public PropertyDeepEnumerator GetDeepEnumerator()
        {
            return new PropertyDeepEnumerator(_PropertyTree.LeftBound);
        }

        /// <summary>
        /// Returns an enumerator able to parse only the visible properties of the property tree in deep order.
        /// </summary>
        /// <returns>A new PropertyVisibleDeepEnumerator pointing before the first property of the collection.</returns>
        public PropertyVisibleDeepEnumerator GetVisibleDeepEnumerator()
        {
            return new PropertyVisibleDeepEnumerator(_PropertyTree.LeftBound);
        }

        /// <summary>
        /// Returns an enumerator able to parse only the sibling properties of the root of the property tree.
        /// </summary>
        /// <returns>A new PropertySiblingEnumerator pointing before the first property of the collection.</returns>
        public PropertySiblingEnumerator GetSiblingEnumerator()
        {
            return new PropertySiblingEnumerator(_PropertyTree.LeftBound);
        }

        /// <summary>
        /// Returns an enumerator able to parse only the visible sibling properties of the root of the property tree.
        /// </summary>
        /// <returns>A new PropertyVisibleSiblingEnumerator pointing before the first property of the collection.</returns>
        public PropertyVisibleSiblingEnumerator GetVisibleSiblingEnumerator()
        {
            return new PropertyVisibleSiblingEnumerator(_PropertyTree.LeftBound);
        }

        /// <summary>
        /// Searches a property given its ID across the whole property tree.
        /// </summary>
        /// <param name="propertyId">The ID of the property being searched.</param>
        /// <returns>A deep enumerator pointing to the found property. If not found it will point to RightBound.</returns>
        public PropertyEnumerator Find(int propertyId)
        {
            PropertyDeepEnumerator enumerator = GetDeepEnumerator();
            return FindAfter(enumerator, propertyId);
        }

        /// <summary>
        /// Searches a property given its ID inside the whole property tree but only from a given enumerator and beyond.
        /// The enumerator can be sibling or deep and will restrict the search accordingly.
        /// </summary>
        /// <param name="afterEnumerator">The enumerator from which the property will be searched.</param>
        /// <param name="propertyId">The ID of the property being searched.</param>
        /// <returns>A deep enumerator pointing to the found property. If not found it will point to RightBound.</returns>
        public PropertyEnumerator FindAfter(PropertyEnumerator afterEnumerator, int propertyId)
        {
            PropertyEnumerator propEnum = new PropertyDeepEnumerator(afterEnumerator.Node);
            while (propEnum.MoveNext() != propEnum.RightBound)
            {
                if (propEnum.Property.Id == propertyId)
                    break;
            }

            // If the element is not found, the enumerator will point to RightBound
            return propEnum;
        }

        /// <summary>
        /// Inserts a property in the property tree, before a given enumerator at the same level.
        /// </summary>
        /// <param name="beforeEnumerator">The new property will be inserted before this enumerator.</param>
        /// <param name="property">The new property to insert.</param>
        /// <returns>A deep enumerator pointing to the new property.</returns>
        public PropertyEnumerator InsertBefore(PropertyEnumerator beforeEnumerator, Property property)
        {
            return new PropertyDeepEnumerator(beforeEnumerator.Node.Owner.InsertBefore(beforeEnumerator.Node, property));
        }

        /// <summary>
        /// Adds a property in the property tree, at the end of a level specified by a given enumerator.
        /// </summary>
        /// <param name="atEnumerator">This enumerator determines a given level. The new property is inserted
        /// at the end of this level.</param>
        /// <param name="property">The new property to add.</param>
        /// <returns>A deep enumerator pointing to the new property.</returns>
        public PropertyEnumerator Append(PropertyEnumerator atEnumerator, Property property)
        {
            PropertySiblingEnumerator sEnum = new PropertySiblingEnumerator(atEnumerator.Node);
            return InsertBefore(sEnum.MoveToRightBound(), property);
        }

        /// <summary>
        /// Removes a property from the tree.
        /// </summary>
        /// <param name="propEnum">An enumerator pointing to the property to remove from the tree.</param>
        /// <returns>A deep enumerator on the property that would be naturally selected if the deleted
        /// item was selected.</returns>
        public PropertyEnumerator Delete(PropertyEnumerator propEnum)
        {
            // Store the previous property (deep)
            PropertyDeepEnumerator dEnumerator = new PropertyDeepEnumerator(propEnum.Node);
            dEnumerator.MovePrev();

            // Remove the property
            propEnum.Node.Owner.Remove(propEnum.Node);

            // Determine the next property of the property that was deleted
            if (dEnumerator.MoveNext() == dEnumerator.RightBound)
                dEnumerator.MovePrev();
            if (dEnumerator == dEnumerator.LeftBound)
                dEnumerator = dEnumerator.RightBound.GetDeepEnumerator();

            // Since the enumerator passed in argument now points to an invalid property,
            // we make it point to the next property (the previous one if no next exists)
            propEnum.Node = dEnumerator.Node;

            return dEnumerator;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            string str = "Deep++\n";

            PropertyDeepEnumerator enumerator = GetDeepEnumerator();
            while (enumerator.MoveNext() != enumerator.RightBound)
            {
                for (int i = 0; i < enumerator.Depth; i++)
                    str += "-";
                str += enumerator.Property.ToString() + "\n";
            }

            str += "\n\nDeep--\n";

            enumerator.MoveToRightBound();
            while (enumerator.MovePrev() != enumerator.LeftBound)
            {
                for (int i = 0; i < enumerator.Depth; i++)
                    str += "-";
                str += enumerator.Property.ToString() + "\n";
            }

            return str;
        }

        public void Clear()
        {
            _PropertyTree.Clear();
        }

        public void Sort(IComparer comparer)
        {
            _PropertyTree.Sort(comparer);
        }
    }

    /// <summary>
    /// Base class for all enumerators that are the way for the client application to access a property.
    /// </summary>
    /// <remarks>
    /// It is abstract and therefore can't be instanciated. The client application never creates an enumerator by itself.
    /// <para>The first time you get an enumerator is when you create a property. A method like
    /// <see href="/VisualHint.SmartPropertyGrid.PropertyGrid.AppendProperty_overloads.html">AppendProperty</see>
    /// returns an enumerator. You can store it to later access the underlying property.</para>
    /// <para>A lot of methods take this enumerator in parameter. For example <see cref="PropertyGrid.EnableProperty"/>
    /// or <see cref="PropertyGrid.SelectProperty"/>.</para>
    /// <para>Having an enumerator also allows you to parse the content of the PropertyGrid. Depending on the type
    /// of enumerator you have you will be able to:</para>
    /// <list type="bullet">
    /// <item><description>Parse the complete tree if you have a <see cref="PropertyDeepEnumerator"/>.</description></item>
    /// <item><description>Parse the complete tree but only visible properties if you have a <see cref="PropertyVisibleDeepEnumerator"/>.</description></item>
    /// <item><description>Parse sibling properties if you have a <see cref="PropertySiblingEnumerator"/>.</description></item>
    /// <item><description>Parse sibling properties but only visible ones if you have a <see cref="PropertyVisibleSiblingEnumerator"/>.</description></item>
    /// <item><description>Get a parent property.</description></item>
    /// </list>
    /// </remarks>
    /// <example>
    /// Here is an example where we want to parse all the sibling properties of a category to disable them:
    /// <code>
    /// PropertyEnumerator childEnum = categoryEnum.Children;
    /// while(childEnum != childEnum.RightBound)
    /// {
    ///     EnableProperty(childEnum, false);
    ///     childEnum.MoveNext();
    /// }
    /// </code>
    /// </example>
    public abstract class PropertyEnumerator : ICloneable
    {
        /// <summary>
        /// Defines a bound at the beginning of the collection refered by this enumerator.
        /// This is the derived class that sets its value. The only requirement is that en enumerator using
        /// MovePrev() can't go beyond this bound.
        /// </summary>
        protected PropertyEnumerator mLeftBound;

        /// <summary>
        /// Defines a bound at the end of the collection refered by this enumerator.
        /// This is the derived class that sets its value. The only requirement is that en enumerator using
        /// MoveNext() can't go beyond this bound.
        /// </summary>
        protected PropertyEnumerator mRightBound;

        /// <summary>
        /// An enumerator points to a node in a double linked list.
        /// </summary>
        private LinkedListNode _node;

        internal LinkedList _restrictToThisLinkedListAndUnder;

        internal LinkedListNode Node
        {
            get { return _node; }
            set { _node = value; }
        }

        protected PropertyEnumerator(){ }

        internal PropertyEnumerator(LinkedListNode node)
        {
            _node = node;
        }

        /// <summary>
        /// Gets the reference to the underlying property.
        /// </summary>
        /// <remarks>
        /// null will be returned if the enumerator equals <see cref="LeftBound"/> or <see cref="RightBound"/>.
        /// </remarks>
        public Property Property
        {
            get { return (Property)Node.Value; }
        }

        /// <summary>
        /// Gets the number of sibling properties at the level of the property pointed to by this enumerator.
        /// </summary>
        public int Count
        {
            get { return Node.Owner.Count; }
        }

        /// <summary>
        /// Gets a boolean equal to true if the underlying property has a parent property, false otherwise.
        /// </summary>
        /// <remarks>
        /// If this enumerator has been restricted at a level and under, it returns false if it points to a property
        /// that is at the top level that it is allowed to parse.
        /// </remarks>
        /// <seealso cref="RestrictedToThisLevelAndUnder"/>
        public bool HasParent
        {
            get
            {
                // If this enumerator is restricted to this level and under test if we are at the top level
                if (_restrictToThisLinkedListAndUnder == Node.Owner)
                    return false;

                return (Node.Owner.Parent != null);
            }
        }

        /// <summary>
        /// Gets an enumerator on the parent property.
        /// </summary>
        /// <remarks><see cref="RightBound"/> is returned when the property has no parent. This is also the case if
        /// the enumerator has been restricted to a level and under and it points to the top most level it is allowed
        /// to parse.</remarks>
        /// <seealso cref="RestrictedToThisLevelAndUnder"/>
        public PropertyEnumerator Parent
        {
            get
            {
                // If this enumerator is restricted to this level and under test if we are at the top level
                if (_restrictToThisLinkedListAndUnder == Node.Owner)
                    return (PropertyEnumerator)RightBound.Clone();

                if (Node.Owner.Parent == null)
                    return (PropertyEnumerator)RightBound.Clone();

                return new PropertyDeepEnumerator(Node.Owner.Parent);
            }
        }

        public bool IsDescendantOf(PropertyEnumerator enumAncestor)
        {
            // This is an agreement that a property is not a descendant of itself
            if (this == enumAncestor)
                return false;

            PropertyDeepEnumerator currentEnum = GetDeepEnumerator();
            while (currentEnum.HasParent && (enumAncestor != currentEnum))
                currentEnum.MoveParent();

            return (enumAncestor == currentEnum);
        }

        public abstract PropertyEnumerator MoveParent();
        public abstract PropertyEnumerator MoveNext();
        public abstract PropertyEnumerator MovePrev();
        public abstract PropertyEnumerator MoveFirst();
        public abstract PropertyEnumerator MoveLast();

        public PropertyDeepEnumerator GetDeepEnumerator()
        {
            return new PropertyDeepEnumerator(Node);
        }

        public PropertyVisibleDeepEnumerator GetVisibleDeepEnumerator()
        {
            return new PropertyVisibleDeepEnumerator(Node);
        }

        public PropertySiblingEnumerator GetSiblingEnumerator()
        {
            return new PropertySiblingEnumerator(Node);
        }

        public PropertyVisibleSiblingEnumerator GetVisibleSiblingEnumerator()
        {
            return new PropertyVisibleSiblingEnumerator(Node);
        }

        public PropertyEnumerator MoveToRightBound()
        {
            Node = mRightBound.Node;
            return this;
        }

        public PropertyEnumerator MoveToLeftBound()
        {
            Node = mLeftBound.Node;
            return this;
        }

        public override bool Equals(object obj)
        {
            return (this == (PropertyEnumerator)obj);
        }

        public static bool operator ==(PropertyEnumerator propEnum1, PropertyEnumerator propEnum2)
        {
            // Only one is null ?
            if ((!ReferenceEquals(null, propEnum1) && ReferenceEquals(null, propEnum2)) ||
                (!ReferenceEquals(null, propEnum2) && ReferenceEquals(null, propEnum1)))
                return false;

            // Both are null or are the same references ?
            if (ReferenceEquals(propEnum1, propEnum2))
                return true;

            // Both are non null : compare their nodes
            return (propEnum1.Node == propEnum2.Node);
        }

        public static bool operator !=(PropertyEnumerator propEnum1, PropertyEnumerator propEnum2)
        {
            return !(propEnum1 == propEnum2);
        }

        public static bool operator <(PropertyEnumerator propEnum1, PropertyEnumerator propEnum2)
        {
            PropertyDeepEnumerator currentEnumerator = new PropertyDeepEnumerator(propEnum1.Node);
            while (currentEnumerator.MoveNext() != currentEnumerator.RightBound)
            {
                if (currentEnumerator == propEnum2)
                    return true;
            }

            return false;
        }

        public static bool operator >(PropertyEnumerator propEnum1, PropertyEnumerator propEnum2)
        {
            return !(propEnum1 < propEnum2);
        }

        public override int GetHashCode()
        {
            return Node.GetHashCode();
        }

        /// <summary>
        /// Gets the depth of the property pointed by this enumerator compared to the root of the property tree.
        /// It doesn't take into account if the enumerator is restricted to a specific level.
        /// </summary>
        /// <value>The depth of the property.</value>
        public int Depth
        {
            get
            {
                return Node.Owner.Depth;
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <value></value>
        public bool RestrictedToThisLevelAndUnder
        {
            get { return (_restrictToThisLinkedListAndUnder != null); }
            set
            {
                _restrictToThisLinkedListAndUnder = (value ? Node.Owner : null);
                CalcLeftAndRightBounds();
            }
        }

        protected abstract void CalcLeftAndRightBounds();

        public PropertyEnumerator LeftBound
        {
            get { return mLeftBound; }
        }

        internal PropertyEnumerator LeftBoundInternal
        {
            set { mLeftBound = value; }
        }

        public PropertyEnumerator RightBound
        {
            get { return mRightBound; }
        }

        internal PropertyEnumerator RightBoundInternal
        {
            set { mRightBound = value; }
        }

        public PropertyEnumerator Children
        {
            get
            {
                if (Node.Children.First == null)
                    return new PropertySiblingEnumerator(Node.Children.RightBound);
                else
                    return new PropertySiblingEnumerator(Node.Children.First);
            }
        }

        public virtual object Clone()
        {
            return null;
        }
    }

    /// <summary>
    /// Allows to parse all the properties of the grid in deep order (in other words in display order).
    /// </summary>
    public class PropertyDeepEnumerator : PropertyEnumerator
    {
        public PropertyDeepEnumerator()
        {
        }

        internal PropertyDeepEnumerator(LinkedListNode node) : base(node)
        {
            // Initialization of the left and right bounds of the collection parsed by this enumerator
            CalcLeftAndRightBounds();
        }

        public override object Clone()
        {
            return GetDeepEnumerator();
        }

        protected override void CalcLeftAndRightBounds()
        {
            if (_restrictToThisLinkedListAndUnder == null)
            {
                // Since this enumerator parses the entire tree, left and right bounds are the bounds
                // of the root linked list
                LinkedList parentList = Node.Owner;
                while (parentList.Parent != null)
                    parentList = parentList.Parent.Owner;

                // We use the constructor with 0 parameters to avoid recursion
                mLeftBound = new PropertyDeepEnumerator();
                mLeftBound.Node = parentList.LeftBound;
                mRightBound = new PropertyDeepEnumerator();
                mRightBound.Node = parentList.RightBound;
            }
            else
            {
                // Since this enumerator parses only properties at and under this level, left and right bounds are the bounds
                // of the current node as if it was in a sibling enumerator

                mLeftBound = new PropertySiblingEnumerator();
                mLeftBound.Node = Node.Owner.LeftBound;

                mRightBound = new PropertySiblingEnumerator();
                mRightBound.Node = Node.Owner.RightBound;
            }

            mLeftBound.LeftBoundInternal = LeftBound;
            mLeftBound.RightBoundInternal = RightBound;

            mRightBound.LeftBoundInternal = LeftBound;
            mRightBound.RightBoundInternal = RightBound;
        }

        public override PropertyEnumerator MoveNext()
        {
            // We can't move to a next property if we are already after the last one
            if (this == RightBound)
                return this;

            // Test if have to go one level down (property has children)
            if ((Node.Previous != null) && (Node.Children.Count > 0))
            {
                Node = Node.Children.First;
            }
            // or if we just increment in the current level
            else
            {
                Node = Node.Next;

                // Test if the current level is finished
                while ((this != RightBound) && (Node == Node.Owner.RightBound))
                {
                    MoveParent();
/*                    // Test to see if we are restricted under the current level
					if (_restrictToThisLinkedListAndUnder == Node.Owner)
						break;

					// We can't go further if we are at the end of the root collection
					if (Node.Owner.Parent  == null)
						break;
*/
                    // Point now to the parent property and go to its next sibling property
                    Node = Node.Next;
                }
            }

            return this;
        }

        public override PropertyEnumerator MovePrev()
        {
            // We can't move to a previous property if we are already before the first one
            if (this == LeftBound)
                return this;

            // If we are on the first property of a level
            if (Node == Node.Owner.First)
            {
                if ((Node.Owner.Parent != null) && (_restrictToThisLinkedListAndUnder != Node.Owner))
                    // Go one level up if we are not on the (restricted) root level
                    Node = Node.Owner.Parent;
                else
                    Node = Node.Owner.LeftBound;
            }

            // else we choose the previous sibling property or go deep in child level
            else
            {
                Node = Node.Previous;

                if (Node != Node.Owner.LeftBound)
                {
                    LinkedListNode lastChildNode = Node.Children.Last;
                    while (lastChildNode != null)
                    {
                        Node = lastChildNode;
                        lastChildNode = Node.Children.Last;
                    }
                }
            }

            return this;
        }

        public override PropertyEnumerator MoveFirst()
        {
            Node = Node.Owner.LeftBound;
            MoveNext();

//            if (this == RightBound)    // No element in the tree so we are on RightBound ?
//                Node = Node.Owner.LeftBound;

            return this;
        }

        public override PropertyEnumerator MoveLast()
        {
            Node = Node.Owner.RightBound;
            MovePrev();

            if (this == LeftBound)    // No element in the tree so we are on LeftBound ?
                Node = Node.Owner.RightBound;

            return this;
        }

        public override PropertyEnumerator MoveParent()
        {
            if (HasParent || (_restrictToThisLinkedListAndUnder != Node.Owner))
                Node = Node.Owner.Parent;
            else
                Node = RightBound.Node;

            return this;
        }
    }

    /// <summary>
    /// Allows to parse all the currently visible properties of the grid in deep order (in other words in
    /// display order).
    /// </summary>
    public class PropertyVisibleDeepEnumerator : PropertyDeepEnumerator
    {
        internal PropertyVisibleDeepEnumerator(LinkedListNode node) : base(node)
        {
            // We may point to an invisible property so we must find for the first visible one after it

            // First we must ensure that all our ancestors are expanded. If not, this enumerator
            // will initially point to the upper enumerator that is collapsed
            PropertyEnumerator pEnumerator = new PropertyDeepEnumerator(Node);
            while (pEnumerator.HasParent)
            {
                pEnumerator.MoveParent();
                if ((pEnumerator.Property.Expanded == false) || (pEnumerator.Property.Visible == false))
                    Node = pEnumerator.Node;
            }

            if (this != LeftBound)
            {
                // Once we are on an expanded part of the tree we go to a visible property
                while ((this != RightBound) && (Property.Visible == false))
                    MoveNext();
            }
        }

        public override object Clone()
        {
            return GetVisibleDeepEnumerator();
        }

        public override PropertyEnumerator MoveNext()
        {
            do
            {
                // If we are at the far end of the collection, we can't go further
                if (this == RightBound)
                    break;

                if ((Property != null) && ((Property.Visible == false) || (Property.Expanded == false)))
                {
                    bool invisible = (Property.Visible == false);

                    // We increment in the current collection
                    PropertyVisibleSiblingEnumerator sEnumerator = new PropertyVisibleSiblingEnumerator(Node);
                    if (!invisible) // If the property was invisible the line above will already have moved next
                        sEnumerator.MoveNext();

                    // Test if the current level is finished
                    while (sEnumerator == sEnumerator.RightBound)
                    {
                        // We can't go further if we are at the end of the root collection
                        sEnumerator.MoveParent();
                        if (sEnumerator == RightBound)
                            break;

                        // Point now to the parent property and go to its next sibling property
                        sEnumerator.MoveNext();
                    }

                    Node = sEnumerator.Node;
                }
                else
                    base.MoveNext();
            }
            while ((this != RightBound) && (Property.Visible == false));

            return this;
        }

        public override PropertyEnumerator MovePrev()
        {
            do
            {
                // If we are on the first property of a level
                if (Node == Node.Owner.First)
                {
                    // Go one level up if we are not on the root level
                    if (HasParent)
                        MoveParent();
                    else
                        Node = LeftBound.Node;
                }

                // if we are not on the first property of a level
                else
                {
                    Node = Node.Previous;

                    if (Property != null)	// If we are on a valid property
                    {
                        if (Property.Visible && (Node.Children.Count > 0) && Property.Expanded)
                        {
                            Node = Node.Children.RightBound;
                            MovePrev();
                        }
                    }
                }
            }
            while ((this != LeftBound) && (Property.Visible == false));

            return this;
        }
    }

    /// <summary>
    /// Allows to parse all the direct child properties of a same parent property (in display order).
    /// </summary>
    public class PropertySiblingEnumerator : PropertyEnumerator
    {
        internal PropertySiblingEnumerator() { }

        internal PropertySiblingEnumerator(LinkedListNode node) : base(node)
        {
            // Initialiwation of the left and right bounds of the collection parsed by this enumerator
            // Since this enumerator parses only sibling properties, left and right bounds are the bounds
            // of the linked list pointed to by this enumerator

            mLeftBound = new PropertySiblingEnumerator();
            mLeftBound.Node = Node.Owner.LeftBound;

            mRightBound = new PropertySiblingEnumerator();
            mRightBound.Node = Node.Owner.RightBound;

            mLeftBound.LeftBoundInternal = LeftBound;
            mLeftBound.RightBoundInternal = RightBound;
            mRightBound.LeftBoundInternal = LeftBound;
            mRightBound.RightBoundInternal = RightBound;
        }

        public override object Clone()
        {
            return GetSiblingEnumerator();
        }

        protected override void CalcLeftAndRightBounds() { }

        public override PropertyEnumerator MoveNext()
        {
            // We can't move to a next property if we are already after the last one
            if (this != RightBound)
                Node = Node.Next;

            return this;
        }

        public override PropertyEnumerator MovePrev()
        {
            // We can't move to a previous property if we are already before the first one
            if (this != LeftBound)
                Node = Node.Previous;

            return this;
        }

        public override PropertyEnumerator MoveFirst()
        {
            if (Node.Owner.Count == 0)
                Node = RightBound.Node;
            else
                Node = Node.Owner.First;

            return this;
        }

        public override PropertyEnumerator MoveLast()
        {
            if (Node.Owner.Count == 0)
                Node = RightBound.Node;
            else
                Node = Node.Owner.Last;

            return this;
        }

        public override PropertyEnumerator MoveParent()
        {
            if (HasParent)
            {
                Node = Node.Owner.Parent;
                LeftBound.Node = Node.Owner.LeftBound;
                RightBound.Node = Node.Owner.RightBound;
            }
            else
                Node = RightBound.Node;

            return this;
        }
    }

    /// <summary>
    /// Allows to parse all the currently visible direct child properties of a same parent property
    /// (in display order).
    /// </summary>
    public class PropertyVisibleSiblingEnumerator : PropertySiblingEnumerator
    {
        internal PropertyVisibleSiblingEnumerator(LinkedListNode node) : base(node)
        {
            // We may point to an invisible property so we must find for the first visible one after it

            // First we must ensure that all our ancestors are expanded. If not, this enumerator
            // will initially point to RightBound
            PropertyEnumerator pEnumerator = new PropertyDeepEnumerator(Node);
            while (pEnumerator.HasParent)
            {
                pEnumerator.MoveParent();
                if ((pEnumerator.Property.Expanded == false) || (pEnumerator.Property.Visible == false))
                {
                    Node = RightBound.Node;
                    break;
                }
            }

            while ((this != RightBound) && (Property.Visible == false))
                MoveNext();
        }

        public override object Clone()
        {
            return GetVisibleSiblingEnumerator();
        }

        public override PropertyEnumerator MoveNext()
        {
            if (this == RightBound)
                return this;

            do
            {
                Node = Node.Next;
            }
            while ((this != RightBound) && (Property.Visible == false));

            return this;
        }

        public override PropertyEnumerator MovePrev()
        {
            if (this == LeftBound)
                return this;

            do
            {
                Node = Node.Previous;
            }
            while ((this != LeftBound) && (Property.Visible == false));

            return this;
        }

        public override PropertyEnumerator MoveFirst()
        {
            if (Node.Owner.Count == 0)
                Node = LeftBound.Node;
            else
            {
                Node = Node.Owner.First;
                while ((this != RightBound) && (Property.Visible == false))
                    Node = Node.Next;
                if (this == RightBound)
                    Node = LeftBound.Node;
            }

            return this;
        }

        public override PropertyEnumerator MoveLast()
        {
            if (Node.Owner.Count == 0)
                Node = RightBound.Node;
            else
            {
                Node = Node.Owner.Last;
                while ((this != LeftBound) && (Property.Visible == false))
                    Node = Node.Previous;
                if (this == LeftBound)
                    Node = RightBound.Node;
            }

            return this;
        }
    }

    internal class LinkedListNode
    {
        /// <summary>
        /// Points to the previous sibling property in the collection owning this node
        /// </summary>
        private LinkedListNode previous = null;

        /// <summary>
        /// Points to the next sibling property in the collection owning this node
        /// </summary>
        private LinkedListNode next = null;

        /// <summary>
        /// Points to the actual object stored in this node of the collection
        /// </summary>
        private object value;

        /// <summary>
        /// Points to a child collection
        /// </summary>
        private LinkedList _children = null;

        /// <summary>
        /// Collection containing this node
        /// </summary>
        private LinkedList _owner;

        public LinkedListNode(LinkedList owner, object value)
        {
            Owner = owner;
            Value = value;

            if ((owner != null) && (value != null))
                _children = new LinkedList(this, owner.Depth + 1);
        }

        public object Value
        {
            get { return value; }
            set { this.value = value; }
        }

        public LinkedList Owner
        {
            get { return _owner; }
            set { _owner = value; }
        }

        public LinkedListNode Previous
        {
            get { return previous; }
            set { previous = value; }
        }

        public LinkedListNode Next
        {
            get { return next; }
            set { next = value; }
        }

        public LinkedList Children
        {
            get { return _children; }
            set { _children = value; }
        }
    }

    internal class LinkedList
    {
        private int _count = 0;
        private LinkedListNode _leftBound;
        private LinkedListNode _head;
        private LinkedListNode _tail;
        private LinkedListNode _rightBound;
        private LinkedListNode _parent;
        private int _depth;

        public int Depth
        {
            get { return _depth; }
        }

        public LinkedList(LinkedListNode parentNode, int depth)
        {
            _depth = depth;
            _parent = parentNode;

            // Initialize the bounds of this list
            _leftBound = new LinkedListNode(this, null);
            _rightBound = new LinkedListNode(this, null);
            LinkNode(_leftBound, _rightBound);
        }

        public int Count
        {
            get { return _count; }
        }

        public object this[int index]
        {
            get
            {
                if (index > Count)
                    throw new ArgumentOutOfRangeException("index", index, "ArgumentOutOfRange_Index");

                LinkedListNode node;

                node = First;

                for (int i = 0; i <= index; i++)
                    node = node.Next;

                return node.Value;
            }
            set
            {
                if (index > Count)
                    throw new ArgumentOutOfRangeException("index", index, "ArgumentOutOfRange_Index");
                LinkedListNode node = First;
                for (int i = 0; i < index; i++)
                    node = node.Next;
                node.Value = value;
            }
        }

        private static void LinkNode(LinkedListNode node1, LinkedListNode node2)
        {
            node1.Next = node2;
            node2.Previous = node1;
        }

        public LinkedListNode Append(object value)
        {
            LinkedListNode node = new LinkedListNode(this, value);

            if (_head == null)
            {
                _head = node;
                LinkNode(_leftBound, node);
                LinkNode(node, _rightBound);
            }
            else
            {
                LinkNode(_tail, node);
                LinkNode(node, _rightBound);
            }

            _tail = node;
            _count++;

            return node;
        }

        public LinkedListNode Prepend(object value)
        {
            LinkedListNode node = new LinkedListNode(this, value);

            if (_tail == null)
            {
                _tail = node;
                LinkNode(_leftBound, node);
                LinkNode(node, _rightBound);
            }
            else
            {
                LinkNode(_leftBound, node);
                LinkNode(node, _head);
            }

            _head = node;
            _count++;

            return node;
        }

        public LinkedListNode InsertBefore(LinkedListNode before, object value)
        {
            if (before == _head)
                return Prepend(value);
            else if (before == _rightBound)
                return Append(value);
            else
            {
                LinkedListNode node = new LinkedListNode(this, value);
                LinkedListNode prev = before.Previous;

                LinkNode(prev, node);
                LinkNode(node, before);

                _count++;

                return node;
            }
        }

        public LinkedListNode InsertAfter(LinkedListNode after, object value)
        {
            if (after == _tail)
                return Append(value);
            else if (after == _leftBound)
                return Prepend(value);
            else
            {
                LinkedListNode node = new LinkedListNode(this, value);
                LinkedListNode next = after.Next;

                LinkNode(after, node);
                LinkNode(node, next);

                _count++;

                return node;
            }
        }

        public void Clear()
        {
            // Reinitialize the bounds of this list
            LinkNode(_leftBound, _rightBound);

            _count = 0;
            _head = null;;
            _tail = null;
        }

        public void Remove(LinkedListNode node)
        {
            LinkedListNode prev = node.Previous;
            LinkedListNode next = node.Next;

            if (node == _head && node == _tail)
                _head = _tail = null;
            else if (node == _head)
                _head = next;
            else if (node == _tail)
                _tail = prev;

            LinkNode(prev, next);

            _count--;
        }

        public LinkedListNode First
        {
            get { return _head; }
        }

        public LinkedListNode Last
        {
            get { return _tail; }
        }

        public LinkedListNode Parent
        {
            get { return _parent; }
            set { _parent = value; }
        }

        public LinkedListNode LeftBound
        {
            get { return _leftBound; }
        }

        public LinkedListNode RightBound
        {
            get { return _rightBound; }
        }

        public void Sort(IComparer comparer)
        {
            int limit = Count - 1;
            while (limit > 0)
            {
                LinkedListNode node = _head;
                for (int i = 0; i < Count - 1; i++)
                {
                    if (comparer.Compare(node.Value, node.Next.Value) > 0)
                    {
                        if (node == _head)
                            _head = node.Next;
                        else if (node.Next == _tail)
                            _tail = node;

                        LinkNode(node.Previous, node.Next);
                        LinkedListNode lastNode = node.Next.Next;
                        LinkNode(node.Next, node);
                        LinkNode(node, lastNode);
                    }
                    else
                        node = node.Next;
                }

                limit--;
            }
        }
    }
}
