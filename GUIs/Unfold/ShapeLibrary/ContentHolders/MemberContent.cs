using System;
using System.Collections.Generic;
using System.Text;
using System.Reflection;
namespace Orbifold.Unfold.ShapeLibrary
{
    public sealed class MemberContent
    {
        #region Fields
        private List<MethodInfo> mMethods;
        private List<PropertyInfo> mProperties;
        #endregion

        public List<MethodInfo> Methods
        {
            get { return mMethods; }
            set { mMethods = value; }
        }
        public List<PropertyInfo> Properties
        {
            get { return mProperties; }
            set { mProperties = value; }
        }


    }
}
